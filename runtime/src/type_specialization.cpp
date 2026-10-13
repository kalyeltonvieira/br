// Type Specialization - Implementação
#include "type_specialization.h"
#include <iostream>
#include <algorithm>

// ===================== TYPE INFERENCE ENGINE =====================

TypeInferenceEngine::TypeInferenceEngine() {
}

TypeInferenceEngine::~TypeInferenceEngine() {
}

std::string TypeInferenceEngine::inferir_tipo(NoAST* no) {
    if (!no) return "desconhecido";
    
    // Inferência baseada no tipo de nó
    switch (no->tipo) {
        case NO_NUMERO_INT:
            return "inteiro";
        case NO_NUMERO_DEC:
            return "decimal";
        case NO_TEXTO_LITERAL:
        case NO_FSTRING:
            return "texto";
        case NO_BOOLEANO:
            return "booleano";
        case NO_NULO:
            return "nulo";
        case NO_LISTA_LITERAL:
        case NO_LISTA_COMPREHENSION:
            return "lista";
        case NO_REGISTRO_LITERAL:
            return "registro";
        case NO_OPERACAO_BINARIA:
            return inferir_tipo_operacao(no);
        case NO_CHAMADA_FUNCAO:
        case NO_CHAMADA_METODO:
            return inferir_tipo_chamada(no);
        case NO_ACESSO_PROPRIEDADE:
            return inferir_tipo_acesso(no);
        case NO_IDENTIFICADOR:
            return inferir_tipo_variavel(no->nome);
        default:
            return "desconhecido";
    }
}

std::string TypeInferenceEngine::inferir_tipo_variavel(const std::string& nome) {
    auto it = tipos_variaveis.find(nome);
    if (it != tipos_variaveis.end()) {
        return it->second;
    }
    return "desconhecido";
}

std::string TypeInferenceEngine::inferir_tipo_expressao(NoAST* expr) {
    if (!expr) return "desconhecido";
    
    std::string expr_id = "expr_" + std::to_string((uintptr_t)expr);
    auto it = tipos_expressoes.find(expr_id);
    if (it != tipos_expressoes.end()) {
        return it->second;
    }
    
    return inferir_tipo(expr);
}

TypeSignature TypeInferenceEngine::inferir_assinatura(FuncaoBR* funcao, ProfileData* profile) {
    if (!funcao) return TypeSignature();
    
    TypeSignature sig;
    sig.nome_funcao = funcao->nome;
    
    // Infer tipos dos parâmetros baseado em profiling
    for (const auto& param : funcao->parametros) {
        std::string chave = "arg_" + param;
        
        // Busca tipo mais comum no profile
        std::string tipo_mais_comum = "desconhecido";
        int max_count = 0;
        
        for (const auto& [chave_profile, count] : profile->tipos_argumentos) {
            if (chave_profile.find(chave) == 0) {
                size_t pos = chave_profile.find(':');
                if (pos != std::string::npos) {
                    std::string tipo = chave_profile.substr(pos + 1);
                    if (count > max_count) {
                        max_count = count;
                        tipo_mais_comum = tipo;
                    }
                }
            }
        }
        
        sig.tipos_parametros.push_back(tipo_mais_comum);
    }
    
    // Infer tipo de retorno (placeholder)
    sig.tipo_retorno = "desconhecido";
    
    return sig;
}

void TypeInferenceEngine::registrar_tipo_variavel(const std::string& nome, const std::string& tipo, int confianca) {
    tipos_variaveis[nome] = tipo;
    confianca_tipos[nome] = confianca;
}

void TypeInferenceEngine::registrar_tipo_expressao(const std::string& id, const std::string& tipo, int confianca) {
    tipos_expressoes[id] = tipo;
    confianca_tipos[id] = confianca;
}

bool TypeInferenceEngine::tipo_estavel(const std::string& nome) {
    auto it = confianca_tipos.find(nome);
    return it != confianca_tipos.end() && it->second >= 80;
}

bool TypeInferenceEngine::tipo_mudou(const std::string& nome, const std::string& novo_tipo) {
    auto it = tipos_variaveis.find(nome);
    if (it == tipos_variaveis.end()) return false;
    return it->second != novo_tipo;
}

void TypeInferenceEngine::limpar() {
    tipos_variaveis.clear();
    tipos_expressoes.clear();
    confianca_tipos.clear();
}

void TypeInferenceEngine::limpar_variavel(const std::string& nome) {
    tipos_variaveis.erase(nome);
    confianca_tipos.erase(nome);
}

