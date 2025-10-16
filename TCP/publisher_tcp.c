#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char mensaje[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Publisher conectado al Broker.\n");
    printf("Escribe mensajes para PARTIDO1 (o 'salir'):\n");

    while (1)
    {
        printf("> ");
        fgets(mensaje, sizeof(mensaje), stdin);
        mensaje[strcspn(mensaje, "\n")] = 0;
        if (strcmp(mensaje, "salir") == 0)
            break;

        char msg_final[1100];
        snprintf(msg_final, sizeof(msg_final), "PARTIDO1: %s", mensaje);
        send(sock, msg_final, strlen(msg_final), 0);
    }

    close(sock);
    printf("Publisher desconectado.\n");
    return 0;
}
