#ifndef BE_VIB_BITS_QUASARBRIGE_UTILS_H
#define BE_VIB_BITS_QUASARBRIGE_UTILS_H

#include <jni.h>

namespace quasar
{
	class QValue;
}

// Returns an observing (non-owning) pointer to the index-th QValue in args.
quasar::QValue* GetQValueArg(JNIEnv* env, jobjectArray args, jsize index);

#endif
