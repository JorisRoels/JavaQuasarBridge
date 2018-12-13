
#include "be_vib_bits_QMethod.h"

#include "quasar_dsl.h"

#include "ExceptionHandling.h"
#include "WideString.h"
#include "Utils.h"

using namespace quasar;

extern jfieldID qvalue_ptr_fieldID;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QMethod_newQMethod(JNIEnv* env, jclass, jlong typePtr, jstring signature)
{
	try
	{
		WideString signatureW(env, signature);

		Type* type = reinterpret_cast<Type*>(typePtr);

		Method* q = new Method(*type, signatureW);

		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QMethod_applyNative(JNIEnv* env, jobject obj, jobject jtarget, jobjectArray args)
{
	try
	{
		// Get the Quasar object whose method we want to call.
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
			ThrowJavaException(env, "java/lang/IllegalArgumentException", "QMethod.apply(QValue target, QValue... args) does not support more than 3 arguments in args.");
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
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}
