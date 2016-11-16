set MY_QUASAR=E:\Program Files\Quasar
set MY_JAVA=E:\Program Files\Java\jdk1.8.0_92

mkdir output\class

javac -d output\class ^
java\src\be\vib\bits\QExecutor.java ^
java\src\be\vib\bits\QTypeBuilder.java ^
java\src\be\vib\bits\QHost.java ^
java\src\be\vib\bits\QUtils.java ^
java\src\be\vib\bits\QValue.java ^
java\src\be\vib\bits\QFunction.java ^
java\src\be\vib\bits\QMethod.java ^
java\src\be\vib\bits\QType.java ^
java\src\be\vib\bits\QRange.java ^
java\src\be\vib\bits\QNativePointer.java ^
java\src\be\vib\bits\QException.java ^
java\src\Test.java ^
java\src\Example1.java

javah -cp java\src -d output\include be.vib.bits.QHost
javah -cp java\src -d output\include be.vib.bits.QValue
javah -cp java\src -d output\include be.vib.bits.QFunction
javah -cp java\src -d output\include be.vib.bits.QType
javah -cp java\src -d output\include be.vib.bits.QMethod
javah -cp java\src -d output\include be.vib.bits.QRange
javah -cp java\src -d output\include be.vib.bits.QTypeBuilder
javah -cp java\src -d output\include be.vib.bits.QUtils

rem /Zi enables debugging
rem /LD creates a DLL
cl /Zi /W3 /LD /EHsc /D_CRT_SECURE_NO_WARNINGS ^
cpp\WideString.cpp ^
cpp\QTypeBuilderJNI.cpp ^
cpp\Utils.cpp ^
cpp\QUtilsJNI.cpp ^
cpp\QHostJNI.cpp ^
cpp\QValueJNI.cpp ^
cpp\QFunctionJNI.cpp ^
cpp\QTypeJNI.cpp ^
cpp\QMethodJNI.cpp ^
cpp\QRangeJNI.cpp ^
"%MY_QUASAR%\include\quasar_dsl.cpp" "%MY_QUASAR%\include\quasar_host.cpp" /I"%MY_JAVA%\include\win32" /I"%MY_JAVA%\include" /Ioutput\include /I"%MY_QUASAR%\include" /Fooutput\ /Feoutput\JavaQuasarBridge.dll

rem Note: To obtain class descriptors for JNI, use e.g. "javap -cp output\class -s -p be.vib.bits.QValue"
