package be.vib.bits;

public class QHost
{
	static
	{		
		System.loadLibrary("QuasarBridge"); // loads QuasarBridge.dll (on Windows)
	}
	
	public native static void init(String deviceName, boolean loadCompiler); // deviceName is "cuda" or "cpu" or ...
	public native static void release();
	
	public native static void loadSourceModule(String modulePath); // load .q
	public native static void loadBinaryModule(String modulePath); // load .qlib
	public native static void loadModuleFromSource(String moduleName, String sourceString);
	
	public native static boolean unloadModule(String moduleName);
	
	public native static boolean functionExists(String name);
	
	public native static void runApp();
}
