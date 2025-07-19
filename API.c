#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


#define MAXLEN 256
#define HASH_SIZE 1024
#define HASH_SIZE_MAGAZZINO 1024
#define HASH_SIZE_INGREDIENTI 512 
#define BUFF_INIZIALE 5000

//HASH-TABLE INGREDIENTI

typedef struct ingredienti
{
    char* ingrediente;
    //puntatore che mi permette di accedere dal ricettario al magazzino
    struct ingredienti_magazzino* magazzino;
    struct ingredienti* next;

}ingredienti;


typedef struct 
{
    ingredienti* lista_ingredienti;

}Hash_table_ingredienti;



//HASH-TABLE RICETTARIO
typedef struct 
{
    ingredienti* ingrediente;
    uint32_t qt;

}ingredienti_ricettario;


typedef struct Ricetta
{
    uint32_t count; 
    uint32_t numero_ingredienti;
    char* nome;
    struct Ricetta* next;
    struct Ricetta* prev;
    ingredienti_ricettario* ingredienti_ricettario; 
    
    
}Ricetta;


typedef struct 
{
    Ricetta* lista_ricette;

}Hash_table_ricettario;



//HASH-TABLE MAGAZZINO
typedef struct 
{
    uint32_t scadenza; 
    int q; 
    
}min_heap;


typedef struct ingredienti_magazzino
{
    uint32_t dimensione;
    ingredienti* ingrediente;
    int q_tot;
    min_heap* lotti; 
    struct ingredienti_magazzino* next;
    
}ingredienti_magazzino;


typedef struct 
{
    ingredienti_magazzino* lista_ingredienti;

}Hash_table_magazzino;



//CODA ORDINI
typedef struct Ordine
{
    Ricetta* ordine; 
    int tempo_ordine;
    uint32_t numero_ordini;
    uint32_t index;
    
    struct Ordine* next;
    struct Ordine* prev;

}Ordine;


typedef struct 
{
    Ordine* testa;
    Ordine* coda;


}coda_ordini;



//CODA SPEDIZIONI
typedef struct 
{
    int tempo_ordine;
    Ricetta* ordine; //per il nome e altro
    uint32_t numero_ordini;
    uint32_t peso;
        
}min_heap_spedizione;


typedef struct 
{
    min_heap_spedizione* coda;
    uint32_t dimensione;

}coda_spedizioni;



//CODA DI STAMPA
typedef struct 
{
    uint32_t numero_ordini; 
    int tempo_ordine;
    uint32_t peso;
    Ricetta* nome_ordine; 
}max_heap;


typedef struct 
{
    max_heap* pacchi; 
    uint32_t dimensione;

}coda_di_stampa;





Hash_table_ricettario* crea_tabella(){
    Hash_table_ricettario* tabella = (Hash_table_ricettario*)malloc(HASH_SIZE * sizeof(Hash_table_ricettario));
    
    for (int i = 0; i < HASH_SIZE; i++)
    {
        tabella[i].lista_ricette = NULL;
    }
    return tabella;    
}


Hash_table_ingredienti* crea_tabella_ingredienti(){
    Hash_table_ingredienti* tabella_ingredienti = (Hash_table_ingredienti*)malloc(HASH_SIZE_INGREDIENTI * sizeof(Hash_table_ingredienti));
    
    for (int i = 0; i < HASH_SIZE_INGREDIENTI; i++)
    {
        tabella_ingredienti[i].lista_ingredienti = NULL;
    }
    return tabella_ingredienti;    
}


Hash_table_magazzino* crea_tabella_magazzino(){
    Hash_table_magazzino* tabella = (Hash_table_magazzino*)malloc(HASH_SIZE_MAGAZZINO * sizeof(Hash_table_magazzino));
    
    for (int i = 0; i < HASH_SIZE_MAGAZZINO; i++)
    {
        tabella[i].lista_ingredienti = NULL;
    }
    return tabella;    
}


char parser(char parola[],int i){
    return parola[i];
}


//cormen pag 260
unsigned long int hash_function(char stringa[MAXLEN]){
    unsigned long int hash = 1;
    int a = 37;
    int b = 41;
    
    for (int i = 0; i < strlen(stringa); i++)
                {
                    hash += (a * stringa[i] + b) % 3011;
                }

    return hash % HASH_SIZE;
}


unsigned long int hash_function_magazzino(char stringa[MAXLEN]){
    unsigned long int hash = 1;
    int a = 37;
    int b = 41;
    
    for (int i = 0; i < strlen(stringa); i++)
                {
                    hash += (a * stringa[i] + b) % 3011;
                }

    return hash % HASH_SIZE_MAGAZZINO;
}


unsigned long int hash_function_ingredienti(char stringa[MAXLEN]){
    unsigned long int hash = 1;
    int a = 37;
    int b = 41;
    
    for (int i = 0; i < strlen(stringa); i++)
                {
                    
                    hash += (a * stringa[i] + b) % 3011;
                }

    return hash % HASH_SIZE_INGREDIENTI;
}


