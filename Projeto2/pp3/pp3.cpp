#include <iostream>
#include <list>
#include <limits>

typedef unsigned int uint;
typedef unsigned int Vertex;
typedef float Weight;
using namespace std;

template <typename T>
class VertexWeightPair;

template <typename T>
class WeightedGraphAL {
private:
    uint num_vertices;
    uint num_edges;
    list<VertexWeightPair<T>>* adj;

public:
    WeightedGraphAL(uint num_vertices);
    ~WeightedGraphAL();
    void add_edge(Vertex u, Vertex v, Weight w);
    list<VertexWeightPair<T>>& get_adj(Vertex v) const { return adj[v]; }
    uint get_num_vertices() const { return num_vertices + 1; }
    uint get_num_edges() const { return num_edges; }
};

template<typename T>
class VertexWeightPair {
private:
    VertexWeightPair* pi;
    Vertex vertex;
    Weight weight;
    uint doente;
    Weight distancia;
    float somaPesos;
    WeightedGraphAL<Vertex>* blocoNeuronio;

public:
    VertexWeightPair(Vertex vertex, Weight weight)
        : vertex(vertex), weight(weight),pi(nullptr), doente(0),
          distancia(numeric_limits<Weight>::max()),
          somaPesos(0), blocoNeuronio(nullptr) {}

    Vertex get_vertex() const { return vertex; }
    Weight get_weight() const { return weight; }
    uint get_doente() const { return doente; }
    void set_doente(uint valor) { doente = valor; }
    void set_pi(VertexWeightPair* valor) { pi = valor; }
    VertexWeightPair* get_pi() const { return pi; }
    void set_distancia(Weight valor) { distancia = valor; }
    Weight get_distancia() const { return distancia; }

    void set_block(uint num_vertices) {
        if (blocoNeuronio != nullptr) {
            delete blocoNeuronio;
        }
        blocoNeuronio = new WeightedGraphAL<Vertex>(num_vertices);
    }

    WeightedGraphAL<Vertex>& get_blocoNeuronio() {
        if (blocoNeuronio == nullptr) {
            throw runtime_error("blocoNeuronio is not initialized");
        }
        return *blocoNeuronio;
    }

    void set_somaPesos(list<float> A) {
        somaPesos = 0; 
        for (const auto& somando : A) {
            somaPesos += somando;
        }
    }

    float get_somaPesos() const {
        return somaPesos;
    }
};

template<typename T>
WeightedGraphAL<T>::WeightedGraphAL(uint num_vertices) : num_vertices(num_vertices), num_edges(0) {
    adj = new list<VertexWeightPair<T>>[num_vertices + 1];
}

template<typename T>
WeightedGraphAL<T>::~WeightedGraphAL() {
    delete[] adj;
}

template<typename T>
void WeightedGraphAL<T>::add_edge(Vertex u, Vertex v, Weight w) {
    adj[u].push_back(VertexWeightPair<T>(v, w));
    adj[v].push_back(VertexWeightPair<T>(u, w));
    num_edges++;
}

template<typename T>
void InitRedeNeural(WeightedGraphAL<T>& g, uint num_edges) {
    Vertex u, v;
    Weight w;
    for (uint i = 0; i < num_edges; i++) {
        cin >> u >> v >> w;

        if (u < g.get_num_vertices() && v < g.get_num_vertices() && w >= 0) {
            g.add_edge(u, v, w);
        }
    }
}

template<typename T>
void inicializa(WeightedGraphAL<T>& grafo, Vertex inicial) {
    for (uint i = 0; i < grafo.get_num_vertices(); i++) {
        for (auto& pair : grafo.get_adj(i)) {
            pair.set_distancia(numeric_limits<Weight>::max());
            pair.set_pi(nullptr);
            if(pair.get_vertex() == inicial){
                pair.set_distancia(0);
            }
        }
    }
}

uint esquerda(uint i) {if(i > 0) return 2 * i + 1;  else return i + 1;}
uint direita(uint i) { if(i > 0)return 2 * i + 2; else return i + 2; }
uint pai(uint i) { return (i - 1) / 2;}

