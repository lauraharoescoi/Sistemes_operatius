/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: controlador_v2.c
 Laura Haro Escoi
 Jonàs Salat Torres
 ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define STRLONG 1024

char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

int sumesReader = 21;
int sumesWriter = 20;

int main(int argc, char *argv[]) {
    long int i;
    long long sumTotal = 0;         //variable que acumula el valor de la suma parcial de cada calculador
    char *args[] = {"./calculador_v2", "./calculador_v2", "1", "20"};
    pid_t pid;
    char *nomPrograma;
    char cadena[STRLONG];
    int pipeSumes[2];           //pipe per a connectar el pare amb el fill
    long long result;           //variable on guardarem la informació rebuda a través de la pipe

    if (argc != 4) {
        printf("Us: %s <1r terme> <darrer terme> <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }

    if ((atoll(argv[2]) - atoll(argv[1]) + 1) % atoll(argv[3]) != 0) {
        printf("Rang de valors no divisible entre el nombre de processos calculadors.\n");
        exit(2);
    }

    nomPrograma = argv[0];
    long long terme_1 = atoll(argv[1]);
    long long terme_n = atoll(argv[2]);
    unsigned int numCalculadors = atoi(argv[3]);

    long long termesPerCalculador = (terme_n - terme_1 + 1) / numCalculadors;

    if (pipe(pipeSumes) < 0) {
        perror("Pare :: Error creació pipe");
        exit(3);
    }

    for (i = 0; i < numCalculadors; i++) {
        long double terme_1_Calculador = i * termesPerCalculador + terme_1;
        char terme_1_Calculador_Str[STRLONG];
        sprintf(terme_1_Calculador_Str, "%.0Lf", terme_1_Calculador);

        long double terme_n_Calculador = (i + 1) * termesPerCalculador + terme_1 - 1;
        char terme_n_Calculador_Str[STRLONG];
        sprintf(terme_n_Calculador_Str, "%.0Lf", terme_n_Calculador);

        pid = fork();
        if(pid == -1) {         //Error
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(4);

        } else if (pid == 0) {        //Fill
            dup2(pipeSumes[1], sumesWriter);        //Copiem l'extrem d'escriptura a sumesWriter (20)
            close(pipeSumes[0]);
            close(pipeSumes[1]);

            execl(args[0], args[1], terme_1_Calculador_Str, terme_n_Calculador_Str, NULL);     

        } else {
            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] Termes: %.0Lf -> %.0Lf %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, terme_1_Calculador, terme_n_Calculador, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    dup2(pipeSumes[0], sumesReader);        //Copiem l'extrem de lectura a sumesReader (21)
    close(pipeSumes[0]);
    close(pipeSumes[1]);

    for (i = 0; i < numCalculadors; i++) {
        pid = wait(NULL);       //ja no ens importa el valor enviat a través del return del fill

        if (pid == -1) {
                sprintf(cadena, "%s%s - %d> No existeixen fills.%s\n", color_blue, nomPrograma, getpid(), color_end);
                write(1, cadena, strlen(cadena));
                exit(5);
        }

        sprintf(cadena, "%s%s - %d> Fill %d finalitzat%s.\n\n", color_blue, nomPrograma, getpid(), pid, color_end);
        write(1, cadena, strlen(cadena));

        read(sumesReader, &result, sizeof(long long));      //obtenim el valor de cada calculador dins la variable result
        sumTotal += result;
    }

    close(sumesReader);
    
    long long int sumForm = ((terme_1 + terme_n) / 2.0) * (terme_n + 1 - terme_1);

    sprintf(cadena, "%s%s - %d> Suma amb caluladors=%.0llu - Suma amb formula=%.0lld%s\n", color_blue, nomPrograma, getpid(), sumTotal, sumForm, color_end);
    write(1, cadena, strlen(cadena));


    return EXIT_SUCCESS;
}