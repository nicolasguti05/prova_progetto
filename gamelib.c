#include "gamelib.h"

unsigned short durata_intervallo = 2;


static Stanza* pFirst = NULL; //puntatore alla prima stanza della mappa
static Stanza* pLast = NULL;
static int turno_corrente = 0; // Variabile globale per tenere traccia del turno corrente

static Giocatore* giocatori[3] = {0,0,0}; //array di 3 giocatori(NULL se non partecipano)

//funzioni per imposta_gioco()
static void creazione_giocatori();
static int ottieni_numero_giocatori();
static void crea_giocatore(int i, bool* prince);
static void menu_impostazione_mappa();
static void ins_stanza();
static void canc_stanza();
static void stampa_stanze();
static void genera_random();
static void svuota_mappa();
static void chiudi_mappa();
static bool posizione_occupata(int x, int y); 
static int numero_stanze = 0;
static int mappa_chiusa = 0;


//funzioni per gioca()
static void avanza(Giocatore* giocatore);
static void combatti(Giocatore* giocatore);
static void scappa(Giocatore* giocatore);
static void stampa_giocatore(Giocatore* giocatore);
static void stampa_zona(Giocatore* giocatore);
static void prendi_tesoro(Giocatore* giocatore);
static void cerca_porta_segreta(Giocatore* giocatore);
static void passa(int num_giocatori);

static unsigned short calcola_colpi(unsigned short dadi_attacco);
static unsigned short calcola_parate(unsigned short dadi_difesa);
static void esegui_attacco(Giocatore* giocatore, unsigned short* vita_nemico, unsigned short difesa_nemico);
static void esegui_attacco_nemico(Giocatore* giocatore, unsigned short attacco_nemico);
unsigned short lancia_dado();
static bool presenza_nemico(Giocatore* giocatore);
static void gestisci_scappatoia(Giocatore* giocatore);
static void gestisci_trabocchetto(Giocatore*giocatore);
static void ritorna_stanza_precedente(Giocatore* giocatore);
static void stampa_collegamenti(Stanza* zona);
static void stampa_pericoli(Stanza* zona);
static const char* verifica_tipo_zona(enum Tipo_stanza tipo);
static const char* verifica_tipo_trabocchetto(enum Tipo_trabocchetto tipo);
static const char* verifica_tipo_tesoro(enum Tipo_tesoro tipo);
static void stampa_tesori(Stanza* zona);
static void applica_effetto_tesoro(Giocatore* giocatore, Tipo_tesoro tesoro);
static Stanza* crea_stanza_segreta();
static void collega_stanza_segreta(Stanza* stanza_originale, Stanza* stanza_segreta, const char* direzione);
static bool esiste_porta_segreta();



//funzioni per termina_gioco()




//funzioni per crediti()



void imposta_gioco() {
    if(settato == false) {
        printf("Questa è la prima volta che imposti il gioco, quindi ti chiediamo di iniziare con la creazione dei giocatori.\n");
        sleep(durata_intervallo);
        creazione_giocatori();
        printf("Ora che hai impostato i giocatori, procedi con la creazione della mappa.\n");
        sleep(durata_intervallo);
        menu_impostazione_mappa();
    }
    settato = true;

}


void gioca() {
    if(!settato) {
        printf(RED"Il gioco non è stato impostato!\n");
        printf("Impostarlo prima di iniziare a giocare\n"RESET);
        return;
    }

    printf("Gioco iniziato\n");
    srand(time(NULL));

    Giocatore* ordine_giocatori[3];
    int num_giocatori = 0;

    //funzione per portare tutti i giocatori al punto di partenza
    for (int i = 0; i < 3; i++) {
        if (giocatori[i] != NULL) {
            ordine_giocatori[num_giocatori++] = giocatori[i];
            giocatori[i]->posizione = pFirst;
        }
    }

    while(1) {
        //funzione per mesolare l'ordine dei giocatori
        for (int i = num_giocatori - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Giocatore* temp = ordine_giocatori[i];
            ordine_giocatori[i] = ordine_giocatori[j];
            ordine_giocatori[j] = temp;
        }

        //turno di ogni giocatore
        for (int i = 0; i < num_giocatori; i++) {
            Giocatore* giocatore = ordine_giocatori[i];

            // Salta se il giocatore è morto
            if (giocatore->p_vita <= 0) {
                printf (RED"Il giocatore %d (%s) è stato sconfitto e non può più giocare.\n", 
                i + 1, (giocatore->classe_giocatore == principe) ? "Principe" : "Doppleganger"RESET);
                continue;
            }


            printf("Turno del giocatore %s (%s)\n", giocatore->nome_giocatore + 1, (giocatore->classe_giocatore == principe) ? "Principe" : "Doppleganger");

            int avanzato = 0;
            int azione;

            do {
                printf("--- Azioni diponibili: ---\n");
                printf("1) Avanza\n");
                printf("2) Combatti\n");
                printf("3) Scappa\n");
                printf("4) Stampa valori giocaotre\n");
                printf("5) Stampa zona\n");
                printf("6) Prendi tesoro\n");
                printf("7) Cerca porta segreta\n");
                printf("8) Passa turno\n");
                printf("Scegliere un'azione: ");
                scanf("%d", &azione);

                switch (azione) {
                    case 1:
                        if (!avanzato) {
                            avanza(giocatore);
                            avanzato = 1;
                        } else {
                            printf("Hai già avanzato in questo turno!\n");
                        }
                    break;

                    case 2:
                        combatti(giocatore);
                    break;

                    case 3:
                        scappa(giocatore);
                    break;

                    case 4:
                        stampa_giocatore(giocatore);
                    break;

                    case 5:
                        stampa_zona(giocatore);
                    break;

                    case 6:
                        prendi_tesoro(giocatore);
                    break;

                    case 7:
                        if(!avanzato) {
                            cerca_porta_segreta(giocatore);
                        } else {
                            printf("Non puoi cercare una porta segreta dopo aver avanzato!\n");
                        }
                    break;

                    case 8:
                        printf("Passo il turno...\n");
                        passa(num_giocatori);
                    break;

                    default: 
                        printf("Scelta non valida.\n");
                    break;
                }
            }
            while (azione != 8);
        }
    }
}


