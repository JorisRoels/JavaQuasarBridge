package be.vib.bits;

public class Example1 {

	public static void main(String[] args)
	{
		boolean loadCompiler = true;
		QHost.init("cuda", loadCompiler);
				
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
		
		QHost.loadModuleFromSource("example", program);
		
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
		
		// Handle events on the imshow image window.
		// Blocks until the users closes all open imshow windows.
		QHost.runApp();

		QHost.release();
	}

}
