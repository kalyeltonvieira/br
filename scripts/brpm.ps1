# brpm – simple package manager for BR modules
# Usage: \n#   powershell -File d:\br\scripts\brpm.ps1 install <moduleName>\n#   powershell -File d:\br\scripts\brpm.ps1 uninstall <moduleName>\n
param(
    [Parameter(Mandatory=$true)][ValidateSet('install','uninstall')]
    [string]$action,
    [Parameter(Mandatory=$true)][string]$module
)

$lib   = "D:\br\lib"
$store = "D:\br\packages"

switch ($action) {
    'install' {
        $src = Join-Path $store $module
        $dst = Join-Path $lib $module
        if (-not (Test-Path $src)) {
            Write-Error "Package $module not found in $store"
            exit 1
        }
        Copy-Item -Path $src -Destination $dst -Recurse -Force
        Write-Host "Installed $module"
    }
    'uninstall' {
        $target = Join-Path $lib $module
        if (-not (Test-Path $target)) {
            Write-Error "Module $module not present in $lib"
            exit 1
        }
        Remove-Item -Path $target -Recurse -Force
        Write-Host "Uninstalled $module"
    }
}
