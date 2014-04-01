/* Programma che, acquisito un numero intero n, genera casualmente un labirinto di lato n e trova tutti i cammini semplici dalla casella di partenza 
a quella di arrivo (generate anch'esse casualmente). Il labirinto è una matrice in cui ogni casella è attraversabile nelle quattro direzioni se il 
suo valore è 1. 
  Studente: Elvis Ciotti  Prof. Marco Bernardo 
  Algoritmi e Strutture Dati -  C.d.l. in Informatica Applicata - Facoltà SMFN - Urbino 
  11 giugno 2003 */ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
 
 
typedef struct albero { 
   int i,j; 
   char padre;  /* Direzione presa dal padre. 'n'=nord, 'e'=est, 's'=sud, 'o'=ovest, 'r'=radice */ 
   int figlio[4];  /* Indicatore esistenza figli se impostato a 1. [0]=nord, [1]=est, [2]=sud, [3]=ovest */ 
   struct albero *nord, *est, *sud, *ovest, *rit; /* Puntatori a i 4 figli e uno per il ritorno */ 
} albero_t;   
 
 
#define COORD_REL 0 /* Se uguale a zero inizia il conteggio del numero di riga/colonna da 1, se impostato a 1 inizia da 0*/ 
 
 
void crea_matrice(int ***a, int *n, int *part_i, int *part_j, int *arr_i, int *arr_j); 
void stampa_mat_e_perc(int **a, int n, int part_i, int part_j, int arr_i, int arr_j); 
void crea_stamp_distr(int **a, int n, int corr_i, int corr_j, albero_t **p, albero_t **prec, int part_i, int part_j, int *n_perc, char 
prov); 
void stampa_per(albero_t *p, int *n_perc); 
void rimuovi_term(albero_t **p); 
int gia_pres(albero_t *p, int i, int j); 
void disalloca_matrice(int ***a, int n); 
 
 
int main(void) 
{ 
   int **a = NULL, 
       n,     /* Dimensione labirinto */ 
       part_i, part_j, arr_i, arr_j, /* Coordinate casella di partenza e arrivo */ 
       n_perc = 0; /* Contatore numero di percorsi trovati */ 
   albero_t *head = NULL,   /* Puntatore alla radice dell'albero*/ 
   *inizio = NULL;  
 
      
   /* Creazione matrice e coordinate di arrivo e partenza */ 
   crea_matrice(&a, &n, &part_i, &part_j, &arr_i, &arr_j); 
    
   /* Stampa la matrice labirinto generata e le coordinate di arrivo e partenza */ 
   stampa_mat_e_perc(a, n, part_i, part_j, arr_i, arr_j); 
 
   /* Inizia la costruzione-stampa-rimozione dinamica in modo ricorsivo dell'albero dei percorsi trovati */ 
   crea_stamp_distr(a, n, arr_i, arr_j, &head, &inizio, part_i, part_j, &n_perc,  'r'); 
 
   /* Disalloca la matrice labirinto */ 
   disalloca_matrice(&a, n+2); 
 
   printf("Percorsi totali trovati: %d.\n", n_perc); 
  
   return (0); 
} 
 
 
/* Acquisce la dimensione del labirinto e lo genera casualmente assieme alle caselle di arrivo e partenza */ 
void crea_matrice(int ***a, int *n, int *part_i, int *part_j, int *arr_i, int *arr_j) 
{ 
   char temp[5]; /* Stringa temporanea per l'acquisizione del numero intero */ 
   int i, j; 
 
    
   /* Acquisisco dimensione labirinto */ 
   do 
   { 
     printf("Inserisci dimensione matrice (maggiore o uguale a 3): "); 
     scanf("%s", temp); 
     *n = atoi(temp); /* Se la stringa non contiene un numero la funzione atoi restituisce 0 e viene richiesto di nuovo l'input */ 
   } while(*n < 3); 
   
   /* Alloco memoria al puntatore passato per indirizzo per creare la matrice dinamica di dim n+2  
     inizialmente azzerata */ 
   *a = (int **)calloc((size_t)*n+2, sizeof(int *)); 
   for (i=0; (i < *n+2); i++) 
     (*a)[i] = (int *)calloc((size_t)*n+2, sizeof(int)); 
 
   /* Inizializzo il seme */ 
   srand((unsigned int)time(NULL)); 
   
   /* Genero labirinto casualmente (cornici escluse) */ 
   for (i=1; (i <= *n); i++) 
     for (j=1; (j <= *n); j++)    
        (*a)[i][j] = rand() % 2; 
    /* Genero coordinate di partenza scelte a caso nel labirinto */ 
   *part_i = rand() % *n + 1; 
   *part_j = rand() % *n + 1; 
   (*a)[*part_i][*part_j] = 1; 
   /* Genero coordinate di arrivo scelte a caso nel labirinto */ 
   *arr_i = rand() % *n + 1; 
   *arr_j = rand() % *n + 1; 
   (*a)[*arr_i][*arr_j] = 1; 
}   
 
 
/* Stampa la matrice e le coordinate di arrivo e partenza */ 
void stampa_mat_e_perc(int **a, int n, int part_i, int part_j, int arr_i, int arr_j) 
{ 
   int i,j;   
    
    
   printf("Labirinto generato:\n"); 
   for (i = 1; (i <= n); i++) 
   { 
      for (j = 1; (j <= n); j++) 
         printf("%3d", a[i][j]); 
      printf("\n"); 
   }      
   printf("\nPERCORSI TROVATI DA (%d:%d) A (%d:%d):\n", part_i-COORD_REL, part_j-COORD_REL, arr_i-COORD_REL, arr_j-COORD_REL); 
}   
 
 /* Creazione ricorsiva con stampa e distruzione dei percorsi memorizzati in una struttura ad albero */ 
