#include "hw4.h"

void initialize_game(ChessGame *game) {
	const char *startingBoard[8] = {
			"rnbqkbnr",
			"pppppppp",
			"........",
			"........",
			"........",
			"........",
			"PPPPPPPP",
			"RNBQKBNR"
	};

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			game->chessboard[i][j] = startingBoard[i][j];
		}
	}

	// Initialize counts
	game->moveCount = 0;
	game->capturedCount = 0;

	// Set the current player to WHITE_PLAYER at the start of the game
	game->currentPlayer = WHITE_PLAYER;
	(void) game;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
	int index = 0; // Index for appending characters to fen string

	for (int row = 0; row < 8; row++) {
		int emptyCount = 0; // Count consecutive empty squares

		for (int col = 0; col < 8; col++) {
			char piece = game->chessboard[row][col];

			if (piece == '.') { // Empty square
				emptyCount++;
			} else {
				if (emptyCount > 0) {
					// Append count of empty squares before the piece
					fen[index++] = '0' + emptyCount;
					emptyCount = 0;
				}
				fen[index++] = piece;
			}
		}

		if (emptyCount > 0) {
			// Append count of empty squares at the end of the row
			fen[index++] = '0' + emptyCount;
		}

		if (row < 7) {
			fen[index++] = '/'; // Separate rows
		}
	}

	// Append space and the current player's turn
	fen[index++] = ' ';
	fen[index++] = (game->currentPlayer == WHITE_PLAYER) ? 'w' : 'b';
	fen[index] = '\0'; // Null-terminate the string
	(void) fen;
	(void) game;
}

bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	// Check if the piece is a pawn
	if (piece != 'P' && piece != 'p') {
		return false; // Not a pawn
	}

	// Calculate row and column differences
	int row_diff = dest_row - src_row;
	int col_diff = dest_col - src_col;

	// Movement logic for white pawns
	if (piece == 'P') {
		// Check if the move is a standard single step forward
		if (col_diff == 0 && row_diff == -1 && game->chessboard[dest_row][dest_col] == '.') {
			return true;
		}
			// Check for initial double step move from the pawn's starting row (row 6 if we consider the board from 0 to 7)
		else if (src_row == 6 && col_diff == 0 && row_diff == -2 && game->chessboard[dest_row][dest_col] == '.' &&
				 game->chessboard[dest_row + 1][dest_col] == '.') {
			return true;
		}
			// Check for capturing move
		else if (abs(col_diff) == 1 && row_diff == -1 && game->chessboard[dest_row][dest_col] != '.' &&
				 islower(game->chessboard[dest_row][dest_col])) {
			return true;
		}
	}
		// Movement logic for black pawns
	else if (piece == 'p') {
		// Check if the move is a standard single step forward
		if (col_diff == 0 && row_diff == 1 && game->chessboard[dest_row][dest_col] == '.') {
			return true;
		}
			// Check for initial double step move from the pawn's starting row (row 1 if we consider the board from 0 to 7)
		else if (src_row == 1 && col_diff == 0 && row_diff == 2 && game->chessboard[dest_row][dest_col] == '.' &&
				 game->chessboard[dest_row - 1][dest_col] == '.') {
			return true;
		}
			// Check for capturing move
		else if (abs(col_diff) == 1 && row_diff == 1 && game->chessboard[dest_row][dest_col] != '.' &&
				 isupper(game->chessboard[dest_row][dest_col])) {
			return true;
		}
	}

	(void) piece;
	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;
	(void) game;
	return false;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	// Check for horizontal move
	if (src_row == dest_row) {
		int step = (dest_col > src_col) ? 1 : -1;
		for (int col = src_col + step; col != dest_col; col += step) {
			if (game->chessboard[src_row][col] != '.') {
				return false; // Found a piece blocking the path
			}
		}
	}
		// Check for vertical move
	else if (src_col == dest_col) {
		int step = (dest_row > src_row) ? 1 : -1;
		for (int row = src_row + step; row != dest_row; row += step) {
			if (game->chessboard[row][src_col] != '.') {
				return false; // Found a piece blocking the path
			}
		}
	} else {
		// Not a straight line move
		return false;
	}

	// The path is clear, move is valid
	return true;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	// Check for L-shape move: 2 squares one axis and 1 square the other axis
	return (row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2);

	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	// Check if the move is diagonal by comparing row and column differences
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	if (row_diff != col_diff) {
		return false; // Not a diagonal move
	}

	// Determine the direction of the move
	int row_step = (dest_row > src_row) ? 1 : -1;
	int col_step = (dest_col > src_col) ? 1 : -1;

	// Check for blocking pieces along the diagonal path
	int cur_row = src_row + row_step;
	int cur_col = src_col + col_step;
	while (cur_row != dest_row && cur_col != dest_col) {
		if (game->chessboard[cur_row][cur_col] != '.') {
			return false; // Found a piece blocking the path
		}
		cur_row += row_step;
		cur_col += col_step;
	}

	// The path is clear, move is valid
	return true;

	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;
	(void) game;
}

