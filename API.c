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

// INGREDIENTS HASH TABLE STRUCTURES

/**
 * Structure representing an ingredient
 * Contains ingredient name and pointer to warehouse entry
 */
typedef struct ingredienti
{
    char* ingrediente;                              // Ingredient name
    struct ingredienti_magazzino* magazzino;        // Pointer to warehouse entry
    struct ingredienti* next;                       // Next ingredient in chain (for collision handling)
}ingredienti;

/**
 * Hash table for ingredients lookup
 */
typedef struct 
{
    ingredienti* lista_ingredienti;                 // Head of ingredients list
}Hash_table_ingredienti;

// RECIPE BOOK HASH TABLE STRUCTURES

/**
 * Structure representing ingredients used in recipes with quantities
 */
typedef struct 
{
    ingredienti* ingrediente;                       // Pointer to ingredient
    uint32_t qt;                                   // Quantity needed
}ingredienti_ricettario;

/**
 * Structure representing a complete recipe
 */
typedef struct Ricetta
{
    uint32_t count;                                // Number of pending orders for this recipe
    uint32_t numero_ingredienti;                   // Number of ingredients in recipe
    char* nome;                                    // Recipe name
    struct Ricetta* next;                          // Next recipe in hash chain
    struct Ricetta* prev;                          // Previous recipe in hash chain
    ingredienti_ricettario* ingredienti_ricettario; // Array of ingredients with quantities
}Ricetta;

/**
 * Hash table for recipe lookup
 */
typedef struct 
{
    Ricetta* lista_ricette;                        // Head of recipes list
}Hash_table_ricettario;

// WAREHOUSE HASH TABLE STRUCTURES

/**
 * Min-heap node for ingredient batches (sorted by expiration date)
 */
typedef struct 
{
    uint32_t scadenza;                             // Expiration date
    int q;                                         // Quantity in this batch
}min_heap;

/**
 * Warehouse entry for each ingredient type
 */
typedef struct ingredienti_magazzino
{
    uint32_t dimensione;                           // Number of batches (heap size)
    ingredienti* ingrediente;                      // Pointer to ingredient info
    int q_tot;                                     // Total quantity available
    min_heap* lotti;                               // Min-heap of batches by expiration
    struct ingredienti_magazzino* next;            // Next ingredient in hash chain
}ingredienti_magazzino;

/**
 * Hash table for warehouse ingredient lookup
 */
typedef struct 
{
    ingredienti_magazzino* lista_ingredienti;      // Head of warehouse ingredients list
}Hash_table_magazzino;

// ORDER QUEUE STRUCTURES

/**
 * Structure representing a customer order
 */
typedef struct Ordine
{
    Ricetta* ordine;                               // Recipe ordered
    int tempo_ordine;                              // Time when order was placed
    uint32_t numero_ordini;                        // Quantity ordered
    uint32_t index;                                // Index for tracking missing ingredients
    struct Ordine* next;                           // Next order in queue
    struct Ordine* prev;                           // Previous order in queue
}Ordine;

/**
 * Queue for pending orders (FIFO)
 */
typedef struct 
{
    Ordine* testa;                                 // Head of queue
    Ordine* coda;                                  // Tail of queue
}coda_ordini;

// SHIPPING QUEUE STRUCTURES

/**
 * Min-heap node for shipping queue (sorted by order time)
 */
typedef struct 
{
    int tempo_ordine;                              // Time when order was placed
    Ricetta* ordine;                               // Recipe information
    uint32_t numero_ordini;                        // Quantity ordered
    uint32_t peso;                                 // Total weight of order
}min_heap_spedizione;

/**
 * Shipping queue (min-heap by order time)
 */
typedef struct 
{
    min_heap_spedizione* coda;                     // Heap array
    uint32_t dimensione;                           // Heap size
}coda_spedizioni;

// PRINTING QUEUE STRUCTURES

/**
 * Max-heap node for printing orders (sorted by weight, then by time)
 */
