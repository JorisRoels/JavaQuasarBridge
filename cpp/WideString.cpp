#include "WideString.h"

WideString::WideString(JNIEnv *env, jstring jstr) : env(env), jstr(jstr)
{
	chars = env->GetStringChars(jstr, nullptr);
}

WideString::~WideString()
{
	env->ReleaseStringChars(jstr, chars);
}

WideString::operator LPCWSTR() const
{
	return data();
}

wchar_t const* WideString::data() const
{
	// Note: the jchar type is an unsigned 16 bits data type
	return reinterpret_cast<wchar_t const*>(chars);
}
