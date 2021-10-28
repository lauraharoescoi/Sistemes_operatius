/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: calculador_v1.c
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

#define STRLONG 1024

char *color_green = "\033[01;32m";
char *color_end = "\033[00m";

typedef struct
{
    long long terme_1;
    long long terme_n;
} t_rang;

int main(int argc, char *argv[])
{
    long int i;
    long long sum = 0, sumForm, terme_i, terme_i_mes_1;
    char *nomPrograma;
    char cadena[STRLONG];
    t_rang total;
    nomPrograma = argv[0];

    while (read(11, &total, sizeof(t_rang)) != 0){
        if (argc != 1)
        {
            printf("Us: %s \n\n", argv[0]);
                exit(1);
        }

        sprintf(cadena, "%s%s - %d> Calcula progressio termes: %.0lld -> %.0lld %s\n", color_green, nomPrograma, getpid(), total.terme_1, total.terme_n, color_end);
        write(1, cadena, strlen(cadena));

        terme_i = total.terme_1;
        terme_i_mes_1 = terme_i + 1;
        sum = terme_i;

        for (i = total.terme_1; i < total.terme_n; i++)
        {
            sum = sum + terme_i_mes_1;
            terme_i = terme_i_mes_1;
            terme_i_mes_1 = terme_i_mes_1 + 1;
        }

        sumForm = ((total.terme_1 + total.terme_n) / 2.0) * (total.terme_n + 1 - total.terme_1);

        sprintf(cadena, "%s%s - %d> Suma amb bucle=%.0lld - Suma amb formula=%.0lld%s\n", color_green, nomPrograma, getpid(), sum, sumForm, color_end);
        write(1, cadena, strlen(cadena));

        write(20, &sum, sizeof(int));
    }

    close(11);
    close(20);
    return EXIT_SUCCESS;
}