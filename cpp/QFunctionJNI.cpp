#include "be_vib_bits_QFunction.h"

#include "quasar_dsl.h"

#include "Utils.h"
#include "WideString.h"

#include <iostream>

using namespace quasar;

extern jfieldID qvalue_ptr_fieldID;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QFunction_newQFunction(JNIEnv *env, jclass, jstring signature)
{
	WideString signatureW(env, signature);

	Function* q = new Function(signatureW);

	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QFunction_applyNative(JNIEnv* env, jobject obj, jobjectArray args)
{
	jsize numArgs = env->GetArrayLength(args);

	if (numArgs > 8)
	{
		ThrowByName(env, "java/lang/IllegalArgumentException", "QFunction.apply(QValue... args) does not support more than 8 arguments in args.");
		return 0;
	}

	QValue * arg1 = nullptr, *arg2 = nullptr,
		   * arg3 = nullptr, *arg4 = nullptr,
		   * arg5 = nullptr, *arg6 = nullptr,
		   * arg7 = nullptr, *arg8 = nullptr;

	switch (numArgs)
	{
		// Deliberate fall-through
		case 8: arg8 = GetQValueArg(env, args, 7);
		case 7: arg7 = GetQValueArg(env, args, 6);
		case 6: arg6 = GetQValueArg(env, args, 5);
		case 5: arg5 = GetQValueArg(env, args, 4);
		case 4: arg4 = GetQValueArg(env, args, 3);
		case 3: arg3 = GetQValueArg(env, args, 2);
		case 2: arg2 = GetQValueArg(env, args, 1);
		case 1: arg1 = GetQValueArg(env, args, 0);
		case 0: break;
		default: return 0;
	}

	jlong funcPtr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(funcPtr != 0);
	Function* f = reinterpret_cast<Function*>(funcPtr);

	try
	{
		QValue* resultCopy = nullptr;
		switch (numArgs)
		{
			case 0:	resultCopy = new QValue((*f)()); break;
			case 1:	resultCopy = new QValue((*f)(*arg1)); break;
			case 2:	resultCopy = new QValue((*f)(*arg1, *arg2)); break;
			case 3:	resultCopy = new QValue((*f)(*arg1, *arg2, *arg3)); break;
			case 4:	resultCopy = new QValue((*f)(*arg1, *arg2, *arg3, *arg4)); break;
			case 5: resultCopy = new QValue((*f)(*arg1, *arg2, *arg3, *arg4, *arg5)); break;
			case 6: resultCopy = new QValue((*f)(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6)); break;
			case 7: resultCopy = new QValue((*f)(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7)); break;
			case 8: resultCopy = new QValue((*f)(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8)); break;
			default: return 0;
		}
		return reinterpret_cast<jlong>(resultCopy);
	}
	catch (exception_t e)
	{
		// FIXME: we should throw a Java exception here
		std::wcout << "Caught C++ exception during function application. The function was passed %d argument(s)." << std::endl;
		std::wcout << "Source: " << e.source.get_buf() << std::endl;
		std::wcout << "Message: " << e.message.get_buf() << std::endl;
		std::wcout << "Stacktrace: " << e.stack_trace.get_buf() << std::endl;
		return 0;
	}
}
