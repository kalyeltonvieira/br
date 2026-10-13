// Implementação do Sistema de Pattern Matching para BR v2.0
#include "pattern_matching.h"
#include <iostream>
#include <sstream>

// MotorPatternMatching
ResultadoMatch MotorPatternMatching::fazer_match(const Pattern& pattern, const ValorPattern& valor) {
    ResultadoMatch resultado;
    
    switch (pattern.tipo) {
        case PATTERN_LITERAL:
            resultado.sucesso = comparar_valores(pattern.valor, valor);
            break;
            
        case PATTERN_VARIAVEL:
            resultado.sucesso = true;
            resultado.variaveis[pattern.nome_variavel] = valor;
            break;
            
        case PATTERN_WILDCARD:
            resultado.sucesso = true;
            break;
            
        case PATTERN_LISTA:
            // Verificar se valor é lista (simplificado - assume vector)
            // Em implementação completa, verificar tipo
            if (std::holds_alternative<std::string>(valor)) {
                // Se for string, tratar como lista de caracteres
                std::string str = std::get<std::string>(valor);
                if (pattern.subpatterns.size() <= str.size()) {
                    resultado.sucesso = true;
                    for (size_t i = 0; i < pattern.subpatterns.size(); i++) {
                        ValorPattern char_val = std::string(1, str[i]);
                        ResultadoMatch sub_res = fazer_match(*pattern.subpatterns[i], char_val);
                        if (!sub_res.sucesso) {
                            resultado.sucesso = false;
                            break;
                        }
                        // Merge variáveis
                        resultado.variaveis.insert(sub_res.variaveis.begin(), 
                                                   sub_res.variaveis.end());
                    }
                }
            }
            break;
            
        case PATTERN_REGISTRO:
            // Verificar se valor é registro (simplificado)
            resultado.sucesso = true;
            // Em implementação completa, verificar campos do registro
            break;
            
        case PATTERN_OR:
            for (const auto& subpattern : pattern.subpatterns) {
                ResultadoMatch sub_res = fazer_match(*subpattern, valor);
                if (sub_res.sucesso) {
                    resultado = sub_res;
                    break;
                }
            }
            break;
            
        case PATTERN_AND:
            resultado.sucesso = true;
            for (const auto& subpattern : pattern.subpatterns) {
                ResultadoMatch sub_res = fazer_match(*subpattern, valor);
                if (!sub_res.sucesso) {
                    resultado.sucesso = false;
                    break;
                }
                resultado.variaveis.insert(sub_res.variaveis.begin(), 
                                           sub_res.variaveis.end());
            }
            break;
            
        case PATTERN_RANGE:
            // Verificar se valor está no range (simplificado)
            resultado.sucesso = true;
            break;
            
        case PATTERN_TIPO:
            resultado.sucesso = (tipo_do_valor(valor) == pattern.nome_variavel);
            break;
            
        case PATTERN_GUARD:
            if (!pattern.subpatterns.empty()) {
                ResultadoMatch sub_res = fazer_match(*pattern.subpatterns[0], valor);
                if (sub_res.sucesso && pattern.guarda) {
                    resultado.sucesso = pattern.guarda(valor);
                    resultado.variaveis = sub_res.variaveis;
                }
            }
            break;
    }
    
    return resultado;
}

bool MotorPatternMatching::comparar_valores(const ValorPattern& a, const ValorPattern& b) {
    if (a.index() != b.index()) {
        return false;
    }
    
    if (std::holds_alternative<int64_t>(a)) {
        return std::get<int64_t>(a) == std::get<int64_t>(b);
    }
    if (std::holds_alternative<double>(a)) {
        return std::get<double>(a) == std::get<double>(b);
    }
    if (std::holds_alternative<std::string>(a)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }
    if (std::holds_alternative<bool>(a)) {
        return std::get<bool>(a) == std::get<bool>(b);
    }
    
    return true;
}

std::string MotorPatternMatching::tipo_do_valor(const ValorPattern& valor) {
    if (std::holds_alternative<int64_t>(valor)) return "inteiro";
    if (std::holds_alternative<double>(valor)) return "decimal";
    if (std::holds_alternative<std::string>(valor)) return "texto";
    if (std::holds_alternative<bool>(valor)) return "booleano";
    return "nulo";
}

