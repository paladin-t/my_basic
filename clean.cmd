@echo off
echo Cleaning, a moment please...

attrib *.suo -s -r -h
del /f /s /q *.suo
del /f /s /q *.user
del /f /s /q *.ncb

rd /s /q temp

del /f /s /q output\*.res
del /f /s /q output\*.dep
del /f /s /q output\*.htm
del /f /s /q output\*.ilk
del /f /s /q output\*.manifest
del /f /s /q output\*.obj
del /f /s /q output\*.pdb
del /f /s /q output\*.idb
del /f /s /q output\my_basic_d.exe

echo Cleaning done!
