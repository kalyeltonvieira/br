$libPath = "d:\br\lib"
Get-ChildItem $libPath -Directory | ForEach-Object {
    $module = $_.Name
    $modulePath = $_.FullName
    # api.br
    $apiPath = Join-Path $modulePath "api.br"
    $apiContent = "# API for $module`n`nfunção nome`nfunção descricao`nfunção versao`nfunção exemplo`nfunção fechar"
    Set-Content -Path $apiPath -Value $apiContent -Encoding UTF8
    # README.md
    $readmePath = Join-Path $modulePath "README.md"
    $readmeContent = "# $module`n`n$module module for the BR standard library. Provides a simple demo implementation.\n\n## Functions\n- `nome()` – returns module name\n- `exemplo(x)` – demo function\n- `fechar()` – returns true\n\n## Example\n```br`nimportar $module`nmostre $module.exemplo('teste')`n```"
    Set-Content -Path $readmePath -Value $readmeContent -Encoding UTF8
    # exemplos/basico.br
    $exDir = Join-Path $modulePath "exemplos"
    if (-not (Test-Path $exDir)) { New-Item -ItemType Directory -Path $exDir | Out-Null }
    $basicPath = Join-Path $exDir "basico.br"
    $basicContent = "importar $module`nmostre $module.exemplo('demo')"
    Set-Content -Path $basicPath -Value $basicContent -Encoding UTF8
    # testes/<module>_teste.brt
    $testDir = Join-Path $modulePath "testes"
    if (-not (Test-Path $testDir)) { New-Item -ItemType Directory -Path $testDir | Out-Null }
    $testPath = Join-Path $testDir "${module}_teste.brt"
    $testContent = "# Test for $module`nimportar $module`nassert $module.nome() == \"${module}\"`nassert $module.exemplo('x') != null"
    Set-Content -Path $testPath -Value $testContent -Encoding UTF8
}
Write-Host "Artifacts generated for all modules."
