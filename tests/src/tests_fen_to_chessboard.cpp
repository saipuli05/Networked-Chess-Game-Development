#include "test_utils.h"
#include <fstream>

extern "C" {
#include "hw4.h"
}

void initialize_game_soln(ChessGame *game);
void fen_to_chessboard_soln(const char *fen, ChessGame *game);

class fen_to_chessboard_TestSuite : public testing::Test { 
};

TEST_F(fen_to_chessboard_TestSuite, fen_to_board01) {
    const char *fen = "r1bqkbnr/pp1ppp1p/2n3p1/8/3NP3/8/PPP2PPP/RNBQKB1R w";  // B32 Sicilian Defense: Accelerated Dragon
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game); 
    structs_equal(&exp_game, &act_game);
}

TEST_F(fen_to_chessboard_TestSuite, fen_to_board02) {
    const char *fen = "5rk1/5ppp/p1pbr3/1p1n3q/P2P2b1/1BPQB1P1/1P1N1P1P/R3R1K1 w"; //  C89 Ruy Lopez: Marshall, Main line, Spassky variation
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game); 
    structs_equal(&exp_game, &act_game);
}

TEST_F(fen_to_chessboard_TestSuite, fen_to_board03) {
    const char *fen = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPPKPPP/RNBQ1BNR b"; // Bongcloud Attack
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game); 
    structs_equal(&exp_game, &act_game);
}

TEST_F(fen_to_chessboard_TestSuite, fen_to_board04) {
    const char *fen = "p6p/8/1rp2p2/3pP3/2pPbpP1/1PKpkBr1/1RNQNPR1/q1nBnb1P b"; // Random board
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game); 
    structs_equal(&exp_game, &act_game);
}

TEST_F(fen_to_chessboard_TestSuite, fen_to_board05) {
    const char *fen = "3k4/8/8/8/8/8/8/4K3 w"; // Random board
    ChessGame exp_game;
    initialize_game_soln(&exp_game);
    fen_to_chessboard_soln(fen, &exp_game);
    ChessGame act_game;
    initialize_game(&act_game);
    fen_to_chessboard(fen, &act_game); 
    structs_equal(&exp_game, &act_game);
}