bool is_clear_path(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	int row_diff = dest_row - src_row;
	int col_diff = dest_col - src_col;
	int row_step = (row_diff != 0) ? (row_diff / abs(row_diff)) : 0; // Determine step direction or 0 if no step
	int col_step = (col_diff != 0) ? (col_diff / abs(col_diff)) : 0;

	int cur_row = src_row + row_step;
	int cur_col = src_col + col_step;
	while (cur_row != dest_row || cur_col != dest_col) {
		if (game->chessboard[cur_row][cur_col] != '.') {
			return false; // Found a piece blocking the path
		}
		cur_row += row_step;
		cur_col += col_step;
	}
	return true;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	// Check for horizontal or vertical move
	if (src_row == dest_row || src_col == dest_col) {
		return is_clear_path(src_row, src_col, dest_row, dest_col, game);
	}

	// Check for diagonal move
	if (abs(dest_row - src_row) == abs(dest_col - src_col)) {
		return is_clear_path(src_row, src_col, dest_row, dest_col, game);
	}

	// If not horizontal, vertical, or diagonal, it's not a valid move
	return false;

	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;
	(void) game;
}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
	// Calculate the differences in rows and columns to determine the move's distance
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	// The king can move one square in any direction
	// Therefore, both row_diff and col_diff should be 0 or 1, but not both 0
	if ((row_diff == 1 || row_diff == 0) && (col_diff == 1 || col_diff == 0) && !(row_diff == 0 && col_diff == 0)) {
		return true;
	} else {
		return false;
	}

	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;

}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	// Convert piece to lowercase to simplify switch cases (assumes white pieces are uppercase and black are lowercase)
	char piece_type = tolower(piece);
	bool result = false;

	switch (piece_type) {
		case 'p': // Pawn
			result = is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
			break;
		case 'r': // Rook
			result = is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'n': // Knight
			result = is_valid_knight_move(src_row, src_col, dest_row, dest_col);
			break;
		case 'b': // Bishop
			result = is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'q': // Queen
			result = is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'k': // King
			result = is_valid_king_move(src_row, src_col, dest_row, dest_col);
			break;
		default:
			// Invalid piece type
			result = false;
	}

	return result;

	(void) piece;
	(void) src_row;
	(void) src_col;
	(void) dest_row;
	(void) dest_col;
	(void) game;

}

void fen_to_chessboard(const char *fen, ChessGame *game) {
	int row = 0, col = 0;

	while (*fen != ' ') { // Parse until space (end of board state)
		if (*fen == '/') {
			row++; // Move to the next row
			col = 0; // Reset column to the start
		} else if (isdigit(*fen)) {
			int emptySpaces = *fen - '0';
			for (int i = 0; i < emptySpaces; i++) {
				game->chessboard[row][col++] = '.'; // Mark empty squares
			}
		} else {
			game->chessboard[row][col++] = *fen; // Place the piece on the board
		}
		fen++; // Move to the next character
	}

	fen++; // Skip the space
	game->currentPlayer = (*fen == 'w') ? WHITE_PLAYER : BLACK_PLAYER; // Set the current player based on FEN

	// Reset other game state if necessary (e.g., move counters)
	game->moveCount = 0;
	game->capturedCount = 0;

	(void) fen;
	(void) game;
}

