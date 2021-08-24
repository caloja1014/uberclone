#ifndef CLIENTE
#define CLIENTE
#include <stdlib.h>
#include <stdbool.h>

typedef struct cliente_struct
{
    int id;
    bool isvip;
    bool isPriorizado;
    int x_turnos_espera;
    int px_inicial;
    int px_final;
    int py_inicial;
    int py_final;
}  Cliente;

Cliente *crear_cliente( bool isvip, int x_turnos_espera, int px_inicial, int px_final, int py_inicial, int py_final, int id);
void destruir_cliente(Cliente *cliente);

#endif /*CLIENTE */