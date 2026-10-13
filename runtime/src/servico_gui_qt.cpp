// Implementação do Serviço GUI Nativo com Qt para BR v2.0
#include "servico_gui_qt.h"
#include <iostream>
#include <sstream>

#ifdef USE_QT
ServicoGUIQt::ServicoGUIQt() 
    : larguraJanela(400), alturaJanela(300), janelaVisivel(false),
      app(nullptr), janelaPrincipal(nullptr), widgetCentral(nullptr), layoutPrincipal(nullptr) {
    
    // Inicializar QApplication se não existir
    int argc = 0;
    char** argv = nullptr;
    if (!QApplication::instance()) {
        app = new QApplication(argc, argv);
    }
}

ServicoGUIQt::~ServicoGUIQt() {
    fechar();
    if (app && app == QApplication::instance()) {
        delete app;
    }
}

void ServicoGUIQt::criarJanela(const std::string& titulo, int largura, int altura) {
    tituloJanela = titulo;
    larguraJanela = largura;
    alturaJanela = altura;
    
#ifdef USE_QT
    janelaPrincipal = new QMainWindow();
    janelaPrincipal->setWindowTitle(QString::fromStdString(titulo));
    janelaPrincipal->resize(largura, altura);
    
    widgetCentral = new QWidget();
    layoutPrincipal = new QVBoxLayout(widgetCentral);
    layoutPrincipal->setSpacing(10);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);
    
    janelaPrincipal->setCentralWidget(widgetCentral);
    
    // Conectar evento de fechamento
    QObject::connect(janelaPrincipal, &QMainWindow::closeRequested, [this]() {
        if (callbackAoFechar) {
            callbackAoFechar();
        }
    });
    
    janelaVisivel = true;
#else
    std::cout << "[GUI Qt] Janela criada: " << titulo << " (" << largura << "x" << altura << ")" << std::endl;
    std::cout << "[GUI Qt] Nota: Compile com -DUSE_QT e link com Qt5/Qt6 para GUI nativa completa." << std::endl;
    janelaVisivel = true;
#endif
}

void ServicoGUIQt::adicionarBotao(const std::string& id, const std::string& texto, 
                                   int x, int y, std::function<void()> aoClicar) {
    auto botao = std::make_shared<ComponenteGUIQt>();
    botao->id = id;
    botao->tipo = "botao";
    botao->texto = texto;
    botao->x = x;
    botao->y = y;
    botao->largura = 100;
    botao->altura = 30;
    botao->aoClicar = aoClicar;
    componentes.push_back(botao);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        botao->botao = new QPushButton(QString::fromStdString(texto));
        botao->widget = botao->botao;
        layoutPrincipal->addWidget(botao->botao);
        
        if (aoClicar) {
            QObject::connect(botao->botao, &QPushButton::clicked, [aoClicar]() {
                aoClicar();
            });
        }
    }
#else
    std::cout << "[GUI Qt] Botão adicionado: " << id << " = \"" << texto << "\"" << std::endl;
#endif
}

void ServicoGUIQt::adicionarCampo(const std::string& id, const std::string& texto, 
                                  int x, int y, int largura, std::function<void(const std::string&)> aoMudar) {
    auto campo = std::make_shared<ComponenteGUIQt>();
    campo->id = id;
    campo->tipo = "campo";
    campo->texto = texto;
    campo->x = x;
    campo->y = y;
    campo->largura = largura;
    campo->altura = 25;
    campo->aoMudar = aoMudar;
    componentes.push_back(campo);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        campo->campo = new QLineEdit(QString::fromStdString(texto));
        campo->widget = campo->campo;
        campo->campo->setFixedWidth(largura);
        layoutPrincipal->addWidget(campo->campo);
        
        if (aoMudar) {
            QObject::connect(campo->campo, &QLineEdit::textChanged, [aoMudar](const QString& texto) {
                aoMudar(texto.toStdString());
            });
        }
    }
#else
    std::cout << "[GUI Qt] Campo adicionado: " << id << " = \"" << texto << "\"" << std::endl;
#endif
}

void ServicoGUIQt::adicionarRotulo(const std::string& id, const std::string& texto, int x, int y) {
    auto rotulo = std::make_shared<ComponenteGUIQt>();
    rotulo->id = id;
    rotulo->tipo = "rotulo";
    rotulo->texto = texto;
    rotulo->x = x;
    rotulo->y = y;
    rotulo->largura = 200;
    rotulo->altura = 20;
    componentes.push_back(rotulo);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        rotulo->rotulo = new QLabel(QString::fromStdString(texto));
        rotulo->widget = rotulo->rotulo;
        layoutPrincipal->addWidget(rotulo->rotulo);
    }
