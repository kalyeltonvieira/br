// Runtime Nativo BR v3.0 - Pipeline Completo: Lexer → Parser → AST → VM
// Compilador e executor nativo da linguagem BR em C++17
// COM MELHORIAS DE PERFORMANCE: JIT, GC, Memory Pool, String Pool
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#include <regex>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cstdint>
#include <variant>
#include "servico_arquivo.h"
#include "servico_rede.h"
#include "servico_gui.h"
#include "sqlite_servico.cpp"

// Performance v3.0 - Novos sistemas
#include "jit_compiler.h"
#include "garbage_collector.h"
#include "memory_pool.h"
#include "string_pool.h"

// Performance v3.0 Fase 2 - Otimizações avançadas
#include "jit_optimizing.h"
#include "inline_cache.h"
#include "type_specialization.h"
#include "compilation_cache.h"

// SQLite para banco de dados nativo (implementação embutida)

// HTTP Server
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#define VERSAO_BR "3.0.0"
#define MAX_PILHA 10000
#define MAX_CALL_DEPTH 1000

// Sistemas de Performance v3.0 (globais)
static JITCompiler* g_jit_compiler = nullptr;
static GarbageCollector* g_gc = nullptr;
static MemoryPool* g_memory_pool = nullptr;
static StringPool* g_string_pool = nullptr;

// Sistemas de Performance v3.0 Fase 2 (globais)
static JITOptimizing* g_jit_optimizing = nullptr;
static InlineCacheManager* g_inline_cache_manager = nullptr;
static TypeSpecializer* g_type_specializer = nullptr;
static CompilationCache* g_compilation_cache = nullptr;

// Inicialização dos sistemas de performance
void inicializar_sistemas_performance() {
    // Fase 1
    if (!g_jit_compiler) g_jit_compiler = new JITCompiler();
    if (!g_gc) g_gc = new GarbageCollector();
    if (!g_memory_pool) g_memory_pool = new MemoryPool();
    if (!g_string_pool) g_string_pool = new StringPool();
    
    // Fase 2
    if (!g_jit_optimizing) g_jit_optimizing = new JITOptimizing();
    if (!g_inline_cache_manager) g_inline_cache_manager = new InlineCacheManager();
    if (!g_type_specializer) g_type_specializer = new TypeSpecializer();
    if (!g_compilation_cache) g_compilation_cache = new CompilationCache();
    
    std::cout << "[Performance v3.0] Sistemas Fase 1 inicializados: JIT, GC, Memory Pool, String Pool" << std::endl;
    std::cout << "[Performance v3.0] Sistemas Fase 2 inicializados: JIT Optimizing, Inline Cache, Type Specialization, Compilation Cache" << std::endl;
}

// Limpeza dos sistemas de performance
void limpar_sistemas_performance() {
    // Fase 1
    delete g_jit_compiler;
    delete g_gc;
    delete g_memory_pool;
    delete g_string_pool;
    
    g_jit_compiler = nullptr;
    g_gc = nullptr;
    g_memory_pool = nullptr;
    g_string_pool = nullptr;
    
    // Fase 2
    delete g_jit_optimizing;
    delete g_inline_cache_manager;
    delete g_type_specializer;
    delete g_compilation_cache;
    
    g_jit_optimizing = nullptr;
    g_inline_cache_manager = nullptr;
    g_type_specializer = nullptr;
    g_compilation_cache = nullptr;
}

enum TipoToken {
    PALAVRA_RESERVADA, IDENTIFICADOR, TEXTO, FSTRING, INTEIRO, NUMERO, OPERADOR,
    PAREN_ESQ, PAREN_DIR, COLCHETE_ESQ, COLCHETE_DIR, CHAVE_ESQ, CHAVE_DIR,
    PONTO, VIRGULA, DOIS_PONTOS, ARROBA,
    FIM_DE_ARQUIVO, NOVA_LINHA, INDENTAR, DESINDENTAR
};

struct Token {
    TipoToken tipo;
    std::string valor;
    int linha, coluna;
    Token() : tipo(FIM_DE_ARQUIVO), linha(0), coluna(0) {}
    Token(TipoToken t, const std::string& v, int l, int c) : tipo(t), valor(v), linha(l), coluna(c) {}
};

enum TipoValor {
    VAL_INTEIRO, VAL_DECIMAL, VAL_TEXTO, VAL_BOOLEANO,
    VAL_NULO, VAL_LISTA, VAL_REGISTRO, VAL_FUNCAO, VAL_CLASSE, VAL_MODULO
};

struct ErroRuntime {
    std::string mensagem;
    int linha, coluna;
    ErroRuntime(const std::string& msg, int l = 0, int c = 0) : mensagem(msg), linha(l), coluna(c) {}
};

struct NoAST;

struct FuncaoBR {
    std::string nome;
    std::vector<std::string> parametros;
    NoAST* corpoAST;
    FuncaoBR(const std::string& n) : nome(n), corpoAST(nullptr) {}
};

struct ClasseBR {
    std::string nome, classeMae;
    std::vector<std::string> campos;
    std::map<std::string, std::shared_ptr<FuncaoBR>> metodos;
    ClasseBR(const std::string& n) : nome(n) {}
};

struct ModuloBR {
    std::string nome, caminho;
    std::map<std::string, class Valor> exportacoes;
    ModuloBR(const std::string& n) : nome(n) {}
};

struct Valor {
    TipoValor tipo;
    int64_t inteiro;
    double decimal;
    std::string texto;
    bool booleano;
    std::vector<Valor> lista;
    std::map<std::string, Valor> registro;
    std::shared_ptr<FuncaoBR> funcao;
    std::shared_ptr<ClasseBR> classe;
    std::shared_ptr<ModuloBR> modulo;
    
    Valor() : tipo(VAL_NULO), inteiro(0), decimal(0.0), booleano(false) {}
    
    static Valor criarInteiro(int64_t v) { Valor val; val.tipo = VAL_INTEIRO; val.inteiro = v; return val; }
    static Valor criarDecimal(double v) { Valor val; val.tipo = VAL_DECIMAL; val.decimal = v; return val; }
    static Valor criarTexto(const std::string& v) { Valor val; val.tipo = VAL_TEXTO; val.texto = v; return val; }
    static Valor criarBooleano(bool v) { Valor val; val.tipo = VAL_BOOLEANO; val.booleano = v; return val; }
    static Valor criarNulo() { return Valor(); }
    static Valor criarLista() { Valor val; val.tipo = VAL_LISTA; return val; }
    static Valor criarRegistro() { Valor val; val.tipo = VAL_REGISTRO; return val; }
    static Valor criarFuncao(std::shared_ptr<FuncaoBR> fn) { Valor val; val.tipo = VAL_FUNCAO; val.funcao = fn; return val; }
    static Valor criarClasse(std::shared_ptr<ClasseBR> c) { Valor val; val.tipo = VAL_CLASSE; val.classe = c; return val; }
    static Valor criarModulo(std::shared_ptr<ModuloBR> m) { Valor val; val.tipo = VAL_MODULO; val.modulo = m; return val; }
    
    std::string paraTexto() const {
        switch (tipo) {
            case VAL_INTEIRO: return std::to_string(inteiro);
            case VAL_DECIMAL: {
                std::string s = std::to_string(decimal);
                size_t p = s.find_last_not_of('0');
                if (p != std::string::npos && s[p] == '.') p--;
                return s.substr(0, p + 1);
            }
            case VAL_TEXTO: return texto;
            case VAL_BOOLEANO: return booleano ? "verdadeiro" : "falso";
            case VAL_NULO: return "nulo";
            case VAL_LISTA: {
                std::string r = "[";
                for (size_t i = 0; i < lista.size(); i++) {
                    if (i > 0) r += ", ";
                    r += lista[i].paraTexto();
                }
                return r + "]";
            }
            case VAL_REGISTRO: {
                std::string r = "{";
                bool primeiro = true;
                for (auto& [k, v] : registro) {
                    if (!primeiro) r += ", ";
                    r += k + ": " + v.paraTexto();
                    primeiro = false;
                }
                return r + "}";
            }
            default: return "<objeto>";
        }
    }
    
    double paraNumero() const {
        switch (tipo) {
            case VAL_INTEIRO: return (double)inteiro;
            case VAL_DECIMAL: return decimal;
            case VAL_TEXTO: { try { return std::stod(texto); } catch (...) { return 0; } }
            case VAL_BOOLEANO: return booleano ? 1.0 : 0.0;
            default: return 0;
        }
    }
    
    bool paraBooleano() const {
        switch (tipo) {
            case VAL_BOOLEANO: return booleano;
            case VAL_NULO: return false;
            case VAL_INTEIRO: return inteiro != 0;
            case VAL_DECIMAL: return decimal != 0.0;
            case VAL_TEXTO: return !texto.empty();
            case VAL_LISTA: return !lista.empty();
            case VAL_REGISTRO: return !registro.empty();
            default: return true;
        }
    }
};

enum TipoNo {
    NO_PROGRAMA, NO_BLOCO, NO_NUMERO_INT, NO_NUMERO_DEC, NO_TEXTO_LITERAL,
    NO_BOOLEANO, NO_NULO, NO_IDENTIFICADOR,
    NO_ATRIBUICAO, NO_DECLARACAO, NO_CONSTANTE,
    NO_OPERACAO_BINARIA, NO_OPERACAO_UNARIA,
    NO_CHAMADA_FUNCAO, NO_CHAMADA_METODO,
    NO_ACESSO_PROPRIEDADE, NO_ACESSO_INDICE,
    NO_LISTA_LITERAL, NO_REGISTRO_LITERAL,
    NO_LISTA_COMPREHENSION,
    NO_MOSTRE, NO_DEVOLVA, NO_SALVE, NO_REMOVA,
    NO_SE, NO_ENQUANTO, NO_REPITA, NO_CONTE,
    NO_PARA_CADA, NO_ESCOLHA_CASO,
    NO_TENTE_CAPTURE, NO_ESPERE,
    NO_FUNCAO, NO_TAREFA, NO_EXECUTE_PARALELO,
    NO_CLASSE, NO_IMPORTAR,
    NO_IA_PERGUNTE, NO_IA_AGENTE,
    NO_AGUARDE, NO_EXPRESSAO_STMT,
    NO_DECORATOR, NO_YIELD, NO_CONTEXT_MANAGER
};

struct NoAST {
    TipoNo tipo;
    int linha, coluna;
    int64_t valInteiro;
    double valDecimal;
    std::string valTexto;
    bool valBooleano;
    std::vector<std::unique_ptr<NoAST>> filhos;
    std::string nome;
    std::string operador;
    std::map<std::string, std::unique_ptr<NoAST>> propriedades;
    
    NoAST(TipoNo t, int l = 0, int c = 0) 
        : tipo(t), linha(l), coluna(c), valInteiro(0), valDecimal(0.0), valBooleano(false) {}
};

// ===================== LEXER =====================
class LexerBR {
private:
    std::string codigo;
    size_t pos;
    int linha, coluna;
    std::vector<int> niveis;
    bool inicioLinha;
    
    std::map<std::string, std::string> palavrasReservadas = {
        {"funcao", "funcao"}, {"função", "funcao"},
        {"recebendo", "recebendo"}, {"devolva", "devolva"}, {"fim", "fim"},
        {"se", "se"}, {"então", "entao"}, {"entao", "entao"},
        {"senão", "senao"}, {"senao", "senao"},
        {"enquanto", "enquanto"}, {"para", "para"}, {"cada", "cada"},
        {"em", "em"}, {"repita", "repita"}, {"vezes", "vezes"},
        {"conte", "conte"}, {"de", "de"}, {"até", "ate"}, {"ate", "ate"},
        {"como", "como"}, {"escolha", "escolha"}, {"caso", "caso"},
        {"padrao", "padrao"}, {"padrão", "padrao"},
        {"tente", "tente"}, {"capture", "capture"},
        {"execute", "execute"}, {"paralelo", "paralelo"},
        {"tarefa", "tarefa"}, {"aguarde", "aguarde"},
        {"objetivo", "objetivo"}, {"ia", "ia"}, {"agente", "agente"},
        {"crie", "crie"}, {"especialidade", "especialidade"},
        {"importar", "importar"}, {"importe", "importe"},
        {"classe", "classe"}, {"contrato", "contrato"},
        {"constante", "constante"}, {"é", "é"}, {"e", "e"},
        {"lista", "lista"}, {"registro", "registro"},
        {"texto", "texto"}, {"inteiro", "inteiro"},
        {"decimal", "decimal"}, {"booleano", "booleano"},
        {"verdadeiro", "verdadeiro"}, {"falso", "falso"}, {"nulo", "nulo"},
        {"explicacao", "explicacao"}, {"explicação", "explicacao"},
        {"teste", "teste"}, {"espere", "espere"}, {"igual", "igual"},
        {"a", "a"}, {"mostre", "mostre"}, {"salve", "salve"},
        {"remova", "remova"}, {"busque", "busque"}, {"acesse", "acesse"},
        {"site", "site"}, {"rota", "rota"}, {"porta", "porta"},
        {"janela", "janela"}, {"campo", "campo"}, {"botao", "botao"},
        {"botão", "botao"}, {"quando", "quando"}, {"clicar", "clicar"},
        {"titulo", "titulo"}, {"título", "titulo"},
        {"largura", "largura"}, {"altura", "altura"}, {"abra", "abra"},
        {"isto", "isto"}, {"onde", "onde"}, {"metodo", "metodo"},
        {"método", "metodo"}, {"herda", "herda"}, {"falhou", "falhou"},
        {"nao", "nao"}, {"não", "nao"}, {"pergunte", "pergunte"},
        {"yield", "yield"}, {"contexto", "contexto"}, {"como", "como"}
    };
    
