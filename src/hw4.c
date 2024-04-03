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
    (void)game;
}

void chessboard_to_fen(char fen[], ChessGame *game) {
    (void)fen;
    (void)game;
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
        else if (src_row == 6 && col_diff == 0 && row_diff == -2 && game->chessboard[dest_row][dest_col] == '.' && game->chessboard[dest_row + 1][dest_col] == '.') {
            return true;
        }
        // Check for capturing move
        else if (abs(col_diff) == 1 && row_diff == -1 && game->chessboard[dest_row][dest_col] != '.' && islower(game->chessboard[dest_row][dest_col])) {
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
        else if (src_row == 1 && col_diff == 0 && row_diff == 2 && game->chessboard[dest_row][dest_col] == '.' && game->chessboard[dest_row - 1][dest_col] == '.') {
            return true;
        }
        // Check for capturing move
        else if (abs(col_diff) == 1 && row_diff == 1 && game->chessboard[dest_row][dest_col] != '.' && isupper(game->chessboard[dest_row][dest_col])) {
            return true;
        }
    }
    
    (void)piece;
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
    (void)game;
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
    
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
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
    
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
    (void)game;
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
    
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
    (void)game;
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
   
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
   
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
    
    (void)piece;
    (void)src_row;
    (void)src_col;
    (void)dest_row;
    (void)dest_col;
    (void)game;
    
}

void fen_to_chessboard(const char *fen, ChessGame *game) {
    (void)fen;
    (void)game;
}

int parse_move(const char *move, ChessMove *parsed_move) {
    // Check the length of the move string
    size_t len = strlen(move);
    if (len < 4 || len > 5) {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    // Check if row letters are within 'a' to 'h' and row numbers within '1' to '8'
    if (move[0] < 'a' || move[0] > 'h' || move[2] < 'a' || move[2] > 'h' ||
        move[1] < '1' || move[1] > '8' || move[3] < '1' || move[3] > '8') {
        return PARSE_MOVE_INVALID_FORMAT;
    }

    // Parse the source and destination squares
    strncpy(parsed_move->startSquare, move, 2);
    parsed_move->startSquare[2] = '\0'; // Null-terminate the string
    
    // For moves without promotion, copy directly; handle promotion separately
    if (len == 4) {
        strncpy(parsed_move->endSquare, move + 2, 2); // Copy the next two characters for the end square
        parsed_move->endSquare[2] = '\0'; // Null-terminate the string
    } else if (len == 5) { // Handle promotion
        strncpy(parsed_move->endSquare, move + 2, 2); // Copy the destination square
        char promotionPiece = tolower(move[4]); // Convert to lowercase to simplify comparison
        if (promotionPiece != 'q' && promotionPiece != 'r' && promotionPiece != 'b' && promotionPiece != 'n') {
            return PARSE_MOVE_INVALID_PROMOTION; // Invalid promotion piece
        }
        parsed_move->endSquare[2] = promotionPiece; // Append the promotion piece to endSquare
        parsed_move->endSquare[3] = '\0'; // Null-terminate the string
    }

    return 0; // Successful parsing
    
    (void)move;
    (void)parsed_move;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    return -999;
}

int send_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int receive_command(ChessGame *game, const char *message, int socketfd, bool is_client) {
    (void)game;
    (void)message;
    (void)socketfd;
    (void)is_client;
    return -999;
}

int save_game(ChessGame *game, const char *username, const char *db_filename) {
    (void)game;
    (void)username;
    (void)db_filename;
    return -999;
}

int load_game(ChessGame *game, const char *username, const char *db_filename, int save_number) {
    (void)game;
    (void)username;
    (void)db_filename;
    (void)save_number;
    return -999;
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
