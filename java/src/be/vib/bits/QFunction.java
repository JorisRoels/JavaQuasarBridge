package be.vib.bits;

public class QFunction extends QValue
{  
	public QFunction(String signature)
	{
		super();
		ptr = newQFunction(signature);
	}
	
	// Note: the maximum supported number of args is 8.
	// If you pass more arguments to apply() it will throw an IllegalArgumentException. 
	//
	// TODO?
	// We could add some syntactic sugar and define an
	//    apply(Object...)
	// instead of
	//    apply(QValue...)
	// so we can use this syntax:
	//    f.apply(1, 2.0f, "abc");
	// instead of
	//    f.apply(new QValue(1), new QValue(2.0f), new QValue("abc"))
	//
	// It would make the QFunction JNI code more complex though. Also a bit slower?
	//
	public QValue apply(QValue... args) throws java.lang.IllegalArgumentException
	{
		long resultPtr = applyNative(args);
		return new QValue(new QNativePointer(resultPtr));
	}
	
	private native static long newQFunction(String signature);
	
	private native long applyNative(QValue... args);	
}
