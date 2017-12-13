#include "be_vib_bits_QValue.h"

#include "quasar_dsl.h"

#include "ExceptionHandling.h"
#include "WideString.h"

using namespace quasar;

extern IQuasarHost* host;

static_assert(sizeof(jlong) == 8, "jlong must be 64 bit");  // so it can be used to hold a C++ pointer

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
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		QValue* q = reinterpret_cast<QValue*>(ptr);

		return static_cast<int>(*q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jfloat JNICALL Java_be_vib_bits_QValue_getFloat(JNIEnv* env, jobject obj)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		QValue* q = reinterpret_cast<QValue*>(ptr);
		float f = static_cast<float>(*q);

		return f;
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0.0;
	}
}

JNIEXPORT jstring JNICALL Java_be_vib_bits_QValue_getString(JNIEnv* env, jobject obj)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		QValue* q = reinterpret_cast<QValue*>(ptr);
		string_t s = static_cast<string_t>(*q);

		const jchar* uchars = reinterpret_cast<const jchar*>(s.get_buf());
		jsize len = s.get_length(); // len = number of Unicode characters, not the number of bytes
		return env->NewString(uchars, len);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return nullptr;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__(JNIEnv* env, jclass)
{
	try
	{
		QValue *q = new QValue();
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__I(JNIEnv *env, jclass, jint i)
{
	try
	{
		QValue *q = new QValue(i);
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_newQValue__F(JNIEnv *env, jclass, jfloat f)
{
	try
	{
		QValue *q = new QValue(f);
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

template<typename JavaArrType, typename JavaElemType, typename QuasarElemType>
jlong newQValueArrayHelper(JNIEnv* env, JavaArrType arr)
{
	try
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
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
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
	try
	{
		WideString stringW(env, s);
		QValue* q = new QValue(stringW);
		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QValue_dispose(JNIEnv* env, jobject obj)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		if (ptr == 0) return;  // ptr being 0 is a perfectly acceptable code path - not an error.

		QValue* q = reinterpret_cast<QValue*>(ptr);
		delete q;  // If all went fine, there now are no more references to the C++ QValue and it got destructed nicely.

		env->SetLongField(obj, qvalue_ptr_fieldID, 0);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QValue_retain(JNIEnv* env, jobject obj)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		if (ptr == 0) return;

		QValue* q = reinterpret_cast<QValue*>(ptr);

		host->AddRef(static_cast<qvalue_t>(*q));
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_readhostVariableNative(JNIEnv* env, jclass, jstring varName)
{
	try
	{
		WideString varNameW(env, varName);
		QValue* q = new QValue();  // We need the QValue on the heap so we can control its lifetime from the Java side

		bool success = QValue::ReadHostVariable(varNameW, *q);
		if (!success)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "QValue.readhostVariable() failed");
			return 0;
		}

		return reinterpret_cast<jlong>(q);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jint JNICALL Java_be_vib_bits_QValue_size(JNIEnv* env, jobject obj, jint dim)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		QValue* q = reinterpret_cast<QValue*>(ptr);
		return size(*q, static_cast<int>(dim));
		// About the type cast to int in the line above:
		// jint is a long, and if we call size(QValue, long) we call the quasar_dsl.h
		// template <class T> INLINE QValue(QValue cal, T dim)
		// and the resulting QValue (of type scalar) is then converted via "template <class R> operator R() const"
		// in quasar_dsl.h to a long, but that type conversion does not support scalars, so instead of returning the value of the scalar,
		// the result is just "long()", so always 0. Which is wrong.
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_sizeNative(JNIEnv* env, jobject obj)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		QValue* q = reinterpret_cast<QValue*>(ptr);

		QValue *s = new QValue(size(*q)); // Copy construct a QValue object on the heap with the result

		return reinterpret_cast<jlong>(s);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

template<typename ...Indices>
jlong atNativeHelper(JNIEnv* env, jobject obj, Indices... indices)
{
	try
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
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
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
	try
	{
		const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
		return atNativeHelper<const QValue&>(env, obj, *q1);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNativeQ__JJ(JNIEnv* env, jobject obj, jlong ptr1, jlong ptr2)
{
	try
	{
		const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
		const QValue* q2 = reinterpret_cast<QValue*>(ptr2);
		return atNativeHelper<const QValue&>(env, obj, *q1, *q2);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_atNativeQ__JJJ(JNIEnv* env, jobject obj, jlong ptr1, jlong ptr2, jlong ptr3)
{
	try
	{
		const QValue* q1 = reinterpret_cast<QValue*>(ptr1);
		const QValue* q2 = reinterpret_cast<QValue*>(ptr2);
		const QValue* q3 = reinterpret_cast<QValue*>(ptr3);
		return atNativeHelper<const QValue&>(env, obj, *q1, *q2, *q3);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QValue_getFieldNative(JNIEnv* env, jobject obj, jstring fieldName)
{
	try
	{
		jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(ptr != 0);

		WideString fieldNameW(env, fieldName);

		QValue* q = reinterpret_cast<QValue*>(ptr);

		QValue* value = new QValue(q->GetField(fieldNameW));  // Since we need a pointer to a QValue we need to copy it.

		return reinterpret_cast<jlong>(value);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QValue_setField(JNIEnv* env, jobject obj, jstring fieldName, jobject value)
{
	try
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
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}