Ricetta* ricerca_ricetta_chained(Hash_table_ricettario* tabella,char ricetta_nome[MAXLEN],int key){
    
    if (tabella[key].lista_ricette == NULL)
    {
        //se non è presente una lista_ricetta nel bucket ritorno NULL
        return NULL;
    }
    else
    {
        
        Ricetta* ricetta = tabella[key].lista_ricette;
        
        while (ricetta != NULL)
        {
            if (ricetta->nome[1] == ricetta_nome[1])
            {
                if (strcmp(ricetta->nome, ricetta_nome) == 0)
                {
                    //altrimenti lo cerco e se è presente lo ritorno
                    return ricetta;
                }
            }
            
            
            ricetta = ricetta->next;            
        }

        return NULL;
        
    }
    
}


ingredienti* ricerca_ingredienti_chained(Hash_table_ingredienti* tabella,char ingrediente_nome[MAXLEN],int key){
//simile a ricerca_ricetta_chained
    if (tabella[key].lista_ingredienti == NULL)
    {
        return NULL;
    }

    else
    { 
        
        ingredienti* ingrediente = tabella[key].lista_ingredienti;

        while (ingrediente != NULL)
        {
            if (ingrediente->ingrediente[2]==ingrediente_nome[2])
            {
                if (strcmp(ingrediente->ingrediente, ingrediente_nome) == 0)
                {
                    return ingrediente;
                }
            }
            
            
            ingrediente = ingrediente->next;            
        }
        return NULL;
        
    }
    
}


ingredienti_magazzino* ricerca_ingredienti_magazzino_chained(Hash_table_magazzino* tabella,char ingrediente_nome[MAXLEN],int key){
    
    if (tabella[key].lista_ingredienti == NULL)
    {
        return NULL;
    }
    else
    {
        
        ingredienti_magazzino* ingrediente_temp = tabella[key].lista_ingredienti;
        
        while (ingrediente_temp != NULL)
        {
            if (strlen(ingrediente_temp->ingrediente->ingrediente) == strlen(ingrediente_nome))
            {
                if (strcmp(ingrediente_temp->ingrediente->ingrediente, ingrediente_nome) == 0)
                {
                    return ingrediente_temp;
                }
            }
                        
            ingrediente_temp = ingrediente_temp->next;            
        }
        
        return NULL;
        
    }
    
}



int parent(int i){
    return (i - 1)/2;
}


int left(int i){
    return 2*i + 1;
}


int right(int i){
    return 2*i + 2;
}


void MinHeapify_lotti(ingredienti_magazzino* ingrediente, uint32_t i){
    uint32_t l = left(i);
    uint32_t r = right(i);  
    uint32_t minimo = i;

    //printf("\n%d %d %d", ingrediente->lotti[l].scadenza, ingrediente->lotti[r].scadenza, ingrediente->lotti[i].scadenza);
    if (l < ingrediente->dimensione && (ingrediente->lotti[l].scadenza < ingrediente->lotti[i].scadenza))
    {
        minimo = l;
    }
    else
    {
        minimo = i;
    }
    if (r < ingrediente->dimensione && (ingrediente->lotti[r].scadenza < ingrediente->lotti[minimo].scadenza))
    {
        minimo = r;
    }
    if (minimo != i)
    {
        
        min_heap lotto_temp;
        lotto_temp = ingrediente->lotti[minimo];
        ingrediente->lotti[minimo] = ingrediente->lotti[i];
        ingrediente->lotti[i] = lotto_temp;
        MinHeapify_lotti(ingrediente, minimo);
    }
    
}


min_heap* inserisci_lotto(ingredienti_magazzino* ingrediente, uint32_t scadenza, uint32_t qt){
    
    ingrediente->dimensione++;

    ingrediente->lotti = (min_heap*)realloc(ingrediente->lotti,( ingrediente->dimensione )*(sizeof(min_heap)));
    
    ingrediente->lotti[ingrediente->dimensione - 1].q = qt;
    ingrediente->lotti[ingrediente->dimensione - 1].scadenza = scadenza;

    int i = ingrediente->dimensione - 1;
    

    //slide barenghi
    while (i > 0 && ingrediente->lotti[parent(i)].scadenza > ingrediente->lotti[i].scadenza)
    {
        min_heap lotto_temp;
        lotto_temp = ingrediente->lotti[parent(i)];
        ingrediente->lotti[parent(i)] = ingrediente->lotti[i];
        ingrediente->lotti[i] = lotto_temp;
        i = parent(i);
    }
    
    return ingrediente->lotti;

}


min_heap* rimuovi_lotto(ingredienti_magazzino* ingrediente){
    
    //slide barenghi
    if (ingrediente->dimensione > 0 )
    {
        ingrediente->lotti[0] = ingrediente->lotti[ingrediente->dimensione - 1];
        ingrediente->dimensione--;
       
        MinHeapify_lotti(ingrediente,0);
        
    }
    return ingrediente->lotti;
    
}


min_heap* controlla_scadenze(ingredienti_magazzino** ingrediente, uint32_t time){
    
    //TROVATO IL BUG, accedevo anche se la dimensione era 0
    if ((*ingrediente)->dimensione != 0)
    {
            if ((*ingrediente)->lotti[0].scadenza <= time)
        {
            //printf("\nPRIMA %d", (*ingrediente)->q_tot);
            (*ingrediente)->q_tot = (*ingrediente)->q_tot - (*ingrediente)->lotti[0].q;
            (*ingrediente)->lotti = rimuovi_lotto((*ingrediente));
            //printf("  DOPO %d\n", (*ingrediente)->q_tot);
            return (*ingrediente)->lotti;
        }
        else
        {
            return NULL;
        }

    }

    else
    {
        return NULL;
    }
        
}


