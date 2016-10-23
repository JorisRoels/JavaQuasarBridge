package be.vib.bits;

public class QMethod extends QValue
{  
	public QMethod(QType type, String signature)
	{
		super();
		ptr = newQMethod(type.ptr, signature);
	}
	
	// Note: the maximum supported number of args is 3.
	// If you pass more arguments to apply() it will throw an IllegalArgumentException. 
	public QValue apply(QValue target, QValue... args)
	{
		long resultPtr = applyNative(target, args);
		return new QValue(new QNativePointer(resultPtr));
	}
	
	private native static long newQMethod(long typePtr, String signature);

	private native long applyNative(QValue target, QValue... args);
}