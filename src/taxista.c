#include "../include/taxista.h"

Taxista * crear_taxista( int pos_x, int pos_y,int id){
    
    Taxista *taxista = (Taxista *) malloc(sizeof(Taxista));
    taxista->id = id;
    taxista->pos_x = pos_x;
    taxista->pos_y = pos_y;
    taxista->is_available = true;
    return taxista;
}