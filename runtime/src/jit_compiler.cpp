// JIT Compiler Tier 0-1 para Linguagem BR - Implementação
#include "jit_compiler.h"
#include <iostream>
#include <cstring>
#include <sys/mman.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/mman.h>
#endif

// ===================== JIT COMPILER IMPLEMENTATION =====================

JITCompiler::JITCompiler() {
    // Inicializa estruturas
}

JITCompiler::~JITCompiler() {
    // Limpa memória executável
    for (auto& [nome, codigo] : codigo_compilado) {
        if (codigo.endereco && codigo.tamanho > 0) {
            liberar_memoria_executavel(codigo.endereco, codigo.tamanho);
        }
    }
}

// ===================== TIER 0: INTERPRETER =====================

Valor JITCompiler::executar_interpretado(NoAST* no) {
    // O interpretador já existe em main.cpp
    // Este é um wrapper para manter a interface consistente
    // Na prática, chamaríamos a VM existente
    return Valor::criarNulo();
}

// ===================== TIER 1: BASELINE JIT =====================

void* JITCompiler::compilar_baseline(FuncaoBR* funcao) {
    if (!funcao) return nullptr;
    
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    // Verifica se já está compilado
    auto it = codigo_compilado.find(funcao->nome);
    if (it != codigo_compilado.end() && it->second.tier >= TierJIT::TIER_1_BASELINE) {
        return it->second.endereco;
    }
    
    // Gera código baseline
    void* codigo = gerar_codigo_baseline(funcao);
    
    if (codigo) {
        CodigoBaseline cb;
        cb.endereco = codigo;
        cb.tamanho = 256; // Tamanho estimado
        cb.nome_funcao = funcao->nome;
        cb.tier = TierJIT::TIER_1_BASELINE;
        codigo_compilado[funcao->nome] = cb;
        
        std::cout << "[JIT] Compilou baseline: " << funcao->nome << std::endl;
    }
    
    return codigo;
}

CodigoBaseline JITCompiler::compilar_baseline_arvore(NoAST* ast_raiz) {
    CodigoBaseline cb;
    
    if (!ast_raiz) return cb;
    
    // Aloca buffer para código
    size_t tamanho_buffer = 4096;
    uint8_t* buffer = alocar_memoria_executavel(tamanho_buffer);
    
    if (!buffer) {
        std::cerr << "[JIT] Erro ao alocar memória executável" << std::endl;
        return cb;
    }
    
    size_t offset = 0;
    
    // Gera código baseline simples da AST
    // Este é um exemplo simplificado - na prática precisaria
    // traversar a AST e gerar opcodes apropriados
    
    // Exemplo: gerar prologue
    escrever_opcode(buffer, offset, OpcodeBaseline::OP_PUSH_INT);
    escrever_int32(buffer, offset, 0); // Return value default
    
    // Exemplo: gerar epilogue
    escrever_opcode(buffer, offset, OpcodeBaseline::OP_RETURN);
    
    cb.endereco = buffer;
    cb.tamanho = offset;
    cb.tier = TierJIT::TIER_1_BASELINE;
    
    return cb;
}

void* JITCompiler::gerar_codigo_baseline(FuncaoBR* funcao) {
    // Gera código de máquina simples para a função
    // Este é um placeholder - na prática usaria LLVM ou
    // geraria código assembly manual
    
    size_t tamanho_codigo = 1024;
    uint8_t* codigo = alocar_memoria_executavel(tamanho_codigo);
    
    if (!codigo) return nullptr;
    
    // Placeholder: código NOP (no operation)
    for (size_t i = 0; i < tamanho_codigo; i++) {
        codigo[i] = 0x90; // NOP x86
    }
    
    return codigo;
}

// ===================== HOTSPOT DETECTION =====================

void JITCompiler::registrar_chamada(const std::string& nome, int64_t tempo_ns) {
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    ProfileData& profile = perfis[nome];
    profile.registrar_chamada(tempo_ns);
    
    // Verifica se deve compilar
    if (deve_compilar_baseline(nome)) {
        std::cout << "[JIT] Hotspot detectado: " << nome 
                  << " (" << profile.chamadas_totais << " chamadas)" << std::endl;
    }
}

void JITCompiler::registrar_tipo_recebedor(const std::string& nome_funcao, const std::string& tipo) {
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    ProfileData& profile = perfis[nome_funcao];
    profile.tipos_recebedor[tipo]++;
}

void JITCompiler::registrar_tipo_argumento(const std::string& nome_funcao, const std::string& tipo, int idx_arg) {
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    ProfileData& profile = perfis[nome_funcao];
    std::string chave = "arg_" + std::to_string(idx_arg);
    profile.tipos_argumentos[chave + ":" + tipo]++;
}

bool JITCompiler::deve_compilar_baseline(const std::string& nome) {
    auto it = perfis.find(nome);
    if (it == perfis.end()) return false;
    
    return it->second.chamadas_totais >= THRESHOLD_BASELINE;
}

bool JITCompiler::deve_compilar_otimizado(const std::string& nome) {
    auto it = perfis.find(nome);
    if (it == perfis.end()) return false;
    
    return it->second.chamadas_totais >= THRESHOLD_OPTIMIZING;
}

bool JITCompiler::deve_compilar_native(const std::string& nome) {
    auto it = perfis.find(nome);
    if (it == perfis.end()) return false;
    
    return it->second.chamadas_totais >= THRESHOLD_NATIVE;
}

