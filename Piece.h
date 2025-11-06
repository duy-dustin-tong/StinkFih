// Piece.h

#pragma once
#include <bits/stdc++.h>
using namespace std;
/**
 * @brief Defines the constants and helper methods for chess pieces.
 * * Includes piece types, color masks, and static arrays for basic move vectors.
 */
class Piece{
public:
    // Piece Types
    const static int None = 0;
    const static int King = 1;      // 00001
    const static int Pawn = 2;      // 00010
    const static int Knight = 3;    // 00011
    const static int Bishop = 5;    // 00101
    const static int Rook = 6;      // 00110
    const static int Queen = 7;     // 00111

    // Color Masks
    const static int White = 8;     // 01000
    const static int Black = 16;    // 10000

    // Masks for bitwise operations
    const static int typeMask = 0b00111;
    const static int blackMask = 0b10000;
    const static int whiteMask = 0b01000;
    const static int colorMask = whiteMask | blackMask;

    // Declarations for static move arrays (definitions in Piece.cpp)
    static const pair<int,int> knightMoves[8];
    static const pair<int,int> rookMoves[4];
    static const pair<int,int> bishopMoves[4];
    static const pair<int,int> kingMoves[8];
    
    // Static Helper Methods (declared here, implemented here or in cpp if complex)
    
    /** Returns the color mask (Piece::White or Piece::Black) of a piece. */
    static int Color(int piece){
        return piece & colorMask;
    }

    /** Returns the type of a piece (Piece::King, Piece::Pawn, etc.). */
    static int Type(int piece){
        return piece & typeMask;
    }

    /** Checks if a piece is a Rook or Queen (for sliding moves). */
    static bool IsRookOrQueen(int piece){
        return (piece & 6) == 6; // Rook is 6, Queen is 7. Both have the 0b110 bits set.
    }

    /** Checks if a piece is a Bishop or Queen (for sliding moves). */
    static bool IsBishopOrQueen(int piece){
        return (piece & 5) == 5; // Bishop is 5, Queen is 7. Both have the 0b101 bits set.
    }
};