/*
//non la uso sprecherei tempo
int cerca_lotto(ingredienti_magazzino* ingrediente, uint32_t scadenza){


    for (int i = 0; i < ingrediente->dimensione; i++)
    {
        if (ingrediente->lotti[i].scadenza == scadenza)
        {
            return i;
        }
        
    }

    return -1;
    //se non trovo il lotto ritorno -1

}*/



void alloca_ingredienti(Ricetta* ricetta, int numero_ingredienti){

    ricetta->ingredienti_ricettario = (ingredienti_ricettario*)realloc(ricetta->ingredienti_ricettario, numero_ingredienti*sizeof(ingredienti_ricettario));
    ricetta->numero_ingredienti = numero_ingredienti;
    
    for (int i = 0; i < numero_ingredienti; i++)
    {   
        ricetta->ingredienti_ricettario[i].ingrediente = NULL;
        ricetta->ingredienti_ricettario[i].qt= 0;
    }
    
}


//inserisco ingredienti nella Hash-Ingredienti, inizializzo il puntatore al magazzino a NULL
ingredienti* inserisci_ingredienti_chained(Hash_table_ingredienti* tabella,char ingrediente_nome[MAXLEN],int key, int len_parola){
    //chiamo ricerca_ingredienti_chained per aggiungere se necessario gli ingredienti nell'hash_table ingredienti in modo univoco
    ingredienti* temp = ricerca_ingredienti_chained(tabella, ingrediente_nome,key);
    if ( temp != NULL)
    {
        return temp ;
    }
    else
    {
        ingredienti * new = (ingredienti*)malloc(sizeof(ingredienti)); 
        new->next = tabella[key].lista_ingredienti;
        new->magazzino = NULL;
        
        tabella[key].lista_ingredienti = new;
        
        new->ingrediente = (char*)malloc((len_parola + 1)*sizeof(char));

        strcpy(new->ingrediente,ingrediente_nome);
        return new;
    }
    
    
}


void inserisci_ingredienti_magazzino_chained(Hash_table_magazzino* magazzino,Hash_table_ingredienti* tabella_ingredienti, char ingrediente[MAXLEN],uint32_t scadenza, int qt, uint32_t tempo){
    //controllo se il lotto che inserisco è già scaduto
    if (scadenza <= tempo)
    {
        return;
    }
    
    //inserisco, se necessario, nel array_ingredienti, mi serve temp perchè da qui so il nome dell'ingrediente (per usare puntatore)
    ingredienti* temp = inserisci_ingredienti_chained(tabella_ingredienti,ingrediente, hash_function_ingredienti(ingrediente),strlen(ingrediente));
    int key = hash_function_magazzino(ingrediente);
    ingredienti_magazzino* temp_ingrediente = ricerca_ingredienti_magazzino_chained(magazzino,ingrediente, key);
    if (temp_ingrediente!= NULL)
    {
        //ingrediente già nel magazzino
        temp_ingrediente->lotti = inserisci_lotto(temp_ingrediente,scadenza,qt);
        
        temp_ingrediente->q_tot += qt;
        
    }
    else
    {
        //nuovo ingrediente
        ingredienti_magazzino * new = (ingredienti_magazzino*)malloc(sizeof(ingredienti_magazzino)); 
        new->next = magazzino[key].lista_ingredienti;
        
        new->dimensione = 0;
        new->ingrediente = temp;
        new->q_tot = qt;
        magazzino[key].lista_ingredienti = new;
        temp->magazzino = new;
        
        new->lotti = NULL; //se non lo inizializzo mi da errore, un po' inutile
        new->lotti = inserisci_lotto(new, scadenza,qt);
           
    }
    
}


void inserisci_ingredienti_ricettario(Ricetta* ricetta, Hash_table_ingredienti* ingredienti_tabella, int indice_ingrediente, char ingrediente_nome[MAXLEN], int qt, int len_parola){
    unsigned long int key_ingrediente = hash_function_ingredienti(ingrediente_nome);
    
    ricetta->ingredienti_ricettario[indice_ingrediente - 1].ingrediente = inserisci_ingredienti_chained(ingredienti_tabella,ingrediente_nome,key_ingrediente,len_parola);
    ricetta->ingredienti_ricettario[indice_ingrediente - 1].qt = qt;
    
}


Ricetta* inserisci_ricetta_chained(Hash_table_ricettario* tabella,char ricetta_nome[MAXLEN],int key, int len_parola){
    
    
    if (ricerca_ricetta_chained(tabella,ricetta_nome,key) != NULL)
    {
        
        return NULL;// leggendo NULL nel main->printf("ignorato\n")
    }
    else
    {
        
        Ricetta * new = (Ricetta*)malloc(sizeof(Ricetta)); 
        new->next = tabella[key].lista_ricette;
        new->prev = NULL;
        if (tabella[key].lista_ricette != NULL)
        {
            tabella[key].lista_ricette->prev = new;
        }
        
        new->count = 0;
        new->ingredienti_ricettario = NULL;
        tabella[key].lista_ricette = new;
        new->numero_ingredienti = 0;

        new->nome = (char*)malloc((len_parola + 1)*sizeof(char));
        strcpy(new->nome,ricetta_nome);

        return new;
    }
    
    
}



