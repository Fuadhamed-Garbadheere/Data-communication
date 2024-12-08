#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// Function to handle client communication
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    char server_message[BUFFER_SIZE];
    int bytes_received;

    while (1) {
        // Receive message from client
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received message
        printf("Client: %s\n", buffer);

        // Send a response to the client
        printf("Enter message to send to client: ");
        fgets(server_message, BUFFER_SIZE, stdin);
        server_message[strcspn(server_message, "\n")] = '\0'; // Remove newline
        send(client_socket, server_message, strlen(server_message), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Bind done.\n");

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Waiting for incoming connections...\n");

    // Accept and handle clients
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) != INVALID_SOCKET) {
        printf("Client connected.\n");
        handle_client(client_socket);
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
    }

    // Cleanup
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

