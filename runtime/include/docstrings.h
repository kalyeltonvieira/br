// Sistema de Docstrings Integradas para BR v2.0
// Implementa documentação embutida no código
#ifndef DOCSTRINGS_H
#define DOCSTRINGS_H

#include <string>
#include <vector>
#include <map>
#include <memory>

// Tipos de elementos documentados
enum TipoElementoDoc {
    DOC_FUNCAO,
    DOC_CLASSE,
    DOC_METODO,
    DOC_VARIAVEL,
    DOC_CONSTANTE,
    DOC_MODULO,
    DOC_PARAMETRO
};

// Seção de documentação
struct SecaoDoc {
    std::string nome;
    std::string conteudo;
    
    SecaoDoc() {}
    SecaoDoc(const std::string& n, const std::string& c) : nome(n), conteudo(c) {}
};

// Parâmetro documentado
struct ParametroDoc {
    std::string nome;
    std::string tipo;
    std::string descricao;
    bool opcional;
    std::string valor_padrao;
    
    ParametroDoc() : opcional(false) {}
};

// Docstring completa
struct Docstring {
    std::string resumo;
    std::string descricao_detalhada;
    std::vector<SecaoDoc> secoes;
    std::vector<ParametroDoc> parametros;
    std::string retorno;
    std::vector<std::string> excecoes;
    std::vector<std::string> exemplos;
    std::string versao;
    std::string autor;
    std::string desde;
    
    Docstring() {}
    
    std::string para_markdown() const;
    std::string para_html() const;
    std::string para_texto() const;
};

// Elemento documentado
struct ElementoDoc {
    TipoElementoDoc tipo;
    std::string nome;
    std::string nome_completo; // incluindo módulo/classe
    Docstring docstring;
    int linha;
    int coluna;
    std::string arquivo;
    
    ElementoDoc() : linha(0), coluna(0) {}
};

// Parser de docstrings
class ParserDocstrings {
private:
    std::map<std::string, ElementoDoc> elementos;
    std::vector<std::string> erros;
    
public:
    ParserDocstrings();
    
    // Parser de código BR
    bool parsear_arquivo(const std::string& caminho_arquivo);
    bool parsear_codigo(const std::string& codigo);
    
    // Parser de docstring individual
    Docstring parsear_docstring(const std::string& texto_docstring);
    
    // Obter elementos documentados
    std::vector<ElementoDoc> obter_elementos() const;
    ElementoDoc obter_elemento(const std::string& nome) const;
    
    // Obter erros
    std::vector<std::string> obter_erros() const { return erros; }
    
private:
    void adicionar_elemento(const ElementoDoc& elemento);
    void adicionar_erro(const std::string& erro);
    std::string extrair_docstring(const std::string& linha);
    std::vector<std::string> parsear_linhas_docstring(const std::string& bloco);
};

// Gerador de documentação
class GeradorDocumentacao {
public:
    // Gerar documentação em diferentes formatos
    static std::string gerar_markdown(const std::vector<ElementoDoc>& elementos);
    static std::string gerar_html(const std::vector<ElementoDoc>& elementos);
    static std::string gerar_texto(const std::vector<ElementoDoc>& elementos);
    static std::string gerar_json(const std::vector<ElementoDoc>& elementos);
    
    // Gerar documentação para um elemento específico
    static std::string gerar_doc_elemento(const ElementoDoc& elemento, const std::string& formato);
    
    // Gerar índice
    static std::string gerar_indice(const std::vector<ElementoDoc>& elementos);
    
private:
    static std::string formatar_tipo_elemento(TipoElementoDoc tipo);
};

// Sistema de ajuda em tempo de execução
class SistemaAjuda {
private:
    std::map<std::string, ElementoDoc> elementos;
    
public:
    SistemaAjuda();
    
    // Carregar documentação
    void carregar_do_parser(const ParserDocstrings& parser);
    void carregar_arquivo(const std::string& caminho);
    
    // Consultas
    std::string ajuda(const std::string& nome);
    std::string ajuda_resumo(const std::string& nome);
    std::string ajuda_parametros(const std::string& nome);
    std::string ajuda_retorno(const std::string& nome);
    std::string ajuda_exemplos(const std::string& nome);
    
    // Busca
    std::vector<std::string> buscar(const std::string& termo);
    std::vector<std::string> listar_funcoes();
    std::vector<std::string> listar_classes();
    std::vector<std::string> listar_modulos();
    
    // Formatação
    void definir_formato_saida(const std::string& formato); // "texto", "markdown", "html"
    
private:
    std::string formato_saida;
};

// Funções auxiliares para docstrings no parser BR
namespace DocstringBR {
    // Extrair docstring de uma linha de código
    std::string extrair(const std::string& linha);
    
    // Verificar se linha contém docstring
    bool tem_docstring(const std::string& linha);
    
    // Formatar docstring para exibição
    std::string formatar(const Docstring& doc);
    
    // Validar formato de docstring
    bool validar(const std::string& docstring);
}

#endif // DOCSTRINGS_H
