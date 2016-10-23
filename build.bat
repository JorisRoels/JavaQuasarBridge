mkdir output\class

javac -d output\class java\src\be\vib\bits\Example1.java java\src\be\vib\bits\QHost.java java\src\be\vib\bits\QValue.java java\src\be\vib\bits\QFunction.java java\src\be\vib\bits\QMethod.java java\src\be\vib\bits\QType.java java\src\be\vib\bits\QRange.java java\src\be\vib\bits\QNativePointer.java

javah -cp java\src -d output\include be.vib.bits.QHost
javah -cp java\src -d output\include be.vib.bits.QValue
javah -cp java\src -d output\include be.vib.bits.QFunction
javah -cp java\src -d output\include be.vib.bits.QType
javah -cp java\src -d output\include be.vib.bits.QMethod
javah -cp java\src -d output\include be.vib.bits.QRange

rem /Zi enables debugging
rem /LD creates a DLL
cl /Zi /W3 /LD /EHsc /D_CRT_SECURE_NO_WARNINGS cpp\WideString.cpp cpp\Utils.cpp cpp\QHostJNI.cpp cpp\QValueJNI.cpp cpp\QFunctionJNI.cpp cpp\QTypeJNI.cpp cpp\QMethodJNI.cpp cpp\QRangeJNI.cpp   "e:\Program Files\Quasar\include\quasar_dsl.cpp" "e:\Program Files\Quasar\include\quasar_host.cpp" /I"E:\Program Files\Java\jdk1.8.0_92\include\win32" /I"e:\Program Files\Java\jdk1.8.0_92\include" /Ioutput\include /I"e:\Program Files\Quasar\include" /Fooutput\ /Feoutput\JavaQuasarBridge.dll

rem To obtain class descriptors for JNI, use e.g. "javap -cp output\class -s -p be.vib.bits.QValue"