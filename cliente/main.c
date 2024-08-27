#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Enlaza automáticamente con la biblioteca Winsock

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char message[100], server_reply[2000];
    int message_length, recv_size;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar la estructura del servidor
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección IP del servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(8888); // Puerto del servidor

    // Conectarse al servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect error. Error Code : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Conectado al servidor.\n");

    // Menú de opciones
    int option;
    printf("Seleccione una opción:\n");
    printf("1. Generar nombre de usuario\n");
    printf("2. Generar contraseña\n");
    printf("Opción: ");
    scanf("%d", &option);

    if (option == 1) {
        printf("Ingrese la longitud del nombre de usuario (5-15): ");
        scanf("%d", &message_length);
        if (message_length < 5 || message_length > 15) {
            printf("Longitud inválida.\n");
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        sprintf(message, "username %d", message_length);
    } else if (option == 2) {
        printf("Ingrese la longitud de la contraseña (8-50): ");
        scanf("%d", &message_length);
        if (message_length < 8 || message_length > 50) {
            printf("Longitud inválida.\n");
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        sprintf(message, "password %d", message_length);
    } else {
        printf("Opción inválida.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Enviar mensaje al servidor
    if (send(sock, message, strlen(message), 0) < 0) {
        printf("Send failed. Error Code : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Recibir respuesta del servidor
    if ((recv_size = recv(sock, server_reply, 2000, 0)) == SOCKET_ERROR) {
        printf("Recv failed. Error Code : %d\n", WSAGetLastError());
    } else {
        server_reply[recv_size] = '\0'; // Añadir terminador nulo al final de la cadena
        printf("Respuesta del servidor: %s\n", server_reply);
    }

    // Cerrar el socket y limpiar Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
