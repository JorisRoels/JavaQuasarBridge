package be.vib.bits;

import be.vib.bits.QHost;
import be.vib.bits.QValue;
import be.vib.bits.QFunction;

import java.io.IOException;

public class Test {
	
	static
	{		
		System.out.println("About to load JavaQuasarBridge dynamic library");
		System.loadLibrary("JavaQuasarBridge"); // loads JavaQuasarBridge.dll (on Windows)
		System.out.println("JavaQuasarBridge loaded.");
	}

	public static void main(String[] args)
	{
		boolean loadCompiler = true;
		QHost.init("cuda", loadCompiler);
				
		testLoadSource();
		
		testLoadSourceFromString();
		
		testFunctionWithTooManyArgs();
		
		testSimpleQValues();
		
		testArrayQValue();
		
		testFunction1();

		testFunction2();
		
		testGaussian();
		
//		testRange1();
		
//		testUserStruct();  // FIXME
		
//		testTypeBuilder();

		// TODO: test QValue.readHostVariable("foo");

		QHost.release();
	}
	
	private static void testSimpleQValues()
	{
		final int answer = 42;
		final float pi = 3.14f;
		final String haiku = "furu ike ya / kawazu tobikomu / mizu no oto";
		
		QValue qv = new QValue();
		QValue qi = new QValue(answer);
		QValue qf = new QValue(pi);
		QValue qs = new QValue(haiku);
		
		assert(qi.getInt() == answer);   // CHECKME: fails with newest (>22 oct but < 7 nov 2016) Quasar lib, but works with the 22 oct version
		assert(qf.getFloat() == pi);
//		assert(qs.getString() == haiku);
		
		qv.delete();
		qi.delete();
		qf.delete();
		qs.delete();
	}
	
	private static void testArrayQValue()
	{
		float[] a = new float[10000];
		for (int i = 0; i < a.length; i++)
			a[i] = i;
		
		System.out.println("a[5000]=" + a[5000]);
		
		QValue q = new QValue(a);
	
		assert(q.size(0) == 1);
		assert(q.size(1) == 10000);
		assert(q.size(2) == 1);
		
		q.delete();
	}

	private static void testLoadSource()
	{
		// TODO: avoid dependency on files external to this project
		QHost.loadSourceModule("E:\\Program Files\\Quasar\\Library\\dwt.q");
		
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
		
		System.out.println("myprint exists? " + (exists ? "yes" : "no"));
		
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

		// FIXME? what about the unused void return values of tic() and toc()? Check if and how they get deleted.
		tic.apply();
		QValue image = imread.apply(new QValue("lena_big.tif"));  // FIXME: check if/how the argument to 'apply' gets deleted
		toc.apply(); // prints the time taken to read the image
		
		assert(image.size(0) == 512);
		assert(image.size(1) == 512);
		assert(image.size(2) == 3);

		System.out.println("Java: dimensions of result from imread: " + image.size(0) + " x " + image.size(1) + " x " + image.size(2));
		
		imshow.apply(image);
		
		// Handle user interaction with the image window.
		// (Blocks until all image windows are closed.)
		QHost.runApp();
				
		// NOT SO GOOD
		//image.delete(); // TODO: if we do not do the delete, Quasar release will warn us that there still is an image on the GPU (QValue.finalize is not called because the JVM did not create the QValue object, we created it on the C++ side.)
		
		// FIXME: delete it all - delete functions?
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
		System.out.println(one.getFloat());
	}
	
