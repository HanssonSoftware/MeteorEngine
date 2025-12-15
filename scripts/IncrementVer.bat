@echo off
setlocal enabledelayedexpansion

set HEADER_FILE=%~dp0\..\Engine\Version.h
set TEMP_FILE=%~dp0\..\Engine\Version.tmp

for /f "tokens=2 delims==." %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set YEAR=%datetime:~0,4%
set MONTH=%datetime:~4,2%
set DAY=%datetime:~6,2%
set HOUR=%datetime:~8,2%
set MINUTE=%datetime:~10,2%

set BUILD_NUMBER=0
if exist %HEADER_FILE% (
    for /f "tokens=3" %%A in ('findstr /C:"#define BUILD_NUMBER" %HEADER_FILE%') do set /A BUILD_NUMBER=%%A+1
)

(for /f "delims=" %%A in ('findstr /V "#define BUILD_NUMBER" %HEADER_FILE%') do echo %%A) > %TEMP_FILE%
echo #define BUILD_NUMBER %BUILD_NUMBER% >> %TEMP_FILE%
echo #define BUILD_YEAR %YEAR% >> %TEMP_FILE%
echo #define BUILD_MONTH %MONTH% >> %TEMP_FILE%
echo #define BUILD_DATE L"%MONTH%/%YEAR%" >> %TEMP_FILE%
echo #define BUILD_DAY %DAY% >> %TEMP_FILE%

move /Y %TEMP_FILE% %HEADER_FILE% > nul

echo Version updated: %BUILD_NUMBER% (%YEAR%-%MONTH%-%DAY% %HOUR%:%MINUTE%)
endlocal
