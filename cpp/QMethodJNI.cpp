
#include "be_vib_bits_QMethod.h"

#include "quasar_dsl.h"

#include "Utils.h"
#include "WideString.h"

#include <iostream>

using namespace quasar;

extern jfieldID qvalue_ptr_fieldID;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QMethod_newQMethod(JNIEnv* env, jclass, jlong typePtr, jstring signature)
{
	WideString signatureW(env, signature);

	Type* type = reinterpret_cast<Type*>(typePtr);

	Method* q = new Method(*type, signatureW);  // TODO: check/ask if copy of signature needed (it used to be needed in newFunction for example - but not anymore),

	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QMethod_applyNative(JNIEnv* env, jobject obj, jobject jtarget, jobjectArray args)
{
	// Get the quasar object whose method we want to call.
	jlong targetPtr = env->GetLongField(jtarget, qvalue_ptr_fieldID);
	assert(targetPtr != 0);
	QValue* target = reinterpret_cast<QValue*>(targetPtr);

	// Get the method we want to call.
	jlong methPtr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(methPtr != 0);
	Method* method = reinterpret_cast<Method*>(methPtr);

	// Get the arguments for the method call
	jsize numArgs = env->GetArrayLength(args);
	if (numArgs > 3)
	{
		ThrowByName(env, "java/lang/IllegalArgumentException", "QMethod.apply(QValue target, QValue... args) does not support more than 3 arguments in args.");
		return 0;
	}

	QValue *arg1 = nullptr,
		   *arg2 = nullptr,
		   *arg3 = nullptr;

	switch (numArgs)
	{
		// Deliberate fall-through
		case 3: arg3 = GetQValueArg(env, args, 2);
		case 2: arg2 = GetQValueArg(env, args, 1);
		case 1: arg1 = GetQValueArg(env, args, 0);
	    case 0: break;
		default: return 0;
	}

	// Call the method
	QValue* resultCopy = nullptr;
	switch (numArgs)
	{
	    case 0: resultCopy = new QValue((*method)(*target)); break;
		case 1: resultCopy = new QValue((*method)(*target, *arg1)); break;
		case 2: resultCopy = new QValue((*method)(*target, *arg1, *arg2)); break;
		case 3: resultCopy = new QValue((*method)(*target, *arg1, *arg2, *arg3)); break;
		default: return 0;
	}

	return reinterpret_cast<jlong>(resultCopy);
}
