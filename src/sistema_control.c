#include "../include/common.h"
#define MAX_BUF_SIZE 1024
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
    int fd;
    char * numeros="numbers";
    char buf[MAX_BUF_SIZE] ;

    fd=open(numeros,O_RDONLY);
    printf("Received: %s\n", buf);
    close(fd);
    return 0;
}
