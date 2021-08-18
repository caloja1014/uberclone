#include "../include/taxista.h"

Taxista * crear_taxista( int pos_x, int pos_y){
    Taxista *taxista = (Taxista *) malloc(sizeof(Taxista));
    taxista->id = 1;
    taxista->pos_x = pos_x;
    taxista->pos_y = pos_y;
    return taxista;
}