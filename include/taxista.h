#ifndef TAXISTA
#define TAXISTA
#include <stdlib.h>

typedef struct taxista_struct
{
    int id;
    int pos_x;
    int pos_y;

}  Taxista;

Taxista * crear_taxista(int id, int pos_x, int pos_y);

#endif /*TAXISTA */