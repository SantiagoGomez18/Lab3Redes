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
    char partido[64];
};

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    struct Subscriber subs[MAX_SUBS];
    int num_subs = 0;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
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
        buffer[n] = '\0';

        // Si el mensaje es de suscripción
        if (strncmp(buffer, "SUBSCRIBE|", 10) == 0) {
            char *partido = buffer + 10;

            if (num_subs < MAX_SUBS) {
                subs[num_subs].addr = cliaddr;
                strncpy(subs[num_subs].partido, partido, sizeof(subs[num_subs].partido));
                num_subs++;
                printf("Nuevo suscriptor para [%s]. Total: %d\n", partido, num_subs);
            }
        } else {
            // Mensaje de publicador
            printf("Broker recibió: %s\n", buffer);

            // Extraer partido antes del '|'
            char *sep = strchr(buffer, '|');
            if (!sep) continue; // formato inválido
            *sep = '\0';
            char *partido = buffer;
            char *mensaje = sep + 1;

            // Reenviar a suscriptores que siguen ese partido
            for (int i = 0; i < num_subs; i++) {
                if (strcmp(subs[i].partido, partido) == 0) {
                    sendto(sockfd, mensaje, strlen(mensaje), 0,
                           (struct sockaddr *)&subs[i].addr, sizeof(subs[i].addr));
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
