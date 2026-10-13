@echo off
REM Instalador da Linguagem BR para Windows
REM Este script instala o BR e configura o sistema

echo Instalando Linguagem BR para Windows...

set "INSTALL_ROOT=%~dp0"
if "%INSTALL_ROOT:~-1%"=="\" set "INSTALL_ROOT=%INSTALL_ROOT:~0,-1%"
set "INSTALL_DIR=%INSTALL_ROOT%\bin"

REM Criar diretorios da instalacao local
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if not exist "%INSTALL_ROOT%\logs" mkdir "%INSTALL_ROOT%\logs"
if not exist "%INSTALL_ROOT%\temp" mkdir "%INSTALL_ROOT%\temp"
if not exist "%INSTALL_ROOT%\cache" mkdir "%INSTALL_ROOT%\cache"
if not exist "%INSTALL_ROOT%\modulos" mkdir "%INSTALL_ROOT%\modulos"
if not exist "%INSTALL_ROOT%\lib" mkdir "%INSTALL_ROOT%\lib"
if not exist "%INSTALL_ROOT%\projetos" mkdir "%INSTALL_ROOT%\projetos"

REM Copiar executável br.exe
if exist "%~dp0dist\br.exe" (
    copy /Y "%~dp0dist\br.exe" "%INSTALL_DIR%\br.exe"
) else (
    echo Erro: "%~dp0dist\br.exe" nao encontrado. Execute build_exe.py antes de instalar.
    exit /b 1
)

REM Adicionar ao PATH do usuario sem sobrescrever ou truncar o valor existente
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$dir='%INSTALL_DIR%';" ^
  "$path=[Environment]::GetEnvironmentVariable('Path','User');" ^
  "$parts=$path -split ';' | Where-Object { $_ };" ^
  "if ($parts -notcontains $dir) { [Environment]::SetEnvironmentVariable('Path', (($parts + $dir) -join ';'), 'User') }"

REM Registrar associacoes de arquivos BR para Windows 10/11
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$exe='%INSTALL_DIR%\br.exe';" ^
  "$cmd='C:\Windows\System32\cmd.exe /k \"\"' + $exe + '\" \"%1\" %*\"';" ^
  "$classes='HKCU:\Software\Classes';" ^
  "$exts=@('.br','.brc','.brp','.brt','.bri','.bra','.brm','.brd');" ^
  "New-Item -Path ($classes + '\BRLangFile\shell\open\command') -Force | Out-Null;" ^
  "Set-Item -Path ($classes + '\BRLangFile') -Value 'Arquivo BR';" ^
  "New-Item -Path ($classes + '\BRLangFile\DefaultIcon') -Force | Out-Null;" ^
  "Set-Item -Path ($classes + '\BRLangFile\DefaultIcon') -Value ('\"' + $exe + '\",0');" ^
  "Set-Item -Path ($classes + '\BRLangFile\shell\open\command') -Value $cmd;" ^
  "foreach ($ext in $exts) { New-Item -Path ($classes + '\' + $ext + '\OpenWithProgids') -Force | Out-Null; Set-Item -Path ($classes + '\' + $ext) -Value 'BRLangFile'; New-ItemProperty -Path ($classes + '\' + $ext + '\OpenWithProgids') -Name 'BRLangFile' -PropertyType None -Force | Out-Null }"

REM Adicionar extensoes BR ao PATHEXT do usuario sem duplicar
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$pathext=[Environment]::GetEnvironmentVariable('PATHEXT','User');" ^
  "if (-not $pathext) { $pathext=[Environment]::GetEnvironmentVariable('PATHEXT','Machine') };" ^
  "$parts=$pathext -split ';' | Where-Object { $_ };" ^
  "foreach ($ext in '.BR','.BRC','.BRP','.BRT','.BRI','.BRA','.BRM','.BRD') { if ($parts -notcontains $ext) { $parts += $ext } };" ^
  "[Environment]::SetEnvironmentVariable('PATHEXT', ($parts -join ';'), 'User')"

REM Permitir executar arquivo.br no PowerShell digitando apenas arquivo.br
if exist "%INSTALL_ROOT%\scripts\install_powershell_hook.ps1" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%INSTALL_ROOT%\scripts\install_powershell_hook.ps1" -BrExe "%INSTALL_DIR%\br.exe"
)

REM Notificar Explorer/terminal sobre mudancas de ambiente e associacao
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "Add-Type -Namespace Win32 -Name NativeMethods -MemberDefinition '[DllImport(\"user32.dll\", SetLastError=true, CharSet=CharSet.Auto)] public static extern IntPtr SendMessageTimeout(IntPtr hWnd, uint Msg, UIntPtr wParam, string lParam, uint fuFlags, uint uTimeout, out UIntPtr lpdwResult);';" ^
  "$result=[UIntPtr]::Zero;" ^
  "[Win32.NativeMethods]::SendMessageTimeout([IntPtr]0xffff,0x1A,[UIntPtr]::Zero,'Environment',2,5000,[ref]$result) | Out-Null"

echo.
echo Instalação concluída com sucesso!
echo.
echo Abra um novo terminal para usar o comando 'br'.
echo Executavel instalado em "%INSTALL_DIR%\br.exe".
echo.
pause
