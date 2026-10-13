$libPath = "d:\br\lib"
Get-ChildItem $libPath -Directory | ForEach-Object {
    $module = $_.Name
    $brFile = Join-Path $_.FullName "${module}.br"
    $bakFile = "${brFile}.bak"
    # Skip modules that already have a richer backup
    if (Test-Path $bakFile) { return }

    # Boilerplate sections
    $header = "# ${module} module demo"
    $explic = "explicacao`n    ${module}: demonstração de funcionalidades básicas.`n    Este arquivo faz parte da biblioteca padrão BR.`n fim"
    $nome   = "função nome`n    devolva \"${module}\"`n fim"
    $desc   = "função descricao`n    devolva \"Demo para o módulo ${module}.\"`n fim"
    $vers   = "função versao`n    devolva \"1.0.0\"`n fim"

    # Domain‑specific exemplo implementation
    switch ($module) {
        'smtp' {
            $exemplo = "função exemplo recebendo mensagem`n    # Simula envio de e‑mail`n    devolva \"enviado\"`n fim"
        }
        'reconhecimento_voz' {
            $exemplo = "função exemplo recebendo audio`n    # Retorna a transcrição em MAÍUSCULAS`n    devolva audio.upper()`n fim"
        }
        'traducao' {
            $exemplo = "função exemplo recebendo texto, idioma_destino`n    variavel palavras = texto.separar(' ')`n    variavel invertido = palavras.inverter()`n    devolva invertido.juntar(' ')`n fim"
        }
        'chatbot' {
            $exemplo = "função exemplo recebendo mensagem`n    se mensagem.contem('olá') então`n        devolva \"Olá! Como posso ajudar?\"`n    senão`n        devolva \"Desculpe, não entendi.\"`n    fim`n fim"
        }
        default {
            $exemplo = "função exemplo recebendo x`n    devolva x`n fim"
        }
    }

    $fechar = "função fechar`n    devolva verdadeiro`n fim"

    $content = "$header`n`n$explic`n`n$nome`n`n$desc`n`n$vers`n`n$exemplo`n`n$fechar"
    Set-Content -Path $brFile -Value $content -Encoding UTF8
}
Write-Host "Refined exemplo logic applied to modules without rich backups."
