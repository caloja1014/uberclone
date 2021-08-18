#include "../include/planificador.h"
#include <time.h>
int rand_between(int minimum_number, int max_number);
void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla);
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla);

Planificador *crear_planificador(int n_clientes_vip, int n_clientes_no_vip, int n_taxistas, unsigned tamanio_grilla)
{
  Planificador *planificador = (Planificador *)malloc(sizeof(Planificador));
  planificador->n_clientes_vip = n_clientes_vip;
  planificador->n_clientes_no_vip = n_clientes_no_vip;
  planificador->n_taxistas = n_taxistas;
  planificador->tamanio_grilla = tamanio_grilla;
  planificador->clientes_vip = crear_queue(sizeof(Cliente));
  planificador->clientes_no_vip = crear_queue(sizeof(Cliente));
  planificador->taxistas = crear_queue(sizeof(Taxista));
  planificador->clientes_prioritarios = crear_queue(sizeof(Cliente));
  aumentar_clientes(planificador,n_clientes_vip,n_clientes_no_vip);
  crear_taxistas(planificador,n_taxistas,tamanio_grilla);

  return planificador;
}

void aumentar_clientes(Planificador *planificador, int n_clientes_vip, int n_clientes_no_vip)
{
  planificador->n_clientes_vip += n_clientes_vip;
  planificador->n_clientes_no_vip += n_clientes_no_vip;
  int tamanio_grilla = planificador->tamanio_grilla;
  crear_clientes(planificador->clientes_vip, planificador->n_clientes_vip, tamanio_grilla);
  crear_clientes(planificador->clientes_no_vip, planificador->n_clientes_no_vip, tamanio_grilla);
}

void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla)
{
  Queue *cola = (Queue *)queue_cliente;
  for (int i = 0; i < n_cliente; i++)
  {
    int px_inicial = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int py_inicial = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int px_final = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int py_final = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    Cliente *cliente = crear_cliente(true, 0, px_inicial, px_final, py_inicial, py_final);
    enqueue(queue_cliente, cliente);
  }
}
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla){
  Queue *cola = (Queue *)queue_taxista;
  for (int i = 0; i < n_taxistas; i++)
  {
    int pos_x = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int pos_y = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    Taxista *taxista = crear_taxista(pos_x, pos_y);
    enqueue(queue_taxista, taxista);
    }
}
int rand_between(int minimum_number, int max_number)
{
  srand(time(0));
  return rand() % (max_number + 1 - minimum_number) + minimum_number;
}
void aumentar_turnos(Queue *clientes){
    Node *temp=clientes->head;
    while (temp!=NULL)
    {
      ((Cliente *)(temp->data))->x_turnos_espera++;
      temp=temp->next;
    }
    
    
      
}
