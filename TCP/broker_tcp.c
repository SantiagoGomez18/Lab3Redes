#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10

struct Subscriber
{
    int socket;
    char topic[50];
};

int main()
{
    int server_fd, client_fd[MAX_CLIENTS], new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024];
    fd_set readfds;
    struct Subscriber subs[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_fd[i] = 0;
        subs[i].socket = 0;
        subs[i].topic[0] = '\0';
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Broker TCP escuchando en puerto %d...\n", PORT);

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int max_sd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_fd[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds))
        {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fd[i] == 0)
                {
                    client_fd[i] = new_socket;
                    subs[i].socket = new_socket;
                    subs[i].topic[0] = '\0';
                    printf("Nuevo cliente conectado (socket %d)\n", new_socket);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_fd[i];
            if (FD_ISSET(sd, &readfds))
            {
                int valread = read(sd, buffer, 1024);
                if (valread == 0)
                {
                    close(sd);
                    client_fd[i] = 0;
                    subs[i].socket = 0;
                    subs[i].topic[0] = '\0';
                    printf("Cliente desconectado (socket %d)\n", sd);
                }
                else
                {
                    buffer[valread] = '\0';
                    printf("Mensaje recibido: %s\n", buffer);

                    if (strncmp(buffer, "SUBSCRIBE", 9) == 0)
                    {
                        char topic[50];
                        sscanf(buffer, "SUBSCRIBE %s", topic);
                        strcpy(subs[i].topic, topic);
                        printf("Cliente %d suscrito a %s\n", sd, subs[i].topic);
                    }
                    else
                    {
                        char topic[50];
                        sscanf(buffer, "%[^:]:", topic);
                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if (subs[j].socket != 0 && strcmp(subs[j].topic, topic) == 0)
                            {
                                send(subs[j].socket, buffer, strlen(buffer), 0);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
