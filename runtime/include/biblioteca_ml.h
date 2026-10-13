// Biblioteca ML/AI Nativa para BR v2.0
// Implementa funções de machine learning e inteligência artificial
#ifndef BIBLIOTECA_ML_H
#define BIBLIOTECA_ML_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// Tipos de dados para ML
using Tensor = std::vector<double>;
using Matriz = std::vector<std::vector<double>>;

// Estrutura de dataset
struct Dataset {
    std::vector<std::vector<double>> X; // Features
    std::vector<double> y; // Labels (para classificação/regressão)
    std::vector<std::string> nomes_features;
    std::string nome_label;
    
    Dataset() {}
    Dataset(size_t n_amostras, size_t n_features) {
        X.resize(n_amostras, std::vector<double>(n_features));
        y.resize(n_amostras);
    }
    
    size_t tamanho() const { return X.size(); }
    size_t num_features() const { return X.empty() ? 0 : X[0].size(); }
};

// Estrutura de modelo
struct ModeloML {
    std::string tipo;
    std::map<std::string, double> parametros;
    std::vector<double> pesos;
    std::vector<double> vies;
    bool treinado;
    
    ModeloML() : treinado(false) {}
    
    virtual double prever(const std::vector<double>& entrada) = 0;
    virtual void treinar(const Dataset& dados) = 0;
    virtual ~ModeloML() {}
};

// Regressão Linear
class RegressaoLinear : public ModeloML {
private:
    std::vector<double> coeficientes;
    double intercepto;
    
public:
    RegressaoLinear();
    
    double prever(const std::vector<double>& entrada) override;
    void treinar(const Dataset& dados) override;
    std::vector<double> obter_coeficientes() const;
    double obter_intercepto() const;
};

// Regressão Logística (Classificação Binária)
class RegressaoLogistica : public ModeloML {
private:
    std::vector<double> pesos;
    double vies;
    double taxa_aprendizado;
    
public:
    RegressaoLogistica(double taxa = 0.01);
    
    double prever(const std::vector<double>& entrada) override;
    void treinar(const Dataset& dados, int epocas = 1000);
    void treinar(const Dataset& dados) override;
    int prever_classe(const std::vector<double>& entrada);
    
    void definir_taxa_aprendizado(double taxa);
};

// K-Nearest Neighbors (KNN)
class KNN : public ModeloML {
private:
    Dataset dados_treinamento;
    int k;
    
public:
    KNN(int vizinhos = 5);
    
    double prever(const std::vector<double>& entrada) override;
    void treinar(const Dataset& dados) override;
    int prever_classe(const std::vector<double>& entrada);
    
    void definir_k(int vizinhos);
};

// Rede Neural Simples (Perceptron Multicamadas)
class RedeNeural : public ModeloML {
private:
    std::vector<std::vector<double>> camadas;
    std::vector<std::vector<double>> pesos_camadas;
    std::vector<std::vector<double>> vies_camadas;
    std::vector<int> arquitetura;
    double taxa_aprendizado;
    
public:
    RedeNeural(const std::vector<int>& camadas_ocultas, double taxa = 0.1);
    
    double prever(const std::vector<double>& entrada) override;
    void treinar(const Dataset& dados, int epocas = 1000);
    void treinar(const Dataset& dados) override;
    
    std::vector<double> feedforward(const std::vector<double>& entrada);
    void backpropagation(const std::vector<double>& entrada, double saida_alvo);
    
    double sigmoid(double x);
    double derivada_sigmoid(double x);
};

// K-Means Clustering
class KMeans {
private:
    std::vector<std::vector<double>> centroides;
    std::vector<int> clusters;
    int k;
    
public:
    KMeans(int num_clusters = 3);
    
    void treinar(const Dataset& dados, int max_iteracoes = 100);
    int prever_cluster(const std::vector<double>& ponto);
    std::vector<int> obter_clusters() const;
    std::vector<std::vector<double>> obter_centroides() const;
    
    double calcular_inercia(const Dataset& dados);
};

// Processamento de dados
class ProcessadorDados {
public:
    // Normalização
    static std::vector<std::vector<double>> normalizar_min_max(const std::vector<std::vector<double>>& dados);
    static std::vector<std::vector<double>> normalizar_z_score(const std::vector<std::vector<double>>& dados);
    
    // Divisão treino/teste
    static std::pair<Dataset, Dataset> dividir_treino_teste(const Dataset& dados, double proporcao_treino = 0.8);
    
    // Feature engineering
    static std::vector<std::vector<double>> criar_polinomios(const std::vector<std::vector<double>>& dados, int grau = 2);
    static std::vector<std::vector<double>> one_hot_encode(const std::vector<int>& classes, int num_classes);
    
    // Redução de dimensionalidade (PCA simplificado)
    static std::vector<std::vector<double>> pca(const std::vector<std::vector<double>>& dados, int n_componentes);
};

// Métricas de avaliação
class MetricasML {
public:
    static double acuracia(const std::vector<int>& previsto, const std::vector<int>& real);
    static double precisao(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva = 1);
    static double recall(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva = 1);
    static double f1_score(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva = 1);
    
    static double mse(const std::vector<double>& previsto, const std::vector<double>& real);
    static double rmse(const std::vector<double>& previsto, const std::vector<double>& real);
    static double mae(const std::vector<double>& previsto, const std::vector<double>& real);
    static double r2(const std::vector<double>& previsto, const std::vector<double>& real);
    
    static std::vector<std::vector<int>> matriz_confusao(const std::vector<int>& previsto, 
                                                          const std::vector<int>& real, 
                                                          int num_classes);
};

// Utilitários matemáticos
class MatematicaML {
public:
    static double distancia_euclidiana(const std::vector<double>& a, const std::vector<double>& b);
    static double distancia_manhattan(const std::vector<double>& a, const std::vector<double>& b);
    static double produto_escalar(const std::vector<double>& a, const std::vector<double>& b);
    
    static std::vector<double> soma_vetores(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<double> subtrair_vetores(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<double> multiplicar_escalar(const std::vector<double>& v, double escalar);
    
    static Matriz multiplicar_matrizes(const Matriz& A, const Matriz& B);
    static Matriz transpor(const Matriz& M);
    
    static double media(const std::vector<double>& v);
    static double desvio_padrao(const std::vector<double>& v);
    static double variancia(const std::vector<double>& v);
};

// Biblioteca principal ML/IA para BR
class BibliotecaML {
private:
    std::map<std::string, std::unique_ptr<ModeloML>> modelos;
    
public:
    BibliotecaML();
    
    // Criar modelos
    std::string criar_regressao_linear(const std::string& nome);
    std::string criar_regressao_logistica(const std::string& nome, double taxa = 0.01);
    std::string criar_knn(const std::string& nome, int k = 5);
    std::string criar_rede_neural(const std::string& nome, const std::vector<int>& camadas);
    std::string criar_kmeans(const std::string& nome, int k = 3);
    
    // Operações com modelos
    bool treinar_modelo(const std::string& nome, const Dataset& dados);
    double prever_modelo(const std::string& nome, const std::vector<double>& entrada);
    int prever_classe_modelo(const std::string& nome, const std::vector<double>& entrada);
    
    // Carregar/salvar modelos
    bool salvar_modelo(const std::string& nome, const std::string& caminho);
    bool carregar_modelo(const std::string& nome, const std::string& caminho);
    
    // Remover modelo
    bool remover_modelo(const std::string& nome);
    
    // Listar modelos
    std::vector<std::string> listar_modelos();
};

#endif // BIBLIOTECA_ML_H
