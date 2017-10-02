#ifndef BE_VIB_BITS_QUASARBRIGE_WIDESTRING_H
#define BE_VIB_BITS_QUASARBRIGE_WIDESTRING_H

#include <jni.h>

#include "char16_string.h"   // Quasar include file, defines LPCTSTR and TCHAR

// Helper class for safe and convenient read access to a jstring.
class WideString
{
	public:
		WideString(JNIEnv *env, jstring jstr);
		~WideString();
		operator LPCTSTR() const;
		TCHAR const* data() const;

	private:
		JNIEnv* env;
		jstring jstr;
		const jchar* chars;
};

#endif
