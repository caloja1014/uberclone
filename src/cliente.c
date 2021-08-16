#include "../include/cliente.h"

Cliente *creat_cliente(int id, bool isvip, int x_turnos_espera, int px_inicial, int px_final, int py_inicial, int py_final)
{
    Cliente *cliente = (Cliente *)malloc(sizeof(Cliente));
    cliente->id = id;
    cliente->isvip = isvip;
    cliente->x_turnos_espera = x_turnos_espera;
    cliente->px_inicial = px_inicial;
    cliente->px_final = px_final;
    cliente->py_inicial = py_inicial;
    cliente->py_final = py_final;
    return cliente;
}