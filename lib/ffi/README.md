# FFI

Módulo `ffi` da biblioteca padrão BR.

## Escopo

Este módulo cobre integração com C, C++, DLLs, Python, JavaScript e Shell.

## Arquivos

- `ffi.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `ffi.bri`: contrato público.
- `ffi.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/ffi_teste.brt`: testes do módulo.

## API Inicial

- `dll(caminho)`: Carrega DLL.
- `python(codigo)`: Executa Python.
- `javascript(codigo)`: Executa JavaScript.
- `shell(comando)`: Executa shell.
