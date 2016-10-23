#ifndef BE_VIB_BITS_QUASARBRIGE_WIDESTRING_H
#define BE_VIB_BITS_QUASARBRIGE_WIDESTRING_H

#include <jni.h>

typedef const wchar_t *LPCWSTR;

// Helper class for safe and convenient read access to a jstring.
class WideString
{
	public:
		WideString(JNIEnv *env, jstring jstr);
		~WideString();
		operator LPCWSTR() const;
		wchar_t const* data() const;

	private:
		JNIEnv* env;
		jstring jstr;
		const jchar* chars;
};

#endif
