#ifndef TAXISTA
#define TAXISTA
#include <stdlib.h>
#include <stdbool.h>

typedef struct taxista_struct
{
    int id;
    int pos_x;
    int pos_y;
    bool is_free;

}  Taxista;

Taxista * crear_taxista( int pos_x, int pos_y);

#endif /*TAXISTA */