void dealloca_ricettario(Hash_table_ricettario* tabella){
    
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Ricetta* ricetta = tabella[i].lista_ricette;
        while (ricetta != NULL)
        {
            //printf("\nlibera!\n");
            Ricetta* temp = ricetta;
            ricetta = ricetta->next;
                       

            
            if (temp != NULL)
            {
                free(temp->nome);
                free(temp->ingredienti_ricettario);
                free(temp);
            }
              
        } 
    }
}


void dealloca_ingredienti(Hash_table_ingredienti* tabella){
    
    for (int i = 0; i < HASH_SIZE_INGREDIENTI; i++)
    {
        ingredienti* ingrediente = tabella[i].lista_ingredienti;
        while (ingrediente != NULL)
        {
            //printf("\nlibera!\n");
            ingredienti* temp = ingrediente;
            ingrediente = ingrediente->next;
            

            if (temp != NULL)
            {
                free(temp->ingrediente);
                free(temp);
            }
           
        } 
    }
}


void rimuovi_nodo(Ricetta* ricetta_ptr, Ricetta** testa){
    //printf("\nrimuovo nodo: %s", ricetta_ptr->nome);
    if (ricetta_ptr->prev != NULL)
    {
        //se non sono in testa, il nodo prima di me punta a chi punto io
        ricetta_ptr->prev->next = ricetta_ptr->next;
    }
    else
    {
        //se sono in testa, la nuova testa diventa il nodo a chi punto io
        *testa = ricetta_ptr->next;
    }
    if (ricetta_ptr->next != NULL)
    {
        //se punto a qualcuno, il nodo puntato punterà (<--) a chi mi puntava
        ricetta_ptr->next->prev = ricetta_ptr->prev; //lo "inizializza" anche a NULL teoricamente
    }

    free(ricetta_ptr->ingredienti_ricettario);
    free(ricetta_ptr->nome);
    free(ricetta_ptr);
    
}


void dealloca_magazzino(Hash_table_magazzino* magazzino){
    for (int i = 0; i < HASH_SIZE_MAGAZZINO; i++)
    {
        ingredienti_magazzino* ingrediente = magazzino[i].lista_ingredienti;
        while (ingrediente != NULL)
        {
            ingredienti_magazzino* temp = ingrediente;
            ingrediente = ingrediente->next;
                       

            if (temp != NULL)
            {   
                if (temp->lotti != NULL)
                {
                    free(temp->lotti);
                }
                
                free(temp);
            }  
        } 
    }
}
    


void MinHeapify_spedizioni(coda_spedizioni* coda, uint32_t i){
    uint32_t l = left(i);
    uint32_t r = right(i);  
    uint32_t minimo = i;
    
    if (l < coda->dimensione && (coda->coda[l].tempo_ordine < coda->coda[i].tempo_ordine))
    {
        minimo = l;
    }
    else
    {
        minimo = i;
    }
    if (r < coda->dimensione && (coda->coda[r].tempo_ordine < coda->coda[minimo].tempo_ordine))
    {
        minimo = r;
    }
    if (minimo != i)
    {
        min_heap_spedizione spedizione_temp;
        spedizione_temp = coda->coda[minimo];
        coda->coda[minimo] = coda->coda[i];
        coda->coda[i] = spedizione_temp;
        MinHeapify_spedizioni(coda, minimo);
    }
    
}


void inserisci_spedizione(coda_spedizioni** coda, Ordine* ordine){
    (*coda)->dimensione++;
    
    (*coda)->coda = (min_heap_spedizione*)realloc((*coda)->coda,((*coda)->dimensione)*(sizeof(min_heap_spedizione)));
    

    (*coda)->coda[(*coda)->dimensione - 1].numero_ordini = ordine->numero_ordini;
    (*coda)->coda[(*coda)->dimensione - 1].ordine = ordine->ordine;
    (*coda)->coda[(*coda)->dimensione - 1].peso = 0;

    for (int j = 0; j < ordine->ordine->numero_ingredienti; j++)
    {
        (*coda)->coda[(*coda)->dimensione - 1].peso += ordine->ordine->ingredienti_ricettario[j].qt * ordine->numero_ordini;
    }
    
    (*coda)->coda[(*coda)->dimensione - 1].tempo_ordine = ordine->tempo_ordine;
    int i = 0;
    i = (*coda)->dimensione - 1;
    //slide barenghi heap/grafi
    while (i > 0 && (*coda)->coda[parent(i)].tempo_ordine > (*coda)->coda[i].tempo_ordine)
    {

        min_heap_spedizione spedizione_temp;
        spedizione_temp = (*coda)->coda[parent(i)];
        (*coda)->coda[parent(i)] = (*coda)->coda[i];
        (*coda)->coda[i] = spedizione_temp;
        i = parent(i);
        
    }
    

}


void preleva_ingredienti(Hash_table_magazzino* magazzino, Ricetta* ricetta, int numero_ordini){

    int q_temp = 0;
    for (int i = 0; i < ricetta->numero_ingredienti; i++)
    {
        q_temp = ricetta->ingredienti_ricettario[i].qt * numero_ordini;
        ingredienti_magazzino* temp = ricerca_ingredienti_magazzino_chained(magazzino, ricetta->ingredienti_ricettario[i].ingrediente->ingrediente, hash_function_magazzino(ricetta->ingredienti_ricettario[i].ingrediente->ingrediente));
        temp->q_tot = temp->q_tot - q_temp;
                
        while (q_temp != 0)
        {

            if (q_temp >= temp->lotti[0].q )
            {
                q_temp = q_temp - temp->lotti[0].q;
                rimuovi_lotto(temp);
            }
            else
            {
                temp->lotti[0].q = temp->lotti[0].q - q_temp;
                q_temp = 0;
            }
            
        }
        
        
    }
    
}



