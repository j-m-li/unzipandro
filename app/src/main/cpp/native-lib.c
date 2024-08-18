#include <jni.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int java_main(int argc, char *argv[]);

JNIEXPORT jstring
JNICALL
Java_com_cod5_unzipandro_MainActivity_unzip(
	JNIEnv *env,
	jobject this,
	jstring src,
	jstring dest
	) {
    char *nsrc = (char*)(*env)->GetStringUTFChars(env,src, 0);
    char *ndest = (char*)(*env)->GetStringUTFChars(env,dest, 0);
    char *arg[] = {"hwzip", "extract", nsrc};

    mkdir(ndest, 0777);
    chdir(ndest);

    java_main(3, arg);
    (*env)->ReleaseStringUTFChars(env,src, nsrc);
    (*env)->ReleaseStringUTFChars(env,dest, ndest);
    return (*env)->NewStringUTF(env,"Yo");
}

void * __memcpy_chk(void * dest, const void * src, size_t len, size_t destlen)
{
	return memcpy(dest, src, len);
}

void * __memset_chk(void * dest, int c, size_t len, size_t destlen)
{
	return memset(dest, c, len);
}

void * __memmove_chk(void * dest, const void * src, size_t len, size_t destlen)
{
	return memmove(dest, src,len);
}

size_t __strlen_chk(const char *s, size_t s_len)
{
    size_t ret = strlen(s);
    return ret;
}