#include "gamelib.h"


int main() {
    system("clear");
    unsigned short scelta = 0;
    char otitle[] = "*---------------------*";
    char title[] = "|  PRINCE OF INERTIA  |";

    printf(YELLOW);
    for (int i = 0; otitle[i] != '\0'; i++) {
        printf("%c", otitle[i]);
        fflush(stdout);
        usleep(50000); //ritardo di 100ms tra i caratteri
    }
    printf(RESET"\n");

    printf(YELLOW);
    for (int i = 0; title[i] != '\0'; i++) {
        printf("%c", title[i]);
        fflush(stdout);
        usleep(100000); //ritardo di 100ms tra i caratteri
    }
    printf(RESET"\n");

    printf(YELLOW);
    for (int i = 0; otitle[i] != '\0'; i++) {
        printf("%c", otitle[i]);
        fflush(stdout);
        usleep(50000); //ritardo di 100ms tra i caratteri
    }
    printf(RESET"\n");

    do {
        printf(YELLOW"Premere INVIO per continuare\n"RESET);
    } while (getchar() != '\n');
    system ("clear");
    do {
        printf("\n");
        if (settato == false)
        printf(BLUE"Menù Principale\n"RESET);
        printf(YELLOW"1)Imposta gioco\n");
        printf("2)Gioca\n");
        printf("3)Termina gioco\n");
        printf("4)Visualizza i crediti\n"RESET);
        printf(BLUE"Scegli un opzione: \n"RESET);
        scanf("%hu", &scelta);

        switch (scelta)
        {
        case 1:
            imposta_gioco();
            break;

        case 2:
            gioca();
            break;

        case 3:
            termina_gioco();
            break;

        case 4:
            crediti();
            break;
        
        default:
            printf(RED"ERRORE! COMANDO NON VALIDO\n"RESET);
            printf(RED"RIPROVARE"RESET);
            break;
        }
    }
    while (scelta != 3);

}