void crea_stamp_distr(int **a, int n, int corr_i, int corr_j, albero_t **p, albero_t **prec, int part_i, int part_j, int *n_perc, char prov) 
{ 
   /* Alloca memoria per il puntatore al nodo corrente*/ 
   *p = (albero_t *)malloc(sizeof(albero_t)); 
   /* Controllo disponibilità memoria */ 
   if (*p != NULL) 
   { 
      /* Scrivo coordinate, puntatore al padre e direzione presa dal padre in base ai parametri in input alla funzione */      
      (*p)->i = corr_i; 
      (*p)->j = corr_j; 
      (*p)->rit = *prec; 
      (*p)->padre = prov; 
      /* Controllo caso in cui si è trovato la part in fondo all'albero con relativa stampa del perc e distr del ramo terminale */ 
      if ((corr_i == part_i) && (corr_j == part_j)) 
      { 
         stampa_per(*p, &(*n_perc)); 
         rimuovi_term(&(*p)); 
      } 
      else 
      { 
   /* Imposta il parametro figlio[n] che non è attraversabile se le coordinate nella direzione n sono a zero, oppure se le 
            coordinate sono già presenti nel perc semplice, cioè fra il nodo (escluso) e la radice.(legge di De Morgan) */  
         (*p)->figlio[0] = ((a[corr_i-1][corr_j] != 0) && !gia_pres(*p, corr_i-1, corr_j)); 
         (*p)->figlio[1] = ((a[corr_i][corr_j+1] != 0) && !gia_pres(*p, corr_i, corr_j+1)); 
         (*p)->figlio[2] = ((a[corr_i+1][corr_j] != 0) && !gia_pres(*p, corr_i+1, corr_j)); 
         (*p)->figlio[3] = ((a[corr_i][corr_j-1] != 0) && !gia_pres(*p, corr_i, corr_j-1)); 
 
   /* Eventuale rimozione del ramo terminale che non ha figli (e che non contiene la partenza in fondo) */ 
   if ((*p)->figlio[0] == 0 && (*p)->figlio[1] == 0 && (*p)->figlio[2] == 0 && (*p)->figlio[3] == 0) 
            rimuovi_term(&(*p)); 
         else 
          { 
           /* Richiama questa funzione ricorsivamente nelle direzioni libere non ancora visitate */ 
             if ((*p)->figlio[0] != 0) 
                crea_stamp_distr(a, n, corr_i-1, corr_j, &((*p)->nord), &(*p), part_i, part_j, &(*n_perc), 'n'); 
             if ((*p)->figlio[1] != 0)   
                crea_stamp_distr(a, n, corr_i, corr_j+1, &((*p)->est), &(*p), part_i, part_j, &(*n_perc), 'e'); 
             if ((*p)->figlio[2] != 0) 
                crea_stamp_distr(a, n, corr_i+1, corr_j, &((*p)->sud), &(*p), part_i, part_j, &(*n_perc), 's'); 
             if ((*p)->figlio[3] != 0) 
                crea_stamp_distr(a, n, corr_i, corr_j-1, &((*p)->ovest), &(*p), part_i, part_j, &(*n_perc), 'o'); 
          }         
      } 
   } 
   else 
      printf("Memoria esaurita. Impossibile continuare.\n"); 
}    
 
