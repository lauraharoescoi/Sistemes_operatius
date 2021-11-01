/* -----------------------------------------------------------------------
 PRA1: Suma seqüència d’enters
 Codi font: controlador_v4.c
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

unsigned int numCalculadors;
int* pidsV;         //creem un array dinàmic per a guardar els pid's dels fills
char *nomPrograma;
char cadena[STRLONG];
pid_t pid;
int estatWait;

int sumesReader = 21;
int sumesWriter = 20;

int rangsReader = 11;
int rangsWriter = 10;

/*Funció que executa els waits per a gestionar els fills que han acabat*/

void finalitzar_fills() {
    for (int i = 0; i < numCalculadors; i++) {    
        pid = wait(NULL);
        sprintf(cadena, "%s%s - %d> Fill %i finalitzat amb estat %i.\n%s", color_blue, nomPrograma, getpid(), pid, estatWait, color_end);
        write(1, cadena, strlen(cadena));
    }
    close(sumesReader);
}

/*Funció que gestiona la senyal SIGINT que fem al terminal amb Ctrl+C*/

void gestor_sigint(int sig) {
    printf("\n%s%s - %i> gestioSOGINT: \n%s", color_blue, nomPrograma, getpid(), color_end);

    for (int i = 0; i < numCalculadors; i++) {
        printf("%s%s - %i> Enviant SIGTERM al fill %i - %i ...\n%s", color_blue, nomPrograma, getpid(), i +1, pidsV[i], color_end);
        kill(pidsV[i], SIGTERM);        //Enviem la senyal SIGTERM als fills
    }
    finalitzar_fills();
    close(rangsWriter);
    exit(1);
}

int main(int argc, char *argv[]) {
    long int i;
    long long sumTotal = 0;
    char *args[] = {"./calculador_v4", "./calculador_v4"};
    int pipeSumes[2];       //pipe per a rebre els càlculs dels fills
    int pipeRangs[2];       //pipe per a enviar els rangs als fills
    long long result;         //variable on guardarem la informació rebuda a través de la pipe

    typedef struct {
        long long terme_1;
        long long terme_n;
    } t_rang;

    if (argc != 2) {
        printf("Us: %s <nombre_calculadors>\n\n", argv[0]);
        exit(2);
    }

    nomPrograma = argv[0];
    numCalculadors = atoi(argv[1]);
    pidsV = malloc(numCalculadors * sizeof(int));       //definim la capacitat que tindrà l'array dinàmic

    if (pipe(pipeSumes) < 0) {
        perror("Pare :: Error creació pipe");
        exit(3);
    }

    if (pipe(pipeRangs) < 0) {
        perror("Pare :: Error creació pipe");
        exit(4);
    }

    for (i = 0; i < numCalculadors; i++) {
        pid = fork();
        pidsV[i] = pid;         //Guardem el pid del fill a l'array de pids

        if (pid == -1) {        //Error
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(5);

        } else if (pid == 0) {      //Fill
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

    t_rang total;
    int rep;

    dup2(pipeSumes[0], sumesReader);        //Copiem l'extrem de lectura a sumerReader (21)
    close(pipeSumes[0]);
    close(pipeSumes[1]);

    dup2(pipeRangs[1], rangsWriter);        //Copiem l'extrem d'escriptura a rangsWriter (10)
    close(pipeRangs[0]);
    close(pipeRangs[1]);

    if (signal(SIGINT, gestor_sigint) == SIG_ERR) {         //Gestió d'errors de la senyal SIGINT
        perror("Signal");
        exit(-1);
    }
    
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
        
        for (i = 0; i < numCalculadors; i ++) {
            particio.terme_1 = i * termesPerCalculador + total.terme_1;
            particio.terme_n = (i + 1) * termesPerCalculador + total.terme_1 - 1;

            if (write(rangsWriter, &particio, sizeof(t_rang)) < 0) {        //Enviem els rangs a calcular als fills i comprovem possibles errors
                perror("Error");
                exit(-1);
            }
        }

        for (i = 0; i < numCalculadors; i++) {
            read(sumesReader, &result, sizeof(long long));        //Guardem a la variable result les sumes parcials dels calculadors
            sumTotal += result;
        }

        long long int sumForm = ((total.terme_1 + total.terme_n) / 2.0) * (total.terme_n + 1 - total.terme_1);

        sprintf(cadena, "%s%s - %d> *************** Suma formula %s = %.0lld | %.0lld = Suma rebuda dels calculadors ***************%s\n", color_blue, nomPrograma, getpid(), nomPrograma, sumForm, sumTotal, color_end);
        write(1, cadena, strlen(cadena));

        do {
            printf("\n\n%s%s - %d> Vols entrar en un nou rang [Si=1, No=0]? %s",color_blue, nomPrograma, getpid(), color_end);
            scanf("%i", &rep);
        } while(rep <0 || rep >1);

        sumTotal = 0;

    } while(rep == 1);

    close(rangsWriter);
    finalitzar_fills();
    return EXIT_SUCCESS;
}