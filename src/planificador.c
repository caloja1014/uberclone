#include "../include/planificador.h"

int rand_between(int minimum_number, int max_number);
void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla, bool isvip);
void crear_taxistas(void *queue_taxista, int n_taxistas, unsigned tamanio_grilla);
void sleep_thread_taxista(void *arg);
void sleep_thread_cliente(void *arg);
int prioritarios_no_vip = 0;
int prioritarios_vip = 0;
Tupla *respuesta;
pthread_mutex_t lock_taxista;
typedef struct tuple_sleep_thread
{
  void *usuario;
  Cliente *cliente;
  int tiempo_sleep;
  Queue *queue;
} tuple_sleep;
Planificador *crear_planificador(int n_clientes_vip, int n_clientes_no_vip, int n_taxistas, unsigned tamanio_grilla, double z_distance, double u_segundos, int x_turnos)
{
  pthread_mutex_init(&lock_taxista, NULL);
  respuesta = (Tupla *)malloc(sizeof(Tupla));
  Planificador *planificador = (Planificador *)malloc(sizeof(Planificador));
  planificador->is_waiting_x_turnos = false;
  planificador->n_clientes_vip = 0;
  planificador->n_clientes_no_vip = 0;
  planificador->n_taxistas = n_taxistas;
  planificador->tamanio_grilla = tamanio_grilla;
  planificador->z_distance = z_distance;
  planificador->u_segundos = u_segundos;
  planificador->x_turnos = x_turnos;
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
  crear_clientes(planificador->clientes_vip, n_clientes_vip, tamanio_grilla, true);
  crear_clientes(planificador->clientes_no_vip, n_clientes_no_vip, tamanio_grilla, false);
}

