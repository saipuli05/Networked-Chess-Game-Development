#include "hw4.h"

int main() {
	int listenfd, connfd;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	// Create socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set options to reuse the IP address and IP port if either is already in use
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
		return -1;
	}
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
		return -1;
	}

	// Bind socket to port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if (bind(listenfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(listenfd, 1) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	INFO("Server listening on port %d", PORT);
	// Accept incoming connection
	if ((connfd = accept(listenfd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	INFO("Server accepted connection");

	ChessGame game;
	initialize_game(&game);
	display_chessboard(&game);

	char buffer[BUFFER_SIZE] = {0};
	char command[BUFFER_SIZE] = {0};
	while (1) {
		// Fill this in
		ssize_t bytes_received = read(connfd, buffer, BUFFER_SIZE - 1);
		if (bytes_received <= 0) {
			INFO("Client closed the connection or error in receiving data.");
			break;
		}
		buffer[bytes_received] = '\0';

		int receive_status = receive_command(&game, buffer, connfd, false);
		if (receive_status == COMMAND_FORFEIT) {
			INFO("Game over. Client has forfeited.");
			break; // Exit the loop if the client forfeits
		}

		if (receive_status == COMMAND_MOVE) {
			display_chessboard(&game);
		}

		int send_status;

		do {
			printf("Enter command: ");
			fgets(command, BUFFER_SIZE, stdin);
			command[strcspn(command, "\n")] = 0; // Removes the newline character

			send_status = send_command(&game, command, connfd, false);

			if (send_status == COMMAND_ERROR || send_status == COMMAND_UNKNOWN) {
				printf("Invalid command. Please try again.\n");
			}

		} while (send_status == COMMAND_ERROR || send_status == COMMAND_UNKNOWN || send_status == COMMAND_SAVE);


		if (send_status == COMMAND_FORFEIT) {
			break; // Exit the loop if the command is to forfeit
		}

		display_chessboard(&game);
	}

	close(listenfd);
	return 0;
}
