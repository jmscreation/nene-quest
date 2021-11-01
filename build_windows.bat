@echo off
REM		Build Script

REM Set Compiler Settings Here

cls

set CPP=c++
set GPP=g++
set OUTPUT=nenequest\nenequest.exe
set DEBUGMODE=1

set LINK_ONLY=0
set VERBOSE=0
set CLEAN_BUILD=1

set ASYNC_BUILD=1

set SOURCE_DIRECTORY=.\nenequest\src

set COMPILER_FLAGS=-std=c++20 -DSFML_STATIC
set ADDITIONAL_LIBRARIES=-static-libstdc++ -lsfml-graphics-s -lsfml-audio-s -lsfml-window-s -lsfml-system-s -lsfml-network-s -lsetupapi -lopenal32 -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg -lwinmm -lopengl32 -lgdi32 -lfreetype -lws2_32
set ADDITIONAL_LIBDIRS=-LSFML\lib64
set ADDITIONAL_INCLUDEDIRS=-Inenequest\headers -ISFML\include

del %OUTPUT% 2>nul

setlocal enabledelayedexpansion


if %LINK_ONLY% GTR 0 (
	goto linker
)

if %DEBUGMODE% GTR 0 (
	set DEBUG_INFO=-ggdb -g
) else (
	set DEBUG_INFO=-s
)

if %ASYNC_BUILD% GTR 0 (
	set WAIT=
) else (
	set WAIT=/WAIT
)

if %CLEAN_BUILD% GTR 0 (
	del /S /Q ".objs64\*.o" 2>nul
)

if not exist .objs64 (
	echo Creating Object Directory Structure...
	mkdir .objs64
)

echo Building API Files...
set /a n=0
for /R %SOURCE_DIRECTORY% %%F in (*.cpp) do (
	if not exist .objs64\%%~nF!n!.o (
		echo Building %%~nF!n!.o
		start /B %WAIT% "%%~nF!n!.o" %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o .objs64\%%~nF!n!.o

		if %VERBOSE% GTR 0 (
			echo %CPP% %ADDITIONAL_INCLUDEDIRS% %COMPILER_FLAGS% %DEBUG_INFO% -c %%F -o .objs64\%%~nF.o
		)
	)
	set /a n+=1
)

REM Wait for building process to finish
:loop
for /f %%G in ('tasklist ^| find /c "%CPP%"') do ( set count=%%G )
if %count%==0 (
	goto linker
) else (
	timeout /t 2 /nobreak>nul
	goto loop
)

:linker

set "files="
for /f "delims=" %%A in ('dir /b /a-d ".objs64\*.o" ') do set "files=!files! .objs64\%%A"

:link
echo Linking Executable...

if %DEBUGMODE% GTR 0 (
	set MWINDOWS=
) else (
	set MWINDOWS=-mwindows
)

if %VERBOSE% GTR 0 (
	echo %GPP% %ADDITIONAL_LIBDIRS% -o %OUTPUT% %files% %ADDITIONAL_LIBRARIES% %MWINDOWS%
)

%GPP% %ADDITIONAL_LIBDIRS% -o %OUTPUT% %files% %ADDITIONAL_LIBRARIES% %MWINDOWS%

:finish
if exist .\%OUTPUT% (
	echo Build Success!
) else (
	echo Build Failed!
)