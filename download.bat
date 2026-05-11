@echo off
cd /d %USERPROFILE%\Downloads
bitsadmin /transfer myDownload /download /priority normal "https://github.com/mstorsjo/llvm-mingw/releases/download/20260505/llvm-mingw-20260505-msvcrt-x86_64.zip" llvm-mingw.zip