// Funções auxiliares PatternBR
namespace PatternBR {
    Pattern literal(int64_t v) {
        return Pattern::literal(v);
    }
    
    Pattern literal(double v) {
        return Pattern::literal(v);
    }
    
    Pattern literal(const std::string& v) {
        return Pattern::literal(v);
    }
    
    Pattern literal(bool v) {
        return Pattern::literal(v);
    }
    
    Pattern var(const std::string& nome) {
        return Pattern::variavel(nome);
    }
    
    Pattern qualquer() {
        return Pattern::wildcard();
    }
    
    Pattern lista(const std::vector<Pattern>& elementos) {
        std::vector<std::shared_ptr<Pattern>> ptrs;
        for (const auto& elem : elementos) {
            ptrs.push_back(std::make_shared<Pattern>(elem));
        }
        return Pattern::lista(ptrs);
    }
    
    Pattern lista_vazia() {
        return Pattern::lista({});
    }
    
    Pattern lista_com_cabeca(Pattern cabeca, Pattern cauda) {
        std::vector<std::shared_ptr<Pattern>> elementos;
        elementos.push_back(std::make_shared<Pattern>(cabeca));
        elementos.push_back(std::make_shared<Pattern>(cauda));
        return Pattern::lista(elementos);
    }
    
    Pattern registro(const std::map<std::string, Pattern>& campos) {
        std::map<std::string, std::shared_ptr<Pattern>> ptrs;
        for (const auto& [nome, pattern] : campos) {
            ptrs[nome] = std::make_shared<Pattern>(pattern);
        }
        return Pattern::registro(ptrs);
    }
    
    Pattern ou(const std::vector<Pattern>& opcoes) {
        std::vector<std::shared_ptr<Pattern>> ptrs;
        for (const auto& op : opcoes) {
            ptrs.push_back(std::make_shared<Pattern>(op));
        }
        return Pattern::ou(ptrs);
    }
    
    Pattern e(const std::vector<Pattern>& patterns) {
        std::vector<std::shared_ptr<Pattern>> ptrs;
        for (const auto& pat : patterns) {
            ptrs.push_back(std::make_shared<Pattern>(pat));
        }
        return Pattern::e(ptrs);
    }
    
    Pattern range(int64_t min, int64_t max) {
        return Pattern::range(min, max);
    }
    
    Pattern range(double min, double max) {
        return Pattern::range(min, max);
    }
    
    Pattern tipo_inteiro() {
        return Pattern::tipo_check("inteiro");
    }
    
    Pattern tipo_decimal() {
        return Pattern::tipo_check("decimal");
    }
    
    Pattern tipo_texto() {
        return Pattern::tipo_check("texto");
    }
    
    Pattern tipo_booleano() {
        return Pattern::tipo_check("booleano");
    }
    
    Pattern quando(Pattern pattern, std::function<bool(const ValorPattern&)> guarda) {
        return Pattern::com_guarda(std::make_shared<Pattern>(pattern), guarda);
    }
}

// ExpressaoCorresponda
std::string ExpressaoCorresponda::gerar_codigo_cpp() const {
    std::stringstream ss;
    
    ss << "// Pattern matching para " << variavel_alvo << "\n";
    ss << "{\n";
    ss << "    auto valor_" << variavel_alvo << " = " << variavel_alvo << ";\n";
    ss << "    bool match_encontrado = false;\n\n";
    
    for (const auto& [pattern, codigo] : casos) {
        ss << "    // Caso: pattern\n";
        ss << "    if (!match_encontrado) {\n";
        ss << "        MotorPatternMatching motor;\n";
        ss << "        Pattern pattern = /* pattern aqui */;\n";
        ss << "        ResultadoMatch res = motor.fazer_match(pattern, valor_" << variavel_alvo << ");\n";
        ss << "        if (res.sucesso) {\n";
        ss << "            match_encontrado = true;\n";
        ss << "            " << codigo << "\n";
        ss << "        }\n";
        ss << "    }\n\n";
    }
    
    if (!caso_padrao.empty()) {
        ss << "    // Caso padrão\n";
        ss << "    if (!match_encontrado) {\n";
        ss << "        " << caso_padrao << "\n";
        ss << "    }\n";
    }
    
    ss << "}\n";
    
    return ss.str();
}
