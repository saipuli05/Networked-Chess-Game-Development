#include "hw4.h"

// Helper functions
int is_valid_column(char c);

int is_valid_row(char c);

bool is_valid_format(const char *move);

char **split_string(const char *string, char i);


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

	
	game->moveCount = 0;
	game->capturedCount = 0;

	game->currentPlayer = WHITE_PLAYER;
	(void) game;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
	int index = 0;

	for (int row = 0; row < 8; row++) {
		int emptyCount = 0;

		for (int col = 0; col < 8; col++) {
			char piece = game->chessboard[row][col];

			if (piece == '.') {
				emptyCount++;
			} else {
				if (emptyCount > 0) {
					
					fen[index++] = '0' + emptyCount;
					emptyCount = 0;
				}
				fen[index++] = piece;
			}
		}

		if (emptyCount > 0) {
			
			fen[index++] = '0' + emptyCount;
		}

		if (row < 7) {
			fen[index++] = '/';
		}
	}

	fen[index++] = ' ';
	fen[index++] = (game->currentPlayer == WHITE_PLAYER) ? 'w' : 'b';
	fen[index] = '\0'; 
	(void) fen;
	(void) game;
}

bool is_valid_pawn_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	
	if (piece != 'P' && piece != 'p') {
		return false;
	}

	
	int row_diff = dest_row - src_row;
	int col_diff = dest_col - src_col;

	
	if (piece == 'P') {
		
		if (col_diff == 0 && row_diff == -1 && game->chessboard[dest_row][dest_col] == '.') {
			return true;
		}
			
		else if (src_row == 6 && col_diff == 0 && row_diff == -2 && game->chessboard[dest_row][dest_col] == '.' &&
				 game->chessboard[dest_row + 1][dest_col] == '.') {
			return true;
		}
			
		else if (abs(col_diff) == 1 && row_diff == -1 && game->chessboard[dest_row][dest_col] != '.' &&
				 islower(game->chessboard[dest_row][dest_col])) {
			return true;
		}
	}
		
	else if (piece == 'p') {
		if (col_diff == 0 && row_diff == 1 && game->chessboard[dest_row][dest_col] == '.') {
			return true;
		}
			
		else if (src_row == 1 && col_diff == 0 && row_diff == 2 && game->chessboard[dest_row][dest_col] == '.' &&
				 game->chessboard[dest_row - 1][dest_col] == '.') {
			return true;
		}
			
		else if (abs(col_diff) == 1 && row_diff == 1 && game->chessboard[dest_row][dest_col] != '.' &&
				 isupper(game->chessboard[dest_row][dest_col])) {
			return true;
		}
	}

	return false;
}

bool is_valid_rook_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	
	if (src_row == dest_row) {
		int step = (dest_col > src_col) ? 1 : -1;
		for (int col = src_col + step; col != dest_col; col += step) {
			if (game->chessboard[src_row][col] != '.') {
				return false; 
			}
		}
	}
		
	else if (src_col == dest_col) {
		int step = (dest_row > src_row) ? 1 : -1;
		for (int row = src_row + step; row != dest_row; row += step) {
			if (game->chessboard[row][src_col] != '.') {
				return false; 
			}
		}
	} else {
		
		return false;
	}

	
	return true;
}

bool is_valid_knight_move(int src_row, int src_col, int dest_row, int dest_col) {
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	
	return (row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2);
}

bool is_valid_bishop_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	if (row_diff != col_diff) {
		return false; 
	}

	int row_step = (dest_row > src_row) ? 1 : -1;
	int col_step = (dest_col > src_col) ? 1 : -1;

	int cur_row = src_row + row_step;
	int cur_col = src_col + col_step;
	while (cur_row != dest_row && cur_col != dest_col) {
		if (game->chessboard[cur_row][cur_col] != '.') {
			return false; 
		}
		cur_row += row_step;
		cur_col += col_step;
	}

	return true;
}

bool is_clear_path(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	int row_diff = dest_row - src_row;
	int col_diff = dest_col - src_col;
	int row_step = (row_diff != 0) ? (row_diff / abs(row_diff)) : 0; 
	int col_step = (col_diff != 0) ? (col_diff / abs(col_diff)) : 0;

	int cur_row = src_row + row_step;
	int cur_col = src_col + col_step;
	while (cur_row != dest_row || cur_col != dest_col) {
		if (game->chessboard[cur_row][cur_col] != '.') {
			return false; 
		}
		cur_row += row_step;
		cur_col += col_step;
	}
	return true;
}

