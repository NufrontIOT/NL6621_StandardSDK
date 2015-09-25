C:\Keil\ARM\BIN40\fromelf.exe --bin -o OUTPUT\SdkFw.bin OUTPUT\OBJ\SdkFw.axf
C:\Keil\ARM\BIN40\fromelf.exe -c -s -o OUTPUT\SdkFw.lst OUTPUT\OBJ\SdkFw.axf
copy OUTPUT\LIST\SdkFw.map OUTPUT\SdkFw.map /B/Y

copy ..\..\Tool\GenBootBins.exe OUTPUT\GenBootBins.exe
cd OUTPUT
GenBootBins.exe SdkFw.bin
del GenBootBins.exe


