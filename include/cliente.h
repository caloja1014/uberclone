#indef CLIENTE
#define CLIENTE

struct cliente_struct
{
    int id;
    bool isvip;
    int x_turnos_espera;
    int px_inicial;
    int px_final;
    int py_inicial;
    int py_final;
} typedef Cliente;

Cliente *creat_cliente(int id, bool isvip, int x_turnos_espera, int px_inicial, int px_final, int py_inicial, int py_final);
void destruir_cliente(Cliente *cliente);