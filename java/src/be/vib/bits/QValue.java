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
	
	public native void delete(); // TODO: does this need to be synchronized?

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
	
	public QValue at(int i) // access element i in a 1D array; indexing is 0-based
	{
		long p = atNative(i);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(int i, int j) // access element (i, j) in a 2D array; indexing is 0-based
	{
		long p = atNative(i, j);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(int i, int j, int k) // access element (i, j, k) in a 3D array; indexing is 0-based
	{
		long p = atNative(i, j, k);
		return new QValue(new QNativePointer(p));
	}
	
	public QValue at(QValue q1, QValue q2, QValue q3)  // TODO - this is an experiment, can be used for 3D array access using QValues (such as ranges) as indices - useful for slicing cubes
	{
		long p = atNativeQ(q1.ptr, q2.ptr, q3.ptr);
		return new QValue(new QNativePointer(p));
	}
	
	private native long atNative(int i);
	
	private native long atNative(int i, int j);
	
	private native long atNative(int i, int j, int k);

	private native long atNativeQ(long ptr1, long ptr2, long ptr3);
	
	private native long getFieldNative(String fieldName);
	
	public native void setField(String fieldName, QValue value);
	
	private static native long readhostVariableNative(String varName);

	@Override
	protected void finalize() throws Throwable
	{
		// FIXME: there are several possible pitfalls here:
		//        - no guarantee finalize() gets called (?)
		//        - called in a separate finalizer thread instead of the Quasar thread
		//        - may be called concurrently
		//        - possibly called after Quasar host is already released
		//        - ...
		// Instead of relying on a finalizer, we're considering to
		// implement our own QValue registry that automatically keeps
		// track of created and deleted QValue objects. The user is then
		// responsible for manually calling a cleanup function
		// before program termination that will free all
		// QValue objects that have not been manually deleted yet. Additionally
		// the user can still manually delete individual QValue objects earlier on,
		// which makes sense especially if the QValue object wraps a large array.
	    try
	    {
	    	// Queue the delete on the Quasar thread.
	    	System.out.println("QValue.finalize() for ptr = " + ptr);
			QExecutor.getInstance().execute(() -> { delete(); });
	    }
	    finally
	    {
	        super.finalize();
	    }
	}
	
	public native int size(int dim); // note: dim = 0 is the first dimension
		
	private static native void initIDs();
	
	static
	{
		// Calculate and cache field and method IDs for JNI.
		// TODO: check that this indeed gets called if we only use a derived class of QValue, such as QFunction.
		initIDs();
	}
}
