#include "WideString.h"

WideString::WideString(JNIEnv *env, jstring jstr) : env(env), jstr(jstr)
{
	chars = env->GetStringChars(jstr, nullptr);
}

WideString::~WideString()
{
	env->ReleaseStringChars(jstr, chars);
}

WideString::operator LPCTSTR() const
{
	return data();
}

TCHAR const* WideString::data() const
{
	// Note: the jchar type is an unsigned 16 bits data type, as is TCHAR
	return reinterpret_cast<TCHAR const*>(chars);
}
