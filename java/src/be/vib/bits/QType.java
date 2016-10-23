package be.vib.bits;

public class QType extends QValue
{  
	public QType(QValue q)
	{
		super();
		ptr = newQType(q.ptr);
	}
	
	public QType(String typeName)
	{
		super();
		ptr = newQType(typeName);
	}
	
	private native static long newQType(long ptr);
	private native static long newQType(String typeName);
}