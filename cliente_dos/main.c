#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define IP_SERVIDOR "127.0.0.1"
#define PUERTO 8080
#define TAM_BUF 1024

int main() {
    WSADATA wsa;
    SOCKET socket_cliente;
    struct sockaddr_in servidor;
    char buffer[TAM_BUF];
    int eleccion, longitud;

    printf("\nInicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock. Codigo de error: %d", WSAGetLastError());
        return 1;
    }

    if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear el socket. Codigo de error: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
    servidor.sin_port = htons(PUERTO);

    if (connect(socket_cliente, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
        printf("Error al conectar con el servidor. Codigo de error: %d", WSAGetLastError());
        closesocket(socket_cliente);
        WSACleanup();
        return 1;
    }
    printf("Conectado al servidor.\n");

    while (1) {
        int tam_recv = recv(socket_cliente, buffer, TAM_BUF, 0);
        if (tam_recv <= 0) {
            printf("Error al recibir el menú del servidor.\n");
            break;
        }
        buffer[tam_recv] = '\0';
        printf("%s", buffer);

        printf("Ingrese su eleccion: ");
        scanf("%d", &eleccion);
        snprintf(buffer, TAM_BUF, "%d", eleccion);
        send(socket_cliente, buffer, strlen(buffer), 0);

        if (eleccion == 3) {
            printf("Desconectando...\n");
            break;
        }

        tam_recv = recv(socket_cliente, buffer, TAM_BUF, 0);
        if (tam_recv <= 0) {
            printf("Error al recibir la solicitud de longitud del servidor.\n");
            break;
        }
        buffer[tam_recv] = '\0';
        printf("%s", buffer);

        scanf("%d", &longitud);
        snprintf(buffer, TAM_BUF, "%d", longitud);
        send(socket_cliente, buffer, strlen(buffer), 0);

        tam_recv = recv(socket_cliente, buffer, TAM_BUF, 0);
        if (tam_recv > 0) {
            buffer[tam_recv] = '\0';
            printf("Respuesta del servidor: %s\n", buffer);
        } else {
            printf("Error al recibir la respuesta del servidor.\n");
            break;
        }
    }

    closesocket(socket_cliente);
    WSACleanup();

    return 0;
}
