@echo off
echo delete .\bin\
rd /s /q .\bin

echo delete .\x64\
rd /s /q .\x64
echo delete .\src\x64\
rd /s /q .\src\x64
echo delete .\test\x64\
rd /s /q .\test\x64

echo delete .\Win32\
rd /s /q .\Win32
echo delete .\src\Win32\
rd /s /q .\src\Win32
echo delete .\test\Win32\
rd /s /q .\test\Win32

echo delete .\debug\
rd /s /q .\debug
echo delete .\src\debug\
rd /s /q .\src\debug
echo delete .\test\debug\
rd /s /q .\test\debug
echo delete .\release\
rd /s /q .\release
echo delete .\src\release\
rd /s /q .\src\release
echo delete .\test\release\
rd /s /q .\test\release
echo delete .\GeneratedFiles\
rd /s /q .\GeneratedFiles
echo delete .\src\GeneratedFiles\
rd /s /q .\src\GeneratedFiles
echo delete .\test\GeneratedFiles\
rd /s /q .\test\GeneratedFiles

set locAlgrithomBuild=build-locAlgrithom
echo delete .\build-locAlgrithom ...
for /f "delims=" %%i in ('dir %locAlgrithomBuild%* /ad/b') do rd "%%i" /s/q

set testBuild=build-test
echo delete .\build-test ...
for /f "delims=" %%i in ('dir %testBuild%* /ad/b') do rd "%%i" /s/q

set mainProjBuild=build-mainProj
echo delete .\build-mainProj ...
for /f "delims=" %%i in ('dir %mainProjBuild%* /ad/b') do rd "%%i" /s/q


echo clean finished
#pause