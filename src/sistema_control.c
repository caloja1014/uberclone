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
    int fd,noread;
    char *numbers = "numbers";
    char buf[MAX_BUF_SIZE];

    mkfifo(numbers, 0666);

    while (1)
    {
        memset(buf, 0, MAX_BUF_SIZE);
        fd = open(numbers, O_RDONLY);
        noread=read(fd, buf, sizeof(buf));
        buf[noread] = '\0';
        printf("%s\n", buf);
        close(fd);
        
    }

    printf("Received: %s\n", buf);
    close(fd);
    return 0;
}
