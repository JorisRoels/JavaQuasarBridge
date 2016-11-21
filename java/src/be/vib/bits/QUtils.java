package be.vib.bits;

public class QUtils
{
	public static QValue newCubeFromGrayscaleArray(int width, int height, byte[] pixels)
	{
		long p = newCubeFromGrayscaleArrayNative(width, height, pixels);
		return new QValue(new QNativePointer(p));
	}
	public native static byte[] newGrayscaleArrayFromCube(int width, int height, QValue cube);	

	// Helpers:
	public native static long newCubeFromGrayscaleArrayNative(int width, int height, byte[] pixels);
}

