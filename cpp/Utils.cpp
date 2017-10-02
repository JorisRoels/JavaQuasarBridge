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

void ThrowByName(JNIEnv *env, const std::string& exception, const std::string& message)
{
	jclass cls = env->FindClass(exception.c_str());
	if (cls == nullptr) return;
	env->ThrowNew(cls, message.c_str());
	env->DeleteLocalRef(cls);
}

void ThrowQException(JNIEnv *env, const quasar::exception_t &e)
{
	std::string source = UTF16toModifiedUTF8(e.source.get_buf());
	std::string message = UTF16toModifiedUTF8(e.message.get_buf());
	std::string stacktrace = UTF16toModifiedUTF8(e.stack_trace.get_buf());
	ThrowByName(env, "be/vib/bits/QException", "\n---"
			"\nSource:\n" + source + "\nMessage:\n" + message + "\nStacktrace:\n" + stacktrace + "\n---\n");
}

// Given a null-terminated modified UTF-8 encoded string,
// returns a regular UTF-8 std::string.
std::string UTF16toModifiedUTF8(const TCHAR* in)
{
	// The *modified* UTF-8 encoding that JNI uses is slightly different
	// from regular UTF-8 encoding. See thee JNI specification for details.
	// https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html#modified_utf_8_strings

	static_assert(sizeof(TCHAR) == 2, "Expected TCHAR to be 2 bytes");

	std::string out;
	while (*in != 0)
	{
		TCHAR w = *in++;
		if (w >= 0x0001 && w <= 0x007f)
		{
			out.push_back(static_cast<char>(w));
		}
		else if (w == 0x0000 || w <= 0x07ff)
		{
			out.push_back(static_cast<char>(0xc0 | ((w >> 6) & 0x1f)));
			out.push_back(static_cast<char>(0x80 | ( w       & 0x3f)));
		}
		else if (w <= 0xffff)
		{
			out.push_back(static_cast<char>(0xe0 | ((w >> 12) & 0x0f)));
			out.push_back(static_cast<char>(0x80 | ((w >>  6) & 0x3f)));
			out.push_back(static_cast<char>(0x80 | ( w        & 0x3f)));
		}
	}
	return out;
}
