
import java.io.IOException;
import java.util.concurrent.ExecutionException;

import be.vib.bits.JavaQuasarBridge;
import be.vib.bits.QExecutor;
import be.vib.bits.QFunction;
import be.vib.bits.QHost;
import be.vib.bits.QMethod;
import be.vib.bits.QRange;
import be.vib.bits.QType;
import be.vib.bits.QTypeBuilder;
import be.vib.bits.QUtils;
import be.vib.bits.QValue;

public class Test
{		
	// Set haveQuasarLicense to true if you obtained a license for Quasar development;
	// set it to false if you only have a minimal Quasar for executing pre-compiled Quasar code.
	final static boolean haveQuasarLicense = true;
	
	static String quasarFolder;
	
	static
	{				
		try
		{			
			System.out.println("About to load JavaQuasarBridge native library.");
			JavaQuasarBridge.loadLibrary();
			System.out.println("JavaQuasarBridge native library loaded.");
		}
		catch (ClassNotFoundException | IOException e)
		{
			e.printStackTrace();
		}
	}

	public static void main(String[] args) throws InterruptedException, ExecutionException
	{
		// We're using the QExecutor to run all Quasar code on a the same single Quasar thread.
		// Beware: executor submit() will swallow exceptions generated by the submitted task,
		//         whereas execute() will print a stacktrace - we want the latter for testing so we see failed asserts!
		
		quasarFolder = System.getenv("QUASAR_PATH");
		if (quasarFolder == null)
		{
			// QUASAR_PATH must to be set: either to the full Quasar folder, or to the folder with the minimal Quasar.			
			throw new RuntimeException("The environment variable QUASAR_PATH is not set.");
		}
		
		System.out.println("QUASAR_PATH=" + quasarFolder);
		
		boolean loadCompiler = haveQuasarLicense;
		JavaQuasarBridge.startQuasar("cuda", loadCompiler);
		
		QExecutor.getInstance().execute(() -> {
									
			QHost.printMachineInfo();
			
			// Request feedback on Quasar memory leaks
			// QHost.enableProfiling(QHost.ProfilingMode.MEMLEAKS);
			
			testSimpleValues();
			
			testFloatArray();

			testIntArray();
			
			testInplaceOps();

			testInplaceOps2();

			testFunction1();
	
			testFunction2();
			
			testFunctionWithTooManyArgs();
									
			testRange();
			
			testArraySlicing();
			
			testUserTypes();
			
			testMisc();
			
			// Perform additional tests of functionality for which a Quasar license is required,
			// such as compiling .q files.			
			if (haveQuasarLicense)
			{			
				System.out.println("Performing additional tests that require a full Quasar installation and a license.");
				testLoadSource(quasarFolder);			
				testLoadSourceFromString();
				testGaussian();
				testArrayAccess(quasarFolder);
			}
				
			System.out.println("About to release the Quasar host.");
			
			QHost.release();
		});
			
		// Wait for tasks to complete and then stop the executor.
		// (Without it the executor will keep waiting for new tasks
		// and the program will not terminate.)
		QExecutor.getInstance().shutdown();
	}
	
	private static void testSimpleValues()
	{
		final int answer = 42;
		final float pi = 3.14f;
		final String haiku = "furu ike ya / kawazu tobikomu / mizu no oto";
		final String nihongo = "\u65E5\u672C\u8A9E";
		
		QValue qv = new QValue();
		QValue qi = new QValue(answer);
		QValue qf = new QValue(pi);
		QValue qs1 = new QValue(haiku);
		QValue qs2 = new QValue(nihongo);
		
		assert(qi.getInt() == answer);		
		assert(qf.getFloat() == pi);
		assert(qs1.getString().equals(haiku));
		assert(qs2.getString().equals(nihongo));
		
		qv.dispose();
		qi.dispose();
		qf.dispose();
		qs1.dispose();
		qs2.dispose();
	}
	
	private static void testFloatArray()
	{
		float[] a = new float[10000];
		for (int i = 0; i < a.length; i++)
			a[i] = i;
		
		QValue q = new QValue(a);
	
		// Check size
		assert(q.size(0) == 1);
		assert(q.size(1) == 10000);
		assert(q.size(2) == 1);
		
		// Check content
		System.out.println(q.at(5).getFloat());
		System.out.println(q.at(0,5,0).getFloat());
		assert(q.at(5000).getFloat() == 5000.0f);
		
		QFunction print = new QFunction("print(...)");
		assert(QHost.functionExists("print"));
		
		print.apply(q);		
		
	    // Immediate manual cleanup - it's a "large" array.
		q.dispose();
	}

