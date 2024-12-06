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
    list<VertexWeightPair<T>> get_adj(Vertex v) const { return adj[v]; }
    uint get_num_vertices() const { return num_vertices; }
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
        somaPesos = 0; // Reset antes de calcular
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
    adj = new list<VertexWeightPair<T>>[num_vertices];
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
void inicializa(WeightedGraphAL<T>& grafo, Vertex vertice) {
    for (uint i = 0; i < grafo.get_num_vertices(); i++) {
        for (auto& pair : grafo.get_adj(i)) {
            pair.set_distancia(numeric_limits<Weight>::max());
            pair.set_pi(nullptr);
        }
    }
    for (auto& pair : grafo.get_adj(vertice)) {
        pair.set_distancia(0);
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
        if (r_it->get_distancia() < A->get_distancia()) {
            minor = r;
        }
    }

    if (minor != i) {
        swap(*A, *next(Q.begin(), minor));
        min_heapfy(Q, minor);
    }
}

void build_min_heap(list<VertexWeightPair<Vertex>>& Q) {
    for (int i = Q.size() / 2 - 1; i >= 0; i--) {
        min_heapfy(Q, i);
    }
}

VertexWeightPair<Vertex> extract_min(list<VertexWeightPair<Vertex>>& Q) {
    if (Q.empty()) throw runtime_error("Heap is empty");

    auto min_elem = Q.front();
    Q.front() = Q.back();
    Q.pop_back();
    min_heapfy(Q, 0);
    return min_elem;
}

template<typename T>
void relaxa(WeightedGraphAL<T>& grafo, Vertex u, VertexWeightPair<T> v, list<VertexWeightPair<Vertex>>& Q) {
    for (const auto& adj : grafo.get_adj(u)) {
        if (adj.get_vertex() == v.get_vertex()) {
            if (v.get_distancia() > adj.get_distancia() + v.get_weight()) {
                Weight novaDistancia = adj.get_distancia() + v.get_weight();
                for (auto& elemento : Q) {
                    if (elemento.get_vertex() == v.get_vertex()) {
                        elemento.set_distancia(novaDistancia);
                        elemento.set_pi(&v); 
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

    for (uint i = 0; i < grafo.get_num_vertices(); i++) {
        for (auto& elemento : grafo.get_adj(i)) {
            for(auto& verificando : Q){
                if(elemento.get_vertex() == verificando.get_vertex()){
                    break;
                }
                Q.push_back(elemento);
            }
        }
    }

    
    while (!Q.empty()) {
        build_min_heap(Q);
        VertexWeightPair<Vertex> u = extract_min(Q);
        S.push_back(u);
        
        for (const auto& v : grafo.get_adj(u.get_vertex())) {
            relaxa(grafo, u.get_vertex(), v, Q);
        }
    }

    return S;
}

list<list<float>> make_set(int num_vertices) {
    list<list<float>> vertices;
    for (float i = 1; i <= num_vertices; i++) {
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
        cout << "Vetex: " << blocosNeuronioMinimo.front().get_vertex() << "somaVAlor: " << blocosNeuronioMinimo.front().get_somaPesos();

    
    preencherBlocos(blocosNeuronioMinimo);
    
    
    for(const auto& guia : blocosNeuronioMinimo){
        if(guia.get_vertex() == saida){
            total += guia.get_somaPesos();
            condicao = guia.get_pi();
        }
    }

    while(condicao != nullptr){
        float valor = condicao->get_somaPesos();
        total += valor;
        condicao = condicao->get_pi();
    }

    cout << total;
    return 0;
}
