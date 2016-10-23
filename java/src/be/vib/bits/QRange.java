package be.vib.bits;

public class QRange extends QValue
{  
	public QRange(int minVal, int maxVal)
	{
		this(minVal, maxVal, 1);
	}
	
	public QRange(int minVal, int maxVal, int step)
	{
		super();
		ptr = newQRange(minVal, maxVal, step);
	}
	
	public QRange(float minVal, float maxVal)
	{
		this(minVal, maxVal, 1.0f);
	}
	
	public QRange(float minVal, float maxVal, float step)
	{
		super();
		ptr = newQRange(minVal, maxVal, step);
	}
	
	private native static long newQRange(int minVal, int maxVal, int step);
	private native static long newQRange(float minVal, float maxVal, float step);
}