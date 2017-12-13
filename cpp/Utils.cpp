#include "Utils.h"

#include "quasar_dsl.h"

extern jfieldID qvalue_ptr_fieldID;

using namespace quasar;

QValue* GetQValueArg(JNIEnv* env, jobjectArray args, jsize index)
{
	jobject arg = env->GetObjectArrayElement(args, index);

#ifndef NDEBUG
	jclass cls = env->FindClass("be/vib/bits/QValue");
	assert(cls != nullptr);
	assert(env->IsInstanceOf(arg, cls) == JNI_TRUE);
	env->DeleteLocalRef(cls);
#endif

	jlong ptr = env->GetLongField(arg, qvalue_ptr_fieldID);
	assert(ptr != 0);

	env->DeleteLocalRef(arg);

	return reinterpret_cast<QValue*>(ptr);
}
