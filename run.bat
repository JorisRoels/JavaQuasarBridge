"e:\Program Files\Java\jdk1.8.0_92\bin\java.exe" -enableassertions -cp dist\JavaQuasarBridge-0.0.1.jar  -Djava.library.path=.\dist Test
@rem The java.library.path is only needed so java can find the dll for System.loadLibrary, since that is also located in the "dist" folder
@rem This can be removed once we add the dll to the jar, and extract it at runtime.
@echo note: bridge was run with assertions enabled



