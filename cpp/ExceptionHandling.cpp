#include <exception>

#include "quasar_dsl.h"

#include "Encoding.h"
#include "ExceptionHandling.h"

void ThrowJavaException(JNIEnv *env, const std::string& exception, const std::string& message)
{
	jclass cls = env->FindClass(exception.c_str());
	if (cls == nullptr) return;
	env->ThrowNew(cls, message.c_str());
	env->DeleteLocalRef(cls);
}

void RethrowAsJavaException(JNIEnv *env)
{
	try
	{
		throw;
	}
	catch (const quasar::exception_t &e)
	{
		std::string source = UTF16toModifiedUTF8(e.source.get_buf());
		std::string message = UTF16toModifiedUTF8(e.message.get_buf());
		std::string stacktrace = UTF16toModifiedUTF8(e.stack_trace.get_buf());
		ThrowJavaException(env, "be/vib/bits/QException", "\n---"
				"\nSource:\n" + source + "\nMessage:\n" + message + "\nStacktrace:\n" + stacktrace + "\n---\n");
	}
	catch (const std::exception &e)
	{
		ThrowJavaException(env, "java/lang/Error", e.what());
	}
	catch (...)
	{
		ThrowJavaException(env, "java/lang/Error", "Unknown exception.");
	}
}
