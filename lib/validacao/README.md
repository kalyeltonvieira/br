# Validação

Módulo `validacao` da biblioteca padrão BR.

## Escopo

Este módulo cobre validar CPF, CNPJ, email, telefone, CEP e cartão.

## Arquivos

- `validacao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `validacao.bri`: contrato público.
- `validacao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/validacao_teste.brt`: testes do módulo.

## API Inicial

- `cpf(valor)`: Valida CPF.
- `cnpj(valor)`: Valida CNPJ.
- `email(valor)`: Valida email.
- `cep(valor)`: Valida CEP.
