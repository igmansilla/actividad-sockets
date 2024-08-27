#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // Vincula el programa con la librería winsock

#define PORT 8080
#define BUF_SIZE 1024

// Funciones de utilidad para la generación de nombres y contraseñas
char generate_vowel() {
    char vowels[] = "aeiouAEIOU";
    return vowels[rand() % 10];
}

char generate_consonant() {
    char consonants[] = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ";
    return consonants[rand() % 42];
}

char generate_alphanumeric() {
    char alphanumeric[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    return alphanumeric[rand() % 62];
}

void generate_username(char *username, int length) {
    int start_with_vowel = rand() % 2;
    for (int i = 0; i < length; i++) {
        if (start_with_vowel) {
            username[i] = (i % 2 == 0) ? generate_vowel() : generate_consonant();
        } else {
            username[i] = (i % 2 == 0) ? generate_consonant() : generate_vowel();
        }
    }
    username[length] = '\0';
}

void generate_password(char *password, int length) {
    for (int i = 0; i < length; i++) {
        password[i] = generate_alphanumeric();
    }
    password[length] = '\0';
}

// Función para manejar el cliente
void handle_client(SOCKET client_socket) {
    char buffer[BUF_SIZE];
    char username[16]; // Máximo 15 caracteres + terminador nulo
    char password[51]; // Máximo 50 caracteres + terminador nulo
    int choice, length;

    // Enviar instrucciones al cliente
    send(client_socket, "Seleccione una opción:\n1. Generar nombre de usuario\n2. Generar contraseña\n", 71, 0);
    int recv_len = recv(client_socket, buffer, BUF_SIZE, 0);
    if (recv_len <= 0) {
        closesocket(client_socket);
        return;
    }
    buffer[recv_len] = '\0';
    choice = atoi(buffer);

    // Validar elección
    if (choice < 1 || choice > 2) {
        send(client_socket, "Opción no válida.\n", 18, 0);
        closesocket(client_socket);
        return;
    }

    // Solicitar longitud de la cadena
    if (choice == 1) {
        send(client_socket, "Ingrese la longitud del nombre de usuario (5-15):\n", 50, 0);
    } else {
        send(client_socket, "Ingrese la longitud de la contraseña (8-50):\n", 46, 0);
    }
    recv_len = recv(client_socket, buffer, BUF_SIZE, 0);
    if (recv_len <= 0) {
        closesocket(client_socket);
        return;
    }
    buffer[recv_len] = '\0';
    length = atoi(buffer);

    // Validar longitud
    if ((choice == 1 && (length < 5 || length > 15)) ||
        (choice == 2 && (length < 8 || length > 50))) {
        send(client_socket, "Longitud no válida.\n", 19, 0);
        closesocket(client_socket);
        return;
    }

    // Generar y enviar la cadena solicitada
    if (choice == 1) {
        generate_username(username, length);
        snprintf(buffer, BUF_SIZE, "Nombre de usuario: %s\n", username);
    } else {
        generate_password(password, length);
        snprintf(buffer, BUF_SIZE, "Contraseña: %s\n", password);
    }
    send(client_socket, buffer, strlen(buffer), 0);

    // Cerrar el socket del cliente
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    // Inicializar Winsock
    printf("\nInicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error al inicializar Winsock. Código de error: %d", WSAGetLastError());
        return 1;
    }

    // Crear socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Preparar la estructura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Vincular
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Error en bind. Código de error: %d", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Escuchar
    listen(server_socket, 3);

    // Aceptar y manejar conexiones entrantes
    printf("Esperando conexiones entrantes...\n");
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Conexión aceptada.\n");

        // Manejar cliente en una nueva función
        handle_client(client_socket);
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Error en accept. Código de error: %d", WSAGetLastError());
    }

    // Limpiar y cerrar
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
