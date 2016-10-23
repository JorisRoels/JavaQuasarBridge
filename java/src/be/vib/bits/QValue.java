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
	
	public QValue(float[] a)
	{
		ptr = newQValue(a);
	}
	
	protected QValue(QNativePointer p) 
	{
		// QValue receives ownership over the native pointer that QNativePointer wraps,
		// so QValue becomes responsible for delete()ing it.
		ptr = p.getPointer();
		System.out.println("   Made QValue from native ptr: " + ptr);
	}
	
	private static native long newQValue();

	private static native long newQValue(int i);

	private static native long newQValue(float f);

	private static native long newQValue(float[] a); // TODO: higher dimensional arrays

	private static native long newQValue(String s);
	
	public native int getInt();	
	
	public native float getFloat();
	
	public native void delete(); // TODO: does this need to be synchronized?

	public static QValue readhostVariable(String varName)
	{
		long p = readhostVariableNative(varName);
		return new QValue(new QNativePointer(p));
	}

	private static native long readhostVariableNative(String varName);

	@Override
	protected void finalize() throws Throwable
	{
		// TODO: there are several issues here:
		//       no guarantee it gets called, possibly called in wrong thread, called concurrently, maybe called after Quasar host is already released,...
	    try
	    {
			delete();
	    }
	    finally
	    {
	        super.finalize();
	    }
	}
	
	public native int size(int dim);
		
	private static native void initIDs();
	
	static
	{
		// Calculate and cache field and method IDs for JNI.
		// TODO: check that this indeed gets called if we only use a derived class of QValue, such as QFunction.
		initIDs();
	}
}