void termina_gioco() {

}


void crediti() {

}


static void creazione_giocatori() {
    // Libera i giocatori precedenti
    for (int i = 0; i < 3; i++) {
        free (giocatori[i]);
        giocatori[i] = NULL;
    }
    
    int num_giocatori = ottieni_numero_giocatori();

    bool prince = false;
    for (int i = 0; i < num_giocatori; i++) {
        crea_giocatore(i, &prince);
    }

    if (!prince) {
        printf("ERROE! Deve esserci almeno un principe per giocare.\n");
        return;
    }
}

static int ottieni_numero_giocatori() {
    int num_giocatori;
    int risultato;

    do {
        printf ("Inserisci il numero di giocatori (da 1 a 3): \n");
        risultato = scanf ("%d", &num_giocatori);

        if (risultato != 1 || num_giocatori < 1 || num_giocatori > 3) {
            printf (RED"Numero di giocatori non valido.\n"RESET);
            sleep(durata_intervallo);

            // Pulisce il buffer di input se l'input non è valido
            while (getchar() != '\n');
        }
    }
    while (risultato != 1 || num_giocatori < 1 || num_giocatori > 3);

    return num_giocatori;
}

static void crea_giocatore(int i, bool* prince) {
    giocatori[i] = malloc(sizeof(Giocatore));
        if (!giocatori[i]) {
            printf(RED"Errore di allocazione memoria!"RESET);
            exit(1);
        }

        printf ("Inserisci il nome del giocatore %d: ", i + 1);
        scanf("%s", giocatori[i]->nome_giocatore);

        if(!*prince) {
            printf ("Il primo giocatore deve essere un Principe.\n");
            giocatori[i]->classe_giocatore = principe;
            *prince = true;
        } else {
            printf("Il giocatore sarà un Doppleganger.\n"); 
            giocatori[i]->classe_giocatore = doppleganger;
        }

        giocatori[i]->p_vita_max = 3;
        giocatori[i]->p_vita = 3;
        giocatori[i]->dadi_attaco = 2;
        giocatori[i]->dadi_difesa = 2;
        giocatori[i]->posizione = NULL;
}


    //menu di gestione della mappa
    static void menu_impostazione_mappa(){
    int scelta;
    do {
        printf(YELLOW"--- GESTIONE MAPPA ---\n"RESET);
        printf("1) Inserisci stanza \n");
        printf("2) Cancella stanza \n");
        printf("3) Stampa stanze \n");
        printf("4) Genera mappa random \n");
        printf("5) Chiudi mappa \n");
        printf("Scelta: \n");
        scanf("%d", &scelta);

        switch (scelta) {
        case 1:
            ins_stanza();
            break;
        case 2:
            canc_stanza();
            break;
        case 3:
            stampa_stanze();
            break;
        case 4:
            genera_random();
            break;
        case 5:
            chiudi_mappa();
            return;

        default:
            printf(RED"Scelta non valida! \n"RESET);
            break;
        }
    } while(1);
}

