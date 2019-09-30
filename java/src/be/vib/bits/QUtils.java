package be.vib.bits;

public class QUtils
{
	// Multiply all elements in the cube with the given scalar s. 
	public native static void inplaceMultiply(QValue cube, float s);

	// Divide all elements in the cube by the given scalar s. 
	public native static void inplaceDivide(QValue cube, float s);
	
	// Add scalar s to every element in the cube. 
	public native static void inplaceAdd(QValue cube, float s);
	
	// Subtract scalar s from every element in the cube. 
	public native static void inplaceSubtract(QValue cube, float s);
	
	// q = min(q, a)
	public native static void inplaceMin(QValue q, QValue a);
	
	// q = max(q, a)
	public native static void inplaceMax(QValue q, QValue a);
	
	// Clamp q to the range [lo, hi]:
	// q = max(min(q, hi), lo)
	// The value q can be an array, for example.
	public static void inplaceClamp(QValue q, QValue lo, QValue hi)
	{
		inplaceMin(q, hi);
		inplaceMax(q, lo);
	}
	
	public static QValue newCubeFromGrayscaleByteArray(int width, int height, byte[] pixels)
	{
		long p = newCubeFromGrayscaleByteArrayNative(width, height, pixels);
		return new QValue(new QNativePointer(p));
	}

	public static QValue newCubeFromGrayscaleShortArray(int width, int height, short[] pixels)
	{
		long p = newCubeFromGrayscaleShortArrayNative(width, height, pixels);
		return new QValue(new QNativePointer(p));
	}
	
	public native static byte[] newGrayscaleByteArrayFromCube(int width, int height, QValue cube);	// cube values will be clipped to [0, 255] before storing them into the byte[]
	public native static short[] newGrayscaleShortArrayFromCube(int width, int height, QValue cube);	// cube values will be clipped to [0, 65535] before storing them into the short[]

	// Helpers:
	public native static long newCubeFromGrayscaleByteArrayNative(int width, int height, byte[] pixels);
	public native static long newCubeFromGrayscaleShortArrayNative(int width, int height, short[] pixels);
}