#else
    std::cout << "[GUI Qt] Rótulo adicionado: " << id << " = \"" << texto << "\"" << std::endl;
#endif
}

void ServicoGUIQt::adicionarAreaTexto(const std::string& id, int x, int y, int largura, int altura) {
    auto area = std::make_shared<ComponenteGUIQt>();
    area->id = id;
    area->tipo = "area_texto";
    area->x = x;
    area->y = y;
    area->largura = largura;
    area->altura = altura;
    componentes.push_back(area);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        area->texto_area = new QTextEdit();
        area->widget = area->texto_area;
        area->texto_area->setFixedSize(largura, altura);
        layoutPrincipal->addWidget(area->texto_area);
    }
#else
    std::cout << "[GUI Qt] Área de texto adicionada: " << id << std::endl;
#endif
}

void ServicoGUIQt::adicionarSlider(const std::string& id, int minimo, int maximo, int valor, 
                                    int x, int y, std::function<void(int)> aoMudar) {
    auto slider = std::make_shared<ComponenteGUIQt>();
    slider->id = id;
    slider->tipo = "slider";
    slider->x = x;
    slider->y = y;
    slider->largura = 200;
    slider->altura = 30;
    componentes.push_back(slider);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        slider->slider = new QSlider(Qt::Horizontal);
        slider->widget = slider->slider;
        slider->slider->setMinimum(minimo);
        slider->slider->setMaximum(maximo);
        slider->slider->setValue(valor);
        layoutPrincipal->addWidget(slider->slider);
        
        if (aoMudar) {
            QObject::connect(slider->slider, &QSlider::valueChanged, [aoMudar](int valor) {
                aoMudar(valor);
            });
        }
    }
#else
    std::cout << "[GUI Qt] Slider adicionado: " << id << " [" << minimo << "-" << maximo << "]" << std::endl;
#endif
}

void ServicoGUIQt::adicionarCheckbox(const std::string& id, const std::string& texto, int x, int y,
                                      std::function<void(bool)> aoMudar) {
    auto checkbox = std::make_shared<ComponenteGUIQt>();
    checkbox->id = id;
    checkbox->tipo = "checkbox";
    checkbox->texto = texto;
    checkbox->x = x;
    checkbox->y = y;
    checkbox->largura = 200;
    checkbox->altura = 25;
    componentes.push_back(checkbox);
    
#ifdef USE_Qt
    if (layoutPrincipal) {
        checkbox->checkbox = new QCheckBox(QString::fromStdString(texto));
        checkbox->widget = checkbox->checkbox;
        layoutPrincipal->addWidget(checkbox->checkbox);
        
        if (aoMudar) {
            QObject::connect(checkbox->checkbox, &QCheckBox::stateChanged, [aoMudar](int estado) {
                aoMudar(estado == Qt::Checked);
            });
        }
    }
#else
    std::cout << "[GUI Qt] Checkbox adicionado: " << id << " = \"" << texto << "\"" << std::endl;
#endif
}

void ServicoGUIQt::adicionarComboBox(const std::string& id, const std::vector<std::string>& opcoes,
                                      int x, int y, std::function<void(const std::string&)> aoMudar) {
    auto combo = std::make_shared<ComponenteGUIQt>();
    combo->id = id;
    combo->tipo = "combo_box";
    combo->x = x;
    combo->y = y;
    combo->largura = 150;
    combo->altura = 25;
    combo->aoMudar = aoMudar;
    componentes.push_back(combo);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        combo->combo_box = new QComboBox();
        combo->widget = combo->combo_box;
        for (const auto& opcao : opcoes) {
            combo->combo_box->addItem(QString::fromStdString(opcao));
        }
        layoutPrincipal->addWidget(combo->combo_box);
        
        if (aoMudar) {
            QObject::connect(combo->combo_box, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
                [aoMudar](const QString& texto) {
                    aoMudar(texto.toStdString());
                });
        }
    }
#else
    std::cout << "[GUI Qt] ComboBox adicionado: " << id << " com " << opcoes.size() << " opções" << std::endl;
#endif
}

