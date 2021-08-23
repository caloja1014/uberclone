#ifndef PLANIFICADOR
#define PLANIFICADOR
#include <time.h>
#include <math.h>
#include "../include/queue.h"
#include "../include/taxista.h"
#include "../include/cliente.h"
#include <unistd.h>

typedef struct planificador_struct{
    int n_clientes_vip;
    int n_clientes_no_vip;
    int n_taxistas;
    unsigned tamanio_grilla;
    double z_distance;
    double u_segundos;
    bool is_waiting_x_turnos;
    Queue *clientes_no_vip;
    Queue *clientes_vip;
    Queue *taxistas;
    Queue *clientes_prioritarios;
} Planificador;
typedef struct tuple_struct{
  Queue *cola;
  Node *nodo;
}Tupla;

Planificador * crear_planificador(int n_clientes_vip, int n_clientes_no_vip, int n_taxistas, unsigned tamanio_grilla, double z_distance,double u_segundos);
void aumentar_clientes(Planificador *planificador, int n_clientes_vip, int n_clientes_no_vip);
void atender_cliente (Planificador *planificador, Cliente *cliente);
bool seleccionar_taxista(Planificador *planificador, Cliente *cliente);
void aumentar_turnos(Queue * clientes,bool is_prioritario);
void *planificar(void *plan);
Tupla *choose_queue_cliente(Planificador *planificador);
Node *choose_queue_taxista(Planificador *planificador , Cliente *cliente);
int tiempo_sleep(Taxista *taxista, Cliente *cliente, Planificador *planificador);
#endif /*PLANIFICADOR */