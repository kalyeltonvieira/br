# Senha

Módulo `senha` da biblioteca padrão BR.

## Escopo

Este módulo cobre geração, validação e força de senhas.

## Arquivos

- `senha.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `senha.bri`: contrato público.
- `senha.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/senha_teste.brt`: testes do módulo.

## API Inicial

- `gerar()`: Gera senha.
- `validar(valor)`: Valida senha.
- `forca(valor)`: Calcula força da senha.
- `hash(valor)`: Gera hash de senha.
