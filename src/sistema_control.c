#include "../include/common.h"
#include "../include/queue.h"
#include "../include/taxista.h"
#include "../include/cliente.h"
#include <pthread.h>

#define MAX_BUF_SIZE 1024

volatile terminar;

char **parse_comand(char *line, char *delim);
void *receive_information();

void print_help(char *command)
{

    printf("Sensor\n");
    printf("Use:\n %s", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tHelp, show this message\n");
}

int main(int argc, char const *argv[])
{

    Queue *queue = crear_queue(sizeof(Taxista));

    Taxista *t1 = crear_taxista( 1, 1);
    Taxista *t2 = crear_taxista( 2, 2);
    enqueue(queue, t1);
    enqueue(queue, t2);

    // pid_t pid;
    // pthread_create(&pid, NULL, receive_information, NULL);
    receive_information();

    return 0;
}

char **parse_comand(char *linea, char *delim)
{
    char *token;
    char *linea_copy;
    int i, num_tokens = 0;
    char **argv = NULL;
    char *saveptr = NULL;

    linea_copy = (char *)malloc(strlen(linea) + 1);
    strcpy(linea_copy, linea);

    /* Obtiene un conteo del número de argumentos */
    token = strtok_r(linea_copy, delim, &saveptr);
    /* recorre todos los tokens */
    while (token != NULL)
    {
        token = strtok_r(NULL, delim, &saveptr);
        num_tokens++;
    }
    free(linea_copy);
    if (num_tokens > 0)
    {
        /* Crea el arreglo argv */
        argv = (char **)malloc((num_tokens + 1) * sizeof(char **));
        /* obtiene el primer token */
        token = strtok_r(linea, delim, &saveptr);
        /* recorre todos los tokens */
        for (i = 0; i < num_tokens; i++)
        {
            argv[i] = (char *)malloc(strlen(token) + 1);
            strcpy(argv[i], token);
            token = strtok_r(NULL, delim, &saveptr);
        }
        argv[i] = NULL;
    }
    return argv;
}

void *receive_information()
{
    printf("here\n");
    pthread_detach(pthread_self());
    char *numbers = "numbers";
    char buf[MAX_BUF_SIZE];

    mkfifo(numbers, 0666);
    int fd, noread;
    while (1)
    {
        memset(buf, 0, MAX_BUF_SIZE);
        fd = open(numbers, O_RDONLY);
        noread = read(fd, buf, sizeof(buf));
        buf[noread] = '\0';
        printf("%s\n", buf);
        close(fd);
        char **numbers_recieve = parse_comand(buf, ",");
        int nvip = atoi(numbers_recieve[0]);
        int nnvip = atoi(numbers_recieve[1]);
        printf("Received: %d - %d\n", nvip, nnvip);
    }
    close(fd);
}