# gerar_documentacao.ps1 – creates api.br files from inline comments
$libPath = "D:\br\lib"
Get-ChildItem $libPath -Directory | ForEach-Object {
    $modulePath = $_.FullName
    $moduleName = $_.Name
    $brFile = Join-Path $modulePath "${moduleName}.br"
    if (-not (Test-Path $brFile)) { return }
    $lines = Get-Content $brFile
    # Extract comment lines that start with '# '
    $docLines = $lines | Where-Object { $_ -match '^#' }
    $apiPath = Join-Path $modulePath "api.br"
    $apiHeader = "# API extracted from ${moduleName}.br"
    $apiContent = $apiHeader + "`n`n" + ($docLines -join "`n")
    Set-Content -Path $apiPath -Value $apiContent -Encoding UTF8
    Write-Host "Generated api.br for $moduleName"
}
Write-Host "Documentation generation completed."
