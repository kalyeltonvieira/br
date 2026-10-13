// Runtime BR para JavaScript (IDE Web)
// Simula o runtime C++ para execução no navegador

class BRRuntime {
    constructor() {
        this.memoria = new Map();
        this.pilha = [];
        this.variaveis = {};
        this.funcoes = {};
        this.saida = [];
        
        this.inicializarFuncoesPadrao();
    }
    
    inicializarFuncoesPadrao() {
        // Funções básicas
        this.funcoes['mostre'] = (args) => {
            const valor = args.length > 0 ? args[0] : '';
            this.saida.push(this.formatarValor(valor));
            return valor;
        };
        
        this.funcoes['somar'] = (args) => {
            return args.reduce((a, b) => a + b, 0);
        };
        
        this.funcoes['subtrair'] = (args) => {
            return args.length >= 2 ? args[0] - args[1] : 0;
        };
        
        this.funcoes['multiplicar'] = (args) => {
            return args.reduce((a, b) => a * b, 1);
        };
        
        this.funcoes['dividir'] = (args) => {
            return args.length >= 2 && args[1] !== 0 ? args[0] / args[1] : 0;
        };
        
        this.funcoes['fatorial'] = (args) => {
            const n = args[0] || 0;
            if (n <= 1) return 1;
            let resultado = 1;
            for (let i = 2; i <= n; i++) {
                resultado *= i;
            }
            return resultado;
        };
        
        this.funcoes['fibonacci'] = (args) => {
            const n = args[0] || 0;
            if (n <= 0) return 0;
            if (n === 1) return 1;
            let a = 0, b = 1;
            for (let i = 2; i <= n; i++) {
                const temp = a + b;
                a = b;
                b = temp;
            }
            return b;
        };
    }
    
    formatarValor(valor) {
        if (typeof valor === 'string') {
            return valor;
        }
        if (typeof valor === 'number') {
            return valor.toString();
        }
        if (typeof valor === 'boolean') {
            return valor ? 'verdadeiro' : 'falso';
        }
        if (valor === null || valor === undefined) {
            return 'nulo';
        }
        return String(valor);
    }
    
    executar(codigo) {
        this.saida = [];
        this.variaveis = {};
        
        try {
            this.parserSimples(codigo);
        } catch (erro) {
            this.saida.push('Erro: ' + erro.message);
        }
        
        return this.saida;
    }
    
    parserSimples(codigo) {
        const linhas = codigo.split('\n');
        
        for (const linha of linhas) {
            const trim = linha.trim();
            
            // Ignorar comentários e linhas vazias
            if (!trim || trim.startsWith('//') || trim.startsWith('#')) {
                continue;
            }
            
            // mostre "texto"
            const matchMostre = trim.match(/^mostre\s+(.+)$/);
            if (matchMostre) {
                const valor = this.avaliarExpressao(matchMostre[1]);
                this.funcoes['mostre']([valor]);
                continue;
            }
            
            // variável é valor
            const matchAtribuicao = trim.match(/^(\w+)\s+é\s+(.+)$/);
            if (matchAtribuicao) {
                const nome = matchAtribuicao[1];
                const valor = this.avaliarExpressao(matchAtribuicao[2]);
                this.variaveis[nome] = valor;
                continue;
            }
            
            // se condição entao
            const matchSe = trim.match(/^se\s+(.+)\s+entao$/);
            if (matchSe) {
                // Implementação simplificada - apenas avalia condição
                const condicao = this.avaliarExpressao(matchSe[1]);
                if (!condicao) {
                    // Pular próxima linha (simplificado)
                    continue;
                }
            }
            
            // para i de X ate Y
            const matchPara = trim.match(/^para\s+(\w+)\s+de\s+(\d+)\s+ate\s+(\d+)$/);
            if (matchPara) {
                const varLoop = matchPara[1];
                const inicio = parseInt(matchPara[2]);
                const fim = parseInt(matchPara[3]);
                
                for (let i = inicio; i <= fim; i++) {
                    this.variaveis[varLoop] = i;
                }
                continue;
            }
            
            // chamada de função
            const matchFuncao = trim.match(/^(\w+)\((.*)\)$/);
            if (matchFuncao) {
                const nomeFuncao = matchFuncao[1];
                const argsStr = matchFuncao[2];
                const args = argsStr ? argsStr.split(',').map(a => this.avaliarExpressao(a.trim())) : [];
                
                if (this.funcoes[nomeFuncao]) {
                    this.funcoes[nomeFuncao](args);
                }
                continue;
            }
        }
    }
    
