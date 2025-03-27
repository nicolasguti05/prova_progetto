#include "gamelib.h"

unsigned short durata_intervallo = 2;


static Stanza* pFirst = NULL; //puntatore alla prima stanza della mappa
static Stanza* pLast = NULL;
static int turno_corrente = 0; // Variabile globale per tenere traccia del turno corrente

static Giocatore* giocatori[3] = {0,0,0}; //array di 3 giocatori(NULL se non partecipano)

//funzioni per imposta_gioco()
static void creazione_giocatori();
static void menu_impostazione_mappa();
static void ins_stanza();
static void canc_stanza();
static void stampa_stanze();
static void genera_random();
static void chiudi_mappa();
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

static unsigned short calcola_colpi(dadi_attacco);
static unsigned short calcola_parate(dadi_difesa);
static void esegui_attacco(Giocatore* giocatore, unsigned short* vita_nemico, unsigned short difesa_nemico);
static void esegui_attacco_nemico(Giocatore* giocatore, unsigned short attacco_nemico);
unsigned short lancia_dado();
static bool presenza_nemico(Giocatore* giocatore);
static void gestisci_scappatoia(Giocatore* giocatore);
static void gestisci_trabocchetto(Giocatore*giocatore);
static void ritorna_stanza_precedente(Giocatore* giocatore);
static void stampa_collegamenti(Stanza* zona);
static void stampa_pericoli(Stanza* zona);
static void stampa_tesori(Stanza* zona);
static void applica_effetto_tesoro(Giocatore* giocatore, Tipo_tesoro tesoro);
static bool esiste_stanza_segreta();
static Stanza* crea_stanza_segreta();
static void collega_stanza_segreta(Stanza* stanza_originale, Stanza* stanza_segreta, const char* direzione);




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
            printf("Turno del giocatore %d (%s)\n", i + 1, (giocatore->classe_giocatore == principe) ? "Principe" : "Doppleganger");

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
                        stampa_zona(giocatore->posizione);
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
    
    printf ("Inserisci il numero di giocatori (da 1 a 3): ");
    int num_giocatori;
    scanf ("%d", &num_giocatori);
    while (num_giocatori < 1 || num_giocatori > 3) {
        printf (RED"Numero di giocatori non valido.\n"RESET);
        sleep (durata_intervallo);
        printf ("Inserire un numero di giocatori valido: ");
        scanf ("%d", &num_giocatori);
    }

    bool prince = false;
    for (int i = 0; i < num_giocatori; i++) {
        giocatori[i] = malloc(sizeof(Giocatore));
        if (!giocatori[i]) {
            printf(RED"Errore di allocazione memoria!"RESET);
            exit(1);
        }

        printf ("Inserisci il nome del giocatore %d: ", i + 1);
        scanf("%s", giocatori[i]->nome_giocatore);

        if(!prince) {
            printf ("Il primo giocatore deve essere un Principe.\n");
            giocatori[i]->classe_giocatore = principe;
            prince = true;
        } else {
            printf("Il giocatore sarà un Doppleganger.\n"), i +1;
            giocatori[i]->classe_giocatore = doppleganger;
        }

        giocatori[i]->p_vita_max = 3;
        giocatori[i]->p_vita = 3;
        giocatori[i]->dadi_attaco = 2;
        giocatori[i]->dadi_difesa = 2;
        giocatori[i]->posizione = NULL;
    }

    if (!principe) {
        printf(RED"ERROE: Deve esserci almeno un principe per giocare!\n"RESET);
        return;
    }
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
        printf("6) Esci \n");
        printf("Scelta: ");
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
            break;
        case 6: return;

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

    printf ("Inserire tipo stanza (1-10): ");
    scanf ("%d", (int*)nuova_stanza->tipo_stanza);
    printf ("Inserire tipo trabocchetto: (1-5): ");
    scanf ("%d", (int*)nuova_stanza->tipo_trabocchetto);
    printf ("Inserire tipo tesoro (1-6): ");
    scanf ("%d", (int*)nuova_stanza->tipo_tesoro);

    nuova_stanza->stanza_sopra = NULL;
    nuova_stanza->stanza_sotto = NULL;
    nuova_stanza->stanza_sinistra = NULL;
    nuova_stanza->stanza_destra = NULL;

    if (!pFirst) {
        pFirst = nuova_stanza;
    } else {
        int scelta;
        printf ("Dove collegare la stanza? \n(1=sopra, 2=detra, 3=sinistra, 4=sotto): ");
        scanf ("%d", &scelta);

        switch (scelta) {
            case 1:
                pLast->stanza_sopra = nuova_stanza;
                break;
            case 2:
                pLast->stanza_destra = nuova_stanza;
                break;
            case 3:
                pLast->stanza_sinistra = nuova_stanza;
                break;
            case 4:
                pLast->stanza_sotto = nuova_stanza;
                break;
            default:
                printf ("Scelta non valida! Stanza non collegata.\n");
                free(nuova_stanza);
                return;
        }
    }

    pLast = nuova_stanza;
    numero_stanze++;
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
    Stanza* temp = pFirst;
    while (temp) {
        printf ("Stanza: Tipo = %d, trabocchetto = %d, tesoro = %d\n", temp->tipo_stanza, temp->tipo_trabocchetto, temp->tipo_tesoro);
        temp = temp->stanza_sopra ? temp->stanza_sopra : temp->stanza_destra ? temp->stanza_destra : temp->stanza_sinistra ? temp->stanza_sinistra : temp->stanza_sotto;
    }
}