	private static void testIntArray()
	{
		int[] a = new int[10000];
		for (int i = 0; i < a.length; i++)
			a[i] = i;
		
		QValue q = new QValue(a);
	
		// Check size
		assert(q.size(0) == 1);
		assert(q.size(1) == 10000);
		assert(q.size(2) == 1);
		
		// Check content
		System.out.println(q.at(5000).getFloat());
		
		assert(q.at(5000).getFloat() == 5000.0f);   // ! note that the int array got turned into scalars (which are floats).
		
	    // Immediate manual cleanup - it's a "large" array.
		q.dispose();
	}
	
	private static void testInplaceOps()
	{
		QFunction print = new QFunction("print(...)");
		assert(QHost.functionExists("print"));
		
		QValue q = new QRange(0, 100);	
		print.apply(q);		
		QUtils.inplaceDivide(q, 10);		
		print.apply(q);
		QUtils.inplaceMultiply(q, 10);
		print.apply(q);

		q.dispose();		
	}

	private static void testInplaceOps2()
	{
		QFunction imshow = new QFunction("imshow(cube)");
		QFunction imread = new QFunction("imread(string)");
		
		QValue image = imread.apply(new QValue("lena_big.tif"));
		imshow.apply(image);

		QUtils.inplaceDivide(image, 2);				
		imshow.apply(image);

		image.dispose();		
	}
	
	private static void testLoadSource(String quasarInstallationFolder)
	{
		// IMPROVEME: avoid dependency on external files
		QHost.loadSourceModule(quasarInstallationFolder + "\\Library\\dwt.q");
		
		boolean exists = QHost.functionExists("dwt2d");
		assert(exists);
	}

	private static void testLoadSourceFromString()
	{
		String program = 
				"function [] = myprint(x)\n"
				+ "  print(x)\n"
				+ "end";
				
		QHost.loadModuleFromSource("mymodule", program);
				
		boolean exists = QHost.functionExists("myprint");
		assert(exists);
		
		boolean unloaded = QHost.unloadModule("mymodule");
		assert(unloaded);
	}
	
	private static void testFunction1()
	{
		QFunction tic = new QFunction("tic()");
		QFunction toc = new QFunction("toc()");
		QFunction imread = new QFunction("imread(string)");
		QFunction imshow = new QFunction("imshow(cube)");
		
		assert(QHost.functionExists("tic"));
		assert(QHost.functionExists("toc"));
		assert(QHost.functionExists("imread"));
		assert(QHost.functionExists("imshow"));

		tic.apply();
		QValue image = imread.apply(new QValue("lena_big.tif"));
		toc.apply(); // prints the time taken to read the image
		
		assert(image.size(0) == 512);
		assert(image.size(1) == 512);
		assert(image.size(2) == 3);

		imshow.apply(image);
		
		// Handle user interaction with the image window.
		// (Blocks until all image windows are closed.)
		QHost.runApp();
				
	    // Immediate manual cleanup - it's a "large" array.
		image.dispose();
	}

	private static void testFunction2()
	{
		// Build a Quasar array of floats
		float[] a = { 3.0f, -5.0f, 12.0f, -1.0f };
		QValue array = new QValue(a);
		
		// Print it
		QFunction print = new QFunction("print(...)");
		assert(QHost.functionExists("print"));
		
		print.apply(array);
		
		// Also calculate the maximum value in the array
		QFunction maximum = new QFunction("max(??)");
		assert(QHost.functionExists("max"));
		
		QValue maxVal = maximum.apply(array);
		assert(maxVal.getFloat() == 12.0f);
		
		// Let's calculate ln(e) via Quasar.
		QFunction ln = new QFunction("log(??)");
		assert(QHost.functionExists("log"));
		
		QValue e = new QValue(2.718281828f);  // more or less
		QValue one = ln.apply(e);
		assert(Math.abs(one.getFloat() - 1.0f) < 0.0001f);

		// Manually cleanup matrices in GPU memory
		array.dispose();
	}
	
