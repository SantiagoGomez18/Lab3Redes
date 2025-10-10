#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 512

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <broker_ip> <broker_port> <mi_puerto> <mensaje>\n", argv[0]);
        exit(1);
    }

    const char *broker_ip = argv[1];
    int broker_port = atoi(argv[2]);
    int mi_puerto = atoi(argv[3]);
    const char *mensaje = argv[4];

    int sockfd;
    struct sockaddr_in localAddr, brokerAddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(mi_puerto);

    if (bind(sockfd, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    memset(&brokerAddr, 0, sizeof(brokerAddr));
    brokerAddr.sin_family = AF_INET;
    brokerAddr.sin_port = htons(broker_port);
    inet_pton(AF_INET, broker_ip, &brokerAddr.sin_addr);

    // Enviar suscripción
    char subMsg[BUF_SIZE];
    snprintf(subMsg, sizeof(subMsg), "SUBSCRIBE|%s", mensaje);
    sendto(sockfd, subMsg, strlen(subMsg), 0,
           (struct sockaddr*)&brokerAddr, sizeof(brokerAddr));

    printf("Suscrito al publicador [%s]. Esperando eventos...\n", mensaje);

    int count = 0;
    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';
            printf("(%d) >> %s\n", ++count, buffer);
            fflush(stdout);
        }
    }

    close(sockfd);
    return 0;
}
