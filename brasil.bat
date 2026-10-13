@echo off
if "%~1" == "compilar" (
    if "%~3" == "" (
        python "%~dp0ferramentas\compilador.py" compilar "%~2"
    ) else (
        python "%~dp0ferramentas\compilador.py" compilar "%~2" "%~3"
    )
) else if "%~1" == "testar" (
    python "%~dp0ferramentas\compilador.py" testar "%~2"
) else if "%~1" == "executar" (
    python "%~dp0ferramentas\maquina_virtual.py" %*
) else (
    echo CLI Oficial da Linguagem BR
    echo Uso:
    echo   brasil compilar ^<arquivo.br/brt/bri/bra^> [saida.brc]
    echo   brasil testar ^<arquivo.br/brt^>
    echo   brasil executar ^<arquivo.brc/brp^>
)
