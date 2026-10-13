# Sistema operacional

Módulo `sistema` da biblioteca padrão BR.

## Escopo

Este módulo cobre argumentos, variáveis de ambiente, processos e comandos.

## Arquivos

- `sistema.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `sistema.bri`: contrato público.
- `sistema.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/sistema_teste.brt`: testes do módulo.

## API Inicial

- `plataforma()`: Nome da plataforma padrão da instalação atual.
- `argumentos()`: Representação textual dos argumentos do processo.
- `ambiente(chave)`: Busca uma variável de ambiente.
- `executar(comando)`: Executa um comando do sistema e retorna o código de saída.
- `executar_detalhado(comando)`: Executa um comando e retorna `codigo`, `saida` e `erro`.