static void genera_random() {
    while (pFirst) {
        canc_stanza();
    }

    srand(time(NULL));
    for (int i = 0; i < 15; i++) {
        Stanza* nuova_stanza = (Stanza*)malloc(sizeof(Stanza));
        nuova_stanza->tipo_stanza = rand() % 10;
        
        int prob_trabocchetto = rand() % 100;
        if(prob_trabocchetto < 65) nuova_stanza->tipo_trabocchetto = 0;
        else if(prob_trabocchetto < 75) nuova_stanza->tipo_trabocchetto = 1;
        else if (prob_trabocchetto < 84) nuova_stanza->tipo_trabocchetto = 2;
        else if (prob_trabocchetto < 92) nuova_stanza->tipo_trabocchetto = 3;
        else nuova_stanza->tipo_trabocchetto = 4;

        int prob_tesoro = rand() %100;
        if (prob_tesoro < 20) nuova_stanza->tipo_tesoro = 0;
        else if (prob_tesoro < 40) nuova_stanza->tipo_tesoro = 1;
        else if (prob_tesoro < 60) nuova_stanza->tipo_tesoro = 2;
        else if (prob_tesoro < 75) nuova_stanza->tipo_tesoro = 3;
        else if (prob_tesoro < 90) nuova_stanza->tipo_tesoro = 4;
        else nuova_stanza->tipo_tesoro = 5;

        if (!pFirst) pFirst = nuova_stanza;
        else pLast->stanza_destra = nuova_stanza;
        pLast = nuova_stanza;
        numero_stanze++;
    }
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
    printf("Sei avanzato nella stanza successiva");

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
        } else {
            if(rand() % 100 < 60) {
                printf("Nemico Scheletro apparso!\n");
            } else {
                printf("Nemico Guardia apparso!\n");
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
            esegui_attacco(giocatore, &vita_nemico, difesa_nemico);
            if (vita_nemico > 0) {
                esegui_attacco_nemico(giocatore, attacco_nemico);
            } else {
                esegui_attacco_nemico(giocatore, attacco_nemico);
                if (giocatore->p_vita > 0) {
                    esegui_attacco(giocatore, &vita_nemico, difesa_nemico);
                }
            }
        }

        if (giocatore->p_vita > 0) {
            printf ("Hai vinto il combattimento! Guadagni 1 punto vita.\n");
            if (giocatore->p_vita < giocatore->p_vita_max) {
                giocatore->p_vita++;
            }
        } else {
            printf("Sei stato sconfitto!\n");
        }
    }
}

//calcolo lancio del dado
unsigned short lancia_dado() {
    time_t t;

    srand((unsigned) time(&t));

    printf("%d\n", rand() % 7);
}

static unsigned short calcola_colpi(dadi_attacco) {
    int colpi = 0;
    for (int i = 0; i < dadi_attacco; i++) {
        int risultato = lancia_dado();
        if (risultato >= 4) colpi++;
        if (risultato == 6) colpi++; //critico d'attacco
    }
    return colpi;
}

static unsigned short clcola_parate(dadi_difesa) {
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
}

static void esegui_attacco_nemico(Giocatore* giocatore, unsigned short attacco_nemico) {
    int attacchi_nemico = calcola_colpi(attacco_nemico);
    int parate_giocatore = calcola_parate(giocatore->dadi_difesa);
    int danni = attacchi_nemico - parate_giocatore;
    if (danni >0) {
        giocatore->p_vita -= danni;
        printf("Il nemico ti ha inflitto %d danni!\n");
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
        return;
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
