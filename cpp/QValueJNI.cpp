#include "be_vib_bits_QValue.h"

#include "quasar_dsl.h"

#include "WideString.h"

#include <iostream>

using namespace quasar;

extern IQuasarHost* host;

static_assert(sizeof(jlong) == 8, "jlong must be 64 bit");  /// so it can be used to hold a C++ pointer

// We cache the ID of the 'ptr' field of be.vib.bits.QValue in a global variable.
// It is also used in the JNI implementation of the derived classes of QValue.
jfieldID qvalue_ptr_fieldID = nullptr;


JNIEXPORT void JNICALL Java_be_vib_bits_QValue_initIDs(JNIEnv* env, jclass cls)
{
	qvalue_ptr_fieldID = env->GetFieldID(cls, "ptr", "J");
	// If qvalue_ptr_fieldID == nullptr here, then GetFieldID() has thrown an exception.
	// The exception is still pending but will become active the instant we leave native code and return to Java.
}

JNIEXPORT jint JNICALL Java_be_vib_bits_QValue_getInt(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	int i = static_cast<int>(*q);

	return i;
}

JNIEXPORT jfloat JNICALL Java_be_vib_bits_QValue_getFloat(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	float f = static_cast<float>(*q);

	return f;
}

JNIEXPORT jstring JNICALL Java_be_vib_bits_QValue_getString(JNIEnv* env, jobject obj)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	string_t s = static_cast<string_t>(*q);

	const jchar* uchars = reinterpret_cast<const jchar*>(s.get_buf());
	jsize len = s.get_length(); // len = number of unicode characters, not the number of bytes
	return env->NewString(uchars, len);
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

template<typename JavaArrType, typename JavaElemType, typename QuasarElemType>
jlong newQValueArrayHelper(JNIEnv* env, JavaArrType arr)
{
	jint numElems = env->GetArrayLength(arr);

	jboolean isCopy = JNI_FALSE;

	JavaElemType* jelems = (JavaElemType*)env->GetPrimitiveArrayCritical(arr, &isCopy); // TODO: make 100% sure we won't trigger deadlock between here and ReleasePrimitiveArrayCritical()

	if (jelems == nullptr)
	{
		// GetPrimitiveArrayCritical() has thrown an exception. It is still pending,
		// but will become active the instant we return to Java.
		return 0;
	}

	QValue v = QValue::CreateVector<QuasarElemType>(numElems);

	VectorBase<QuasarElemType> qelems = host->LockVector<QuasarElemType>(v);
	for (int i = 0; i < numElems; ++i)
		qelems.data[i] = jelems[i];
	host->UnlockVector(v);

	env->ReleasePrimitiveArrayCritical(arr, jelems, 0);

	assert(size(v, 0) == 1);
	assert(size(v, 1) == numElems);
	assert(size(v, 2) == 1);

	// We want a QValue on the heap so we can control its lifetime
	// from the Java side.
	QValue *q = new QValue(v);

	return reinterpret_cast<jlong>(q);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue___3I(JNIEnv* env, jclass, jintArray arr)
{
	return newQValueArrayHelper<jintArray, jint, int>(env, arr);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue___3F(JNIEnv* env, jclass, jfloatArray arr)
{
	return newQValueArrayHelper<jfloatArray, jfloat, float>(env, arr);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__Ljava_lang_String_2(JNIEnv* env, jclass, jstring s)
{
	WideString stringW(env, s);
	QValue* q = new QValue(stringW);
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

template<typename ...Indices>
jlong atNativeHelper(JNIEnv* env, jobject obj, Indices... indices)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	QValue* q = reinterpret_cast<QValue*>(ptr);

	// Index the QValue *q.
	// Integer as well as QValue indices are supported.
	// The latter is typically used for slicing QValue vectors/arrays/cubes/... by using a QRange as index.
	QValue* value = new QValue((*q)(indices...));  // Since we need a pointer to a QValue we need to copy it.

	return reinterpret_cast<jlong>(value);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNative__I(JNIEnv* env, jobject obj, jint m)
{
	return atNativeHelper<jint>(env, obj, m);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNative__II(JNIEnv* env, jobject obj, jint m, jint n)
{
	return atNativeHelper<jint>(env, obj, m, n);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNative__III(JNIEnv* env, jobject obj, jint m, jint n, jint k)
{
	return atNativeHelper<jint>(env, obj, m, n, k);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNativeQ__JJ(JNIEnv* env, jobject obj, jlong ptr1)
{
	const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
	return atNativeHelper<const QValue&>(env, obj, *q1);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNativeQ__JJ(JNIEnv* env, jobject obj, jlong ptr1, jlong ptr2)
{
	const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
	const QValue* q2 = reinterpret_cast<QValue*>(ptr2);
	return atNativeHelper<const QValue&>(env, obj, *q1, *q2);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNativeQ__JJJ(JNIEnv* env, jobject obj, jlong ptr1, jlong ptr2, jlong ptr3)
{
	const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
	const QValue* q2 = reinterpret_cast<QValue*>(ptr2);
	const QValue* q3 = reinterpret_cast<QValue*>(ptr3);
	return atNativeHelper<const QValue&>(env, obj, *q1, *q2, *q3);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_getFieldNative(JNIEnv* env, jobject obj, jstring fieldName)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	WideString fieldNameW(env, fieldName);

	QValue* q = reinterpret_cast<QValue*>(ptr);

	QValue* value = new QValue(q->GetField(fieldNameW));  // Since we need a pointer to a QValue we need to copy it.

	return reinterpret_cast<jlong>(value);
}

JNIEXPORT void JNICALL Java_be_vib_bits_QValue_setField(JNIEnv* env, jobject obj, jstring fieldName, jobject value)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);

	jlong valuePtr = env->GetLongField(value, qvalue_ptr_fieldID);
	assert(valuePtr != 0);

	WideString fieldNameW(env, fieldName);

	QValue* q = reinterpret_cast<QValue*>(ptr);
	QValue* qValue = reinterpret_cast<QValue*>(valuePtr);

	q->SetField(fieldNameW, *qValue);
}
