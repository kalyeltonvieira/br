// Compilador BR para Bytecode BRC1
// Gera arquivos binários .brc executáveis pela VM BR
#ifndef COMPILADOR_BYTECODE_H
#define COMPILADOR_BYTECODE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>

// Opcodes da VM BR
enum OpcodeBR {
    OP_EMPILHAR_CONST = 1,
    OP_DEFINIR_VAR = 2,
    OP_OBTER_VAR = 3,
    OP_SOMAR = 5,
    OP_SUBTRAIR = 6,
    OP_MULTIPLICAR = 7,
    OP_DIVIDIR = 8,
    OP_IGUAL = 9,
    OP_DIFERENTE = 10,
    OP_MAIOR = 11,
    OP_MENOR = 12,
    OP_MAIOR_IGUAL = 13,
    OP_MENOR_IGUAL = 14,
    OP_E_LOGICO = 15,
    OP_OU_LOGICO = 16,
    OP_NAO_LOGICO = 17,
    OP_CHAMAR_METODO = 18,
    OP_CHAMAR_SERVICO = 19,
    OP_SALTO = 20,
    OP_SALTO_SE_FALSO = 21,
    OP_SALTO_SE_VERDADEIRO = 22,
    OP_PARALELO_EXECUTE = 23,
    OP_INICIO_BLOCO = 24,
    OP_FIM_BLOCO = 25,
    OP_RETORNAR = 26,
    OP_MOSTRE = 27,
    OP_LEIA = 28,
    OP_EMPILHAR_NULO = 29,
    OP_EMPILHAR_VERDADEIRO = 30,
    OP_EMPILHAR_FALSO = 31,
    OP_EMPILHAR_LISTA = 32,
    OP_EMPILHAR_REGISTRO = 33,
    OP_ACESSAR_INDICE = 34,
    OP_ACESSAR_PROPRIEDADE = 35,
    OP_DEFINIR_PROPRIEDADE = 36,
    OP_TENTE_CAPTURE = 37,
    OP_LANCE_ERRO = 38,
    OP_YIELD = 39,
    OP_AGUARDE = 40,
    OP_REPITA_VEZES = 41,
    OP_CONTE_ATE = 42,
    OP_PARA_CADA = 43,
    OP_ESCOLHA_CASO = 44,
    OP_IMPORTAR = 45,
    OP_CLASSE = 46,
    OP_NOVA_INSTANCIA = 47,
    OP_HERDA = 48,
    OP_DECORATOR = 49,
    OP_CONTEXT_MANAGER = 50
};

// Tipos de constantes
enum TipoConstante {
    CONST_INTEIRO,
    CONST_DECIMAL,
    CONST_TEXTO,
    CONST_BOOLEANO,
    CONST_NULO
};

// Estrutura de constante
struct ConstanteBR {
    TipoConstante tipo;
    union {
        int64_t inteiro;
        double decimal;
    } valor;
    std::string texto;
    bool booleano;
    
    ConstanteBR() : tipo(CONST_NULO), booleano(false) {
        valor.inteiro = 0;
    }
    
    static ConstanteBR criarInteiro(int64_t v) {
        ConstanteBR c; c.tipo = CONST_INTEIRO; c.valor.inteiro = v; return c;
    }
    
    static ConstanteBR criarDecimal(double v) {
        ConstanteBR c; c.tipo = CONST_DECIMAL; c.valor.decimal = v; return c;
    }
    
    static ConstanteBR criarTexto(const std::string& v) {
        ConstanteBR c; c.tipo = CONST_TEXTO; c.texto = v; return c;
    }
    
    static ConstanteBR criarBooleano(bool v) {
        ConstanteBR c; c.tipo = CONST_BOOLEANO; c.booleano = v; return c;
    }
    
    static ConstanteBR criarNulo() {
        return ConstanteBR();
    }
};

// Estrutura de função
struct FuncaoBytecode {
    std::string nome;
    std::vector<std::string> parametros;
    int endereco;
    int tamanho;
    
