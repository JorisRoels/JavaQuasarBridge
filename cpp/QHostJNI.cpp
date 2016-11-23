#include "be_vib_bits_QHost.h"

#include "quasar_host.h"

#include "Utils.h"
#include "WideString.h"

#include <iostream>

using namespace quasar;

// Global variable host - used also in QValueJNI.cpp
IQuasarHost* host = nullptr;

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_init(JNIEnv *env, jclass, jstring deviceName, jboolean loadCompiler)
{
	if (host != nullptr)
	{
		ThrowByName(env, "java/lang/RuntimeException", "There can be only one Quasar host. Probably QHost.init() was called already. If you want to re-initialize the Quasar host then call QHost.release() first.");
		return;
	}

	WideString deviceW(env, deviceName);

 	host = quasar::IQuasarHost::Create(deviceW, loadCompiler == JNI_TRUE);
	if (host == nullptr)
	{
		ThrowByName(env, "java/lang/RuntimeException", "QHost.init() failed for an unknown reason.");
		return;
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_release(JNIEnv* env, jclass)
{
	if (host == nullptr)
	{
		ThrowByName(env, "java/lang/RuntimeException", "QHost.release() failed because there does not seem to be a Quasar host active. Did you call QHost.init() first?");
		return;
	}

	host->Release();
	host = nullptr;
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadSourceModule(JNIEnv *env, jclass, jstring modulePath)
{
	assert(host != nullptr);

	WideString pathW(env, modulePath);

	LPCWSTR errorW;
	bool loaded = host->LoadSourceModule(pathW, &errorW);
	if (!loaded)
	{
		ThrowByName(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadBinaryModule(JNIEnv *env, jclass, jstring modulePath)
{
	assert(host != nullptr);

	WideString pathW(env, modulePath);

	LPCWSTR errorW;
	bool loaded = host->LoadBinaryModule(pathW, &errorW);
	if (!loaded)
	{
		ThrowByName(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadModuleFromSource(JNIEnv *env, jclass, jstring moduleName, jstring sourceString)
{
	assert(host != nullptr);

	WideString moduleW(env, moduleName);
	WideString sourceW(env, sourceString);

	LPCWSTR errorW;
	bool loaded = host->LoadModuleFromSource(moduleW, sourceW, &errorW);
	if (!loaded)
	{
		ThrowByName(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
	}
}

JNIEXPORT jboolean JNICALL Java_be_vib_bits_QHost_unloadModule(JNIEnv *env, jclass, jstring moduleName)
{
	assert(host != nullptr);

	WideString moduleNameW(env, moduleName);

	bool unloaded = host->UnloadModule(moduleNameW);
 	return unloaded ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_be_vib_bits_QHost_functionExists(JNIEnv *env, jclass, jstring functionName)
{
	assert(host != nullptr);
	WideString functionNameW(env, functionName);
	bool exists = host->FunctionExists(functionNameW);
	return exists ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_runApp(JNIEnv *, jclass)
{
	assert(host != nullptr);
	host->RunApp();
}