void ServicoGUIQt::adicionarLista(const std::string& id, int x, int y, int largura, int altura) {
    auto lista = std::make_shared<ComponenteGUIQt>();
    lista->id = id;
    lista->tipo = "lista";
    lista->x = x;
    lista->y = y;
    lista->largura = largura;
    lista->altura = altura;
    componentes.push_back(lista);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        lista->lista = new QListWidget();
        lista->widget = lista->lista;
        lista->lista->setFixedSize(largura, altura);
        layoutPrincipal->addWidget(lista->lista);
    }
#else
    std::cout << "[GUI Qt] Lista adicionada: " << id << std::endl;
#endif
}

void ServicoGUIQt::adicionarProgressBar(const std::string& id, int x, int y, int largura, int altura) {
    auto progresso = std::make_shared<ComponenteGUIQt>();
    progresso->id = id;
    progresso->tipo = "progresso";
    progresso->x = x;
    progresso->y = y;
    progresso->largura = largura;
    progresso->altura = altura;
    componentes.push_back(progresso);
    
#ifdef USE_QT
    if (layoutPrincipal) {
        progresso->progresso = new QProgressBar();
        progresso->widget = progresso->progresso;
        progresso->progresso->setFixedSize(largura, altura);
        progresso->progresso->setRange(0, 100);
        progresso->progresso->setValue(0);
        layoutPrincipal->addWidget(progresso->progresso);
    }
#else
    std::cout << "[GUI Qt] ProgressBar adicionado: " << id << std::endl;
#endif
}

std::string ServicoGUIQt::obterValorCampo(const std::string& id) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "campo") {
#ifdef USE_QT
        if (comp->campo) {
            return comp->campo->text().toStdString();
        }
#endif
        return comp->texto;
    }
    return "";
}

void ServicoGUIQt::definirValorCampo(const std::string& id, const std::string& valor) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "campo") {
        comp->texto = valor;
#ifdef USE_QT
        if (comp->campo) {
            comp->campo->setText(QString::fromStdString(valor));
        }
#endif
    }
}

int ServicoGUIQt::obterValorSlider(const std::string& id) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "slider") {
#ifdef USE_QT
        if (comp->slider) {
            return comp->slider->value();
        }
#endif
    }
    return 0;
}

void ServicoGUIQt::definirValorSlider(const std::string& id, int valor) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "slider") {
#ifdef USE_QT
        if (comp->slider) {
            comp->slider->setValue(valor);
        }
#endif
    }
}

bool ServicoGUIQt::obterValorCheckbox(const std::string& id) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "checkbox") {
#ifdef USE_QT
        if (comp->checkbox) {
            return comp->checkbox->isChecked();
        }
#endif
    }
    return false;
}

void ServicoGUIQt::definirValorCheckbox(const std::string& id, bool valor) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "checkbox") {
#ifdef USE_QT
        if (comp->checkbox) {
            comp->checkbox->setChecked(valor);
        }
#endif
    }
}

std::string ServicoGUIQt::obterValorComboBox(const std::string& id) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "combo_box") {
#ifdef USE_QT
        if (comp->combo_box) {
            return comp->combo_box->currentText().toStdString();
        }
#endif
    }
    return "";
}

void ServicoGUIQt::definirValorComboBox(const std::string& id, const std::string& valor) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "combo_box") {
#ifdef USE_QT
        if (comp->combo_box) {
            int idx = comp->combo_box->findText(QString::fromStdString(valor));
            if (idx >= 0) {
                comp->combo_box->setCurrentIndex(idx);
            }
        }
#endif
    }
}

void ServicoGUIQt::adicionarItemLista(const std::string& id, const std::string& item) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "lista") {
#ifdef USE_QT
        if (comp->lista) {
            comp->lista->addItem(QString::fromStdString(item));
        }
#endif
    }
}

void ServicoGUIQt::limparLista(const std::string& id) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "lista") {
#ifdef USE_QT
        if (comp->lista) {
            comp->lista->clear();
        }
#endif
    }
}

void ServicoGUIQt::definirProgresso(const std::string& id, int valor) {
    auto comp = buscarComponente(id);
    if (comp && comp->tipo == "progresso") {
#ifdef USE_QT
        if (comp->progresso) {
            comp->progresso->setValue(valor);
        }
#endif
    }
}

