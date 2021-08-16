#ifndef TAXISTA
#define TAXISTA

struct taxista_struct
{
    int id;
    int pos_x;
    int pos_y;

} typedef Taxista;

Taxista * crear_taxista(int id, int pos_x, int pos_y);