# 📚 Bibliotecas Padrão da Linguagem BR
**Versão**: 2.0.0  
**Total**: 13 bibliotecas

---

## 📦 Bibliotecas Disponíveis

### 1. **matematica.br** - Matemática
Funções matemáticas avançadas:
- Operações básicas (somar, subtrair, multiplicar, dividir)
- Potência e raiz
- Trigonometria (seno, cosseno, tangente)
- Estatística (media, mediana, minimo, maximo)
- Números aleatórios
- Utilitários (fatorial, fibonacci, eh_primo, mdc, mmc)

**Exemplo:**
```br
importar "matematica"
resultado é somar(10, 20)
mostre resultado  # 30
```

### 2. **arquivo.br** - Arquivos
Manipulação de arquivos e diretórios:
- Leitura (ler, ler_linhas, ler_csv, ler_json)
- Escrita (escrever, escrever_linhas, adicionar)
- Informações (existe, tamanho, extensao, nome)
- Diretórios (listar, criar_diretorio, remover, renomear)
- Utilitários (copiar, mover, caminho_absoluto)

**Exemplo:**
```br
importar "arquivo"
conteudo é ler("dados.txt")
escrever("saida.txt", conteudo)
```

### 3. **texto.br** - Texto
Manipulação de strings:
- Operações básicas (maiusculo, minusculo, capitalizar, inverter)
- Busca e substituição (contem, comeca_com, termina_com, buscar, substituir)
- Divisão e junção (dividir, juntar)
- Remoção (remover_espacos)
- Validação (eh_vazio, eh_numero, eh_letra, eh_alfanumerico)
- Formatação (preencher_esquerda, preencher_direita, recortar)

**Exemplo:**
```br
importar "texto"
nome é "maria silva"
mostre capitalizar(nome)  # "Maria silva"
mostre maiusculo(nome)    # "MARIA SILVA"
```

### 4. **colecoes.br** - Coleções
Estruturas de dados avançadas:
- Lista (adicionar, remover, obter, tamanho, primeiro, ultimo)
- Filtros (filtrar, mapear, reduzir, encontrar, todos, algum)
- Ordenação (ordenar, embaralhar, inverter)
- Agregação (soma, media, minimo, maximo)
- Concatenação (concatenar, unico, planificar)

**Exemplo:**
```br
importar "colecoes"
numeros é [3, 1, 4, 1, 5, 9]
ordenados é ordenar(numeros)
mostre ordenados  # [1, 1, 3, 4, 5, 9]
```

### 5. **data_hora.br** - Data e Hora
Manipulação de datas e horários:
- Data atual (data_atual, hora_atual, agora)
- Criação (criar_data, criar_hora)
- Formatação (formatar_data, formatar_hora)
- Extração (extrair_ano, extrair_mes, extrair_dia)
- Operações (somar_dias, somar_meses, diferenca_dias)
- Comparação (eh_hoje, eh_passado, eh_futuro)
- Dia da semana (dia_semana, nome_dia_semana, eh_fim_semana)
- Utilitários (eh_bissexto, dias_no_mes, idade)

**Exemplo:**
```br
importar "data_hora"
hoje é data_atual()
mostre formatar_data(hoje, "DD/MM/AAAA")
```

### 6. **sistema.br** - Sistema
Funções do sistema operacional:
- Ambiente (variavel_ambiente, diretorio_atual)
- Execução (executar_comando, executar_script)
- Informações (sistema_operacional, nome_computador, nome_usuario)
- Processos (lista_processos, executar_em_segundo_plano)
- Rede (nome_host, endereco_ip)
- Tempo (dormir, tempo_atual_ms)

**Exemplo:**
```br
importar "sistema"
mostre sistema_operacional()  # "Windows"
dormir(2)  # Aguarda 2 segundos
```

### 7. **rede.br** - Rede
Comunicação e protocolos:
- HTTP (http_get, http_post, http_put, http_delete)
- Requisições (requisicao_get, requisicao_post)
- URL (url_parse, url_join, url_codificar)
- JSON (json_parse, json_gerar)
- Utilitários (eh_url_valida, extrair_dominio)

**Exemplo:**
```br
importar "rede"
resposta é http_get("https://api.exemplo.com")
mostre resposta
```

### 8. **json.br** - JSON
Manipulação de JSON:
- Parsing (parse, stringify)
- Acesso (obter, definir)
- Validação (eh_json_valido)
- Utilitários (json_para_texto, texto_para_json)

**Exemplo:**
```br
importar "json"
dados é parse('{"nome": "João", "idade": 30}')
mostre obter(dados, "nome")  # "João"
```

