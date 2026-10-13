// BR IDE - JavaScript
// Editor online para Linguagem BR

class BRIde {
    constructor() {
        this.editor = null;
        this.runtime = null;
        this.codigoAtual = '';
        this.exemplos = {
            ola: `// Olá Mundo em BR
mostre "Olá, Mundo!"`,
            
            fatorial: `// Calcular Fatorial
funcao fatorial recebendo n
    se n <= 1 entao
        retorne 1
    senao
        retorne n * fatorial(n - 1)
    fim
fim

mostre "Fatorial de 5: " + fatorial(5)`,
            
            fibonacci: `// Sequência de Fibonacci
funcao fibonacci recebendo n
    se n <= 1 entao
        retorne n
    senao
        retorne fibonacci(n - 1) + fibonacci(n - 2)
    fim
fim

mostre "Fibonacci de 10:"
para i de 0 ate 10
    mostre fibonacci(i)
fim`,
            
            sqlite: `// Exemplo de SQLite Nativo
banco = abrir_banco("teste.db")

crie tabela usuarios (
    id inteiro chave primaria,
    nome texto,
    email texto
)

insira em usuarios (nome, email) valores ("João", "joao@email.com")
insira em usuarios (nome, email) valores ("Maria", "maria@email.com")

resultados = selecione * de usuarios
mostre resultados`,

            gui: `// Exemplo de GUI Nativa
janela = criar_janela("Minha Janela", 400, 300)

adicionar_botao(janela, "btn1", "Clique Aqui", 50, 50)
adicionar_campo(janela, "campo1", "Digite algo", 50, 100)
adicionar_rotulo(janela, "rotulo1", "Label", 50, 150)

mostrar_janela(janela)`,

            async: `// Exemplo de Async/Await
funcao tarefa_assincrona recebendo tempo
    aguarar(tempo)  // aguarda em milissegundos
    retorne "Tarefa concluída após " + tempo + "ms"
fim

// Executar assincronamente
resultado = assincrono tarefa_assincrono(1000)
mostre resultado`
        };
        
        this.documentacao = {
            sintaxe: `
<h3>Sintaxe Básica</h3>
<p>A linguagem BR usa sintaxe em português brasileiro.</p>
<pre><code>// Comentário
nome é "João"
idade é 25
mostre nome</code></pre>
            `,
            
            variaveis: `
<h3>Variáveis</h3>
<p>Declaração usando "é":</p>
<pre><code>nome é "João"
idade é 25
ativo é verdadeiro</code></pre>
            `,
            
            funcoes: `
<h3>Funções</h3>
<pre><code>funcao saudacao recebendo nome
    retorne "Olá, " + nome
fim</code></pre>
            `,
            
            controle: `
<h3>Controle de Fluxo</h3>
<pre><code>se condição entao
    // código
senao
    // código
fim

enquanto condição
    // código
fim

para i de 0 ate 10
    // código
fim</code></pre>
            `,
            
            classes: `
<h3>Classes e POO</h3>
<pre><code>classe Pessoa
    campo nome
    campo idade
    
    funcao apresentar recebendo
        mostre "Sou " + this.nome
    fim
fim</code></pre>
            `,
            
            modulos: `
<h3>Módulos</h3>
<pre><code>importar matematica
importar sqlite
importar rede</code></pre>
            `,
            
            sqlite: `
<h3>SQLite Nativo</h3>
<pre><code>banco = abrir_banco("dados.db")
crie tabela usuarios (id inteiro, nome texto)
insira em usuarios valores (1, "João")
resultados = selecione * de usuarios</code></pre>
            `,
            
            rede: `
<h3>Rede e HTTP</h3>
<pre><code>resposta = http_get("https://api.exemplo.com")
dados = http_post("https://api.exemplo.com", corpo)
iniciar_servidor(8080)</code></pre>
            `,
            
            gui: `
<h3>GUI Nativa</h3>
<pre><code>janela = criar_janela("Título", 400, 300)
adicionar_botao(janela, "btn", "Texto", x, y)
adicionar_campo(janela, "campo", "Texto", x, y)
mostrar_janela(janela)</code></pre>
            `,
            
            ia: `
<h3>IA Nativa</h3>
<pre><code>resposta = ia_perguntar("Explique buracos negros")
agente = criar_agente("assistente", "ajuda geral")
resposta = agente_perguntar(agente, "Como compilar?")</code></pre>
            `
        };
        
        this.inicializar();
    }
    
