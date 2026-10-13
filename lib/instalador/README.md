# Instalador

Módulo `instalador` da biblioteca padrão BR.

## Escopo

Este módulo cobre criação de instaladores para projetos .br.

## Arquivos

- `instalador.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `instalador.bri`: contrato público.
- `instalador.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/instalador_teste.brt`: testes do módulo.

## API Inicial

- `criar(projeto)`: Cria instalador.
- `windows(projeto)`: Cria instalador Windows.
- `linux(projeto)`: Cria instalador Linux.
- `manifesto(projeto)`: Gera manifesto de instalação.