typedef struct 
{
    uint32_t numero_ordini;                        // Quantity ordered
    int tempo_ordine;                              // Time when order was placed
    uint32_t peso;                                 // Total weight
    Ricetta* nome_ordine;                          // Recipe information
}max_heap;

/**
 * Print queue (max-heap by weight, then chronological)
 */
typedef struct 
{
    max_heap* pacchi;                              // Heap array
    uint32_t dimensione;                           // Heap size
}coda_di_stampa;

// HASH TABLE CREATION FUNCTIONS

/**
 * Creates and initializes recipe hash table
 * @return Pointer to initialized hash table
 */
Hash_table_ricettario* crea_tabella(){
    Hash_table_ricettario* tabella = (Hash_table_ricettario*)malloc(HASH_SIZE * sizeof(Hash_table_ricettario));
    
    for (int i = 0; i < HASH_SIZE; i++)
    {
        tabella[i].lista_ricette = NULL;
    }
    return tabella;    
}

/**
 * Creates and initializes ingredients hash table
 * @return Pointer to initialized hash table
 */
Hash_table_ingredienti* crea_tabella_ingredienti(){
    Hash_table_ingredienti* tabella_ingredienti = (Hash_table_ingredienti*)malloc(HASH_SIZE_INGREDIENTI * sizeof(Hash_table_ingredienti));
    
    for (int i = 0; i < HASH_SIZE_INGREDIENTI; i++)
    {
        tabella_ingredienti[i].lista_ingredienti = NULL;
    }
    return tabella_ingredienti;    
}

/**
 * Creates and initializes warehouse hash table
 * @return Pointer to initialized hash table
 */
Hash_table_magazzino* crea_tabella_magazzino(){
    Hash_table_magazzino* tabella = (Hash_table_magazzino*)malloc(HASH_SIZE_MAGAZZINO * sizeof(Hash_table_magazzino));
    
    for (int i = 0; i < HASH_SIZE_MAGAZZINO; i++)
    {
        tabella[i].lista_ingredienti = NULL;
    }
    return tabella;    
}

/**
 * Simple character parser helper function
 * @param parola String to parse
 * @param i Index of character to return
 * @return Character at position i
 */
char parser(char parola[],int i){
    return parola[i];
}

// HASH FUNCTIONS

/**
 * Hash function for recipe names (Cormen page 260 inspired)
 * @param stringa String to hash
 * @return Hash value for recipe table
 */
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

/**
 * Hash function for warehouse ingredient names
 * @param stringa String to hash
 * @return Hash value for warehouse table
 */
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

/**
 * Hash function for ingredient names
 * @param stringa String to hash
 * @return Hash value for ingredients table
 */
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

// SEARCH FUNCTIONS

/**
 * Searches for a recipe in the hash table using chaining
 * @param tabella Recipe hash table
 * @param ricetta_nome Recipe name to search
 * @param key Hash key for the recipe
 * @return Pointer to recipe if found, NULL otherwise
 */
Ricetta* ricerca_ricetta_chained(Hash_table_ricettario* tabella,char ricetta_nome[MAXLEN],int key){
    
    if (tabella[key].lista_ricette == NULL)
    {
        return NULL;
    }
    else
    {
        Ricetta* ricetta = tabella[key].lista_ricette;
        
        while (ricetta != NULL)
        {
            // Optimization: check second character first for faster comparison
            if (ricetta->nome[1] == ricetta_nome[1])
            {
                if (strcmp(ricetta->nome, ricetta_nome) == 0)
                {
                    return ricetta;
                }
            }
            
            ricetta = ricetta->next;            
        }

        return NULL;
    }
}

/**
 * Searches for an ingredient in the hash table using chaining
 * @param tabella Ingredients hash table
 * @param ingrediente_nome Ingredient name to search
 * @param key Hash key for the ingredient
 * @return Pointer to ingredient if found, NULL otherwise
 */
