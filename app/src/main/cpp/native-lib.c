#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#include <process.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#include <unistd.h>
#else
#include <unistd.h>
#include <wait.h>
#endif

#define bld__MAX_CHILDREN 20


#ifndef _WIN32
#define P_WAIT 1
extern char **environ;

/**
 * Execute command and wait until it terminates.
 */
int _spawnvp(int flags, const char *cmd, const char *const *args)
{
    pid_t pid;
    int status = 0;
    pid = fork();
    if (!pid) {
	return execve(cmd, (char *const*)args, environ);
    } else if (flags & P_WAIT) {
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
	    return WEXITSTATUS(status);
	}
	status = -1;
    }
    return status;
}

#endif

/**
 * exiecute vscode task.
 * \param t the task
 * \param workspace path to the root of the project
 * \returns 0 if success
 */
int bld__exec(const char *cwd, const char *command, const char *targs[])
{
    char *args[bld__MAX_CHILDREN + 5];
    char old[1024];
    int n;
    int i;
    char *p;
    char *q;

    getcwd(old, 1024);
    chdir(cwd);
#ifdef _WIN32
    args[0] = (os_utf8 *)"cmd";
	args[1] = (os_utf8 *)"/c";
	args[2] = command;
	n = 3;
#else
    args[0] = "/usr/bin/env";
    args[1] = "-S";
    args[2] = command;
    n = 3;
#endif
    for (i = 0; i < bld__MAX_CHILDREN; i++)
    {
	if (targs[i][0])
	{
	    p = targs[i];
	    if (p[0] == '\'') {
		q = p + 1;
		while (*q) {
		    q++;
		}
		q--;
		if (*q == '\'') {
#ifdef _WIN32
		    p++;
					*q = '\0';
#endif
		}
	    }
	    args[i + n] = p;
	}
	else
	{
	    args[i + n] = 0;
	}
    }
    args[i + n] = 0;

    if (_spawnvp(P_WAIT, (const char *)args[0],
		 (const char *const *)args))
    {
	chdir((char*)old);
	return -1;
    }
    chdir((char*)old);
    return 0;
}


JNIEXPORT jstring
JNICALL
Java_com_cod5_unzipandro_MainActivity_exec(
	JNIEnv *env,
	jobject this,
	jstring cmd,
	jstring pwd,
	jstring arg1,
	jstring arg2,
	jstring arg3
)
{
    int link[2];
    pid_t pid;
    char buf[14096];
    char *ncmd = (char*)(*env)->GetStringUTFChars(env,cmd, 0);
    char *npwd = (char*)(*env)->GetStringUTFChars(env,pwd, 0);
    char *narg1 = (char*)(*env)->GetStringUTFChars(env,arg1, 0);
    char *narg2 = (char*)(*env)->GetStringUTFChars(env,arg2, 0);
    char *narg3 = (char*)(*env)->GetStringUTFChars(env,arg3, 0);

    char *arg[] = {ncmd, narg1, narg2, 0, 0};
    buf[0] = 'K';
    buf[1] = 0;
    if (pipe(link)==-1) {
	(*env)->ReleaseStringUTFChars(env,cmd, ncmd);
	(*env)->ReleaseStringUTFChars(env,pwd, npwd);
	(*env)->ReleaseStringUTFChars(env,arg1, narg1);
	(*env)->ReleaseStringUTFChars(env,arg2, narg2);
	(*env)->ReleaseStringUTFChars(env,arg3, narg3);
	return (*env)->NewStringUTF(env,"pipe failed");
    }

    if ((pid = fork()) == -1) {
	(*env)->ReleaseStringUTFChars(env,cmd, ncmd);
	(*env)->ReleaseStringUTFChars(env,pwd, npwd);
	(*env)->ReleaseStringUTFChars(env,arg1, narg1);
	(*env)->ReleaseStringUTFChars(env,arg2, narg2);
	(*env)->ReleaseStringUTFChars(env,arg3, narg3);
	return (*env)->NewStringUTF(env,"fork failed");
    }
    if(pid == 0) {
	dup2 (link[1], STDERR_FILENO);
	dup2 (link[1], STDOUT_FILENO);
	close(link[0]);
	close(link[1]);
	chdir(npwd);
	execv(ncmd, arg);
	return  0;
    } else {
	close(link[1]);
	int nbytes = read(link[0], buf, sizeof(buf));
	wait(NULL);
	if (nbytes <= 0) {
	    buf[0] = 0;
	} else {
	    buf[nbytes] = 0;
	}
    }

    (*env)->ReleaseStringUTFChars(env,cmd, ncmd);
    (*env)->ReleaseStringUTFChars(env,pwd, npwd);
    (*env)->ReleaseStringUTFChars(env,arg1, narg1);
    (*env)->ReleaseStringUTFChars(env,arg2, narg2);
    (*env)->ReleaseStringUTFChars(env,arg3, narg3);
    return (*env)->NewStringUTF(env,buf);
}

