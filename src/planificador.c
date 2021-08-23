#include "../include/planificador.h"

int rand_between(int minimum_number, int max_number);
void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla);
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla);
void sleep_thread_taxista(void *arg);
void sleep_thread_cliente(void *arg);
typedef struct tuple_sleep_thread
{
  Taxista *taxista;
  int tiempo_sleep;
} tuple_sleep;
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
  crear_taxistas(planificador->taxistas, n_taxistas, tamanio_grilla);

  return planificador;
}

void aumentar_clientes(Planificador *planificador, int n_clientes_vip, int n_clientes_no_vip)
{
  planificador->n_clientes_vip += n_clientes_vip;
  planificador->n_clientes_no_vip += n_clientes_no_vip;
  int tamanio_grilla = planificador->tamanio_grilla;
  crear_clientes(planificador->clientes_vip, n_clientes_vip, tamanio_grilla);
  crear_clientes(planificador->clientes_no_vip, n_clientes_no_vip, tamanio_grilla);
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
    printf("Cliente %d: (%d, %d) -> (%d, %d)\n", i, px_inicial, py_inicial, px_final, py_final);
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
    printf("Pos Taxis %d %d\n", pos_x, pos_y);
    Taxista *taxista = crear_taxista(pos_x, pos_y);
    enqueue(queue_taxista, taxista);
  }
}
int rand_between(int minimum_number, int max_number)
{
  // srand(time(NULL));
  rand();
  return rand() % (max_number + 1 - minimum_number) + minimum_number;
}
void aumentar_turnos(Queue *clientes, bool is_prioritario)
{
  Node *temp = clientes->head;
  while (temp != NULL)
  {
    ((Cliente *)(temp->data))->x_turnos_espera++;
    temp = temp->next;
  }
}
void *planificar(void *plan)
{
  Planificador *planificador = (Planificador *)plan;

  Queue *taxistas = planificador->taxistas;

  while (1)
  {
    Tupla *respuesta = choose_queue_cliente(planificador);
    Node *cliente = respuesta->nodo;
    Queue *clientes = respuesta->cola;
    if (cliente != NULL)
    {
      if (taxistas->no_ocuped > 0)
      {
        Node *taxista = choose_queue_taxista(planificador, (Cliente *)(cliente->data));

        if (taxista == NULL)
        {
          Node *previo_actual_cliente;
          Node *futuro_siguiente_actual_cliente;

          if (clientes->size_queue == 0 || clientes->size_queue == 1)
          {
            cliente->destroy = true;
            pthread_mutex_lock(&cliente->lock);
            pthread_mutex_destroy(&cliente->lock);
            pthread_cond_destroy(&cliente->notify);
            // pthread_cancel(cliente->thread_id);
            enqueue(clientes, cliente->data);
          }
          else if (clientes->size_queue == 2)
          {
            Node *temp1 = dequeue(clientes);
            Node *temp2 = dequeue(clientes);
            temp1->destroy = true;
            temp2->destroy = true;
            pthread_mutex_destroy(&temp1->lock);
            pthread_cond_destroy(&temp1->notify);
            pthread_mutex_destroy(&temp2->lock);
            pthread_cond_destroy(&temp2->notify);

            // pthread_cancel(temp1->thread_id);
            // pthread_cancel(temp2->thread_id);
            enqueue(clientes, temp1->data);
            enqueue(clientes, cliente->data);
            enqueue(clientes, temp2->data);
          }
          else
          {
            futuro_siguiente_actual_cliente = cliente->next->next;
            previo_actual_cliente = cliente->prev;

            if (previo_actual_cliente == NULL)
            {
              cliente->next->prev = NULL;
              clientes->head = cliente->next;
            }
            else
            {

              previo_actual_cliente->next = cliente->next;
              cliente->next->prev = previo_actual_cliente;
            }
            cliente->next->next = cliente;
            cliente->prev = cliente->next;
            cliente->next = futuro_siguiente_actual_cliente;
            if (futuro_siguiente_actual_cliente != NULL)
            {
              futuro_siguiente_actual_cliente->prev = cliente;
            }
            clientes->size_queue++;
            clientes->no_ocuped++;
          }
        }
        else
        {
          ((Taxista *)(taxista->data))->is_available = false;
          int time_sleep = tiempo_sleep((Taxista *)(taxista->data), (Cliente *)(cliente->data), planificador);
          tuple_sleep *tuple_s = (tuple_sleep *)malloc(sizeof(tuple_sleep));
          tuple_s->taxista = (Taxista *)(taxista->data);
          tuple_s->tiempo_sleep = time_sleep;

          taxista->task->argument = tuple_s;
          taxista->task->function = &sleep_thread_taxista;
          cliente->task->argument = time_sleep;
          cliente->task->function = &sleep_thread_cliente;
          pthread_cond_signal(&taxista->notify);
          pthread_cond_signal(&cliente->notify);
          taxistas->no_ocuped--;
          taxista->is_running = true;
          cliente->is_running = true;
        }
      }
    }
  }
  pthread_detach(pthread_self());
}
void sleep_thread_taxista(void *arg)
{
  tuple_sleep *tuple = (tuple_sleep *)arg;
  int time_sleep = tuple->tiempo_sleep;
  printf("INICIO sleep taxi %d\n", time_sleep);
  sleep(time_sleep);

  printf("FIN DORMIR taxi\n");
  tuple->taxista->is_available = true;
  // pthread_detach(pthread_self());
  // pthread_cancel(pthread_self());
}
void sleep_thread_cliente(void *arg)
{
  int time_sleep = (int)arg;
  printf("INICIO sleep cliente %d\n", time_sleep);
  sleep(time_sleep);

  printf("FIN DORMIR cliente\n");
  // pthread_detach(pthread_self());
  pthread_cancel(pthread_self());
}
Tupla *choose_queue_cliente(Planificador *planificador)
{
  float weigths[] = {0, 65, 35};
  Tupla *respuesta = (Tupla *)malloc(sizeof(Tupla));
  if (planificador->is_waiting_x_turnos)
  {
    weigths[0] = 50;
    weigths[1] = 32.5;
    weigths[2] = 17.5;
  }
  int r = rand_between(1, 101);

  if (r <= weigths[0] && planificador->is_waiting_x_turnos)
  {
    respuesta->cola = planificador->clientes_prioritarios;
    respuesta->nodo = dequeue(planificador->clientes_prioritarios);
    return respuesta;
  }
  else if (r <= weigths[1] && !planificador->is_waiting_x_turnos)
  {
    respuesta->cola = planificador->clientes_vip;
    respuesta->nodo = dequeue(planificador->clientes_vip);
    return respuesta;
  }
  else
  {
    respuesta->cola = planificador->clientes_no_vip;
    respuesta->nodo = dequeue(planificador->clientes_no_vip);
    return respuesta;
  }
}

Node *choose_queue_taxista(Planificador *planificador, Cliente *cliente)
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
          return temp;
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