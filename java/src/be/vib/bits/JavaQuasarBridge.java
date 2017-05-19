package be.vib.bits;

import java.io.IOException;

import be.vib.bits.jartools.Jar;

public class JavaQuasarBridge
{
	// TODO: check platform (windows, linux, mac)
	// TODO: check architecture (32/64 bit)
	public static void loadLibrary(String tempDir, boolean useEmbeddedQuasarLibrary) throws IOException, ClassNotFoundException
	{
		System.out.println("tempDir = " + tempDir);
		
		if (useEmbeddedQuasarLibrary)
		{
			Jar.extractResource(tempDir, "libraries/win64/Quasar.Runtime.dll");
			// TODO: and a lot more!
		}

		String bridgeLibrary = Jar.extractResource(tempDir, "libraries/win64/JavaQuasarBridge.dll");

		System.out.println("Loading " + bridgeLibrary);
		System.load(bridgeLibrary);

		// FIXME: delete temporary files / directories on exit
		// File outputFile = bridgeLibraryPath.toFile();
		// outputFile.deleteOnExit();    // FIXME: this does not seem to work?
	}
	
}
