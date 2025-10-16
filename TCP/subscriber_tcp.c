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
    char buffer[1024];
    char partido[50];
    char sub_msg[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Subscriber conectado al Broker.\n");
    printf("Escriba el partido al que se quiere suscribir (PARTIDO1, PARTIDO2, PARTIDO3): ");
    fgets(partido, sizeof(partido), stdin);
    partido[strcspn(partido, "\n")] = 0;
    
    snprintf(sub_msg, sizeof(sub_msg), "SUBSCRIBE %s", partido);

    send(sock, sub_msg, strlen(sub_msg), 0);
    printf("Suscrito al partido %s!!!! ESperando nuevos mensajes de ese partido... \n", partido);

    while (1)
    {
        int valread = read(sock, buffer, 1024);
        if (valread <= 0)
            break;
        buffer[valread] = '\0';
        printf("Actualización: %s\n", buffer);
    }

    close(sock);
    printf("Subscriber desconectado.\n");
    return 0;
}
