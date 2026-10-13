# Permissão

Módulo `permissao` da biblioteca padrão BR.

## Escopo

Este módulo cobre controle de permissões e acesso.

## Arquivos

- `permissao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `permissao.bri`: contrato público.
- `permissao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/permissao_teste.brt`: testes do módulo.

## API Inicial

- `permitir(acao)`: Permite ação.
- `negar(acao)`: Nega ação.
- `verificar(acao)`: Verifica permissão.
- `papel(nome_item)`: Define papel de acesso.
