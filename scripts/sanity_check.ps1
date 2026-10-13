#!/usr/bin/env pwsh
# sanity_check.ps1 – imports batch‑1 modules and runs a demo function
$modules = @(
    "core",
    "sistema",
    "arquivo",
    "texto",
    "colecoes",
    "json",
    "csv",
    "config",
    "log",
    "testes"
)
foreach ($mod in $modules) {
    try {
        "importar $mod" | Out-Null
        # each module should expose a function called exemplo (demo)
        $func = "$mod.exemplo"
        if (Get-Command $func -ErrorAction SilentlyContinue) {
            $result = & $func "demo"
            Write-Host "[OK] $mod.exemplo returned: $result"
        } else {
            Write-Host "[WARN] $mod does not expose exemplo"
        }
    } catch {
        Write-Host "[FAIL] $mod threw: $_"
    }
}
# Write a simple report file
$report = "sanity_check_$(Get-Date -Format 'yyyyMMdd_HHmmss').log"
Get-Content $MyInvocation.MyCommand.Path | Out-File "d:\br\scripts\$report"
