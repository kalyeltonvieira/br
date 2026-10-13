# Build

Módulo `build` da biblioteca padrão BR.

## Escopo

Este módulo cobre sistema de build e empacotamento.

## Arquivos

- `build.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `build.bri`: contrato público.
- `build.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/build_teste.brt`: testes do módulo.

## API Inicial

- `construir(projeto)`: Executa build do projeto.
- `limpar(projeto)`: Limpa artefatos de build.
- `empacotar(projeto)`: Empacota projeto.
- `artefato(nome_item)`: Define artefato de saída.
