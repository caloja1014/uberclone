DEPS = include/queue.h
CFLAGS= -Wall -c -o

.PHONY: all debug sanitize clean



debug: DFLAGS = -g
debug: clean all

# Compila habilitando la herramienta AddressSanitizer para
# facilitar la depuración en tiempo de ejecución.
sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean all