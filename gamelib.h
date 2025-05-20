#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define YELLOW  "\e[0;33m"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define BLUE    "\e[0;34m"
#define RESET   "\e[0m"
#define MAX_STANZE 100

void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();


static bool settato = false;

typedef enum Classe_giocatore {principe, doppleganger} Classe_giocatore;
typedef enum Classe_nemico {nessun_nemico,scheletro, guardia, Jaffar} Classe_nemico;
typedef enum Tipo_stanza {corridoio, scala, sala_banchetto, magazzino, posto_guardia, prigione, armeria, moschea, torre, bagni} Tipo_stanza;
typedef enum Tipo_trabocchetto {nessuno, tegola, lame, caduta, burrone} Tipo_trabocchetto;
typedef enum Tipo_tesoro {nessun_tesoro, verde_veleno, blu_guarigione, rosso_aumenta_vita, spada_tagliente, scudo} Tipo_tesoro;



typedef struct Giocatore {
    char nome_giocatore [25];
    enum Classe_giocatore classe_giocatore;
    struct Stanza* posizione;
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attaco;
    unsigned char dadi_difesa;
    unsigned char scappatoie_usate; //campo aggiunto per gestire le scappatoie in base al giocatore
} Giocatore;

typedef struct Stanza {
    struct Stanza* stanza_destra;
    struct Stanza* stanza_sinistra;
    struct Stanza* stanza_sopra;
    struct Stanza* stanza_sotto;
    enum Tipo_stanza tipo_stanza;
    enum Tipo_trabocchetto tipo_trabocchetto;
    enum Tipo_tesoro tipo_tesoro;
    enum Classe_nemico nemico;
    int x; // coordinate per le stanze
    int y;
} Stanza;

typedef struct Nemico {
    enum Classe_nemico nemico;
    unsigned char vita_nemico;
    unsigned char attacco_nemico;
    unsigned char difesa_nemico;
} Nemico;


#define DEBUG 1
#if DEBUG
    #define DBG_PRINTF(...) printf(BLUE"[DEBUG]"__VA_ARGS__); printf(RESET)
#else
    #define DBG_PRINTF(...)
#endif
