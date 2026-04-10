@echo off
SETLOCAL EnableDelayedExpansion

REM ===== SETTINGS =====
SET packagingDirectory=product
SET developerToolsDirectory=devtools

REM ====================

IF NOT EXIST %~dp0..\%developerToolsDirectory% ( MKDIR %~dp0..\%developerToolsDirectory% )
IF NOT EXIST %~dp0..\%developerToolsDirectory%\MeteorBuild ( MKDIR %~dp0..\%developerToolsDirectory%\MeteorBuild )

SET sourcesFromCore=
FOR /R %~dp0..\engine\Core %%G IN (*.cpp) DO (
	SET sourcesFromCore=!sourcesFromCore! %%G
)


SET includes=-I%~dp0..\engine\Core
SET compiler=-g -Wvarargs
SET linker=-L%~dp0..\%packagingDirectory% -lMeteorEngine-Core.lib
SET defines=-DMR_DEBUG -DMR_PLATFORM_WINDOWS -D_CRT_SECURE_NO_WARNINGS 

REM echo %sources%

SET sources=
FOR /R %~dp0..\devtools\MeteorBuild %%G IN (*.cpp) DO (
	SET sources=!sources! %%G
)

echo Building Core...
clang++ %sourcesFromCore% -std=c++17 -o %~dp0..\%packagingDirectory%\MeteorEngine-Core.dll %defines% -DMR_CORE_EXPORTS %includes% %compiler% -shared
IF %errorlevel% == 0 (
    echo 
	echo Compiling done!
) ELSE (
    echo
	echo Something went wrong!
	pause 
	exit
)

echo echo Building MeteorBuild...
clang++ %sources% -std=c++17 -o %~dp0..\%packagingDirectory%\MeteorBuild.exe %defines% %compiler% %linker% %includes%

IF %errorlevel% == 0 (
    echo 
	echo Compiling done!
) ELSE (
    echo 
	echo Something went wrong!
	pause 
	exit
)

pause
exit