bool is_valid_queen_move(int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	if (src_row == dest_row || src_col == dest_col) {
		return is_clear_path(src_row, src_col, dest_row, dest_col, game);
	}

	if (abs(dest_row - src_row) == abs(dest_col - src_col)) {
		return is_clear_path(src_row, src_col, dest_row, dest_col, game);
	}

	return false;

}

bool is_valid_king_move(int src_row, int src_col, int dest_row, int dest_col) {
	int row_diff = abs(dest_row - src_row);
	int col_diff = abs(dest_col - src_col);

	if ((row_diff == 1 || row_diff == 0) && (col_diff == 1 || col_diff == 0) && !(row_diff == 0 && col_diff == 0)) {
		return true;
	} else {
		return false;
	}

}

bool is_valid_move(char piece, int src_row, int src_col, int dest_row, int dest_col, ChessGame *game) {
	char piece_type = tolower(piece);
	bool result = false;

	switch (piece_type) {
		case 'p': 
			result = is_valid_pawn_move(piece, src_row, src_col, dest_row, dest_col, game);
			break;
		case 'r': 
			result = is_valid_rook_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'n': 
			result = is_valid_knight_move(src_row, src_col, dest_row, dest_col);
			break;
		case 'b': 
			result = is_valid_bishop_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'q': 
			result = is_valid_queen_move(src_row, src_col, dest_row, dest_col, game);
			break;
		case 'k': 
			result = is_valid_king_move(src_row, src_col, dest_row, dest_col);
			break;
		default:
			
			result = false;
	}

	return result;

}

void fen_to_chessboard(const char *fen, ChessGame *game) {
	int row = 0, col = 0;

	while (*fen != ' ') { 
		if (*fen == '/') {
			row++; 
			col = 0; 
		} else if (isdigit(*fen)) {
			int emptySpaces = *fen - '0';
			for (int i = 0; i < emptySpaces; i++) {
				game->chessboard[row][col++] = '.'; 
			}
		} else {
			game->chessboard[row][col++] = *fen; 
		}
		fen++; 
	}

	fen++; 
	game->currentPlayer = (*fen == 'w') ? WHITE_PLAYER : BLACK_PLAYER; 

	
	game->moveCount = 0;
	game->capturedCount = 0;

}


int is_valid_row(char c) {
	return c >= 'a' && c <= 'h';
}

int is_valid_column(char c) {
	return c >= '1' && c <= '8';
}

bool is_valid_format(const char *move) {
	size_t len = strlen(move);
	if (len < 4 || len > 5) {
		return false;
	}

	if (!is_valid_row(move[0]) || !is_valid_row(move[2])) {
		return false;
	}

	return true;
}

