echo set building enviroment
set VS_PATH=c:\Program Files\Microsoft Visual Studio 8
call "%VS_PATH%\VC\vcvarsall.bat"

echo clean
devenv gtest-1.6.0\msvc\gtest.sln /clean "Debug|Win32"
devenv gtest-1.6.0\msvc\gtest.sln /clean "Release|Win32"

echo compile
devenv gtest-1.6.0\msvc\gtest.sln /rebuild "Debug|Win32"
devenv gtest-1.6.0\msvc\gtest.sln /rebuild "Release|Win32"

xcopy gtest-1.6.0\msvc\gtest\Debug\gtestd.lib ..\lib\gtestd.lib
xcopy gtest-1.6.0\msvc\gtest\Debug\gtest_maind.lib ..\lib\gtest_maind.lib
xcopy gtest-1.6.0\msvc\gtest\Release\gtest.lib ..\lib\gtest.lib
xcopy gtest-1.6.0\msvc\gtest\Release\gtest_main.lib ..\lib\gtest_main.lib