    inicializar() {
        this.inicializarEditor();
        this.inicializarEventos();
        this.inicializarRuntime();
        this.atualizarEstatisticas();
    }
    
    inicializarEditor() {
        this.editor = CodeMirror.fromTextArea(document.getElementById('editor'), {
            mode: 'javascript',
            theme: 'dracula',
            lineNumbers: true,
            indentUnit: 4,
            tabSize: 4,
            indentWithTabs: false,
            lineWrapping: true,
            autofocus: true
        });
        
        this.editor.setValue(this.exemplos.ola);
        this.codigoAtual = this.exemplos.ola;
        
        this.editor.on('change', () => {
            this.codigoAtual = this.editor.getValue();
            this.atualizarEstatisticas();
        });
    }
    
    inicializarEventos() {
        // Executar
        document.getElementById('btn-executar').addEventListener('click', () => {
            this.executarCodigo();
        });
        
        // Compilar
        document.getElementById('btn-compilar').addEventListener('click', () => {
            this.compilarCodigo();
        });
        
        // Limpar
        document.getElementById('btn-limpar').addEventListener('click', () => {
            this.editor.setValue('');
            this.limparConsole();
        });
        
        // Limpar console
        document.getElementById('btn-limpar-console').addEventListener('click', () => {
            this.limparConsole();
        });
        
        // Exemplos
        document.getElementById('btn-exemplos').addEventListener('click', () => {
            this.abrirModalExemplos();
        });
        
        // Compartilhar
        document.getElementById('btn-compartilhar').addEventListener('click', () => {
            this.compartilharCodigo();
        });
        
        // Seleção de exemplo
        document.getElementById('select-exemplo').addEventListener('change', (e) => {
            if (e.target.value && this.exemplos[e.target.value]) {
                this.editor.setValue(this.exemplos[e.target.value]);
                this.codigoAtual = this.exemplos[e.target.value];
            }
        });
        
        // Atalhos de teclado
        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey && e.key === 'Enter') {
                e.preventDefault();
                this.executarCodigo();
            }
            if (e.ctrlKey && e.key === 'l') {
                e.preventDefault();
                this.limparConsole();
            }
        });
        
        // Links de documentação
        document.querySelectorAll('.doc-links a').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                const doc = link.getAttribute('data-doc');
                this.abrirDocumentacao(doc);
            });
        });
        
        // Fechar modais
        document.querySelectorAll('.close').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.modal').forEach(m => {
                    m.style.display = 'none';
                });
            });
        });
        
        // Fechar modal ao clicar fora
        window.addEventListener('click', (e) => {
            if (e.target.classList.contains('modal')) {
                e.target.style.display = 'none';
            }
        });
    }
    
    inicializarRuntime() {
        // Inicializar runtime BR simulado
        this.runtime = {
            executar: (codigo) => {
                return new Promise((resolve) => {
                    setTimeout(() => {
                        // Simulação de execução
                        const linhas = codigo.split('\n');
                        const saida = [];
                        
                        for (const linha of linhas) {
                            const trim = linha.trim();
                            if (trim.startsWith('mostre')) {
                                const match = trim.match(/mostse\s+(.+)/);
                                if (match) {
                                    saida.push(match[1].replace(/["']/g, ''));
                                }
                            }
                        }
                        
                        resolve(saida);
                    }, 100);
                });
            },
            
            compilar: (codigo) => {
                return new Promise((resolve) => {
                    setTimeout(() => {
                        // Simulação de compilação
                        const bytecode = this.gerarBytecodeSimulado(codigo);
                        resolve(bytecode);
                    }, 200);
                });
            }
        };
    }
    
    async executarCodigo() {
        const inicio = performance.now();
        this.definirStatus('Executando...');
        this.limparConsole();
        
        try {
            const saida = await this.runtime.executar(this.codigoAtual);
            
            if (saida.length > 0) {
                saida.forEach(linha => {
                    this.escreverConsole(linha, 'success');
                });
            } else {
                this.escreverConsole('Código executado (sem saída)', 'info');
            }
            
            const tempo = Math.round(performance.now() - inicio);
            document.getElementById('stat-tempo').textContent = tempo + 'ms';
            
        } catch (erro) {
            this.escreverConsole('Erro: ' + erro.message, 'error');
        }
        
        this.definirStatus('Pronto');
    }
    
    async compilarCodigo() {
        this.definirStatus('Compilando...');
        this.limparConsole();
        
        try {
            const bytecode = await this.runtime.compilar(this.codigoAtual);
            this.escreverConsole('Compilação concluída!', 'success');
            this.escreverConsole('Bytecode gerado: ' + bytecode.length + ' bytes', 'info');
            
        } catch (erro) {
            this.escreverConsole('Erro de compilação: ' + erro.message, 'error');
        }
        
        this.definirStatus('Pronto');
    }
    
    escreverConsole(texto, tipo = '') {
        const console = document.getElementById('console-output');
        const linha = document.createElement('div');
        linha.className = 'console-line ' + tipo;
        linha.textContent = texto;
        console.appendChild(linha);
        console.scrollTop = console.scrollHeight;
    }
    
    limparConsole() {
        document.getElementById('console-output').innerHTML = '';
    }
    
    definirStatus(status) {
        document.getElementById('status-editor').textContent = status;
    }
    
    atualizarEstatisticas() {
        const codigo = this.editor.getValue();
        document.getElementById('stat-linhas').textContent = codigo.split('\n').length;
        document.getElementById('stat-caracteres').textContent = codigo.length;
    }
    
    abrirDocumentacao(doc) {
        const modal = document.getElementById('modal-doc');
        document.getElementById('modal-title').textContent = 'Documentação: ' + doc.charAt(0).toUpperCase() + doc.slice(1);
        document.getElementById('modal-body').innerHTML = this.documentacao[doc] || '<p>Documentação não encontrada.</p>';
        modal.style.display = 'block';
    }
    
    abrirModalExemplos() {
        const modal = document.getElementById('modal-exemplos');
        const lista = document.getElementById('exemplos-list');
        lista.innerHTML = '';
        
        Object.entries(this.exemplos).forEach(([nome, codigo]) => {
            const item = document.createElement('div');
            item.className = 'exemplo-item';
            item.innerHTML = `
                <h4>${nome.charAt(0).toUpperCase() + nome.slice(1)}</h4>
                <p>${codigo.split('\n')[0].substring(0, 50)}...</p>
            `;
            item.addEventListener('click', () => {
                this.editor.setValue(codigo);
                this.codigoAtual = codigo;
                modal.style.display = 'none';
            });
            lista.appendChild(item);
        });
        
        modal.style.display = 'block';
    }
    
    compartilharCodigo() {
        const codigo = btoa(this.codigoAtual);
        const url = window.location.origin + '?codigo=' + codigo;
        
        navigator.clipboard.writeText(url).then(() => {
            this.escreverConsole('Link copiado para área de transferência!', 'success');
        }).catch(() => {
            this.escreverConsole('URL: ' + url, 'info');
        });
    }
    
    gerarBytecodeSimulado(codigo) {
        // Simulação de geração de bytecode
        const bytes = [];
        for (let i = 0; i < codigo.length; i++) {
            bytes.push(codigo.charCodeAt(i) % 256);
        }
        return new Uint8Array(bytes);
    }
}

// Inicializar IDE quando DOM estiver pronto
document.addEventListener('DOMContentLoaded', () => {
    window.brIde = new BRIde();
    
    // Carregar código da URL se existir
    const urlParams = new URLSearchParams(window.location.search);
    const codigo = urlParams.get('codigo');
    if (codigo) {
        try {
            window.brIde.editor.setValue(atob(codigo));
            window.brIde.codigoAtual = atob(codigo);
            window.brIde.limparConsole();
            window.brIde.escreverConsole('Código carregado da URL', 'info');
        } catch (e) {
            console.error('Erro ao carregar código da URL:', e);
        }
    }
});