void crear_clientes(void *queue_cliente, int n_cliente, int tamanio_grilla, bool isvip)
{
  Queue *cola = (Queue *)queue_cliente;
  for (int i = 0; i < n_cliente; i++)
  {
    int px_inicial = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int py_inicial = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int px_final = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    int py_final = rand_between(-1 * tamanio_grilla, tamanio_grilla);
    cola->last_id++;
    Cliente *cliente = crear_cliente(isvip, 0, px_inicial, px_final, py_inicial, py_final, cola->last_id);
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
    Taxista *taxista = crear_taxista(pos_x, pos_y, i + 1);
    enqueue(queue_taxista, taxista);
  }
}
int rand_between(int minimum_number, int max_number)
{
  rand();
  return rand() % (max_number + 1 - minimum_number) + minimum_number;
}
void aumentar_turnos(Queue *clientes, Planificador *planificador)
{
  Queue *clientes_prioritarios = planificador->clientes_prioritarios;
  int x_turnos = planificador->x_turnos;
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
    if (taxistas->no_ocuped > 0)
    {
      Tupla *respuesta = choose_queue_cliente(planificador);
      Node *cliente = respuesta->nodo;
      Queue *clientes = respuesta->cola;
      if (cliente != NULL)
      {
        if (((Cliente *)(cliente->data))->isPriorizado && clientes->size_queue == 0)
        {
          planificador->is_waiting_x_turnos = false;
        }
        if (((Cliente *)(cliente->data))->x_turnos_espera >= planificador->x_turnos && !((Cliente *)(cliente->data))->isPriorizado)
        {

          if (((Cliente *)(cliente->data))->isvip)
          {
            prioritarios_vip++;
          }
          else
          {
            prioritarios_no_vip++;
          }
          Queue *clientes_prioritarios = planificador->clientes_prioritarios;
          pthread_mutex_lock(&clientes_prioritarios->mutex_dequeue);
          ((Cliente *)(cliente->data))->isPriorizado = true;
          char *t_vip = ((Cliente *)(cliente->data))->isvip ? "VIP" : "NO VIP";
          int id_cliente = ((Cliente *)(cliente->data))->id;

          if (clientes_prioritarios->size_queue == 0)
          {
            cliente->prev = cliente->next = NULL;
            clientes_prioritarios->head = clientes_prioritarios->tail = cliente;
          }
          else
          {
            clientes_prioritarios->tail->next = cliente;
            cliente->prev = clientes_prioritarios->tail;
            cliente->next = NULL;
            clientes_prioritarios->tail = cliente;
          }

          clientes_prioritarios->size_queue++;
          clientes_prioritarios->no_ocuped++;

          planificador->is_waiting_x_turnos = true;
          pthread_mutex_lock(&clientes->mutex_dequeue);
          aumentar_turnos(clientes, planificador);
          pthread_mutex_unlock(&clientes->mutex_dequeue);
          pthread_mutex_unlock(&clientes_prioritarios->mutex_dequeue);
        }

        else
        {
          Node *taxista = choose_queue_taxista(planificador, (Cliente *)(cliente->data));
          if (taxista == NULL)
          {

            Node *previo_actual_cliente;
            Node *futuro_siguiente_actual_cliente;

            if (clientes->size_queue == 0)
            {
              clientes->head = clientes->tail = cliente;
              cliente->next = cliente->prev = NULL;
              clientes->size_queue++;
              clientes->no_ocuped++;
            }

            else if (clientes->size_queue == 1)
            {
              clientes->head->next = cliente;
              cliente->prev = clientes->head;
              clientes->tail = cliente;
              cliente->next = NULL;
              clientes->size_queue++;
              clientes->no_ocuped++;
            }
            else if (clientes->size_queue >= 2)
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
            pthread_mutex_lock(&clientes->mutex_dequeue);
            aumentar_turnos(clientes, planificador);
            pthread_mutex_unlock(&clientes->mutex_dequeue);
            // pthread_mutex_lock(&lock_taxista);
            ((Taxista *)(taxista->data))->is_available = false;
            // printf("CAMBIO TAXI %d a %d DESHABILITADO\n", ((Taxista *)(taxista->data))->id, ((Taxista *)(taxista->data))->is_available);
            // pthread_mutex_unlock(&lock_taxista);
            int time_sleep = tiempo_sleep((Taxista *)(taxista->data), (Cliente *)(cliente->data), planificador);
            // printf("TIEMPO SLEEP%d\n\n",time_sleep);
            tuple_sleep *tuple_s_taxi = (tuple_sleep *)malloc(sizeof(tuple_sleep));
            tuple_sleep *tuple_s_cliente = (tuple_sleep *)malloc(sizeof(tuple_sleep));
            tuple_s_taxi->queue = taxistas;
            tuple_s_taxi->usuario = (void *)(taxista);

            tuple_s_taxi->cliente = (Cliente *)(cliente->data);

            tuple_s_cliente->queue = clientes;
            tuple_s_cliente->usuario = (void *)(cliente);
            tuple_s_cliente->cliente = (Cliente *)(cliente->data);

            tuple_s_taxi->tiempo_sleep = tuple_s_cliente->tiempo_sleep = time_sleep;

            taxista->task->argument = tuple_s_taxi;
            taxista->task->function = &sleep_thread_taxista;
            cliente->task->argument = tuple_s_cliente;
            cliente->task->function = &sleep_thread_cliente;
            taxistas->no_ocuped--;
            clientes->no_ocuped--;
            taxista->is_running = true;
            cliente->is_running = true;
            pthread_cond_signal(&taxista->notify);
            pthread_cond_signal(&cliente->notify);
          }
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
  Queue *taxistas = tuple->queue;
  Taxista *taxista = (Taxista *)(((Node *)(tuple->usuario))->data);
  Cliente *cliente = tuple->cliente;
  // printf("INICIO TAXI %d a %d\n", taxista->id, taxista->is_available);
  char *text_v = cliente->isvip ? "VIP" : "NO VIP\0";
  printf("Taxis:       libres %d, ocupados %d\n", taxistas->no_ocuped, taxistas->size_queue - taxistas->no_ocuped);
  printf("Taxi %d asignado a cliente %s %d, tiempo de viajes esperado %d seg\n", taxista->id, text_v, cliente->id, time_sleep);
  sleep(time_sleep);

  taxista->is_available = true;

  taxista->pos_x = cliente->px_final;
  taxista->pos_y = cliente->py_final;
  taxistas->no_ocuped++;
  printf("Taxis:       libres %d, ocupados %d\n", taxistas->no_ocuped, taxistas->size_queue - taxistas->no_ocuped);
  // pthread_detach(pthread_self());
  // pthread_mutex_unlock(&lock_taxista);
}
void sleep_thread_cliente(void *arg)
{
  tuple_sleep *tuple = (tuple_sleep *)arg;
  int time_sleep = tuple->tiempo_sleep;
  Queue *clientes = tuple->queue;
  Cliente *cliente = (Cliente *)(((Node *)(tuple->usuario))->data);
  char *text_v = cliente->isvip ? "VIP" : "NO VIP\0";
  int cantidad_prioritarios = cliente->isvip ? prioritarios_vip : prioritarios_no_vip;
  clientes->cantidad_atendidos++;
  printf("Clientes %s siendo atendidos %d, en espera normal %d, priorizados %d\n", text_v, clientes->cantidad_atendidos, clientes->no_ocuped, cantidad_prioritarios);
  sleep(time_sleep);
  clientes->cantidad_atendidos--;

  free(((Node *)(tuple->usuario))->data);
  free(((Node *)(tuple->usuario)));
  free(tuple);
  // pthread_mutex_lock(&((Node *)(tuple->usuario))->lock);
  // pthread_mutex_destroy(&((Node *)(tuple->usuario))->lock);
  // pthread_cond_destroy(&((Node *)(tuple->usuario))->notify);
  pthread_detach(pthread_self());
  pthread_cancel(pthread_self());
}
Tupla *choose_queue_cliente(Planificador *planificador)
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
        // printf("Taxi %d, posicion %d %d, cliente %d %d %d, distancia %d\n", ((Taxista *)(temp->data))->id, pos_x_taxista, pos_y_taxista,cliente->id ,pos_x_inicial_cliente, pos_y_inicial_cliente, calculo);
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