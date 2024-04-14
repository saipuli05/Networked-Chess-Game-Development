#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hw4.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    ChessGame game;
    int connfd = 0;
    struct sockaddr_in serv_addr;

    
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(connfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    initialize_game(&game);
    display_chessboard(&game);

	char buffer[BUFFER_SIZE] = {0};
	char command[BUFFER_SIZE] = {0};

    while (1) {

		printf("Enter command: ");
		fgets(command, BUFFER_SIZE, stdin); 
		command[strcspn(command, "\n")] = 0; 
		int send_status = send_command(&game, command, connfd, true);
		if (send_status == COMMAND_ERROR || send_status == COMMAND_UNKNOWN) {
			printf("Invalid command. Please try again.\n");
			continue;
		} else if (send_status == COMMAND_FORFEIT) {
			break; 
		} else if (send_status == COMMAND_SAVE){
			printf("Game saved. \n");
			continue; 
		}

		display_chessboard(&game); 

		ssize_t bytes_received = read(connfd, buffer, BUFFER_SIZE - 1);
		if (bytes_received <= 0) {
			printf("Server closed the connection or error in receiving data.\n");
			break;
		}
		buffer[bytes_received] = '\0';

		if (receive_command(&game, buffer, connfd, true) == COMMAND_FORFEIT) {
			printf("Game over. The server has forfeited.\n");
			break; 
		}

		display_chessboard(&game); 
	}

    
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(connfd);
    return 0;
}
