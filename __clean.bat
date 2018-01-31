@echo off
echo delete .\x64\
rd /s /q .\x64
echo delete .\src\x64\
rd /s /q .\src\x64
echo delete .\debug\
rd /s /q .\debug
echo delete .\src\debug\
rd /s /q .\src\debug
echo delete .\release\
rd /s /q .\release
echo delete .\src\release\
rd /s /q .\src\release
echo delete .\GeneratedFiles\
rd /s /q .\GeneratedFiles
echo delete .\src\GeneratedFiles\
rd /s /q .\src\GeneratedFiles

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
pause