ingredienti* ricerca_ingredienti_chained(Hash_table_ingredienti* tabella,char ingrediente_nome[MAXLEN],int key){
    
    if (tabella[key].lista_ingredienti == NULL)
    {
        return NULL;
    }
    else
    { 
        ingredienti* ingrediente = tabella[key].lista_ingredienti;

        while (ingrediente != NULL)
        {
            // Optimization: check third character first for faster comparison
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

/**
 * Searches for an ingredient in the warehouse hash table
 * @param tabella Warehouse hash table
 * @param ingrediente_nome Ingredient name to search
 * @param key Hash key for the ingredient
 * @return Pointer to warehouse ingredient if found, NULL otherwise
 */
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
            // Optimization: check string length first
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

// HEAP UTILITY FUNCTIONS

/**
 * Returns parent index in heap
 */
int parent(int i){
    return (i - 1)/2;
}

/**
 * Returns left child index in heap
 */
int left(int i){
    return 2*i + 1;
}

/**
 * Returns right child index in heap
 */
int right(int i){
    return 2*i + 2;
}

// MIN-HEAP OPERATIONS FOR INGREDIENT BATCHES

/**
 * Maintains min-heap property for ingredient batches (by expiration date)
 * @param ingrediente Warehouse ingredient with batch heap
 * @param i Index to heapify from
 */
void MinHeapify_lotti(ingredienti_magazzino* ingrediente, uint32_t i){
    uint32_t l = left(i);
    uint32_t r = right(i);  
    uint32_t minimo = i;

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

/**
 * Inserts a new batch into ingredient's min-heap
 * @param ingrediente Warehouse ingredient
 * @param scadenza Expiration date of batch
 * @param qt Quantity of batch
 * @return Updated heap array
 */
min_heap* inserisci_lotto(ingredienti_magazzino* ingrediente, uint32_t scadenza, uint32_t qt){
    
    ingrediente->dimensione++;

    ingrediente->lotti = (min_heap*)realloc(ingrediente->lotti,( ingrediente->dimensione )*(sizeof(min_heap)));
    
    ingrediente->lotti[ingrediente->dimensione - 1].q = qt;
    ingrediente->lotti[ingrediente->dimensione - 1].scadenza = scadenza;

    int i = ingrediente->dimensione - 1;
    
    // Bubble up to maintain heap property (Barenghi slides)
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

/**
 * Removes the minimum (earliest expiring) batch from heap
 * @param ingrediente Warehouse ingredient
 * @return Updated heap array
 */
min_heap* rimuovi_lotto(ingredienti_magazzino* ingrediente){
    
    if (ingrediente->dimensione > 0 )
    {
        ingrediente->lotti[0] = ingrediente->lotti[ingrediente->dimensione - 1];
        ingrediente->dimensione--;
       
        MinHeapify_lotti(ingrediente,0);
    }
    return ingrediente->lotti;
}

/**
 * Checks and removes expired batches from ingredient
 * @param ingrediente Pointer to warehouse ingredient
 * @param time Current time
 * @return Updated heap if batch was removed, NULL otherwise
 */
min_heap* controlla_scadenze(ingredienti_magazzino** ingrediente, uint32_t time){
    
    if ((*ingrediente)->dimensione != 0)
    {
        if ((*ingrediente)->lotti[0].scadenza <= time)
        {
            (*ingrediente)->q_tot = (*ingrediente)->q_tot - (*ingrediente)->lotti[0].q;
            (*ingrediente)->lotti = rimuovi_lotto((*ingrediente));
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

// RECIPE MANAGEMENT FUNCTIONS

/**
 * Allocates memory for recipe ingredients array
 * @param ricetta Recipe to allocate for
 * @param numero_ingredienti Number of ingredients needed
 */
void alloca_ingredienti(Ricetta* ricetta, int numero_ingredienti){

    ricetta->ingredienti_ricettario = (ingredienti_ricettario*)realloc(ricetta->ingredienti_ricettario, numero_ingredienti*sizeof(ingredienti_ricettario));
    ricetta->numero_ingredienti = numero_ingredienti;
    
    for (int i = 0; i < numero_ingredienti; i++)
    {   
        ricetta->ingredienti_ricettario[i].ingrediente = NULL;
        ricetta->ingredienti_ricettario[i].qt= 0;
    }
}

/**
 * Inserts ingredient into ingredients hash table (ensures uniqueness)
 * @param tabella Ingredients hash table
 * @param ingrediente_nome Ingredient name
 * @param key Hash key
 * @param len_parola Length of ingredient name
 * @return Pointer to ingredient (new or existing)
 */
ingredienti* inserisci_ingredienti_chained(Hash_table_ingredienti* tabella,char ingrediente_nome[MAXLEN],int key, int len_parola){
    
    ingredienti* temp = ricerca_ingredienti_chained(tabella, ingrediente_nome,key);
    if ( temp != NULL)
    {
        return temp;
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

/**
 * Inserts or updates ingredient in warehouse with new batch
 * @param magazzino Warehouse hash table
 * @param tabella_ingredienti Ingredients hash table
 * @param ingrediente Ingredient name
 * @param scadenza Expiration date
 * @param qt Quantity
 * @param tempo Current time
 */
void inserisci_ingredienti_magazzino_chained(Hash_table_magazzino* magazzino,Hash_table_ingredienti* tabella_ingredienti, char ingrediente[MAXLEN],uint32_t scadenza, int qt, uint32_t tempo){
    
    // Don't add already expired batches
    if (scadenza <= tempo)
    {
        return;
    }
    
    // Ensure ingredient exists in ingredients table
    ingredienti* temp = inserisci_ingredienti_chained(tabella_ingredienti,ingrediente, hash_function_ingredienti(ingrediente),strlen(ingrediente));
    int key = hash_function_magazzino(ingrediente);
    ingredienti_magazzino* temp_ingrediente = ricerca_ingredienti_magazzino_chained(magazzino,ingrediente, key);
    
    if (temp_ingrediente!= NULL)
    {
        // Ingredient already in warehouse - add new batch
        temp_ingrediente->lotti = inserisci_lotto(temp_ingrediente,scadenza,qt);
        temp_ingrediente->q_tot += qt;
    }
    else
    {
        // New ingredient in warehouse
        ingredienti_magazzino * new = (ingredienti_magazzino*)malloc(sizeof(ingredienti_magazzino)); 
        new->next = magazzino[key].lista_ingredienti;
        
        new->dimensione = 0;
        new->ingrediente = temp;
        new->q_tot = qt;
        magazzino[key].lista_ingredienti = new;
        temp->magazzino = new;
        
        new->lotti = NULL;
        new->lotti = inserisci_lotto(new, scadenza,qt);
    }
}

/**
 * Adds ingredient to recipe with specified quantity
 * @param ricetta Recipe to modify
 * @param ingredienti_tabella Ingredients hash table
 * @param indice_ingrediente Index in recipe's ingredients array
 * @param ingrediente_nome Ingredient name
 * @param qt Quantity needed
 * @param len_parola Length of ingredient name
 */
void inserisci_ingredienti_ricettario(Ricetta* ricetta, Hash_table_ingredienti* ingredienti_tabella, int indice_ingrediente, char ingrediente_nome[MAXLEN], int qt, int len_parola){
    unsigned long int key_ingrediente = hash_function_ingredienti(ingrediente_nome);
    
    ricetta->ingredienti_ricettario[indice_ingrediente - 1].ingrediente = inserisci_ingredienti_chained(ingredienti_tabella,ingrediente_nome,key_ingrediente,len_parola);
    ricetta->ingredienti_ricettario[indice_ingrediente - 1].qt = qt;
}

/**
 * Inserts new recipe into recipe hash table
 * @param tabella Recipe hash table
 * @param ricetta_nome Recipe name
 * @param key Hash key
 * @param len_parola Length of recipe name
 * @return Pointer to new recipe if created, NULL if already exists
 */
Ricetta* inserisci_ricetta_chained(Hash_table_ricettario* tabella,char ricetta_nome[MAXLEN],int key, int len_parola){
    
    if (ricerca_ricetta_chained(tabella,ricetta_nome,key) != NULL)
    {
        return NULL; // Recipe already exists
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

// CLEANUP FUNCTIONS

/**
 * Deallocates entire recipe hash table
 */
void dealloca_ricettario(Hash_table_ricettario* tabella){
    
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Ricetta* ricetta = tabella[i].lista_ricette;
        while (ricetta != NULL)
        {
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

/**
 * Deallocates entire ingredients hash table
 */
void dealloca_ingredienti(Hash_table_ingredienti* tabella){
    
    for (int i = 0; i < HASH_SIZE_INGREDIENTI; i++)
    {
        ingredienti* ingrediente = tabella[i].lista_ingredienti;
        while (ingrediente != NULL)
        {
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

/**
 * Removes a recipe node from doubly linked list and deallocates it
 * @param ricetta_ptr Recipe to remove
 * @param testa Pointer to head of list
 */
void rimuovi_nodo(Ricetta* ricetta_ptr, Ricetta** testa){
    
    if (ricetta_ptr->prev != NULL)
    {
        ricetta_ptr->prev->next = ricetta_ptr->next;
    }
    else
    {
        *testa = ricetta_ptr->next;
    }
    
    if (ricetta_ptr->next != NULL)
    {
        ricetta_ptr->next->prev = ricetta_ptr->prev;
    }

    free(ricetta_ptr->ingredienti_ricettario);
    free(ricetta_ptr->nome);
    free(ricetta_ptr);
}

/**
 * Deallocates entire warehouse hash table
 */
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

// SHIPPING QUEUE OPERATIONS

/**
 * Maintains min-heap property for shipping queue (by order time)
 */
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

/**
 * Inserts order into shipping queue (min-heap by order time)
 */
void inserisci_spedizione(coda_spedizioni** coda, Ordine* ordine){
    (*coda)->dimensione++;
    
    (*coda)->coda = (min_heap_spedizione*)realloc((*coda)->coda,((*coda)->dimensione)*(sizeof(min_heap_spedizione)));
    
    (*coda)->coda[(*coda)->dimensione - 1].numero_ordini = ordine->numero_ordini;
    (*coda)->coda[(*coda)->dimensione - 1].ordine = ordine->ordine;
    (*coda)->coda[(*coda)->dimensione - 1].peso = 0;

    // Calculate total weight
    for (int j = 0; j < ordine->ordine->numero_ingredienti; j++)
    {
        (*coda)->coda[(*coda)->dimensione - 1].peso += ordine->ordine->ingredienti_ricettario[j].qt * ordine->numero_ordini;
    }
    
    (*coda)->coda[(*coda)->dimensione - 1].tempo_ordine = ordine->tempo_ordine;
    int i = (*coda)->dimensione - 1;
    
    // Bubble up to maintain heap property
    while (i > 0 && (*coda)->coda[parent(i)].tempo_ordine > (*coda)->coda[i].tempo_ordine)
    {
        min_heap_spedizione spedizione_temp;
        spedizione_temp = (*coda)->coda[parent(i)];
        (*coda)->coda[parent(i)] = (*coda)->coda[i];
        (*coda)->coda[i] = spedizione_temp;
        i = parent(i);
    }
}

/**
 * Consumes ingredients from warehouse for order fulfillment
 * Uses FIFO expiration policy (earliest expiring batches first)
 */
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

/**
 * Checks ingredient availability for recipe fulfillment
 * @param ricetta Recipe to check
 * @param magazzino Warehouse
 * @param numero_ordini Quantity needed
 * @param time Current time (for expiration check)
 * @return 0 if available, -1 if not available
 */
int controlla_disp(Ricetta* ricetta, Hash_table_magazzino* magazzino, int numero_ordini, uint32_t time){
    
    for (int i = 0; i < ricetta->numero_ingredienti; i++)
    {
        ingredienti_magazzino* temp = ricerca_ingredienti_magazzino_chained(magazzino, ricetta->ingredienti_ricettario[i].ingrediente->ingrediente, hash_function_magazzino(ricetta->ingredienti_ricettario[i].ingrediente->ingrediente));
        
        if (temp == NULL)
        {
            return -1;
        }
        else
        {
            // Check and remove expired batches
            min_heap* lotto_temp = controlla_scadenze(&temp,time);
            
            while (lotto_temp != NULL && temp->dimensione > 0)
            {
                lotto_temp = controlla_scadenze(&temp,time);
            }
          
            if (temp->q_tot < ricetta->ingredienti_ricettario[i].qt * numero_ordini)
            {
                return -1;   
            }
        }
    }
    return 0;
}

/**
 * Enhanced availability check for pending orders (with index tracking)
 * @param ordine Order to check
 * @param magazzino Warehouse
 * @param numero_ordini Quantity needed
 * @param time Current time
 * @return 1 if available, -1 if not available
 */
int controlla_disp_2(Ordine* ordine, Hash_table_magazzino* magazzino, uint32_t numero_ordini, uint32_t time){

    // Safety checks
    if (ordine->ordine == NULL ||ordine->index > ordine->ordine->numero_ingredienti|| ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente == NULL|| ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente->magazzino == NULL || ordine->ordine->ingredienti_ricettario[ordine->index].ingrediente->magazzino->q_tot < ordine->ordine->ingredienti_ricettario[ordine->index].qt * numero_ordini)
    {
        return -1;
    }
    
    for (int i = 0; i < ordine->ordine->numero_ingredienti; i++)
    {
        if (ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino == NULL)
        {
            ordine->index = i;
            return -1;      
        }
        else
        {
            // Remove expired batches
            min_heap* lotto_temp = controlla_scadenze(&ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino,time);
            
            while (lotto_temp != NULL && ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino->dimensione >0)
            {
                lotto_temp = controlla_scadenze(&ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino,time);
            }
           
            if (ordine->ordine->ingredienti_ricettario[i].ingrediente->magazzino->q_tot < ordine->ordine->ingredienti_ricettario[i].qt * numero_ordini)
            {
                ordine->index = i;
                return -1;   
            }
        }
    }
    return 1;
}

// ORDER QUEUE MANAGEMENT

/**
 * Inserts order into pending orders queue (FIFO)
 * Initializes index for tracking missing ingredients
 */
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
        (*coda)->coda= new;
        (*coda)->testa = new;
    }
    else
    {
        new->prev = (*coda)->coda;
        (*coda)->coda->next = new;
        (*coda)->coda = new;
    }
}

/**
 * Processes order fulfillment - removes from pending queue and adds to shipping
 * @param coda Pending orders queue
 * @param ordine_da_evadere Order to fulfill
 * @param coda_sp Shipping queue
 */
void evadi_ordini(coda_ordini** coda, Ordine** ordine_da_evadere, coda_spedizioni** coda_sp){

   Ordine* temp = *ordine_da_evadere;
   
   if (temp == (*coda)->testa)
   {
        if ((*coda)->testa->next != NULL)
        {
            (*coda)->testa->next->prev = NULL;
            (*coda)->testa = (*coda)->testa->next;
        }
        else
        {
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

// PRINTING QUEUE OPERATIONS

/**
 * Inserts completed order into print queue
 * @param coda Print queue (max-heap)
 * @param spedizione Shipping order to add
 */
void inserisci_in_stampa(coda_di_stampa** coda, min_heap_spedizione spedizione){
    (*coda)->dimensione++;
    (*coda)->pacchi = (max_heap*)realloc((*coda)->pacchi, (*coda)->dimensione * sizeof(max_heap));
    (*coda)->pacchi[(*coda)->dimensione - 1].nome_ordine = spedizione.ordine;
    (*coda)->pacchi[(*coda)->dimensione - 1].tempo_ordine = spedizione.tempo_ordine;
    (*coda)->pacchi[(*coda)->dimensione - 1].peso = spedizione.peso;
    (*coda)->pacchi[(*coda)->dimensione - 1].numero_ordini = spedizione.numero_ordini;
}

/**
 * Maintains max-heap property for print queue
 * Orders by weight (descending), then by time (ascending) for tie-breaking
 */
void MaxHeapify_stampa(coda_di_stampa** coda, uint32_t i){
    
    uint32_t l = left(i);
    uint32_t r = right(i);  
    uint32_t massimo = i;

    // Check left child (max-heap by weight, then min-heap by time for ties)
    if (l < (*coda)->dimensione && ((*coda)->pacchi[l].peso > (*coda)->pacchi[massimo].peso || ((*coda)->pacchi[l].peso == (*coda)->pacchi[massimo].peso &&(*coda)->pacchi[l].tempo_ordine < (*coda)->pacchi[massimo].tempo_ordine)))
    {
        massimo = l;
    }
    else
    {
        massimo = i;
    }
    
    // Check right child
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

/**
 * Builds max-heap from unordered array (bottom-up approach)
 */
void buildMaxHeap(coda_di_stampa** coda){
    int temp = (*coda)->dimensione;
    
    for (int i = ((temp) / 2) ; i >= 0; i--)
    {
        MaxHeapify_stampa(coda,i);
    }
}

// MAIN PROGRAM

int main(){
    
    // System configuration variables
    int camion = 0;                    // Courier frequency
    int dimensione_camion = 0;         // Truck capacity
    uint32_t time = 0;                 // Current simulation time
    int spedizioni = 0;                // Counter for courier visits

    // Input processing variables
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
    
    // Initialize all data structures
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
    
    // Read system configuration (courier frequency and truck capacity)
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

    // Processing variables
    uint32_t flag = 0;    
    Ricetta* ricetta_ptr = NULL;
    uint32_t indice = 0;
    bool ingredienti_f = false;
    unsigned long int ascii = 0;

    // MAIN PROCESSING LOOP - Read and process commands
    while((c = getc(stdin)) != EOF){
         
        flag = 0;
        
        // Read command
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

        // Command classification
        if (flag == 0 && comando[0] == 'a')
        {
            flag = 1; // aggiungi_ricetta
        }
        else if (flag == 0 && comando[0] == 'r' && comando[7] == '_')
        {
            flag = 2; // rimuovi_ricetta
        }
        else if (flag == 0 && comando[0] == 'r')
        {
            flag = 3; // rifornimento
        }
        else if (flag == 0 && comando[0] == 'o')
        {
            flag = 4; // ordine
        }
        
        // COMMAND: ADD RECIPE (aggiungi_ricetta)
        if (flag == 1)
        {
            numero_ingredienti = 0;
            len_parole = 0;
            
            // Read recipe name
            while ((c = getc(stdin)) != ' ')
            {
                buffer[len_parole] = c;
                len_parole++;
            }
            buffer[len_parole] = '\0';
            strcpy(ricetta, buffer);

            memset(buffer, '\0', buff_size);
            ascii = hash_function(ricetta);
            
            // Try to insert recipe
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
                // Parse ingredients and quantities
                len_parole = 0;
                while ((c = getc(stdin)) != '\n')
                {
                    // Count ingredients while reading
                    if (c == ' ')
                    {
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
                
                // Parse and store ingredients
                numero_ingredienti = 0;
                char c_t = 'a';
                char temp[MAXLEN];
                int z = 0;
                indice = 0;
                
                for (int i = 0; i <= strlen(buffer); i++)
                {
                    c_t = parser(buffer, i);

                    temp[z] = c_t;
                    z++;
                    
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
                // Skip rest of line if recipe was ignored
                while ((c = getc(stdin)) != '\n')
                {
                    ;
                }
            }

            // Reset variables
            memset(buffer, '\0', buff_size);
            ingredienti_f = false;
            len_parole = 0;
        }
        
        // COMMAND: REMOVE RECIPE (rimuovi_ricetta)
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

        // COMMAND: RESTOCK (rifornimento)
        if (flag == 3)
        {            
            indice = 0;
            len_parole = 0;
            
            // Parse ingredient batches
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
                        
                        // Add batch to warehouse
                        inserisci_ingredienti_magazzino_chained(magazzino,tabella_ingredienti,ingrediente,scadenza,qt, time);
                    }
                    len_parole = 0;
                }
            }
            
            // Process last batch
            buffer[len_parole] = '\0';
            scadenza = atoi(buffer);
            memset(buffer,'\0' ,buff_size);
            
            indice = 0;
            len_parole = 0;
            
            inserisci_ingredienti_magazzino_chained(magazzino,tabella_ingredienti,ingrediente,scadenza,qt, time);
            indice = 0;

            // Try to fulfill pending orders after restocking
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
        
        // COMMAND: ORDER (ordine)
        if (flag == 4)
        {
            indice = 0;
            len_parole = 0;
            
            // Parse recipe name and quantity
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
            
            // Check if recipe exists
            ricetta_ptr = ricerca_ricetta_chained(tabella, ricetta, hash_function(ricetta));
            if (ricetta_ptr == NULL)
            {
                fputs("rifiutato\n", stdout);
            }
            else
            {
                fputs("accettato\n", stdout);
                
                ricetta_ptr->count++;
                
                // Check ingredient availability
                if ( controlla_disp(ricetta_ptr,magazzino,numero_ordini, time) == -1)
                {
                    // Add to pending queue
                    inserisci_ordine_attesa(&Coda_ordini, ricetta_ptr, magazzino,time,numero_ordini);
                }
                else
                {
                    // Fulfill immediately
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
        
        // COURIER DELIVERY PROCESSING
        int dimensione_camion_temp = 0;
        if (spedizioni == camion)
        {
            dimensione_camion_temp = dimensione_camion;
            spedizioni = 0;
            
            if (Coda_spedizioni->dimensione != 0)
            {
                dimensione_camion_temp = dimensione_camion_temp - Coda_spedizioni->coda[0].peso;

                // Load orders chronologically until truck is full
                while ( Coda_spedizioni->dimensione != 0 && dimensione_camion_temp > 0)
                {
                    if (dimensione_camion_temp - Coda_spedizioni->coda[0].peso >0)
                    {
                        // Add order to print queue
                        inserisci_in_stampa(&Coda_di_stampa,Coda_spedizioni->coda[0]);
                        Coda_spedizioni->coda[0].ordine->count--;
                        
                        // Remove from shipping queue
                        Coda_spedizioni->coda[0] = Coda_spedizioni->coda[Coda_spedizioni->dimensione - 1];
                        Coda_spedizioni->dimensione--;
                        MinHeapify_spedizioni(Coda_spedizioni, 0);
                    }

                    dimensione_camion_temp = dimensione_camion_temp - Coda_spedizioni->coda[0].peso;
                }

                // Sort print queue by weight (desc) then time (asc)
                buildMaxHeap(&Coda_di_stampa);

                // Print delivery manifest
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
        
    // CLEANUP - Free all allocated memory
    free(Coda_spedizioni->coda);
    free(Coda_spedizioni);
    
    free(buffer);
    
    dealloca_ricettario(tabella);
    free(tabella);
    
    dealloca_ingredienti(tabella_ingredienti);
    free(tabella_ingredienti);
    
    dealloca_magazzino(magazzino);
    free(magazzino);
    
    // Free pending orders queue
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