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
#include <sys/stat.h>

#endif

#define bld__MAX_CHILDREN 20


#ifndef _WIN32
#define P_WAIT 1
extern char **environ;

/**
 * Execute command and wait until it terminates.
 */
int _spawnvp(int flags, const char *cmd, const char *const *args) {
    pid_t pid;
    int status = 0;
    pid = fork();
    if (!pid) {
	return execve(cmd, (char *const *) args, environ);
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
int bld__exec(const char *cwd, const char *command, const char *targs[]) {
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
    for (i = 0; i < bld__MAX_CHILDREN; i++) {
	if (targs[i][0]) {
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
	} else {
	    args[i + n] = 0;
	}
    }
    args[i + n] = 0;

    if (_spawnvp(P_WAIT, (const char *) args[0],
		 (const char *const *) args)) {
	chdir((char *) old);
	return -1;
    }
    chdir((char *) old);
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
) {
    jstring ret;
    int link[2];
    pid_t pid;
    int status;
    int nbytes, llen;
    char *buf = NULL;
    int alloced = 0;
    char *ncmd = (char *) (*env)->GetStringUTFChars(env, cmd, 0);
    char *npwd = (char *) (*env)->GetStringUTFChars(env, pwd, 0);
    char *narg1 = (char *) (*env)->GetStringUTFChars(env, arg1, 0);
    char *narg2 = (char *) (*env)->GetStringUTFChars(env, arg2, 0);
    char *narg3 = (char *) (*env)->GetStringUTFChars(env, arg3, 0);
    char *nnarg1 = narg1[0] ? narg1 : NULL;
    char *nnarg2 = narg2[0] ? narg2 : NULL;
    char *nnarg3 = narg3[0] ? narg3 : NULL;
    char *arg[] = {ncmd, nnarg1, nnarg2, nnarg3, 0};

    mkdir(npwd, 0777);
    if (pipe(link) == -1) {
	(*env)->ReleaseStringUTFChars(env, cmd, ncmd);
	(*env)->ReleaseStringUTFChars(env, pwd, npwd);
	(*env)->ReleaseStringUTFChars(env, arg1, narg1);
	(*env)->ReleaseStringUTFChars(env, arg2, narg2);
	(*env)->ReleaseStringUTFChars(env, arg3, narg3);
	return (*env)->NewStringUTF(env, "pipe failed");
    }

    if ((pid = fork()) == -1) {
	(*env)->ReleaseStringUTFChars(env, cmd, ncmd);
	(*env)->ReleaseStringUTFChars(env, pwd, npwd);
	(*env)->ReleaseStringUTFChars(env, arg1, narg1);
	(*env)->ReleaseStringUTFChars(env, arg2, narg2);
	(*env)->ReleaseStringUTFChars(env, arg3, narg3);
	return (*env)->NewStringUTF(env, "fork failed");
    }
    if (pid == 0) {
	dup2(link[1], STDERR_FILENO);
	dup2(link[1], STDOUT_FILENO);
	close(link[0]);
	close(link[1]);
	chdir(npwd);
	execv(ncmd, arg);
	exit(0);
    } else {
	llen = 0;
	nbytes = 0;
	if (alloced < 1024) {
	    alloced += 1024;
	    buf = malloc(alloced + 1);
	}
	close(link[1]);
	status = 0;
	do {
	    usleep(10000);
	    llen += nbytes;
	    while (llen + 1024 > alloced) {
		alloced += 1024;
		buf = realloc(buf, alloced + 1);
	    }
	    if (pid > 0 && pid == waitpid(pid, &status, WNOHANG)) {
		pid = 0;
	    }
	    nbytes = read(link[0], buf + llen, 1024);
	} while (nbytes > 0 || pid > 0);

	if (llen <= 0) {
	    buf[0] = 0;
	} else {
	    buf[llen] = 0;
	}
    }

    (*env)->ReleaseStringUTFChars(env, cmd, ncmd);
    (*env)->ReleaseStringUTFChars(env, pwd, npwd);
    (*env)->ReleaseStringUTFChars(env, arg1, narg1);
    (*env)->ReleaseStringUTFChars(env, arg2, narg2);
    (*env)->ReleaseStringUTFChars(env, arg3, narg3);
    ret =  (*env)->NewStringUTF(env, buf);
    free(buf);
    return ret;
}

