# Navegador

Módulo para automação básica de navegador em BR.

## Instalação

```br
usar rede.navegador
```

## Exemplo de Uso

```br
usar rede.navegador

# Criar navegador
navegador é criar_navegador verdadeiro
iniciar navegador

# Abrir página
pagina é abrir_pagina navegador, "https://exemplo.com"

# Interagir com elementos
clicar_elemento pagina, "#botao"
digitar_texto pagina, "#campo", "texto"

# Obter dados
texto é obter_texto pagina, "#resultado"
```

## Funções Disponíveis

- `criar_navegador(cabeca)` - Cria navegador
- `iniciar(navegador)` - Inicia navegador
- `parar(navegador)` - Para navegador
- `abrir_pagina(navegador, url)` - Abre página
- `fechar_pagina(navegador, pagina)` - Fecha página
- `navegar(pagina, url)` - Navega para URL
- `voltar(pagina)` - Volta para página anterior
- `avancar(pagina)` - Avança para próxima página
- `recarregar(pagina)` - Recarrega página
- `clicar_elemento(pagina, seletor)` - Clica elemento
- `digitar_texto(pagina, seletor, texto)` - Digita texto
- `obter_texto(pagina, seletor)` - Obtém texto
- `obter_atributo(pagina, seletor, atributo)` - Obtém atributo
- `esperar_elemento(pagina, seletor)` - Espera elemento
- `executar_script(pagina, script)` - Executa script
- `tirar_screenshot(pagina, caminho)` - Tira screenshot
- `definir_tamanho(pagina, largura, altura)` - Define tamanho
