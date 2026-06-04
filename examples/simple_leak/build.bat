@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
set "INCLUDE_DIR=%SCRIPT_DIR%..\..\include"
set "BUILD_DIR=%SCRIPT_DIR%build"
set "SRC=%SCRIPT_DIR%main.cpp"
set "OUT=%BUILD_DIR%\simple_leak.exe"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

call :try_gpp
if %ERRORLEVEL%==0 goto :run

call :try_cl_path
if %ERRORLEVEL%==0 goto :run

call :try_vswhere
if %ERRORLEVEL%==0 goto :run

echo.
echo ERROR: No C++ compiler found.
echo   - Install Visual Studio with "Desktop development with C++", or
echo   - Install MinGW-w64 / MSYS2 and add g++ to PATH, or
echo   - Install CMake: https://cmake.org/download/
echo.
echo You can also build from JetBrains Rider: open CMakeLists.txt and use the CMake tool window.
exit /b 1

:try_gpp
where g++ >nul 2>&1
if errorlevel 1 exit /b 1
echo [build] Using g++
g++ -std=c++17 -I"%INCLUDE_DIR%" "%INCLUDE_DIR%\leak_detector.cpp" "%SRC%" -o "%OUT%"
exit /b %ERRORLEVEL%

:try_cl_path
where cl >nul 2>&1
if errorlevel 1 exit /b 1
echo [build] Using cl (PATH)
pushd "%BUILD_DIR%"
    cl /nologo /EHsc /std:c++17 /I"%INCLUDE_DIR%" "%INCLUDE_DIR%\leak_detector.cpp" "%SRC%" /Fe:simple_leak.exe
set "ERR=%ERRORLEVEL%"
popd
exit /b %ERR%

:try_vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" exit /b 1

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL exit /b 1

set "VCVARS=%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" exit /b 1

echo [build] Using MSVC from: %VSINSTALL%
call "%VCVARS%" >nul
if errorlevel 1 exit /b 1

pushd "%BUILD_DIR%"
    cl /nologo /EHsc /std:c++17 /I"%INCLUDE_DIR%" "%INCLUDE_DIR%\leak_detector.cpp" "%SRC%" /Fe:simple_leak.exe
set "ERR=%ERRORLEVEL%"
popd
exit /b %ERR%

:run
if not exist "%OUT%" (
    echo ERROR: Build failed - executable not found.
    exit /b 1
)
echo.
echo Build OK: %OUT%
echo.
"%OUT%"
exit /b 0
