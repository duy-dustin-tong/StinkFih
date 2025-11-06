// Board.h

#pragma once
#include <bits/stdc++.h>
#include "Piece.h" // Requires Piece definition
using namespace std;

/**
 * @brief Represents the main chess board state and move history.
 */
class Board{
public:
    // Starting FEN constant
    const string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; 
    static const char fileID[8];
    static const char rankID[8];

    int board[8][8];
    bool blackCastleKingSide = true;
    bool blackCastleQueenSide = true;
    bool whiteCastleKingSide = true;
    bool whiteCastleQueenSide = true;

    bool botIsWhite = true;
    bool isWhiteTurn = true; // Whose turn it is
    bool missingKing = false;

    // The index 0-63 of the en passant target square. -1 if none.
    int enPassantTarget = -1; 

    pair<int,int> kingSquare; // first = white, second = black

    // Struct to hold state needed to revert a move
    struct BoardChanges{
        // Stores pair of {index 0-63, original piece value} for all changed squares
        vector<pair<int,int>> changedSquares; 
        
        bool blackCastleKingSide;
        bool blackCastleQueenSide;
        bool whiteCastleKingSide;
        bool whiteCastleQueenSide;
        bool isWhiteTurn;
        bool missingking;
        int enPassantTarget;
        pair<int,int> kingSquare;
    };
    stack<BoardChanges> boardHistory;
    
    // Constructors
    Board(bool initial_color = true); 

    // Helper Functions

    static int CoordToIndex(pair<int,int> x){
        if(x.first == -1 || x.second == -1) return -1;
        return x.first * 8 + x.second;
    }

    static pair<int,int> IndexToCoord(int x){
        if(x == -1) return make_pair(-1,-1);
        return make_pair(x / 8, x % 8);
    }

    static bool InsideBoard(pair<int,int> x){
        return 0<=x.first && x.first<8 && 0<=x.second && x.second<8;
    }

    static string FormatMove(pair<int,int> fr, pair<int,int> to){
        int x = fr.first;
        int y = fr.second;
        int u = to.first;
        int v = to.second;
        string res = "";
        res+=fileID[y];
        res+=rankID[x];
        res+=fileID[v];
        res+=rankID[u];
        
        return res;
    }


    // Core Board Management Functions
    void InitBoard(const string& fen, bool x);
    void MakeMove(const string& move);
    void UndoMove();
    void PrintBoard();
    bool IsSquareAttacked(pair<int,int> square, bool byWhite);
    bool IsKingSafe();

};