	private static void testFunctionWithTooManyArgs()
	{
		QFunction f = new QFunction("print(...)");
		
		boolean exceptionThrown = false;
		try
		{
			// QFunction.apply() does not support more than 8 arguments and should throw an exception.
			f.apply(new QValue(1), new QValue(2), new QValue(3), new QValue(4),
					new QValue(5), new QValue(6), new QValue(7), new QValue(8),
					new QValue(9));
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
		QValue imageOut = filter.apply(imageIn, new QValue(filterCoeff), new QValue(2));

		QValue p1 = imshow.apply(imageIn);
		QValue p2 = imshow.apply(imageOut);

		// We now "connect" the two imshow windows.
		// (Connecting them has the effect that zooming and panning
		// in one image window will do the same zoom/pan in the other.)
		QType t1 = new QType(p1);
		QMethod connect = new QMethod(t1, "connect(??)");
		connect.apply(p1, p2);  // Call the function p1.connect(p2)
		
		QHost.runApp();
	}
	
	private static void testTypeBuilder()
	{
		/*
		// Define the type so that it can be used from Quasar. Note that using this
		// mechanism, objects of the type are subject to automatic memory management
		// e.g. transfer between CPU and GPU etc. The objects can also be inspected
		// in the debugger, serialized to file, ...
		TypeBuilder builder(L"sample7", L"point");
		builder.AddField(L"x", Type(L"int"));
		builder.AddField(L"y", Type(L"int"));
		Type type = builder.CreateType<point>();

		// Create a typed object.
		QValue obj2 = host->CreateTypedObject(type->private_obj);
		// The "slow" way of setting properties
		obj2.SetField(L"x", 4.5);
		obj2.SetField(L"y", 7.5);
		*/
		
		QTypeBuilder builder = new QTypeBuilder("sample", "point");
		builder.addField("x", new QType("int"));
		builder.addField("y", new QType("int"));

	}
	
	private static void testUserStruct()
	{
		QFunction object = new QFunction("object()");  // TODO: inform/check with Bart - it looks like this doesn't work or is not implemented yet
		QFunction print = new QFunction("print(...)");
		
		boolean exists = QHost.functionExists("object");
		System.out.println("object function exists? " + (exists ? "yes" : "no"));
		assert(exists);

		System.out.println("calling object()");
		
		// Create an untyped (Python-like) object, that has properties that can be
		// assigned at run-time.
		QValue obj = object.apply();
//		obj.setField("x", new QValue(2.5f));
//		print.apply(obj);

		// Define the type so that it can be used from Quasar. Note that using this
		// mechanism, objects of the type are subject to automatic memory management
		// e.g. transfer between CPU and GPU etc. The objects can also be inspected
		// in the debugger, serialized to file, ...
//		TypeBuilder builder(L"sample7", L"point");
//		builder.AddField(L"x", Type(L"int"));
//		builder.AddField(L"y", Type(L"int"));
//		Type type = builder.CreateType<point>();
		
	}
	
	private static void testRange1()
	{
		QFunction print = new QFunction("print(...)");
		QFunction maximum = new QFunction("max(??)");

		/*
		 * Examples from the Quasar REPL
		 * :: 1..10..1
		 * [ 1]
		 * :: 1..1..10
		 * [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
		 * :: 1..3..10
		 * [ 1, 4, 7, 10]
		 */
		QRange range1 = new QRange(1, 10, 1);
		print.apply(range1);  // [ 1]
		assert(maximum.apply(range1).getInt() == 1);
		assert(range1.size(1) == 1);
		
		QRange range2 = new QRange(1, 1, 10);
		print.apply(range2);  // [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
		assert(maximum.apply(range2).getInt() == 10);
		assert(range2.size(1) == 10);
		
		QRange range3 = new QRange(1, 3, 10);
		print.apply(range3); //  [ 1, 4, 7, 10]
		assert(maximum.apply(range3).getInt() == 10);
		assert(range3.size(1) == 4);

		QRange range4 = new QRange(1, 10, 3);
		print.apply(range4); //  [ 1]
		assert(maximum.apply(range4).getInt() == 1);
		assert(range4.size(1) == 1);

		// TODO: log C++ API bug: QRange(1,10,1) says it is minval,maxval,step but gets interpreted as minval,step,maxval (due to default argument step being last...)

		// A float range
		QRange range5 = new QRange(1, 0.5f, 10);
		print.apply(range5); // [ 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5, 10]
		assert(maximum.apply(range5).getInt() == 10.0f);
		assert(range5.size(1) == 19);
	}
	
//	private static void testRange2()
//	{
//		
//		QFunction imread = new QFunction("imread(string)");
//		QFunction imshow = new QFunction("imshow(cube)");
//		
//		assert(QHost.functionExists("imread"));
//		assert(QHost.functionExists("imshow"));
//
//		QValue image = imread.apply(new QValue("lena_big.tif"));
//		
//		assert(image.size(0) == 512);
//		assert(image.size(1) == 512);
//		assert(image.size(2) == 3);
//
//		System.out.println("Original dimensions: " + image.size(0) + " x " + image.size(1) + " x " + image.size(2));
//		
//		// img = img[:,:,1]
//		
//		// TODO: this does not work yet!
//		image = image(new QRange(), new QRange(), new QValue(1)); // TODO: verify/test that memory handling is correct here
//		
//		System.out.println("New dimensions: " + image.size(0) + " x " + image.size(1) + " x " + image.size(2));
//
//		imshow.apply(image);
//		
//		// Handle user interaction with the image window.
//		// (Blocks until all image windows are closed.)
//		QHost.runApp();
//	}
}
