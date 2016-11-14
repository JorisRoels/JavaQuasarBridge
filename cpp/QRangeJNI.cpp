
#include "be_vib_bits_QRange.h"

#include "quasar_dsl.h"

#include <iostream>

using namespace quasar;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__(JNIEnv *, jclass)
{
	QValue *q = new Range();
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__III(JNIEnv *, jclass, jint minVal, jint maxVal, jint step)
{
	QValue *q = new Range(minVal, maxVal, step);
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__FFF(JNIEnv *, jclass, jfloat minVal, jfloat maxVal, jfloat step)
{
	QValue *q = new Range(minVal, maxVal, step);
	return reinterpret_cast<jlong>(q);
}
