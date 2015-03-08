@echo off

set START=%time%
echo %time%
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
msbuild /t:rebuild /maxcpucount:8 /nologo /m vs2013\playallthegames.sln /p:Configuration=Release
taskkill /f /im msbuild.exe
echo %START% - %time%

:START


REM echo set DIR=%%CD%% > copygame.bat
REM echo cd "games\%%1\%%1\bin\x86\Release\" >> copygame.bat
REM echo ..\..\..\..\..\..\GamePlayable\bin\Release\GamePlayable.exe "%%1" >> copygame.bat
REM echo if %%ERRORLEVEL%%==0 GOTO YAY >> copygame.bat
REM echo cd %%DIR%% >> copygame.bat
REM echo GOTO END >> copygame.bat
REM echo :YAY >> copygame.bat
REM echo cd %%DIR%% >> copygame.bat
REM echo xcopy /y "games\%%1\%%1\bin\x86\Release\%%1.dll" patg\games >> copygame.bat
REM echo xcopy /s /y "games\%%1\%%1\bin\x86\Release\Content\*" patg\Content >> copygame.bat
REM echo :END >> copygame.bat

rmdir /q /s patg
mkdir patg
xcopy /s /y vs2013\release\PlayAllTheGames.exe patg\
REM xcopy /y %SystemRoot%\System32\vccorlib110.dll patg\
REM xcopy /y %SystemRoot%\System32\msvcp110.dll patg\
REM xcopy /y %SystemRoot%\System32\msvcr110.dll patg\
REM xcopy /y blib\externals\glew\bin\glew32.dll patg\

xcopy /s /y blib\assets\*.* patg\assets\
xcopy /s /y assets\*.* patg\assets\
mkdir patg\games

dir games /b > gamelist
for /F "tokens=*" %%A in (gamelist) do xcopy vs2013\Release\%%A.dll patg\games\
del gamelist




del PlayAllTheGames.rar
"c:\Program Files\WinRAR\Rar.exe" -r a PlayAllTheGames.rar patg\*
rmdir /q /s patg
xcopy /y PlayAllTheGames.rar w:\dump\
xcopy /y PlayAllTheGames.rar w:\playallthegames\
pause