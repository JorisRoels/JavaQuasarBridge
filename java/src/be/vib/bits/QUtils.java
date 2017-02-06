package be.vib.bits;

public class QUtils
{
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
	
	public native static byte[] newGrayscaleByteArrayFromCube(int width, int height, QValue cube);	// assumes cube values are in [0, 255]
	public native static short[] newGrayscaleShortArrayFromCube(int width, int height, QValue cube);	// assumes cube values are in [0, 65535]

	// Helpers:
	public native static long newCubeFromGrayscaleByteArrayNative(int width, int height, byte[] pixels);
	public native static long newCubeFromGrayscaleShortArrayNative(int width, int height, short[] pixels);
}

