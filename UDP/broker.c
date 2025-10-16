#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000
#define MAXLINE 1024
#define MAX_SUBS 10

struct Subscriber {
    struct sockaddr_in addr;
    char mensaje[64];
};

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    struct Subscriber subs[MAX_SUBS];
    int num_subs = 0;

    // Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Reducir el buffer de recepción para provocar pérdida
    int buf_size = 2048; // 2 KB
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) < 0) {
        perror("Error al ajustar el tamaño del buffer");
    } else {
        printf("Buffer de recepción ajustado a %d bytes\n", buf_size);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Broker UDP en puerto %d esperando mensajes...\n", PORT);

    while (1) {
        int n = recvfrom(sockfd, buffer, MAXLINE - 1, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) continue;

        buffer[n] = '\0';

        // Si el mensaje es de suscripción
        if (strncmp(buffer, "SUBSCRIBE|", 10) == 0) {
            char *mensaje = buffer + 10;

            if (num_subs < MAX_SUBS) {
                subs[num_subs].addr = cliaddr;
                strncpy(subs[num_subs].mensaje, mensaje, sizeof(subs[num_subs].mensaje));
                num_subs++;
                printf("Nuevo suscriptor para [%s]. Total: %d\n", mensaje, num_subs);
            }
        } else {
            // Mensaje de publicador
            printf("Broker recibió: %s\n", buffer);

            char *sep = strchr(buffer, '|');
            if (!sep) continue;
            *sep = '\0';
            char *topic = buffer;
            char *mensaje = sep + 1;

            for (int i = 0; i < num_subs; i++) {
                if (strcmp(subs[i].mensaje, topic) == 0) {
                    sendto(sockfd, mensaje, strlen(mensaje), 0,
                           (struct sockaddr *)&subs[i].addr, sizeof(subs[i].addr));
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
