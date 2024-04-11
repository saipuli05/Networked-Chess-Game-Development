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

    // Connect to the server
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
		fgets(command, BUFFER_SIZE, stdin); // Reads the command from the standard input
		command[strcspn(command, "\n")] = 0; // Removes the newline character from the input

		int send_status = send_command(&game, command, connfd, true);
		if (send_status == COMMAND_ERROR || send_status == COMMAND_UNKNOWN) {
			printf("Invalid command. Please try again.\n");
			continue;
		} else if (send_status == COMMAND_FORFEIT) {
			break; // Exit the loop if the command is to forfeit
		} else if (send_status == COMMAND_SAVE){
			printf("Game saved. \n");
			continue; // Skip the next part of the loop if the command is to save
		}

		display_chessboard(&game); // Optionally display the chessboard after each command

		ssize_t bytes_received = read(connfd, buffer, BUFFER_SIZE - 1);
		if (bytes_received <= 0) {
			printf("Server closed the connection or error in receiving data.\n");
			break;
		}
		buffer[bytes_received] = '\0';

		if (receive_command(&game, buffer, connfd, true) == COMMAND_FORFEIT) {
			printf("Game over. The server has forfeited.\n");
			break; // Exit the loop if the server forfeits
		}

		display_chessboard(&game); // Optionally display the chessboard after each command
	}

    // Please ensure that the following lines of code execute just before your program terminates.
    // If necessary, copy and paste it to other parts of your code where you terminate your program.
    FILE *temp = fopen("./fen.txt", "w");
    char fen[200];
    chessboard_to_fen(fen, &game);
    fprintf(temp, "%s", fen);
    fclose(temp);
    close(connfd);
    return 0;
}
