#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidget>
#include <QtCore/QDebug>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/stdx/optional.hpp>

using namespace std;
using namespace bsoncxx::builder::stream;
using bsoncxx::document::value;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::stdx::string_view;

// Classe para representar um livro na biblioteca
class Livro {
public:
    string titulo;
    string autor;
    string editora;
    int ano;
    string isbn;
    bool disponivel;

    Livro(string t, string a, string e, int an, string i, bool d = true) :
        titulo(t), autor(a), editora(e), ano(an), isbn(i), disponivel(d) {}
};

// Classe para representar o modelo da tabela que mostra os livros na interface gráfica
class TabelaLivrosModel : public QTableWidget {
public:
    TabelaLivrosModel(QWidget* parent = nullptr) : QTableWidget(parent) {
        setColumnCount(6);
        setHorizontalHeaderLabels({"Título", "Autor", "Editora", "Ano", "ISBN", "Disponível"});
        horizontalHeader()->setStretchLastSection(true);
    }

    void adicionarLivro(const Livro& livro) {
        int row = rowCount();
        setRowCount(row + 1);
        setItem(row, 0, new QTableWidgetItem(QString::fromStdString(livro.titulo)));
        setItem(row, 1, new QTableWidgetItem(QString::fromStdString(livro.autor)));
        setItem(row, 2, new QTableWidgetItem(QString::fromStdString(livro.editora)));
        setItem(row, 3, new QTableWidgetItem(QString::number(livro.ano)));
        setItem(row, 4, new QTableWidgetItem(QString::fromStdString(livro.isbn)));
        setItem(row, 5, new QTableWidgetItem(livro.disponivel ? "Sim" : "Não"));
    }

    void limpar() {
        setRowCount(0);
    }
};

