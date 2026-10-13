param(
    [string]$BrExe = "D:\br\bin\br.exe"
)

$profilePath = [string]$PROFILE
$profileDir = Split-Path -Parent $profilePath
$start = "# >>> BR language command hook >>>"
$end = "# <<< BR language command hook <<<"
$escapedBrExe = $BrExe.Replace("'", "''")

New-Item -ItemType Directory -Path $profileDir -Force | Out-Null

$snippet = @'
# >>> BR language command hook >>>
# Allows local BR files to run in PowerShell by typing exemplo.br.
# Limited to BR language extensions.
if (-not (Get-Variable -Name BR_PreviousCommandNotFoundAction -Scope Global -ErrorAction SilentlyContinue)) {
    $global:BR_PreviousCommandNotFoundAction = $ExecutionContext.InvokeCommand.CommandNotFoundAction
}

$global:BR_ExecutablePath = '__BR_EXE__'
$ExecutionContext.InvokeCommand.CommandNotFoundAction = {
    param($commandName, $eventArgs)

    $brExtensions = @('.br', '.brc', '.brp', '.brt', '.bri', '.bra', '.brm', '.brd')
    $requested = $eventArgs.CommandName
    $ext = [IO.Path]::GetExtension($requested).ToLowerInvariant()

    if ($brExtensions -contains $ext) {
        if ([IO.Path]::IsPathRooted($requested)) {
            $candidate = $requested
        } else {
            $candidate = Join-Path (Get-Location) $requested
        }

        if ((Test-Path -LiteralPath $candidate -PathType Leaf) -and
            (Test-Path -LiteralPath $global:BR_ExecutablePath -PathType Leaf)) {
            $resolved = (Resolve-Path -LiteralPath $candidate).ProviderPath.Replace("'", "''")
            $br = $global:BR_ExecutablePath.Replace("'", "''")
            $eventArgs.CommandScriptBlock = [scriptblock]::Create("& '$br' '$resolved' @args")
            $eventArgs.StopSearch = $true
            return
        }
    }

    if ($global:BR_PreviousCommandNotFoundAction) {
        & $global:BR_PreviousCommandNotFoundAction $commandName $eventArgs
    }
}
# <<< BR language command hook <<<
'@
$snippet = $snippet.Replace("__BR_EXE__", $escapedBrExe)

$content = ""
if (Test-Path -LiteralPath $profilePath -PathType Leaf) {
    $content = Get-Content -LiteralPath $profilePath -Raw
}

$pattern = "(?s)\r?\n?# >>> BR language command hook >>>.*?# <<< BR language command hook <<<\r?\n?"
$content = [regex]::Replace($content, $pattern, "").TrimEnd()

if ($content) {
    $content = $content + "`r`n`r`n" + $snippet + "`r`n"
} else {
    $content = $snippet + "`r`n"
}

Set-Content -LiteralPath $profilePath -Value $content -Encoding UTF8
Write-Output "BR PowerShell hook instalado em $profilePath"
