
#include "be_vib_bits_QUtils.h"

#include "quasar_dsl.h"

#include <algorithm>
#include <iostream>

using namespace quasar;

extern IQuasarHost* host;

extern jfieldID qvalue_ptr_fieldID;

JNIEXPORT void JNICALL Java_be_vib_bits_QUtils_inplaceMultiply(JNIEnv* env, jclass, jobject obj, jfloat s)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);
	QValue* q = reinterpret_cast<QValue*>(ptr);
	(*q) *= s;
}

JNIEXPORT void JNICALL Java_be_vib_bits_QUtils_inplaceDivide(JNIEnv* env, jclass, jobject obj, jfloat s)
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);
	QValue* q = reinterpret_cast<QValue*>(ptr);

	(*q) /= s;  // CHECKME: what about division by zero - does this throw a Quasar exception? If so we probably want to catch it and turn it into a Java exception
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QUtils_newCubeFromGrayscaleByteArrayNative(JNIEnv* env, jclass, jint width, jint height, jbyteArray pixels)
{
	// - We're assuming pixels represents an 8 bit grayscale image - TODO: document layout of data in 'pixels'
	// - Important note: Quasar cubes use the (y, x, z) indexing convention. See QuickReferenceManual.pdf section 2.1 page 11.

	QValue q = host->CreateCube<scalar>(height, width, 1);
	Cube cube = host->LockCube<scalar>(q);

	jbyte *values = env->GetByteArrayElements(pixels, nullptr);
	int3 sz = make_int3(height, width, 1);
	for (jint y = 0; y < height; y++)
	{
		for (jint x = 0; x < width; x++)
		{
			jint idx = y * width + x;
			float gray = (float)(values[idx] & 0xff);  // CHECKME: is the mask needed?

			// store gray value in Quasar cube object
			cube.data[pos2ind(sz, make_int3(y, x, 0))] = gray;
		}
	}
	env->ReleaseByteArrayElements(pixels, values, JNI_ABORT);
	host->UnlockCube(q);

	// TODO: check if we can avoid this - to be able to control lifetime of the cube on the Java side,
	//       we need to copy it onto the heap. Does this do a deep copy, or does it just make a new quasar smart pointer object?

	QValue* result = new QValue(q);
	return reinterpret_cast<jlong>(result);
}

JNIEXPORT jlong JNICALL Java_be_vib_bits_QUtils_newCubeFromGrayscaleShortArrayNative(JNIEnv* env, jclass, jint width, jint height, jshortArray pixels)
{
	// - We're assuming pixels represents a 16 bit grayscale image - TODO: document layout of data in 'pixels'
	// - Important note: Quasar cubes use the (y, x, z) indexing convention. See QuickReferenceManual.pdf section 2.1 page 11.

	QValue q = host->CreateCube<scalar>(height, width, 1);
	Cube cube = host->LockCube<scalar>(q);

	jshort *values = env->GetShortArrayElements(pixels, nullptr);
	int3 sz = make_int3(height, width, 1);
	for (jint y = 0; y < height; y++)
	{
		for (jint x = 0; x < width; x++)
		{
			jint idx = y * width + x;
			float gray = (float)(values[idx] & 0xffff);  // CHECKME: is the mask needed?

			// store gray value in Quasar cube object
			cube.data[pos2ind(sz, make_int3(y, x, 0))] = gray;
		}
	}
	env->ReleaseShortArrayElements(pixels, values, JNI_ABORT);
	host->UnlockCube(q);

	// TODO: check if we can avoid this - to be able to control lifetime of the cube on the Java side,
	//       we need to copy it onto the heap. Does this do a deep copy, or does it just make a new quasar smart pointer object?

	QValue* result = new QValue(q);
	return reinterpret_cast<jlong>(result);
}

JNIEXPORT jbyteArray JNICALL Java_be_vib_bits_QUtils_newGrayscaleByteArrayFromCube(JNIEnv* env, jclass, jint width, jint height, jobject obj)
// TODO: fix API -  we should actually return width and height as well
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);
	QValue* q = reinterpret_cast<QValue*>(ptr);

	Cube cube = host->LockCube<scalar>(*q);

	jbyteArray pixels = env->NewByteArray(width * height);

	// FIXME: remove width and height parameters, and get them from the Quasar cube instead

	jbyte *values = env->GetByteArrayElements(pixels, nullptr);
	int3 sz = make_int3(height, width, 1);
	for (jint y = 0; y < height; y++)
	{
		for (jint x = 0; x < width; x++)
		{
			// read gray value from quasar cube object
			// (note: Quasar cubes use (y, x, z) indexing convention)
			float gray = cube.data[pos2ind(sz, make_int3(y, x, 0))];

			// Clamp gray to [0, 255].
			gray = std::min(std::max(0.0f, gray), 255.0f);

			unsigned grayint = static_cast<unsigned>(gray);

			jbyte color = (jbyte)grayint;

			jint idx = y * width + x;
			values[idx] = color;
		}
	}
	env->SetByteArrayRegion(pixels, 0, width * height, values);
	env->ReleaseByteArrayElements(pixels, values, JNI_ABORT);

	host->UnlockCube(*q);

	return pixels;
}

JNIEXPORT jshortArray JNICALL Java_be_vib_bits_QUtils_newGrayscaleShortArrayFromCube(JNIEnv* env, jclass, jint width, jint height, jobject obj)
// TODO: fix API -  we should actually return width and height as well
{
	jlong ptr = env->GetLongField(obj, qvalue_ptr_fieldID);
	assert(ptr != 0);
	QValue* q = reinterpret_cast<QValue*>(ptr);

	Cube cube = host->LockCube<scalar>(*q);

	jshortArray pixels = env->NewShortArray(width * height);

	// FIXME: remove width and height parameters, and get them from the Quasar cube instead

	jshort *values = env->GetShortArrayElements(pixels, nullptr);
	int3 sz = make_int3(height, width, 1);
	for (jint y = 0; y < height; y++)
	{
		for (jint x = 0; x < width; x++)
		{
			// read gray value from quasar cube object
			// (note: Quasar cubes use (y, x, z) indexing convention)
			float gray = cube.data[pos2ind(sz, make_int3(y, x, 0))];

			// Clamp gray to [0, 65535].
			gray = std::min(std::max(0.0f, gray), 65535.0f);

			unsigned grayint = static_cast<unsigned>(gray);

			jshort color = (jshort)grayint;

			jint idx = y * width + x;
			values[idx] = color;
		}
	}
	env->SetShortArrayRegion(pixels, 0, width * height, values);
	env->ReleaseShortArrayElements(pixels, values, JNI_ABORT);

	host->UnlockCube(*q);

	return pixels;
}

// IMPROVEME: can we unify byte and short versions using a template function or so

