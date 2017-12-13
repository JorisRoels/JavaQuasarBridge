
#include "be_vib_bits_QTypeBuilder.h"

#include "quasar_dsl.h"

#include "ExceptionHandling.h"
#include "WideString.h"

#include <iostream>

using namespace quasar;

extern jfieldID qvalue_ptr_fieldID;

JNIEXPORT jlong JNICALL Java_be_vib_bits_QTypeBuilder_newQTypeBuilder(JNIEnv* env, jclass, jstring moduleName, jstring typeName)
{
	try
	{
		WideString moduleNameW(env, moduleName);
		WideString typeNameW(env, typeName);
		TypeBuilder* tb = new TypeBuilder(moduleNameW, typeNameW);
		return reinterpret_cast<jlong>(tb);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return 0;
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QTypeBuilder_addField(JNIEnv* env, jobject obj, jstring fieldName, jobject fieldType)
{
	try
	{
		WideString fieldNameW(env, fieldName);

		jlong typeBuilderPtr = env->GetLongField(obj, qvalue_ptr_fieldID);
		assert(typeBuilderPtr != 0);
		TypeBuilder* tb = reinterpret_cast<TypeBuilder*>(typeBuilderPtr);

		jlong typePtr = env->GetLongField(fieldType, qvalue_ptr_fieldID);
		assert(typePtr != 0);
		Type* t = reinterpret_cast<Type*>(typePtr);

		tb->AddField(fieldNameW, *t);
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}