static void ins_stanza() {
    if(mappa_chiusa) {
        printf ("La mappa è chiusa! Non puoi aggiungere altre stanze.\n");
        return;
    }

    Stanza* nuova_stanza = (Stanza*)malloc(sizeof(Stanza));
    if (!nuova_stanza) {
        printf (RED"Errore di allocazione memoria.\n"RESET);
        return;
    }

    //Inizializza la nuova stanza
    nuova_stanza->stanza_sopra = NULL;
    nuova_stanza->stanza_sotto = NULL;
    nuova_stanza->stanza_sinistra = NULL;
    nuova_stanza->stanza_destra = NULL;

    printf ("Inserire tipo stanza (1-10): ");
    scanf ("%u", &nuova_stanza->tipo_stanza);

    printf ("Inserire tipo trabocchetto: (1-5): ");
    scanf ("%u", &nuova_stanza->tipo_trabocchetto);
    
    printf ("Inserire tipo tesoro (1-6): ");
    scanf ("%u", &nuova_stanza->tipo_tesoro);

   

    if (!pFirst) {
        nuova_stanza->x = 0;
        nuova_stanza->y = 0;
        pFirst = nuova_stanza;
        pLast = nuova_stanza; //La prima stanza è anche l'ultima
    } else {
        int scelta;
        printf ("Dove collegare la stanza? \n(1=sopra, 2=detra, 3=sinistra, 4=sotto): ");
        scanf ("%d", &scelta);


        int new_x = pLast->x;
        int new_y = pLast->y;

        switch (scelta) {
            case 1:
                if (pLast->stanza_sopra) {
                    printf ("Una stanza è già collegata sopra!\n");
                    free(nuova_stanza);
                    return;
                }
                new_y--;
                break;

            case 2:
                if (pLast->stanza_destra) {
                    printf ("Una stanza è già collegata a destra!\n");
                    free(nuova_stanza);
                    return;
                }
                new_x++;
                break;

            case 3:
                if (pLast->stanza_sinistra) {
                    printf ("Una stanza è già collegata a sinistra!\n");
                    free(nuova_stanza);
                    return;
                }
                new_x--;
                break;

            case 4:
                if (pLast->stanza_sotto) {
                    printf ("Una stanza è già colegata sotto!\n");
                    free(nuova_stanza);
                    return;
                }
                new_y++;
                break;

            default:
                printf ("Scelta non valida!\n");
                free(nuova_stanza);
                return;
        }


        //Verifica se la posizione è gia occupata
        if (posizione_occupata(new_x, new_y)) {
            printf ("C'è già una stanza in quella posizione!\n");
            free(nuova_stanza);
            return;
        }

        // Assegna coordinate
        nuova_stanza->x = new_x;
        nuova_stanza->y = new_y;

        // Collega le stanze
        switch (scelta) {
            case 1:
                pLast->stanza_sopra = nuova_stanza;
                nuova_stanza->stanza_sotto = pLast;
                break;
            case 2:
                pLast->stanza_destra = nuova_stanza;
                nuova_stanza->stanza_sinistra = pLast;
                break;
            case 3:
                pLast->stanza_sinistra = nuova_stanza;
                nuova_stanza->stanza_destra = pLast;
                break;
            case 4:
                pLast->stanza_sotto = nuova_stanza;
                nuova_stanza->stanza_sopra = pLast;
                break;
        }
    }

    pLast = nuova_stanza;
    numero_stanze++;
}

// Funzione per verificare se una stanza è già occupata
static bool posizione_occupata(int x, int y) {
    Stanza* corrente = pFirst;
    while (corrente) {
        if (corrente->x == x && corrente->y == y) {
            return true;
        }
        if (corrente->stanza_destra) corrente = corrente->stanza_destra;
        else if (corrente->stanza_sinistra) corrente = corrente->stanza_sinistra;
        else if (corrente->stanza_sopra) corrente = corrente->stanza_sopra;
        else if (corrente->stanza_sotto) corrente = corrente->stanza_sotto;
        else break;
    }
    return false;
}

static void canc_stanza() {
    if (!pFirst) {
        printf ("Nessuna stanza da eliminare!\n");
        return;
    }

    Stanza* temp = pFirst;
    Stanza* prev = NULL;
    while (temp->stanza_sopra || temp->stanza_destra || temp->stanza_sinistra || temp->stanza_sotto) {
        prev = temp;
        if (temp->stanza_sopra) temp = temp->stanza_sopra;
        else if (temp->stanza_destra) temp = temp->stanza_destra;
        else if (temp->stanza_sinistra) temp = temp->stanza_sinistra;
        else temp = temp->stanza_sotto;
    }

    if (prev) {
        if (prev->stanza_sopra == temp) prev->stanza_sopra = NULL;
        if (prev->stanza_destra == temp) prev->stanza_destra = NULL;
        if (prev->stanza_sinistra == temp) prev->stanza_sinistra = NULL;
        if (prev->stanza_sotto == temp) prev->stanza_sotto = NULL;
    } else {
        pFirst = NULL;
    }
    free(temp);
    numero_stanze--;
}


static void stampa_stanze() {
    if (!pFirst) {
        printf ("Non ci sono stanze da stampare!\n");
        return;
    }

    Stanza* visitate[MAX_STANZE];
    int num_visitate = 0;

    Stanza* temp = pFirst;
    int contatore = 1;

    while (temp && num_visitate < MAX_STANZE) {
        // Controlla se la stanza è già stata visitata
        int gia_stampata = 0;
        for (int i = 0; i < num_visitate; i++) {
            if(visitate[i] == temp) {
                gia_stampata = 1;
                break;
            }
        }

        if (gia_stampata) break;

        // Aggiunge la stanza all'elenco delle visitate
        visitate[num_visitate++] = temp; 

        printf ("Stanza %d;  Tipo: %s;  Trabocchetto: %s;  Tesoro: %s \n", contatore, 
        verifica_tipo_zona(temp->tipo_stanza),
        verifica_tipo_trabocchetto(temp->tipo_trabocchetto),
        verifica_tipo_tesoro(temp->tipo_tesoro));

        contatore++;

        if (temp->stanza_sopra && !gia_stampata) temp = temp->stanza_sopra;
        else if (temp->stanza_destra && !gia_stampata) temp = temp->stanza_destra;
        else if (temp->stanza_sinistra && !gia_stampata) temp = temp->stanza_sinistra;
        else if (temp->stanza_sotto && !gia_stampata) temp = temp->stanza_sotto;
        else temp = NULL;
    }

    if (num_visitate == MAX_STANZE) {
        printf ("Attenzione: raggiunto massimo di stanze stampabili (%d)\n", MAX_STANZE);
    }
}

