#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#include "udt.h"
#include "com_udt_udt.h"
#include <android/log.h>

#include <string>

jint JNICALL Java_com_udt_udt_startup(JNIEnv *, jobject)
{
    return UDT::startup();
}

jint JNICALL Java_com_udt_udt_cleanup(JNIEnv *, jobject)
{
    return UDT::cleanup();
}

jint JNICALL Java_com_udt_udt_socket(JNIEnv *, jobject)
{
    struct addrinfo hints, *local;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int result = 0;
    if ((result = getaddrinfo(NULL, "9000", &hints, &local)) != 0)
    {
        char error[100] = {0};
        sprintf(error, "%s: %d", "incorrect network address", result);
        __android_log_write(ANDROID_LOG_ERROR, "Giggle", error);
        return 0;
    }

    int handle = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);
    freeaddrinfo(local);

    return handle;
}

jint JNICALL Java_com_udt_udt_connect(JNIEnv* env, jobject thiz, jint handle, jstring ip, jint port)
{
    const char *ip_address = env->GetStringUTFChars(ip, NULL);

    struct addrinfo hints, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[100] = {0};
    sprintf(port_str, "%d", port);
    if (0 != getaddrinfo(ip_address, port_str, &hints, &peer))
    {
        __android_log_write(ANDROID_LOG_ERROR, "Giggle", "incorrect server/peer address. ");
        return 0;
    }

    // connect to the server, implict bind
    int connect_result = 0;
    if ((connect_result = UDT::connect(handle, peer->ai_addr, peer->ai_addrlen)) == UDT::ERROR)
    {
        __android_log_write(ANDROID_LOG_ERROR, "Giggle", "connect error");
        return 0;
    }

    freeaddrinfo(peer);
    return connect_result;
}

jint JNICALL Java_com_udt_udt_close(JNIEnv *env, jobject thiz, jint handle)
{
    return UDT::close(handle);
}

jint JNICALL Java_com_udt_udt_send(JNIEnv *env, jobject thiz, jint handle, jbyteArray data, jint flag)
{
    jsize size = env->GetArrayLength(data);

    jboolean is_copy = JNI_FALSE;
    jbyte* data_ptr = env->GetByteArrayElements(data, &is_copy);

    int result = UDT::send(handle, (const char*)data_ptr, size, flag);
    if (result == UDT::ERROR)
    {
        __android_log_write(ANDROID_LOG_ERROR, "Giggle", "send data fail!");
    }

    env->ReleaseByteArrayElements(data, data_ptr, JNI_ABORT);

    return result;
}

jint Java_com_udt_udt_recv(JNIEnv *, jobject, jint, jbyteArray, jint, jint)
{
    return 0;
}