void min_heapfy(list<VertexWeightPair<Vertex>>& Q, uint i) {
    uint l = esquerda(i);
    uint r = direita(i);
    uint minor = i;

    auto A = Q.begin();
    advance(A, i);

    if (l < Q.size()) {
        auto l_it = Q.begin();
        advance(l_it, l);
        if (l_it->get_distancia() < A->get_distancia()) {
            minor = l;
        }
    }

    if (r < Q.size()) {
        auto r_it = Q.begin();
        advance(r_it, r);
        auto minor_it = Q.begin();
        advance(minor_it, minor); 

        if (r_it->get_distancia() < minor_it->get_distancia()) {
            minor = r;
        }
    }


    if (minor != i) {
        swap(*A, *next(Q.begin(), minor));

        min_heapfy(Q, minor);
    }
}


void build_min_heap(list<VertexWeightPair<Vertex>>& Q) {
    for (int i = Q.size() - 1; i >= 0; i--) {
        min_heapfy(Q, i);
    }
}

VertexWeightPair<Vertex> extract_min(list<VertexWeightPair<Vertex>>& Q) {
    if (Q.empty()) throw runtime_error("Heap is empty");

    auto min_elem = Q.front();
    Q.front() = Q.back();
    Q.pop_back();
    return min_elem;
}

template<typename T>
void relaxa(list<VertexWeightPair<T>> &S, Vertex u, const VertexWeightPair<T>& v, Weight peso, list<VertexWeightPair<Vertex>>& Q) {
    for (auto& valor_dist_u : S) {
        if (valor_dist_u.get_vertex() == u) {
            if (v.get_distancia() > valor_dist_u.get_distancia() + peso) {
                Weight novaDistancia = valor_dist_u.get_distancia() + peso;
                for (auto& elementoU : Q) {
                    if (elementoU.get_vertex() == v.get_vertex()) {
                            
                            elementoU.set_distancia(novaDistancia);
                            elementoU.set_pi(&valor_dist_u);
                        //     if(elementoU.get_vertex() == 7) {
                        //     cout << "Vertex: " << elementoU.get_vertex() << ", SomaPesos: " << elementoU.get_somaPesos() <<", Doentes: "<< elementoU.get_doente();
                        //     if(elementoU.get_pi() != nullptr){
                        //         cout<<", Ponteiro: "<< elementoU.get_pi()->get_vertex() << endl;
                        //     }
                        // }
                            break;
                    }
                }
            }
        }
    }
}

template<typename T>
list<VertexWeightPair<Vertex>> AlgoritmoDijkstra(WeightedGraphAL<T>& grafo, Vertex inicial) {
    inicializa(grafo, inicial);
    list<VertexWeightPair<Vertex>> S;
    list<VertexWeightPair<Vertex>> Q;
    bool existe = false;

    for (uint i = 1; i < grafo.get_num_vertices(); i++) {
        for (const auto& elemento : grafo.get_adj(i)) {
            existe = false;
            for(const auto& verificador: Q){
                if(elemento.get_vertex() == verificador.get_vertex()){
                    existe = true;
                    break;
                }
            }
            if(!existe){
                Q.push_back(elemento);
            }
        }
    }
    
    build_min_heap(Q);

    while (!Q.empty()) {
        VertexWeightPair<Vertex> u = extract_min(Q);
        S.push_back(u);
        for (const auto& v : grafo.get_adj(u.get_vertex())) {
            for(const auto& trocando : Q){
                if(v.get_vertex() == trocando.get_vertex()){
                    relaxa(S, u.get_vertex(), trocando, v.get_weight(), Q);
                }
            }
        }
    //     cout << "Antes" << endl;
    //     for(auto& checando : Q){

    //     if(checando.get_pi() != nullptr){
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: "<< checando.get_pi()->get_vertex() << " Distancia: "<< checando.get_distancia() << endl;
    //     }
    //     else{
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: VAZIO"<< endl;
    //     }
    //   }
    //     cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<< endl;

        build_min_heap(Q);  

    //     cout << "Depois" << endl;
    //     for(auto& checando : Q){

    //     if(checando.get_pi() != nullptr){
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: "<< checando.get_pi()->get_vertex() << " Distancia: "<< checando.get_distancia() << endl;
    //     }
    //     else{
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: VAZIO"<< endl;
    //     }
    // }
        // cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<< endl;
    }
    // for(auto& checando : S){
    //     if(checando.get_pi() != nullptr){
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: "<< checando.get_pi()->get_vertex()<< endl;
    //     }
    //     else{
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: VAZIO"<< endl;
    //     }
    // }
    return S;
}