double TypeInferenceEngine::obter_confianca_media() const {
    if (confianca_tipos.empty()) return 0.0;
    
    int total = 0;
    for (const auto& [nome, conf] : confianca_tipos) {
        total += conf;
    }
    
    return (double)total / confianca_tipos.size();
}

std::string TypeInferenceEngine::inferir_tipo_literal(NoAST* no) {
    return inferir_tipo(no);
}

std::string TypeInferenceEngine::inferir_tipo_operacao(NoAST* no) {
    if (!no || no->filhos.size() < 2) return "desconhecido";
    
    std::string tipo_esq = inferir_tipo(no->filhos[0].get());
    std::string tipo_dir = inferir_tipo(no->filhos[1].get());
    
    // Operações aritméticas
    if (no->operador == "+" || no->operador == "-" || no->operador == "*" || no->operador == "/") {
        if (tipo_esq == "inteiro" && tipo_dir == "inteiro") return "inteiro";
        if (tipo_esq == "decimal" || tipo_dir == "decimal") return "decimal";
    }
    
    // Operações de concatenação
    if (no->operador == "+" && (tipo_esq == "texto" || tipo_dir == "texto")) {
        return "texto";
    }
    
    // Comparações
    if (no->operador == "==" || no->operador == "!=" || no->operador == ">" || 
        no->operador == "<" || no->operador == ">=" || no->operador == "<=") {
        return "booleano";
    }
    
    return "desconhecido";
}

std::string TypeInferenceEngine::inferir_tipo_chamada(NoAST* no) {
    // Placeholder - na prática analisaria assinatura da função
    return "desconhecido";
}

std::string TypeInferenceEngine::inferir_tipo_acesso(NoAST* no) {
    // Placeholder - na prática analisaria tipo do objeto e propriedade
    return "desconhecido";
}

// ===================== TYPE SPECIALIZER =====================

TypeSpecializer::TypeSpecializer() 
    : inference_engine(new TypeInferenceEngine()),
      total_specializations(0), total_deoptimizations(0),
      total_guard_checks(0), total_guard_failures(0) {
}

TypeSpecializer::~TypeSpecializer() {
    delete inference_engine;
    
    // Limpa funções especializadas
    for (auto& [sig, func] : funcoes_especializadas) {
        if (func.codigo_compilado) {
            // Liberar código compilado (placeholder)
        }
    }
    funcoes_especializadas.clear();
}

void* TypeSpecializer::especializar_funcao(FuncaoBR* funcao, TypeSignature assinatura) {
    if (!funcao) return nullptr;
    
    std::cout << "[TypeSpecializer] Especializando: " << assinatura.para_string() << std::endl;
    
    // Verifica se já existe especialização
    auto it = funcoes_especializadas.find(assinatura);
    if (it != funcoes_especializadas.end()) {
        it->second.usos++;
        it->second.hits++;
        return it->second.codigo_compilado;
    }
    
    // Cria nova especialização
    void* codigo = gerar_codigo_especializado(funcao, assinatura);
    
    if (codigo) {
        FuncaoEspecializada func_spec;
        func_spec.assinatura = assinatura;
        func_spec.codigo_compilado = codigo;
        func_spec.estavel = true;
        func_spec.usos = 1;
        func_spec.hits = 1;
        
        funcoes_especializadas[assinatura] = func_spec;
        total_specializations++;
        
        std::cout << "[TypeSpecializer] Especialização criada: " << assinatura.para_string() << std::endl;
    }
    
    return codigo;
}

void* TypeSpecializer::especializar_operador(const std::string& operador, TypeSignature assinatura) {
    std::cout << "[TypeSpecializer] Especializando operador: " << operador << std::endl;
    
    return gerar_codigo_operador_especializado(operador, assinatura);
}

void* TypeSpecializer::buscar_funcao_especializada(TypeSignature assinatura) {
    auto it = funcoes_especializadas.find(assinatura);
    if (it != funcoes_especializadas.end()) {
        it->second.usos++;
        it->second.hits++;
        return it->second.codigo_compilado;
    }
    
    return nullptr;
}

void* TypeSpecializer::buscar_ou_criar_especializada(FuncaoBR* funcao, TypeSignature assinatura) {
    void* codigo = buscar_funcao_especializada(assinatura);
    if (codigo) return codigo;
    
    return especializar_funcao(funcao, assinatura);
}

void TypeSpecializer::adicionar_guard(TypeSignature sig, std::function<bool()> guard) {
    guards[sig].push_back(guard);
}

bool TypeSpecializer::verificar_guards(TypeSignature sig) {
    auto it = guards.find(sig);
    if (it == guards.end()) return true;
    
    total_guard_checks++;
    
    for (const auto& guard : it->second) {
        if (!guard()) {
            total_guard_failures++;
            return false;
        }
    }
    
    return true;
}

