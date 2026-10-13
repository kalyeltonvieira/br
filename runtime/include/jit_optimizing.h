// JIT Compiler Tier 2 (Optimizing JIT) para Linguagem BR
// Otimizações agressivas baseadas em profiling
#ifndef JIT_OPTIMIZING_H
#define JIT_OPTIMIZING_H

#include "jit_compiler.h"
#include <vector>
#include <map>
#include <set>
#include <unordered_map>

// Forward declarations
struct NoAST;
struct FuncaoBR;

// Otimizações disponíveis
enum class Otimizacao {
    INLINE_CACHING,           // Inline caching para chamadas
    TYPE_SPECIALIZATION,      // Specialização por tipo
    FUNCTION_INLINING,        // Inlining de funções
    LOOP_UNROLLING,          // Loop unrolling
    CONSTANT_FOLDING,         // Constant folding
    DEAD_CODE_ELIMINATION,   // Dead code elimination
    TAIL_CALL_OPTIMIZATION,  // Tail call optimization
    ESCAPE_ANALYSIS,         // Escape analysis
    CONSTANT_PROPAGATION,     // Constant propagation
    STRENGTH_REDUCTION       // Strength reduction
};

// Informação de tipo para specialization
struct TypeInfo {
    std::string nome_tipo;
    bool estavel;            // Tipo não muda durante execução
    int confianca;           // 0-100, baseado em profiling
    
    TypeInfo() : estavel(false), confianca(0) {}
    TypeInfo(const std::string& tipo, bool est, int conf) 
        : nome_tipo(tipo), estavel(est), confianca(conf) {}
};

// Cache inline para chamadas de método
struct InlineCache {
    std::string nome_metodo;
    std::string tipo_recebedor;
    void* codigo_compilado;
    int hits;
    int misses;
    bool monomorphic;        // Apenas 1 tipo de recebedor
    
    InlineCache() : codigo_compilado(nullptr), hits(0), misses(0), monomorphic(true) {}
};

// Contexto de otimização
struct OtimizacaoContexto {
    std::map<std::string, TypeInfo> tipos_inferidos;
    std::map<std::string, InlineCache> inline_caches;
    std::set<std::string> funcoes_inline_candidates;
    std::set<std::string> loops_para_unroll;
    std::map<std::string, Valor> constantes;
    
    // Otimizações habilitadas
    std::set<Otimizacao> otimizacoes_habilitadas;
    
    OtimizacaoContexto() {
        // Habilita otimizações padrão
        otimizacoes_habilitadas.insert(Otimizacao::INLINE_CACHING);
        otimizacoes_habilitadas.insert(Otimizacao::TYPE_SPECIALIZATION);
        otimizacoes_habilitadas.insert(Otimizacao::CONSTANT_FOLDING);
        otimizacoes_habilitadas.insert(Otimizacao::DEAD_CODE_ELIMINATION);
    }
    
    bool habilitada(Otimizacao opt) const {
        return otimizacoes_habilitadas.find(opt) != otimizacoes_habilitadas.end();
    }
    
    void habilitar(Otimizacao opt) {
        otimizacoes_habilitadas.insert(opt);
    }
    
    void desabilitar(Otimizacao opt) {
        otimizacoes_habilitadas.erase(opt);
    }
};

// JIT Compiler Tier 2 - Optimizing
class JITOptimizing : public JITCompiler {
private:
    // Contexto de otimização
    OtimizacaoContexto contexto;
    
    // Análise de fluxo de controle
    std::map<std::string, std::vector<std::string>> grafo_fluxo;
    
    // Análise de dependências
    std::map<std::string, std::set<std::string>> dependencias;
    
public:
    JITOptimizing();
    ~JITOptimizing();
    
    // Tier 2: Optimizing JIT
    void* compilar_otimizado(FuncaoBR* funcao, ProfileData* profile);
    CodigoBaseline compilar_otimizado_arvore(NoAST* ast_raiz, ProfileData* profile);
    
    // Análise estática
    void analisar_fluxo_controle(NoAST* ast);
    void analisar_dependencias(NoAST* ast);
    void inferir_tipos(NoAST* ast, ProfileData* profile);
    
    // Otimizações específicas
    void* aplicar_inline_caching(FuncaoBR* funcao, ProfileData* profile);
    void* aplicar_type_specialization(FuncaoBR* funcao, ProfileData* profile);
    void* aplicar_function_inlining(FuncaoBR* funcao, ProfileData* profile);
    void* aplicar_loop_unrolling(NoAST* loop_no, ProfileData* profile);
    void* aplicar_constant_folding(NoAST* ast);
    void* aplicar_dead_code_elimination(NoAST* ast);
    void* aplicar_tail_call_optimization(NoAST* ast);
    void* aplicar_escape_analysis(NoAST* ast);
    void* aplicar_constant_propagation(NoAST* ast);
    void* aplicar_strength_reduction(NoAST* ast);
    
    // Inline cache management
    InlineCache* obter_ou_criar_inline_cache(const std::string& metodo);
    void atualizar_inline_cache(InlineCache* cache, const std::string& tipo_recebedor, void* codigo);
    
    // Type inference
    TypeInfo inferir_tipo_variavel(const std::string& nome_var, ProfileData* profile);
    void registrar_tipo_variavel(const std::string& nome_var, const TypeInfo& tipo);
    
    // Configuração de otimizações
    void habilitar_otimizacao(Otimizacao opt);
    void desabilitar_otimizacao(Otimizacao opt);
    void habilitar_todas_otimizacoes();
    void desabilitar_todas_otimizacoes();
    
    // Estatísticas de otimização
    struct EstatisticasOtimizacao {
        int funcoes_otimizadas;
        int loops_unrolled;
        int funcoes_inlined;
        int caches_inline_criados;
        int tipos_specializados;
        int constantes_folded;
        int dead_code_removido;
        int tail_calls_otimizadas;
        int escapes_analisados;
        int strength_reductions;
    };
    
    EstatisticasOtimizacao obter_estatisticas();
    void resetar_estatisticas();
    
private:
    EstatisticasOtimizacao stats;
    
    // Utilitários de análise
    bool eh_loop_hot(NoAST* loop_no, ProfileData* profile);
    bool eh_funcao_hot(FuncaoBR* funcao, ProfileData* profile);
    bool pode_inline(FuncaoBR* funcao, ProfileData* profile);
    
    // Utilitários de geração de código
    void* gerar_codigo_otimizado(FuncaoBR* funcao, OtimizacaoContexto& ctx);
    void escrever_prologue_otimizado(uint8_t* buffer, size_t& offset);
    void escrever_epilogue_otimizado(uint8_t* buffer, size_t& offset);
    
    // Utilitários de otimização
    NoAST* dobrar_constantes(NoAST* no);
    NoAST* eliminar_codigo_morto(NoAST* no);
    NoAST* propagar_constantes(NoAST* no);
    NoAST* reduzir_forca(NoAST* no);
    NoAST* unroll_loop(NoAST* loop_no, int fator);
};

#endif // JIT_OPTIMIZING_H
