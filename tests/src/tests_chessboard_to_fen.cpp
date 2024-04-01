#include "test_utils.h"
#include <fstream>

extern "C" {
#include "hw4.h"
}

void initialize_game_soln(ChessGame *game);
int make_move_soln(ChessGame *game, ChessMove *move, bool is_client, bool validate_move);
void chessboard_to_fen_soln(char fen[], ChessGame *game);
void fen_to_chessboard_soln(const char *fen, ChessGame *game);

class chessboard_to_fen_TestSuite : public testing::Test { 
};

TEST_F(chessboard_to_fen_TestSuite, board_to_fen01) {
    INFO("Convert a chessboard to a FEN string after a sequence of moves.");
    ChessGame game;
    initialize_game_soln(&game);
    
    ChessMove moves[] = {
        {"e2", "e4"}, // w
        {"c7", "c6"}, // b
        {"b1", "c3"}, // w
        {"d7", "d6"}, // b
        {"d2", "d4"}, // w
        {"b8", "d7"}, // b
        {"g1", "f3"}, // w
        {"e7", "e5"}, // b
        {"f1", "c4"}, // w
        {"f8", "e7"}  // b
    };

    for (int i = 0; i < 10; i++) {
        make_move_soln(&game, &moves[i], (i + 1) % 2, 1);
    }
    
    char act_fen[BUFFER_SIZE];
    chessboard_to_fen(act_fen, &game);
    char exp_fen[BUFFER_SIZE];
    chessboard_to_fen_soln(exp_fen, &game);
    EXPECT_STREQ(exp_fen, act_fen);
}


TEST_F(chessboard_to_fen_TestSuite, board_to_fen02) {
    INFO("Convert a chessboard to a FEN string after a sequence of moves.");
    ChessGame game;
    initialize_game_soln(&game);
    
    ChessMove moves[] = {
        {"e2", "e4"},
        {"e7", "e5"},
        {"g1", "f3"},
        {"b8", "c6"},
        {"f1", "b5"},
        {"a7", "a6"},
        {"b5", "a4"},
        {"g8", "f6"}
    };

    for (int i = 0; i < 8; i++) {
        make_move_soln(&game, &moves[i], (i + 1) % 2, 1);
    }
    
    char act_fen[BUFFER_SIZE];
    chessboard_to_fen(act_fen, &game);
    char exp_fen[BUFFER_SIZE];
    chessboard_to_fen_soln(exp_fen, &game);
    EXPECT_STREQ(exp_fen, act_fen);
    // C89 Ruy Lopez: Marshall, Main line, Spassky variation (up to the castling part of the opening since castling isn't in specifications)
}

TEST_F(chessboard_to_fen_TestSuite, board_to_fen03) {
    INFO("Convert a chessboard to a FEN string after a sequence of moves.");
    ChessGame game;
    initialize_game_soln(&game);
    
    ChessMove moves[] = {
        {"e2", "e4"},
        {"e7", "e5"},
        {"g1", "f3"},
        {"b8", "c6"},
        {"f1", "b5"}
    };

    for (int i = 0; i < 5; i++) {
        make_move_soln(&game, &moves[i], (i + 1) % 2, 1);
    }
    
    char act_fen[BUFFER_SIZE];
    chessboard_to_fen(act_fen, &game);
    char exp_fen[BUFFER_SIZE];
    chessboard_to_fen_soln(exp_fen, &game);
    EXPECT_STREQ(exp_fen, act_fen);
}


TEST_F(chessboard_to_fen_TestSuite, board_to_fen04) {
    INFO("Convert a chessboard to a FEN string after a sequence of moves.");
    ChessGame game;
    initialize_game_soln(&game);
    
    ChessMove moves[] = {
        {"e2", "e4"},
        {"h7", "h5"},
        {"a2", "a4"},
        {"h8", "h7"},
        {"b2", "b4"},
        {"c7", "c5"},
        {"d2", "d4"},
        {"f7", "f5"},
        {"h2", "h4"},
        {"g8", "f6"},
        {"b1", "d2"},
        {"b8", "c6"},
        {"g1", "f3"},
        {"a8", "b8"},
        {"a1", "b1"},
        {"c6", "d4"},
        {"b1", "b3"},
        {"h7", "h8"}
    };

    for (int i = 0; i < 18; i++) {
        make_move_soln(&game, &moves[i], (i + 1) % 2, 1);
    }
    
    char act_fen[BUFFER_SIZE];
    chessboard_to_fen(act_fen, &game);
    char exp_fen[BUFFER_SIZE];
    chessboard_to_fen_soln(exp_fen, &game);
    EXPECT_STREQ(exp_fen, act_fen);
}

TEST_F(chessboard_to_fen_TestSuite, board_to_fen05) {
    INFO("Convert a chessboard to a FEN string given a random board.");
    ChessGame game;
    initialize_game_soln(&game);

    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    fen_to_chessboard_soln(fen, &game);

    char test_fen[BUFFER_SIZE];
    chessboard_to_fen(test_fen, &game);
    EXPECT_STREQ(fen, test_fen);
    EXPECT_EQ(WHITE_PLAYER, game.currentPlayer);
}


TEST_F(chessboard_to_fen_TestSuite, board_to_fen06) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkb1r/pppp1ppp/4pn2/8/2PP4/6P1/PP2PP1P/RNBQKBNR b";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(BLACK_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board22) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "r1bqkbnr/pppp1ppp/2n5/4p3/3PP3/5N2/PPP2PPP/RNBQKB1R b";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(BLACK_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board23) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkb1r/pppp1ppp/5n2/4p3/4P3/2N2N2/PPPP1PPP/R1BQKB1R b";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(BLACK_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board24) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkbnr/pppp1p1p/8/6p1/4Pp2/5N2/PPPP2PP/RNBQKB1R w";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(WHITE_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board25) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkbnr/ppp2ppp/4p3/8/3Pp3/2N5/PPP2PPP/R1BQKBNR w";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(WHITE_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board26) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkbnr/ppp2ppp/4p3/3p4/3PP3/8/PPPN1PPP/R1BQKBNR b";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(BLACK_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board27) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqk1nr/ppp2ppp/4p3/3p4/1b1PP3/2N5/PPP2PPP/R1BQKBNR w";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(WHITE_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board28) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(WHITE_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board29) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkb1r/ppp1pp1p/3p1np1/8/3PP3/2N2N2/PPP2PPP/R1BQKB1R b";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(BLACK_PLAYER, act_game.currentPlayer);
}

TEST_F(chessboard_to_fen_TestSuite, fen_to_board30) {
    INFO("Convert a FEN string to a chessboard given a random board.");
    const char *fen = "rnbqkb1r/p2ppppp/5n2/1ppP4/2P5/8/PP2PPPP/RNBQKBNR w";
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game);
    structs_equal(&exp_game, &act_game);
    EXPECT_EQ(WHITE_PLAYER, act_game.currentPlayer);
}