static const char* verifica_tipo_zona (enum Tipo_stanza tipo) {
    switch (tipo) {
        case corridoio: return "Corridoio";
        case scala: return "Scala";
        case sala_banchetto: return "Sala banchetto";
        case magazzino: return "Magazzino";
        case posto_guardia: return "Posto guardia";
        case prigione: return "Prigione";
        case armeria: return "Armeria";
        case moschea: return "Moschea";
        case torre: return "Torre";
        case bagni: return "Bagni";
        default: return "Sconosiuto";
    }
}

static const char* verifica_tipo_trabocchetto(enum Tipo_trabocchetto tipo) {
    switch (tipo) {
        case nessuno: return "Nessuno";
        case tegola: return "Tegola";
        case lame: return "Lame";
        case caduta: return "Caduta";
        case burrone: return "Burrone";
        default: return "Sconosciuto";
    }
}


static const char* verifica_tipo_tesoro(enum Tipo_tesoro tipo) {
    switch (tipo) {
        case nessun_tesoro: return "Nessuno";
        case verde_veleno: return "Verde veleno";
        case blu_guarigione: return "Blu guarigione";
        case rosso_aumenta_vita: return "Rosso aumenta vita";
        case spada_tagliente: return "Spada tagliente";
        case scudo: return "Scudo";
        default: return "Sconosciuto";
    }
}

static void genera_random() {

    char scelta;

    // Avverte l'utente se ci sono stanze già presenti
    if (pFirst) {
        printf ("Generando una mappa random le stanze precedentemente create verranno cancellate!\n");
        printf ("Continuare? (s/n): ");
        scanf (" %c", &scelta);

        if (scelta != 's' && scelta != 'S' ) {
            printf ("Operazione annullata.\n");
            return;
        }

        svuota_mappa();
    }

    srand(time(NULL));

    Stanza* stanze[MAX_STANZE];
    int count = 0;

    Stanza* prima = malloc(sizeof(Stanza));
    prima->x = 0;
    prima->y = 0;
    prima->stanza_sopra = NULL;
    prima->stanza_sotto = NULL;
    prima->stanza_sinistra = NULL;
    prima->stanza_destra = NULL;
    prima->tipo_stanza = rand() % 10;
    prima->tipo_trabocchetto =  rand() % 100 < 65 ? nessuno :
                                rand() % 100 < 75 ? tegola :
                                rand() % 100 < 84 ? lame :
                                rand() % 100 < 92 ? caduta : burrone;
    prima->tipo_tesoro =    rand() % 100 < 20 ? nessun_tesoro :
                            rand() % 100 < 40 ? verde_veleno :
                            rand() % 100 < 60 ? blu_guarigione :
                            rand() % 100 < 75 ? rosso_aumenta_vita :
                            rand() % 100 < 90 ? spada_tagliente : scudo;
    prima->nemico = nessun_nemico;

    pFirst = prima;
    pLast = prima;
    stanze[count++] = prima;
    numero_stanze++;

    while (count < 15) {
        Stanza* base = stanze[rand() % count];
        int dir = rand() % 4;
        int new_x = base->x, new_y = base->y;
        Stanza** direzione = NULL;

        switch (dir) {
            case 0:
                if (base->stanza_sopra) continue;
                new_y--;
                direzione = &base->stanza_sopra;
            break;

            case 1:
                if (base->stanza_sotto) continue;
                new_y++;
                direzione = &base->stanza_sotto;
            break;

            case 2:
                if (base->stanza_sinistra) continue;
                new_x--;
                direzione = &base->stanza_sinistra;
            break;

            case 3:
                if (base->stanza_destra) continue;
                new_x++;
                direzione = &base->stanza_destra;
            break;
        }
        if (posizione_occupata(new_x, new_y)) continue;

        Stanza* nuova = malloc(sizeof(Stanza));
        nuova->x = new_x;
        nuova->y = new_y;
        nuova->stanza_sopra = NULL;
        nuova->stanza_sotto = NULL;
        nuova->stanza_sinistra = NULL;
        nuova->stanza_destra = NULL;
        nuova->tipo_stanza = rand() % 10;
        nuova->tipo_trabocchetto =  rand() % 100 < 65 ? nessuno :
                                    rand() % 100 < 75 ? tegola :
                                    rand() % 100 < 84 ? lame :
                                    rand() % 100 < 92 ? caduta : burrone;
        nuova->tipo_tesoro =    rand() % 100 < 20 ? nessun_tesoro :
                                rand() % 100 < 40 ? verde_veleno :
                                rand() % 100 < 60 ? blu_guarigione :
                                rand() % 100 < 75 ? rosso_aumenta_vita :
                                rand() % 100 < 90 ? spada_tagliente : scudo;
        nuova->nemico = nessun_nemico;

        *direzione = nuova;

        // Collega ritorno
        switch (dir) {
            case 0: nuova->stanza_sotto = base; break;
            case 1: nuova->stanza_sopra = base; break;
            case 2: nuova->stanza_destra = base; break;
            case 3: nuova->stanza_sinistra = base; break;
        }

        stanze[count++] = nuova;
        pLast = nuova;
        numero_stanze++;
    }
    printf (GREEN"Mappa generata con successo!\n"RESET);
}

