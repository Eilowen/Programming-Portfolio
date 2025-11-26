@echo off
echo === Build & Run: System Resource Monitor ===

REM --- Explicit project folder ---
set "PROJECT=C:\Users\ADMIN\OneDrive - FAST National University\Desktop\PF project final"

echo Changing to project folder:
cd /d "%PROJECT%"
if errorlevel 1 (
  echo ERROR: Could not change directory to "%PROJECT%"
  echo Please make sure the folder exists and you have permissions.
  pause
  exit /b 1
)
echo Now in: %cd%
echo.

REM --- Path to GCC (Dev-C++) ---
set "GCC=C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\gcc.exe"

REM --- Raylib include & lib folders (explicit) ---
set "RAYLIB_INC=C:\Users\ADMIN\Downloads\raylib-5.5_win64_mingw-w64\raylib-5.5_win64_mingw-w64\include"
set "RAYLIB_LIB=C:\Users\ADMIN\Downloads\raylib-5.5_win64_mingw-w64\raylib-5.5_win64_mingw-w64\lib"

echo Using GCC: %GCC%
echo Raylib include: %RAYLIB_INC%
echo Raylib lib:     %RAYLIB_LIB%
echo.

REM --- Basic sanity checks ---
if not exist "%GCC%" (
    echo ERROR: gcc not found at "%GCC%".
    echo Edit this script and set the correct path to gcc.exe.
    pause
    exit /b 1
)
if not exist "%RAYLIB_INC%\raylib.h" (
    echo ERROR: raylib.h not found in "%RAYLIB_INC%".
    pause
    exit /b 1
)
if not exist "%RAYLIB_LIB%\libraylibdll.a" (
    echo ERROR: libraylibdll.a not found in "%RAYLIB_LIB%".
    pause
    exit /b 1
)

REM --- Compile ---
echo Compiling...
"%GCC%" -std=c99 -O2 -o monitor.exe main_raylib.c win_sysinfo.c -I"%RAYLIB_INC%" "%RAYLIB_LIB%\libraylibdll.a" -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
    echo.
    echo Build failed. See errors above.
    pause
    exit /b 1
)

REM --- Copy raylib.dll (overwrite quietly if present) ---
if exist "%RAYLIB_LIB%\raylib.dll" (
    copy /Y "%RAYLIB_LIB%\raylib.dll" "%cd%\" >nul
) else (
    echo WARNING: raylib.dll not found in "%RAYLIB_LIB%". Program may fail if DLL is required.
)

echo.
echo Build succeeded. Running monitor.exe...
echo.

REM --- Run program ---
monitor.exe

pause
