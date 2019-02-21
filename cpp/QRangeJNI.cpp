
#include "be_vib_bits_QRange.h"

#include "ExceptionHandling.h"

#include "quasar_dsl.h"

using namespace quasar;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__(JNIEnv* env, jclass)
{
	try
	{
		QValue r = Range();
		QValue *q = new QValue(r);  // copy on the heap
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__III(JNIEnv* env, jclass, jint minVal, jint maxVal, jint step)
{
	try
	{
		QValue r = Range(minVal, maxVal, step);
		QValue *q = new QValue(r);  // copy on the heap
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QRange_newQRange__FFF(JNIEnv* env, jclass, jfloat minVal, jfloat maxVal, jfloat step)
{
	try
	{
		QValue r = Range(minVal, maxVal, step);
		QValue *q = new QValue(r);  // copy on the heap
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}