// Classe principal da aplicação
class BibliotecaApp : public QMainWindow {
public:
    BibliotecaApp() {
        // Configuração da janela principal
        setWindowTitle("Sistema de Gerenciamento de Biblioteca");
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QHBoxLayout* horizontalLayout = new QHBoxLayout(centralWidget);

        // Configuração da tabela de livros
        TabelaLivrosModel* tabelaLivros = new TabelaLivrosModel(this);
        horizontalLayout->addWidget(tabelaLivros);

        // Configuração dos campos de busca
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    horizontalLayout->addLayout(verticalLayout);
    QLabel* buscaLabel = new QLabel("Buscar por Título ou Autor:", this);
    QLineEdit* buscaLineEdit = new QLineEdit(this);
    QPushButton* buscarButton = new QPushButton("Buscar", this);
    verticalLayout->addWidget(buscaLabel);
    verticalLayout->addWidget(buscaLineEdit);
    verticalLayout->addWidget(buscarButton);

    // Configuração dos campos de cadastro de livro
    QLabel* tituloLabel = new QLabel("Título:", this);
    QLineEdit* tituloLineEdit = new QLineEdit(this);
    QLabel* autorLabel = new QLabel("Autor:", this);
    QLineEdit* autorLineEdit = new QLineEdit(this);
    QLabel* editoraLabel = new QLabel("Editora:", this);
    QLineEdit* editoraLineEdit = new QLineEdit(this);
    QLabel* anoLabel = new QLabel("Ano:", this);
    QLineEdit* anoLineEdit = new QLineEdit(this);
    QLabel* isbnLabel = new QLabel("ISBN:", this);
    QLineEdit* isbnLineEdit = new QLineEdit(this);
    QPushButton* cadastrarButton = new QPushButton("Cadastrar", this);
    verticalLayout->addWidget(tituloLabel);
    verticalLayout->addWidget(tituloLineEdit);
    verticalLayout->addWidget(autorLabel);
    verticalLayout->addWidget(autorLineEdit);
    verticalLayout->addWidget(editoraLabel);
    verticalLayout->addWidget(editoraLineEdit);
    verticalLayout->addWidget(anoLabel);
    verticalLayout->addWidget(anoLineEdit);
    verticalLayout->addWidget(isbnLabel);
    verticalLayout->addWidget(isbnLineEdit);
    verticalLayout->addWidget(cadastrarButton);

    // Conexão com o banco de dados MongoDB
    mongocxx::instance instance{}; // Inicialização do driver do MongoDB
    mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}}; // Conexão com o servidor do MongoDB
    auto collection = client["biblioteca"]["livros"]; // Seleção da coleção de livros

    // Função para atualizar a tabela de livros com os dados do banco de dados
    auto atualizarTabelaLivros = [&]() {
        tabelaLivros->limpar();
        auto cursor = collection.find(document{} << finalize);
        for (auto&& doc : cursor) {
            string titulo = doc["titulo"].get_utf8().value.to_string();
            string autor = doc["autor"].get_utf8().value.to_string();
            string editora = doc["editora"].get_utf8().value.to_string();
            int ano = doc["ano"].get_int32().value;
            string isbn = doc["isbn"].get_utf8().value.to_string();
            bool disponivel = doc["disponivel"].get_bool().value;
            tabelaLivros->adicionarLivro(Livro(titulo, autor, editora, ano, isbn, disponivel));
        }
    };

    // Função para buscar livros pelo título ou autor
    auto buscarLivros = [&](const string& termo) {
        tabelaLivros->limpar();
        auto cursor = collection.find(
            document{} << "$or" << bsoncxx::builder::stream::array{
                bsoncxx::builder::stream::document{} << "titulo" << bsoncxx::builder::stream::open_document
                    << "$regex" << termo << bsoncxx::builder::stream::close_document << finalize,
                bsoncxx::builder::stream::document{} << "autor" << bsoncxx::builder::stream::open_document
                    << "$regex" << termo << bsoncxx::builder::stream::close_document << finalize
            }

        );
        for (auto&& doc : cursor) {
            string titulo = doc["titulo"].get_utf8().value.to_string();
            string autor = doc["autor"].get_utf8().value.to_string();
            string editora = doc["editora"].get_utf8().value.to_string();
            int ano = doc["ano"].get_int32().value;
            string isbn = doc["isbn"].get_utf8().value.to_string();
            bool disponivel = doc["disponivel"].get_bool().value;
            tabelaLivros->adicionarLivro(Livro(titulo, autor, editora, ano, isbn, disponivel));
        }
    };

    // Função para cadastrar um novo livro
    auto cadastrarLivro = [&]() {
        string titulo = tituloLineEdit->text().toStdString();
        string autor = autorLineEdit->text().toStdString();
        string editora = editoraLineEdit->text().toStdString();
        int ano = anoLineEdit->text().toInt();
        string isbn = isbnLineEdit->text().toStdString();
        bool disponivel = true;
        auto doc = document{} << "titulo" << titulo << "autor" << autor << "editora" << editora
                              << "ano" << ano << "isbn" << isbn << "disponivel" << disponivel << finalize;
        collection.insert_one(doc.view());
        atualizarTabelaLivros();
        tituloLineEdit->clear();
        autorLineEdit->clear();
        editoraLineEdit->clear();
        anoLineEdit->clear();
        isbnLineEdit->clear();
    };

    // Conexão dos botões com as funções correspondentes
    connect(buscarButton, &QPushButton::clicked, [=]() {
        string termo = buscaLineEdit->text().toStdString();
        buscarLivros(termo);
    });
    connect(cadastrarButton, &QPushButton::clicked, cadastrarLivro);

    // Configuração da janela principal
    setWindowTitle("Biblioteca");
    setCentralWidget(mainWidget);
    resize(800, 600);
}
};

int main(int argc, char* argv[])
{
QApplication app(argc, argv);
BibliotecaWindow biblioteca;
biblioteca.show();
return app.exec();
}