// Helper function to check if a character is within 'a' to 'h'
int is_valid_row(char c) {
	return c >= 'a' && c <= 'h';
}

// Helper function to check if a character is within '1' to '8'
int is_valid_column(char c) {
	return c >= '1' && c <= '8';
}

bool is_valid_format(const char *move) {
	// Check if the move string is of the correct length
	size_t len = strlen(move);
	if (len < 4 || len > 5) {
		return false;
	}

	//if the row letter is not in the range 'a' through 'h'
	if (!is_valid_row(move[0]) || !is_valid_row(move[2])) {
		return false;
	}

	return true;
}

int parse_move(const char *move, ChessMove *parsed_move) {
	// Validate the length of the move string
	size_t len = strlen(move);

	if (!is_valid_format(move)) {
		return PARSE_MOVE_INVALID_FORMAT; // Error code for invalid format
	}

	if (!is_valid_column(move[1]) || !is_valid_column(move[3])) {
		return PARSE_MOVE_OUT_OF_BOUNDS;
	}

	if (len == 5) {
		if (move[3] != '1' && move[3] != '8') {
			return PARSE_MOVE_INVALID_DESTINATION;
		}

		if (move[4] != 'q' && move[4] != 'r' && move[4] != 'b' && move[4] != 'n') {
			return PARSE_MOVE_INVALID_PROMOTION;
		}
	}

	// Fill the ChessMove struct
	strncpy(parsed_move->startSquare, move, 2);
	parsed_move->startSquare[2] = '\0'; // Ensure null termination
	strncpy(parsed_move->endSquare, move + 2, 2);
	parsed_move->endSquare[2] = '\0'; // Ensure null termination for non-promotion moves
	if (len == 5) {
		parsed_move->endSquare[2] = tolower(move[4]);
		parsed_move->endSquare[3] = '\0'; // Ensure null termination for promotion moves
	}

	return 0; // Indicate successful parsing

	(void) move;
	(void) parsed_move;
}


