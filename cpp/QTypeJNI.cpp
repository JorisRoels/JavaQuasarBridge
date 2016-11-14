
#include "be_vib_bits_QType.h"

#include "quasar_dsl.h"

#include "WideString.h"

#include <iostream>

using namespace quasar;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QType_newQType__J(JNIEnv*, jclass, jlong ptr)
{
	assert(ptr != 0);
	QValue* q = reinterpret_cast<QValue*>(ptr);
	QValue* t = new Type(static_cast<qvalue_t>(*q));
	return reinterpret_cast<jlong>(t);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QType_newQType__Ljava_lang_String_2(JNIEnv* env, jclass, jstring typeName)
{
	WideString typeNameW(env, typeName);
	Type* q = new Type(typeNameW);
	return reinterpret_cast<jlong>(q);
}
