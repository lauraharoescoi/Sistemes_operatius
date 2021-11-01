/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: controlador_v3.c
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

int sumesReader = 21;
int sumesWriter = 20;

int rangsReader = 11;
int rangsWriter = 10;

typedef struct {
    long long terme_1;
    long long terme_n;
} t_rang;

char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

int main(int argc, char *argv[]) {
    long int i;
    long long sumTotal = 0;         //variable que acumula el valor de la suma parcial de cada calculador
    char *args[] = {"./calculador_v3", "./calculador_v3"};
    pid_t pid;
    char *nomPrograma;
    char cadena[STRLONG];
    int estatWait;
    int pipeSumes[2];       //pipe per a rebre els càlculs dels fills
    int pipeRangs[2];       //pipe per a enviar els rangs als fills
    long long result;       //variable on guardarem la informació rebuda a través de la pipe
    t_rang total;
    int rep;        //variable per indicar si repetim el procés

    if (argc != 2) {
        printf("Us: %s <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }

    nomPrograma = argv[0];
    unsigned int numCalculadors = atoi(argv[1]);

    if (pipe(pipeSumes) < 0) {
        perror("Pare :: Error creació pipe");
        exit(2);
    }

    if (pipe(pipeRangs) < 0) {
        perror("Pare :: Error creació pipe");
        exit(3);
    }

    for (i = 0; i < numCalculadors; i++) {
        pid = fork();

        if (pid == -1){     //Error
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(4);

        } else if (pid == 0) {     //Fill
            dup2(pipeSumes[1], sumesWriter);         //Copiem l'extrem d'escriptura a sumesWriter (20)
            close(pipeSumes[0]);
            close(pipeSumes[1]);

            dup2(pipeRangs[0], rangsReader);         //Copiem l'extrem de lectura a rangsReader (11)
            close(pipeRangs[0]);
            close(pipeRangs[1]);

            execl(args[0], args[1], NULL);

        } else {
            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    dup2(pipeSumes[0], sumesReader);        //Copiem l'extrem de lectura a sumerReader (21)
    close(pipeSumes[0]);
    close(pipeSumes[1]);

    dup2(pipeRangs[1], rangsWriter);        //Copiem l'extrem d'escriptura a rangsWriter (10)
    close(pipeRangs[0]);
    close(pipeRangs[1]);

    do {
        do {
            printf("\n%s%s - %d> ---------- INTRODUCCIO TERMES RANG ----------\n\n",color_blue, nomPrograma,getpid());
            printf("%s%s - %d> Introdueix el 1r terme de la progressio: %s",color_blue, nomPrograma,getpid(),color_end);
            scanf("%lld", &total.terme_1);
            printf("%s%s - %d> Introdueix el darrer terme de la progressio: %s",color_blue, nomPrograma,getpid(),color_end);
            scanf("%lld", &total.terme_n);

            if (((total.terme_n - total.terme_1) + 1) % numCalculadors != 0) {
                printf("%s%s - %d>	ERROR! Rang de valors no divisible entre el nombre de processos calculadors.\n%s",color_blue, nomPrograma,getpid(),color_end);
            }

        } while (((total.terme_n - total.terme_1) + 1) % numCalculadors != 0);

        long long termesPerCalculador = (total.terme_n - total.terme_1 + 1) / numCalculadors;
        t_rang particio;
        
        for(i = 0; i < numCalculadors; i ++) {
            particio.terme_1 = i * termesPerCalculador + total.terme_1;
            particio.terme_n = (i + 1) * termesPerCalculador + total.terme_1 - 1;

            if (write(rangsWriter, &particio, sizeof(t_rang)) < 0) {        //Enviem els rangs a calcular als fills i comprovem possibles errors
                perror("Error");
                exit(-1);
            }
        }

        for (i = 0; i < numCalculadors; i++) {
            read(sumesReader, &result, sizeof(long long));      //Guardem a la variable result les sumes parcials dels calculadors
            sumTotal += result;
        }

        long long int sumForm = ((total.terme_1 + total.terme_n) / 2.0) * (total.terme_n + 1 - total.terme_1);

        sprintf(cadena, "%s%s - %d> *************** Suma formula %s = %.0lld | %.0lld = Suma rebuda dels calculadors ***************%s\n", color_blue, nomPrograma, getpid(), nomPrograma, sumForm, sumTotal, color_end);
        write(1, cadena, strlen(cadena));

        do {
            printf("\n\n%s%s - %d> Vols entrar en un nou rang [Si=1, No=0]? %s",color_blue, nomPrograma, getpid(), color_end);
            scanf("%i", &rep);

        } while(rep < 0 || rep > 1);

        sumTotal = 0; 

    } while (rep == 1);

    close(rangsWriter);

    for (i = 0; i < numCalculadors; i++) {    
        pid = wait(&estatWait);

        sprintf(cadena, "%s%s - %d> Fill %i finalitzat amb estat %i.\n%s", color_blue, nomPrograma, getpid(), pid, estatWait, color_end);
        write(1, cadena, strlen(cadena));
    }

    close(sumesReader);
    return EXIT_SUCCESS;
}