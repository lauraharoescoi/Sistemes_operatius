/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: calculador_v4.c
 Laura Haro Escoi
 Jonàs Salat Torres
 ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define STRLONG 1024

char *color_green = "\033[01;32m";
char *color_end = "\033[00m";

char *nomPrograma;

int sumesWriter = 20;
int rangsReader = 11;

typedef struct {
    long long terme_1;
    long long terme_n;
} t_rang;

/* Funció per a gestionar l'enviament de la senyal SIGTERM que fa el pare*/

void gestio_SIGTERM(int sig) {      
    printf("%s%s - %i> gestioSIGTERM. Sortint ...\n", color_green, nomPrograma, getpid());
    close(rangsReader);
    close(sumesWriter);
    exit(0);
}

int main(int argc, char *argv[]) {
    long int i;
    long long sum = 0, sumForm, terme_i, terme_i_mes_1;
    char cadena[STRLONG];
    t_rang total;
    nomPrograma = argv[0];

    signal(SIGINT, SIG_IGN);        //fem que el calculador ignori la senyal SIGINT

    if (signal(SIGTERM, gestio_SIGTERM) == SIG_ERR) {       //gestió d'errors de la senyal SIGTERM
        perror("Signal");
        exit(-1);
    }

    while (read(rangsReader, &total, sizeof(t_rang)) > 0) {
        sprintf(cadena, "%s%s - %d> Calcula progressio termes: %.0lld -> %.0lld %s\n", color_green, nomPrograma, getpid(), total.terme_1, total.terme_n, color_end);
        write(1, cadena, strlen(cadena));

        terme_i = total.terme_1;
        terme_i_mes_1 = terme_i + 1;
        sum = terme_i;

        for (i = total.terme_1; i < total.terme_n; i++) {
            sum = sum + terme_i_mes_1;
            terme_i = terme_i_mes_1;
            terme_i_mes_1 = terme_i_mes_1 + 1;
        }

        sumForm = ((total.terme_1 + total.terme_n) / 2.0) * (total.terme_n + 1 - total.terme_1);

        sprintf(cadena, "%s%s - %d> Suma amb bucle=%.0lld - Suma amb formula=%.0lld%s\n", color_green, nomPrograma, getpid(), sum, sumForm, color_end);
        write(1, cadena, strlen(cadena));

        if (write(sumesWriter, &sum, sizeof(long long)) < 0) {        //Enviem al pare el càlcul parcial i comprovem que no hi hagi cap error
            perror("Error");
            exit(-1);
        }
    }
    close(rangsReader);
    close(sumesWriter);
    return EXIT_SUCCESS;
}