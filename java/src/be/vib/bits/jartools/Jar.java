package be.vib.bits.jartools;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Jar
{
	public static String extractResource(String targetDirectory, String resource) throws IOException, ClassNotFoundException
	{		
		String resourceFilename = Paths.get(resource).getFileName().toString();			
		Path outputPath = Paths.get(targetDirectory, resourceFilename);
		
		// Create intermediate folders of the location where the JAR resource will be extract to.
		File parentDir = new File(outputPath.getParent().toString());
		parentDir.mkdirs();

		// Extract the resource.
		System.out.println("Extracting " + resource + " from JAR to " + outputPath.toString());
		ClassLoader classLoader = Class.forName("be.vib.bits.JavaQuasarBridge").getClassLoader();
		InputStream inputStream = classLoader.getResourceAsStream(resource);		
		Files.copy(inputStream, outputPath);
		
		return outputPath.toString();
	}
}
