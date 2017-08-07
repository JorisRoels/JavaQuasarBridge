package be.vib.bits;

import java.io.IOException;

import be.vib.bits.jartools.Jar;

public class JavaQuasarBridge
{
	// TODO: check platform (windows, linux, mac) and architecture (32/64 bit) - for now only 64bit Windows is supported
	public static void loadLibrary(String tempDir) throws IOException, ClassNotFoundException
	{
		System.out.println("Extracting Java-Quasar bridge to temporary folder " + tempDir);		
		String bridgeLibrary = Jar.extractResource(tempDir, "libraries/win64/JavaQuasarBridge.dll");

		System.out.println("Loading " + bridgeLibrary);
		System.load(bridgeLibrary);

		// TODO: delete temporary files / directories on exit
		// File outputFile = bridgeLibraryPath.toFile();
		// outputFile.deleteOnExit();    // this does not seem to work?
	}
	
}
