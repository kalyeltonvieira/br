# Migração

Módulo `migracao` da biblioteca padrão BR.

## Escopo

Este módulo cobre migração de versões, APIs e projetos.

## Arquivos

- `migracao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `migracao.bri`: contrato público.
- `migracao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/migracao_teste.brt`: testes do módulo.

## API Inicial

- `migrar(projeto)`: Migra projeto.
- `versao(projeto)`: Consulta versão de migração.
- `api(nome_item)`: Migra API.
- `relatorio(projeto)`: Gera relatório de migração.
