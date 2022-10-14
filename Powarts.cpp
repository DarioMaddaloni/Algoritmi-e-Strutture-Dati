#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

struct link
{
    uint16_t id;
    uint16_t weight;
};

struct nodo
{
    uint16_t id;
    uint16_t contatore;
    bool visitato;
    int distanza;
    vector<link> padri;
    vector<link> figli;
    vector<link> linked;
};

vector<nodo> grafo;

auto cmp = [](int left, int right) { return grafo[left].distanza > grafo[right].distanza; };

//data la struttura con tutti i padri, costruisco un'altra funzione che mi costruisce i figli
void dijkstra(int N, int P)
{

    for (int i = 0; i < N; i++)
    { //associo ad ogni nodo una distanza infinita, cos� appena trovo il primo percorso, avr� sicuramente peso minore
        grafo[i].distanza = INT_MAX;
    }
    grafo[P].distanza = 0; //associo a Powarts peso 0

    priority_queue<int, vector<int>, decltype(cmp)> codaPrioritaria(cmp);

    codaPrioritaria.push(P); //Inizio da Powarts per cercare i percorsi pi� brevi
    while (codaPrioritaria.size() != 0)
    {

        uint16_t indice = codaPrioritaria.top();
        codaPrioritaria.pop();

        if (grafo[indice].visitato == true)
        {
            //sei gi� passato da quel nodo attraverso una strada migliore
        }
        else
        {

            for (int i = 0; i < (int)grafo[indice].linked.size(); i++)
            { //guardo tutte le citt� vicine alla citt� di id indice

                if (grafo[indice].distanza + (int)grafo[indice].linked[i].weight < grafo[grafo[indice].linked[i].id].distanza)
                {                                                    //ho trovato un percorso pi� breve
                    grafo[grafo[indice].linked[i].id].padri.clear(); //elimino tutti i padri

                    link padreCollegato; //cambio padre (almeno per ora)
                    padreCollegato.id = indice;
                    padreCollegato.weight = grafo[indice].linked[i].weight;
                    grafo[grafo[indice].linked[i].id].padri.push_back(padreCollegato);

                    grafo[grafo[indice].linked[i].id].distanza = grafo[indice].distanza + grafo[indice].linked[i].weight; //memorizzo la distanza del nodo i finora raggiunta

                    codaPrioritaria.push(grafo[indice].linked[i].id); //partir� da qui per i prossimi tragitti
                }
                else if (grafo[indice].distanza + (int)grafo[indice].linked[i].weight > grafo[grafo[indice].linked[i].id].distanza)
                {
                    //non fare nulla
                }
                else if (grafo[indice].distanza + (int)grafo[indice].linked[i].weight == grafo[grafo[indice].linked[i].id].distanza)
                {
                    //non serve aggiungerlo perch� se ha gi� un valore distanza associato, significa che ci sono gi� passato con un percorso equivalentemente pesante

                    link padreCollegato; //aggiungo un padre
                    padreCollegato.id = indice;
                    padreCollegato.weight = grafo[indice].linked[i].weight;
                    grafo[grafo[indice].linked[i].id].padri.push_back(padreCollegato);
                    //non serve che memorizzo la distanza perch� � gi� stata memorizzata precedentemente e non verrebbe variata.

                    codaPrioritaria.push(grafo[indice].linked[i].id); //partir� da qui per i prossimi tragitti
                }
            }

            grafo[indice].visitato = true;
        }
    }

    //ora costruisco vector<link> figli
    for (int i = 0; i < N; i++)
    {
        link nodoFiglio;
        nodoFiglio.id = i;
        for (int j = 0; j < grafo[i].padri.size(); j++)
        {
            nodoFiglio.weight = grafo[i].padri[j].weight;
            grafo[grafo[i].padri[j].id].figli.push_back(nodoFiglio);
        }
    }
}

// calcola le città per cui i gufi sono obbligati a passare e alza i rispettivi contatori di 1 (se non erano già stati alzati)
set<uint16_t> alzaContatori(uint16_t city, uint16_t P)
{
    // Il caso base è Powarts che non ha passaggi obbligati
    set<uint16_t> passaggiObbligati;
    if (city == P)
        return passaggiObbligati;

    // Itero tutti i padri, ottengo i loro passaggi obbligati e l'intersezione di questi
    // assieme alla città corrente sono i passaggi obbligati per la città corrente
    for (int i = 0; i < grafo[city].padri.size(); i++)
    {
        uint16_t padre = grafo[city].padri[i].id;
        set<uint16_t> passaggiObbligatiPadre = alzaContatori(padre, P);

        if (i == 0)
        {
            passaggiObbligati = passaggiObbligatiPadre;
        }
        else
        {
            set<uint16_t> intersect;
            set_intersection(passaggiObbligati.begin(), passaggiObbligati.end(),
                passaggiObbligatiPadre.begin(), passaggiObbligatiPadre.end(),
                std::inserter(intersect, intersect.begin()));
            passaggiObbligati = intersect;
        }
    }
    passaggiObbligati.insert(city);

    // se è la prima volta che calcolo i passaggi obbligati per questa città
    // allora alzo i contatori dei passaggi obbligati
    if (!grafo[city].visitato)
    {
        for (set<uint16_t>::iterator it = passaggiObbligati.begin(); it != passaggiObbligati.end(); it++)
        {
            grafo[*it].contatore++;
        }
    }
    grafo[city].visitato = true;

    return passaggiObbligati;
}