	private static void testFunctionWithTooManyArgs()
	{
		QFunction f = new QFunction("print(...)");
		
		// QFunction.apply() supports up to 10 arguments
		boolean exceptionThrown = false;
		try
		{
			f.apply(new QValue(1), new QValue(2), new QValue(3), new QValue(4),
					new QValue(5), new QValue(6), new QValue(7), new QValue(8),
					new QValue(9), new QValue(10));
		}
		catch (java.lang.IllegalArgumentException e)
		{
			exceptionThrown = true;
		}
		assert(!exceptionThrown);

		// QFunction.apply() does not support more than 10 arguments and should throw an exception.
		exceptionThrown = false;
		try
		{
			f.apply(new QValue(1), new QValue(2), new QValue(3), new QValue(4),
					new QValue(5), new QValue(6), new QValue(7), new QValue(8),
					new QValue(9), new QValue(10), new QValue(11));
		}
		catch (java.lang.IllegalArgumentException e)
		{
			exceptionThrown = true;
		}
		assert(exceptionThrown);
	}
	
	private static void testGaussian()
	{
		// This Quasar fragment was borrowed from the sample program sample3_inlineprogram.cpp
		// which is part of the Quasar installation.
		String program =
				"function y = gaussian_filter(x, fc, n)\n"
				+ "	function [] = __kernel__ gaussian_filter_hor(x : cube, y : cube'unchecked, fc : vec'unchecked, n : int, pos : vec3)\n"
				+ "		sum = 0.\n"
				+ "		for i=0..numel(fc)-1\n"
		    	+ "			sum = sum + x[pos + [0,i-n,0]] * fc[i]\n"
				+ "		end\n"
				+ "		y[pos] = sum\n"
				+ "	end\n"
				+ "\n"
				+ "	function [] = __kernel__ gaussian_filter_ver(x : cube, y : cube'unchecked, fc : vec'unchecked, n : int, pos : vec3)\n"
				+ "		sum = 0.\n"
				+ "		for i=0..numel(fc)-1\n"
		    	+ "			sum = sum + x[pos + [i-n,0,0]] * fc[i]\n"
				+ "		end\n"
				+ "		y[pos] = sum\n"
				+ "	end\n"
				+ "\n"
				+ "	z = uninit(size(x))\n"
				+ "	y = uninit(size(x))\n"
				+ "	parallel_do (size(y), x, z, fc, n, gaussian_filter_hor)\n"
				+ "	parallel_do (size(y), z, y, fc, n, gaussian_filter_ver)\n"
				+ "end";
		
		QHost.loadModuleFromSource("example", program);   // TODO: check/ask what the meaning is of the moduleName in loadModuleFromSource
		
		boolean exists = QHost.functionExists("gaussian_filter");
		assert(exists);
		
		QFunction imread = new QFunction("imread(string)");
		QFunction imshow = new QFunction("imshow(cube)");
		QFunction filter = new QFunction("gaussian_filter(cube,vec,int)");
		
		QValue imageIn = imread.apply(new QValue("lena_big.tif"));
		System.out.println("Image dimensions: " + imageIn.size(0) + " x " + imageIn.size(1) + " x " + imageIn.size(2));

		float filterCoeff[] = { 1.0f / 9, 2.0f / 9, 3.0f / 9, 2.0f / 9, 1.0f / 9 };
		QValue coeff = new QValue(filterCoeff);
		QValue imageOut = filter.apply(imageIn, coeff, new QValue(2));
		coeff.dispose();

		QValue p1 = imshow.apply(imageIn);
		QValue p2 = imshow.apply(imageOut);

		// We now "connect" the two imshow windows.
		// (Connecting them has the effect that zooming and panning
		// in one image window will do the same zoom/pan in the other.)

		// QType t1 = new QType(p1);
		// QMethod connect = new QMethod(t1, "connect(??)");
		// connect.apply(p1, p2);  // Call the function p1.connect(p2)
		
		QHost.runApp();
		
		// Manually cleanup matrices in GPU memory
		imageIn.dispose();
		imageOut.dispose();
	}
	
