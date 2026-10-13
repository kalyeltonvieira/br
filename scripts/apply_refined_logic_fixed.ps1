$libPath = "d:\br\lib"
Get-ChildItem $libPath -Directory | ForEach-Object {
    $module = $_.Name
    $brFile = Join-Path $_.FullName "$module.br"
    $backupFile = "$brFile.bak"
    # If a richer backup exists, keep it untouched
    if (Test-Path $backupFile) { return }
    # Header
    $header = "# $module module demo"
    # Explanation (no variable expansion inside string)
    $explic = "explicacao`n    $module: demonstração de funcionalidades básicas.`n    Este arquivo faz parte da biblioteca padrão BR.`nfim"
    $nome = "função nome`n    devolva \"$module\"`nfim"
    $desc = "função descricao`n    devolva \"Demo para o módulo $module.\"`nfim"
    $vers = "função versao`n    devolva \"1.0.0\"`nfim"
    # Domain‑specific exemplo
    switch ($module) {
        'smtp' {
            $exemplo = "função exemplo recebendo mensagem`n    # Simula envio de email`n    devolva \"enviado\"`nfim"
        }
        'reconhecimento_voz' {
            $exemplo = "função exemplo recebendo audio`n    # Retorna transcrição em maiúsculas`n    devolva audio.upper()`nfim"
        }
        'traducao' {
            $exemplo = "função exemplo recebendo texto, idioma_destino`n    # Mock translation: inverte as palavras`n    variavel palavras = texto.separar(' ')`n    variavel invertido = palavras.inverter()`n    devolva invertido.juntar(' ')`nfim"
        }
        'chatbot' {
            $exemplo = "função exemplo recebendo mensagem`n    se mensagem.contem('olá') então`n        devolva \"Olá! Como posso ajudar?\"`n    senão`n        devolva \"Desculpe, não entendi.\"`n    fim`nfim"
        }
        default {
            $exemplo = "função exemplo recebendo x`n    devolva x`nfim"
        }
    }
    $fechar = "função fechar`n    devolva verdadeiro`nfim"
    $content = "$header`n`n$explic`n`n$nome`n`n$desc`n`n$vers`n`n$exemplo`n`n$fechar"
    Set-Content -Path $brFile -Value $content -Encoding UTF8
}
Write-Host "Refined exemplo logic applied to modules without rich backups."