int parse_move(const char *move, ChessMove *parsed_move) {
	size_t len = strlen(move);

	if (!is_valid_format(move)) {
		return PARSE_MOVE_INVALID_FORMAT;
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

	strncpy(parsed_move->startSquare, move, 2);
	parsed_move->startSquare[2] = '\0'; 
	strncpy(parsed_move->endSquare, move + 2, 2);
	parsed_move->endSquare[2] = '\0'; 
	if (len == 5) {
		parsed_move->endSquare[2] = tolower(move[4]);
		parsed_move->endSquare[3] = '\0'; 
	}

	return 0; 
}


int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {

	int startRow = 8 - (move->startSquare[1] - '0');
	int startCol = move->startSquare[0] - 'a';
	int endRow = 8 - (move->endSquare[1] - '0');
	int endCol = move->endSquare[0] - 'a';
	char piece = game->chessboard[startRow][startCol];
	char endPiece = game->chessboard[endRow][endCol];


	if (validate_move) {

		if ((is_client && game->currentPlayer != WHITE_PLAYER) ||
			(!is_client && game->currentPlayer != BLACK_PLAYER)) {
			return MOVE_OUT_OF_TURN;
		}

		if (piece == '.') {
			return MOVE_NOTHING;
		}
		bool isWhitePiece = piece >= 'A' && piece <= 'Z';
		if ((is_client && !isWhitePiece) || (!is_client && isWhitePiece)) {
			return MOVE_WRONG_COLOR;
		}

		if ((is_client && isupper(endPiece)) || (!is_client && islower(endPiece))) {
			return MOVE_SUS;
		}
		if (strlen(move->endSquare) == 3 && (piece != 'P' && piece != 'p')) {
			return MOVE_NOT_A_PAWN;
		}


		if (strlen(move->endSquare) == 2 && (piece == 'P' || piece == 'p') && (endRow == 0 || endRow == 7)) {
			return MOVE_MISSING_PROMOTION;
		}
		if (!is_valid_move(piece, startRow, startCol, endRow, endCol, game)) {
			return MOVE_WRONG;
		}

	}

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

	return 0; 

}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
	if (game == NULL || message == NULL) {
		return COMMAND_ERROR;
	}

	if (strncmp(message, "/move ", 6) == 0) {
		ChessMove move;
		if (parse_move(message + 6, &move) != 0 || make_move(game, &move, is_client, true) != 0) {
			return COMMAND_ERROR;
		}
		send(socketfd, message, strlen(message), 0);
		return COMMAND_MOVE;
	} else if (strcmp(message, "/forfeit") == 0) {
		send(socketfd, message, strlen(message), 0);
		return COMMAND_FORFEIT;
	} else if (strcmp(message, "/chessboard") == 0) {
		display_chessboard(game);
		return COMMAND_DISPLAY;
	} else if (strncmp(message, "/import ", 8) == 0 && !is_client) {
		fen_to_chessboard(message + 8, game);
		send(socketfd, message, strlen(message), 0);
		return COMMAND_IMPORT;
	} else if (strncmp(message, "/load ", 6) == 0) {
		
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
		
		if (save_game(game, 6 + message, "game_database.txt") != 0) {
			return COMMAND_ERROR;
		}
		return COMMAND_SAVE;
	} else {
		
		return COMMAND_UNKNOWN;
	}

}

char **split_string(const char *string, char i) {
	
	int count = 0;
	for (const char *c = string; *c; c++) {
		if (*c == i) {
			count++;
		}
	}
	char **result = malloc((count + 2) * sizeof(char *));
	if (!result) {
		return NULL;
	}
	int index = 0;
	const char *start = string;
	for (const char *c = string; *c; c++) {
		if (*c == i) {
			result[index] = malloc(c - start + 1);
			if (!result[index]) {
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
	if (game == NULL || message == NULL) {
		return COMMAND_ERROR;
	}

	if (strncmp(message, "/move ", 6) == 0) {
		ChessMove move;
		if (parse_move(message + 6, &move) != 0) {
			return COMMAND_ERROR;
		}
		make_move(game, &move, is_client, false);
		return COMMAND_MOVE;
	} else if (strcmp(message, "/forfeit") == 0) {
		close(socketfd); 
		return COMMAND_FORFEIT;
	} else if (strncmp(message, "/import ", 8) == 0 && is_client) {
		fen_to_chessboard(message + 8, game);
		return COMMAND_IMPORT;
	} else if (strncmp(message, "/load ", 6) == 0) {
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
		return -1;
	}
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {

	if (strchr(username, ' ') != NULL || strlen(username) == 0) {
		return -1; 
	}


	FILE *file = fopen(db_filename, "a"); 
	if (!file) {
		perror("Failed to open file");
		return -1; 
	}

	char fen[100]; 
	chessboard_to_fen(fen, game); 
	if (fprintf(file, "%s:%s\n", username, fen) < 0) {
		fclose(file); 
		return -1;
	}

	fclose(file); 
	return 0; 
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
	FILE *file = fopen(db_filename, "r"); 
	if (!file) {
		perror("Failed to open file");
		return -1; 
	}

	char line[150]; 
	int current_save = 0;
	while (fgets(line, sizeof(line), file)) {
		char *found_username = strtok(line, ":");
		if (found_username && strcmp(found_username, username) == 0) {
			current_save++;
			if (current_save == save_number) {
				char *fen = strtok(NULL, "\n");
				if (fen) {
					fen_to_chessboard(fen, game); 
					fclose(file);
					return 0; 
				}
			}
		}
	}

	fclose(file); 
	return -1; 

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
