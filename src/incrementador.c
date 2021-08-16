
#include "../include/common.h"
#define MAX_BUFFER_SIZE 1020
void print_help(char *command)
{

    printf("Incrementador\n");
    printf("Use:\n %s <#VIP,#NVIP>", command);
    printf(" %s -h\n", command);
    printf("Opciones:\n");
    printf(" -h\t\t\tHelp, show this message\n");
}
int main(int argc, char const *argv[])
{
    int opt, fd;
    char *numbers = "numbers";
    char *cantidad_clientes;
    // mknod(numbers,S_IFIFO|0666,0);
    // mkfifo(numbers, 0666);
    fd = open("numbers", O_WRONLY);
    while ((opt = getopt(argc, argv, "h:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            print_help(argv[0]);
            break;
        case '?':
        default:
            fprintf(stderr, "Use:\n %s <#VIP,#NVIP>", argv[0]);
            fprintf(stderr, "\t\t\t\t%s -h\n", argv[0]);
            break;
        }
    }
    int contador = 0;
    int index = 0;
    for (index = optind; index < argc; index++)
        contador++;

    if (contador != 1)
    {
        fprintf(stderr, "Use:\n %s <#VIP,#NVIP>", argv[0]);
        fprintf(stderr, "\t\t\t\t%s -h\n", argv[0]);
        return 1;
    }
    else
    {
        cantidad_clientes = argv[optind];
    }
    printf("%s\n", cantidad_clientes);
    char dato[MAX_BUFFER_SIZE];
    memset(dato, 0, MAX_BUFFER_SIZE);
    strcat(dato, cantidad_clientes);
    
    write(fd, dato, sizeof(dato));
    close(fd);
    return 0;
}