int controlla_disp(Ricetta* ricetta, Hash_table_magazzino* magazzino, int numero_ordini, uint32_t time){
    //printf("\nPer %s ho bisogno di:", ricetta->nome);
    
    for (int i = 0; i < ricetta->numero_ingredienti; i++)
    {
        ingredienti_magazzino* temp = ricerca_ingredienti_magazzino_chained(magazzino, ricetta->ingredienti_ricettario[i].ingrediente->ingrediente, hash_function_magazzino(ricetta->ingredienti_ricettario[i].ingrediente->ingrediente));
        //trovo l'ingrediente che mi serve e lo passo in controlla scandenze che elimina eventuali lotti scaduti (dalla radice)
        
        if (temp == NULL)
        {
            
            return -1;
        }
        
        else
        {
            min_heap* lotto_temp = controlla_scadenze(&temp,time);
            
            while (lotto_temp != NULL && temp->dimensione > 0)
                {
                    lotto_temp = controlla_scadenze(&temp,time);
                    // se non arriva mai a null mi blocco qui mi sa  
                }
          
            if (temp->q_tot < ricetta->ingredienti_ricettario[i].qt * numero_ordini)
            {
                return -1;   
            }
            
        }
        
    }
    return 0;

}


int controlla_disp_2(Ordine* ordine, Hash_table_magazzino* magazzino, uint32_t numero_ordini, uint32_t time){

    if (ordine->ordine == NULL ||ordine->index > ordine->ordine->numero_ingredienti|| ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente == NULL|| ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente->magazzino == NULL || ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente->magazzino->q_tot < ordine->ordine->ingredienti_ricettario[ordine->index].qt * numero_ordini)
    {
        //printf("\n %d %d\n", ordine->index, ordine->ordine->numero_ingredienti);
        return -1;
    }
    

    for (int i = 0; i < ordine->ordine->numero_ingredienti; i++)
    
    {
        
        if (ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino == NULL)
        {
            //printf("\nNon posso produrre %d %s %d\n", ordine->tempo_ordine, ordine->ordine->nome, ordine->numero_ordini);
            ordine->index = i;
            return -1;      
        }
        
        else
        {
            
            min_heap* lotto_temp = controlla_scadenze(&ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino,time);
            
            while (lotto_temp != NULL && ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino->dimensione >0)
                {
                    lotto_temp = controlla_scadenze(&ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino,time);
                    
                }
           
            if (ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino->q_tot < ordine->ordine->ingredienti_ricettario[i].qt * numero_ordini)
                {
                    //printf("\nNon posso produrre %d %s %d\n", ordine->tempo_ordine, ordine->ordine->nome, ordine->numero_ordini);
                    ordine->index = i;
                    return -1;   
                }
                //printf("Ho %d fronte di %d\n",ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino->q_tot , ordine->ordine->ingredienti_ricettario[i].qt * numero_ordini);
            
        }
        
        
    }
    //printf("\nPosso produrre %d %s %d\n", ordine->tempo_ordine, ordine->ordine->nome, ordine->numero_ordini);
    return 1;

}


//inizializza index
void inserisci_ordine_attesa(coda_ordini** coda, Ricetta* ricetta, Hash_table_magazzino* magazzino,uint32_t tempo, uint32_t numero_ordini){

    Ordine* new = (Ordine*)malloc(sizeof(Ordine));

    new->index = 0;    
    new->numero_ordini = numero_ordini;
    new->ordine = ricetta;
    new->tempo_ordine = tempo;
    new->next = NULL;
    new->prev = NULL;
    if ((*coda)->testa == NULL || (*coda)->coda == NULL)
    {
        //printf("\nsono qui!");
        (*coda)->coda= new;
        (*coda)->testa = new;
    }
    else
    {
        //printf("\ninvece sono qui!");
        new->prev = (*coda)->coda;
        (*coda)->coda->next = new;
        (*coda)->coda = new;
    }
   
    
}


void evadi_ordini(coda_ordini** coda, Ordine** ordine_da_evadere, coda_spedizioni** coda_sp){

   Ordine* temp = NULL;
   temp = (*ordine_da_evadere);
   if (temp == (*coda)->testa)
   {
    
        if ((*coda)->testa->next != NULL)
        {
            //printf("\nc'è successore\n");
            //printf("\n%s : %d",(*coda)->testa->ordine->nome, (*coda)->testa->tempo_ordine );
            (*coda)->testa->next->prev = NULL;
            (*coda)->testa = (*coda)->testa->next;
            //printf("\n%s : %d",(*coda)->testa->ordine->nome, (*coda)->testa->tempo_ordine );
        }
        else
        {
            //printf("\nnon c'è successore\n");
            (*coda)->testa = NULL;
            (*coda)->coda = NULL;
        }
           
   }

   else
   {
        temp->prev->next = temp->next;
        if (temp != (*coda)->coda)
        {
            temp->next->prev = temp->prev; 
        }
        else
        {
            (*coda)->coda = temp->prev;
        }
        
   }

   inserisci_spedizione(coda_sp,temp);
   free(temp);
   
}