	private static void testArrayAccess(String quasarInstallationFolder)
	{		
		QHost.loadSourceModule(quasarInstallationFolder + "\\Library\\dtcwt.q"); // IMPROVEME: add our own .q file specifically for this purpose, so we can avoid the dependency on this Quasar installation file
		
		QValue selcw = QValue.readhostVariable("filtercoeff_selcw");  // filtercoeff_selcw is a 6 x 6 cell array		
		assert(selcw.size(0) == 6);
		assert(selcw.size(1) == 6);
		
		QValue coeffs = selcw.at(3, 1); // coeffs is now a 2 x 12 matrix of scalars
		assert(coeffs.size(0) == 2);
		assert(coeffs.size(1) == 12);

		QValue coeff = coeffs.at(0, 1); // coeff is a scalar
		assert(coeff.size(0) == 1);
		assert(coeff.size(1) == 1);

		assert(Math.abs(coeff.getFloat() - 0.0133588733151555f) < 0.0001f);
		
		// Manually cleanup matrices in GPU memory.
		selcw.dispose();
		coeffs.dispose();
		coeff.dispose();
	}
	
	private static void testUserTypes()
	{
		// This is work in progress. Quasar user defined types are used only seldom; supporting them is not crucial.
		// The goal is to mimick the example %QUASAR_PATH%\Interop_Samples\Cpp_API\sample7_usertypes.cpp
		
		// This works.
		QTypeBuilder builder = new QTypeBuilder("sample", "point");
		builder.addField("x", new QType("int"));
		builder.addField("y", new QType("int"));
	
		// This does not work yet. It seems the object() function cannot be found.
		QFunction object = new QFunction("object()");		
		boolean exists = QHost.functionExists("object");
		System.out.println("object function exists? " + (exists ? "yes" : "no"));
		// assert(exists); // fails
	}
	
	private static void testRange()
	{
		QFunction maximum = new QFunction("max(??)");

		// Some integer ranges
		QRange range1 = new QRange(1, 10, 1);  // [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
		assert(maximum.apply(range1).getFloat() == 10.0f);  // Note: integer range became range of scalars
		assert(range1.size(1) == 10);
		
		QRange range2 = new QRange(1, 1, 10); // [ 1 ]
		assert(maximum.apply(range2).getFloat() == 1);  // Note: integer range became range of scalars
		assert(range2.size(1) == 1);
		
		QRange range3 = new QRange(1, 3, 10); //  [ 1 ]
		assert(maximum.apply(range3).getFloat() == 1);  // Note: integer range became range of scalars
		assert(range3.size(1) == 1);

		QRange range4 = new QRange(1, 10, 3); //  [ 1, 4, 7, 10 ]
		assert(maximum.apply(range4).getFloat() == 10);  // Note: integer range became range of scalars
		assert(range4.size(1) == 4);

		// A float range
		QRange range5 = new QRange(1.0f, 10.0f, 0.5f); // [ 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5, 10 ]
		assert(maximum.apply(range5).getFloat() == 10.0f);
		assert(range5.size(1) == 19);
		
		// Manually cleanup matrices in GPU memory.
		range1.dispose();
		range2.dispose();
		range3.dispose();
		range4.dispose();
		range5.dispose();
		maximum.dispose();
	}
	
	private static void testArraySlicing()
	{
		QFunction imread = new QFunction("imread(string)");
		assert(QHost.functionExists("imread"));

		QValue image = imread.apply(new QValue("lena_big.tif"));  // TODO: maybe copy this file into our own project, to avoid dependency on QUASAR installation folder?
		assert(image.size(0) == 512);
		assert(image.size(1) == 512);
		assert(image.size(2) == 3);

		// Slice channel 2 from the image (= the blue channel)
		QValue blueChannel = image.at(new QRange(), new QRange(), new QValue(2));
		
		assert(blueChannel.size(0) == image.size(0));
		assert(blueChannel.size(1) == image.size(1));
		assert(blueChannel.size(2) == 1);
		
		assert(blueChannel.at(100, 200).getFloat() == image.at(100, 200, 2).getFloat());
		
		// Crop the image
		QValue face = image.at(new QRange(128, 383), new QRange(128, 383), new QValue());
		
		assert(face.size(0) == 256);
		assert(face.size(1) == 256);
		assert(face.size(2) == 3);

		QFunction imshow = new QFunction("imshow(cube)");
		imshow.apply(face);
		QHost.runApp();
		
		// Manually cleanup matrices in GPU memory.
		image.dispose();
		blueChannel.dispose();
		face.dispose();
	}
	
	private static void testMisc()
	{
		// Not particularly useful for now,
		// but at least ensures that
		// QValue.retain() is implemented.
		QValue t = new QValue(42);
		t.retain();
		t.dispose();
	}
}
