// Type Specialization para Linguagem BR
// Specialização de funções baseada em tipos inferidos
#ifndef TYPE_SPECIALIZATION_H
#define TYPE_SPECIALIZATION_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

// Forward declarations
struct NoAST;
struct FuncaoBR;
struct Valor;
struct ProfileData;

// Assinatura de tipo
struct TypeSignature {
    std::string nome_funcao;
    std::vector<std::string> tipos_parametros;
    std::string tipo_retorno;
    
    TypeSignature() {}
    TypeSignature(const std::string& nome, const std::vector<std::string>& params, const std::string& ret)
        : nome_funcao(nome), tipos_parametros(params), tipo_retorno(ret) {}
    
    bool operator==(const TypeSignature& other) const {
        if (nome_funcao != other.nome_funcao) return false;
        if (tipos_parametros.size() != other.tipos_parametros.size()) return false;
        if (tipo_retorno != other.tipo_retorno) return false;
        
        for (size_t i = 0; i < tipos_parametros.size(); i++) {
            if (tipos_parametros[i] != other.tipos_parametros[i]) return false;
        }
        
        return true;
    }
    
    std::string para_string() const {
        std::string sig = nome_funcao + "(";
        for (size_t i = 0; i < tipos_parametros.size(); i++) {
            if (i > 0) sig += ", ";
            sig += tipos_parametros[i];
        }
        sig += ") -> " + tipo_retorno;
        return sig;
    }
};

// Hash para TypeSignature
namespace standard {
    template<>
    struct hash<TypeSignature> {
        size_t operator()(const TypeSignature& sig) const {
            size_t h = hash<string>{}(sig.nome_funcao);
            for (const auto& tipo : sig.tipos_parametros) {
                h ^= hash<string>{}(tipo) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            h ^= hash<string>{}(sig.tipo_retorno) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

// Função especializada
struct FuncaoEspecializada {
    TypeSignature assinatura;
    void* codigo_compilado;
    int usos;
    int hits;
    int misses;
    bool estavel;
    
    FuncaoEspecializada() : codigo_compilado(nullptr), usos(0), hits(0), misses(0), estavel(false) {}
    
    double hit_rate() const {
        int total = hits + misses;
        return total > 0 ? (double)hits / total * 100.0 : 0.0;
    }
};

// Motor de inferência de tipos
class TypeInferenceEngine {
private:
    // Tipos inferidos por variável
    std::map<std::string, std::string> tipos_variaveis;
    
    // Tipos inferidos por expressão
    std::map<std::string, std::string> tipos_expressoes;
    
    // Confiança em cada inferência (0-100)
    std::map<std::string, int> confianca_tipos;
    
public:
    TypeInferenceEngine();
    ~TypeInferenceEngine();
    
    // Inferência de tipos
    std::string inferir_tipo(NoAST* no);
    std::string inferir_tipo_variavel(const std::string& nome);
    std::string inferir_tipo_expressao(NoAST* expr);
    
    // Inferência baseada em profiling
    TypeSignature inferir_assinatura(FuncaoBR* funcao, ProfileData* profile);
    
    // Registro de tipos
    void registrar_tipo_variavel(const std::string& nome, const std::string& tipo, int confianca = 100);
    void registrar_tipo_expressao(const std::string& id, const std::string& tipo, int confianca = 100);
    
    // Verificação de estabilidade
    bool tipo_estavel(const std::string& nome);
    bool tipo_mudou(const std::string& nome, const std::string& novo_tipo);
    
    // Limpeza
    void limpar();
    void limpar_variavel(const std::string& nome);
    
    // Estatísticas
    size_t obter_total_inferencias() const { return tipos_variaveis.size() + tipos_expressoes.size(); }
    double obter_confianca_media() const;
    
private:
    std::string inferir_tipo_literal(NoAST* no);
    std::string inferir_tipo_operacao(NoAST* no);
    std::string inferir_tipo_chamada(NoAST* no);
    std::string inferir_tipo_acesso(NoAST* no);
};

// Specializer principal
class TypeSpecializer {
private:
    TypeInferenceEngine* inference_engine;
    
    // Funções especializadas
    std::map<TypeSignature, FuncaoEspecializada> funcoes_especializadas;
    
    // Guards para deoptimização
    std::map<TypeSignature, std::vector<std::function<bool()>>> guards;
    
    // Estatísticas
    int total_specializations;
    int total_deoptimizations;
    int total_guard_checks;
    int total_guard_failures;
    
public:
    TypeSpecializer();
    ~TypeSpecializer();
    
    // Specialização
    void* especializar_funcao(FuncaoBR* funcao, TypeSignature assinatura);
    void* especializar_operador(const std::string& operador, TypeSignature assinatura);
    
    // Lookup de função especializada
    void* buscar_funcao_especializada(TypeSignature assinatura);
    void* buscar_ou_criar_especializada(FuncaoBR* funcao, TypeSignature assinatura);
    
    // Guards
    void adicionar_guard(TypeSignature sig, std::function<bool()> guard);
    bool verificar_guards(TypeSignature sig);
    
    // Deoptimization
    void deoptimizar(TypeSignature sig);
    void deoptimizar_tudo();
    
    // Type inference
    TypeInferenceEngine* obter_inference_engine() { return inference_engine; }
    TypeSignature inferir_assinatura(FuncaoBR* funcao, ProfileData* profile);
    
    // Estatísticas
    int obter_total_specializations() const { return total_specializations; }
    int obter_total_deoptimizations() const { return total_deoptimizations; }
    int obter_total_guard_checks() const { return total_guard_checks; }
    int obter_total_guard_failures() const { return total_guard_failures; }
    
    double taxa_sucesso_guards() const;
    size_t obter_total_funcoes_especializadas() const { return funcoes_especializadas.size(); }
    
    void relatorio_estatisticas();
    void resetar_estatisticas();
    
private:
    void* gerar_codigo_especializado(FuncaoBR* funcao, TypeSignature assinatura);
    void* gerar_codigo_operador_especializado(const std::string& operador, TypeSignature assinatura);
    
    bool deve_especializar(FuncaoBR* funcao, TypeSignature assinatura);
    bool assinatura_estavel(TypeSignature sig, ProfileData* profile);
};

// Contexto de especialização para compilação
struct SpecializationContext {
    TypeSpecializer* specializer;
    TypeSignature assinatura_alvo;
    std::map<std::string, std::string> tipos_conhecidos;
    
    SpecializationContext() : specializer(nullptr) {}
    
    std::string obter_tipo_variavel(const std::string& nome) {
        auto it = tipos_conhecidos.find(nome);
        return it != tipos_conhecidos.end() ? it->second : "desconhecido";
    }
    
    void definir_tipo_variavel(const std::string& nome, const std::string& tipo) {
        tipos_conhecidos[nome] = tipo;
    }
};

#endif // TYPE_SPECIALIZATION_H
