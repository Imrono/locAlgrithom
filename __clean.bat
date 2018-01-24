@echo off
echo delete .\x64\
rd /s /q .\x64
echo delete .\debug\
rd /s /q .\debug
echo delete .\release\
rd /s /q .\release
echo delete .\GeneratedFiles\
rd /s /q .\GeneratedFiles
echo delete .\build-locAlgrithom-Desktop_Qt_5_9_1_MSVC2015_64bit-Debug\
rd /s /q .\build-locAlgrithom-Desktop_Qt_5_9_1_MSVC2015_64bit-Debug
echo delete .\build-locAlgrithom-Desktop_Qt_5_9_1_MSVC2015_64bit-Release\
rd /s /q .\build-locAlgrithom-Desktop_Qt_5_9_1_MSVC2015_64bit-Release
echo delete .\build-test*
rd /s /q .\build-test*
echo clean finished
pause