    char charAtual() const { return pos < codigo.size() ? codigo[pos] : '\0'; }
    
    char avancar() {
        char c = charAtual();
        if (c == '\n') { linha++; coluna = 1; } else { coluna++; }
        pos++;
        return c;
    }
    
    std::string lerIdentificador() {
        std::string r;
        while (pos < codigo.size()) {
            unsigned char c = (unsigned char)codigo[pos];
            if (std::isalnum(c) || c == '_' || c >= 0x80)
                r += avancar();
            else
                break;
        }
        return r;
    }
    
    std::string lerNumero() {
        std::string r;
        bool temPonto = false;
        while (pos < codigo.size() && (std::isdigit(codigo[pos]) || (!temPonto && codigo[pos] == '.'))) {
            if (codigo[pos] == '.') temPonto = true;
            r += avancar();
        }
        return r;
    }
    
    std::string lerTexto(char delim) {
        avancar(); std::string r;
        while (pos < codigo.size() && codigo[pos] != delim) {
            if (codigo[pos] == '\\') {
                avancar(); char esc = avancar();
                switch (esc) {
                    case 'n': r += '\n'; break; case 't': r += '\t'; break;
                    case 'r': r += '\r'; break; case '\\': r += '\\'; break;
                    case '"': r += '"'; break; case '\'': r += '\''; break;
                    default: r += '\\'; r += esc; break;
                }
            } else r += avancar();
        }
        if (pos < codigo.size() && codigo[pos] == delim) avancar();
        return r;
    }
    
    std::string lerFString(char delim) {
        avancar(); std::string r;
        while (pos < codigo.size() && codigo[pos] != delim) {
            if (codigo[pos] == '{') {
                avancar();
                std::string expr;
                int profundidade = 1;
                while (pos < codigo.size() && profundidade > 0) {
                    if (codigo[pos] == '{') profundidade++;
                    else if (codigo[pos] == '}') profundidade--;
                    if (profundidade > 0) expr += avancar();
                    else avancar();
                }
                r += "${" + expr + "}";
            } else if (codigo[pos] == '\\') {
                avancar(); char esc = avancar();
                switch (esc) {
                    case 'n': r += '\n'; break; case 't': r += '\t'; break;
                    case 'r': r += '\r'; break; case '\\': r += '\\'; break;
                    case '"': r += '"'; break; case '\'': r += '\''; break;
                    default: r += '\\'; r += esc; break;
                }
            } else r += avancar();
        }
        if (pos < codigo.size() && codigo[pos] == delim) avancar();
        return r;
    }

public:
    LexerBR(const std::string& src) 
        : codigo(src), pos(0), linha(1), coluna(1), inicioLinha(true) {
        niveis.push_back(0);
        if (codigo.size() >= 3 && (unsigned char)codigo[0] == 0xEF && 
            (unsigned char)codigo[1] == 0xBB && (unsigned char)codigo[2] == 0xBF)
            pos = 3;
    }
    
    std::vector<Token> tokenizar() {
        std::vector<Token> tokens;
        while (pos < codigo.size()) {
            if (codigo[pos] == '\n') {
                tokens.push_back(Token(NOVA_LINHA, "\n", linha, coluna));
                avancar(); inicioLinha = true; continue;
            }
            if (inicioLinha) {
                int esp = 0;
                while (pos < codigo.size() && codigo[pos] == ' ') { esp++; avancar(); }
                char c = codigo[pos];
                if (c != '\n' && c != '\0' && c != '#') {
                    int nivel = niveis.back();
                    if (esp > nivel) { niveis.push_back(esp); tokens.push_back(Token(INDENTAR, "INDENTAR", linha, coluna)); }
                    else if (esp < nivel) {
                        while (!niveis.empty() && niveis.back() > esp) { niveis.pop_back(); tokens.push_back(Token(DESINDENTAR, "DESINDENTAR", linha, coluna)); }
                    }
                }
                inicioLinha = false;
            }
            if (codigo[pos] == ' ' || codigo[pos] == '\t') { avancar(); continue; }
            if (codigo[pos] == '\r') { avancar(); continue; }
            char c = codigo[pos];
            int l = linha, col = coluna;
            if (c == '#') { while (pos < codigo.size() && codigo[pos] != '\n') avancar(); continue; }
            if (c == '"' || c == '\'') {
                std::string texto = lerTexto(c);
                if (texto.find("${") != std::string::npos)
                    tokens.push_back(Token(FSTRING, texto, l, col));
                else
                    tokens.push_back(Token(TEXTO, texto, l, col));
                continue;
            }
            if (std::isdigit(c)) {
                std::string num = lerNumero();
                bool ehInteiro = num.find('.') == std::string::npos;
                tokens.push_back(Token(ehInteiro ? INTEIRO : NUMERO, num, l, col)); continue;
            }
            unsigned char uc = (unsigned char)c;
            if (std::isalpha(uc) || uc == '_' || uc >= 0x80) {
                std::string id = lerIdentificador();
                auto it = palavrasReservadas.find(id);
                if (it != palavrasReservadas.end()) tokens.push_back(Token(PALAVRA_RESERVADA, it->second, l, col));
                else tokens.push_back(Token(IDENTIFICADOR, id, l, col));
                continue;
            }
            if (c == '@') { avancar(); tokens.push_back(Token(ARROBA, "@", l, col)); continue; }
            if (c == ':') { avancar(); tokens.push_back(Token(DOIS_PONTOS, ":", l, col)); continue; }
            if (c == '.') { avancar(); tokens.push_back(Token(PONTO, ".", l, col)); continue; }
            if (c == ',') { avancar(); tokens.push_back(Token(VIRGULA, ",", l, col)); continue; }
            if (c == '(') { avancar(); tokens.push_back(Token(PAREN_ESQ, "(", l, col)); continue; }
            if (c == ')') { avancar(); tokens.push_back(Token(PAREN_DIR, ")", l, col)); continue; }
            if (c == '[') { avancar(); tokens.push_back(Token(COLCHETE_ESQ, "[", l, col)); continue; }
            if (c == ']') { avancar(); tokens.push_back(Token(COLCHETE_DIR, "]", l, col)); continue; }
            if (c == '{') { avancar(); tokens.push_back(Token(CHAVE_ESQ, "{", l, col)); continue; }
            if (c == '}') { avancar(); tokens.push_back(Token(CHAVE_DIR, "}", l, col)); continue; }
            if ((c == '=' && pos + 1 < codigo.size() && codigo[pos + 1] == '=') ||
                (c == '!' && pos + 1 < codigo.size() && codigo[pos + 1] == '=') ||
                (c == '>' && pos + 1 < codigo.size() && codigo[pos + 1] == '=') ||
                (c == '<' && pos + 1 < codigo.size() && codigo[pos + 1] == '=')) {
                std::string op; op += c; op += codigo[pos + 1];
                avancar(); avancar();
                tokens.push_back(Token(OPERADOR, op, l, col)); continue;
            }
            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<' || c == '!') {
                std::string op; op += c; avancar();
                tokens.push_back(Token(OPERADOR, op, l, col)); continue;
            }
            avancar();
        }
        while (niveis.size() > 1) { niveis.pop_back(); tokens.push_back(Token(DESINDENTAR, "DESINDENTAR", linha, coluna)); }
        tokens.push_back(Token(FIM_DE_ARQUIVO, "EOF", linha, coluna));
        return tokens;
    }
};

// ===================== PARSER =====================
class ParserBR {
private:
    std::vector<Token> tokens;
    size_t pos;
    std::vector<std::string> erros;
    
    Token& atual() { return pos < tokens.size() ? tokens[pos] : tokens.back(); }
    Token& avancarToken() { if (pos < tokens.size()) return tokens[pos++]; return tokens.back(); }
    bool verificar(TipoToken t) { return atual().tipo == t; }
    bool verificarPalavra(const std::string& p) { return verificar(PALAVRA_RESERVADA) && atual().valor == p; }
    
    Token consumir(TipoToken t, const std::string& msg) {
        if (verificar(t)) return avancarToken();
        ErroSintaxe(msg); return tokens.back();
    }
    
    Token consumirPalavra(const std::string& p, const std::string& msg) {
        if (verificarPalavra(p)) return avancarToken();
        ErroSintaxe(msg); return tokens.back();
    }
    
    void ErroSintaxe(const std::string& msg) {
        std::string erro = "Erro Sintático [Linha " + std::to_string(atual().linha) + 
                          ", Coluna " + std::to_string(atual().coluna) + "]: " + msg;
        erros.push_back(erro);
        throw std::runtime_error(erro);
    }
    
    void pularNovasLinhas() { while (verificar(NOVA_LINHA)) avancarToken(); }
    
    bool fimInstrucao() {
        return verificar(NOVA_LINHA) || verificar(INDENTAR) || 
               verificar(DESINDENTAR) || verificar(FIM_DE_ARQUIVO) || verificarPalavra("fim");
    }
    
    std::unique_ptr<NoAST> criarNo(TipoNo t) { return std::make_unique<NoAST>(t, atual().linha, atual().coluna); }
    
    std::unique_ptr<NoAST> expressao();
    std::unique_ptr<NoAST> expressaoLogica();
    std::unique_ptr<NoAST> expressaoComparacao();
    std::unique_ptr<NoAST> expressaoSoma();
    std::unique_ptr<NoAST> expressaoMultiplica();
    std::unique_ptr<NoAST> expressaoUnaria();
    std::unique_ptr<NoAST> chamada();
    std::unique_ptr<NoAST> primario();
    std::unique_ptr<NoAST> parseListaInline();
    std::unique_ptr<NoAST> parseRegistroInline();
    std::unique_ptr<NoAST> parseListaLiteral();
    std::unique_ptr<NoAST> parseRegistroLiteral();
    std::unique_ptr<NoAST> parseBloco();
    std::unique_ptr<NoAST> declaracao();
    std::unique_ptr<NoAST> parseFuncao();
    std::unique_ptr<NoAST> parseClasse();
    std::unique_ptr<NoAST> parseImportacao();
    std::unique_ptr<NoAST> parseConstante();
    std::unique_ptr<NoAST> parseSe();
    std::unique_ptr<NoAST> parseEnquanto();
    std::unique_ptr<NoAST> parseParaCada();
    std::unique_ptr<NoAST> parseRepita();
    std::unique_ptr<NoAST> parseConte();
    std::unique_ptr<NoAST> parseTenteCapture();
    std::unique_ptr<NoAST> parseEscolha();
    std::unique_ptr<NoAST> parseExecuteParalelo();
    std::unique_ptr<NoAST> parseTarefa();
    std::unique_ptr<NoAST> parseExplicacao();
    std::unique_ptr<NoAST> parseObjetivo();
    std::unique_ptr<NoAST> parseTeste();
    std::unique_ptr<NoAST> parseMostre();
    std::unique_ptr<NoAST> parseRetorno();
    std::unique_ptr<NoAST> parseSalve();
    std::unique_ptr<NoAST> parseRemova();
    std::unique_ptr<NoAST> parseEspere();
    std::unique_ptr<NoAST> parseAguarde();
    std::unique_ptr<NoAST> parseIAPergunte();
    std::unique_ptr<NoAST> parseIAAgente();
    std::unique_ptr<NoAST> parseSite() { return criarNo(NO_EXPRESSAO_STMT); }
    std::unique_ptr<NoAST> parseJanela() { return criarNo(NO_EXPRESSAO_STMT); }
    std::unique_ptr<NoAST> parseAtribuicaoOuDeclaracao();
    std::unique_ptr<NoAST> parseContextManager();

public:
    ParserBR(const std::vector<Token>& t) : tokens(t), pos(0) {}
    