list<list<float>> make_set(int num_vertices) {
    list<list<float>> vertices;
    for (float i = 1; i < num_vertices; i++) {
        list<float> elemento;
        elemento.push_back(i);
        vertices.push_back(elemento);
    }
    return vertices;
}

template<typename T>
list<list<T>> ordenar(list<list<T>>& listaDeArestas) {
    for (auto it1 = listaDeArestas.begin(); it1 != listaDeArestas.end(); ++it1) {
        for (auto it2 = next(it1); it2 != listaDeArestas.end(); ++it2) {
            if (it1->back() > it2->back()) {
                swap(*it1, *it2);
            }
        }
    }
    return listaDeArestas;
}

list<VertexWeightPair<Vertex>> ordenarCaminhoMinimo(list<VertexWeightPair<Vertex>>& listaCaminhoMinimo) {
    list<VertexWeightPair<Vertex>> listaTemporaria;
    for (const auto& item : listaCaminhoMinimo) {
        listaTemporaria.push_back(item);
    }

    bool trocou;
    do {
        trocou = false;
        for (auto it1 = listaTemporaria.begin(); next(it1) != listaTemporaria.end(); ++it1) {
            auto it2 = next(it1);
            if (it1->get_vertex() > it2->get_vertex()) {
                iter_swap(it1, it2);
                trocou = true;
            }
        }
    } while (trocou);

    return listaTemporaria;
}

template<typename T>
list<list<T>> makeParArestas(WeightedGraphAL<T>& grafo) {
    list<list<T>> todas_arestas;
    for (Vertex i = 0; i < grafo.get_num_vertices(); i++) {
        for (const auto& elemento : grafo.get_adj(i)) {
            list<T> par_de_arestas;
            par_de_arestas.push_back(i);
            par_de_arestas.push_back(elemento.get_vertex());
            par_de_arestas.push_back(elemento.get_weight());
            todas_arestas.push_back(par_de_arestas);
        }
    }
    ordenar(todas_arestas);
    return todas_arestas;
}

template<typename T>
list<float> mst_kruskal(WeightedGraphAL<T>& grafoDentroDoBloco) {
    list<float> A;
    list<list<float>> verticeDijunto = make_set(grafoDentroDoBloco.get_num_vertices());
    list<list<T>> parArestas = makeParArestas(grafoDentroDoBloco);

    auto find = [&verticeDijunto](float vertex) {
        for (auto& set : verticeDijunto) {
            for (const auto& val : set) {
                if (val == vertex) {
                    return &set;
                }
            }
        }
        return static_cast<list<float>*>(nullptr);
    };

    auto union_sets = [&verticeDijunto](list<float>* set1, list<float>* set2) {
        if (set1 && set2 && set1 != set2) {
            set1->insert(set1->end(), set2->begin(), set2->end());
            verticeDijunto.remove(*set2);
        }
    };

    for (const auto& pares : parArestas) {
        Vertex u = pares.front();
        Vertex v = *(++pares.begin());
        Weight w = *(++(++pares.begin()));

        list<float>* set_u = find(u);
        list<float>* set_v = find(v);

        if (set_u != set_v) {
            A.push_back(w);
            union_sets(set_u, set_v);
        }
    }

    return A;
}

