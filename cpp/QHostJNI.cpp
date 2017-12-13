#include "be_vib_bits_QHost.h"

#include "quasar_host.h"

#include "ExceptionHandling.h"
#include "Encoding.h"
#include "WideString.h"

#include <iostream>
#include <cassert>

using namespace quasar;

// Global variable host - used also in QValueJNI.cpp
IQuasarHost* host = nullptr;

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_init(JNIEnv *env, jclass, jstring deviceName, jboolean loadCompiler)
{
	try
	{
		if (host != nullptr)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "The Quasar host was already initialized, and initialization can only occur once.");
			return;
		}

		WideString deviceW(env, deviceName);

		host = quasar::IQuasarHost::Create(deviceW, loadCompiler == JNI_TRUE);
		if (host == nullptr)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "QHost.init() failed for an unknown reason.");
		}
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_release(JNIEnv* env, jclass)
{
	try
	{
		if (host == nullptr)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "QHost.release() failed because there does not seem to be a Quasar host active. Did you call QHost.init() first?");
			return;
		}

		host->Release();
		host = nullptr;
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadSourceModule(JNIEnv *env, jclass, jstring modulePath)
{
	try
	{
		assert(host != nullptr);

		WideString pathW(env, modulePath);

		LPCTSTR errorW;
		bool loaded = host->LoadSourceModule(pathW, &errorW);
		if (!loaded)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
		}
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadBinaryModule(JNIEnv *env, jclass, jstring modulePath)
{
	try
	{
		assert(host != nullptr);

		WideString pathW(env, modulePath);

		LPCTSTR errorW;
		bool loaded = host->LoadBinaryModule(pathW, &errorW);
		if (!loaded)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
		}
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_loadModuleFromSource(JNIEnv *env, jclass, jstring moduleName, jstring sourceString)
{
	try
	{
		assert(host != nullptr);

		WideString moduleW(env, moduleName);
		WideString sourceW(env, sourceString);

		LPCTSTR errorW;
		bool loaded = host->LoadModuleFromSource(moduleW, sourceW, &errorW);
		if (!loaded)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", UTF16toModifiedUTF8(errorW));
		}
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT jboolean JNICALL Java_be_vib_bits_QHost_unloadModule(JNIEnv *env, jclass, jstring moduleName)
{
	try
	{
		assert(host != nullptr);

		WideString moduleNameW(env, moduleName);

		bool unloaded = host->UnloadModule(moduleNameW);
		return unloaded ? JNI_TRUE : JNI_FALSE;
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return JNI_FALSE;
	}
}

JNIEXPORT jboolean JNICALL Java_be_vib_bits_QHost_functionExists(JNIEnv *env, jclass, jstring functionName)
{
	try
	{
		assert(host != nullptr);
		WideString functionNameW(env, functionName);
		bool exists = host->FunctionExists(functionNameW);
		return exists ? JNI_TRUE : JNI_FALSE;
	}
	catch (...)
	{
		RethrowAsJavaException(env);
		return JNI_FALSE;
	}
}

namespace
{
	// Given a Java enum object that represents a Quasar profiling mode,
    // returns the corresponding C++ enum.
	ProfilingModes quasarProfilingMode(JNIEnv *env, jobject profilingEnum)
	{
		jclass enumClass = env->FindClass("be/vib/bits/QHost$ProfilingMode");
		assert(enumClass != nullptr);

		jmethodID nameMethod = env->GetMethodID(enumClass, "name", "()Ljava/lang/String;");
		assert(nameMethod != nullptr);

		jstring enumStringValue = (jstring)env->CallObjectMethod(profilingEnum, nameMethod);
		const char* value = env->GetStringUTFChars(enumStringValue, 0);
		if (value == nullptr)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "Failed to get value string of ProfilingMode object");
			return PROFILE_MEMLEAKS;
		}

		ProfilingModes profilingMode;
		if (!strcmp(value, "MEMLEAKS"))
		{
			profilingMode = PROFILE_MEMLEAKS;
		}
		else if (!strcmp(value, "ACCURACY"))
		{
			profilingMode = PROFILE_ACCURACY;
		}
		else if (!strcmp(value, "EXECUTIONTIME"))
		{
			profilingMode = PROFILE_EXECUTIONTIME;
		}
		else
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "Unsupported Quasar profiling mode"); // IMPROVEME: add name of unsupported mode in exception string
			// Deliberate fall-through
		}

		env->ReleaseStringUTFChars(enumStringValue, value);

		return profilingMode;
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_enableProfiling(JNIEnv* env, jclass, jobject profilingEnum)
{
	try
	{
		assert(host != nullptr);

		ProfilingModes profilingMode = quasarProfilingMode(env, profilingEnum);
		if (env->ExceptionCheck() == JNI_TRUE)
		{
			// Failed to set quasar profiling mode.
			// An exception is pending - bail out.
			return;
		}

		bool success = host->EnableProfiling(profilingMode);
		if (!success)
		{
			ThrowJavaException(env, "java/lang/RuntimeException", "QHost.enableProfile() failed");
		}
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_runApp(JNIEnv* env, jclass)
{
	try
	{
		assert(host != nullptr);
		host->RunApp();
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_printMachineInfo(JNIEnv* env, jclass)
{
	// Small experiment. We should actually wrap the Quasar structures
	// MachineInfo, CUDAInfo, etc., in Java objects and offer those in the bridge API,
	// but for now we just print some information.

	try
	{
		const MachineInfo machineInfo = host->GetMachineInfo();
		tprintf(_T("Host name: %s\n"), machineInfo.hostName);
		tprintf(_T("Domain name: %s\n"), machineInfo.domainName);
		tprintf(_T("IP address: %s\n"), machineInfo.ipAddress);
		tprintf(_T("CPU:\n"));
		tprintf(_T("  name: %s\n"), machineInfo.cpu.cpuName);
		tprintf(_T("  cores: %d\n"), machineInfo.cpu.numCPUcores);
		tprintf(_T("  platform: %s\n"), machineInfo.cpu.platform == PLATFORM_WINDOWS ? _T("Windows") :
			                         	machineInfo.cpu.platform == PLATFORM_UNIX_LINUX ? _T("Unix/Linux") :
										machineInfo.cpu.platform == PLATFORM_MACOS ? _T("MacOS") : _T("Unknown"));
		tprintf(_T("  platform version: %s\n"), (LPCTSTR)machineInfo.cpu.platformVersion.ToString());
		tprintf(_T("  bits: %d\n"), machineInfo.cpu.archBits);
		tprintf(_T("  memory: %.1f GB\n"), machineInfo.cpu.totalMemory / static_cast<float>(1024 * 1024 * 1024)); // totalMemory is the same as the value shown in Windows Task Manager (but not identical to the raw chip size)

		if (machineInfo.cuda.installed)
		{
			string_t cudaRuntimeVer = machineInfo.cuda.runtimeVersion.ToString();
			string_t cudaDriverVer = machineInfo.cuda.driverVersion.ToString();

			tprintf(_T("CUDA installed.\n"));
			tprintf(_T("  runtime version %s\n"), (LPCTSTR)cudaRuntimeVer);
			tprintf(_T("  driver version %s\n"), (LPCTSTR)cudaDriverVer);;
		}
		else
		{
			tprintf(_T("CUDA not installed.\n"));
		}

		if (machineInfo.opencl.installed)
		{
			tprintf(_T("OpenCL installed.\n"));
			for (int i = 0; i < machineInfo.opencl.numPlatforms; ++i)
			{
				const OpenCLPlatformInfo *platform = machineInfo.opencl.platforms[i];
				tprintf(_T("  %d '%s' vendor: '%s' version: '%s'\n"), i + 1, platform->name, platform->vendor, platform->version);
			}
		}
		else
		{
			tprintf(_T("OpenCL not installed.\n"));
		}

		tprintf(_T("Available computation devices:\n"));
		for (int i = 0; i < machineInfo.numComputationDevices; ++i)
		{
			const ComputationDeviceInfo *dev = machineInfo.computationDevices[i];
			tprintf(_T("  %d %s device '%s'\n"), i + 1,
					                             (dev->target == DEVTYPE_CPU ? _T("cpu") :
				                                  dev->target == DEVTYPE_CUDA ? _T("cuda") :
			                                      dev->target == DEVTYPE_OPENCL ? _T("opencl") :
                                                  dev->target == DEVTYPE_HYPERION ? _T("hyperion") : _T("unknown")),
												 dev->name);
		}

		host->FreeMachineInfo(const_cast<MachineInfo&>(machineInfo));
	}
	catch (...)
	{
		RethrowAsJavaException(env);
	}
}