### 9. **banco.br** - Banco de Dados
Interface simplificada para bancos:
- Conexão (conectar, conectar_sqlite)
- Queries (executar, consultar)
- CRUD (inserir, buscar, atualizar, remover)
- Utilitários (fechar, tabelas, estrutura_tabela)

**Exemplo:**
```br
importar "banco"
db é conectar_sqlite("dados.db")
inserir(db, "usuarios", {"nome": "Maria", "idade": 25})
resultado é buscar(db, "usuarios", "nome = 'Maria'")
```

### 10. **ia.br** - IA
Inteligência Artificial:
- IA Básica (perguntar, perguntar_com_contexto)
- Agentes (criar_agente, agente_perguntar)
- Análise (analisar_texto, analisar_codigo, resumir, traduzir)
- Geração (gerar_texto, gerar_codigo, completar_texto)
- Classificação (classificar, sentimento)
- Debugging (sugerir_correcao, explicar_erro)

**Exemplo:**
```br
importar "ia"
resposta é perguntar("Qual a capital do Brasil?")
mostre resposta
```

### 11. **cores.br** - Cores
Manipulação de cores e estilos:
- Cores básicas (cor_vermelho, cor_verde, cor_amarelo, cor_azul)
- Estilos (negrito, italico, sublinhado, piscar)
- Fundos (fundo_vermelho, fundo_verde, fundo_amarelo)
- Combinações (verde_negrito, vermelho_negrito)
- Utilitários (cor_aleatoria)

**Exemplo:**
```br
importar "cores"
mostre cor_verde("Sucesso!")
mostre vermelho_negrito("Erro!")
```

### 12. **crypto.br** - Criptografia
Funções criptográficas:
- Hash (hash_md5, hash_sha1, hash_sha256)
- Codificação (base64_codificar, base64_decodificar, url_codificar)
- Criptografia (cifrar_cesar, decifrar_cesar)
- Senhas (gerar_senha, gerar_token)

**Exemplo:**
```br
importar "crypto"
senha é gerar_senha(16)
mostre senha
hash é hash_sha256("minha_senha")
mostre hash
```

### 13. **log.br** - Log
Sistema de logging:
- Níveis (log_debug, log_info, log_aviso, log_erro, log_critico)
- Com cores (log_debug_cor, log_info_cor, etc.)
- Com data (log_com_data)
- Em arquivo (log_arquivo)
- Métricas (log_tempo_inicio, log_tempo_fim)

**Exemplo:**
```br
importar "log"
log_info("Aplicação iniciada")
inicio é log_tempo_inicio()
# ... código ...
duracao é log_tempo_fim(inicio, "Operação")
```

### 14. **teste.br** - Teste
Framework de testes unitários:
- Asserts (assert_igual, assert_diferente, assert_verdadeiro, assert_falso)
- Testes (teste, suite)
- Mocks (mock, spy)

**Exemplo:**
```br
importar "teste"

funcao teste_soma
    resultado é somar(2, 3)
    assert_igual(5, resultado)
fim

suite("Testes de Matemática", {
    "soma": teste_soma
})
```

---

## 📊 Estatísticas das Bibliotecas

- **Total de bibliotecas**: 13
- **Total de funções**: ~200+
- **Categorias**: 10+
- **Cobertura**: Core completo

---

## 🎯 Como Usar

### Importar biblioteca
```br
importar "matematica"
importar "texto"
importar "arquivo"
```

### Usar funções
```br
importar "matematica"
resultado é somar(10, 20)
mostre resultado
```

### Múltiplas bibliotecas
```br
importar "matematica"
importar "texto"
importar "colecoes"

numeros é [1, 2, 3, 4, 5]
dobro é mapear(numeros, multiplicar(2))
texto é juntar(dobro, ", ")
mostre texto  # "2, 4, 6, 8, 10"
```

---

## 🚀 Próximas Bibliotecas (Futuro)

- **web.br** - Framework web completo
- **gui.br** - Interface gráfica
- **ml.br** - Machine Learning
- **jogo.br** - Desenvolvimento de jogos
- **api.br** - Cliente API genérico
- **cache.br** - Sistema de cache
- **thread.br** - Threads avançadas
- **evento.br** - Sistema de eventos

---

## 💡 Filosofia das Bibliotecas

1. **Nomes em português** - Todas as funções em PT-BR
2. **Simplicidade** - API fácil de usar
3. **Consistência** - Padrões uniformes
4. **Completude** - Cobrem casos comuns
5. **Documentadas** - Exemplos claros

---

**"Bibliotecas que qualquer brasileiro consegue usar!"** 🇧🇷