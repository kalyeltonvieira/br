@echo off
if exist "%~dp0bin\br.exe" (
    "%~dp0bin\br.exe" %*
    exit /b %ERRORLEVEL%
)
if exist "%~dp0dist\br.exe" (
    "%~dp0dist\br.exe" %*
    exit /b %ERRORLEVEL%
)
python "%~dp0br.py" %*
