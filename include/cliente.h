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

Cliente *crearCliente(int id, bool isvip, int x_turnos_espera, int px_inicial, int px_final, int py_inicial, int py_final);
void destruirCliente(Cliente *cliente);