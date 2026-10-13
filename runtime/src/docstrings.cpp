// Implementação do Sistema de Docstrings Integradas para BR v2.0
#include "docstrings.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <regex>

// Docstring
std::string Docstring::para_markdown() const {
    std::stringstream ss;
    
    if (!resumo.empty()) {
        ss << resumo << "\n\n";
    }
    
    if (!descricao_detalhada.empty()) {
        ss << descricao_detalhada << "\n\n";
    }
    
    if (!parametros.empty()) {
        ss << "## Parâmetros\n\n";
        for (const auto& param : parametros) {
            ss << "- `" << param.nome << "`";
            if (!param.tipo.empty()) {
                ss << " (" << param.tipo << ")";
            }
            if (param.opcional) {
                ss << " [opcional]";
            }
            if (!param.valor_padrao.empty()) {
                ss << " = " << param.valor_padrao;
            }
            ss << ": " << param.descricao << "\n";
        }
        ss << "\n";
    }
    
    if (!retorno.empty()) {
        ss << "## Retorno\n\n";
        ss << retorno << "\n\n";
    }
    
    if (!excecoes.empty()) {
        ss << "## Exceções\n\n";
        for (const auto& exc : excecoes) {
            ss << "- " << exc << "\n";
        }
        ss << "\n";
    }
    
    if (!exemplos.empty()) {
        ss << "## Exemplos\n\n";
        for (const auto& ex : exemplos) {
            ss << "```\n" << ex << "\n```\n\n";
        }
    }
    
    if (!secoes.empty()) {
        for (const auto& secao : secoes) {
            ss << "## " << secao.nome << "\n\n";
            ss << secao.conteudo << "\n\n";
        }
    }
    
    return ss.str();
}

std::string Docstring::para_html() const {
    std::stringstream ss;
    
    ss << "<div class=\"docstring\">\n";
    
    if (!resumo.empty()) {
        ss << "  <p class=\"summary\">" << resumo << "</p>\n";
    }
    
    if (!descricao_detalhada.empty()) {
        ss << "  <div class=\"description\">" << descricao_detalhada << "</div>\n";
    }
    
    if (!parametros.empty()) {
        ss << "  <h3>Parâmetros</h3>\n";
        ss << "  <ul class=\"parameters\">\n";
        for (const auto& param : parametros) {
            ss << "    <li>";
            ss << "<code>" << param.nome << "</code>";
            if (!param.tipo.empty()) {
                ss << " <span class=\"type\">(" << param.tipo << ")</span>";
            }
            if (param.opcional) {
                ss << " <span class=\"optional\">[opcional]</span>";
            }
            ss << ": " << param.descricao << "</li>\n";
        }
        ss << "  </ul>\n";
    }
    
    if (!retorno.empty()) {
        ss << "  <h3>Retorno</h3>\n";
        ss << "  <p>" << retorno << "</p>\n";
    }
    
    if (!excecoes.empty()) {
        ss << "  <h3>Exceções</h3>\n";
        ss << "  <ul class=\"exceptions\">\n";
        for (const auto& exc : excecoes) {
            ss << "    <li>" << exc << "</li>\n";
        }
        ss << "  </ul>\n";
    }
    
    if (!exemplos.empty()) {
        ss << "  <h3>Exemplos</h3>\n";
        for (const auto& ex : exemplos) {
            ss << "  <pre><code>" << ex << "</code></pre>\n";
        }
    }
    
    ss << "</div>\n";
    
    return ss.str();
}