    std::unique_ptr<NoAST> parse() {
        auto programa = criarNo(NO_PROGRAMA);
        while (!verificar(FIM_DE_ARQUIVO)) {
            pularNovasLinhas();
            if (verificar(FIM_DE_ARQUIVO)) break;
            try {
                auto decl = declaracao();
                if (decl) programa->filhos.push_back(std::move(decl));
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                while (!verificar(NOVA_LINHA) && !verificar(FIM_DE_ARQUIVO)) avancarToken();
                if (verificar(NOVA_LINHA)) avancarToken();
            }
        }
        return programa;
    }
    
    bool temErros() const { return !erros.empty(); }
    std::vector<std::string> getErros() const { return erros; }
};

// Expressões
std::unique_ptr<NoAST> ParserBR::expressao() { return expressaoLogica(); }

std::unique_ptr<NoAST> ParserBR::expressaoLogica() {
    auto esq = expressaoComparacao();
    while (verificar(PALAVRA_RESERVADA) && (atual().valor == "e" || atual().valor == "nao")) {
        std::string op = avancarToken().valor;
        auto dir = expressaoComparacao();
        auto no = criarNo(NO_OPERACAO_BINARIA);
        no->operador = op;
        no->filhos.push_back(std::move(esq));
        no->filhos.push_back(std::move(dir));
        esq = std::move(no);
    }
    return esq;
}

std::unique_ptr<NoAST> ParserBR::expressaoComparacao() {
    auto esq = expressaoSoma();
    static const std::vector<std::string> comparadores = {
        "igual", "maior que", "menor que", "==", "!=", ">", "<", ">=", "<="
    };
    while (true) {
        bool encontrou = false;
        for (auto& c : comparadores) {
            if ((verificar(OPERADOR) && atual().valor == c) || verificarPalavra(c)) { encontrou = true; break; }
        }
        if (!encontrou) break;
        std::string op = avancarToken().valor;
        if (op == "igual" && verificarPalavra("a")) { avancarToken(); op = "=="; }
        else if (op == "maior" && verificarPalavra("que")) { avancarToken(); op = ">"; }
        else if (op == "menor" && verificarPalavra("que")) { avancarToken(); op = "<"; }
        else if (op == "nao") { op = "!="; }
        auto dir = expressaoSoma();
        auto no = criarNo(NO_OPERACAO_BINARIA);
        no->operador = op;
        no->filhos.push_back(std::move(esq)); no->filhos.push_back(std::move(dir));
        esq = std::move(no);
    }
    return esq;
}

std::unique_ptr<NoAST> ParserBR::expressaoSoma() {
    auto esq = expressaoMultiplica();
    while (verificar(OPERADOR) && (atual().valor == "+" || atual().valor == "-")) {
        std::string op = avancarToken().valor;
        auto dir = expressaoMultiplica();
        auto no = criarNo(NO_OPERACAO_BINARIA); no->operador = op;
        no->filhos.push_back(std::move(esq)); no->filhos.push_back(std::move(dir));
        esq = std::move(no);
    }
    return esq;
}

std::unique_ptr<NoAST> ParserBR::expressaoMultiplica() {
    auto esq = expressaoUnaria();
    while (verificar(OPERADOR) && (atual().valor == "*" || atual().valor == "/")) {
        std::string op = avancarToken().valor;
        auto dir = expressaoUnaria();
        auto no = criarNo(NO_OPERACAO_BINARIA); no->operador = op;
        no->filhos.push_back(std::move(esq)); no->filhos.push_back(std::move(dir));
        esq = std::move(no);
    }
    return esq;
}

std::unique_ptr<NoAST> ParserBR::expressaoUnaria() {
    if (verificar(OPERADOR) && atual().valor == "-") {
        avancarToken();
        auto no = criarNo(NO_OPERACAO_UNARIA); no->operador = "-";
        no->filhos.push_back(expressaoUnaria());
        return no;
    }
    if (verificarPalavra("nao")) {
        avancarToken();
        auto no = criarNo(NO_OPERACAO_UNARIA); no->operador = "nao";
        no->filhos.push_back(expressaoUnaria());
        return no;
    }
    return chamada();
}

std::unique_ptr<NoAST> ParserBR::chamada() {
    auto expr = primario();
    
    while (true) {
        if (verificar(PONTO)) {
            avancarToken();
            if (verificar(IDENTIFICADOR) || verificar(PALAVRA_RESERVADA)) {
                Token prop = avancarToken();
                auto no = criarNo(NO_ACESSO_PROPRIEDADE);
                no->valTexto = prop.valor; no->filhos.push_back(std::move(expr));
                expr = std::move(no);
                
                if (verificar(IDENTIFICADOR) || verificar(TEXTO) || verificar(INTEIRO) || 
                    verificar(NUMERO) || verificarPalavra("verdadeiro") || verificarPalavra("falso") || verificarPalavra("nulo")) {
                    std::vector<std::unique_ptr<NoAST>> args;
                    while (!fimInstrucao() && !verificar(PAREN_DIR) && !verificar(VIRGULA)) {
                        if (verificarPalavra("e")) { avancarToken(); continue; }
                        args.push_back(expressao());
                    }
                    auto no2 = criarNo(NO_CHAMADA_METODO);
                    no2->nome = no->valTexto; no2->filhos.push_back(std::move(no->filhos[0]));
                    for (auto& a : args) no2->filhos.push_back(std::move(a));
                    expr = std::move(no2);
                }
            }
            continue;
        }
        
        if (verificar(COLCHETE_ESQ)) {
            avancarToken(); auto indice = expressao();
            consumir(COLCHETE_DIR, "Esperava ']'");
            auto no = criarNo(NO_ACESSO_INDICE); no->filhos.push_back(std::move(expr)); no->filhos.push_back(std::move(indice));
            expr = std::move(no); continue;
        }
        
        if (verificar(PAREN_ESQ)) {
            avancarToken();
            std::vector<std::unique_ptr<NoAST>> args;
            pularNovasLinhas();
            if (!verificar(PAREN_DIR)) {
                args.push_back(expressao());
                while (verificar(VIRGULA) || verificarPalavra("e")) {
                    avancarToken(); pularNovasLinhas();
                    if (verificar(PAREN_DIR)) break;
                    args.push_back(expressao());
                }
            }
            consumir(PAREN_DIR, "Esperava ')'");
            auto no = criarNo(NO_CHAMADA_FUNCAO); no->nome = "__parenteses__";
            no->filhos.push_back(std::move(expr));
            for (auto& a : args) no->filhos.push_back(std::move(a));
            expr = std::move(no); continue;
        }
        
        if (expr->tipo == NO_IDENTIFICADOR && !fimInstrucao() &&
            !verificar(OPERADOR) && !verificar(PAREN_DIR) && !verificar(VIRGULA) &&
            !verificar(COLCHETE_DIR) && !verificar(PONTO)) {
            std::vector<std::unique_ptr<NoAST>> args;
            while (!fimInstrucao() && !verificar(PAREN_DIR) && !verificar(VIRGULA)) {
                if (verificar(PALAVRA_RESERVADA)) {
                    std::string v = atual().valor;
                    if (v == "e" || v == "fim" || v == "entao" || v == "senao" || v == "como") break;
                    if (v == "ate" || v == "de" || v == "em") break;
                    if (v == "vezes") break;
                }
                args.push_back(expressao());
                if (verificarPalavra("e")) { avancarToken(); continue; }
                if (verificar(VIRGULA)) { avancarToken(); continue; }
                break;
            }
            auto no = criarNo(NO_CHAMADA_FUNCAO); no->nome = expr->nome;
            for (auto& a : args) no->filhos.push_back(std::move(a));
            expr = std::move(no); continue;
        }
        break;
    }
    return expr;
}

std::unique_ptr<NoAST> ParserBR::primario() {
    pularNovasLinhas();
    if (verificarPalavra("nulo")) { avancarToken(); return criarNo(NO_NULO); }
    if (verificarPalavra("verdadeiro")) { avancarToken(); auto no = criarNo(NO_BOOLEANO); no->valBooleano = true; return no; }
    if (verificarPalavra("falso")) { avancarToken(); auto no = criarNo(NO_BOOLEANO); no->valBooleano = false; return no; }
    if (verificar(TEXTO)) { auto no = criarNo(NO_TEXTO_LITERAL); no->valTexto = avancarToken().valor; return no; }
    if (verificar(FSTRING)) {
        auto no = criarNo(NO_TEXTO_LITERAL);
        std::string fstr = avancarToken().valor;
        std::string resultado;
        size_t pos = 0;
        while (pos < fstr.size()) {
            size_t inicio = fstr.find("${", pos);
            if (inicio == std::string::npos) {
                resultado += fstr.substr(pos);
                break;
            }
            resultado += fstr.substr(pos, inicio - pos);
            size_t fim = fstr.find("}", inicio);
            if (fim == std::string::npos) {
                resultado += fstr.substr(inicio);
                break;
            }
            std::string expr = fstr.substr(inicio + 2, fim - inicio - 2);
            resultado += "${" + expr + "}";
            pos = fim + 1;
        }
        no->valTexto = resultado;
        return no;
    }
    if (verificar(INTEIRO)) { auto no = criarNo(NO_NUMERO_INT); no->valInteiro = std::stoll(avancarToken().valor); return no; }
    if (verificar(NUMERO)) { auto no = criarNo(NO_NUMERO_DEC); no->valDecimal = std::stod(avancarToken().valor); return no; }
    if (verificar(PAREN_ESQ)) { avancarToken(); auto e = expressao(); consumir(PAREN_DIR, "Esperava ')'"); return e; }
        if (verificar(COLCHETE_ESQ)) return parseListaInline();
        if (verificar(CHAVE_ESQ)) return parseRegistroInline();
        if (verificarPalavra("lista")) {
            avancarToken();
            if (verificar(COLCHETE_ESQ)) {
                auto no = criarNo(NO_LISTA_COMPREHENSION);
                no->filhos.push_back(expressao());
                consumirPalavra("para", "Esperava 'para'");
                no->nome = consumir(IDENTIFICADOR, "Esperava variável").valor;
                consumirPalavra("em", "Esperava 'em'");
                no->filhos.push_back(expressao());
                if (verificarPalavra("se")) {
                    avancarToken();
                    auto cond = criarNo(NO_SE);
                    cond->filhos.push_back(expressao());
                    no->filhos.push_back(std::move(cond));
                }
                consumir(COLCHETE_DIR, "Esperava ']'");
                return no;
            }
            return parseListaLiteral();
        }
    if (verificarPalavra("registro")) return parseRegistroLiteral();
    if (verificar(IDENTIFICADOR)) { auto no = criarNo(NO_IDENTIFICADOR); no->nome = avancarToken().valor; return no; }
    if (verificar(PALAVRA_RESERVADA) && atual().valor == "isto") { avancarToken(); auto no = criarNo(NO_IDENTIFICADOR); no->nome = "isto"; return no; }
    throw std::runtime_error("Expressão inesperada");
}

std::unique_ptr<NoAST> ParserBR::parseListaInline() {
    avancarToken(); auto no = criarNo(NO_LISTA_LITERAL);
    if (!verificar(COLCHETE_DIR)) {
        while (!verificar(FIM_DE_ARQUIVO) && !verificar(COLCHETE_DIR)) {
            no->filhos.push_back(expressao());
            if (verificar(VIRGULA)) avancarToken(); else break;
        }
    }
    consumir(COLCHETE_DIR, "Esperava ']'"); return no;
}

std::unique_ptr<NoAST> ParserBR::parseRegistroInline() {
    avancarToken(); auto no = criarNo(NO_REGISTRO_LITERAL);
    if (!verificar(CHAVE_DIR)) {
        while (!verificar(FIM_DE_ARQUIVO) && !verificar(CHAVE_DIR)) {
            Token chave;
            if (verificar(TEXTO) || verificar(IDENTIFICADOR)) chave = avancarToken();
            else { avancarToken(); ErroSintaxe("Esperava chave"); }
            consumir(DOIS_PONTOS, "Esperava ':'");
            auto no2 = criarNo(NO_REGISTRO_LITERAL); no2->nome = chave.valor;
            no2->filhos.push_back(expressao());
            no->filhos.push_back(std::move(no2));
            if (verificar(VIRGULA)) avancarToken(); else break;
        }
    }
    consumir(CHAVE_DIR, "Esperava '}'"); return no;
}

