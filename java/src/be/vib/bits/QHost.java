package be.vib.bits;

public class QHost
{
	public native static void setenv(String str); // str is of the form "key=value"
	
	public native static void init(String deviceName, boolean loadCompiler); // deviceName is "cuda" or "cpu" or ...
	public native static void release(); // note: After release() the Quasar host cannot be used anymore. It cannot be re-initialized.
	public native static boolean initialized(); // returns if the Quasar host was initialized already (for example by another process)
	
	public native static void loadSourceModule(String modulePath); // load .q (throws a RunTimeException if loading failed)
	public native static void loadBinaryModule(String modulePath); // load .qlib (throws a RunTimeException if loading failed)
	
	public native static void loadModuleFromSource(String moduleName, String sourceString);
	
	public native static boolean unloadModule(String moduleName);
	
	public native static boolean functionExists(String name);
	
	public enum ProfilingMode { MEMLEAKS }; // TODO: test ACCURACY and EXECUTIONTIME profiling modes
	public native static void enableProfiling(ProfilingMode mode);
	
	public native static void runApp();
	
	public native static void printMachineInfo();
}
