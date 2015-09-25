C:\Keil\ARM\BIN40\fromelf.exe --bin -o OUTPUT OUTPUT\OBJ\SdkFw.axf
C:\Keil\ARM\BIN40\fromelf.exe -c -s -o OUTPUT\SdkFw.lst OUTPUT\OBJ\SdkFw.axf
copy OUTPUT\LIST\SdkFw.map OUTPUT\SdkFw.map /B/Y
del cmpl.fed
copy ..\..\Tool\fblink.exe .\fblink.exe
fblink.exe
del fblink.exe
del link.fed
copy ..\..\Tool\GenBootBins.exe OUTPUT\GenBootBins.exe
cd OUTPUT
GenBootBins.exe fwCodeSRAM
del GenBootBins.exe