/* Stampa il percorso a ritroso a puntatore al nodo passato per copia che contiene le coordinate di partenza */ 
void stampa_per(albero_t *p, int *n_perc) 
{ 
   (*n_perc)++; 
      
    
   printf("[ PERCORSO %d ] : partenza -> %d,%d ->", *n_perc, p->i-COORD_REL, p->j-COORD_REL); 
   /* Scorrimento fino alla radice dell'albero  e stampa delle coordinate trovate */ 
   while ( p->padre != 'r') 
   { 
      p = p->rit; 
      printf(" %d,%d ->", p->i-COORD_REL, p->j-COORD_REL); 
   } 
   printf(" arrivo.\n"); 
} 
 
 
/* Rimuove ramo terminale puntato da 'p' */ 
void rimuovi_term(albero_t **p) 
{ 
   int prov; 
   albero_t *temp; /* Puntatore temporaneo per rimozione */ 
 
    
   /* Controlla il caso in cui il puntatore passato sia la testa dell'albero e in tal caso lo libera (avviene solo se le caselle di arrivo e 
      partenza coincidono) */ 
   if ((*p)->padre == 'r') 
   free(*p); 
   else  /* se il puntatore non punta alla radice  */ 
   {   
      /* Rimuove ramo terminale salendo finchè trova rami liberi o è alla radice. (De Morgan) */      
      while( (*p)->figlio[0] == 0 && (*p)->figlio[1] == 0 && (*p)->figlio[2] == 0 && (*p)->figlio[3] == 0 && (*p)->padre != 'r' ) 
      { 
         temp = *p; /* Copia il puntatore corrente */ 
         prov = temp->padre; /* Memorizza la direzione presa dal puntatore */ 
         (*p) = (*p)->rit; /* Salita di un livello del puntatore */ 
         free(temp); /* Free del puntatore copiato all'inizio*/ 
       
   /* Annullamento della direzione da cui proveniva il puntatore (prima di salire) */ 
         if (prov == 'n') 
           (*p)->figlio[0] = 0; 
         if (prov == 'e') 
           (*p)->figlio[1] = 0;  
         if (prov == 's') 
           (*p)->figlio[2] = 0;  
         if (prov == 'o') 
           (*p)->figlio[3] = 0;   
      } 
   }   
}    
         
 
/* Ricerca all'indietro delle coordinate (i,j) a partire dal nodo puntato (escluso); restituisce vero (non zero) se la coordinata è già presente */ 
int gia_pres(albero_t *p, int i, int j) 
{   
   int ris = 0; 
    
   /* La ricerca prosegue finchè non si è alla radice o finchè non si è già trovata una coordinata uguale */ 
   while ( (p->padre != 'r') && !ris) 
   { 
      p = p->rit; 
      ris = ( (p->i == i) && (p->j == j) ); 
   }      
   return ris; /* Se non si sono trovate la coordinate, restituisce zero */ 
} 
 
   
/* Disalloca un array bidimensionale dinamico di lato n, prendendo per indirizzo il puntatore originario */ 
void disalloca_matrice(int ***a, int n) 
{ 
   int i; 
 
    
   /* Disalloco memoria ai puntatori del vettore */ 
   for (i = 0; (i < n); i++) 
      free((*a)[i]); 
   /* Disalloco memoria al puntatore originario */ 
   free(*a); 
} 