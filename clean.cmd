@echo off
echo Cleaning, a moment please...

rd /s /q .vs
rd /s /q temp

attrib *.suo -s -r -h

del /f /s /q *.exp
del /f /s /q *.log
del /f /s /q *.ncb
del /f /s /q *.obj
del /f /s /q *.pdb
del /f /s /q *.sdf
del /f /s /q *.suo
del /f /s /q *.user
del /f /s /q *.vc.db

del /f /s /q output\*.dep
del /f /s /q output\*.htm
del /f /s /q output\*.idb
del /f /s /q output\*.ilk
del /f /s /q output\*.iobj
del /f /s /q output\*.ipdb
del /f /s /q output\*.manifest
del /f /s /q output\*.res

echo Cleaning done!
