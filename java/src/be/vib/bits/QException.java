package be.vib.bits;

// The QException class represents a C++ quasar::exception_t.
public class QException extends RuntimeException
{
	public QException(String message)
	{
		super(message);
	}
}
