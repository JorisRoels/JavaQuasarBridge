#include "be_vib_bits_QValue.h"

#include "quasar_dsl.h"

#include "WideString.h"

#include <iostream>

using namespace quasar;

extern IQuasarHost* host;

static_assert(sizeof(jlong) == 8, "jlong must be 64 bit");

// We cache the ID of the 'ptr' field of be.vib.bits.QValue in a global variable.
// It is also used in the JNI implementation of the derived classes of QValue.
jfieldID qvalue_ptr_fieldID = nullptr;


JNIEXPORT void JNICALL Java_be_vib_bits_QValue_initIDs(JNIEnv* env, jclass cls)
{
	std::cout << "Java_be_vib_bits_QValue_initIDs " << std::endl;
	qvalue_ptr_fieldID = env->GetFieldID(cls, "ptr", "J");
	// If qvalue_ptr_fieldID == nullptr here, then GetFieldID() has thrown an exception.
	// It is still pending but will become active the instant we leave native code and return to Java.
}

JNIEXPORT jint JNICALL Java_be_vib_bits_QValue_getInt(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	return static_cast<int>(*q);
}

JNIEXPORT jfloat JNICALL Java_be_vib_bits_QValue_getFloat(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	return static_cast<float>(*q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__(JNIEnv*, jclass)
{
	QValue *q = new QValue();
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__I(JNIEnv*, jclass, jint i)
{
	QValue *q = new QValue(i);
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__F(JNIEnv*, jclass, jfloat f)
{
	QValue *q = new QValue(f);
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue___3F(JNIEnv* env, jclass, jfloatArray arr)
{
	jint numElems = env->GetArrayLength(arr);

	jboolean isCopy = JNI_FALSE;

	jfloat* jelems = (jfloat*)env->GetPrimitiveArrayCritical(arr, &isCopy); // TODO: make 100% sure we won't trigger deadlock between here and ReleasePrimitiveArrayCritical()

	if (jelems == nullptr)
	{
		// GetPrimitiveArrayCritical() has thrown an exception. It is still pending,
		// but will become active the instant we return to Java.
		return 0;
	}

	QValue v = QValue::CreateVector<float>(numElems);

	Vector qelems = host->LockVector<float>(v);
	for (int i = 0; i < numElems; ++i)
		qelems.data[i] = jelems[i];
	host->UnlockVector(v);

	env->ReleasePrimitiveArrayCritical(arr, jelems, 0);

	assert(size(v, 0) == 1);
	assert(size(v, 1) == numElems);
	assert(size(v, 2) == 1);

	// We want an object on the heap so we can control its lifetime
	// from the Java side.
	QValue *q = new QValue(v);

	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__Ljava_lang_String_2(JNIEnv* env, jclass, jstring s)
{
	WideString strW(env, s);
	QValue* q = new QValue(strW);
	return reinterpret_cast<jlong>(q);
}

JNIEXPORT void JNICALL Java_be_vib_bits_QValue_delete(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	if (ptr == 0) return;  // ptr being 0 is a perfectly acceptable code path - not an error.

	QValue* q = reinterpret_cast<QValue*>(ptr);
	delete q;  // If all went fine, there now are no more references to the C++ QValue and it got destructed nicely.

	env->SetLongField(obj, qvalue_ptr_fieldID, 0);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_readhostVariableNative(JNIEnv* env, jclass, jstring varName)
{
	WideString varNameW(env, varName);
	QValue* q = new QValue(QValue::ReadHostVariable(varNameW));
	// Note the QValue copy above because we need the QValue on the heap so we can control its lifetime from the Java side

	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jint JNICALL Java_be_vib_bits_QValue_size(JNIEnv* env, jobject obj, jint dim)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	return size(*q, dim);
}

