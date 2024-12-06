#include <list>
#include <limits>
#include <iostream>

typedef unsigned int uint;
using namespace std;

class Vertex {
private:
    int identificator;
    int position_x;
    int position_y;
    char cor[7];
    int d;
    Vertex* pi;
public:
    Vertex(int identificator, int position_x, int position_y) 
        : identificator(identificator), position_x(position_x), position_y(position_y), 
          d(numeric_limits<int>::max()), pi(nullptr) {
        const char* initial_color = "BRANCO";
        for (size_t i = 0; i < 6; ++i) {
            cor[i] = initial_color[i]; 
        }
        cor[6] = '\0'; 
    }

    int get_identificator() const { return identificator; }
    int get_position_x() const { return position_x; }
    int get_position_y() const { return position_y; }
    const char* get_cor() const { return cor; } 
    void set_cor(const char* nova_cor) {
        for (size_t i = 0; i < 6; ++i) {
            cor[i] = nova_cor[i]; 
        }
        cor[6] = '\0'; 
    }
    int get_d() const { return d; }
    void set_d(int valor) { d = valor; }
    void set_pi(Vertex* novo_pi) { pi = novo_pi; }
};

class Graph {
private:
    unsigned int num_vertices = 64;
    list<Vertex> adj[64]; 
public:
    Graph() {}

    list<Vertex>* get_adj() {
        return adj;
    }

    void add_edge(int position, int identificator, int position_x, int position_y) {
        adj[position].push_back(Vertex(identificator, position_x, position_y));
    }

    void preencher_grafo() {
        for (int position = 0; position < num_vertices; position++) {
            add_edge(position, position, position / 8, position % 8);
        }
    }

    int adicionar_cavalo(const char* posicao) {
        int coluna = posicao[1] - '1';
        int linha = posicao[0] - 'a';
        for (unsigned int i = 0; i < num_vertices; i++) {
            for (auto& vetor : adj[i]) {
                if (vetor.get_position_x() == linha && vetor.get_position_y() == coluna) {
                    return vetor.get_identificator();
                }
            }
        }
        return -1; 
    }

    int adicionar_rei(const char* posicao) {
        return adicionar_cavalo(posicao); 
    }
};

bool compare_strings(const char* str1, const char* str2) {
    while (*str1 && (*str1 == ' ')) {
        ++str1;
    }
    while (*str2 && (*str2 == ' ')) {
        ++str2;
    }
    while (*str1 && *str2 && (*str1 == *str2)) {
        ++str1;
        ++str2;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

int distancia_rei(Graph& grafo, int identificador_posicao_cavalo, int identificador_posicao_rei) {
    list<int> Q;

    for (int i = 0; i < 64; i++) {
        for (auto& vertice : grafo.get_adj()[i]) {
            if (vertice.get_identificator() == identificador_posicao_cavalo) {
                vertice.set_cor("CINZA");
                vertice.set_d(0);
                vertice.set_pi(nullptr);
                Q.push_back(identificador_posicao_cavalo);
                break;
            }
        }
    }

    while (!Q.empty()) {
        int u = Q.front();
        Q.pop_front();
        Vertex u_vertex(-1, -1, -1);

        for (int i = 0; i < 64; i++) {
            for (auto& vertice : grafo.get_adj()[i]) {
                if (vertice.get_identificator() == u) {
                    u_vertex = vertice;
                    break;
                }
            }
        }

        int moves[8][2] = {
            {-2, -1}, {-1, -2}, {1, -2}, {2, -1},
            {2, 1}, {1, 2}, {-1, 2}, {-2, 1}
        };

        for (int i = 0; i < 8; i++) {
            int new_x = u_vertex.get_position_x() + moves[i][0];
            int new_y = u_vertex.get_position_y() + moves[i][1];

            if (new_x >= 0 && new_x < 8 && new_y >= 0 && new_y < 8) {
                for (int j = 0; j < 64; j++) {
                    for (auto& vertice : grafo.get_adj()[j]) {
                        if (vertice.get_position_x() == new_x && vertice.get_position_y() == new_y) {
                            if (compare_strings(vertice.get_cor(), "BRANCO")) {
                                vertice.set_cor("CINZA");
                                vertice.set_d(u_vertex.get_d() + 1);
                                vertice.set_pi(&u_vertex);
                                Q.push_back(vertice.get_identificator());
                            }
                        }
                    }
                }
            }
        }

        u_vertex.set_cor("PRETO");
    }

    for (int i = 0; i < 64; i++) {
        for (auto& vertice_rei : grafo.get_adj()[i]) {
            if (vertice_rei.get_identificator() == identificador_posicao_rei) {
                return vertice_rei.get_d() - 1;
            }
        }
    }

    return -1; 
}

int main() {
    unsigned int num_testes;
    cin >> num_testes;
    list<list<int>> jogadas_em_passos;
    list<int> melhor_jogada_de_todas;
    list<list<char*>> jogadas_com_letras;

    for (unsigned int i = 0; i < num_testes; i++) {
        list<char*> jogada_atual;
        for (int j = 0; j < 5; j++) {
            char* jogada_ptr = new char[3];
            cin >> jogada_ptr; 
            jogada_atual.push_back(jogada_ptr);
        }
        jogadas_com_letras.push_back(jogada_atual);
    }

    while (!jogadas_com_letras.empty()) {
        int menor = numeric_limits<int>::max(); 
        list<int> movimentos_jogada;
        auto jogada = jogadas_com_letras.front();
        jogadas_com_letras.pop_front();
        auto posicao_rei = jogada.back();
        jogada.pop_back();

        for (auto& posicao_cavalo : jogada) {
            Graph grafo;
            grafo.preencher_grafo();
            int qnt_movimentos = distancia_rei(grafo, grafo.adicionar_cavalo(posicao_cavalo), grafo.adicionar_rei(posicao_rei));
            movimentos_jogada.push_back(qnt_movimentos);
            if (menor > qnt_movimentos) {
                menor = qnt_movimentos;
            }
        }
        jogadas_em_passos.push_back(movimentos_jogada);
        melhor_jogada_de_todas.push_back(menor);
    }

    while (!jogadas_em_passos.empty()) {
        list<int> jogadas_em_passos0 = jogadas_em_passos.front();
        jogadas_em_passos.pop_front();
        int melhor_jogada_de_todas0 = melhor_jogada_de_todas.front();
        melhor_jogada_de_todas.pop_front();
        for (auto& passos : jogadas_em_passos0) {
            if (passos == melhor_jogada_de_todas0) {
                cout << passos << " ";
            }
        }
        cout << "\n";
    }

    while (!jogadas_com_letras.empty()) {
        auto jogada = jogadas_com_letras.front();
        jogadas_com_letras.pop_front();
        for (auto& jogada_ptr : jogada) {
            delete[] jogada_ptr;
        }
    }

    return 0;
}
