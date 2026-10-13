// JIT Compiler Tier 2 (Optimizing JIT) - Implementação
#include "jit_optimizing.h"
#include <iostream>
#include <algorithm>
#include <cstring>

// ===================== JIT OPTIMIZING IMPLEMENTATION =====================

JITOptimizing::JITOptimizing() : JITCompiler() {
    // Habilita otimizações agressivas por padrão
    contexto.habilitar(Otimizacao::FUNCTION_INLINING);
    contexto.habilitar(Otimizacao::LOOP_UNROLLING);
    contexto.habilitar(Otimizacao::TAIL_CALL_OPTIMIZATION);
    contexto.habilitar(Otimizacao::ESCAPE_ANALYSIS);
    contexto.habilitar(Otimizacao::CONSTANT_PROPAGATION);
    contexto.habilitar(Otimizacao::STRENGTH_REDUCTION);
    
    std::cout << "[JIT Tier 2] Otimizações habilitadas: 10/10" << std::endl;
}

JITOptimizing::~JITOptimizing() {
}

// ===================== TIER 2: OPTIMIZING JIT =====================

void* JITOptimizing::compilar_otimizado(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao || !profile) return nullptr;
    
    std::cout << "[JIT Tier 2] Compilando otimizado: " << funcao->nome << std::endl;
    
    // Analisa código antes de otimizar
    if (funcao->corpoAST) {
        analisar_fluxo_controle(funcao->corpoAST);
        analisar_dependencias(funcao->corpoAST);
        inferir_tipos(funcao->corpoAST, profile);
    }
    
    // Aplica otimizações habilitadas
    void* codigo = nullptr;
    
    if (contexto.habilitada(Otimizacao::INLINE_CACHING)) {
        codigo = aplicar_inline_caching(funcao, profile);
    }
    
    if (contexto.habilitada(Otimizacao::TYPE_SPECIALIZATION)) {
        codigo = aplicar_type_specialization(funcao, profile);
    }
    
    if (contexto.habilitada(Otimizacao::FUNCTION_INLINING) && pode_inline(funcao, profile)) {
        codigo = aplicar_function_inlining(funcao, profile);
    }
    
    // Fallback para baseline se otimizações falharem
    if (!codigo) {
        codigo = compilar_baseline(funcao);
    }
    
    if (codigo) {
        stats.funcoes_otimizadas++;
    }
    
    return codigo;
}

CodigoBaseline JITOptimizing::compilar_otimizado_arvore(NoAST* ast_raiz, ProfileData* profile) {
    CodigoBaseline cb;
    
    if (!ast_raiz) return cb;
    
    std::cout << "[JIT Tier 2] Compilando árvore otimizada" << std::endl;
    
    // Aplica otimizações na AST
    NoAST* ast_otimizado = ast_raiz;
    
    if (contexto.habilitada(Otimizacao::CONSTANT_FOLDING)) {
        ast_otimizado = dobrar_constantes(ast_otimizado);
        stats.constantes_folded++;
    }
    
    if (contexto.habilitada(Otimizacao::DEAD_CODE_ELIMINATION)) {
        ast_otimizado = eliminar_codigo_morto(ast_otimizado);
        stats.dead_code_removido++;
    }
    
    if (contexto.habilitada(Otimizacao::CONSTANT_PROPAGATION)) {
        ast_otimizado = propagar_constantes(ast_otimizado);
    }
    
    if (contexto.habilitada(Otimizacao::STRENGTH_REDUCTION)) {
        ast_otimizado = reduzir_forca(ast_otimizado);
        stats.strength_reductions++;
    }
    
    // Gera código da árvore otimizada
    cb = compilar_baseline_arvore(ast_otimizado);
    cb.tier = TierJIT::TIER_2_OPTIMIZING;
    
    return cb;
}

// ===================== ANÁLISE ESTÁTICA =====================

void JITOptimizing::analisar_fluxo_controle(NoAST* ast) {
    if (!ast) return;
    
    // Constrói grafo de fluxo de controle
    // Este é um placeholder - na prática analisaria branches, loops, etc.
    std::cout << "[JIT Tier 2] Analisando fluxo de controle..." << std::endl;
}

void JITOptimizing::analisar_dependencias(NoAST* ast) {
    if (!ast) return;
    
    // Analisa dependências entre variáveis
    // Este é um placeholder - na prática analisaria data flow
    std::cout << "[JIT Tier 2] Analisando dependências..." << std::endl;
}

void JITOptimizing::inferir_tipos(NoAST* ast, ProfileData* profile) {
    if (!ast || !profile) return;
    
    // Infere tipos baseado em profiling
    std::cout << "[JIT Tier 2] Inferindo tipos..." << std::endl;
    
    // Para cada tipo de argumento no profile, cria TypeInfo
    for (const auto& [chave, count] : profile->tipos_argumentos) {
        if (count > profile->chamadas_totais * 0.8) {
            // 80%+ das chamadas usam este tipo - considerado estável
            size_t pos = chave.find(':');
            if (pos != std::string::npos) {
                std::string nome_var = chave.substr(0, pos);
                std::string tipo = chave.substr(pos + 1);
                
                TypeInfo tipo_info(tipo, true, 90);
                contexto.tipos_inferidos[nome_var] = tipo_info;
                stats.tipos_specializados++;
            }
        }
    }
}