std::string Docstring::para_texto() const {
    std::stringstream ss;
    
    if (!resumo.empty()) {
        ss << resumo << "\n\n";
    }
    
    if (!descricao_detalhada.empty()) {
        ss << descricao_detalhada << "\n\n";
    }
    
    if (!parametros.empty()) {
        ss << "Parâmetros:\n";
        for (const auto& param : parametros) {
            ss << "  " << param.nome;
            if (!param.tipo.empty()) {
                ss << " (" << param.tipo << ")";
            }
            if (param.opcional) {
                ss << " [opcional]";
            }
            ss << ": " << param.descricao << "\n";
        }
        ss << "\n";
    }
    
    if (!retorno.empty()) {
        ss << "Retorno: " << retorno << "\n\n";
    }
    
    if (!excecoes.empty()) {
        ss << "Exceções:\n";
        for (const auto& exc : excecoes) {
            ss << "  - " << exc << "\n";
        }
        ss << "\n";
    }
    
    if (!exemplos.empty()) {
        ss << "Exemplos:\n";
        for (const auto& ex : exemplos) {
            ss << "  " << ex << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

// ParserDocstrings
ParserDocstrings::ParserDocstrings() {
}

bool ParserDocstrings::parsear_arquivo(const std::string& caminho_arquivo) {
    std::ifstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        adicionar_erro("Não foi possível abrir arquivo: " + caminho_arquivo);
        return false;
    }
    
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    std::string codigo = buffer.str();
    arquivo.close();
    
    return parsear_codigo(codigo);
}

bool ParserDocstrings::parsear_codigo(const std::string& codigo) {
    std::stringstream ss(codigo);
    std::string linha;
    int num_linha = 0;
    
    while (std::getline(ss, linha)) {
        num_linha++;
        
        // Verificar se linha tem docstring
        if (DocstringBR::tem_docstring(linha)) {
            std::string docstring_texto = DocstringBR::extrair(linha);
            Docstring doc = parsear_docstring(docstring_texto);
            
            // Tentar identificar elemento associado
            ElementoDoc elemento;
            elemento.docstring = doc;
            elemento.linha = num_linha;
            
            // Parser simplificado - em produção seria mais robusto
            if (linha.find("função") != std::string::npos || linha.find("funcao") != std::string::npos) {
                elemento.tipo = DOC_FUNCAO;
                // Extrair nome da função
                size_t pos = linha.find("função");
                if (pos == std::string::npos) pos = linha.find("funcao");
                std::string resto = linha.substr(pos + 7);
                resto.erase(0, resto.find_first_not_of(" \t"));
                size_t fim = resto.find_first_of(" \t(");
                if (fim != std::string::npos) {
                    elemento.nome = resto.substr(0, fim);
                }
            } else if (linha.find("classe") != std::string::npos) {
                elemento.tipo = DOC_CLASSE;
                size_t pos = linha.find("classe");
                std::string resto = linha.substr(pos + 6);
                resto.erase(0, resto.find_first_not_of(" \t"));
                size_t fim = resto.find_first_of(" \t");
                if (fim != std::string::npos) {
                    elemento.nome = resto.substr(0, fim);
                }
            }
            
            if (!elemento.nome.empty()) {
                adicionar_elemento(elemento);
            }
        }
    }
    
    return true;
}

Docstring ParserDocstrings::parsear_docstring(const std::string& texto_docstring) {
    Docstring doc;
    
    std::vector<std::string> linhas = parsear_linhas_docstring(texto_docstring);
    
    if (linhas.empty()) {
        return doc;
    }
    
    // Primeira linha é o resumo
    doc.resumo = linhas[0];
    
    // Parser simplificado de seções
    std::string secao_atual;
    std::string conteudo_secao;
    
    for (size_t i = 1; i < linhas.size(); i++) {
        std::string linha = linhas[i];
        
        // Verificar se é início de seção
        if (linha.size() > 0 && linha[0] == '@') {
            // Salvar seção anterior
            if (!secao_atual.empty() && !conteudo_secao.empty()) {
                doc.secoes.push_back(SecaoDoc(secao_atual, conteudo_secao));
            }
            
            // Nova seção
            size_t espaco = linha.find(' ');
            if (espaco != std::string::npos) {
                secao_atual = linha.substr(1, espaco - 1);
                conteudo_secao = linha.substr(espaco + 1);
            } else {
                secao_atual = linha.substr(1);
                conteudo_secao.clear();
            }
        } else {
            // Continuar seção atual
            if (!secao_atual.empty()) {
                if (!conteudo_secao.empty()) {
                    conteudo_secao += " ";
                }
                conteudo_secao += linha;
            } else {
                // Descrição detalhada
                if (!doc.descricao_detalhada.empty()) {
                    doc.descricao_detalhada += " ";
                }
                doc.descricao_detalhada += linha;
            }
        }
    }
    
    // Salvar última seção
    if (!secao_atual.empty() && !conteudo_secao.empty()) {
        doc.secoes.push_back(SecaoDoc(secao_atual, conteudo_secao));
    }
    
    // Parser de seções específicas
    for (const auto& secao : doc.secoes) {
        if (secao.nome == "param" || secao.nome == "parametro") {
            // Parser de parâmetro: nome tipo descricao
            std::stringstream ss(secao.conteudo);
            std::string nome, tipo, descricao;
            ss >> nome >> tipo;
            std::getline(ss, descricao);
            
            ParametroDoc param;
            param.nome = nome;
            param.tipo = tipo;
            param.descricao = descricao;
            doc.parametros.push_back(param);
        } else if (secao.nome == "retorno" || secao.nome == "retorna") {
            doc.retorno = secao.conteudo;
        } else if (secao.nome == "exemplo") {
            doc.exemplos.push_back(secao.conteudo);
        } else if (secao.nome == "excecao" || secao.nome == "lanca") {
            doc.excecoes.push_back(secao.conteudo);
        }
    }
    
    return doc;
}

std::vector<ElementoDoc> ParserDocstrings::obter_elementos() const {
    std::vector<ElementoDoc> resultado;
    for (const auto& [nome, elemento] : elementos) {
        resultado.push_back(elemento);
    }
    return resultado;
}

ElementoDoc ParserDocstrings::obter_elemento(const std::string& nome) const {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        return it->second;
    }
    return ElementoDoc();
}

void ParserDocstrings::adicionar_elemento(const ElementoDoc& elemento) {
    elementos[elemento.nome] = elemento;
}

void ParserDocstrings::adicionar_erro(const std::string& erro) {
    erros.push_back(erro);
}

std::vector<std::string> ParserDocstrings::parsear_linhas_docstring(const std::string& bloco) {
    std::vector<std::string> linhas;
    std::stringstream ss(bloco);
    std::string linha;
    
    while (std::getline(ss, linha)) {
        // Remover aspas se presentes
        if (linha.size() >= 2 && linha[0] == '"' && linha.back() == '"') {
            linha = linha.substr(1, linha.size() - 2);
        }
        linhas.push_back(linha);
    }
    
    return linhas;
}

// GeradorDocumentacao
std::string GeradorDocumentacao::gerar_markdown(const std::vector<ElementoDoc>& elementos) {
    std::stringstream ss;
    
    ss << "# Documentação BR\n\n";
    
    for (const auto& elemento : elementos) {
        ss << "## " << formatar_tipo_elemento(elemento.tipo) << ": " << elemento.nome << "\n\n";
        ss << elemento.docstring.para_markdown() << "\n";
        ss << "---\n\n";
    }
    
    return ss.str();
}

std::string GeradorDocumentacao::gerar_html(const std::vector<ElementoDoc>& elementos) {
    std::stringstream ss;
    
    ss << "<html>\n<head>\n<title>Documentação BR</title>\n";
    ss << "<style>\n";
    ss << ".docstring { margin: 20px; padding: 15px; border: 1px solid #ddd; }\n";
    ss << ".summary { font-weight: bold; }\n";
    ss << "</style>\n";
    ss << "</head>\n<body>\n";
    ss << "<h1>Documentação BR</h1>\n\n";
    
    for (const auto& elemento : elementos) {
        ss << "<h2>" << formatar_tipo_elemento(elemento.tipo) << ": " << elemento.nome << "</h2>\n";
        ss << elemento.docstring.para_html() << "\n";
        ss << "<hr>\n";
    }
    
    ss << "</body>\n</html>\n";
    
    return ss.str();
}

std::string GeradorDocumentacao::gerar_texto(const std::vector<ElementoDoc>& elementos) {
    std::stringstream ss;
    
    ss << "Documentação BR\n";
    ss << "================\n\n";
    
    for (const auto& elemento : elementos) {
        ss << formatar_tipo_elemento(elemento.tipo) << ": " << elemento.nome << "\n";
        ss << std::string(elemento.nome.size() + 20, '=') << "\n\n";
        ss << elemento.docstring.para_texto() << "\n";
        ss << "\n";
    }
    
    return ss.str();
}

std::string GeradorDocumentacao::gerar_json(const std::vector<ElementoDoc>& elementos) {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"elementos\": [\n";
    
    for (size_t i = 0; i < elementos.size(); i++) {
        const auto& el = elementos[i];
        ss << "    {\n";
        ss << "      \"tipo\": \"" << formatar_tipo_elemento(el.tipo) << "\",\n";
        ss << "      \"nome\": \"" << el.nome << "\",\n";
        ss << "      \"resumo\": \"" << el.docstring.resumo << "\",\n";
        ss << "      \"descricao\": \"" << el.docstring.descricao_detalhada << "\"\n";
        ss << "    }";
        if (i < elementos.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}\n";
    
    return ss.str();
}

std::string GeradorDocumentacao::gerar_doc_elemento(const ElementoDoc& elemento, const std::string& formato) {
    if (formato == "markdown") {
        return elemento.docstring.para_markdown();
    } else if (formato == "html") {
        return elemento.docstring.para_html();
    } else {
        return elemento.docstring.para_texto();
    }
}

std::string GeradorDocumentacao::gerar_indice(const std::vector<ElementoDoc>& elementos) {
    std::stringstream ss;
    
    ss << "# Índice\n\n";
    
    // Agrupar por tipo
    std::map<TipoElementoDoc, std::vector<ElementoDoc>> agrupado;
    for (const auto& el : elementos) {
        agrupado[el.tipo].push_back(el);
    }
    
    for (const auto& [tipo, elems] : agrupado) {
        ss << "## " << formatar_tipo_elemento(tipo) << "\n\n";
        for (const auto& el : elems) {
            ss << "- [" << el.nome << "](#" << el.nome << ")\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string GeradorDocumentacao::formatar_tipo_elemento(TipoElementoDoc tipo) {
    switch (tipo) {
        case DOC_FUNCAO: return "Função";
        case DOC_CLASSE: return "Classe";
        case DOC_METODO: return "Método";
        case DOC_VARIAVEL: return "Variável";
        case DOC_CONSTANTE: return "Constante";
        case DOC_MODULO: return "Módulo";
        case DOC_PARAMETRO: return "Parâmetro";
        default: return "Elemento";
    }
}

// SistemaAjuda
SistemaAjuda::SistemaAjuda() : formato_saida("texto") {
}

void SistemaAjuda::carregar_do_parser(const ParserDocstrings& parser) {
    auto elems = parser.obter_elementos();
    for (const auto& el : elems) {
        elementos[el.nome] = el;
    }
}

void SistemaAjuda::carregar_arquivo(const std::string& caminho) {
    ParserDocstrings parser;
    parser.parsear_arquivo(caminho);
    carregar_do_parser(parser);
}

std::string SistemaAjuda::ajuda(const std::string& nome) {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        return GeradorDocumentacao::gerar_doc_elemento(it->second, formato_saida);
    }
    return "Elemento não encontrado: " + nome;
}

std::string SistemaAjuda::ajuda_resumo(const std::string& nome) {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        return it->second.docstring.resumo;
    }
    return "";
}

std::string SistemaAjuda::ajuda_parametros(const std::string& nome) {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        std::stringstream ss;
        for (const auto& param : it->second.docstring.parametros) {
            ss << param.nome << " (" << param.tipo << "): " << param.descricao << "\n";
        }
        return ss.str();
    }
    return "";
}

std::string SistemaAjuda::ajuda_retorno(const std::string& nome) {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        return it->second.docstring.retorno;
    }
    return "";
}

