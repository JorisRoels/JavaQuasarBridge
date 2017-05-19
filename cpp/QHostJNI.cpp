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

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_enableProfiling(JNIEnv* env, jclass)
{
	assert(host != nullptr);
	bool success = host->EnableProfiling(PROFILE_MEMLEAKS);  // also possible: PROFILE_ACCURACY or PROFILE_EXECUTIONTIME
	if (!success)
	{
		ThrowByName(env, "java/lang/RuntimeException", "QHost.enableProfile() failed");
	}
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_runApp(JNIEnv *, jclass)
{
	assert(host != nullptr);
	host->RunApp();
}

JNIEXPORT void JNICALL Java_be_vib_bits_QHost_printMachineInfo(JNIEnv *, jclass)
{
	// Small experiment. We should actually wrap the Quasar structures
	// MachineInfo, CUDAInfo, etc., in Java objects and offer those in the bridge API,
	// but for now we just print some information.

	try
	{
		const MachineInfo machineInfo = host->GetMachineInfo();
		std::wcout << "Host name: " << machineInfo.hostName << std::endl;
		std::wcout << "Domain name: " << machineInfo.domainName << std::endl;
		std::wcout << "IP address: " << machineInfo.ipAddress << std::endl;
		std::wcout << "CPU: " << std::endl;
		std::wcout << "  name: " << machineInfo.cpu.cpuName << std::endl;
		std::wcout << "  cores: " << machineInfo.cpu.numCPUcores << std::endl;
		std::wcout << "  platform: " << (machineInfo.cpu.platform == PLATFORM_WINDOWS ? "Windows" :
			                         	 machineInfo.cpu.platform == PLATFORM_UNIX_LINUX ? "Unix/Linux" :
										 machineInfo.cpu.platform == PLATFORM_MACOS ? "MacOS" : "Unknown") << std::endl;
		std::wcout << "  platform version: " << (LPCWSTR)machineInfo.cpu.platformVersion.ToString() << std::endl;
		std::wcout << "  bits: " << machineInfo.cpu.archBits << std::endl;
		std::wcout << "  total memory: ~" << machineInfo.cpu.totalMemory << " bytes" << std::endl; // totalMemory doesn't return exactly 16 GB on my 16 GB laptop ?!

		if (machineInfo.cuda.installed)
		{
			string_t cudaRuntimeVer = machineInfo.cuda.runtimeVersion.ToString();
			string_t cudaDriverVer = machineInfo.cuda.driverVersion.ToString();

			std::wcout << "CUDA installed. " << std::endl;
			std::wcout << "  runtime version " << (LPCWSTR)cudaRuntimeVer << std::endl;
			std::wcout << "  driver version " << (LPCWSTR)cudaDriverVer << std::endl;
		}
		else
		{
			std::wcout << "CUDA not installed." << std::endl;
		}

		if (machineInfo.opencl.installed)
		{
			std::wcout << "OpenCL installed." << std::endl;
			for (int i = 0; i < machineInfo.opencl.numPlatforms; ++i)
			{
				const OpenCLPlatformInfo *platform = machineInfo.opencl.platforms[i];
				std::wcout << "  " << i + 1 << " '" << platform->name << "' vendor: '" << platform->vendor << "' version: '" << platform->version << "'" << std::endl;
			}
		}
		else
		{
			std::wcout << "OpenCL not installed." << std::endl;
		}

		std::wcout << "Available computation devices:" << std::endl;
		for (int i = 0; i < machineInfo.numComputationDevices; ++i)
		{
			const ComputationDeviceInfo *dev = machineInfo.computationDevices[i];
			std::wcout << "  " << i + 1 << " " << (dev->target == DEVTYPE_CPU ? "cpu" :
				                                   dev->target == DEVTYPE_CUDA ? "cuda" :
			                                       dev->target == DEVTYPE_OPENCL ? "opencl" :
                                                   dev->target == DEVTYPE_HYPERION ? "hyperion" : "unknown") << " device '" << dev->name << "'" << std::endl;
		}

		host->FreeMachineInfo(const_cast<MachineInfo&>(machineInfo));
	}
	catch (exception_t ex)
	{
		std::wcout << "An error occurred: " << (LPCWSTR)ex.message << std::endl;
	}
}
