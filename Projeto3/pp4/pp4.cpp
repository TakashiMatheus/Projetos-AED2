#include <vector>
#include <list>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <iostream>

using namespace std;

unsigned int potencia(int base, unsigned int expoente){
    int resultado = 1;
    for(unsigned int i = 0; i < expoente; i++){
        resultado *= base;
    }
    return resultado;
}

int calcularTamanhoString(const char* texto){
    int tamanho = 0;
    while (texto[tamanho] != '\0'){
        tamanho++;
    }
    return tamanho;
}

int funcaoHash(const char* texto, unsigned int tamanho){
    int iteradorTexto = 0;
    unsigned int k = 0;
    unsigned int i = 1;
    int valorHash;
    while(texto[iteradorTexto] != '\0'){
        k = k + ( texto[iteradorTexto] * (potencia(5, tamanho - i)));
        iteradorTexto++;
        i++;
    }
    valorHash = k % 11;
    return valorHash;
}

void mostrar_tabela_saltos(const vector<int>& R) {
    for (unsigned int i = 0; i < R.size(); ++i) {
        if (R[i] != -1) {
            cout << "R[" << char(i) << "] = " << R[i] << endl;
        } 
    }
}

vector<int> computar_tabela_saltos(vector<char> P){
    vector<int> R;
    int M = P.size();
    R.resize(256, -1);
    for( int j = 0; j < M; j++){
        R[P[j]] = j;
    }

    return R;
}

bool compararStrings(const char* str1, const char* str2) {
            while (*str1 != '\0' && *str2 != '\0') {
                if (*str1 != *str2) {
                    return false; 
                }
                str1++; 
                str2++; 
            }
            return (*str1 == '\0' && *str2 == '\0');
}


class Item {
    private:
        const char* chave;
        const char* latim;
        Item* anterior;
        Item* posterior;
    public:
        Item(const char* chave_, const char* latim_) : chave(chave_), latim(latim_), anterior(nullptr), posterior(nullptr) {};
        const char* get_chave() {return chave;}
        char get_latim() {return latim[0];}
        Item* get_anterior() {return anterior;}
        Item* get_posterior() {return posterior;}
        void set_posterior(Item* posterior_) {posterior = posterior_ ;}
        void set_anterior(Item* anterior_) {anterior = anterior_ ;}
};

class TabelaHash{
    private: 
        vector<Item*> tabela;
        unsigned int tamanho;
    public:
        TabelaHash() : tamanho(11) { tabela.resize(11, nullptr);};
        

        void inserir(const char* chave, const char* latim){
            unsigned int tamanho = calcularTamanhoString(chave);
            int indice = funcaoHash(chave, tamanho);
            if(tabela[indice] == nullptr){
                tabela[indice] = new Item(chave, latim);
            }
            else{
                Item* atual = tabela[indice];
                Item* novo = new Item(chave, latim);
                while(atual->get_posterior() != nullptr){
                    atual = atual->get_posterior();
                }
                atual->set_posterior(novo);
                novo->set_anterior(atual);
            }
        }

        char buscar(const char* chave){
            unsigned int tamanho = calcularTamanhoString(chave);
            int indice = funcaoHash(chave, tamanho);
            Item* iterando = tabela[indice];

            while(!(compararStrings(iterando->get_chave(), chave)) && iterando->get_posterior() != nullptr){
                iterando = iterando->get_posterior();
            }

            return iterando->get_latim();
        }

        void mostrarTabelaHash() {
        for (unsigned int i = 0; i < tabela.size(); ++i) {
            if (tabela[i] != nullptr) {
                cout << "Indice " << i << ": ";
                Item* iterando = tabela[i];
                while (iterando != nullptr) {
                    cout << "[" << iterando->get_chave() << " -> " << iterando->get_latim() << "] ";
                    iterando = iterando->get_posterior();
                }
                cout << endl;
            } else {
                cout << "Indice " << i << ": Vazio" << endl;
            }
    }
}
};

char voltar3Maiusculas(char letra, int valor) {
    if (letra >= 'A' && letra <= 'Z' && (letra != ' ' && letra != '.')) { 
        return 'A' + (letra - 'A' - valor + 26) % 26; 
    }
    return letra; 
}

char comparador(vector<char> trio, TabelaHash tabela){
    char trio_char[4];
    char latim;
    for(long long unsigned int k = 0; k < trio.size(); k++){
        trio_char[k] = trio[k];
    }
    trio_char[3] = '\0';
    latim = tabela.buscar(trio_char);
    return latim; 
}