void inserisci_in_stampa(coda_di_stampa** coda, min_heap_spedizione spedizione){
    (*coda)->dimensione++;
    (*coda)->pacchi = (max_heap*)realloc((*coda)->pacchi, (*coda)->dimensione * sizeof(max_heap));
    (*coda)->pacchi[(*coda)->dimensione - 1].nome_ordine = spedizione.ordine;
    (*coda)->pacchi[(*coda)->dimensione - 1].tempo_ordine = spedizione.tempo_ordine;
    (*coda)->pacchi[(*coda)->dimensione - 1].peso = spedizione.peso;
    (*coda)->pacchi[(*coda)->dimensione - 1].numero_ordini = spedizione.numero_ordini;
}


void MaxHeapify_stampa(coda_di_stampa** coda, uint32_t i){
    
    uint32_t l = left(i);
    uint32_t r = right(i);  
    uint32_t massimo = i;

    if (l < (*coda)->dimensione && ((*coda)->pacchi[l].peso > (*coda)->pacchi[massimo].peso || ((*coda)->pacchi[l].peso == (*coda)->pacchi[massimo].peso &&(*coda)->pacchi[l].tempo_ordine < (*coda)->pacchi[massimo].tempo_ordine)))
    {
        massimo = l;
    }
    else
    {
        massimo = i;
    }
    if (r < (*coda)->dimensione && ((*coda)->pacchi[r].peso > (*coda)->pacchi[massimo].peso || ((*coda)->pacchi[r].peso == (*coda)->pacchi[massimo].peso &&(*coda)->pacchi[r].tempo_ordine < (*coda)->pacchi[massimo].tempo_ordine)))
    {
        massimo = r;
    }
    if (massimo != i)
    {
        max_heap stampa_temp;
        
        stampa_temp = (*coda)->pacchi[massimo];
        (*coda)->pacchi[massimo] = (*coda)->pacchi[i];
        (*coda)->pacchi[i] = stampa_temp;
        MaxHeapify_stampa(coda, massimo);
    }
    
}


void buildMaxHeap(coda_di_stampa** coda){
    int temp = (*coda)->dimensione ;
    
    for (int i = ((temp) / 2) ; i >= 0; i--)
    {
        MaxHeapify_stampa(coda,i);
    }

}




