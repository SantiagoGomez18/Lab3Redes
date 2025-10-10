#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 512

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <broker_ip> <broker_port> <mensaje>\n", argv[0]);
        exit(1);
    }

    const char *broker_ip = argv[1];
    int broker_port = atoi(argv[2]);
    const char *topic = argv[3];

    int sockfd;
    struct sockaddr_in brokerAddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&brokerAddr, 0, sizeof(brokerAddr));
    brokerAddr.sin_family = AF_INET;
    brokerAddr.sin_port = htons(broker_port);
    inet_pton(AF_INET, broker_ip, &brokerAddr.sin_addr);

    printf("Publisher para mensaje [%s]. Escribe eventos (Ctrl+C para salir):\n", topic);

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        buffer[strcspn(buffer, "\n")] = '\0';

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s|%s", topic, buffer);

        sendto(sockfd, msg, strlen(msg), 0,
               (struct sockaddr*)&brokerAddr, sizeof(brokerAddr));
    }

    close(sockfd);
    return 0;
}
