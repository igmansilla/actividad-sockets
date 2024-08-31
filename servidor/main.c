#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 8080
#define TAM_BUF 1024

char generar_vocal() {
    char vocales[] = "aeiouAEIOU";
    return vocales[rand() % 10];
}

char generar_consonante() {
    char consonantes[] = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ";
    return consonantes[rand() % 42];
}

char generar_alfanumerico() {
    char alfanumerico[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    return alfanumerico[rand() % 62];
}

void generar_nombre_usuario(char *nombre_usuario, int longitud) {
    int empezar_con_vocal = rand() % 2;
    for (int i = 0; i < longitud; i++) {
        if (empezar_con_vocal) {
            nombre_usuario[i] = (i % 2 == 0) ? generar_vocal() : generar_consonante();
        } else {
            nombre_usuario[i] = (i % 2 == 0) ? generar_consonante() : generar_vocal();
        }
    }
    nombre_usuario[longitud] = '\0';
}

void generar_contrasena(char *contrasena, int longitud) {
    for (int i = 0; i < longitud; i++) {
        contrasena[i] = generar_alfanumerico();
    }
    contrasena[longitud] = '\0';
}

void manejar_cliente(SOCKET socket_cliente) {
    char buffer[TAM_BUF];
    char nombre_usuario[16];
    char contrasena[51];
    int opcion, longitud;

    while (1) {
        send(socket_cliente, "Seleccione una opción:\n1. Generar nombre de usuario\n2. Generar contraseña\n3. Salir\n", 75, 0);
        int tam_recv = recv(socket_cliente, buffer, TAM_BUF, 0);
        if (tam_recv <= 0) {
            printf("Cliente desconectado.\n");
            closesocket(socket_cliente);
            return;
        }
        buffer[tam_recv] = '\0';
        opcion = atoi(buffer);

        if (opcion == 3) {
            send(socket_cliente, "Desconectando...\n", 17, 0);
            break;
        }
        if (opcion < 1 || opcion > 2) {
            send(socket_cliente, "Opción no válida.\n", 18, 0);
            continue;
        }

        if (opcion == 1) {
            send(socket_cliente, "Ingrese la longitud del nombre de usuario (5-15):\n", 50, 0);
        } else {
            send(socket_cliente, "Ingrese la longitud de la contraseña (8-50):\n", 46, 0);
        }
        tam_recv = recv(socket_cliente, buffer, TAM_BUF, 0);
        if (tam_recv <= 0) {
            printf("Cliente desconectado.\n");
            closesocket(socket_cliente);
            return;
        }
        buffer[tam_recv] = '\0';
        longitud = atoi(buffer);

        if ((opcion == 1 && (longitud < 5 || longitud > 15)) || (opcion == 2 && (longitud < 8 || longitud > 50))) {
            send(socket_cliente, "Longitud no válida.\n", 19, 0);
            continue;
        }

        if (opcion == 1) {
            generar_nombre_usuario(nombre_usuario, longitud);
            snprintf(buffer, TAM_BUF, "Nombre de usuario: %s\n", nombre_usuario);
        } else {
            generar_contrasena(contrasena, longitud);
            snprintf(buffer, TAM_BUF, "Contraseña: %s\n", contrasena);
        }
        send(socket_cliente, buffer, strlen(buffer), 0);
    }

    closesocket(socket_cliente);
}

int main() {
    WSADATA wsa;
    SOCKET socket_servidor, socket_cliente;
    struct sockaddr_in servidor, cliente;
    int c;

    printf("\nInicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock. Código de error: %d", WSAGetLastError());
        return 1;
    }

    if ((socket_servidor = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(PUERTO);

    if (bind(socket_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) == SOCKET_ERROR) {
        printf("Error en bind. Código de error: %d", WSAGetLastError());
        closesocket(socket_servidor);
        WSACleanup();
        return 1;
    }

    listen(socket_servidor, 3);

    printf("Esperando conexiones entrantes...\n");
    c = sizeof(struct sockaddr_in);
    while ((socket_cliente = accept(socket_servidor, (struct sockaddr *)&cliente, &c)) != INVALID_SOCKET) {
        printf("Conexión aceptada.\n");
        manejar_cliente(socket_cliente);
    }

    if (socket_cliente == INVALID_SOCKET) {
        printf("Error en accept. Código de error: %d", WSAGetLastError());
    }

    closesocket(socket_servidor);
    WSACleanup();

    return 0;
}
