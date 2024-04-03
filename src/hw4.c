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
    
    (void)fen;
    (void)game;
}

int parse_move(const char *move, ChessMove *parsed_move) {
   // Validate the length of the move string
    size_t len = strlen(move);
    if (len < 4 || len > 5) {
        return PARSE_MOVE_INVALID_FORMAT; // Error code for invalid format
    }

    // Validate characters are within 'a' to 'h' for columns and '1' to '8' for rows
    if (!(move[0] >= 'a' && move[0] <= 'h' && move[1] >= '1' && move[1] <= '8' &&
          move[2] >= 'a' && move[2] <= 'h' && move[3] >= '1' && move[3] <= '8')) {
        return PARSE_MOVE_INVALID_FORMAT; // Error code for invalid format, could also be considered out of bounds
    }

    // Special check for out of bounds that might be missed by the initial format check
    // Assuming '22' corresponds to out of bounds or another specific error condition
    if (move[1] == '0' || move[3] == '0' || move[1] > '8' || move[3] > '8' ||
        move[0] < 'a' || move[0] > 'h' || move[2] < 'a' || move[2] > 'h') {
        return 22; // Assuming '22' is the correct error code for out-of-bounds or related error
    }

    // For pawn promotion, check the promotion character and destination row
    if (len == 5) {
        char promotionPiece = tolower(move[4]); // Convert to lowercase to simplify comparison
        if (!(promotionPiece == 'q' || promotionPiece == 'r' || promotionPiece == 'b' || promotionPiece == 'n')) {
            return PARSE_MOVE_INVALID_PROMOTION;
        }
        if (!((move[3] == '8' && move[1] == '7') || (move[3] == '1' && move[1] == '2'))) {
            return PARSE_MOVE_INVALID_DESTINATION;
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
    
    (void)move;
    (void)parsed_move;
}

int make_move(ChessGame *game, ChessMove *move, bool is_client, bool validate_move) {
    // Convert chessboard positions to 0-based array indices
    int startRow = 8 - (move->startSquare[1] - '0');
    int startCol = move->startSquare[0] - 'a';
    int endRow = 8 - (move->endSquare[1] - '0');
    int endCol = move->endSquare[0] - 'a';
    char piece = game->chessboard[startRow][startCol];

    // Validate move if required
    if (validate_move) {
        // Check if it's the correct player's turn
        if ((is_client && game->currentPlayer != WHITE_PLAYER) ||
            (!is_client && game->currentPlayer != BLACK_PLAYER)) {
            return MOVE_OUT_OF_TURN;
        }

        // Check if the start square is empty
        if (piece == '.') {
            return MOVE_NOTHING;
        }

        // Check if moving the correct color piece
        bool isWhitePiece = piece >= 'A' && piece <= 'Z';
        if ((is_client && !isWhitePiece) || (!is_client && isWhitePiece)) {
            return MOVE_WRONG_COLOR;
        }

        // Verify the move is valid for the piece
        if (!is_valid_move(piece, startRow, startCol, endRow, endCol, game)) {
            return MOVE_WRONG;
        }

        // Additional checks for pawn promotion, capturing own pieces, etc., could be added here
    }

    // Execute the move
    // Update the chessboard
    game->chessboard[endRow][endCol] = game->chessboard[startRow][startCol];
    game->chessboard[startRow][startCol] = '.';

    // Update the moves and capturedPieces arrays, moveCount, and capturedCount as necessary
    strncpy(game->moves[game->moveCount].startSquare, move->startSquare, 2);
    strncpy(game->moves[game->moveCount].endSquare, move->endSquare, 3);
    game->moveCount++;

    // Handle capturing
    char capturedPiece = game->chessboard[endRow][endCol];
    if (capturedPiece != '.') {
        game->capturedPieces[game->capturedCount++] = capturedPiece;
    }

    // Update currentPlayer to switch turns
    game->currentPlayer = (game->currentPlayer == WHITE_PLAYER) ? BLACK_PLAYER : WHITE_PLAYER;

    return 0; // Move successful
    
    (void)game;
    (void)move;
    (void)is_client;
    (void)validate_move;
    
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