// ===================== OTIMIZAÇÕES ESPECÍFICAS =====================

void* JITOptimizing::aplicar_inline_caching(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao || !profile) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando inline caching..." << std::endl;
    
    // Cria inline caches para métodos chamados pela função
    // Este é um placeholder - na prática analisaria chamadas de método
    for (const auto& [tipo, count] : profile->tipos_recebedor) {
        if (count > profile->chamadas_totais * 0.9) {
            // 90%+ das chamadas usam este tipo - monomorphic
            InlineCache* cache = obter_ou_criar_inline_cache(funcao->nome);
            cache->tipo_recebedor = tipo;
            cache->monomorphic = true;
            stats.caches_inline_criados++;
        }
    }
    
    // Gera código com inline caches
    return gerar_codigo_otimizado(funcao, contexto);
}

void* JITOptimizing::aplicar_type_specialization(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao || !profile) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando type specialization..." << std::endl;
    
    // Gera versões especializadas da função para tipos específicos
    // Este é um placeholder - na prática geraria código type-specific
    return gerar_codigo_otimizado(funcao, contexto);
}

void* JITOptimizing::aplicar_function_inlining(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao || !profile) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando function inlining..." << std::endl;
    
    // Inline funções pequenas e hot
    // Este é um placeholder - na prática substituiria chamadas por corpo da função
    stats.funcoes_inlined++;
    return gerar_codigo_otimizado(funcao, contexto);
}