static void svuota_mappa() {
    if (!pFirst) return;

    Stanza* visitate[MAX_STANZE];
    int fine = 0;

    visitate[fine++] = pFirst;

    for (int i = 0; i < fine; i++) {
        Stanza* corrente = visitate[i];
        Stanza* vicine[4] = {
            corrente->stanza_destra,
            corrente->stanza_sinistra,
            corrente->stanza_sopra,
            corrente->stanza_sotto,
        };

        for (int j = 0; j < 4; j++) {
            if (!vicine[j]) continue;

            bool gia_visitata = false;
            for (int k = 0; k < fine; k++) {
                if (vicine[j] == visitate[k]) {
                    gia_visitata = true;
                    break;
                }
            }

            
            if (!gia_visitata && fine < MAX_STANZE) {
                visitate[fine++] = vicine[j];
            }
        }
    }

    // Libera le stanze al contrario
    for (int i = fine - 1; i >= 0; i--) {
        free(visitate[i]);
    }

    pFirst = NULL;
    pLast = NULL;
    numero_stanze = 0;
}


static void chiudi_mappa() {
    if (numero_stanze < 15) {
        printf(RED"Non puoi chiudere la mappa con meno di 15 stanze!\n"RESET);
        return;
    }
    mappa_chiusa = 1;
    printf("Mappa chiusa con successo!\n");
}

static void avanza(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf("Errore: giocatore o posizione non validi.\n");
        return;
    }

    Stanza* next = NULL;
    if (giocatore->posizione->stanza_destra) next = giocatore->posizione->stanza_destra;
    else if (giocatore->posizione->stanza_sinistra) next = giocatore->posizione->stanza_sinistra;
    else if (giocatore->posizione->stanza_sopra) next = giocatore->posizione->stanza_sopra;
    else if (giocatore->posizione->stanza_sotto) next = giocatore->posizione->stanza_sotto;

    if (next == NULL) {
        printf("Non ci sono stanze in cui poter avanzare.\n");
        return;
    }

    giocatore->posizione = next;
    printf("Sei avanzato nella stanza successiva\n");

    //attiva evetuale trabocchetto
    if (next->tipo_trabocchetto != nessuno) {
        printf("Hai attivato un trabocchetto!\n");
        switch (next->tipo_trabocchetto) {
            case tegola: 
                giocatore->p_vita--;
                printf("Una tegola ti ha colpito! -1 punti vita.\n"); 
            break;

            case lame:
                giocatore->p_vita -= 2;
                printf("Lame rotanti! -2 punti vita.\n");
            break;

            case caduta:
                giocatore->p_vita -= (rand() % 2) + 1;
                printf("Sei caduto! -%d punti vita\n", (rand() % 2) + 1);
            break;

            case burrone:
                giocatore->p_vita = 0;
                printf("Sei caduto in un burrone!\nGAME OVER\n");
            break;

            default:
            break;
        }
    }

    //probabilità del 25% di generare un nemico
    if (rand() % 100 < 25) {
        //nel caso sia l'ultima stanza
        if (next->stanza_destra == NULL && next->stanza_sinistra == NULL && next->stanza_sopra == NULL && next->stanza_sotto == NULL) {
            printf("Jaffar è apparso davanti a te!\n");
            next->nemico = Jaffar; // Assegna Jaffar come nemico
        } else {
            if(rand() % 100 < 60) {
                printf("Nemico Scheletro apparso!\n");
                next->nemico = scheletro;
            } else {
                printf("Nemico Guardia apparso!\n");
                next->nemico = guardia;
            }
        }
    }
}

static void combatti(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf ("ERRORE. Giocatore o posizione non valida.\n");
        return;
    } 

    if (giocatore->posizione->nemico == nessun_nemico) {
        printf("Non ci sono nemici da combattere.\n");
    }

    //controlliamo se c'è un nemico nella stanza
    unsigned short vita_nemico, attacco_nemico, difesa_nemico;

    switch (giocatore->posizione->nemico) {
        case scheletro:
            vita_nemico = 1;
            attacco_nemico = 1;
            difesa_nemico = 1;
        break;
        
        case guardia:
            vita_nemico = 2;
            attacco_nemico = 2;
            difesa_nemico = 2;
        break;

        case Jaffar:
            vita_nemico = 3;
            attacco_nemico = 3;
            difesa_nemico = 2;
        break;

        default:
        return;
    }

    printf("Inizia il combattimento.\n");

    //ciclo di combattimento
    while (giocatore->p_vita > 0 && vita_nemico > 0) {

        int iniziativa_giocatore = lancia_dado();
        int iniziativa_nemico = lancia_dado();

        if (iniziativa_giocatore >= iniziativa_nemico) {
            // Il giocatore attacca prima
            esegui_attacco(giocatore, &vita_nemico, difesa_nemico);
            if (vita_nemico > 0) {
                esegui_attacco_nemico(giocatore, attacco_nemico);
            } 
        } else {
            // Il nemico attacca prima
            esegui_attacco_nemico(giocatore, attacco_nemico);
            if (giocatore->p_vita > 0) {
                esegui_attacco(giocatore, &vita_nemico, difesa_nemico);
            }
        }

            printf ("Punti vita giocatore: %d, Punti vita nemico: %d\n", giocatore->p_vita, vita_nemico);
        }

        if (giocatore->p_vita > 0) {
            printf ("Hai vinto il combattimento! Guadagni 1 punto vita.\n");
            if (giocatore->p_vita < giocatore->p_vita_max) {
                giocatore->p_vita++;
        }
    } else {
        printf ("Sei stato sconfitto!\n");
    }
}

