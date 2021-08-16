#ifndef PLANIFICADOR
#define PLANIFICADOR
#include "../include/queue.h"
#include "../include/taxista.h"
#include "../include/cliente.h"
typedef struct planificador_struct{
    int n_clientes_vip;
    int n_clientes_no_vip;
    int n_taxistas;
    Queue *clientes_no_vip;
    Queue *clientes_vip;
    Queue *taxistas;
    Queue *clientes_prioritarios;
} Planificador;

Planificador * crear_planificador(int n_clientes_vip, int n_clientes_no_vip, int n_taxistas);
void aumentar_clientes(Planificador *planificador, int n_clientes_vip, int n_clientes_no_vip);
void atender_cliente (Planificador *planificador, Cliente *cliente);
bool seleccionar_taxista(Planificador *planificador, Cliente *cliente);

#endif /*PLANIFICADOR */