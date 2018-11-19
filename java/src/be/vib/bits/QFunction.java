package be.vib.bits;

public class QFunction extends QValue
{  
	public QFunction(String signature)
	{
		super();
		ptr = newQFunction(signature);
	}
	
	// Note: the maximum supported number of args is 10.
	//       If you pass more arguments to apply() it will throw an IllegalArgumentException.
	//
	// Implementation note 1:
	//
	// It would be nice if the Quasar API supported calling functions with a pointer to an array of QValue's and a number of values.
	// I don't think we can leverage Quasar's current API of using templates with fixed size arrays to convert Java arrays of QValues to arguments
	// for a Quasar function.
	//
	// Implementation note 2:
	//
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
	
	// TODO: support Quasar functions that return > 1 value.
	// Java does not support returning tuples, but all return values happen to be
	// of the QValue wrapper type. So we can probably return a QValue[] instead:
	//
	//     public QValue[] applyn(QValue... args) { .... }
	
	private native static long newQFunction(String signature);
	
	private native long applyNative(QValue... args);	
}
