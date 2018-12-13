package be.vib.bits;

public class QTypeBuilder extends QValue
{
	public QTypeBuilder(String moduleName, String typeName)
	{
		super();
		ptr = newQTypeBuilder(moduleName, typeName);
	}
	
	public native void addField(String fieldName, QType fieldType);
	
	private native static long newQTypeBuilder(String moduleName, String typeName);
	
}