//calcolo lancio del dado
unsigned short lancia_dado() {
    time_t t;

    srand((unsigned) time(&t));

    unsigned short risultato = rand() % 6 + 1;
    printf("%d\n", risultato);
    return risultato;
}

static unsigned short calcola_colpi(unsigned short dadi_attacco) {
    int colpi = 0;
    for (int i = 0; i < dadi_attacco; i++) {
        int risultato = lancia_dado();
        if (risultato >= 4) colpi++;
        if (risultato == 6) colpi++; //critico d'attacco
    }
    return colpi;
}

static unsigned short calcola_parate(unsigned short dadi_difesa) {
    int parate = 0;
    for (int i = 0; i < dadi_difesa; i++) {
        int risultato = lancia_dado();
        if (risultato >= 4) parate++;
        if (risultato == 6) parate++; //critico di difesa
    }
    return parate;
}

static void esegui_attacco(Giocatore* giocatore, unsigned short* vita_nemico, unsigned short difesa_nemico) {
    int attacchi_giocatore = calcola_colpi(giocatore->dadi_attaco);
    int parate_nemico = calcola_parate(difesa_nemico);
    int danni = attacchi_giocatore - parate_nemico;
    if (danni > 0) {
        *vita_nemico -= danni;
        printf ("Hai inflitto %d danni al tuo nemico!\n", danni);
    } else {
        printf("Il nemico ha parato i tuoi colpi!\n");
    }

    if (*vita_nemico < 0) {
        *vita_nemico = 0;
    }
}

static void esegui_attacco_nemico(Giocatore* giocatore, unsigned short attacco_nemico) {
    int attacchi_nemico = calcola_colpi(attacco_nemico);
    int parate_giocatore = calcola_parate(giocatore->dadi_difesa);
    int danni = attacchi_nemico - parate_giocatore;
    if (danni >0) {
        giocatore->p_vita -= danni;
        printf("Il nemico ti ha inflitto %d danni!\n", danni);
    } else {
        printf("Hai parato i colpi del nemico!\n");
    }
}


static void scappa(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf("ERRORE! Giocatore o posizione non valida.\n");
        return;
    }

    //verifica se c'è un nemico
    if (!presenza_nemico(giocatore)) {
        printf("Non ci sono nemici da cui scappare.\n");
        return;
    }

    gestisci_scappatoia(giocatore);

    gestisci_trabocchetto(giocatore);

    ritorna_stanza_precedente(giocatore);
}

//Funzione per verificare se è presente un nemico nella stanza
static bool presenza_nemico(Giocatore* giocatore) {
    return giocatore->posizione->nemico != nessun_nemico;
}

//Funzione per gestire la logica della scappatoia in base al tipo di giocatore
static void gestisci_scappatoia(Giocatore* giocatore) {
    if (giocatore->classe_giocatore == principe) {
        //il Principe può scappare solo 1 volta
        if (giocatore->scappatoie_usate >= 1) {
            printf ("Hai già usato la tua scappatoia, non puoi più scappare!\n");
            return;
        }
        //aumentiamo il numero di scappatoie usate
        giocatore->scappatoie_usate++;
        printf("Sei riuscito a scappare dal combattimento!\n");
    }
    else if (giocatore->classe_giocatore == doppleganger) {
        //un Doppleganger può scappare 2 volte
        if (giocatore->scappatoie_usate >= 2) {
            printf("Hai già usato le tue scappatoie, non puoi più scappare!\n");
            return;
        }
        giocatore->scappatoie_usate++;
        printf("Sei riuscito a scappare dal combattimento!\n");
    }
}

//Funzione per gestire il trabocchetto per il Principe
static void gestisci_trabocchetto(Giocatore* giocatore) {
    if (giocatore->classe_giocatore == principe && giocatore->posizione->tipo_trabocchetto != nessuno) {
        printf ("Il principe ignora il primo trabocchetto.\n");
    }
}

//Funzione per tornare nella stanza precedente
static void ritorna_stanza_precedente(Giocatore* giocatore) {
    if(giocatore->posizione->stanza_destra != NULL) {
        giocatore->posizione = giocatore->posizione->stanza_destra;
    }
    else if(giocatore->posizione->stanza_sinistra != NULL) {
        giocatore->posizione = giocatore->posizione->stanza_sinistra;
    }
    else if(giocatore->posizione->stanza_sopra != NULL) {
        giocatore->posizione = giocatore->posizione->stanza_sopra;
    }
    else if(giocatore->posizione->stanza_sotto != NULL) {
        giocatore->posizione = giocatore->posizione->stanza_sotto;
    }
    printf("Sei tornato nella stanza precedente.\n");
}


