package be.vib.bits;

public class QValue
{
	protected long ptr; // pointer to the C++ QValue object

	public QValue()
	{
		ptr = newQValue();
	}
	
	public QValue(int i)
	{
		ptr = newQValue(i);
	}
	
	public QValue(float f)
	{
		ptr = newQValue(f);
	}
	
	public QValue(String s)
	{
		ptr = newQValue(s);
	}
	
	public QValue(int[] a)
	{
		ptr = newQValue(a);
	}
	
	public QValue(float[] a)
	{
		ptr = newQValue(a);
	}
	
	protected QValue(QNativePointer p) 
	{
		// QValue receives ownership over the native pointer that QNativePointer wraps,
		// so QValue becomes responsible for delete()ing it.
		ptr = p.getPointer();
	}
	
	private static native long newQValue();

	private static native long newQValue(int i);

	private static native long newQValue(float f);

	private static native long newQValue(int[] a);
	
	private static native long newQValue(float[] a);
	// TODO: higher dimensional arrays

	private static native long newQValue(String s);
	
	public native int getInt();	
	
	public native float getFloat();
	
	public native String getString();
	
	public native void dispose(); // TODO: does this need to be synchronized?
	
	public native void retain();

	public static QValue readhostVariable(String varName)
	{
		long p = readhostVariableNative(varName);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue getField(String fieldName)
	{
		long p = getFieldNative(fieldName);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(int m) // access element m in a 1D array; indexing is 0-based
	{
		long p = atNative(m);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(int m, int n) // access element (m, n) in a 2D array; indexing is 0-based
	{
		long p = atNative(m, n);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(int m, int n, int k) // access element (m, n, k) in a 3D array; indexing is 0-based
	{
		long p = atNative(m, n, k);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(QValue m)  // access element with index m in a 1D array; the index m is a QValue (for example a range)
	{
		long p = atNativeQ(m.ptr);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(QValue m, QValue n)   // access element with indices (m, n) in a 2D array; the indices are QValues (for example ranges)
	{
		long p = atNativeQ(m.ptr, n.ptr);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(QValue m, QValue n, QValue k)   // access element with indices (m, n, k) in a 3D array; the indices are QValues (for example ranges)
	{
		long p = atNativeQ(m.ptr, n.ptr, k.ptr);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue size()
	{
		long p = sizeNative();
		return new QValue(new QNativePointer(p));		
	}
	
	// Shorthand for size().at(dim)
	public native int size(int dim); // note: dim = 0 is the first dimension
	
	private native long sizeNative();
	
	private native long atNative(int m);
	
	private native long atNative(int m, int n);
	
	private native long atNative(int m, int n, int k);

	private native long atNativeQ(long ptr1);

	private native long atNativeQ(long ptr1, long ptr2);

	private native long atNativeQ(long ptr1, long ptr2, long ptr3);
	
	private native long getFieldNative(String fieldName);
	
	public native void setField(String fieldName, QValue value);
	
	private static native long readhostVariableNative(String varName);

	@Override
	protected void finalize() throws Throwable
	{
		// Note: there are several potential pitfalls here:
		//       - no guarantee finalize() gets called (?)
		//       - called in a separate finalizer thread instead of the Quasar thread
		//       - may be called concurrently
		//       - possibly called after Quasar host is already released
		//       - ...
		// Instead of relying on a finalizer, we're considering to
		// implement our own QValue registry that automatically keeps
		// track of created and deleted QValue objects. The user is then
		// responsible for manually calling a cleanup function
		// before program termination that will free all
		// QValue objects that have not been manually deleted yet. Additionally
		// the user can still manually delete individual QValue objects earlier on,
		// which makes sense especially if the QValue object wraps a large array.
		// This registry approach would avoid the fragility of a finalize(),
		// but has the disadvantage of not automatically freeing QValues
		// that the user never created a reference for, such as rvalues passed directly
		// to a function. The user cannot free them (it's an rvalue) and this
		// registry cannot either (it doesn't know about the lifetime of the QValue)
		// until the user calls the cleanup() function.
		
		// FIXME:
		// In an actual program heaviliy using the JavaQuasarBridge we've seen runtime errors
		// where Quasar complains about usage of a DISPOSED matrix! The error disappears
		// when the finalizer code below is commented out...
		// (Not sure if this is caused by a silly bug, or a design flaw.)
//	    try
//	    {
//	    	// Queue the delete on the Quasar thread.
//	    	System.out.println("QValue.finalize() for ptr = " + ptr);
//			QExecutor.getInstance().execute(() -> { delete(); });
//	    }
//	    finally
	    {
	        super.finalize();
	    }
	}
	
	private static native void initIDs();
	
	static
	{
		// Calculate and cache field and method IDs for JNI.
		// TODO: check that this indeed gets called if we only use a derived class of QValue, such as QFunction.
		initIDs();
	}
}