std::string SistemaAjuda::ajuda_exemplos(const std::string& nome) {
    auto it = elementos.find(nome);
    if (it != elementos.end()) {
        std::stringstream ss;
        for (const auto& ex : it->second.docstring.exemplos) {
            ss << ex << "\n\n";
        }
        return ss.str();
    }
    return "";
}

std::vector<std::string> SistemaAjuda::buscar(const std::string& termo) {
    std::vector<std::string> resultados;
    std::string termo_lower = termo;
    std::transform(termo_lower.begin(), termo_lower.end(), termo_lower.begin(), ::tolower);
    
    for (const auto& [nome, elemento] : elementos) {
        std::string nome_lower = nome;
        std::transform(nome_lower.begin(), nome_lower.end(), nome_lower.begin(), ::tolower);
        
        if (nome_lower.find(termo_lower) != std::string::npos ||
            elemento.docstring.resumo.find(termo) != std::string::npos) {
            resultados.push_back(nome);
        }
    }
    
    return resultados;
}

std::vector<std::string> SistemaAjuda::listar_funcoes() {
    std::vector<std::string> funcoes;
    for (const auto& [nome, elemento] : elementos) {
        if (elemento.tipo == DOC_FUNCAO) {
            funcoes.push_back(nome);
        }
    }
    return funcoes;
}

