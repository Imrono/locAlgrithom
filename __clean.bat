@echo off
echo delete .\x64\
rd /s /q .\x64
echo delete .\debug\
rd /s /q .\debug
echo delete .\release\
rd /s /q .\release
echo delete .\GeneratedFiles\
rd /s /q .\GeneratedFiles

set locAlgrithomBuild=build-locAlgrithom
echo delete .\build-locAlgrithom ...
for /f "delims=" %%i in ('dir %locAlgrithomBuild%* /ad/b') do rd "%%i" /s/q

set testBuild=build-test
echo delete .\build-test ...
for /f "delims=" %%i in ('dir %testBuild%* /ad/b') do rd "%%i" /s/q

echo clean finished
pause