int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {

	// Convert chessboard positions to 0-based array indices
	int startRow = 8 - (move->startSquare[1] - '0');
	int startCol = move->startSquare[0] - 'a';
	int endRow = 8 - (move->endSquare[1] - '0');
	int endCol = move->endSquare[0] - 'a';
	char piece = game->chessboard[startRow][startCol];
	char endPiece = game->chessboard[endRow][endCol];


	if (validate_move) {
		//MOVE_OUT_OF_TURN: Moving out of turn is not allowed. If the client tries to move when
		//it's the server's turn, or vice versa, return this error code.

		if ((is_client && game->currentPlayer != WHITE_PLAYER) ||
			(!is_client && game->currentPlayer != BLACK_PLAYER)) {
			return MOVE_OUT_OF_TURN;
		}

		//MOVE_NOTHING: Attempting to move from an empty square is not allowed.

		if (piece == '.') {
			return MOVE_NOTHING;
		}

		//MOVE_WRONG_COLOR: Attempting to move the opponent's piece is not allowed. If the
		//current player tries to move a piece of the opposite color, return this error code.

		bool isWhitePiece = piece >= 'A' && piece <= 'Z';
		if ((is_client && !isWhitePiece) || (!is_client && isWhitePiece)) {
			return MOVE_WRONG_COLOR;
		}

		//MOVE_SUS: Capturing your own pieces is not allowed.

		if ((is_client && isupper(endPiece)) || (!is_client && islower(endPiece))) {
			return MOVE_SUS;
		}

		//MOVE_NOT_A_PAWN: The move string has a length of 5 characters (indicating a pawn
		//promotion), but the piece at the start square is not a pawn ('P' or 'p'). You are not
		//allowed to promote pieces that are not pawns. Example: endSquare = e7e8q, but
		//there is no pawn on e7

		if (strlen(move->endSquare) == 3 && (piece != 'P' && piece != 'p')) {
			return MOVE_NOT_A_PAWN;
		}

		//MOVE_MISSING_PROMOTION: The move string has a length of 4 characters, but the
		//destination square is on the row for pawn promotion and the piece being moved is a
		//pawn. Example: endSquare = e8. To be valid, endSquare would need to be
		//something like endSquare = e8q

		if (strlen(move->endSquare) == 2 && (piece == 'P' || piece == 'p') && (endRow == 0 || endRow == 7)) {
			return MOVE_MISSING_PROMOTION;
		}

		//MOVE_WRONG: Return this error code if is_valid_move() returned false

		if (!is_valid_move(piece, startRow, startCol, endRow, endCol, game)) {
			return MOVE_WRONG;
		}

	}

	//While moving the piece, you will have to promote this piece in make_move if the piece moves
	//onto the last row for either side (White or Black) with a pawn.

	if (strlen(move->endSquare) == 3) {
		piece = game->currentPlayer == WHITE_PLAYER ? toupper(move->endSquare[2]) : tolower(move->endSquare[2]);
	}

	if (endPiece != '.') {
		game->capturedPieces[game->capturedCount++] = endPiece;
	}

	game->chessboard[endRow][endCol] = piece;
	game->chessboard[startRow][startCol] = '.';

	strncpy(game->moves[game->moveCount].startSquare, move->startSquare, 2);
	strncpy(game->moves[game->moveCount].endSquare, move->endSquare, 3);
	game->moveCount++;
	game->currentPlayer = (game->currentPlayer == WHITE_PLAYER) ? BLACK_PLAYER : WHITE_PLAYER;

	return 0; // Move successful

}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
	// Initial check for null pointers
	if (game == NULL || message == NULL) {
		return COMMAND_ERROR;
	}

	if (strncmp(message, "/move ", 6) == 0) {
		// Process a move command
		ChessMove move;
		if (parse_move(message + 6, &move) != 0 || make_move(game, &move, is_client, true) != 0) {
			return COMMAND_ERROR;
		}
		send(socketfd, message, strlen(message), 0);
		return COMMAND_MOVE;
	} else if (strcmp(message, "/forfeit") == 0) {
		// Process a forfeit command
		send(socketfd, message, strlen(message), 0);
		return COMMAND_FORFEIT;
	} else if (strcmp(message, "/chessboard") == 0) {
		// Process a chessboard command
		display_chessboard(game);
		return COMMAND_DISPLAY;
	} else if (strncmp(message, "/import ", 8) == 0 && !is_client) {
		// Server side import command
		fen_to_chessboard(message + 8, game);
		send(socketfd, message, strlen(message), 0);
		return COMMAND_IMPORT;
	} else if (strncmp(message, "/load ", 6) == 0) {
		// Load a game
		// This is simplified; actual argument parsing is required
		char **args = split_string(message + 6, ' ');
		if (args[0] == NULL || args[1] == NULL) {
			free(args);
			return COMMAND_ERROR;
		}
		if (load_game(game, args[0], "game_database.txt", atoi(args[1])) != 0) {
			free(args);
			return COMMAND_ERROR;
		}
		send(socketfd, message, strlen(message), 0);
		free(args);
		return COMMAND_LOAD;
	} else if (strncmp(message, "/save ", 6) == 0) {
		// Save the game
		// Actual username extraction from the command is required
		if (save_game(game, 6 + message, "game_database.txt") != 0) {
			return COMMAND_ERROR;
		}
		return COMMAND_SAVE;
	} else {
		// Command is unknown
		return COMMAND_UNKNOWN;
	}

}