int main(){
    
    int camion = 0;
    int dimensione_camion = 0;
    uint32_t time = 0; 
    int spedizioni = 0;

    
    int c;
    uint32_t buff_size = BUFF_INIZIALE;
    char *buffer = (char*)malloc(buff_size *sizeof(char));


    int len_parole = 0;
    char comando[MAXLEN];
    comando[0] = '\0';
    int numero_ingredienti = 0;
    char ricetta[MAXLEN];
    ricetta[0] = '\0';
    char ingrediente[MAXLEN];
    ingrediente[0] = '\0';
    uint32_t qt = 0;
    uint32_t scadenza = 0;  
    uint32_t numero_ordini = 0;  
    

    Hash_table_ricettario* tabella = crea_tabella();
    Hash_table_ingredienti* tabella_ingredienti = crea_tabella_ingredienti();
    Hash_table_magazzino* magazzino = crea_tabella_magazzino();
    coda_ordini* Coda_ordini = (coda_ordini*)malloc(sizeof(coda_ordini));
    Coda_ordini->coda = NULL;
    Coda_ordini->testa = NULL;
    coda_spedizioni* Coda_spedizioni = (coda_spedizioni*)malloc(sizeof(coda_spedizioni));
    Coda_spedizioni->coda = NULL;
    Coda_spedizioni->dimensione = 0;
    coda_di_stampa* Coda_di_stampa = (coda_di_stampa*)malloc(sizeof(coda_di_stampa));
    Coda_di_stampa->pacchi = NULL;
    Coda_di_stampa->dimensione = 0;
    

    while ((c = getc(stdin)) != ' ')
    { 
        buffer[len_parole] = c;
        len_parole++;
    }


    buffer[len_parole] = '\0';
    camion = atoi(buffer);
    memset(buffer, '\0', strlen(buffer));
    len_parole = 0;


    while ((c = getc(stdin)) != '\n')
    {
        buffer[len_parole] = c;
        len_parole++;
    }


    buffer[len_parole] = '\0';
    dimensione_camion = atoi(buffer);
    memset(buffer, '\0', strlen(buffer));
    len_parole = 0;

    
    uint32_t flag = 0;    
    Ricetta* ricetta_ptr = NULL;
    uint32_t indice = 0;
    
    bool ingredienti_f = false;
    unsigned long int ascii = 0;

    
    while((c = getc(stdin)) != EOF){
         
        flag = 0;
        //leggo comando
        buffer[len_parole] = c;
        len_parole++;

        while ((c = getc(stdin)) != ' ')
            {
                buffer[len_parole] = c;
                len_parole++;
            }
        
        buffer[len_parole] = '\0';
        strcpy(comando, buffer);
        memset(buffer, '\0', strlen(buffer));


        if (flag == 0 && comando[0] == 'a')
        {
            flag = 1;
        }

        else if (flag == 0 && comando[0] == 'r' && comando[7] == '_')
        {
            flag = 2;
        }

        else if (flag == 0 && comando[0] == 'r')
        {
            flag = 3;
        }

        else if (flag == 0 && comando[0] == 'o')
        {
            flag = 4;
        }
        
        //aggiungi_ricetta
        if (flag == 1)
        {
            numero_ingredienti = 0;
            len_parole = 0;
            while ((c = getc(stdin)) != ' ')
            {
                buffer[len_parole] = c;
                len_parole++;
            }
            buffer[len_parole] = '\0';
            strcpy(ricetta, buffer);

            memset(buffer, '\0', buff_size);
            ascii = hash_function(ricetta);
            //INSERISCO RICETTA
            ricetta_ptr = inserisci_ricetta_chained(tabella,ricetta,ascii,strlen(ricetta));
            

            if (ricetta_ptr != NULL)
                {
                    fputs("aggiunta\n", stdout);
                    ingredienti_f = true; 
                    
                }
            else
                {   
                    fputs("ignorato\n", stdout); 
                }
            
            
            if (ingredienti_f)
            {

                len_parole = 0;
                while ((c = getc(stdin)) != '\n')
                {

                    //salvando nel buffer conto anche gli ingredienti
                    if (c == ' ')
                    {
                        //ogni 2 spazi incremento di 1 il numero degli ingredienti
                        indice++;
                        if (indice == 2)
                        {
                            numero_ingredienti++;
                            indice = 0;
                        }
                    }
                    
                    buffer[len_parole] = c;
                    len_parole++;
                }

                numero_ingredienti++; 

                alloca_ingredienti(ricetta_ptr, numero_ingredienti);
                

                numero_ingredienti = 0;
                char c_t = 'a';
                char temp[MAXLEN];
                int z;
                z = 0;
                indice = 0;
                
                //memset(temp, '\0',MAXLEN);
                for (int i = 0; i <= strlen(buffer); i++)
                {
                    c_t = parser(buffer, i);

                    temp[z] = c_t;
                    z++;
                    //se ho letto una parola posso procedere con gli inserimenti 
                    if ((c_t == ' ') || (c_t == '\0'))
                    {
                        temp[z - 1] = '\0';
 
                        if (indice == 1)
                        {

                            qt = atoi(temp);
                            indice = 0;
                            inserisci_ingredienti_ricettario(ricetta_ptr,tabella_ingredienti,numero_ingredienti,ingrediente,qt, strlen(ingrediente));
                            
                        }
                        else
                        {
                            numero_ingredienti++;
                            strcpy(ingrediente, temp);
                            indice++;
                            
                        }

                        memset(temp, '\0',MAXLEN);
                        z = 0;
                        
                            
                    }

                }
                
            }


            else
            {
                //finisco il rigo
                while ((c = getc(stdin)) != '\n')
                {
                    ;
                }
                
            }

            //resetto buffer e flag
            memset(buffer, '\0', buff_size);
            ingredienti_f = false;
            len_parole = 0;
            
        
        }
        
        //rimuovi_ricetta
        if (flag == 2)
            {
                
                len_parole = 0;
                while ((c = getc(stdin)) != '\n')
                {
                    buffer[len_parole]  = c;
                    len_parole++;
                }
                buffer[len_parole] = '\0';
                strcpy(ricetta, buffer);
                memset(buffer, '\0', buff_size);
                len_parole = 0;
                
                ascii = hash_function(ricetta);

                ricetta_ptr = ricerca_ricetta_chained(tabella,ricetta,ascii);
                if (ricetta_ptr == NULL)
                {
                    fputs("non presente\n", stdout);
                }
                else if (ricetta_ptr->count != 0)
                {
                    fputs("ordini in sospeso\n", stdout);
                }
                else
                {
                    
                    rimuovi_nodo(ricetta_ptr, &tabella[ascii].lista_ricette);
                    
                    fputs("rimossa\n", stdout);
                }    
                
            }


        //rifornimento
        if (flag == 3)
            {            
                       
                indice = 0;
                len_parole = 0;
                
                while ((c = getc(stdin)) != '\n')
                {
                    buffer[len_parole] = c;
                    len_parole++;
                    
                    if (c == ' ' || c == '\n')
                    {
                        
                        indice++;
                        
                        if (indice == 1)
                        {
                            buffer[len_parole - 1] = '\0';
                            strcpy(ingrediente, buffer);
                            memset(buffer,'\0' ,buff_size);
                            
                        }
                        else if(indice == 2){
                            buffer[len_parole - 1] = '\0';
                            qt = atoi(buffer);
                            memset(buffer,'\0' ,buff_size);
                            
                        }
                        else if(indice == 3){
                            
                            buffer[len_parole - 1] = '\0';
                            scadenza = atoi(buffer);
                            memset(buffer,'\0' ,buff_size);
                            
                            indice = 0;
                            
                            inserisci_ingredienti_magazzino_chained(magazzino,tabella_ingredienti,ingrediente,scadenza,qt, time);
                            
                        }
                        len_parole = 0;

                    }
                    

                }
                //l'ultimo inserimento lo devo fare fuori dal while
                buffer[len_parole] = '\0';
                scadenza = atoi(buffer);
                memset(buffer,'\0' ,buff_size);
                
                indice = 0;
                len_parole = 0;
                
                inserisci_ingredienti_magazzino_chained(magazzino,tabella_ingredienti,ingrediente,scadenza,qt, time);
                indice = 0;



                Ordine* ordine_temp1 = Coda_ordini->testa;
                Ordine* ordine_temp2 = NULL;
                
                while (ordine_temp1 != NULL)
                {
                    ordine_temp2 = ordine_temp1->next;
                    
                    
                    if ((controlla_disp_2(ordine_temp1, magazzino, ordine_temp1->numero_ordini, time)) == 1 )
                    {
                        
                        preleva_ingredienti(magazzino, ordine_temp1->ordine, ordine_temp1->numero_ordini);
                        evadi_ordini(&Coda_ordini, &ordine_temp1, &Coda_spedizioni);
                                               
                    }
                     
                    ordine_temp1 = ordine_temp2;
                                
                }
                
                fputs("rifornito\n", stdout);
            }
        
        
        //ordine
        if (flag == 4)
            {
                
                indice = 0;
                len_parole = 0;
                while ((c = getc(stdin)) != '\n')
                {
                    buffer[len_parole] = c;
                    len_parole++;
                    if (c == ' ')
                    {
                        buffer[len_parole - 1] = '\0';
                        strcpy(ricetta, buffer);
                        memset(buffer,'\0' ,buff_size);
                        len_parole = 0;
                    }
                    
                }
                buffer[len_parole ] = '\0';
                numero_ordini = atoi(buffer);
                memset(buffer,'\0' ,buff_size);
                len_parole = 0;
                

                //Controllo se la ricetta esiste
                ricetta_ptr = ricerca_ricetta_chained(tabella, ricetta, hash_function(ricetta));
                if (ricetta_ptr == NULL)
                {
                    fputs("rifiutato\n", stdout);
                }
                else
                {
                    fputs("accettato\n", stdout);
                    
                    ricetta_ptr->count++;
                    

                    if ( controlla_disp(ricetta_ptr,magazzino,numero_ordini, time) == -1)
                    {
                        
                        inserisci_ordine_attesa(&Coda_ordini, ricetta_ptr, magazzino,time,numero_ordini);

                    }
                    else
                    {
                        
                        Ordine ordine_temp;
                        ordine_temp.index = 0;
                        ordine_temp.numero_ordini = numero_ordini;
                        ordine_temp.ordine = ricetta_ptr;
                        ordine_temp.tempo_ordine = time;
                        ordine_temp.next = NULL;
                        ordine_temp.prev = NULL;
                        
                        inserisci_spedizione(&Coda_spedizioni, &ordine_temp);
                        
                        preleva_ingredienti(magazzino, ricetta_ptr, numero_ordini);
                        
                    }
                    

                }
            

            }
        
    
        len_parole = 0;
        indice = 0;
        time++;
        spedizioni++;
        int dimensione_camion_temp = 0;
        if (spedizioni == camion)
        {
 
            dimensione_camion_temp = dimensione_camion;
            spedizioni = 0;
            if (Coda_spedizioni->dimensione != 0)
            {
                
                dimensione_camion_temp = dimensione_camion_temp - Coda_spedizioni->coda[0].peso;

                while ( Coda_spedizioni->dimensione != 0 && dimensione_camion_temp > 0)
                {
                    
                    if (dimensione_camion_temp - Coda_spedizioni->coda[0].peso >0)
                    {
                        //printf("\nC %d\n",dimensione_camion_temp);
                        //printf("\n->%s %d %d\n",Coda_spedizioni->coda[0].ordine->nome, Coda_spedizioni->coda[0].peso, Coda_spedizioni->coda[0].tempo_ordine);
                        inserisci_in_stampa(&Coda_di_stampa,Coda_spedizioni->coda[0]);
                        Coda_spedizioni->coda[0].ordine->count--;
                        Coda_spedizioni->coda[0] = Coda_spedizioni->coda[Coda_spedizioni->dimensione - 1];
                        Coda_spedizioni->dimensione--;
                        MinHeapify_spedizioni(Coda_spedizioni, 0);

                    }

                    dimensione_camion_temp = dimensione_camion_temp - Coda_spedizioni->coda[0].peso;
                    
                }

                buildMaxHeap(&Coda_di_stampa);

                while (Coda_di_stampa->dimensione != 0)
                {
                    printf("%d %s %d\n", Coda_di_stampa->pacchi[0].tempo_ordine, Coda_di_stampa->pacchi[0].nome_ordine->nome, Coda_di_stampa->pacchi[0].numero_ordini);
                    Coda_di_stampa->pacchi[0] = Coda_di_stampa->pacchi[Coda_di_stampa->dimensione - 1];
                    Coda_di_stampa->dimensione--;
                    MaxHeapify_stampa(&Coda_di_stampa,0);
                }
            }
            else
            {
                printf("camioncino vuoto\n");
            }
            
        }

    }           
        
   
    //DEALLOCAZIONE

    free(Coda_spedizioni->coda);
    free(Coda_spedizioni);
    

    free(buffer);
    

    dealloca_ricettario(tabella);
    free(tabella);
    

    dealloca_ingredienti(tabella_ingredienti);
    free(tabella_ingredienti);
    
    
    dealloca_magazzino(magazzino);
    free(magazzino);
    

    Ordine* temp1_e;
    Ordine* temp2_e;
    temp1_e = Coda_ordini->testa;
    while (temp1_e != NULL)
    {
        temp2_e = temp1_e;
        temp1_e = temp1_e->next;
        free(temp2_e);
    }


    free(Coda_ordini);


    free(Coda_di_stampa->pacchi);
    free(Coda_di_stampa);
    
    return 0;
}

