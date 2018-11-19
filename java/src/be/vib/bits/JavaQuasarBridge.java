package be.vib.bits;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Comparator;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;

public class JavaQuasarBridge
{
    private static boolean quasarStarted = false;

	static
	{
		try
		{
			System.out.println("About to load JavaQuasarBridge dynamic library");
			JavaQuasarBridge.loadLibrary(); // Just loads the JNI interface, it does not call Quasar itself yet. The Quasar path can be set afterwards (if it's not the default one).
			System.out.println("JavaQuasarBridge dynamic library loaded.");
		}
		catch (ClassNotFoundException | IOException e)
		{
			e.printStackTrace();
		}
	}
	
	// Load the Java-Quasar interface. It is implemented in the JavaQuasarBridge.dll
	// Note: for now only 64-bit Windows is supported.
	public static void loadLibrary() throws IOException, ClassNotFoundException
	{
		// Extract the Java-Quasar bridge dynamic library from the JAR into a temporary folder.
		String tempFolder = Files.createTempDirectory("javaquasarbridge").toString();
		Path bridgeLibrary = extractResource("be.vib.bits.JavaQuasarBridge", "libraries/win64/JavaQuasarBridge.dll", tempFolder);

		// Load the Java-Quasar bridge dll
		System.out.println("Loading " + bridgeLibrary);
		System.load(bridgeLibrary.toString());

		// On exit, delete the extracted dll again.
		// (Not guaranteed to succeed?)
		File outputFile = bridgeLibrary.toFile();
		outputFile.deleteOnExit(); 
	}
	
	// Point Quasar to the path where its runtime is located.
	// This is only needed when using the minimal Quasar installation;
	// otherwise QUASAR_PATH will be set already.
	public static void setQuasarPath(String path)
	{
		QHost.setenv("QUASAR_PATH=" + path);
	}
	
	public static String getQuasarPath()
	{
		return System.getenv("QUASAR_PATH");
	}
	
	public static void startQuasar(String device, boolean loadCompiler) throws InterruptedException, ExecutionException
	{
		// TODO: this naive flag avoids repeated initialization from the same thread, but we still need to fix
		// concurrent initialization from different threads (e.g. via the ImageJ plugin menu and via an ImageJ script,
		// but such a situation is unlikely though).
		if (quasarStarted)
			return;
		
		Callable<Void> task = () -> {
			System.out.println("QHost.init(device=" + device + ", loadcompiler=" + loadCompiler + ")");
			QHost.init(device, loadCompiler);
			
			QHost.printMachineInfo(); // TODO: remove; for debugging only
			
			return null;
		};
		
		// Initialize Quasar now and wait for it to complete.
		QExecutor.getInstance().submit(task).get();
		
		quasarStarted = true;
	}
		
	// module: name of a .q or .qlib quasar module
	public static void extractAndLoadModule(String className, String resource, String module, String prefix) throws InterruptedException, ExecutionException
	{
		System.out.println("extractAndLoadModule className=" + className + " resource=" + resource + " module=" + module + " prefix=" + prefix);

		assert(quasarStarted);
		
		Callable<Void> task = () -> {
			// Create a temporary folder. The Quasar module will be extracted
			// from the JAR file into that folder and loaded from there.
		    String quasarTempFolder = Files.createTempDirectory(prefix).toString();
			addFolderDeleteHook(quasarTempFolder);

		    System.out.println("Extracting resource " + resource + " to " + quasarTempFolder);
			extractResource(className, resource, quasarTempFolder);

			System.out.println("Loading Quasar module " + module);
			loadQuasarModule(quasarTempFolder, module);
						
			return null;
		};
		
		QExecutor.getInstance().submit(task).get();		
	}
	
	public static Path extractResource(String className, String resource, String targetDirectory) throws IOException, ClassNotFoundException
	{		
		System.out.println("extractResource className=" + className + " resource=" + resource + " targetDirectory=" + targetDirectory);
		
		String resourceFilename = Paths.get(resource).getFileName().toString();			
		Path outputPath = Paths.get(targetDirectory, resourceFilename);
		
		// Create intermediate folders of the location where the JAR resource will be extract to.
		File parentDir = new File(outputPath.getParent().toString());
		parentDir.mkdirs();

		// Extract the resource.
		System.out.println("Extracting " + resource + " from JAR to " + outputPath.toString());
		ClassLoader classLoader = Class.forName(className).getClassLoader();
		InputStream inputStream = classLoader.getResourceAsStream(resource);		
		Files.copy(inputStream, outputPath);
		
		return outputPath;
	}	
	
	public static void loadQuasarModule(String folder, String filename)
	{
		String module = Paths.get(folder, filename).toString();
		
		if (module.endsWith(".q"))
		{
			QHost.loadSourceModule(module);
		}
		else // a .qlib
		{
			QHost.loadBinaryModule(module);
		}		
	}

	private static void addFolderDeleteHook(String folder)
	{
		  Runtime.getRuntime().addShutdownHook(new Thread() {
		        @Override
		        public void run()
		        {
					try
					{
						Path rootPath = Paths.get(folder);
						Files.walk(rootPath)
						    .sorted(Comparator.reverseOrder())
						    .map(Path::toFile)
						    //.peek(System.out::println)
						    .forEach(File::delete);
					}
					catch (IOException e)
					{
						e.printStackTrace();
					}
		        }
		    });		
	}

	public static void addQuasarShutdownHook()
	{
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override
			public void run()
			{
				Callable<Void> task = () -> {
					System.out.println("QHost.release()");
					QHost.release();
					System.out.println("Quasar host released");					
					return null;
				};
				
				try
				{
					QExecutor.getInstance().submit(task).get();
				}
				catch (InterruptedException | ExecutionException e)
				{
					e.printStackTrace();
				}				
			}
		});			
	}
	
}
