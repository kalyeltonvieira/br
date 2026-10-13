$libPath = "d:\br\lib"
Get-ChildItem $libPath -Directory | ForEach-Object {
    $module = $_.Name
    $brFile = Join-Path $_.FullName "$module.br"
    if (Test-Path $brFile) {
        Copy-Item $brFile "$brFile.bak" -Force
    }
    $content = @'
# $module module demo

explicacao
    $module: demonstração de funcionalidades básicas.
    Este arquivo faz parte da biblioteca padrão BR.
fim

função nome
    devolva "$module"
fim

função descricao
    devolva "Demo para o módulo $module."
fim

função versao
    devolva "1.0.0"
fim

função exemplo recebendo x
    devolva x
fim

função fechar
    devolva verdadeiro
fim
'@
    Set-Content -Path $brFile -Value $content -Encoding UTF8
}
Write-Host "Demo logic applied to all modules."