static void stampa_giocatore(Giocatore* giocatore) {
    if (giocatore == NULL) {
        printf("ERRORE: Giocatore non valido!\n");
        return;
    }

    //Stampa la classe del giocatore
    printf("INFORMAZIONI GIOCATORE\n");
    printf("Classe giocatore: \n");
    switch (giocatore->classe_giocatore) {
        case principe:
            printf("Principe\n");
        break;
        case doppleganger:
            printf("Doppleganger\n");
        break;
        default:
            printf("Sconosciuta\n");
        break;
    }

    //Stampa i punti vita
    printf("Punti vita: %d/%d \n", giocatore->p_vita, giocatore->p_vita_max);

    //Stampa dadi attacco e difesa
    printf("Dadi attacco: %d\n", giocatore->dadi_attaco);
    printf("Dadi difesa: %d\n", giocatore->dadi_difesa);

    //Stampa quante volte il giocatore ha usato la scappatoia
    printf("Scappatoie usate: %d\n", giocatore->scappatoie_usate);

    //Stampa la posizione attuale
    if (giocatore->posizione != NULL) {
        printf("Posizione attuale: %p\n", (void*)giocatore->posizione);
    } else {
        printf("Posizione attuale: nessuna\n");
    }
}


static void stampa_zona(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf("ERRORE: Giocatore o posizione non valido\n");
        return;
    }

    Stanza* zona = giocatore->posizione;

    printf("=== STATO DELLA STANZA ===\n");
    stampa_collegamenti(zona);
    stampa_pericoli(zona);
    stampa_tesori(zona);
    printf("==========================\n");
}

static void stampa_collegamenti(Stanza* zona) {
    printf("Stanze adiacenti: \n");
    if (zona->stanza_destra != NULL) printf("- Destra.\n");
    if (zona->stanza_sinistra != NULL) printf("- Sinistra.\n");
    if (zona->stanza_sopra != NULL) printf("- Sopra.\n");
    if (zona->stanza_sotto != NULL) printf("- Sotto.\n");

}

static void stampa_pericoli(Stanza* zona) {
    if(zona->nemico != nessun_nemico) {
        printf("ATTENZIONE! C'è un nemico nella stanza: \n");

        //Stampa il tipo di nemico
        switch(zona->nemico) {
            case scheletro:
                printf("uno scheletro.\n");
            break;
            case guardia:
                printf("una guardia.\n");
            break;
            case Jaffar:
                printf("Jaffar.\n");
            break;
            default:
            break;
        }
    } else {
        printf ("La stanza è sicura, non ci sono nemici.\n");
    }

    if (zona->tipo_trabocchetto != nessuno) {
        printf("ATTENZIONE! Potrebbe esserci un trabocchetto.\n");
    }
}

static void stampa_tesori(Stanza* zona) {
    if (zona->tipo_tesoro != nessun_tesoro) {
        printf("C'è un tesoro nella stanza!\n");
    }
}


static void prendi_tesoro(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf("ERRORE! Giocatore o posizione non valida.\n");
        return;
    }

    Stanza* zona = giocatore->posizione;
    if (zona->tipo_tesoro == nessun_tesoro) {
        printf("Qui non c'è nessun tesoro.\n");
        return;
    }

    printf("Hai trovato un tesoro!\n");
    applica_effetto_tesoro(giocatore, zona->tipo_tesoro);
    zona->tipo_tesoro = nessun_tesoro; //Il tesoro viene consumeto e non sarà più utilizzabile
}

static void applica_effetto_tesoro(Giocatore* giocatore, Tipo_tesoro tesoro) {
    switch (tesoro) {
        case verde_veleno:
            printf("Oh no! Era veleno! Perdi 1 punto vita.\n");
            if (giocatore->p_vita > 0) giocatore->p_vita--;
        break;

        case blu_guarigione:
            printf("Ti sei rigenerato! Guadangi 1 punto vita.\n");
            if (giocatore->p_vita < giocatore->p_vita_max) giocatore->p_vita++;
        break;

        case rosso_aumenta_vita:
            printf("Senti come cresce la tua energia! Aumenti il massimo dei tuoi punti vita.\n");
            giocatore->p_vita_max++;
            giocatore->p_vita = giocatore->p_vita_max;
        break;

        case spada_tagliente:
            printf("Hai trovato una spada affilata! I tuoi attacchi sono più forti.\n");
            giocatore->dadi_attaco++;
        break;

        case scudo:
            printf("Uno scudo robusto! La tua difesa migliora.\n");
            giocatore->dadi_difesa++;
        break;

        default:
        break; 
    }
}

