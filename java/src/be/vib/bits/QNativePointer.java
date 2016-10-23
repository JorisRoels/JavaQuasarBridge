package be.vib.bits;

// The QNativePtr type is used to distinguish a regular long integer
// from a long that represents a native C++ pointer.
// (We use it to overload functions that can take either a long integer
// or a C++ pointer stored in a long integer but need to respond differently.)
public class QNativePointer
{
	private long ptr;
	
	public QNativePointer(long ptr)
	{
		this.ptr = ptr;
	}
	
	public long getPointer()
	{
		return ptr;
	}
}