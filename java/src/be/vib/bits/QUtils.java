package be.vib.bits;

public class QUtils
{
	public static QValue newCubeFromGrayscaleArray(int width, int height, byte[] pixels)
	{
		System.out.println("newCubeFromGrayscaleArray (Java thread=" + Thread.currentThread().getId() + ")");
		System.out.println("w=" + width + " h=" + height + " pixels=" + pixels);

		long p = newCubeFromGrayscaleArrayNative(width, height, pixels);
		
		System.out.println("got QValue ptr back from native code.");
		return new QValue(new QNativePointer(p));
	}
	public native static byte[] newGrayscaleArrayFromCube(int width, int height, QValue cube);	

	// Helpers:
	public native static long newCubeFromGrayscaleArrayNative(int width, int height, byte[] pixels);
}