    FuncaoBytecode() : endereco(0), tamanho(0) {}
};

// Estrutura de classe
struct ClasseBytecode {
    std::string nome;
    std::string classe_mae;
    std::vector<std::string> campos;
    std::map<std::string, FuncaoBytecode> metodos;
    
    ClasseBytecode() {}
};

// Cabeçalho do arquivo BRC1
struct CabecalhoBRC {
    uint8_t magic[4]; // 'B', 'R', 'C', '1'
    uint16_t versao_major;
    uint16_t versao_minor;
    uint32_t tamanho_constantes;
    uint32_t tamanho_funcoes;
    uint32_t tamanho_classes;
    uint32_t tamanho_codigo;
    uint32_t flags_permissoes;
    
    CabecalhoBRC() {
        magic[0] = 'B'; magic[1] = 'R'; magic[2] = 'C'; magic[3] = '1';
        versao_major = 1; versao_minor = 0;
        tamanho_constantes = 0;
        tamanho_funcoes = 0;
        tamanho_classes = 0;
        tamanho_codigo = 0;
        flags_permissoes = 0;
    }
};

// Compilador BR para Bytecode
class CompiladorBytecode {
private:
    std::vector<uint8_t> bytecode;
    std::vector<ConstanteBR> constantes;
    std::map<std::string, FuncaoBytecode> funcoes;
    std::map<std::string, ClasseBytecode> classes;
    std::map<std::string, int> tabela_variaveis;
    int contador_variaveis;
    std::vector<std::string> erros;
    uint32_t permissoes;
    
public:
    CompiladorBytecode();
    
    // Compilar código BR para bytecode
    bool compilar(const std::string& codigo_br);
    bool compilar_arquivo(const std::string& caminho_arquivo);
    
    // Salvar bytecode em arquivo .brc
    bool salvar_bytecode(const std::string& caminho_saida);
    
    // Carregar bytecode de arquivo .brc
    bool carregar_bytecode(const std::string& caminho_entrada);
    
    // Obter bytecode gerado
    std::vector<uint8_t> obter_bytecode() const { return bytecode; }
    
    // Definir permissões
    void definir_permissao_internet(bool habilitar);
    void definir_permissao_arquivos(bool habilitar);
    void definir_permissao_rede(bool habilitar);
    void definir_permissao_banco(bool habilitar);
    
    // Obter erros de compilação
    std::vector<std::string> obter_erros() const { return erros; }
    bool tem_erros() const { return !erros.empty(); }
    
private:
    // Geração de bytecode
    void escrever_uint8(uint8_t valor);
    void escrever_uint16(uint16_t valor);
    void escrever_uint32(uint32_t valor);
    void escrever_int32(int32_t valor);
    void escrever_int64(int64_t valor);
    void escrever_double(double valor);
    void escrever_string(const std::string& texto);
    void escrever_opcode(OpcodeBR opcode);
    
    // Gerenciamento de constantes
    int adicionar_constante(const ConstanteBR& constante);
    int buscar_constante(const ConstanteBR& constante);
    
    // Gerenciamento de variáveis
    int adicionar_variavel(const std::string& nome);
    int buscar_variavel(const std::string& nome);
    
    // Parser simplificado (para demonstração)
    bool parsear_codigo(const std::string& codigo);
    bool parsear_declaracao(const std::string& linha);
    bool parsear_atribuicao(const std::string& linha);
    bool parsear_mostre(const std::string& linha);
    bool parsear_se(const std::string& linha);
    bool parsear_enquanto(const std::string& linha);
    bool parsear_funcao(const std::string& linha);
    bool parsear_classe(const std::string& linha);
    bool parsear_importar(const std::string& linha);
    
    // Utilitários
    void adicionar_erro(const std::string& erro);
    std::string trim(const std::string& s) const;
    std::vector<std::string> dividir(const std::string& s, char delim) const;
    bool inicia_com(const std::string& s, const std::string& prefixo) const;
};

#endif // COMPILADOR_BYTECODE_H
