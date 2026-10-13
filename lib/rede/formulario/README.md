# Formulário

Módulo para validação e processamento de formulários em BR.

## Instalação

```br
usar rede.formulario
```

## Exemplo de Uso

```br
usar rede.formulario

# Criar formulário
formulario é criar registro
    nome: registro tipo: "texto"
    email: registro tipo: "texto"
    idade: registro tipo: "numero"
fim

# Configurar campos
definir_obrigatorio formulario, "nome", verdadeiro
definir_obrigatorio formulario, "email", verdadeiro
adicionar_validador formulario, "email", validar_email

# Validar dados
dados é registro
    nome: "João"
    email: "joao@exemplo.com"
    idade: 25
fim

valido é validar formulario, dados
se valido então
    mostre "Formulário válido"
senão
    mostre obter_erros formulario
fim
```

## Funções Disponíveis

- `criar(campos)` - Cria formulário
- `adicionar_campo(formulario, nome, tipo)` - Adiciona campo
- `definir_obrigatorio(formulario, campo, obrigatorio)` - Define obrigatório
- `adicionar_validador(formulario, campo, validador)` - Adiciona validador
- `validar(formulario, dados)` - Valida dados
- `obter_erros(formulario)` - Obtém erros
- `obter_dados(formulario)` - Obtém dados
- `limpar_dados(formulario)` - Limpa dados
- `definir_valor_padrao(formulario, campo, valor)` - Define valor padrão
- `validar_email(email)` - Valida email
- `validar_url(url)` - Valida URL
- `validar_numero(valor)` - Valida número
- `validar_tamanho(valor, minimo, maximo)` - Valida tamanho
- `sanitizar(dados)` - Sanitiza dados