TierJIT JITCompiler::obter_tier(const std::string& nome) {
    auto it = codigo_compilado.find(nome);
    if (it == codigo_compilado.end()) {
        return TierJIT::TIER_0_INTERPRETER;
    }
    
    return it->second.tier;
}

ProfileData JITCompiler::obter_profile(const std::string& nome) {
    auto it = perfis.find(nome);
    if (it == perfis.end()) {
        return ProfileData();
    }
    
    return it->second;
}

// ===================== EXECUÇÃO COM DISPATCH =====================

Valor JITCompiler::executar_funcao(const std::string& nome, FuncaoBR* funcao, std::vector<Valor> args) {
    auto inicio = std::chrono::high_resolution_clock::now();
    
    Valor resultado;
    
    // Dispatch baseado no tier
    TierJIT tier = obter_tier(nome);
    
    switch (tier) {
        case TierJIT::TIER_0_INTERPRETER:
            // Executa interpretado
            resultado = executar_interpretado(funcao->corpoAST);
            break;
            
        case TierJIT::TIER_1_BASELINE:
        case TierJIT::TIER_2_OPTIMIZING:
        case TierJIT::TIER_3_NATIVE: {
            // Executa código compilado
            auto it = codigo_compilado.find(nome);
            if (it != codigo_compilado.end() && it->second.endereco) {
                // Chama função compilada (placeholder)
                typedef Valor (*FuncaoCompilada)(std::vector<Valor>);
                FuncaoCompilada fn = (FuncaoCompilada)it->second.endereco;
                resultado = fn(args);
            } else {
                resultado = executar_interpretado(funcao->corpoAST);
            }
            break;
        }
    }
    
    // Registra profiling
    auto fim = std::chrono::high_resolution_clock::now();
    int64_t tempo_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(fim - inicio).count();
    registrar_chamada(nome, tempo_ns);
    
    // Verifica se deve compilar para tier superior
    if (deve_compilar_baseline(nome) && tier == TierJIT::TIER_0_INTERPRETER) {
        compilar_baseline(funcao);
    }
    
    return resultado;
}

// ===================== INVALIDAÇÃO DE CÓDIGO =====================

void JITCompiler::invalidar_codigo(const std::string& nome) {
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    auto it = codigo_compilado.find(nome);
    if (it != codigo_compilado.end()) {
        if (it->second.endereco && it->second.tamanho > 0) {
            liberar_memoria_executavel(it->second.endereco, it->second.tamanho);
        }
        codigo_compilado.erase(it);
    }
}

void JITCompiler::invalidar_tudo() {
    std::lock_guard<std::mutex> lock(mutex_compilacao);
    
    for (auto& [nome, codigo] : codigo_compilado) {
        if (codigo.endereco && codigo.tamanho > 0) {
            liberar_memoria_executavel(codigo.endereco, codigo.tamanho);
        }
    }
    
    codigo_compilado.clear();
    perfis.clear();
}

// ===================== ESTATÍSTICAS =====================

size_t JITCompiler::codigo_compilado_count() {
    return codigo_compilado.size();
}

size_t JITCompiler::funcoes_hotspot() {
    size_t count = 0;
    for (const auto& [nome, profile] : perfis) {
        if (profile.eh_hotspot) count++;
    }
    return count;
}

double JITCompiler::taxa_hit_cache() {
    if (perfis.empty()) return 0.0;
    
    size_t hits = 0;
    for (const auto& [nome, codigo] : codigo_compilado) {
        if (codigo.tier >= TierJIT::TIER_1_BASELINE) {
            hits++;
        }
    }
    
    return (double)hits / perfis.size() * 100.0;
}

// ===================== MEMÓRIA EXECUTÁVEL =====================

uint8_t* JITCompiler::alocar_memoria_executavel(size_t tamanho) {
#ifdef _WIN32
    void* mem = VirtualAlloc(nullptr, tamanho, 
                             MEM_COMMIT | MEM_RESERVE, 
                             PAGE_EXECUTE_READWRITE);
    return (uint8_t*)mem;
#else
    void* mem = mmap(nullptr, tamanho, 
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (uint8_t*)mem;
#endif
}

void JITCompiler::liberar_memoria_executavel(void* endereco, size_t tamanho) {
    if (!endereco) return;
    
#ifdef _WIN32
    VirtualFree(endereco, 0, MEM_RELEASE);
#else
    munmap(endereco, tamanho);
#endif
}

// ===================== UTILITÁRIOS =====================

std::string JITCompiler::gerar_nome_unico(const std::string& base) {
    static int contador = 0;
    return base + "_" + std::to_string(contador++);
}

void JITCompiler::escrever_opcode(uint8_t* buffer, size_t& offset, OpcodeBaseline op) {
    buffer[offset++] = (uint8_t)op;
}

void JITCompiler::escrever_int32(uint8_t* buffer, size_t& offset, int32_t val) {
    std::memcpy(buffer + offset, &val, sizeof(int32_t));
    offset += sizeof(int32_t);
}

void JITCompiler::escrever_int64(uint8_t* buffer, size_t& offset, int64_t val) {
    std::memcpy(buffer + offset, &val, sizeof(int64_t));
    offset += sizeof(int64_t);
}

void JITCompiler::escrever_double(uint8_t* buffer, size_t& offset, double val) {
    std::memcpy(buffer + offset, &val, sizeof(double));
    offset += sizeof(double);
}