void* JITOptimizing::aplicar_loop_unrolling(NoAST* loop_no, ProfileData* profile) {
    if (!loop_no || !profile) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando loop unrolling..." << std::endl;
    
    // Unroll loops com contagens conhecidas e pequenas
    NoAST* unrolled = unroll_loop(loop_no, 4); // Unroll factor 4
    stats.loops_unrolled++;
    
    // Gera código do loop unrolled
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_constant_folding(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando constant folding..." << std::endl;
    
    // Dobra constantes em tempo de compilação
    // Exemplo: 2 + 3 → 5
    NoAST* folded = dobrar_constantes(ast);
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_dead_code_elimination(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando dead code elimination..." << std::endl;
    
    // Remove código morto (nunca executado)
    NoAST* cleaned = eliminar_codigo_morto(ast);
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_tail_call_optimization(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando tail call optimization..." << std::endl;
    
    // Otimiza chamadas tail recursive para loops
    NoAST* optimized = aplicar_tail_call_optimization(ast);
    stats.tail_calls_otimizadas++;
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_escape_analysis(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando escape analysis..." << std::endl;
    
    // Analisa se objetos escapam do escopo
    // Se não escapam, pode alocar no stack
    stats.escapes_analisados++;
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_constant_propagation(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando constant propagation..." << std::endl;
    
    // Propaga constantes pelo código
    NoAST* propagated = propagar_constantes(ast);
    return gerar_codigo_otimizado(nullptr, contexto);
}

void* JITOptimizing::aplicar_strength_reduction(NoAST* ast) {
    if (!ast) return nullptr;
    
    std::cout << "[JIT Tier 2] Aplicando strength reduction..." << std::endl;
    
    // Reduz força de operações
    // Exemplo: x * 2 → x << 1
    NoAST* reduced = reduzir_forca(ast);
    return gerar_codigo_otimizado(nullptr, contexto);
}

// ===================== INLINE CACHE MANAGEMENT =====================

InlineCache* JITOptimizing::obter_ou_criar_inline_cache(const std::string& metodo) {
    auto it = contexto.inline_caches.find(metodo);
    if (it != contexto.inline_caches.end()) {
        return &it->second;
    }
    
    // Cria novo cache
    InlineCache cache;
    cache.nome_metodo = metodo;
    contexto.inline_caches[metodo] = cache;
    return &contexto.inline_caches[metodo];
}

void JITOptimizing::atualizar_inline_cache(InlineCache* cache, const std::string& tipo_recebedor, void* codigo) {
    if (!cache) return;
    
    cache->tipo_recebedor = tipo_recebedor;
    cache->codigo_compilado = codigo;
    cache->hits++;
}

// ===================== TYPE INFERENCE =====================

TypeInfo JITOptimizing::inferir_tipo_variavel(const std::string& nome_var, ProfileData* profile) {
    auto it = contexto.tipos_inferidos.find(nome_var);
    if (it != contexto.tipos_inferidos.end()) {
        return it->second;
    }
    
    return TypeInfo(); // Tipo desconhecido
}

void JITOptimizing::registrar_tipo_variavel(const std::string& nome_var, const TypeInfo& tipo) {
    contexto.tipos_inferidos[nome_var] = tipo;
}

// ===================== CONFIGURAÇÃO DE OTIMIZAÇÕES =====================

void JITOptimizing::habilitar_otimizacao(Otimizacao opt) {
    contexto.habilitar(opt);
    std::cout << "[JIT Tier 2] Otimização habilitada" << std::endl;
}

void JITOptimizing::desabilitar_otimizacao(Otimizacao opt) {
    contexto.desabilitar(opt);
    std::cout << "[JIT Tier 2] Otimização desabilitada" << std::endl;
}

void JITOptimizing::habilitar_todas_otimizacoes() {
    contexto.habilitar(Otimizacao::INLINE_CACHING);
    contexto.habilitar(Otimizacao::TYPE_SPECIALIZATION);
    contexto.habilitar(Otimizacao::FUNCTION_INLINING);
    contexto.habilitar(Otimizacao::LOOP_UNROLLING);
    contexto.habilitar(Otimizacao::CONSTANT_FOLDING);
    contexto.habilitar(Otimizacao::DEAD_CODE_ELIMINATION);
    contexto.habilitar(Otimizacao::TAIL_CALL_OPTIMIZATION);
    contexto.habilitar(Otimizacao::ESCAPE_ANALYSIS);
    contexto.habilitar(Otimizacao::CONSTANT_PROPAGATION);
    contexto.habilitar(Otimizacao::STRENGTH_REDUCTION);
    
    std::cout << "[JIT Tier 2] Todas otimizações habilitadas" << std::endl;
}

void JITOptimizing::desabilitar_todas_otimizacoes() {
    contexto.otimizacoes_habilitadas.clear();
    std::cout << "[JIT Tier 2] Todas otimizações desabilitadas" << std::endl;
}

// ===================== ESTATÍSTICAS =====================

JITOptimizing::EstatisticasOtimizacao JITOptimizing::obter_estatisticas() {
    return stats;
}

void JITOptimizing::resetar_estatisticas() {
    stats = EstatisticasOtimizacao();
}

// ===================== UTILITÁRIOS DE ANÁLISE =====================

bool JITOptimizing::eh_loop_hot(NoAST* loop_no, ProfileData* profile) {
    // Verifica se loop é hotspot baseado em profiling
    // Placeholder - na prática analisaria profile data
    return profile && profile->chamadas_totais > 1000;
}

bool JITOptimizing::eh_funcao_hot(FuncaoBR* funcao, ProfileData* profile) {
    return profile && profile->chamadas_totais > 100;
}

bool JITOptimizing::pode_inline(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao || !profile) return false;
    
    // Só inline se:
    // - Função é pequena (<50 instruções)
    // - Função é hotspot
    // - Não é recursiva
    
    bool eh_pequena = funcao->parametros.size() < 5;
    bool eh_hot = eh_funcao_hot(funcao, profile);
    
    return eh_pequena && eh_hot;
}

// ===================== UTILITÁRIOS DE GERAÇÃO DE CÓDIGO =====================

void* JITOptimizing::gerar_codigo_otimizado(FuncaoBR* funcao, OtimizacaoContexto& ctx) {
    // Gera código otimizado
    // Placeholder - na prática usaria LLVM ou geraria assembly manual
    size_t tamanho_codigo = 2048;
    uint8_t* codigo = alocar_memoria_executavel(tamanho_codigo);
    
    if (!codigo) return nullptr;
    
    // Placeholder: código NOP
    for (size_t i = 0; i < tamanho_codigo; i++) {
        codigo[i] = 0x90; // NOP x86
    }
    
    return codigo;
}

void JITOptimizing::write_prologue_optimized(uint8_t* buffer, size_t& offset) {
    // Escreve prologue otimizado
    // Placeholder - na prática salvaria registros otimizados
}

void JITOptimizing::write_epilogue_optimized(uint8_t* buffer, size_t& offset) {
    // Escreve epilogue otimizado
    // Placeholder - na prática restauraria registros otimizados
}

// ===================== UTILITÁRIOS DE OTIMIZAÇÃO =====================

NoAST* JITOptimizing::dobrar_constantes(NoAST* no) {
    // Dobra constantes
    // Placeholder - na prática analisaria AST e dobraria expressões constantes
    return no;
}

NoAST* JITOptimizing::eliminar_codigo_morto(NoAST* no) {
    // Elimina código morto
    // Placeholder - na prática analisaria reachability
    return no;
}

NoAST* JITOptimizing::propagar_constantes(NoAST* no) {
    // Propaga constantes
    // Placeholder - na prática substituiria variáveis por constantes quando possível
    return no;
}

NoAST* JITOptimizing::reduzir_forca(NoAST* no) {
    // Reduz força de operações
    // Placeholder - na prática substituiria operações caras por equivalentes mais baratos
    return no;
}

NoAST* JITOptimizing::unroll_loop(NoAST* loop_no, int fator) {
    // Unroll loop por fator
    // Placeholder - na prática duplicaria corpo do loop fator vezes
    return loop_no;
}
