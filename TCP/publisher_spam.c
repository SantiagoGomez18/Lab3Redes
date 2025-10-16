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

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Publisher listo pa spamear (PARTIDO3)\n");

    int MESSAGES = 40;
    int INTERVAL_US = 100000;

    for (int i = 0; i < MESSAGES; i++)
    {
        char msg[256];
        sprintf(msg, "PARTIDO3: Mensaje %d", i);
        send(sock, msg, strlen(msg), 0);
        printf("Mensaje enviado: %s\n", msg);
        usleep(INTERVAL_US);
    }

    close(sock);
    printf("Publisher desconectado.\n");
    return 0;
}
