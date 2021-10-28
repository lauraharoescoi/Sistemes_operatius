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

#include <sys/wait.h>

#include <string.h>

#define STRLONG 1024

char *color_red = "\033[01;31m";
char *color_blue = "\033[01;34m";
char *color_end = "\033[00m";

int main(int argc, char *argv[])
{
    long int i;
    int sumTotal = 0;
    char *args[] = {"./calculador_v3", "./calculador_v3"};
    pid_t pid;
    int estatWait;
    char *nomPrograma;
    char cadena[STRLONG];
    int pipeSumes[2];
    int pipeRangs[2];
    int result;

    typedef struct
    {
        long long terme_1;
        long long terme_n;
    } t_rang;

    if (argc != 2)
    {
        printf("Us: %s <nombre_calculadors>\n\n", argv[0]);
        exit(1);
    }


    nomPrograma = argv[0];
    unsigned int numCalculadors = atoi(argv[1]);

    if (pipe(pipeSumes)<0)
    {
        perror("Padre :: Error creacuión pipe");
        exit(1);
    }

    if (pipe(pipeRangs)<0)
    {
        perror("Padre :: Error creacuión pipe");
        exit(1);
    }
    
    dup2(pipeSumes[0], 21);
    dup2(pipeSumes[1],20);

    close(pipeSumes[0]);
    close(pipeSumes[1]);


    for (i = 0; i < numCalculadors; i++)
    {

        pid = fork();
        if(pid == -1){
            sprintf(cadena, "%s%s - %d> Fill %ld error fork%s", color_red, nomPrograma, getpid(), i + 1, color_end);
            perror(cadena);
            exit(3);
        }
        else if (pid == 0) /* Fill */
        {
            close(21);
            dup2(pipeRangs[0], 11);
            close(pipeRangs[0]);
            close(pipeRangs[1]);
            execl(args[0], args[1], NULL);
        }
        else 
        {

            sprintf(cadena, "%s%s - %d> Crea fill %ld [%d] %s\n", color_blue, nomPrograma, getpid(), i + 1, pid, color_end);
            write(1, cadena, strlen(cadena));
        }
    }

    t_rang total;
    int rep;

    dup2(pipeRangs[1], 10);
    close(11);
    close(pipeRangs[1]);
    close(pipeRangs[0]);

    do{
        do{
            printf("\n%s%s - %d> ---------- INTRODUCCIO TERMES RANG ----------\n\n",color_blue, nomPrograma,getpid());
            printf("%s%s - %d> Introdueix el 1r terme de la progressio: %s",color_blue, nomPrograma,getpid(),color_end);
            scanf("%lld", &total.terme_1);
            printf("%s%s - %d> Introdueix el darrer terme de la progressio: %s",color_blue, nomPrograma,getpid(),color_end);
            scanf("%lld", &total.terme_n);
            if(((total.terme_n - total.terme_1) + 1) % numCalculadors != 0){
                printf("%s%s - %d>	ERROR! Rang de valors no divisible entre el nombre de processos calculadors.\n%s",color_blue, nomPrograma,getpid(),color_end);
            }
        } while(((total.terme_n - total.terme_1) + 1) % numCalculadors != 0);


        long long termesPerCalculador = (total.terme_n - total.terme_1 + 1) / numCalculadors;
        t_rang particio;
        
        for(i = 0; i < numCalculadors; i ++){
            particio.terme_1 = i * termesPerCalculador + total.terme_1;


            particio.terme_n = (i + 1) * termesPerCalculador + total.terme_1 - 1;


            write(10, &particio, sizeof(t_rang));
        }

        for (i = 0; i < numCalculadors; i++){
            read(21, &result, sizeof(int));
            sumTotal += result;
        }

        long long int sumForm = ((total.terme_1 + total.terme_n) / 2.0) * (total.terme_n + 1 - total.terme_1);

        sprintf(cadena, "%s%s - %d> *************** Suma formula %s = %.0lld | %i = Suma rebuda dels calculadors ***************%s\n", color_blue, nomPrograma, getpid(), nomPrograma, sumForm, sumTotal, color_end);
        write(1, cadena, strlen(cadena));

        do{
            printf("\n\n%s%s - %d> Vols entrar en un nou rang [Si=1, No=0]? %s",color_blue, nomPrograma, getpid(), color_end);
            scanf("%i", &rep);
        }while(rep <0 || rep >1);

        sumTotal = 0;
    }   while(rep == 1);

    close(10);

    for (i = 0; i < numCalculadors; i++)
    {    
        pid = wait(&estatWait);
        sprintf(cadena, "%s%s - %d> Fill %i finalitzat amb estat %i.\n%s", color_blue, nomPrograma, getpid(), pid, estatWait, color_end);
        write(1, cadena, strlen(cadena));
    }

    close(21);
    return EXIT_SUCCESS;
}