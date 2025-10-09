
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_SUBS 50
#define BUF_SIZE 1024
#define MAX_PARTIDO 64

typedef struct {
    char partido[MAX_PARTIDO];
    struct sockaddr_in addr;
} Suscriptor;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
        exit(1);
    }

    int puerto = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];
    Suscriptor subs[MAX_SUBS];
    int numSubs = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(puerto);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    printf("Broker escuchando en puerto %d...\n", puerto);

    while (1) {
        socklen_t len = sizeof(cliaddr);
        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                             (struct sockaddr*)&cliaddr, &len);
        if (n < 0) continue;
        buffer[n] = '\0';

        // Verificar si es suscripción
        if (strncmp(buffer, "SUBSCRIBE|", 10) == 0) {
            char *partido = buffer + 10;
            int yaExiste = 0;

            for (int i = 0; i < numSubs; i++) {
                if (strcmp(subs[i].partido, partido) == 0 &&
                    memcmp(&subs[i].addr, &cliaddr, sizeof(cliaddr)) == 0) {
                    yaExiste = 1;
                    break;
                }
            }

            if (!yaExiste && numSubs < MAX_SUBS) {
                strcpy(subs[numSubs].partido, partido);
                subs[numSubs].addr = cliaddr;
                numSubs++;
                printf("Nuevo suscriptor para partido: %s\n", partido);
            }

        } else {
            // Es un mensaje de publicador
            char *sep = strchr(buffer, '|');
            if (!sep) continue;

            *sep = '\0';
            char *partido = buffer;
            char *mensaje = sep + 1;

            printf("Evento recibido: [%s] %s\n", partido, mensaje);

            // Reenviar a los suscriptores de ese partido
            for (int i = 0; i < numSubs; i++) {
                if (strcmp(subs[i].partido, partido) == 0) {
                    sendto(sockfd, mensaje, strlen(mensaje), 0,
                           (struct sockaddr*)&subs[i].addr, sizeof(subs[i].addr));
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