std::unique_ptr<NoAST> ParserBR::parseListaLiteral() {
    avancarToken(); auto no = criarNo(NO_LISTA_LITERAL);
    consumir(INDENTAR, "Esperava indentação após 'lista'");
    while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO)) {
        pularNovasLinhas();
        if (verificar(DESINDENTAR) || verificar(FIM_DE_ARQUIVO)) break;
        no->filhos.push_back(expressao());
    }
    consumir(DESINDENTAR, "Esperava fim da lista");
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseRegistroLiteral() {
    avancarToken(); auto no = criarNo(NO_REGISTRO_LITERAL);
    consumir(INDENTAR, "Esperava indentação após 'registro'");
    while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO)) {
        pularNovasLinhas();
        if (verificar(DESINDENTAR) || verificar(FIM_DE_ARQUIVO)) break;
        Token prop;
        if (verificar(IDENTIFICADOR)) prop = avancarToken();
        else { avancarToken(); ErroSintaxe("Esperava nome da propriedade"); }
        if (verificar(DOIS_PONTOS)) avancarToken();
        auto no2 = criarNo(NO_REGISTRO_LITERAL); no2->nome = prop.valor;
        no2->filhos.push_back(expressao());
        no->filhos.push_back(std::move(no2));
    }
    consumir(DESINDENTAR, "Esperava fim do registro");
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseBloco() {
    auto no = criarNo(NO_BLOCO);
    consumir(INDENTAR, "Esperava indentação");
    while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO)) {
        pularNovasLinhas();
        if (verificar(DESINDENTAR) || verificar(FIM_DE_ARQUIVO)) break;
        no->filhos.push_back(declaracao());
    }
    consumir(DESINDENTAR, "Esperava fim do bloco");
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::declaracao() {
    pularNovasLinhas();
    if (verificar(FIM_DE_ARQUIVO)) return nullptr;
    
    // Verificar decorators
    std::vector<std::string> decorators;
    while (verificar(ARROBA)) {
        avancarToken();
        if (verificar(IDENTIFICADOR)) {
            decorators.push_back(avancarToken().valor);
        } else {
            ErroSintaxe("Esperava nome do decorator após @");
        }
    }
    
    if (verificarPalavra("funcao")) {
        auto no = parseFuncao();
        // Aplicar decorators
        for (auto& dec : decorators) {
            auto decNo = criarNo(NO_DECORATOR);
            decNo->nome = dec;
            decNo->filhos.push_back(std::move(no));
            no = std::move(decNo);
        }
        return no;
    }
    if (verificarPalavra("classe")) return parseClasse();
    if (verificarPalavra("importar")) return parseImportacao();
    if (verificarPalavra("constante")) return parseConstante();
    if (verificarPalavra("se")) return parseSe();
    if (verificarPalavra("enquanto")) return parseEnquanto();
    if (verificarPalavra("para")) return parseParaCada();
    if (verificarPalavra("repita")) return parseRepita();
    if (verificarPalavra("conte")) return parseConte();
    if (verificarPalavra("contexto")) return parseContextManager();
    if (verificarPalavra("tente")) return parseTenteCapture();
    if (verificarPalavra("escolha")) return parseEscolha();
    if (verificarPalavra("execute")) return parseExecuteParalelo();
    if (verificarPalavra("tarefa")) return parseTarefa();
    if (verificarPalavra("explicacao")) return parseExplicacao();
    if (verificarPalavra("objetivo")) return parseObjetivo();
    if (verificarPalavra("teste")) return parseTeste();
    if (verificarPalavra("mostre")) return parseMostre();
    if (verificarPalavra("devolva")) return parseRetorno();
    if (verificarPalavra("salve")) return parseSalve();
    if (verificarPalavra("remova")) return parseRemova();
    if (verificarPalavra("espere")) return parseEspere();
    if (verificarPalavra("aguarde")) return parseAguarde();
    if (verificarPalavra("pergunte")) return parseIAPergunte();
    if (verificarPalavra("crie")) {
        avancarToken();
        if (verificarPalavra("agente")) return parseIAAgente();
        if (verificarPalavra("site") || verificarPalavra("janela")) { /* skip */ return criarNo(NO_EXPRESSAO_STMT); }
        auto no = criarNo(NO_EXPRESSAO_STMT);
        auto id = criarNo(NO_IDENTIFICADOR); id->nome = "crie";
        no->filhos.push_back(std::move(id));
        return no;
    }
    return parseAtribuicaoOuDeclaracao();
}

