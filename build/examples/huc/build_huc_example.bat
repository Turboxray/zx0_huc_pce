rem ***************************************************************************

setlocal

cd /d "%~dp0"

pushd

del C_log.txt
del S_log.txt

set PCE_INCLUDE=%CD%\..\..\zx0;C:\huc\include\huc
set PATH=%PATH%;c:\huc\bin

huc.exe  huc_example.c -T > C_log.txt

type C_log.txt

pause