std::vector<std::string> SistemaAjuda::listar_classes() {
    std::vector<std::string> classes;
    for (const auto& [nome, elemento] : elementos) {
        if (elemento.tipo == DOC_CLASSE) {
            classes.push_back(nome);
        }
    }
    return classes;
}

std::vector<std::string> SistemaAjuda::listar_modulos() {
    std::vector<std::string> modulos;
    for (const auto& [nome, elemento] : elementos) {
        if (elemento.tipo == DOC_MODULO) {
            modulos.push_back(nome);
        }
    }
    return modulos;
}

void SistemaAjuda::definir_formato_saida(const std::string& formato) {
    formato_saida = formato;
}

// Funções auxiliares DocstringBR
namespace DocstringBR {
    std::string extrair(const std::string& linha) {
        size_t inicio = linha.find('"');
        if (inicio == std::string::npos) inicio = linha.find('\'');
        
        if (inicio != std::string::npos) {
            char quote = linha[inicio];
            size_t fim = linha.find(quote, inicio + 1);
            if (fim != std::string::npos) {
                return linha.substr(inicio + 1, fim - inicio - 1);
            }
        }
        
        return "";
    }
    
    bool tem_docstring(const std::string& linha) {
        // Verificar padrão: """ ou ''' ou " no início da linha
        std::string trimmed = linha;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        return (trimmed.find("\"\"\"") == 0 || trimmed.find("'''") == 0 ||
                (trimmed.size() > 0 && trimmed[0] == '"' && trimmed.find("\"", 1) != std::string::npos));
    }
    
    std::string formatar(const Docstring& doc) {
        return doc.para_texto();
    }
    
    bool validar(const std::string& docstring) {
        // Validação básica: não vazia
        return !docstring.empty();
    }
}
