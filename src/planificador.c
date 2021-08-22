#include "../include/planificador.h"
#include <time.h>
#include <math.h>
int rand_between(int minimum_number, int max_number);
void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla);
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla);

Planificador *crear_planificador(int n_clientes_vip, int n_clientes_no_vip, int n_taxistas, unsigned tamanio_grilla, double z_distance, double u_segundos)
{
  Planificador *planificador = (Planificador *)malloc(sizeof(Planificador));
  planificador->is_waiting_x_turnos = false;
  planificador->n_clientes_vip = n_clientes_vip;
  planificador->n_clientes_no_vip = n_clientes_no_vip;
  planificador->n_taxistas = n_taxistas;
  planificador->tamanio_grilla = tamanio_grilla;
  planificador->z_distance = z_distance;
  planificador->u_segundos = u_segundos;
  planificador->clientes_vip = crear_queue(sizeof(Cliente));
  planificador->clientes_no_vip = crear_queue(sizeof(Cliente));
  planificador->taxistas = crear_queue(sizeof(Taxista));
  planificador->clientes_prioritarios = crear_queue(sizeof(Cliente));
  aumentar_clientes(planificador, n_clientes_vip, n_clientes_no_vip);
  crear_taxistas(planificador, n_taxistas, tamanio_grilla);

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
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla)
{
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
void aumentar_turnos(Queue *clientes,bool is_prioritario)
{
  Node *temp = clientes->head;
  while (temp != NULL)
  {
    ((Cliente *)(temp->data))->x_turnos_espera++;
    temp = temp->next;
  }
}
void planificar(Planificador *planificador)
{
}
Cliente *choose_queue_cliente(Planificador *planificador)
{
  float weigths[] = {0, 65, 35};
  if (planificador->is_waiting_x_turnos)
  {
    weigths[0] = 50;
    weigths[1] = 32.5;
    weigths[2] = 17.5;
  }
  int r = rand_between(1, 101);

  if (r <= weigths[0] && planificador->is_waiting_x_turnos)
  {
    return dequeue(planificador->clientes_vip);
  }
  else if (r <= weigths[1] && !planificador->is_waiting_x_turnos)
  {
    return dequeue(planificador->clientes_vip);
  }
  else
  {
    return dequeue(planificador->clientes_no_vip);
  }
}

Taxista *choose_queue_taxista(Planificador *planificador, Cliente *cliente)
{
  Node *temp = planificador->taxistas->head;
  while (temp != NULL)
  {
    if (((Taxista *)(temp->data))->is_available)
    {
      {
        int pos_x_taxista = ((Taxista *)(temp->data))->pos_x;
        int pos_y_taxista = ((Taxista *)(temp->data))->pos_y;
        int pos_x_inicial_cliente = cliente->px_inicial;
        int pos_y_inicial_cliente = cliente->py_inicial;
        int calculo = sqrt(pow(pos_x_taxista - pos_x_inicial_cliente, 2) + pow(pos_y_taxista - pos_y_inicial_cliente, 2));
        if (calculo <= planificador->z_distance)
        {
          return (Taxista *)(temp->data);
        }
      }
    }
    temp = temp->next;
  }
  return NULL;
}
int tiempo_sleep(Taxista *taxista, Cliente *cliente, Planificador *planificador)
{
  int calculo = round(sqrt(pow(taxista->pos_x - cliente->px_inicial, 2) + pow(taxista->pos_y - cliente->py_inicial, 2))) * planificador->u_segundos;
  return calculo;
}