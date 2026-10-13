$mods = @('machine_learning','deep_learning','visao_computacional','recomendacao','classificacao','clusterizacao')
foreach ($m in $mods) {
    $src = "d:\br\lib\$m\$m.br.bak"
    $dst = "d:\br\lib\$m\$m.br"
    if (Test-Path $src) {
        Copy-Item -Path $src -Destination $dst -Force
    }
}
Write-Host "Restored selected modules."
