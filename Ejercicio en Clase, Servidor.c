#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Estructura para mantener la información del cliente
typedef struct {
    char nombre[20];
    int saldo;
    int socket;
} Cliente;

// Estructura para la lista de clientes
typedef struct {
    Cliente clientes[100]; // Suponemos un máximo de 100 clientes
    int numClientes;
    pthread_mutex_t mutex;
} ListaClientes;


// Función para atender a un cliente
void *AtenderCliente(void *arg) {
    int socketCliente = *((int *)arg);
    ListaClientes *lista = (ListaClientes *)arg;
    char buffer[256];
    int saldo;

    // Recibir mensajes del cliente y responder según el protocolo
    while (recv(socketCliente, buffer, sizeof(buffer), 0) > 0) {
        char *token = strtok(buffer, " ");
        if (strcmp(token, "CONNECT") == 0) {
            char nombreCliente[20];
            token = strtok(NULL, " ");
            strcpy(nombreCliente, token);
            TomarNotaClienteConectado(lista, nombreCliente, socketCliente);
        } else if (strcmp(token, "BALANCE") == 0) {
            char nombreCliente[20];
            token = strtok(NULL, " ");
            strcpy(nombreCliente, token);
            saldo = DameSaldoCliente(lista, nombreCliente);
            send(socketCliente, &saldo, sizeof(int), 0);
        } else if (strcmp(token, "DEPOSIT") == 0) {
            int cantidad;
            token = strtok(NULL, " ");
            cantidad = atoi(token);
            char nombreCliente[20];
            token = strtok(NULL, " ");
            strcpy(nombreCliente, token);
            HacerIngreso(lista, cantidad, nombreCliente);
        } else if (strcmp(token, "DISCONNECT") == 0) {
            TomarNotaClienteDesconectado(lista, socketCliente);
            close(socketCliente);
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    return NULL;
}