void ServicoGUIQt::adicionarLayoutHorizontal(int x, int y) {
#ifdef USE_QT
    if (layoutPrincipal) {
        auto* hLayout = new QHBoxLayout();
        layoutPrincipal->addLayout(hLayout);
    }
#else
    std::cout << "[GUI Qt] Layout horizontal adicionado" << std::endl;
#endif
}

void ServicoGUIQt::adicionarLayoutVertical(int x, int y) {
#ifdef USE_QT
    if (layoutPrincipal) {
        auto* vLayout = new QVBoxLayout();
        layoutPrincipal->addLayout(vLayout);
    }
#else
    std::cout << "[GUI Qt] Layout vertical adicionado" << std::endl;
#endif
}

void ServicoGUIQt::finalizarLayout() {
#ifdef USE_QT
    if (layoutPrincipal) {
        layoutPrincipal->addStretch();
    }
#endif
}

void ServicoGUIQt::mostrar() {
#ifdef USE_QT
    if (janelaPrincipal) {
        janelaPrincipal->show();
        janelaVisivel = true;
        
        // Executar loop de eventos
        if (app) {
            app->exec();
        }
    }
#else
    std::cout << "[GUI Qt] Mostrando janela: " << tituloJanela << std::endl;
    std::cout << "[GUI Qt] Componentes: " << componentes.size() << std::endl;
    janelaVisivel = true;
#endif
}

void ServicoGUIQt::fechar() {
#ifdef USE_QT
    if (janelaPrincipal) {
        janelaPrincipal->close();
        delete janelaPrincipal;
        janelaPrincipal = nullptr;
    }
#endif
    janelaVisivel = false;
}

bool ServicoGUIQt::estaVisivel() const {
    return janelaVisivel;
}

void ServicoGUIQt::definirAoFechar(std::function<void()> aoFechar) {
    callbackAoFechar = aoFechar;
}

std::shared_ptr<ComponenteGUIQt> ServicoGUIQt::buscarComponente(const std::string& id) {
    for (auto& comp : componentes) {
        if (comp->id == id) {
            return comp;
        }
    }
    return nullptr;
}

#else // !USE_QT

// Implementações stub quando Qt não está disponível
ServicoGUIQt::ServicoGUIQt() : larguraJanela(400), alturaJanela(300), janelaVisivel(false) {}
ServicoGUIQt::~ServicoGUIQt() { fechar(); }
void ServicoGUIQt::criarJanela(const std::string& titulo, int largura, int altura) {
    tituloJanela = titulo;
    larguraJanela = largura;
    alturaJanela = altura;
    std::cout << "[GUI Qt] Janela criada: " << titulo << " (" << largura << "x" << altura << ")" << std::endl;
    std::cout << "[GUI Qt] Nota: Compile com -DUSE_QT e link com Qt5/Qt6 para GUI nativa completa." << std::endl;
    janelaVisivel = true;
}
void ServicoGUIQt::adicionarBotao(const std::string& id, const std::string& texto, int x, int y, std::function<void()> aoClicar) {
    auto botao = std::make_shared<ComponenteGUIQt>();
    botao->id = id; botao->tipo = "botao"; botao->texto = texto;
    componentes.push_back(botao);
    std::cout << "[GUI Qt] Botão adicionado: " << id << " = \"" << texto << "\"" << std::endl;
}
void ServicoGUIQt::adicionarCampo(const std::string& id, const std::string& texto, int x, int y, int largura, std::function<void(const std::string&)> aoMudar) {
    auto campo = std::make_shared<ComponenteGUIQt>();
    campo->id = id; campo->tipo = "campo"; campo->texto = texto;
    componentes.push_back(campo);
    std::cout << "[GUI Qt] Campo adicionado: " << id << " = \"" << texto << "\"" << std::endl;
}
void ServicoGUIQt::adicionarRotulo(const std::string& id, const std::string& texto, int x, int y) {
    auto rotulo = std::make_shared<ComponenteGUIQt>();
    rotulo->id = id; rotulo->tipo = "rotulo"; rotulo->texto = texto;
    componentes.push_back(rotulo);
    std::cout << "[GUI Qt] Rótulo adicionado: " << id << " = \"" << texto << "\"" << std::endl;
}
void ServicoGUIQt::adicionarAreaTexto(const std::string& id, int x, int y, int largura, int altura) {
    auto area = std::make_shared<ComponenteGUIQt>();
    area->id = id; area->tipo = "area_texto";
    componentes.push_back(area);
    std::cout << "[GUI Qt] Área de texto adicionada: " << id << std::endl;
}
void ServicoGUIQt::adicionarSlider(const std::string& id, int minimo, int maximo, int valor, int x, int y, std::function<void(int)> aoMudar) {
    auto slider = std::make_shared<ComponenteGUIQt>();
    slider->id = id; slider->tipo = "slider";
    componentes.push_back(slider);
    std::cout << "[GUI Qt] Slider adicionado: " << id << " [" << minimo << "-" << maximo << "]" << std::endl;
}
void ServicoGUIQt::adicionarCheckbox(const std::string& id, const std::string& texto, int x, int y, std::function<void(bool)> aoMudar) {
    auto checkbox = std::make_shared<ComponenteGUIQt>();
    checkbox->id = id; checkbox->tipo = "checkbox"; checkbox->texto = texto;
    componentes.push_back(checkbox);
    std::cout << "[GUI Qt] Checkbox adicionado: " << id << " = \"" << texto << "\"" << std::endl;
}
void ServicoGUIQt::adicionarComboBox(const std::string& id, const std::vector<std::string>& opcoes, int x, int y, std::function<void(const std::string&)> aoMudar) {
    auto combo = std::make_shared<ComponenteGUIQt>();
    combo->id = id; combo->tipo = "combo_box";
    componentes.push_back(combo);
    std::cout << "[GUI Qt] ComboBox adicionado: " << id << " com " << opcoes.size() << " opções" << std::endl;
}
void ServicoGUIQt::adicionarLista(const std::string& id, int x, int y, int largura, int altura) {
    auto lista = std::make_shared<ComponenteGUIQt>();
    lista->id = id; lista->tipo = "lista";
    componentes.push_back(lista);
    std::cout << "[GUI Qt] Lista adicionada: " << id << std::endl;
}
void ServicoGUIQt::adicionarProgressBar(const std::string& id, int x, int y, int largura, int altura) {
    auto progresso = std::make_shared<ComponenteGUIQt>();
    progresso->id = id; progresso->tipo = "progresso";
    componentes.push_back(progresso);
    std::cout << "[GUI Qt] RadioButton adicionado: " << id << std::endl;
}
std::string ServicoGUIQt::obterValorCampo(const std::string& id) {
    auto comp = buscarComponente(id);
    return comp ? comp->texto : "";
}
void ServicoGUIQt::definirValorCampo(const std::string& id, const std::string& valor) {
    auto comp = buscarComponente(id);
    if (comp) comp->texto = valor;
}
int ServicoGUIQt::obterValorSlider(const std::string& id) { return 0; }
void ServicoGUIQt::definirValorSlider(const std::string& id, int valor) {}
bool ServicoGUIQt::obterValorCheckbox(const std::string& id) { return false; }
void ServicoGUIQt::definirValorCheckbox(const std::string& id, bool valor) {}
std::string ServicoGUIQt::obterValorComboBox(const std::string& id) { return ""; }
void ServicoGUIQt::definirValorComboBox(const std::string& id, const std::string& valor) {}
void ServicoGUIQt::adicionarItemLista(const std::string& id, const std::string& item) {}
void ServicoGUIQt::limparLista(const std::string& id) {}
void ServicoGUIQt::definirProgresso(const std::string& id, int valor) {}
void ServicoGUIQt::adicionarLayoutHorizontal(int x, int y) { std::cout << "[GUI Qt] Layout horizontal" << std::endl; }
void ServicoGUIQt::adicionarLayoutVertical(int x, int y) { std::cout << "[GUI Qt] Layout vertical" << std::endl; }
void ServicoGUIQt::finalizarLayout() {}
void ServicoGUIQt::mostrar() {
    std::cout << "[GUI Qt] Mostrando janela: " << tituloJanela << std::endl;
    janelaVisivel = true;
}
void ServicoGUIQt::fechar() { janelaVisivel = false; }
bool ServicoGUIQt::estaVisivel() const { return janelaVisivel; }
void ServicoGUIQt::definirAoFechar(std::function<void()> aoFechar) { callbackAoFechar = aoFechar; }
std::shared_ptr<ComponenteGUIQt> ServicoGUIQt::buscarComponente(const std::string& id) {
    for (auto& comp : componentes) if (comp->id == id) return comp;
    return nullptr;
}

#endif // USE_QT
