@echo off

mkdir buildWindows
cd buildWindows

SET MINGW_BIN=K:\Qt\Qt_551\Tools\mingw492_32\bin
set QT_BIN=K:\Qt\Qt_551\5.5\mingw492_32\bin

SET CC=%MINGW_BIN%\gcc.exe
SET CXX=%MINGW_BIN%\g++.exe
SET QMAKE=%QT_BIN%\qmake.exe
SET MAKE=%MINGW_BIN%\mingw32-make.exe
set PATH=%PATH%;%MINGW_BIN%

@echo %QMAKE%
@echo on
%QMAKE% CONFIG+=release ../sultan.pro
%MAKE% -j24
@echo off
