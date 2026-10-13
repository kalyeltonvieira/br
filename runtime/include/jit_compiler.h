// JIT Compiler Tier 0-1 para Linguagem BR
// Tier 0: Interpreter (baseline)
// Tier 1: Baseline JIT (simple code generation)
#ifndef JIT_COMPILER_H
#define JIT_COMPILER_H

#include <string>
#include <map>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

// Forward declarations
struct NoAST;
struct FuncaoBR;
struct Valor;

// Profile data para otimizações
struct ProfileData {
    int64_t chamadas_totais;
    int64_t tempo_total_ns;
    int64_t tempo_medio_ns;
    std::map<std::string, int> tipos_recebedor;
    std::map<std::string, int> tipos_argumentos;
    bool eh_hotspot;
    
    ProfileData() : chamadas_totais(0), tempo_total_ns(0), tempo_medio_ns(0), eh_hotspot(false) {}
    
    void registrar_chamada(int64_t tempo_ns) {
        chamadas_totais++;
        tempo_total_ns += tempo_ns;
        tempo_medio_ns = tempo_total_ns / chamadas_totais;
        
        // Hotspot: chamado mais de 100 vezes
        eh_hotspot = chamadas_totais > 100;
    }
};

// Tier de compilação
enum class TierJIT {
    TIER_0_INTERPRETER,  // Interpretador (baseline)
    TIER_1_BASELINE,     // Baseline JIT (simple)
    TIER_2_OPTIMIZING,   // Optimizing JIT (aggressive)
    TIER_3_NATIVE        // LLVM native code
};

// Código compilado (baseline)
struct CodigoBaseline {
    void* endereco;
    size_t tamanho;
    std::string nome_funcao;
    TierJIT tier;
    
    CodigoBaseline() : endereco(nullptr), tamanho(0), tier(TierJIT::TIER_0_INTERPRETER) {}
};

// JIT Compiler Principal
class JITCompiler {
private:
    // Hotspot detection
    std::map<std::string, ProfileData> perfis;
    std::map<std::string, CodigoBaseline> codigo_compilado;
    
    // Thresholds
    static const int THRESHOLD_BASELINE = 10;    // Compilar tier 1 após 10 chamadas
    static const int THRESHOLD_OPTIMIZING = 100; // Compilar tier 2 após 100 chamadas
    static const int THRESHOLD_NATIVE = 1000;    // Compilar tier 3 após 1000 chamadas
    
    // Mutex para thread safety
    std::mutex mutex_compilacao;
    
public:
    JITCompiler();
    ~JITCompiler();
    
    // Tier 0: Interpreter (já existe no main.cpp)
    Valor executar_interpretado(NoAST* no);
    
    // Tier 1: Baseline JIT
    void* compilar_baseline(FuncaoBR* funcao);
    CodigoBaseline compilar_baseline_arvore(NoAST* ast_raiz);
    
    // Hotspot detection
    void registrar_chamada(const std::string& nome, int64_t tempo_ns);
    void registrar_tipo_recebedor(const std::string& nome_funcao, const std::string& tipo);
    void registrar_tipo_argumento(const std::string& nome_funcao, const std::string& tipo, int idx_arg);
    
    bool deve_compilar_baseline(const std::string& nome);
    bool deve_compilar_otimizado(const std::string& nome);
    bool deve_compilar_native(const std::string& nome);
    
    TierJIT obter_tier(const std::string& nome);
    ProfileData obter_profile(const std::string& nome);
    
    // Execução com dispatch inteligente
    Valor executar_funcao(const std::string& nome, FuncaoBR* funcao, std::vector<Valor> args);
    
    // Invalidação de código
    void invalidar_codigo(const std::string& nome);
    void invalidar_tudo();
    
    // Estatísticas
    size_t codigo_compilado_count();
    size_t funcoes_hotspot();
    double taxa_hit_cache();
    
private:
    // Geração de código baseline (simple machine code)
    void* gerar_codigo_baseline(FuncaoBR* funcao);
    uint8_t* alocar_memoria_executavel(size_t tamanho);
    void liberar_memoria_executavel(void* endereco, size_t tamanho);
    
    // Opcodes simples para baseline
    enum OpcodeBaseline {
        OP_PUSH_INT = 0x01,
        OP_PUSH_DOUBLE = 0x02,
        OP_PUSH_STRING = 0x03,
        OP_LOAD_VAR = 0x10,
        OP_STORE_VAR = 0x11,
        OP_ADD = 0x20,
        OP_SUB = 0x21,
        OP_MUL = 0x22,
        OP_DIV = 0x23,
        OP_CALL = 0x30,
        OP_RETURN = 0x31,
        OP_JUMP = 0x40,
        OP_JUMP_IF_FALSE = 0x41
    };
    
    // Utilitários
    std::string gerar_nome_unico(const std::string& base);
    void escrever_opcode(uint8_t* buffer, size_t& offset, OpcodeBaseline op);
    void escrever_int32(uint8_t* buffer, size_t& offset, int32_t val);
    void escrever_int64(uint8_t* buffer, size_t& offset, int64_t val);
    void escrever_double(uint8_t* buffer, size_t& offset, double val);
};

#endif // JIT_COMPILER_H
