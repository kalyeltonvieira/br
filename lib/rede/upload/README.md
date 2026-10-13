# Upload

Módulo para envio e recebimento de arquivos em BR.

## Instalação

```br
usar rede.upload
```

## Exemplo de Uso

```br
usar rede.upload

# Enviar arquivo
resultado é enviar "https://exemplo.com/upload", arquivo, registro
    max_tamanho: 10485760
    tipos: lista "jpg", "png"
fim

# Receber arquivo
dados é receber arquivo, registro
    diretorio: "/uploads"
fim

# Salvar arquivo
salvar dados, "/caminho/arquivo.jpg"
```

## Funções Disponíveis

- `enviar(url, arquivo, opcoes)` - Envia arquivo
- `receber(arquivo, opcoes)` - Recebe arquivo
- `salvar(arquivo, caminho)` - Salva arquivo
- `validar(arquivo, tipos_permitidos)` - Valida tipo
- `validar_tamanho(arquivo, maximo)` - Valida tamanho
- `definir_max_tamanho(upload, maximo)` - Define max tamanho
- `definir_tipos_permitidos(upload, tipos)` - Define tipos permitidos
- `definir_diretorio(upload, diretorio)` - Define diretório
- `definir_nome_personalizado(upload, funcao)` - Define nome personalizado
- `criar_progresso(upload)` - Cria progresso
- `atualizar_progresso(progresso, enviado)` - Atualiza progresso
- `cancelar(upload)` - Cancela upload
- `pausar(upload)` - Pausa upload
- `retomar(upload)` - Retoma upload
- `obter_info(arquivo)` - Obtém informações