uint16_t idAttaccato(uint16_t P)
{
    // calcolo i contatori per tutti i nodi del grafo
    for (uint16_t i = grafo.size() - 1; i >= 0; i--)
    {
        if (i != P && !grafo[i].visitato)
        {
            alzaContatori(i, P);
        }

        if (i == 0)
            break;
    }

    // cerco nel grafo il nodo con contatore massimo
    uint16_t maxId = 0;
    uint16_t maxContatori = 0;
    for (uint16_t i = grafo.size() - 1; i >= 0; i--)
    {
        if (i != P && grafo[i].contatore > maxContatori)
        {
            maxContatori = grafo[i].contatore;
            maxId = i;
        }

        if (i == 0)
            break;
    }

    return maxId;
}



vector<uint16_t> listaCitta(uint16_t ban);
vector<uint16_t> cityListFiller(vector<uint16_t> acc, uint16_t curr, priority_queue<int, vector<int>, decltype(cmp)> q);
bool parentsInAcc(uint16_t curr, vector<uint16_t> acc);

//setup per la funzione cityListFiller
//per padre di un nodo A intendiamo un nodo che compare subito prima del nodo A in un percorso
//a distanza minima dall'UNI
//(*) ci basiamo sull'osservazione che un nodo viene bannato se tutti i suoi padri sono bannati, altrimenti
//saremo in una situazione in cui c'e' un path a distanza minima dall'UNI che non passa per la citta' bannata
//inoltre, a questo punto del codice visitato=true per tutti i nodi (tranne forse per l'uni),
//possiamo quindi usarlo come variabile d'appoggio, cambiandone il significato: indicherà se il nodo sarà
//già stato messo in queue
vector<uint16_t> listaCitta(uint16_t ban)
{
    vector<uint16_t> acc;
    priority_queue<int, vector<int>, decltype(cmp)> q(cmp);
    return cityListFiller(acc, ban, q);
}

//non so se compila, potrebbe dare problemi la parte della firma relativa alla queue
vector<uint16_t> cityListFiller(vector<uint16_t> acc, uint16_t curr, priority_queue<int, vector<int>, decltype(cmp)> q)
{
    //aggiungo l'elemento corrente e aggiungo in coda i suoi figli
    acc.push_back(curr);
    for (link figlio : grafo[curr].figli)
    {
        if (grafo[figlio.id].visitato == true)
        {
            q.push(figlio.id);
            grafo[figlio.id].visitato = false;
        }
    }
    //prendo l'elemento a distanza minima dall'uni
    if (q.empty())
    {
        //se la coda e' vuota, non ci sono piu' elementi candidabili ad essere esclusivamente collegati
        //tramite un percorso non piu' legale, ho quindi finito e acc contiene tutte le citta'
        //per cui tutti i cammini a distanza minima dall'UNI passano per ban (**)
        return acc;
    }
    uint8_t next = q.top();
    q.pop();
    //ora ciclo finche' l'elemento a distanza minima non rispetta la condizione spiegata qua (*)
    while (!parentsInAcc(next, acc))
    {
        if (q.empty())
        {
            //s(**)
            return acc;
        }
        next = q.top();
        q.pop();
    }
    return cityListFiller(acc, next, q);
}

//questa funzione mi controlla se tutti i genitori di un nodo sono in acc
//visto che prendiamo dalla queue sempre l'elemento con distanza minima, o i suoi padri sono stati
//aggiunti ad acc prima di lui, o non lo saranno mai
bool parentsInAcc(uint16_t curr, vector<uint16_t> acc)
{
    for (link padre : grafo[curr].padri)
    {
        if (find(acc.begin(), acc.end(), padre.id) == acc.end())
        {
            //il genitore corrente non appartiene alle citta' coperte dai maghi,
            //ergo c'� un path alternativo per il gufo per arrivare alla citta' corrente
            return false;
        }
    }
    //ci sono tutti i parents
    return true;
}

int main()
{
    ifstream in("input0.txt");
    int N, M;
    uint16_t P;
    in >> N >> M >> P;
    grafo.resize(N);

    link collegato;
    link collegatoInverso;

    for (int i = 0; i < N; i++)
    {
        grafo[i].id = i;
    }

    for (int j = 0; j < M; j++)
    {
        int from, to, peso;
        in >> from >> to >> peso;

        collegato.id = to;
        collegato.weight = peso;
        grafo[from].linked.push_back(collegato);

        collegatoInverso.id = from;
        collegatoInverso.weight = peso;
        grafo[to].linked.push_back(collegatoInverso);
    }

    dijkstra(N, P);
    uint16_t attaccato = idAttaccato(P);
    vector<uint16_t> res = listaCitta(attaccato);
    cout << res.size() << endl;
    for (uint16_t n : res)
    {
        cout << n << endl;
    }

    return 0;
}