// Sistema de Pattern Matching para BR v2.0
// Implementa corresponda (pattern matching)
#ifndef PATTERN_MATCHING_H
#define PATTERN_MATCHING_H

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <functional>
#include <map>

// Tipos de patterns
enum TipoPattern {
    PATTERN_LITERAL,
    PATTERN_VARIAVEL,
    PATTERN_WILDCARD,
    PATTERN_LISTA,
    PATTERN_REGISTRO,
    PATTERN_OR,
    PATTERN_AND,
    PATTERN_RANGE,
    PATTERN_TIPO,
    PATTERN_GUARD
};

// Valor genérico para pattern matching
using ValorPattern = std::variant<
    int64_t,
    double,
    std::string,
    bool,
    std::monostate
>;

// Estrutura de pattern
struct Pattern {
    TipoPattern tipo;
    ValorPattern valor;
    std::string nome_variavel;
    std::vector<std::shared_ptr<Pattern>> subpatterns;
    std::function<bool(const ValorPattern&)> guarda;
    
    Pattern() : tipo(PATTERN_WILDCARD) {}
    
    static Pattern literal(ValorPattern v) {
        Pattern p;
        p.tipo = PATTERN_LITERAL;
        p.valor = v;
        return p;
    }
    
    static Pattern variavel(const std::string& nome) {
        Pattern p;
        p.tipo = PATTERN_VARIAVEL;
        p.nome_variavel = nome;
        return p;
    }
    
    static Pattern wildcard() {
        return Pattern();
    }
    
    static Pattern lista(const std::vector<std::shared_ptr<Pattern>>& elementos) {
        Pattern p;
        p.tipo = PATTERN_LISTA;
        p.subpatterns = elementos;
        return p;
    }
    
    static Pattern registro(const std::map<std::string, std::shared_ptr<Pattern>>& campos) {
        Pattern p;
        p.tipo = PATTERN_REGISTRO;
        // Converter mapa para vector de pairs
        for (const auto& [nome, pattern] : campos) {
            p.subpatterns.push_back(pattern);
        }
        return p;
    }
    
    static Pattern ou(const std::vector<std::shared_ptr<Pattern>>& opcoes) {
        Pattern p;
        p.tipo = PATTERN_OR;
        p.subpatterns = opcoes;
        return p;
    }
    
    static Pattern e(const std::vector<std::shared_ptr<Pattern>>& patterns) {
        Pattern p;
        p.tipo = PATTERN_AND;
        p.subpatterns = patterns;
        return p;
    }
    
    static Pattern range(ValorPattern min, ValorPattern max) {
        Pattern p;
        p.tipo = PATTERN_RANGE;
        // Armazenar min e max no valor (simplificado)
        return p;
    }
    
    static Pattern tipo_check(const std::string& tipo_esperado) {
        Pattern p;
        p.tipo = PATTERN_TIPO;
        p.nome_variavel = tipo_esperado;
        return p;
    }
    
    static Pattern com_guarda(std::shared_ptr<Pattern> pattern, 
                               std::function<bool(const ValorPattern&)> guarda) {
        Pattern p;
        p.tipo = PATTERN_GUARD;
        p.subpatterns.push_back(pattern);
        p.guarda = guarda;
        return p;
    }
};

// Resultado do match
struct ResultadoMatch {
    bool sucesso;
    std::map<std::string, ValorPattern> variaveis;
    
    ResultadoMatch() : sucesso(false) {}
    ResultadoMatch(bool s) : sucesso(s) {}
    
    ValorPattern obter(const std::string& nome) const {
        auto it = variaveis.find(nome);
        if (it != variaveis.end()) {
            return it->second;
        }
        return std::monostate{};
    }
    
    bool tem(const std::string& nome) const {
        return variaveis.find(nome) != variaveis.end();
    }
};

// Caso de match (pattern + ação)
struct CasoMatch {
    Pattern pattern;
    std::function<ResultadoMatch(const ValorPattern&)> acao;
    
    CasoMatch() {}
    CasoMatch(Pattern p, std::function<ResultadoMatch(const ValorPattern&)> a)
        : pattern(p), acao(a) {}
};

// Motor de pattern matching
class MotorPatternMatching {
private:
    std::vector<CasoMatch> casos;
    
public:
    MotorPatternMatching() {}
    
    // Adicionar caso
    void adicionar_caso(const Pattern& pattern, 
                       std::function<ResultadoMatch(const ValorPattern&)> acao) {
        CasoMatch caso(pattern, acao);
        casos.push_back(caso);
    }
    
    // Adicionar caso simples (retorna valor)
    template<typename T>
    void adicionar_caso_simples(const Pattern& pattern, T valor_retorno) {
        adicionar_caso(pattern, [valor_retorno](const ValorPattern&) {
            ResultadoMatch res(true);
            return res;
        });
    }
    
    // Executar match
    ResultadoMatch corresponder(const ValorPattern& valor) {
        for (const auto& caso : casos) {
            ResultadoMatch resultado = fazer_match(caso.pattern, valor);
            if (resultado.sucesso) {
                if (caso.acao) {
                    return caso.acao(valor);
                }
                return resultado;
            }
        }
        return ResultadoMatch(false);
    }
    
    // Verificar se valor corresponde ao pattern
    bool verificar(const Pattern& pattern, const ValorPattern& valor) {
        return fazer_match(pattern, valor).sucesso;
    }
    
    // Limpar casos
    void limpar() {
        casos.clear();
    }
    
private:
    ResultadoMatch fazer_match(const Pattern& pattern, const ValorPattern& valor);
    bool comparar_valores(const ValorPattern& a, const ValorPattern& b);
    std::string tipo_do_valor(const ValorPattern& valor);
};

// Funções auxiliares para criar patterns
namespace PatternBR {
    Pattern literal(int64_t v);
    Pattern literal(double v);
    Pattern literal(const std::string& v);
    Pattern literal(bool v);
    
    Pattern var(const std::string& nome);
    Pattern qualquer();
    
    Pattern lista(const std::vector<Pattern>& elementos);
    Pattern lista_vazia();
    Pattern lista_com_cabeca(Pattern cabeca, Pattern cauda);
    
    Pattern registro(const std::map<std::string, Pattern>& campos);
    
    Pattern ou(const std::vector<Pattern>& opcoes);
    Pattern e(const std::vector<Pattern>& patterns);
    
    Pattern range(int64_t min, int64_t max);
    Pattern range(double min, double max);
    
    Pattern tipo_inteiro();
    Pattern tipo_decimal();
    Pattern tipo_texto();
    Pattern tipo_booleano();
    
    Pattern quando(Pattern pattern, std::function<bool(const ValorPattern&)> guarda);
}

// Expressão corresponda para uso no parser
class ExpressaoCorresponda {
private:
    std::string variavel_alvo;
    std::vector<std::pair<Pattern, std::string>> casos;
    std::string caso_padrao;
    
public:
    ExpressaoCorresponda() {}
    
    void definir_alvo(const std::string& var) {
        variavel_alvo = var;
    }
    
    void adicionar_caso(const Pattern& pattern, const std::string& codigo) {
        casos.push_back({pattern, codigo});
    }
    
    void definir_caso_padrao(const std::string& codigo) {
        caso_padrao = codigo;
    }
    
    std::string gerar_codigo_cpp() const;
};

#endif // PATTERN_MATCHING_H