void TypeSpecializer::deoptimizar(TypeSignature sig) {
    auto it = funcoes_especializadas.find(sig);
    if (it != funcoes_especializadas.end()) {
        // Liberar código compilado (placeholder)
        if (it->second.codigo_compilado) {
            // free(it->second.codigo_compilado);
        }
        
        funcoes_especializadas.erase(it);
        total_deoptimizations++;
        
        std::cout << "[TypeSpecializer] Deoptimização: " << sig.para_string() << std::endl;
    }
}

void TypeSpecializer::deoptimizar_tudo() {
    for (auto& [sig, func] : funcoes_especializadas) {
        if (func.codigo_compilado) {
            // free(func.codigo_compilado);
        }
    }
    
    int count = funcoes_especializadas.size();
    funcoes_especializadas.clear();
    total_deoptimizations += count;
    
    std::cout << "[TypeSpecializer] Deoptimização total: " << count << " funções" << std::endl;
}

TypeSignature TypeSpecializer::inferir_assinatura(FuncaoBR* funcao, ProfileData* profile) {
    return inference_engine->inferir_assinatura(funcao, profile);
}

double TypeSpecializer::taxa_sucesso_guards() const {
    if (total_guard_checks == 0) return 100.0;
    return (double)(total_guard_checks - total_guard_failures) / total_guard_checks * 100.0;
}

void TypeSpecializer::relatorio_estatisticas() {
    std::cout << "=== Relatório de Type Specialization ===" << std::endl;
    std::cout << "Total de especializações: " << total_specializations << std::endl;
    std::cout << "Total de deoptimizações: " << total_deoptimizations << std::endl;
    std::cout << "Funções especializadas ativas: " << funcoes_especializadas.size() << std::endl;
    std::cout << "Verificações de guard: " << total_guard_checks << std::endl;
    std::cout << "Falhas de guard: " << total_guard_failures << std::endl;
    std::cout << "Taxa de sucesso guards: " << taxa_sucesso_guards() << "%" << std::endl;
    
    std::cout << "\n--- Funções Especializadas ---" << std::endl;
    for (const auto& [sig, func] : funcoes_especializadas) {
        std::cout << sig.para_string() << ": "
                  << "usos=" << func.usos
                  << ", hits=" << func.hits
                  << ", misses=" << func.misses
                  << ", hit_rate=" << func.hit_rate() << "%"
                  << ", estavel=" << (func.estavel ? "sim" : "não") << std::endl;
    }
}

void TypeSpecializer::resetar_estatisticas() {
    total_specializations = 0;
    total_deoptimizations = 0;
    total_guard_checks = 0;
    total_guard_failures = 0;
}

void* TypeSpecializer::gerar_codigo_especializado(FuncaoBR* funcao, TypeSignature assinatura) {
    // Gera código especializado para a assinatura de tipo
    // Placeholder - na prática geraria código type-specific
    size_t tamanho_codigo = 1024;
    uint8_t* codigo = (uint8_t*)malloc(tamanho_codigo);
    
    if (!codigo) return nullptr;
    
    // Placeholder: código NOP
    for (size_t i = 0; i < tamanho_codigo; i++) {
        codigo[i] = 0x90;
    }
    
    return codigo;
}

void* TypeSpecializer::gerar_codigo_operador_especializado(const std::string& operador, TypeSignature assinatura) {
    // Gera código especializado para operador
    // Placeholder - na prática geraria código otimizado para tipos específicos
    size_t tamanho_codigo = 512;
    uint8_t* codigo = (uint8_t*)malloc(tamanho_codigo);
    
    if (!codigo) return nullptr;
    
    for (size_t i = 0; i < tamanho_codigo; i++) {
        codigo[i] = 0x90;
    }
    
    return codigo;
}

bool TypeSpecializer::deve_especializar(FuncaoBR* funcao, TypeSignature assinatura) {
    // Só especializar se:
    // - Assinatura tem tipos concretos (não "desconhecido")
    // - Função é hotspot
    // - Tipos são estáveis
    
    for (const auto& tipo : assinatura.tipos_parametros) {
        if (tipo == "desconhecido") return false;
    }
    
    return true;
}

bool TypeSpecializer::assinatura_estavel(TypeSignature sig, ProfileData* profile) {
    // Verifica se assinatura é estável baseado em profiling
    if (!profile) return false;
    
    // Placeholder - na prática analisaria consistência de tipos no profile
    return profile->chamadas_totais > 50;
}