template<typename T>
void preencherBlocos(list<VertexWeightPair<T>>& caminhoMinimo) {
    for (auto& bloco : caminhoMinimo) {
        uint num_vertices, num_edges, qnt_doentes, neuronio;
        list<uint> neu_doentes;
        list<float> A;
        cin >> num_vertices >> num_edges >> qnt_doentes;
        if (qnt_doentes > 0){
            for (uint i = 0; i < qnt_doentes; i++) {
                cin >> neuronio;
                neu_doentes.push_back(neuronio);
            }
            bloco.set_doente(neu_doentes.size());
        }
        bloco.set_block(num_vertices);
        InitRedeNeural(bloco.get_blocoNeuronio(), num_edges);

        A = mst_kruskal(bloco.get_blocoNeuronio());
        bloco.set_somaPesos(A);
    }
}

int main()
{
    uint num_vertices, num_edges, entrada, saida;
    VertexWeightPair<Vertex>* condicao = nullptr;
    float total = 0;

    cin >> num_vertices >> num_edges;
    WeightedGraphAL<Vertex> blocoDeNeuronios(num_vertices);

    InitRedeNeural(blocoDeNeuronios, num_edges);
    cin >> entrada >> saida;

    list<VertexWeightPair<Vertex>> blocosNeuronioMinimo = AlgoritmoDijkstra(blocoDeNeuronios, entrada);


    // for(auto& checando : blocosNeuronioMinimo){
    //     if(checando.get_pi() != nullptr){
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: "<< checando.get_pi()->get_vertex()<< endl;
    //     }
    //     else{
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: VAZIO"<< endl;
    //     }
    // }
    // cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;


    list<VertexWeightPair<Vertex>>blocosNeuronioMinimoOrdenado = ordenarCaminhoMinimo(blocosNeuronioMinimo);


    // for(auto& checando : blocosNeuronioMinimoOrdenado){
    //     if(checando.get_pi() != nullptr){
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: "<< checando.get_pi()->get_vertex()<< endl;
    //     }
    //     else{
    //         cout << "Vetor: " <<checando.get_vertex() << " PI: VAZIO"<< endl;
    //     }
    // }


    preencherBlocos(blocosNeuronioMinimoOrdenado);

    // for(uint i = 0; i < blocosNeuronioMinimoOrdenado.size(); i++){
    //     cout << i << " : ";
    //     for(const auto& printando : blocoDeNeuronios.get_adj(i)){
    //         cout << "("<< printando.get_vertex()<<", " << printando.get_weight() << ")" << ", ";
    //     }
    //     cout << endl;
    // }
    // cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<< endl;


//     for (const auto& guia : blocosNeuronioMinimoOrdenado) {
//     cout << "Vertex: " << guia.get_vertex() << ", SomaPesos: " << guia.get_somaPesos() <<", Doentes: "<< guia.get_doente();
//     if(guia.get_pi() != nullptr){
//         cout<<", Ponteiro: "<< guia.get_pi()->get_vertex() << endl;
//     }
//     else{
//         cout<<", Ponteiro: NULLPTR" <<endl;
//     }
// }


    for(const auto& guia : blocosNeuronioMinimoOrdenado) {
    if(guia.get_vertex() == saida) {
        // cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<< endl;
        // cout << "Estou: " << guia.get_vertex();
        // if (guia.get_pi() != nullptr) {
        //     cout << "// Vou para: " << guia.get_pi()->get_vertex()<< endl;
        // }
        condicao = guia.get_pi();
        if(guia.get_doente() > 0) {
            total += guia.get_somaPesos();
        }
    }
}

while(condicao != nullptr) {
    Vertex novaPosicao = condicao->get_vertex();
    for(auto& vertices : blocosNeuronioMinimoOrdenado){
        if(vertices.get_vertex() == novaPosicao){
            if(vertices.get_doente() >  0){
                total += vertices.get_somaPesos();
            }
            condicao = vertices.get_pi();
        }
    }

}

cout <<total;
}