vector<char> tradutor(const char* simbolos, TabelaHash tabela){
    vector<char> traduzido;
    vector<char> textoCasasVoltadas;
    int pulador = 0;
    bool loop = true;
    for(int i = 0; i < calcularTamanhoString(simbolos); i+= 3){
        vector<char> trio;
        char letra;
        trio.push_back(simbolos[i]);
        trio.push_back(simbolos[i+1]);
        trio.push_back(simbolos[i+2]);
        letra = comparador(trio, tabela);
        traduzido.push_back(letra);
    }

    while (loop){
        vector<char> textoCasasVoltadasLoop;
        for(int k = 0; k < traduzido.size(); k++){
            textoCasasVoltadasLoop.push_back((voltar3Maiusculas(traduzido[k], pulador)));
        }

        if(textoCasasVoltadasLoop[0] == 'M' && 
           textoCasasVoltadasLoop[1] == 'E' && 
           textoCasasVoltadasLoop[2] == 'N' &&
           textoCasasVoltadasLoop[3] == 'S' && 
           textoCasasVoltadasLoop[4] == 'A' && 
           textoCasasVoltadasLoop[5] == 'G' && 
           textoCasasVoltadasLoop[6] == 'E' && 
           textoCasasVoltadasLoop[7] == 'M'){
            textoCasasVoltadas = textoCasasVoltadasLoop;
            return textoCasasVoltadas;
         }
         pulador++;

    }
    return textoCasasVoltadas;
}

vector<char> toUpper(const char* palavraMinuscula){
    vector<char> maiuscula;
    for(int i = 0; i < calcularTamanhoString(palavraMinuscula); i++){
        char letra = palavraMinuscula[i];
        if (palavraMinuscula[i] >= 'a' && palavraMinuscula[i] <= 'z') {
            letra = palavraMinuscula[i] - 'a' + 'A';  
        }
        maiuscula.push_back(letra);
    }
    return maiuscula;
}

void boyer_moore(vector<char>T, const char* P){
    // for(int i = 0; i < T.size(); i++){
    //     cout << T[i];
    // }
    vector<char> maiusculos = toUpper(P);
    vector<int> R = computar_tabela_saltos(maiusculos);
    int N = T.size();
    int M = calcularTamanhoString(P);

    int salto = 0;
    
    for(int m = 0; m < maiusculos.size() + 1; m++){
        if(maiusculos[m] >= 'A' && maiusculos[m] <= 'Z'){
            cout << maiusculos[m];
        }
    }
    cout <<": " ;
    for(int i = 0; i <= N - M; i += salto){
        salto = 0;
        for(int j = M - 1; j >= 0; j--){
            if (maiusculos[j] != T[i + j]){
                salto = j - R[T[i+j]];
                if(salto < 1){
                    salto = 1;
                }
                cout << salto << ' ';
                break;
            }
        }
        if(salto == 0){
           cout << salto << ' ';
            cout << '(' << i << ')' <<' ';
            salto = 1;
        }
    }
    cout << endl;
}

const char* copiarString(const char* str) {
    int tamanho = calcularTamanhoString(str);
    char* copia = new char[tamanho];  
    for (int i = 0; i < tamanho + 1; i++) {
        copia[i] = str[i];
    }
    return copia;
}


int main() { 
    char texto[20000];
    char palavraProcura[100];
    vector<const char*> palavrasProcura;
    vector<char> textoTraduzido;
    TabelaHash hashTabela;
    hashTabela.inserir(":::", "A");
    hashTabela.inserir(".::", "B");
    hashTabela.inserir(":.:", "C");
    hashTabela.inserir("::.", "D");
    hashTabela.inserir(":..", "E");
    hashTabela.inserir(".:.", "F");
    hashTabela.inserir("..:", "G");
    hashTabela.inserir("...", "H");
    hashTabela.inserir("|::", "I");
    hashTabela.inserir(":|:", "J");
    hashTabela.inserir("::|", "K");
    hashTabela.inserir("|.:", "L");
    hashTabela.inserir(".|:", "M");
    hashTabela.inserir(".:|", "N");
    hashTabela.inserir("|:.", "O");
    hashTabela.inserir(":|.", "P");
    hashTabela.inserir(":.|", "Q");
    hashTabela.inserir("|..", "R");
    hashTabela.inserir(".|.", "S");
    hashTabela.inserir("..|", "T");
    hashTabela.inserir(".||", "U");
    hashTabela.inserir("|.|", "V");
    hashTabela.inserir("||.", "W");
    hashTabela.inserir("-.-", "X");
    hashTabela.inserir(".--", "Y");
    hashTabela.inserir("--.", "Z");
    hashTabela.inserir("---", " ");
    hashTabela.inserir("~~~", ".");

    //hashTabela.mostrarTabelaHash();
    cin >> texto;
    cin >> palavraProcura;
    // cout << texto;

    textoTraduzido = tradutor(texto, hashTabela);
    while(!compararStrings(palavraProcura, "fim") && !compararStrings(palavraProcura, "FIM")){
        
        palavrasProcura.push_back(copiarString(palavraProcura));
        cin >> palavraProcura;
    }
    
    // for(int j = 0; j < textoTraduzido.size(); j++){
    //     cout << textoTraduzido[j];
    // }

    // for (unsigned int i = 0; i < palavrasProcura.size(); ++i) {
    //     cout << "Elemento " << i << ": " << palavrasProcura[i] << endl;
    // }

    for(int i = 0; i < palavrasProcura.size(); i++){
        boyer_moore(textoTraduzido, palavrasProcura[i]);
    }

    // for(long long unsigned int i = 0; i < textoTraduzido.size() + 1; i++){
    //     cout << textoTraduzido[i];
    // }
}