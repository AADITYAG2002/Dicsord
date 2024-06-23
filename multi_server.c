#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENT 10
#define PORT 8080

int main(int argc, char **argv){
    time_t rawtime;
    struct tm TIME;
    char time_str[30] = {0};

    int master_sock, new_socket;
    int client_socket[MAX_CLIENT];
    int max_sd, cli_sd;
    int activity;
    struct sockaddr_in addr;
    int opt = 1;
    fd_set readfds;

    int readsize;
    char buffer[1024];
    char server_buf[2048];

    for (int i = 0; i < MAX_CLIENT; i++){
        client_socket[i] = 0;
    }

    if((master_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if(bind(master_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(master_sock, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(addr);
    puts("waiting for connection ...\n");

    while(1){
        FD_ZERO(&readfds);
        FD_SET(master_sock, &readfds);
        max_sd = master_sock;
        
        for(int i = 0; i < MAX_CLIENT ; i++){
            cli_sd = client_socket[i];
            if (cli_sd > 0)
                FD_SET(cli_sd, &readfds);
            if(cli_sd > max_sd)
                max_sd = cli_sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);        
        if(activity < 0){
            printf("select error\n");
        }
        
        if (FD_ISSET(master_sock, &readfds)){
            if((new_socket = accept(master_sock, (struct sockaddr*)&addr, (socklen_t*)&addrlen)) < 0){
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < MAX_CLIENT; i++){
                if(client_socket[i] == 0){
                    client_socket[i] = new_socket;
                    printf("Adding socket as %d\n", i);
                    break;
                }
            }
        }

        for(int i = 0; i < MAX_CLIENT; i++){
            cli_sd = client_socket[i];
            
            if(FD_ISSET(cli_sd, &readfds)){
                readsize = recv(cli_sd, buffer, 1024 - 1, 0);
                rawtime = time(NULL);

                if(readsize == 0){
                    close(cli_sd);
                    printf("closing socket: %d\n", cli_sd);
                    client_socket[i] = 0;
                }
                else{
                    TIME = *localtime(&rawtime);
                    strftime(time_str, 30, "[%d-%m-%EY] %H:%M:%S -> ", &TIME);
                    buffer[readsize] = '\0';

                    strcpy(server_buf, time_str);
                    strlcat(server_buf, buffer, 1024);
                    for(int j=0; j < MAX_CLIENT; j++){
                        send(client_socket[j], server_buf, strlen(server_buf), 0);
                    }
                }
            }
        }
    
    }

    return EXIT_SUCCESS;
}
