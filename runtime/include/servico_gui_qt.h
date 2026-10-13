// Serviço GUI Nativo com Qt para BR v2.0
// Multiplataforma: Windows, Linux, macOS
#ifndef SERVICO_GUI_QT_H
#define SERVICO_GUI_QT_H

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <map>

#ifdef USE_QT
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QProgressBar>
#include <QTimer>
#include <QThread>
#endif

// Estrutura de componente GUI
struct ComponenteGUIQt {
    std::string id;
    std::string tipo;
    std::string texto;
    int x, y, largura, altura;
    std::function<void()> aoClicar;
    std::function<void(const std::string&)> aoMudar;
    
#ifdef USE_QT
    QWidget* widget;
    QPushButton* botao;
    QLineEdit* campo;
    QLabel* rotulo;
    QTextEdit* texto_area;
    QSlider* slider;
    QCheckBox* checkbox;
    QComboBox* combo_box;
    QListWidget* lista;
    QProgressBar* progresso;
#endif
    
    ComponenteGUIQt() : x(0), y(0), largura(100), altura(30)
#ifdef USE_QT
        , widget(nullptr), botao(nullptr), campo(nullptr), rotulo(nullptr),
          texto_area(nullptr), slider(nullptr), checkbox(nullptr), combo_box(nullptr),
          lista(nullptr), progresso(nullptr)
#endif
    {}
};

// Serviço GUI com Qt
class ServicoGUIQt {
private:
    std::string tituloJanela;
    int larguraJanela, alturaJanela;
    bool janelaVisivel;
    std::vector<std::shared_ptr<ComponenteGUIQt>> componentes;
    
#ifdef USE_QT
    QApplication* app;
    QMainWindow* janelaPrincipal;
    QWidget* widgetCentral;
    QVBoxLayout* layoutPrincipal;
#endif
    
public:
    ServicoGUIQt();
    ~ServicoGUIQt();
    
    // Criar janela principal
    void criarJanela(const std::string& titulo, int largura, int altura);
    
    // Adicionar componentes
    void adicionarBotao(const std::string& id, const std::string& texto, 
                         int x, int y, std::function<void()> aoClicar);
    void adicionarCampo(const std::string& id, const std::string& texto, 
                        int x, int y, int largura, std::function<void(const std::string&)> aoMudar = nullptr);
    void adicionarRotulo(const std::string& id, const std::string& texto, int x, int y);
    void adicionarAreaTexto(const std::string& id, int x, int y, int largura, int altura);
    void adicionarSlider(const std::string& id, int minimo, int maximo, int valor, 
                          int x, int y, std::function<void(int)> aoMudar = nullptr);
    void adicionarCheckbox(const std::string& id, const std::string& texto, int x, int y,
                            std::function<void(bool)> aoMudar = nullptr);
    void adicionarComboBox(const std::string& id, const std::vector<std::string>& opcoes,
                           int x, int y, std::function<void(const std::string&)> aoMudar = nullptr);
    void adicionarLista(const std::string& id, int x, int y, int largura, int altura);
    void adicionarProgressBar(const std::string& id, int x, int y, int largura, int altura);
    
    // Obter/definir valores
    std::string obterValorCampo(const std::string& id);
    void definirValorCampo(const std::string& id, const std::string& valor);
    int obterValorSlider(const std::string& id);
    void definirValorSlider(const std::string& id, int valor);
    bool obterValorCheckbox(const std::string& id);
    void definirValorCheckbox(const std::string& id, bool valor);
    std::string obterValorComboBox(const std::string& id);
    void definirValorComboBox(const std::string& id, const std::string& valor);
    void adicionarItemLista(const std::string& id, const std::string& item);
    void limparLista(const std::string& id);
    void definirProgresso(const std::string& id, int valor);
    
    // Layout
    void adicionarLayoutHorizontal(int x, int y);
    void adicionarLayoutVertical(int x, int y);
    void finalizarLayout();
    
    // Mostrar/fechar
    void mostrar();
    void fechar();
    bool estaVisivel() const;
    
    // Eventos
    void definirAoFechar(std::function<void()> aoFechar);
    
private:
    std::shared_ptr<ComponenteGUIQt> buscarComponente(const std::string& id);
    std::function<void()> callbackAoFechar;
};

#endif // SERVICO_GUI_QT_H
