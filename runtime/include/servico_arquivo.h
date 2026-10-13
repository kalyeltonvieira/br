// Serviço de Arquivos Nativo BR
#ifndef SERVICO_ARQUIVO_H
#define SERVICO_ARQUIVO_H

#include <string>
#include <vector>
#include <map>

class ServicoArquivo {
public:
    // Operações básicas
    static std::string ler(const std::string& caminho);
    static bool escrever(const std::string& caminho, const std::string& conteudo);
    static bool anexar(const std::string& caminho, const std::string& conteudo);
    
    // Informações
    static bool existe(const std::string& caminho);
    static size_t tamanho(const std::string& caminho);
    static std::string modificado_em(const std::string& caminho);
    static bool eh_arquivo(const std::string& caminho);
    static bool eh_diretorio(const std::string& caminho);
    
    // Diretórios
    static bool criar_diretorio(const std::string& caminho);
    static std::vector<std::string> listar(const std::string& caminho);
    
    // Operações
    static bool copiar(const std::string& origem, const std::string& destino);
    static bool mover(const std::string& origem, const std::string& destino);
    static bool remover(const std::string& caminho);
    static bool remover_diretorio(const std::string& caminho);
    
    // Caminhos
    static std::string caminho_absoluto(const std::string& caminho);
    static std::string caminho_relativo(const std::string& caminho, const std::string& base);
    
    // Permissões
    static bool pode_ler(const std::string& caminho);
    static bool pode_escrever(const std::string& caminho);
    static bool pode_executar(const std::string& caminho);
    static bool definir_permissao(const std::string& caminho, const std::string& permissao);
    
    // Sistema de arquivos
    static size_t espaco_disponivel(const std::string& caminho);
    static size_t espaco_total(const std::string& caminho);
    static size_t espaco_usado(const std::string& caminho);
    
    // Utilitários
    static std::string criar_arquivo_temporario(const std::string& prefixo = "br_");
    static std::string criar_diretorio_temporario(const std::string& prefixo = "br_");
    
    // Manipulação de caminhos
    static std::string nome_arquivo(const std::string& caminho);
    static std::string diretorio_pai(const std::string& caminho);
    static std::string extensao(const std::string& caminho);
    
    // Validação
    static bool eh_caminho_absoluto(const std::string& caminho);
    
private:
    static std::string normalizar_caminho(const std::string& caminho);
    static std::vector<std::string> dividir_caminho(const std::string& caminho);
    static std::string juntar_caminho(const std::vector<std::string>& partes);
    
    static void garantir_diretorio_pai(const std::string& caminho);
};

#endif // SERVICO_ARQUIVO_H