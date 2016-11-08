rmdir /s dist
mkdir dist
copy output\JavaQuasarBridge.dll dist
jar cvf dist\JavaQuasarBridge.jar -C output\class .
