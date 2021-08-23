DEPS = include/*.h
CFLAGS= -Wall -c -o

.PHONY: all debug sanitize clean

all: planificador incrementar_cantidad_clientes

planificador: obj/sistema_control.o obj/queue.o obj/cliente.o obj/taxista.o obj/planificador.o $(DEPS)
	gcc -o $@ obj/sistema_control.o obj/queue.o obj/cliente.o obj/taxista.o obj/planificador.o -L. -lpthread -lm $(DFLAGS)
incrementar_cantidad_clientes: obj/incrementador.o $(DEPS)
	gcc -o $@ obj/incrementador.o $(DFLAGS)

obj/%.o: src/%.c $(DEPS)
	mkdir -p obj/
	gcc $(CFLAGS) $@ $< $(DFLAGS)


debug: DFLAGS = -g
debug: clean all

# Compila habilitando la herramienta AddressSanitizer para
# facilitar la depuración en tiempo de ejecución.
sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean all

clean:
	rm -rf incrementar_cantidad_clientes planificador obj/*.o