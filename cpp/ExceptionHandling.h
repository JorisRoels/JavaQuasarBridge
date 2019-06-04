#ifndef BE_VIB_BITS_QUASARBRIGE_EXCEPTIONHANDLING_H
#define BE_VIB_BITS_QUASARBRIGE_EXCEPTIONHANDLING_H

#include <jni.h>
#include <string>

// The function ThrowJavaException() throws a Java exception of class 'exception' and
// with the given 'message'. The message is a string encoded in *modified* UTF8.
// The'exception' parameter could be "java/lang/RuntimeException", for example.
// The Java exception is thrown via JNI.
void ThrowJavaException(JNIEnv *env, const std::string& exception, const std::string& message);

// The function RethrowAsJavaException() rethrows a pending C++ exception
// as a Java exception through JNI. It must only be used inside a C++ catch clause.
void RethrowAsJavaException(JNIEnv *env);

#endif
