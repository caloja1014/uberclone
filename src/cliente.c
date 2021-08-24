#include "../include/cliente.h"

Cliente *crear_cliente( bool isvip, int x_turnos_espera, int px_inicial, int px_final, int py_inicial, int py_final,int id)
{
    Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
    cliente->id = id;
    cliente->isvip = isvip;
    cliente->x_turnos_espera = x_turnos_espera;
    cliente->px_inicial = px_inicial;
    cliente->px_final = px_final;
    cliente->py_inicial = py_inicial;
    cliente->py_final = py_final;
    cliente->isPriorizado = false;
    return cliente;
}