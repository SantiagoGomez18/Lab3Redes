#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    // Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Enlazar socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Broker UDP esperando mensajes en el puerto %d...\n", PORT);

    // Direcciones de los suscriptores
    struct sockaddr_in subs[2];
    int num_subs = 0;

    while (1) {
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        if (strncmp(buffer, "SUB", 3) == 0) {
            // Registrar nuevo suscriptor
            if (num_subs < 2) {
                subs[num_subs++] = cliaddr;
                printf("Nuevo suscriptor registrado. Total: %d\n", num_subs);
            }
        } else {
            // Reenviar mensaje a los suscriptores
            printf("Broker recibió: %s\n", buffer);
            for (int i = 0; i < num_subs; i++) {
                sendto(sockfd, buffer, strlen(buffer), 0,
                       (struct sockaddr *)&subs[i], sizeof(subs[i]));
            }
        }
    }

    close(sockfd);
    return 0;
}
