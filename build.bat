set MY_QUASAR=E:\Program Files\Quasar
set MY_JAVA=E:\Program Files\Java\jdk1.8.0_92
set VERSION=0.0.1

rmdir /s /q build
mkdir build\class
mkdir build\libraries\win64

javac -d build\class ^
java\src\be\vib\bits\JavaQuasarBridge.java ^
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
java\src\be\vib\bits\jartools\Jar.java ^
java\src\Test.java ^
java\src\Example1.java

@if %errorlevel% neq 0 exit /b %errorlevel%

javah -cp java\src -d build\include be.vib.bits.QHost
javah -cp java\src -d build\include be.vib.bits.QValue
javah -cp java\src -d build\include be.vib.bits.QFunction
javah -cp java\src -d build\include be.vib.bits.QType
javah -cp java\src -d build\include be.vib.bits.QMethod
javah -cp java\src -d build\include be.vib.bits.QRange
javah -cp java\src -d build\include be.vib.bits.QTypeBuilder
javah -cp java\src -d build\include be.vib.bits.QUtils

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
"%MY_QUASAR%\include\quasar_dsl.cpp" "%MY_QUASAR%\include\quasar_host.cpp" /I"%MY_JAVA%\include\win32" /I"%MY_JAVA%\include" /Ibuild\include /I"%MY_QUASAR%\include" /Fobuild\ /Febuild\JavaQuasarBridge.dll

@if %errorlevel% neq 0 exit /b %errorlevel%

rem Note: To obtain class descriptors for JNI, use e.g. "javap -cp build\class -s -p be.vib.bits.QValue"

copy build\JavaQuasarBridge.dll build\libraries\win64
@if %errorlevel% neq 0 exit /b %errorlevel%

rmdir /s /q dist
mkdir dist
jar cvf dist\JavaQuasarBridge-%VERSION%.jar -C build\class . -C build libraries
