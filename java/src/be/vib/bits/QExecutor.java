package be.vib.bits;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

// An Executor singleton that uses a single thread.
// It is used by the JavaQuasarBridge to meet the Quasar requirement
// that all Quasar interaction happens from within the same thread.
public class QExecutor {

	final private static ExecutorService executor = Executors.newSingleThreadExecutor();

	private QExecutor()
	{
		// Private constructor to forbid instantiation
	}

	public static ExecutorService getInstance()
	{
		return executor;
	}

}