    avaliarExpressao(expr) {
        expr = expr.trim();
        
        // String literal
        if ((expr.startsWith('"') && expr.endsWith('"')) || 
            (expr.startsWith("'") && expr.endsWith("'"))) {
            return expr.slice(1, -1);
        }
        
        // Número
        if (!isNaN(expr)) {
            return parseFloat(expr);
        }
        
        // Booleanos
        if (expr === 'verdadeiro' || expr === 'true') {
            return true;
        }
        if (expr === 'falso' || expr === 'false') {
            return false;
        }
        
        // Variável
        if (this.variaveis.hasOwnProperty(expr)) {
            return this.variaveis[expr];
        }
        
        // Operações matemáticas simples
        const matchSoma = expr.match(/^(\d+)\s*\+\s*(\d+)$/);
        if (matchSoma) {
            return parseInt(matchSoma[1]) + parseInt(matchSoma[2]);
        }
        
        const matchSub = expr.match(/^(\d+)\s*-\s*(\d+)$/);
        if (matchSub) {
            return parseInt(matchSub[1]) - parseInt(matchSub[2]);
        }
        
        const matchMult = expr.match(/^(\d+)\s*\*\s*(\d+)$/);
        if (matchMult) {
            return parseInt(matchMult[1]) * parseInt(matchMult[2]);
        }
        
        // Concatenação de strings
        const matchConcat = expr.match(/^(.+)\s*\+\s*(.+)$/);
        if (matchConcat) {
            const esq = this.avaliarExpressao(matchConcat[1]);
            const dir = this.avaliarExpressao(matchConcat[2]);
            return String(esq) + String(dir);
        }
        
        return expr;
    }
    
    definirFuncao(nome, funcao) {
        this.funcoes[nome] = funcao;
    }
    
    obterSaida() {
        return this.saida;
    }
    
    limpar() {
        this.saida = [];
        this.variaveis = {};
        this.pilha = [];
    }
}

// Compilador BR para Bytecode (simplificado)
class BRCompiler {
    constructor() {
        this.opcodes = {
            'PUSH': 0x01,
            'POP': 0x02,
            'ADD': 0x05,
            'SUB': 0x06,
            'MUL': 0x07,
            'DIV': 0x08,
            'PRINT': 0x10,
            'LOAD': 0x11,
            'STORE': 0x12,
            'JUMP': 0x20,
            'JUMP_IF_FALSE': 0x21,
            'CALL': 0x30,
            'RET': 0x31,
            'HALT': 0xFF
        };
    }
    
    compilar(codigo) {
        const bytecode = [];
        
        // Header BRC1
        bytecode.push(0x42, 0x52, 0x43, 0x31); // "BRC1"
        
        // Versão
        bytecode.push(0x01, 0x00); // v1.0
        
        // Parser simplificado
        const linhas = codigo.split('\n');
        
        for (const linha of linhas) {
            const trim = linha.trim();
            
            if (!trim || trim.startsWith('//') || trim.startsWith('#')) {
                continue;
            }
            
            // mostre
            if (trim.startsWith('mostre')) {
                bytecode.push(this.opcodes['PRINT']);
                const valor = trim.substring(7).trim();
                this.compilarValor(valor, bytecode);
                continue;
            }
            
            // atribuição
            const matchAtrib = trim.match(/^(\w+)\s+é\s+(.+)$/);
            if (matchAtrib) {
                bytecode.push(this.opcodes['STORE']);
                bytecode.push(matchAtrib[1].length);
                for (const c of matchAtrib[1]) {
                    bytecode.push(c.charCodeAt(0));
                }
                this.compilarValor(matchAtrib[2], bytecode);
                continue;
            }
        }
        
        bytecode.push(this.opcodes['HALT']);
        
        return new Uint8Array(bytecode);
    }
    
    compilarValor(valor, bytecode) {
        valor = valor.trim();
        
        // String
        if ((valor.startsWith('"') && valor.endsWith('"')) || 
            (valor.startsWith("'") && valor.endsWith("'"))) {
            const str = valor.slice(1, -1);
            bytecode.push(0x40); // Tipo string
            bytecode.push(str.length);
            for (const c of str) {
                bytecode.push(c.charCodeAt(0));
            }
            return;
        }
        
        // Número
        if (!isNaN(valor)) {
            bytecode.push(0x41); // Tipo número
            const num = parseFloat(valor);
            const bytes = new Float64Array([num]);
            for (const b of bytes) {
                bytecode.push(b);
            }
            return;
        }
        
        // Variável
        bytecode.push(0x42); // Tipo variável
        bytecode.push(valor.length);
        for (const c of valor) {
            bytecode.push(c.charCodeAt(0));
        }
    }
}

// Exportar para uso global
window.BRRuntime = BRRuntime;
window.BRCompiler = BRCompiler;

// Inicializar runtime global
window.runtimeBR = new BRRuntime();
window.compilerBR = new BRCompiler();