char **split_string(const char *string, char i) {
	// Count the number of occurrences of the delimiter
	int count = 0;
	for (const char *c = string; *c; c++) {
		if (*c == i) {
			count++;
		}
	}

	// Allocate memory for the array of strings
	char **result = malloc((count + 2) * sizeof(char *));
	if (!result) {
		return NULL;
	}

	// Split the string
	int index = 0;
	const char *start = string;
	for (const char *c = string; *c; c++) {
		if (*c == i) {
			result[index] = malloc(c - start + 1);
			if (!result[index]) {
				// Free memory allocated so far
				for (int j = 0; j < index; j++) {
					free(result[j]);
				}
				free(result);
				return NULL;
			}
			strncpy(result[index], start, c - start);
			result[index][c - start] = '\0';
			start = c + 1;
			index++;
		}
	}
	result[index] = malloc(strlen(start) + 1);
	if (!result[index]) {
		// Free memory allocated so far
		for (int j = 0; j < index; j++) {
			free(result[j]);
		}
		free(result);
		return NULL;
	}
	strcpy(result[index], start);
	result[index + 1] = NULL;

	return result;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
	// Initial check for null pointers
	if (game == NULL || message == NULL) {
		return COMMAND_ERROR;
	}

	if (strncmp(message, "/move ", 6) == 0) {
		// Process a move command
		ChessMove move;
		if (parse_move(message + 6, &move) != 0) {
			return COMMAND_ERROR;
		}
		make_move(game, &move, is_client, false);
		return COMMAND_MOVE;
	} else if (strcmp(message, "/forfeit") == 0) {
		// Process a forfeit command
		close(socketfd); // Assuming close is available and correct
		return COMMAND_FORFEIT;
	} else if (strncmp(message, "/import ", 8) == 0 && is_client) {
		// Client side import command
		fen_to_chessboard(message + 8, game);
		return COMMAND_IMPORT;
	} else if (strncmp(message, "/load ", 6) == 0) {
		// Load a game
		char **args = split_string(message + 6, ' ');

		if (args[0] == NULL || args[1] == NULL) {
			free(args);
			return COMMAND_ERROR;
		}

		printf("Loading game for user %s, save number %d\n", args[0], atoi(args[1]));
		if (load_game(game, args[0], "game_database.txt", atoi(args[1])) != 0) {
			free(args);
			return COMMAND_ERROR;
		}
		free(args);
		return COMMAND_LOAD;
	} else {
		// If no command matches
		return -1; // As per requirement, return -1 if no command matches
	}
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
	// A string representing the username associated with the game state. The
	//username may not contain spaces and cannot be empty.

	if (strchr(username, ' ') != NULL || strlen(username) == 0) {
		return -1; // Error: Invalid username
	}


	FILE *file = fopen(db_filename, "a"); // Open file in append mode
	if (!file) {
		perror("Failed to open file");
		return -1; // Error opening file
	}

	char fen[100]; // Buffer for FEN string; adjust size as needed
	chessboard_to_fen(fen, game); // Generate FEN string from current game state

	// Write username and FEN string to file
	if (fprintf(file, "%s:%s\n", username, fen) < 0) {
		fclose(file); // Error writing to file, ensure file is closed
		return -1;
	}

	fclose(file); // Close the file
	return 0; // Success

	(void) game;
	(void) username;
	(void) db_filename;

}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
	FILE *file = fopen(db_filename, "r"); // Open file in read mode
	if (!file) {
		perror("Failed to open file");
		return -1; // Error opening file
	}

	char line[150]; // Buffer for each line in the file; adjust size as needed
	int current_save = 0;
	while (fgets(line, sizeof(line), file)) {
		char *found_username = strtok(line, ":");
		if (found_username && strcmp(found_username, username) == 0) {
			current_save++;
			if (current_save == save_number) {
				char *fen = strtok(NULL, "\n");
				if (fen) {
					fen_to_chessboard(fen, game); // Load the FEN string into the game
					fclose(file);
					return 0; // Success
				}
			}
		}
	}

	fclose(file); // Close the file
	return -1; // Save not found or error occurred

	(void) game;
	(void) username;
	(void) db_filename;
	(void) save_number;

}

void display_chessboard(ChessGame *game) {
	printf("\nChessboard:\n");
	printf("  a b c d e f g h\n");
	for (int i = 0; i < 8; i++) {
		printf("%d ", 8 - i);
		for (int j = 0; j < 8; j++) {
			printf("%c ", game->chessboard[i][j]);
		}
		printf("%d\n", 8 - i);
	}
	printf("  a b c d e f g h\n");
}
