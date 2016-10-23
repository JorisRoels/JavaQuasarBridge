#ifndef BE_VIB_BITS_QUASARBRIGE_UTILS_H
#define BE_VIB_BITS_QUASARBRIGE_UTILS_H

#include <jni.h>
#include <string>

namespace quasar { class QValue; }

quasar::QValue* GetQValueArg(JNIEnv* env, jobjectArray args, jsize index);

// exception: name of the exception, for example "java/lang/RuntimeException"
// message: in modified UTF-8
void ThrowByName(JNIEnv *env, const std::string& exception, const std::string& message);

std::string UTF16toModifiedUTF8(const wchar_t * in);

#endif