// Funzione principale per cercare la porta segreta
static void cerca_porta_segreta(Giocatore* giocatore) {
    if (giocatore == NULL || giocatore->posizione == NULL) {
        printf("ERRORE! Giocatore o stanza non validia.\n");
        return;
    }

    // Se il giocatore è già avanzato non può cercare una porta segreta
    if (giocatore->posizione == giocatore->posizione->stanza_destra ||
        giocatore->posizione == giocatore->posizione->stanza_sinistra ||
        giocatore->posizione == giocatore->posizione->stanza_sopra ||
        giocatore->posizione == giocatore->posizione->stanza_sotto) {
            printf ("Non puoi cercare la porta segreta dopo aver avanzato.\n");
            return;
        }

        // Verifica se c'è la possibilità di cercare una porta segreta
        if (esiste_porta_segreta()) {
            // Determina la direzione in cui il giocatore può cercare la porta
            if (giocatore->posizione->stanza_destra == NULL) {
                printf("Cercando una porta segreta a destra...\n");
                Stanza* nuova_stanza = crea_stanza_segreta();
                collega_stanza_segreta(giocatore->posizione, nuova_stanza, "destra");
                giocatore->posizione = nuova_stanza;
            }
            else if (giocatore->posizione->stanza_sinistra == NULL) {
                printf("Cercando una porta a segreta a sinistra...\n");
                Stanza* nuova_stanza = crea_stanza_segreta();
                collega_stanza_segreta(giocatore->posizione, nuova_stanza, "sinistra");
                giocatore->posizione = nuova_stanza;
            }
            else if (giocatore->posizione->stanza_sopra == NULL) {
                printf("Cercando una porta segreta sopra...\n");
                Stanza* nuova_stanza = crea_stanza_segreta();
                collega_stanza_segreta(giocatore->posizione, nuova_stanza, "Sopra");
                giocatore->posizione = nuova_stanza;
            }
            else if (giocatore->posizione->stanza_sotto == NULL) {
                printf("Cercando una porta segreta sotto...\n");
                Stanza* nuova_stanza = crea_stanza_segreta();
                collega_stanza_segreta(giocatore->posizione, nuova_stanza, "Sotto");
                giocatore->posizione = nuova_stanza;
            }

            // Il giocatore torna alla stanza di partenza
            printf("Tornando indietro alla stanza precedente...\n");
            if (giocatore->posizione->stanza_destra != NULL) {
                giocatore->posizione = giocatore->posizione->stanza_destra;
            }
            else if (giocatore->posizione->stanza_sinistra != NULL) {
                giocatore->posizione = giocatore->posizione->stanza_sinistra;
            }
            else if (giocatore->posizione->stanza_sopra != NULL) {
                giocatore->posizione = giocatore->posizione->stanza_sopra;
            }
            else if (giocatore->posizione->stanza_sotto != NULL) {
                giocatore->posizione = giocatore->posizione->stanza_sotto;
            }
        } else {
            printf("Nessuna porta segreta trovata.\n");
        }
}

static bool esiste_porta_segreta() {
    return (rand() % 3 == 0); //33% di probabilità
}

//Funzione per creare una stanza segreta con valori casuali
static Stanza* crea_stanza_segreta() {
    Stanza* nuova_stanza = malloc(sizeof(Stanza));
    if (nuova_stanza == NULL) {
        printf("Errore nell'allocazione della nuova stanza");
        return NULL;
    }

    //Genera valori casuali per il tipo della stanza
    nuova_stanza->tipo_stanza = rand() % 10; // 10 possibili tipi di stanza
    nuova_stanza->tipo_tesoro = rand() % 5; // 5 possibili tipi di tesoro
    nuova_stanza->tipo_trabocchetto = rand() % 6; // 6 possibili tipi di trabocchetto
    nuova_stanza->nemico = nessun_nemico; // Non ci sono nemici nelle stanze segrete
    nuova_stanza->stanza_destra = NULL;
    nuova_stanza->stanza_sinistra = NULL;
    nuova_stanza->stanza_sopra = NULL;
    nuova_stanza->stanza_sotto = NULL;

    return nuova_stanza;
}

// Funzione per collegare la stanza segreta alla stanza di partenza
static void collega_stanza_segreta(Stanza* stanza_originale, Stanza* stanza_segreta, const char* direzione) {
    if (strcmp(direzione, "destra") == 0) {
        stanza_originale->stanza_destra = stanza_segreta;
        stanza_segreta->stanza_sinistra = stanza_originale;
    }
    else if (strcmp(direzione, "sinistra") == 0) {
        stanza_originale->stanza_sinistra = stanza_segreta;
        stanza_segreta->stanza_destra = stanza_originale;
    }
    else if (strcmp(direzione, "sopra") == 0) {
        stanza_originale->stanza_sopra = stanza_segreta;
        stanza_segreta->stanza_sotto = stanza_originale;
    }
    else if (strcmp(direzione, "sotto") == 0) {
        stanza_originale->stanza_sotto = stanza_segreta;
        stanza_segreta->stanza_sopra = stanza_originale;
    }
}

static void passa(int num_giocatori) {
    if (num_giocatori <= 0) {
        printf ("ERRORE! Numero di giocatori non valido.\n");
        return;
    }

    // Passa al prossimo giocatore
    turno_corrente = (turno_corrente + 1) % num_giocatori;
    printf ("E' il turno del giocatore %d.\n", turno_corrente + 1);
}
