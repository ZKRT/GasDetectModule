@echo off
md ..\Release

echo turn executable files from axf to bin by tool names fromelf in filefordler of D:\Keil_v5\ARM\ARMCC\bin\ .
D:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin -o ..\OBJ\template.bin ..\OBJ\template.axf

echo copy release files to release folder.
copy ..\OBJ\template.bin ..\Release\GasDetectModule-V01-00-170330.bin
copy ..\OBJ\template.hex ..\Release\GasDetectModule-V01-00-170330.hex