std::unique_ptr<NoAST> ParserBR::parseFuncao() {
    avancarToken(); auto no = criarNo(NO_FUNCAO);
    no->nome = consumir(IDENTIFICADOR, "Esperava nome da função").valor;
    if (verificarPalavra("recebendo")) {
        avancarToken();
        if (verificar(IDENTIFICADOR)) {
            no->filhos.push_back(primario());
            while (verificarPalavra("e")) { avancarToken(); no->filhos.push_back(primario()); }
        }
    }
    auto bloco = parseBloco();
    for (auto& f : bloco->filhos) no->propriedades[std::to_string(no->propriedades.size())] = std::move(f);
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseClasse() {
    avancarToken(); auto no = criarNo(NO_CLASSE);
    no->nome = consumir(IDENTIFICADOR, "Esperava nome da classe").valor;
    if (verificarPalavra("herda")) { avancarToken(); no->valTexto = consumir(IDENTIFICADOR, "Esperava nome da classe base").valor; }
    auto bloco = parseBloco();
    for (auto& f : bloco->filhos) no->filhos.push_back(std::move(f));
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseImportacao() {
    avancarToken(); auto no = criarNo(NO_IMPORTAR);
    if (verificar(TEXTO) || verificar(IDENTIFICADOR)) no->valTexto = avancarToken().valor;
    if (verificarPalavra("como")) { avancarToken(); no->nome = avancarToken().valor; }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseConstante() {
    avancarToken(); auto nome = consumir(IDENTIFICADOR, "Esperava nome da constante").valor;
    if (verificarPalavra("é")) avancarToken(); else if (verificar(OPERADOR) && atual().valor == "=") avancarToken();
    else ErroSintaxe("Esperava 'é' ou '='");
    auto no = criarNo(NO_CONSTANTE); no->nome = nome; no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseSe() {
    avancarToken(); auto no = criarNo(NO_SE);
    no->filhos.push_back(expressao());
    if (verificarPalavra("entao")) avancarToken();
    no->filhos.push_back(parseBloco());
    if (verificarPalavra("senao")) { avancarToken(); no->filhos.push_back(parseBloco()); }
    else { no->filhos.push_back(nullptr); }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseEnquanto() {
    avancarToken(); auto no = criarNo(NO_ENQUANTO);
    no->filhos.push_back(expressao()); no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseParaCada() {
    avancarToken(); auto no = criarNo(NO_PARA_CADA);
    consumirPalavra("cada", "Esperava 'cada'"); no->nome = consumir(IDENTIFICADOR, "Esperava variável").valor;
    consumirPalavra("em", "Esperava 'em'"); no->filhos.push_back(expressao()); no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseRepita() {
    avancarToken(); auto no = criarNo(NO_REPITA);
    no->filhos.push_back(expressao());
    if (verificarPalavra("vezes")) avancarToken();
    no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseContextManager() {
    avancarToken(); auto no = criarNo(NO_CONTEXT_MANAGER);
    // Formato: contexto variavel é expressao como
    if (verificar(IDENTIFICADOR)) {
        no->nome = avancarToken().valor;
        if (verificarPalavra("é")) avancarToken();
        else if (verificar(OPERADOR) && atual().valor == "=") avancarToken();
        else ErroSintaxe("Esperava 'é' ou '=' após nome do contexto");
        
        no->filhos.push_back(expressao());
        if (verificarPalavra("como")) avancarToken();
        no->filhos.push_back(parseBloco());
    } else {
        ErroSintaxe("Esperava nome do contexto");
    }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseConte() {
    avancarToken(); auto no = criarNo(NO_CONTE);
    consumirPalavra("de", "Esperava 'de'"); no->filhos.push_back(expressao());
    consumirPalavra("ate", "Esperava 'até'"); no->filhos.push_back(expressao());
    if (verificarPalavra("como")) avancarToken();
    no->nome = consumir(IDENTIFICADOR, "Esperava variável do contador").valor;
    no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseTenteCapture() {
    avancarToken(); auto no = criarNo(NO_TENTE_CAPTURE);
    no->filhos.push_back(parseBloco());
    if (verificarPalavra("capture")) {
        avancarToken();
        no->nome = verificar(IDENTIFICADOR) ? avancarToken().valor : "erro";
        no->filhos.push_back(parseBloco());
    }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseEscolha() {
    avancarToken(); auto no = criarNo(NO_ESCOLHA_CASO);
    no->filhos.push_back(expressao());
    consumir(INDENTAR, "Esperava indentação após 'escolha'");
    while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO)) {
        pularNovasLinhas();
        if (verificar(DESINDENTAR) || verificar(FIM_DE_ARQUIVO)) break;
        if (verificarPalavra("caso")) {
            avancarToken();
            auto casoNo = criarNo(NO_ESCOLHA_CASO); casoNo->nome = "caso";
            casoNo->filhos.push_back(expressao());
            while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO) && 
                   !verificar(NOVA_LINHA) && !verificarPalavra("caso") && !verificarPalavra("padrao")) {
                casoNo->filhos.push_back(declaracao());
            }
            no->filhos.push_back(std::move(casoNo));
        } else if (verificarPalavra("padrao")) {
            avancarToken();
            auto padraoNo = criarNo(NO_ESCOLHA_CASO); padraoNo->nome = "padrao";
            while (!verificar(DESINDENTAR) && !verificar(FIM_DE_ARQUIVO) && 
                   !verificar(NOVA_LINHA) && !verificarPalavra("caso")) {
                padraoNo->filhos.push_back(declaracao());
            }
            no->filhos.push_back(std::move(padraoNo));
        } else break;
    }
    consumir(DESINDENTAR, "Esperava fim do escolha");
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseExecuteParalelo() {
    avancarToken(); consumirPalavra("em", "Esperava 'em'"); consumirPalavra("paralelo", "Esperava 'paralelo'");
    auto no = criarNo(NO_EXECUTE_PARALELO); no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseTarefa() {
    avancarToken(); auto no = criarNo(NO_TAREFA);
    no->nome = consumir(IDENTIFICADOR, "Esperava nome da tarefa").valor;
    if (verificarPalavra("recebendo")) {
        avancarToken();
        if (verificar(IDENTIFICADOR)) {
            no->filhos.push_back(primario());
            while (verificarPalavra("e")) { avancarToken(); no->filhos.push_back(primario()); }
        }
    }
    auto bloco = parseBloco();
    for (auto& f : bloco->filhos) no->propriedades[std::to_string(no->propriedades.size())] = std::move(f);
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseExplicacao() {
    avancarToken(); auto no = criarNo(NO_BLOCO);
    while (!verificarPalavra("fim") && !verificar(FIM_DE_ARQUIVO)) { no->valTexto += atual().valor + " "; avancarToken(); }
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseObjetivo() {
    avancarToken(); auto no = criarNo(NO_BLOCO);
    while (!verificarPalavra("fim") && !verificar(FIM_DE_ARQUIVO)) { no->valTexto += atual().valor + " "; avancarToken(); }
    if (verificarPalavra("fim")) avancarToken();
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseTeste() {
    avancarToken(); auto no = criarNo(NO_BLOCO);
    no->nome = consumir(TEXTO, "Esperava nome do teste").valor;
    auto bloco = parseBloco();
    for (auto& f : bloco->filhos) no->filhos.push_back(std::move(f));
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseMostre() {
    avancarToken(); auto no = criarNo(NO_MOSTRE); no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseRetorno() {
    avancarToken(); auto no = criarNo(NO_DEVOLVA);
    if (verificarPalavra("yield")) {
        avancarToken();
        no->tipo = NO_YIELD;
        if (!fimInstrucao()) no->filhos.push_back(expressao());
    } else if (!fimInstrucao()) {
        no->filhos.push_back(expressao());
    }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseSalve() {
    avancarToken(); auto no = criarNo(NO_SALVE); no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseRemova() {
    avancarToken(); auto no = criarNo(NO_REMOVA); no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseEspere() {
    avancarToken(); auto no = criarNo(NO_ESPERE); no->filhos.push_back(expressao());
    if (verificarPalavra("igual")) { avancarToken(); if (verificarPalavra("a")) avancarToken(); no->nome = "igual"; no->filhos.push_back(expressao()); }
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseAguarde() {
    avancarToken(); auto no = criarNo(NO_AGUARDE); no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseIAPergunte() {
    avancarToken(); auto no = criarNo(NO_IA_PERGUNTE);
    if (verificarPalavra("para")) avancarToken();
    consumirPalavra("ia", "Esperava 'ia'"); no->filhos.push_back(expressao());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseIAAgente() {
    avancarToken(); auto no = criarNo(NO_IA_AGENTE);
    no->valTexto = verificar(TEXTO) ? avancarToken().valor : "geral";
    no->filhos.push_back(parseBloco());
    return no;
}

std::unique_ptr<NoAST> ParserBR::parseAtribuicaoOuDeclaracao() {
    if ((verificar(IDENTIFICADOR) && pos + 2 < tokens.size() && 
         tokens[pos + 1].tipo == DOIS_PONTOS && 
         (tokens[pos + 2].tipo == PALAVRA_RESERVADA || tokens[pos + 2].tipo == IDENTIFICADOR))) {
        auto nome = avancarToken().valor; avancarToken(); avancarToken();
        auto no = criarNo(NO_DECLARACAO); no->nome = nome;
        if (verificarPalavra("é")) avancarToken(); else if (verificar(OPERADOR) && atual().valor == "=") avancarToken();
        no->filhos.push_back(expressao()); return no;
    }
    if (verificar(IDENTIFICADOR) && pos + 1 < tokens.size()) {
        Token prox = tokens[pos + 1];
        if ((prox.tipo == PALAVRA_RESERVADA && prox.valor == "é") ||
            (prox.tipo == OPERADOR && prox.valor == "=")) {
            auto nome = avancarToken().valor; avancarToken();
            auto no = criarNo(NO_ATRIBUICAO); no->nome = nome;
            no->filhos.push_back(expressao()); return no;
        }
    }
    if ((verificar(IDENTIFICADOR) || (verificar(PALAVRA_RESERVADA) && atual().valor == "isto"))) {
        size_t posSalva = pos;
        try {
            auto expr = chamada();
            if (verificarPalavra("é") || (verificar(OPERADOR) && atual().valor == "=")) {
                avancarToken(); auto no = criarNo(NO_ATRIBUICAO); no->nome = "__propriedade__";
                no->filhos.push_back(std::move(expr)); no->filhos.push_back(expressao());
                return no;
            }
            auto stmt = criarNo(NO_EXPRESSAO_STMT); stmt->filhos.push_back(std::move(expr));
            return stmt;
        } catch (...) { pos = posSalva; }
    }
    auto no = criarNo(NO_EXPRESSAO_STMT); no->filhos.push_back(expressao());
    return no;
}

// ===================== VM (Máquina Virtual) =====================
class VMBR {
private:
    std::vector<std::map<std::string, Valor>> escopos;
    std::vector<Valor> bancoDados;
    int callDepth;
    std::string diretorioBase;
    
    Valor executarNo(NoAST* no) {
        if (!no) return Valor::criarNulo();
        
        switch (no->tipo) {
            case NO_NUMERO_INT: return Valor::criarInteiro(no->valInteiro);
            case NO_NUMERO_DEC: return Valor::criarDecimal(no->valDecimal);
            case NO_TEXTO_LITERAL: return Valor::criarTexto(no->valTexto);
            case NO_BOOLEANO: return Valor::criarBooleano(no->valBooleano);
            case NO_NULO: return Valor::criarNulo();
            case NO_IDENTIFICADOR: {
                for (auto it = escopos.rbegin(); it != escopos.rend(); ++it) {
                    auto encontrado = it->find(no->nome);
                    if (encontrado != it->end()) return encontrado->second;
                }
                throw ErroRuntime("Variável '" + no->nome + "' não declarada", no->linha, no->coluna);
            }
            case NO_OPERACAO_BINARIA: {
                auto esq = executarNo(no->filhos[0].get());
                auto dir = executarNo(no->filhos[1].get());
                std::string op = no->operador;
                if (op == "+") {
                    if (esq.tipo == VAL_TEXTO || dir.tipo == VAL_TEXTO) return Valor::criarTexto(esq.paraTexto() + dir.paraTexto());
                    if (esq.tipo == VAL_INTEIRO && dir.tipo == VAL_INTEIRO) return Valor::criarInteiro(esq.inteiro + dir.inteiro);
                    return Valor::criarDecimal(esq.paraNumero() + dir.paraNumero());
                }
                if (op == "-") { return esq.tipo == VAL_INTEIRO && dir.tipo == VAL_INTEIRO ? Valor::criarInteiro(esq.inteiro - dir.inteiro) : Valor::criarDecimal(esq.paraNumero() - dir.paraNumero()); }
                if (op == "*") { return esq.tipo == VAL_INTEIRO && dir.tipo == VAL_INTEIRO ? Valor::criarInteiro(esq.inteiro * dir.inteiro) : Valor::criarDecimal(esq.paraNumero() * dir.paraNumero()); }
                if (op == "/") { double d = dir.paraNumero(); if (d == 0.0) throw ErroRuntime("Divisão por zero", no->linha, no->coluna); return Valor::criarDecimal(esq.paraNumero() / d); }
                if (op == "==") return Valor::criarBooleano(esq.paraTexto() == dir.paraTexto());
                if (op == "!=") return Valor::criarBooleano(esq.paraTexto() != dir.paraTexto());
                if (op == ">") return Valor::criarBooleano(esq.paraNumero() > dir.paraNumero());
                if (op == "<") return Valor::criarBooleano(esq.paraNumero() < dir.paraNumero());
                if (op == ">=") return Valor::criarBooleano(esq.paraNumero() >= dir.paraNumero());
                if (op == "<=") return Valor::criarBooleano(esq.paraNumero() <= dir.paraNumero());
                throw ErroRuntime("Operador desconhecido: " + op, no->linha, no->coluna);
            }
            case NO_OPERACAO_UNARIA: {
                auto val = executarNo(no->filhos[0].get());
                if (no->operador == "-") return val.tipo == VAL_INTEIRO ? Valor::criarInteiro(-val.inteiro) : Valor::criarDecimal(-val.paraNumero());
                if (no->operador == "nao") return Valor::criarBooleano(!val.paraBooleano());
                throw ErroRuntime("Operador unário desconhecido: " + no->operador, no->linha, no->coluna);
            }
            case NO_ATRIBUICAO: {
                if (no->nome == "__propriedade__") {
                    auto alvo = executarNo(no->filhos[0].get());
                    auto valor = executarNo(no->filhos[1].get());
                    if (no->filhos[0]->tipo == NO_ACESSO_PROPRIEDADE) {
                        std::string prop = no->filhos[0]->valTexto;
                        if (alvo.tipo == VAL_REGISTRO) alvo.registro[prop] = valor;
                    }
                    return valor;
                }
                auto valor = executarNo(no->filhos[0].get());
                definirVariavel(no->nome, valor);
                return valor;
            }
            case NO_DECLARACAO: case NO_CONSTANTE: {
                auto valor = executarNo(no->filhos[0].get());
                definirVariavel(no->nome, valor);
                return valor;
            }
            case NO_LISTA_LITERAL: {
                auto lista = Valor::criarLista();
                for (auto& f : no->filhos) lista.lista.push_back(executarNo(f.get()));
                return lista;
            }
            case NO_LISTA_COMPREHENSION: {
                auto lista = Valor::criarLista();
                auto colecao = executarNo(no->filhos[1].get());
                if (colecao.tipo == VAL_LISTA) {
                    for (auto& item : colecao.lista) {
                        escopos.push_back({});
                        definirVariavel(no->nome, item);
                        bool incluir = true;
                        if (no->filhos.size() >= 3) {
                            try { incluir = executarNo(no->filhos[2].get()).paraBooleano(); }
                            catch (...) { incluir = false; }
                        }
                        if (incluir) lista.lista.push_back(executarNo(no->filhos[0].get()));
                        escopos.pop_back();
                    }
                }
                return lista;
            }
            case NO_REGISTRO_LITERAL: {
                auto reg = Valor::criarRegistro();
                for (auto& f : no->filhos) {
                    if (f->filhos.empty()) reg.registro[f->nome] = Valor::criarNulo();
                    else reg.registro[f->nome] = executarNo(f->filhos[0].get());
                }
                return reg;
            }
            case NO_ACESSO_PROPRIEDADE: {
                auto obj = executarNo(no->filhos[0].get());
                std::string prop = no->valTexto;
                if (obj.tipo == VAL_REGISTRO) {
                    auto it = obj.registro.find(prop);
                    return it != obj.registro.end() ? it->second : Valor::criarNulo();
                }
                if (obj.tipo == VAL_LISTA) { try { return obj.lista[std::stoi(prop)]; } catch (...) { return Valor::criarNulo(); } }
                if (obj.tipo == VAL_MODULO && obj.modulo) {
                    auto it = obj.modulo->exportacoes.find(prop);
                    if (it != obj.modulo->exportacoes.end()) return it->second;
                }
                throw ErroRuntime("Propriedade '" + prop + "' não encontrada", no->linha, no->coluna);
            }
            case NO_ACESSO_INDICE: {
                auto obj = executarNo(no->filhos[0].get());
                auto idx = executarNo(no->filhos[1].get());
                int i = (int)idx.paraNumero();
                if (obj.tipo == VAL_LISTA) {
                    if (i < 0 || i >= (int)obj.lista.size()) throw ErroRuntime("Índice fora da lista", no->linha, no->coluna);
                    return obj.lista[i];
                }
                return Valor::criarNulo();
            }
            case NO_CHAMADA_FUNCAO: {
                if (no->nome == "__parenteses__") {
                    auto obj = executarNo(no->filhos[0].get());
                    if (no->filhos[0]->tipo == NO_ACESSO_PROPRIEDADE) {
                        std::string metodoNome = no->filhos[0]->valTexto;
                        std::vector<Valor> args;
                        for (size_t i = 1; i < no->filhos.size(); i++) args.push_back(executarNo(no->filhos[i].get()));
                        return chamarMetodoObjeto(obj, metodoNome, args, no->linha);
                    }
                    throw ErroRuntime("Chamada inválida com parênteses", no->linha, no->coluna);
                }
                std::vector<Valor> args;
                for (auto& f : no->filhos) args.push_back(executarNo(f.get()));
                return chamarFuncao(no->nome, args, no->linha);
            }
            case NO_CHAMADA_METODO: {
                auto obj = executarNo(no->filhos[0].get());
                std::vector<Valor> args;
                for (size_t i = 1; i < no->filhos.size(); i++) args.push_back(executarNo(no->filhos[i].get()));
                return chamarMetodoObjeto(obj, no->nome, args, no->linha);
            }
            case NO_MOSTRE: {
                auto val = executarNo(no->filhos[0].get());
                std::cout << val.paraTexto() << std::endl;
                return Valor::criarNulo();
            }
            case NO_YIELD: {
                // Yield pausa a função e retorna um valor
                if (!no->filhos.empty()) {
                    auto val = executarNo(no->filhos[0].get());
                    // Em um generator real, isso pausaria a execução
                    // Por agora, retornamos o valor
                    return val;
                }
                return Valor::criarNulo();
            }
            case NO_DEVOLVA: {
                if (no->filhos.empty()) return Valor::criarNulo();
                return executarNo(no->filhos[0].get());
            }
            case NO_SALVE: {
                auto val = executarNo(no->filhos[0].get());
                bancoDados.push_back(val);
                std::cout << "[Banco de Dados BR] Registro salvo: " << val.paraTexto() << std::endl;
                return Valor::criarNulo();
            }
            case NO_REMOVA: {
                auto val = executarNo(no->filhos[0].get());
                for (auto it = bancoDados.begin(); it != bancoDados.end(); ++it) {
                    if (it->paraTexto() == val.paraTexto()) { bancoDados.erase(it); break; }
                }
                return Valor::criarNulo();
            }
            case NO_SE: {
                return executarNo(no->filhos[0].get()).paraBooleano() ? executarBloco(no->filhos[1].get()) : 
                       (no->filhos[2] ? executarBloco(no->filhos[2].get()) : Valor::criarNulo());
            }
            case NO_ENQUANTO: {
                Valor resultado;
                while (executarNo(no->filhos[0].get()).paraBooleano())
                    resultado = executarBloco(no->filhos[1].get());
                return resultado;
            }
            case NO_PARA_CADA: {
                auto lista = executarNo(no->filhos[0].get());
                Valor resultado;
                if (lista.tipo == VAL_LISTA) {
                    for (auto& item : lista.lista) {
                        escopos.push_back({});
                        definirVariavel(no->nome, item);
                        resultado = executarBloco(no->filhos[1].get());
                        escopos.pop_back();
                    }
                }
                return resultado;
            }
            case NO_REPITA: {
                auto vezes = executarNo(no->filhos[0].get());
                int n = (int)vezes.paraNumero();
                for (int i = 0; i < n; i++) executarBloco(no->filhos[1].get());
                return Valor::criarNulo();
            }
            case NO_CONTE: {
                auto inicio = executarNo(no->filhos[0].get());
                auto fim = executarNo(no->filhos[1].get());
                Valor resultado;
                for (int i = (int)inicio.paraNumero(); i <= (int)fim.paraNumero(); i++) {
                    escopos.push_back({});
                    definirVariavel(no->nome, Valor::criarInteiro(i));
                    resultado = executarBloco(no->filhos[2].get());
                    escopos.pop_back();
                }
                return resultado;
            }
            case NO_ESCOLHA_CASO: {
                auto expr = executarNo(no->filhos[0].get());
                bool executou = false;
                for (size_t i = 1; i < no->filhos.size(); i++) {
                    auto caso = no->filhos[i].get();
                    if (caso->nome == "caso") {
                        auto valCaso = executarNo(caso->filhos[0].get());
                        if (expr.paraTexto() == valCaso.paraTexto()) {
                            for (size_t j = 1; j < caso->filhos.size(); j++) executarNo(caso->filhos[j].get());
                            executou = true; break;
                        }
                    } else if (caso->nome == "padrao" && !executou) {
                        for (auto& f : caso->filhos) executarNo(f.get());
                        executou = true; break;
                    }
                }
                return Valor::criarNulo();
            }
            case NO_TENTE_CAPTURE: {
                try { return executarBloco(no->filhos[0].get()); }
                catch (const std::exception& e) {
                    escopos.push_back({}); definirVariavel(no->nome, Valor::criarTexto(e.what()));
                    auto r = executarBloco(no->filhos[1].get()); escopos.pop_back(); return r;
                }
            }
            case NO_ESPERE: return executarNo(no->filhos[0].get());
            case NO_AGUARDE: return executarNo(no->filhos[0].get());
            case NO_IA_PERGUNTE: return chamarServicoNativo("IA", "PERGUNTAR", {executarNo(no->filhos[0].get())});
            case NO_DECORATOR: {
                // Executar a função decorada
                auto funcNo = no->filhos[0].get();
                if (funcNo->tipo == NO_FUNCAO) {
                    auto fn = std::make_shared<FuncaoBR>(funcNo->nome);
                    for (auto& f : funcNo->filhos) if (f->tipo == NO_IDENTIFICADOR) fn->parametros.push_back(f->nome);
                    fn->corpoAST = funcNo;
                    auto funcVal = Valor::criarFuncao(fn);
                    
                    // Buscar o decorator
                    Valor decoradorVal;
                    for (auto it = escopos.rbegin(); it != escopos.rend(); ++it) {
                        auto encontrado = it->find(no->nome);
                        if (encontrado != it->end()) { decoradorVal = encontrado->second; break; }
                    }
                    
                    if (decoradorVal.tipo == VAL_FUNCAO && decoradorVal.funcao) {
                        // Aplicar decorator: decorador(original)
                        std::vector<Valor> args = {funcVal};
                        Valor resultado = chamarFuncao(no->nome, args, no->linha);
                        if (resultado.tipo == VAL_FUNCAO) {
                            definirVariavel(funcNo->nome, resultado);
                            return resultado;
                        }
                    }
                    
                    // Se não encontrou decorator, retorna função original
                    definirVariavel(funcNo->nome, funcVal);
                    return funcVal;
                }
                return Valor::criarNulo();
            }
            case NO_FUNCAO: {
                auto fn = std::make_shared<FuncaoBR>(no->nome);
                for (auto& f : no->filhos) if (f->tipo == NO_IDENTIFICADOR) fn->parametros.push_back(f->nome);
                fn->corpoAST = no;
                definirVariavel(no->nome, Valor::criarFuncao(fn));
                return Valor::criarNulo();
            }
            case NO_CLASSE: {
                auto cls = std::make_shared<ClasseBR>(no->nome);
                cls->classeMae = no->valTexto;
                for (auto& f : no->filhos) {
                    if (f->tipo == NO_IDENTIFICADOR) cls->campos.push_back(f->nome);
                    if (f->tipo == NO_FUNCAO) {
                        auto fn = std::make_shared<FuncaoBR>(f->nome);
                        for (auto& p : f->filhos) if (p->tipo == NO_IDENTIFICADOR) fn->parametros.push_back(p->nome);
                        fn->corpoAST = f.get();
                        cls->metodos[f->nome] = fn;
                    }
                }
                definirVariavel(no->nome, Valor::criarClasse(cls));
                return Valor::criarNulo();
            }
            case NO_IMPORTAR: return importarModulo(no->valTexto, no->nome);
            case NO_TAREFA: return Valor::criarNulo();
            case NO_CONTEXT_MANAGER: {
                // Context manager: contexto var é expressao como bloco
                auto contextoVal = executarNo(no->filhos[0].get());
                definirVariavel(no->nome, contextoVal);
                // Executar o bloco do contexto
                auto resultado = executarBloco(no->filhos[1].get());
                return resultado;
            }
            case NO_EXECUTE_PARALELO: {
                std::vector<std::thread> threads;
                std::vector<Valor> resultados;
                resultados.resize(no->filhos[0]->filhos.size());
                size_t idx = 0;
                for (auto& f : no->filhos[0]->filhos) {
                    threads.emplace_back([&resultados, idx, &f, this]() {
                        try { resultados[idx] = executarNo(f.get()); }
                        catch (...) { resultados[idx] = Valor::criarNulo(); }
                    });
                    idx++;
                }
                for (auto& t : threads) t.join();
                return Valor::criarNulo();
            }
            case NO_EXPRESSAO_STMT: return no->filhos.empty() ? Valor::criarNulo() : executarNo(no->filhos[0].get());
            case NO_BLOCO: return executarBloco(no);
            case NO_PROGRAMA: { Valor r; for (auto& f : no->filhos) r = executarNo(f.get()); return r; }
            default: return Valor::criarNulo();
        }
    }
    
    Valor executarBloco(NoAST* no) {
        if (!no) return Valor::criarNulo();
        escopos.push_back({}); Valor resultado;
        for (auto& f : no->filhos) resultado = executarNo(f.get());
        escopos.pop_back();
        return resultado;
    }
    
    void definirVariavel(const std::string& nome, const Valor& val) {
        escopos.back()[nome] = val;
    }
    
    Valor chamarFuncao(const std::string& nome, std::vector<Valor>& args, int linha) {
        if (nome == "leia_arquivo" && args.size() >= 1) return Valor::criarTexto(ServicoArquivo::ler(args[0].paraTexto()));
        if (nome == "tipo_de" && args.size() >= 1) {
            static const std::map<TipoValor, std::string> nomes = {
                {VAL_INTEIRO, "inteiro"}, {VAL_DECIMAL, "decimal"}, {VAL_TEXTO, "texto"},
                {VAL_BOOLEANO, "booleano"}, {VAL_NULO, "nulo"}, {VAL_LISTA, "lista"}, {VAL_REGISTRO, "registro"}
            };
            auto it = nomes.find(args[0].tipo);
            return Valor::criarTexto(it != nomes.end() ? it->second : "desconhecido");
        }
        if (nome == "para_inteiro") return Valor::criarInteiro((int64_t)args[0].paraNumero());
        if (nome == "para_decimal") return Valor::criarDecimal(args[0].paraNumero());
        if (nome == "tamanho_lista") return Valor::criarInteiro((int64_t)args[0].lista.size());
        if (nome == "texto_contem" && args.size() >= 2) return Valor::criarBooleano(args[0].paraTexto().find(args[1].paraTexto()) != std::string::npos);
        if (nome == "texto_substituir" && args.size() >= 3) {
            std::string txt = args[0].paraTexto(), antigo = args[1].paraTexto(), novo = args[2].paraTexto();
            size_t pos = 0;
            while ((pos = txt.find(antigo, pos)) != std::string::npos) { txt.replace(pos, antigo.length(), novo); pos += novo.length(); }
            return Valor::criarTexto(txt);
        }
        if (nome == "acesse" && args.size() >= 1) {
            RespostaHTTP resp = ServicoRede::http_get(args[0].paraTexto());
            return resp.falhou ? Valor::criarTexto("[Erro] " + resp.erro) : Valor::criarTexto(resp.conteudo);
        }
        
        // ==== SQLite Real ====
        if (nome == "sqlite_executar" && args.size() >= 1) {
            std::string query = args[0].paraTexto();
            std::string banco = args.size() >= 2 ? args[1].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLite(query, banco));
        }
        if (nome == "sqlite_consultar" && args.size() >= 1) {
            std::string query = args[0].paraTexto();
            std::string banco = args.size() >= 2 ? args[1].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLite(query, banco));
        }
        if (nome == "sqlite_json" && args.size() >= 1) {
            std::string query = args[0].paraTexto();
            std::string banco = args.size() >= 2 ? args[1].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLiteJSON(query, banco));
        }
        if (nome == "criar_tabela" && args.size() >= 2) {
            std::string nome_tabela = args[0].paraTexto();
            std::string definicao = args[1].paraTexto();
            std::string banco = args.size() >= 3 ? args[2].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLite("CREATE TABLE IF NOT EXISTS " + nome_tabela + " (" + definicao + ")", banco));
        }
        if (nome == "inserir_registro" && args.size() >= 2) {
            std::string tabela = args[0].paraTexto();
            std::string banco = args.size() >= 3 ? args[2].paraTexto() : ":memory:";
            if (args[1].tipo == VAL_REGISTRO) {
                std::stringstream ss;
                ss << "INSERT INTO " << tabela << " (";
                std::stringstream valores;
                bool primeiro = true;
                for (auto& [coluna, valor] : args[1].registro) {
                    if (!primeiro) { ss << ", "; valores << ", "; }
                    ss << coluna;
                    valores << "'" << valor.paraTexto() << "'";
                    primeiro = false;
                }
                ss << ") VALUES (" << valores.str() << ")";
                return Valor::criarTexto(executarSQLite(ss.str(), banco));
            }
            return Valor::criarTexto("[SQLite] Segundo argumento deve ser um registro");
        }
        if (nome == "buscar_registros" && args.size() >= 1) {
            std::string tabela = args[0].paraTexto();
            std::string condicao = args.size() >= 2 ? args[1].paraTexto() : "";
            std::string banco = args.size() >= 3 ? args[2].paraTexto() : ":memory:";
            std::string query = "SELECT * FROM " + tabela;
            if (!condicao.empty()) query += " WHERE " + condicao;
            return Valor::criarTexto(executarSQLite(query, banco));
        }
        if (nome == "atualizar_registro" && args.size() >= 3) {
            std::string tabela = args[0].paraTexto();
            std::string condicao = args[1].paraTexto();
            std::string banco = args.size() >= 4 ? args[3].paraTexto() : ":memory:";
            if (args[2].tipo == VAL_REGISTRO) {
                std::stringstream ss;
                ss << "UPDATE " << tabela << " SET ";
                bool primeiro = true;
                for (auto& [coluna, valor] : args[2].registro) {
                    if (!primeiro) ss << ", ";
                    ss << coluna << " = '" << valor.paraTexto() << "'";
                    primeiro = false;
                }
                if (!condicao.empty()) ss << " WHERE " << condicao;
                return Valor::criarTexto(executarSQLite(ss.str(), banco));
            }
            return Valor::criarTexto("[SQLite] Terceiro argumento deve ser um registro");
        }
        if (nome == "remover_registro" && args.size() >= 1) {
            std::string tabela = args[0].paraTexto();
            std::string condicao = args.size() >= 2 ? args[1].paraTexto() : "";
            std::string banco = args.size() >= 3 ? args[2].paraTexto() : ":memory:";
            std::string query = "DELETE FROM " + tabela;
            if (!condicao.empty()) query += " WHERE " + condicao;
            return Valor::criarTexto(executarSQLite(query, banco));
        }
        if (nome == "listar_tabelas") {
            std::string banco = args.size() >= 1 ? args[0].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLite("SELECT name FROM sqlite_master WHERE type='table'", banco));
        }
        if (nome == "descrever_tabela" && args.size() >= 1) {
            std::string tabela = args[0].paraTexto();
            std::string banco = args.size() >= 2 ? args[1].paraTexto() : ":memory:";
            return Valor::criarTexto(executarSQLite("PRAGMA table_info(" + tabela + ")", banco));
        }
        if (nome == "fechar_banco" && args.size() >= 1) {
            fecharBanco(args[0].paraTexto());
            return Valor::criarTexto("[SQLite] Banco fechado");
        }
        
        // ==== HTTP Server ====
        if (nome == "servico_http" && args.size() >= 1) {
            int porta = (int)args[0].paraNumero();
            int id = ServicoRede::criar_servidor(porta);
            return Valor::criarInteiro(id);
        }
        if (nome == "servidor_iniciar" && args.size() >= 1) {
            int id = (int)args[0].paraNumero();
            bool ok = ServicoRede::iniciar_servidor(id);
            return Valor::criarBooleano(ok);
        }
        if (nome == "servidor_parar" && args.size() >= 1) {
            int id = (int)args[0].paraNumero();
            bool ok = ServicoRede::parar_servidor(id);
            return Valor::criarBooleano(ok);
        }
        if (nome == "servidor_rodando" && args.size() >= 1) {
            int id = (int)args[0].paraNumero();
            bool ok = ServicoRede::servidor_rodando(id);
            return Valor::criarBooleano(ok);
        }
        
        Valor funcVal;
        for (auto it = escopos.rbegin(); it != escopos.rend(); ++it) {
            auto encontrado = it->find(nome);
            if (encontrado != it->end()) { funcVal = encontrado->second; break; }
        }
        
        if (funcVal.tipo == VAL_FUNCAO && funcVal.funcao && funcVal.funcao->corpoAST) {
            auto fn = funcVal.funcao; callDepth++;
            escopos.push_back({});
            for (size_t i = 0; i < fn->parametros.size() && i < args.size(); i++)
                definirVariavel(fn->parametros[i], args[i]);
            Valor resultado;
            NoAST* corpoNo = fn->corpoAST;
            if (corpoNo->tipo == NO_FUNCAO) {
                for (size_t i = 0; i < corpoNo->propriedades.size(); i++) {
                    auto& par = corpoNo->propriedades[std::to_string(i)];
                    if (par) resultado = executarNo(par.get());
                }
                for (auto& f : corpoNo->filhos)
                    if (f->tipo != NO_IDENTIFICADOR) resultado = executarNo(f.get());
            }
            escopos.pop_back(); callDepth--;
            return resultado;
        }
        throw ErroRuntime("Função '" + nome + "' não encontrada", linha);
    }
    
    Valor chamarMetodoObjeto(Valor& obj, const std::string& metodo, std::vector<Valor>& args, int linha) {
        if (obj.tipo == VAL_TEXTO) {
            if (metodo == "tamanho" || metodo == "comprimento") return Valor::criarInteiro((int64_t)obj.texto.size());
            if (metodo == "contem" && args.size() >= 1) return Valor::criarBooleano(obj.texto.find(args[0].paraTexto()) != std::string::npos);
            if (metodo == "substituir" && args.size() >= 2) {
                std::string r = obj.texto; size_t pos = 0;
                while ((pos = r.find(args[0].paraTexto(), pos)) != std::string::npos) { r.replace(pos, args[0].paraTexto().length(), args[1].paraTexto()); pos += args[0].paraTexto().length(); }
                return Valor::criarTexto(r);
            }
        }
        if (obj.tipo == VAL_LISTA) {
            if (metodo == "tamanho" || metodo == "comprimento") return Valor::criarInteiro((int64_t)obj.lista.size());
            if (metodo == "adicionar" || metodo == "adicione") { if (!args.empty()) obj.lista.push_back(args[0]); return obj; }
            if (metodo == "obter" && !args.empty()) {
                int idx = (int)args[0].paraNumero();
                return (idx >= 0 && idx < (int)obj.lista.size()) ? obj.lista[idx] : Valor::criarNulo();
            }
            if (metodo == "primeiro") return obj.lista.empty() ? Valor::criarNulo() : obj.lista[0];
            if (metodo == "ultimo") return obj.lista.empty() ? Valor::criarNulo() : obj.lista.back();
        }
        if (obj.tipo == VAL_REGISTRO) {
            if (metodo == "obter" && !args.empty()) { auto it = obj.registro.find(args[0].paraTexto()); return it != obj.registro.end() ? it->second : Valor::criarNulo(); }
            if (metodo == "definir" && args.size() >= 2) { obj.registro[args[0].paraTexto()] = args[1]; return obj; }
            if (metodo == "tamanho" || metodo == "comprimento") return Valor::criarInteiro((int64_t)obj.registro.size());
            if (metodo == "chaves") { auto lista = Valor::criarLista(); for (auto& [k, v] : obj.registro) lista.lista.push_back(Valor::criarTexto(k)); return lista; }
            if (metodo == "valores") { auto lista = Valor::criarLista(); for (auto& [k, v] : obj.registro) lista.lista.push_back(v); return lista; }
        }
        if (obj.tipo == VAL_CLASSE && obj.classe) {
            auto it = obj.classe->metodos.find(metodo);
            if (it != obj.classe->metodos.end() && it->second->corpoAST) {
                auto fn = it->second;
                escopos.push_back({});
                definirVariavel("isto", obj);
                for (size_t i = 0; i < fn->parametros.size() && i < args.size(); i++)
                    definirVariavel(fn->parametros[i], args[i]);
                Valor resultado;
                NoAST* corpoNo = fn->corpoAST;
                if (corpoNo->tipo == NO_FUNCAO) {
                    for (size_t i = 0; i < corpoNo->propriedades.size(); i++) {
                        auto& par = corpoNo->propriedades[std::to_string(i)];
                        if (par) resultado = executarNo(par.get());
                    }
                    for (auto& f : corpoNo->filhos)
                        if (f->tipo != NO_IDENTIFICADOR) resultado = executarNo(f.get());
                }
                escopos.pop_back();
                return resultado;
            }
        }
        if (obj.tipo == VAL_MODULO && obj.modulo) {
            auto it = obj.modulo->exportacoes.find(metodo);
            if (it != obj.modulo->exportacoes.end() && it->second.tipo == VAL_FUNCAO)
                return it->second;
        }
        throw ErroRuntime("Método '" + metodo + "' não encontrado no objeto", linha);
    }
    
    Valor importarModulo(const std::string& nome, const std::string& alias) {
        std::string nomeReal = nome, nomeLocal = alias.empty() ? nome : alias;
        if (nomeReal.size() > 3 && nomeReal.substr(nomeReal.size() - 3) == ".br") nomeReal = nomeReal.substr(0, nomeReal.size() - 3);
        if (nomeReal.size() >= 2 && nomeReal[0] == '"' && nomeReal[nomeReal.size()-1] == '"') nomeReal = nomeReal.substr(1, nomeReal.size() - 2);
        
        if (nomeReal == "arquivo") {
            auto mod = std::make_shared<ModuloBR>("arquivo");
            definirVariavel(nomeLocal, Valor::criarModulo(mod));
            return Valor::criarNulo();
        }
        
        std::vector<std::string> caminhos = {
            nomeReal + ".br", "lib/" + nomeReal + ".br", "../lib/" + nomeReal + ".br",
            diretorioBase + "/" + nomeReal + ".br", diretorioBase + "/lib/" + nomeReal + ".br"
        };
        
        for (auto& caminho : caminhos) {
            if (ServicoArquivo::existe(caminho)) {
                try {
                    std::string codigo = ServicoArquivo::ler(caminho);
                    LexerBR lexer(codigo); auto tokens = lexer.tokenizar();
                    ParserBR parser(tokens); auto ast = parser.parse();
                    auto mod = std::make_shared<ModuloBR>(nomeReal); mod->caminho = caminho;
                    for (auto& f : ast->filhos) {
                        if (f->tipo == NO_FUNCAO) {
                            auto fn = std::make_shared<FuncaoBR>(f->nome);
                            for (auto& p : f->filhos) if (p->tipo == NO_IDENTIFICADOR) fn->parametros.push_back(p->nome);
                            fn->corpoAST = f.get();
                            mod->exportacoes[f->nome] = Valor::criarFuncao(fn);
                        }
                    }
                    definirVariavel(nomeLocal, Valor::criarModulo(mod));
                    return Valor::criarNulo();
                } catch (...) { return Valor::criarNulo(); }
            }
        }
        return Valor::criarNulo();
    }
    
    Valor chamarServicoNativo(const std::string& servico, const std::string& metodo, const std::vector<Valor>& args) {
        if (servico == "SISTEMA" && metodo == "MOSTRAR") {
            std::cout << args[0].paraTexto() << std::endl; return Valor::criarNulo();
        }
        if (servico == "IA" && metodo == "PERGUNTAR") {
            std::string p = args[0].paraTexto();
            std::string pl = p; std::transform(pl.begin(), pl.end(), pl.begin(), ::tolower);
            if (pl.find("buraco negro") != std::string::npos) return Valor::criarTexto("Um buraco negro é uma região do espaço-tempo de onde nada, nem mesmo a luz, pode escapar.");
            if (pl.find("brasil") != std::string::npos) return Valor::criarTexto("O Brasil foi descoberto por Pedro Álvares Cabral em 22 de abril de 1500.");
            if (pl.find("capital") != std::string::npos && pl.find("frança") != std::string::npos) return Valor::criarTexto("A capital da França é Paris.");
            if (pl.find("capital") != std::string::npos && pl.find("japão") != std::string::npos) return Valor::criarTexto("A capital do Japão é Tóquio.");
            if (pl.find("programação") != std::string::npos || pl.find("programacao") != std::string::npos) return Valor::criarTexto("Programação é o processo de criar instruções para computadores executarem.");
            return Valor::criarTexto("[IA BR] Resposta para: '" + p + "'. (IA nativa em desenvolvimento)");
        }
        if (servico == "BANCO" && metodo == "EXECUTAR") {
            // SQLite nativo real
            std::string query = args[0].paraTexto();
            std::string resultado = executarSQLite(query);
            return Valor::criarTexto(resultado);
        }
        if (servico == "GUI") {
            static ServicoGUI gui;
            if (metodo == "CRIAR_JANELA" && args.size() >= 3) {
                gui.criarJanela(args[0].paraTexto(), (int)args[1].paraNumero(), (int)args[2].paraNumero());
                return Valor::criarNulo();
            }
            if (metodo == "ADICIONAR_BOTAO" && args.size() >= 4) {
                std::string id = args[0].paraTexto();
                std::string texto = args[1].paraTexto();
                int x = (int)args[2].paraNumero();
                int y = (int)args[3].paraNumero();
                gui.adicionarBotao(id, texto, x, y, []() {
                    std::cout << "[GUI] Botão clicado!" << std::endl;
                });
                return Valor::criarNulo();
            }
            if (metodo == "ADICIONAR_CAMPO" && args.size() >= 4) {
                gui.adicionarCampo(args[0].paraTexto(), args[1].paraTexto(), 
                                  (int)args[2].paraNumero(), (int)args[3].paraNumero(), 
                                  args.size() >= 5 ? (int)args[4].paraNumero() : 200);
                return Valor::criarNulo();
            }
            if (metodo == "MOSTRAR") {
                gui.mostrar();
                return Valor::criarNulo();
            }
        }
        throw ErroRuntime("Serviço nativo '" + servico + ":" + metodo + "' não implementado");
    }

public:
    VMBR(const std::string& baseDir = "") : callDepth(0), diretorioBase(baseDir) {
        escopos.push_back({});
    }
    
    Valor executar(NoAST* ast) {
        try { return executarNo(ast); }
        catch (const ErroRuntime& e) {
            std::cerr << "Erro [Linha " << e.linha << "]: " << e.mensagem << std::endl;
            throw;
        }
        catch (const std::exception& e) { std::cerr << "Erro: " << e.what() << std::endl; throw; }
    }
};

// ===================== HTTP SERVER =====================
class ServidorHTTP {
private:
    int porta;
    int socketServidor;
    bool rodando;
    std::map<std::string, std::function<std::string(const std::string&)>> rotas;
    
public:
    ServidorHTTP(int p) : porta(p), socketServidor(-1), rodando(false) {}
    
    void obter(const std::string& caminho, std::function<std::string(const std::string&)> handler) {
        rotas[caminho] = handler;
    }
    
    void postar(const std::string& caminho, std::function<std::string(const std::string&)> handler) {
        rotas[caminho] = handler;
    }
    
    void iniciar() {
        #ifdef _WIN32
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        
        socketServidor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketServidor < 0) {
            std::cerr << "Erro ao criar socket" << std::endl;
            return;
        }
        
        sockaddr_in endereco;
        endereco.sin_family = AF_INET;
        endereco.sin_addr.s_addr = INADDR_ANY;
        endereco.sin_port = htons(porta);
        
        if (bind(socketServidor, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
            std::cerr << "Erro ao fazer bind na porta " << porta << std::endl;
            return;
        }
        
        listen(socketServidor, 5);
        rodando = true;
        std::cout << "Servidor HTTP rodando em http://localhost:" << porta << std::endl;
        
        while (rodando) {
            sockaddr_in clienteEndereco;
            socklen_t clienteTamanho = sizeof(clienteEndereco);
            int socketCliente = accept(socketServidor, (struct sockaddr*)&clienteEndereco, &clienteTamanho);
            
            if (socketCliente < 0) continue;
            
            std::string requisicao = lerRequisicao(socketCliente);
            std::string resposta = processarRequisicao(requisicao);
            enviarResposta(socketCliente, resposta);
            
            #ifdef _WIN32
                closesocket(socketCliente);
            #else
                close(socketCliente);
            #endif
        }
    }
    
    void parar() {
        rodando = false;
        if (socketServidor >= 0) {
            #ifdef _WIN32
                closesocket(socketServidor);
            #else
                close(socketServidor);
            #endif
        }
    }
    
private:
    std::string lerRequisicao(int socket) {
        char buffer[4096] = {0};
        #ifdef _WIN32
            recv(socket, buffer, sizeof(buffer) - 1, 0);
        #else
            read(socket, buffer, sizeof(buffer) - 1);
        #endif
        return std::string(buffer);
    }
    
    void enviarResposta(int socket, const std::string& resposta) {
        #ifdef _WIN32
            send(socket, resposta.c_str(), resposta.size(), 0);
        #else
            write(socket, resposta.c_str(), resposta.size());
        #endif
    }
    
    std::string processarRequisicao(const std::string& req) {
        std::stringstream ss(req);
        std::string metodo, caminho, protocolo;
        ss >> metodo >> caminho >> protocolo;
        
        std::string corpo;
        std::string linha;
        bool temCorpo = false;
        while (std::getline(ss, linha)) {
            if (linha == "\r" || linha.empty()) {
                temCorpo = true;
                continue;
            }
            if (temCorpo) {
                corpo += linha + "\n";
            }
        }
        
        std::string resposta;
        auto it = rotas.find(caminho);
        if (it != rotas.end()) {
            std::string corpoResposta = it->second(corpo);
            resposta = "HTTP/1.1 200 OK\r\n";
            resposta += "Content-Type: text/html; charset=utf-8\r\n";
            resposta += "Content-Length: " + std::to_string(corpoResposta.size()) + "\r\n";
            resposta += "Connection: close\r\n\r\n";
            resposta += corpoResposta;
        } else {
            resposta = "HTTP/1.1 404 Not Found\r\n";
            resposta += "Content-Type: text/html; charset=utf-8\r\n\r\n";
            resposta += "<h1>404 - Página não encontrada</h1>";
        }
        
        return resposta;
    }
};

void executarREPL() {
    std::cout << "Runtime BR Nativo v" << VERSAO_BR << " - Modo Interativo" << std::endl;
    std::cout << "Digite código BR (CTRL+C para sair)" << std::endl;
    VMBR vm;
    std::string linha, codigo;
    while (true) {
        std::cout << (codigo.empty() ? "> " : "... ");
        if (!std::getline(std::cin, linha)) break;
        if (linha == "sair" || linha == "exit" || linha == "quit") break;
        if (linha.empty() || linha == "fim") {
            if (codigo.empty()) continue;
            try {
                LexerBR lexer(codigo); auto tokens = lexer.tokenizar();
                ParserBR parser(tokens); auto ast = parser.parse();
                if (parser.temErros()) { for (auto& e : parser.getErros()) std::cerr << e << std::endl; }
                else { Valor resultado = vm.executar(ast.get()); if (resultado.tipo != VAL_NULO && resultado.tipo != VAL_FUNCAO && resultado.tipo != VAL_CLASSE) std::cout << "=> " << resultado.paraTexto() << std::endl; }
            } catch (const std::exception& e) { std::cerr << "Erro: " << e.what() << std::endl; }
            codigo.clear();
        } else { if (!codigo.empty()) codigo += "\n"; codigo += linha; }
    }
}

int main(int argc, char* argv[]) {
    try {
        // Inicializa sistemas de performance v3.0
        inicializar_sistemas_performance();
        
        if (argc < 2) {
            std::cout << "Runtime BR Nativo v" << VERSAO_BR << " (Performance v3.0)" << std::endl;
            std::cout << "\nUso: br <arquivo.br> [argumentos...]" << std::endl;
            std::cout << "  br -i                Modo interativo (REPL)" << std::endl;
            std::cout << "  br -c \"código\"      Executar código de uma linha" << std::endl;
            std::cout << "  br --help            Exibir ajuda" << std::endl;
            std::cout << "  br --version         Exibir versão" << std::endl;
            std::cout << "  br --stats           Exibir estatísticas de performance" << std::endl;
            return 1;
        }
        
        std::string comando = argv[1];
        if (comando == "--help" || comando == "-h") {
            std::cout << "Runtime BR Nativo v" << VERSAO_BR << std::endl;
            std::cout << "\nComandos:" << std::endl;
            std::cout << "  br arquivo.br        Executa um programa BR" << std::endl;
            std::cout << "  br -i                Modo interativo (REPL)" << std::endl;
            std::cout << "  br -c \"código\"      Executa código de uma linha" << std::endl;
            std::cout << "  br --help            Exibe ajuda" << std::endl;
            std::cout << "  br --version         Exibe versão" << std::endl;
            return 0;
        }
        if (comando == "--version" || comando == "-v") { 
            std::cout << "BR " << VERSAO_BR << " (Runtime Nativo C++ com Performance v3.0)" << std::endl;
            return 0; 
        }
        if (comando == "--stats") {
            std::cout << "=== Estatísticas de Performance v3.0 ===" << std::endl;
            
            std::cout << "\n--- FASE 1 ---" << std::endl;
            
            if (g_jit_compiler) {
                std::cout << "\n[JIT Compiler (Tier 0-1)]" << std::endl;
                std::cout << "  Funções compiladas: " << g_jit_compiler->codigo_compilado_count() << std::endl;
                std::cout << "  Hotspots detectados: " << g_jit_compiler->funcoes_hotspot() << std::endl;
                std::cout << "  Taxa de hit cache: " << g_jit_compiler->taxa_hit_cache() << "%" << std::endl;
            }
            
            if (g_gc) {
                EstatisticasGC stats = g_gc->obter_estatisticas();
                std::cout << "\n[Garbage Collector]" << std::endl;
                std::cout << "  Objetos ativos: " << g_gc->obter_contagem_objetos() << std::endl;
                std::cout << "  Memória usada: " << (g_gc->obter_memoria_usada() / 1024) << " KB" << std::endl;
                std::cout << "  Coleções executadas: " << stats.colecoes_executadas << std::endl;
                std::cout << "  Tempo total de coleta: " << stats.tempo_total_ms << " ms" << std::endl;
            }
            
            if (g_memory_pool) {
                std::cout << "\n[Memory Pool]" << std::endl;
                std::cout << "  Total alocado: " << (g_memory_pool->obter_total_alocado() / 1024) << " KB" << std::endl;
                std::cout << "  Total liberado: " << (g_memory_pool->obter_total_liberado() / 1024) << " KB" << std::endl;
                std::cout << "  Falhas de alocação: " << g_memory_pool->obter_total_falhas() << std::endl;
                std::cout << "  Taxa de falha: " << g_memory_pool->obter_taxa_falha() << "%" << std::endl;
            }
            
            if (g_string_pool) {
                std::cout << "\n[String Pool]" << std::endl;
                std::cout << "  Strings internadas: " << g_string_pool->obter_contagem() << std::endl;
                std::cout << "  Hits de cache: " << g_string_pool->obter_hits() << std::endl;
                std::cout << "  Misses de cache: " << g_string_pool->obter_misses() << std::endl;
                std::cout << "  Taxa de hit: " << g_string_pool->obter_taxa_hit() << "%" << std::endl;
                std::cout << "  Memória economizada: " << (g_string_pool->obter_memoria_economizada() / 1024) << " KB" << std::endl;
            }
            
            std::cout << "\n--- FASE 2 ---" << std::endl;
            
            if (g_jit_optimizing) {
                JITOptimizing::EstatisticasOtimizacao stats_opt = g_jit_optimizing->obter_estatisticas();
                std::cout << "\n[JIT Optimizing (Tier 2)]" << std::endl;
                std::cout << "  Funções otimizadas: " << stats_opt.funcoes_otimizadas << std::endl;
                std::cout << "  Loops unrolled: " << stats_opt.loops_unrolled << std::endl;
                std::cout << "  Funções inlined: " << stats_opt.funcoes_inlined << std::endl;
                std::cout << "  Caches inline criados: " << stats_opt.caches_inline_criados << std::endl;
                std::cout << "  Tipos especializados: " << stats_opt.tipos_specializados << std::endl;
                std::cout << "  Constantes folded: " << stats_opt.constantes_folded << std::endl;
                std::cout << "  Dead code removido: " << stats_opt.dead_code_removido << std::endl;
                std::cout << "  Tail calls otimizadas: " << stats_opt.tail_calls_otimizadas << std::endl;
            }
            
            if (g_inline_cache_manager) {
                std::cout << "\n[Inline Cache Manager]" << std::endl;
                std::cout << "  Total de caches: " << g_inline_cache_manager->obter_total_caches() << std::endl;
                std::cout << "  Caches criados: " << g_inline_cache_manager->obter_caches_criados() << std::endl;
                std::cout << "  Promoções polimórfico: " << g_inline_cache_manager->obter_promocoes_polimorfico() << std::endl;
                std::cout << "  Promoções megamórfico: " << g_inline_cache_manager->obter_promocoes_megamorfico() << std::endl;
                std::cout << "  Hit rate médio: " << g_inline_cache_manager->hit_rate_medio() << "%" << std::endl;
            }
            
            if (g_type_specializer) {
                std::cout << "\n[Type Specializer]" << std::endl;
                std::cout << "  Total de especializações: " << g_type_specializer->obter_total_specializations() << std::endl;
                std::cout << "  Total de deoptimizações: " << g_type_specializer->obter_total_deoptimizations() << std::endl;
                std::cout << "  Funções especializadas ativas: " << g_type_specializer->obter_total_funcoes_especializadas() << std::endl;
                std::cout << "  Verificações de guard: " << g_type_specializer->obter_total_guard_checks() << std::endl;
                std::cout << "  Falhas de guard: " << g_type_specializer->obter_total_guard_failures() << std::endl;
                std::cout << "  Taxa de sucesso guards: " << g_type_specializer->taxa_sucesso_guards() << "%" << std::endl;
            }
            
            if (g_compilation_cache) {
                std::cout << "\n[Compilation Cache]" << std::endl;
                std::cout << "  Entradas no cache: " << g_compilation_cache->obter_tamanho_cache() << std::endl;
                std::cout << "  Memória usada: " << (g_compilation_cache->obter_memoria_usada() / 1024) << " KB" << std::endl;
                std::cout << "  Hits: " << g_compilation_cache->obter_hits() << std::endl;
                std::cout << "  Misses: " << g_compilation_cache->obter_misses() << std::endl;
                std::cout << "  Taxa de hit: " << g_compilation_cache->obter_taxa_hit() << "%" << std::endl;
                std::cout << "  Habilitado: " << (g_compilation_cache->esta_habilitado() ? "sim" : "não") << std::endl;
            }
            
            return 0;
        }
        if (comando == "-i") { executarREPL(); return 0; }
        if (comando == "-c" && argc > 2) {
            std::string codigo = argv[2];
            LexerBR lexer(codigo); auto tokens = lexer.tokenizar();
            ParserBR parser(tokens); auto ast = parser.parse();
            if (parser.temErros()) { for (auto& e : parser.getErros()) std::cerr << e << std::endl; return 1; }
            VMBR vm; vm.executar(ast.get()); return 0;
        }
        
        std::string caminho = comando;
        if (!ServicoArquivo::existe(caminho)) { std::cerr << "Erro: Arquivo '" << caminho << "' não encontrado." << std::endl; return 1; }
        
        std::string codigo = ServicoArquivo::ler(caminho);
        if (codigo.empty()) { std::cerr << "Erro: Arquivo vazio." << std::endl; return 1; }
        
        std::cout << "Executando " << caminho << "..." << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        LexerBR lexer(codigo); auto tokens = lexer.tokenizar();
        ParserBR parser(tokens); auto ast = parser.parse();
        if (parser.temErros()) { for (auto& e : parser.getErros()) std::cerr << e << std::endl; return 1; }
        
        VMBR vm(caminho);
        vm.executar(ast.get());
        
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Execução concluída com sucesso!" << std::endl;
        
        // Limpa sistemas de performance antes de sair
        limpar_sistemas_performance();
        
    } catch (const ErroRuntime& e) {
        std::cerr << "Erro [Linha " << e.linha << "]: " << e.mensagem << std::endl;
        limpar_sistemas_performance();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        limpar_sistemas_performance();
        return 1;
    }
    
    // Limpa sistemas de performance antes de sair
    limpar_sistemas_performance();
    return 0;
}