package be.vib.bits;

public class QHost
{
	public native static void init(String deviceName, boolean loadCompiler); // deviceName is "cuda" or "cpu" or ...
	public native static void release();
	
	public native static void loadSourceModule(String modulePath); // load .q (throws a RunTimeException if loading failed)
	public native static void loadBinaryModule(String modulePath); // load .qlib (throws a RunTimeException if loading failed)
	
	public native static void loadModuleFromSource(String moduleName, String sourceString);
	
	public native static boolean unloadModule(String moduleName);
	
	public native static boolean functionExists(String name);
	
	public native static void enableProfiling(); // TODO: different profiling modes - for now always memleaks profiling
	
	public native static void runApp();
}
