// MoveGenerator.h

#pragma once
#include <bits/stdc++.h>
#include "Piece.h"
#include "Board.h"
using namespace std;

class MoveGenerator{
public:
    static vector<string> PseudoLegalMoves(Board& board){
        vector<string> generatedMoves;
        generatedMoves.clear();
        int matchColor = Piece::Black;
        if(board.isWhiteTurn) matchColor = Piece::White;

        for(int i = 0;i<8;i++){
            for(int j = 0;j<8;j++){
                int piece = board.board[i][j];
                pair<int,int> fromCoord = make_pair(i,j);
                if(Piece::Color(piece) != matchColor) continue;
                

                //handle bishop or queen
                if(Piece::IsBishopOrQueen(piece)){
                    for(int z = 0;z<4;z++){
                        pair<int,int> toCoord = make_pair(i,j);
                        while(true){
                            toCoord.first += Piece::bishopMoves[z].first;
                            toCoord.second += Piece::bishopMoves[z].second;
                            if(!Board::InsideBoard(toCoord)) break;

                            int valueAtTarget = board.board[toCoord.first][toCoord.second];

                            if(Piece::Color(valueAtTarget) == matchColor) break;
                            generatedMoves.push_back(Board::FormatMove(fromCoord, toCoord));

                            if(valueAtTarget != Piece::None) break;
                        }
                        
                    }
                }

                //handle rook or queen
                if(Piece::IsRookOrQueen(piece)){
                    for(int z = 0;z<4;z++){
                        pair<int,int> toCoord = make_pair(i,j);
                        while(true){
                            toCoord.first += Piece::rookMoves[z].first;
                            toCoord.second += Piece::rookMoves[z].second;
                            if(!Board::InsideBoard(toCoord)) break;

                            int valueAtTarget = board.board[toCoord.first][toCoord.second];

                            if(Piece::Color(valueAtTarget) == matchColor) break;
                            generatedMoves.push_back(Board::FormatMove(fromCoord, toCoord));

                            if(valueAtTarget != Piece::None) break;
                        }
                        
                    }
                }

                //handle king
                if(Piece::Type(piece) == Piece::King){
                    for(int z = 0;z<8;z++){
                        int x = i + Piece::kingMoves[z].first;
                        int y = j + Piece::kingMoves[z].second;
                        pair<int,int> toCoord = make_pair(x,y);
                        if(!Board::InsideBoard(toCoord)) continue;

                        int valueAtTarget = board.board[x][y];
                        
                        if(Piece::Color(valueAtTarget) == matchColor) continue;
                        generatedMoves.push_back(Board::FormatMove(fromCoord, toCoord));
                    }
                }

                //handle knight
                if(Piece::Type(piece) == Piece::Knight){
                    for(int z = 0;z<8;z++){
                        int x = i + Piece::knightMoves[z].first;
                        int y = j + Piece::knightMoves[z].second;
                        pair<int,int> toCoord = make_pair(x,y);
                        if(!Board::InsideBoard(toCoord)) continue;
                        
                        int valueAtTarget = board.board[x][y];
                        
                        if(Piece::Color(valueAtTarget) == matchColor) continue;
                        generatedMoves.push_back(Board::FormatMove(fromCoord, toCoord));
                    }
                }

                //handle pawn
                if(Piece::Type(piece) == Piece::Pawn){
                    
                    int x = (matchColor == Piece::White)? i-1 : i+1;
                    int y = j;
                    
                    
                    //normal push
                    if(!Board::InsideBoard(make_pair(x,y))) continue;

                    int valueAtTarget = board.board[x][y];

                    if(valueAtTarget == Piece::None){
                        string tmp = Board::FormatMove(fromCoord, make_pair(x,y));
                        

                        //handle promotion
                        if(x==0 || x==7){
                            generatedMoves.push_back(tmp+'q');
                            generatedMoves.push_back(tmp+'r');
                            generatedMoves.push_back(tmp+'b');
                            generatedMoves.push_back(tmp+'n');
                        }
                        else generatedMoves.push_back(tmp);

                    }
                        

                    //handle captures
                    for(y = j-1; y<=j+1; y+=2){
                        if(!Board::InsideBoard(make_pair(x,y))) continue;
                        valueAtTarget = board.board[x][y];
                        
                        if(valueAtTarget == Piece::None) continue;
                        if(Piece::Color(valueAtTarget) == matchColor) continue;
                        
                        string tmp = Board::FormatMove(fromCoord, make_pair(x,y));
                        

                        //handle promotion
                        if(x==0 || x==7){
                            generatedMoves.push_back(tmp+'q');
                            generatedMoves.push_back(tmp+'r');
                            generatedMoves.push_back(tmp+'b');
                            generatedMoves.push_back(tmp+'n');
                        }
                        else generatedMoves.push_back(tmp);
                    }
                    
                    //handle double step
                    y = j;
                    if(i==6 && (matchColor == Piece::White)){
                        valueAtTarget = board.board[4][y];
                        int pieceInMiddle = board.board[5][y];
                        if(valueAtTarget == Piece::None && pieceInMiddle == Piece::None){
                            generatedMoves.push_back(Board::FormatMove(fromCoord, make_pair(4,y)));
                        }
                    }

                    if(i==1 && (matchColor == Piece::Black)){
                        valueAtTarget = board.board[3][y];
                        int pieceInMiddle = board.board[2][y];
                        if(valueAtTarget == Piece::None && pieceInMiddle == Piece::None){
                            generatedMoves.push_back(Board::FormatMove(fromCoord, make_pair(3,y)));
                        }
                    }

                    //handle EP
                    int EP = board.enPassantTarget;
                    if(EP != -1) for(y = j-1; y<=j+1; y+=2){
                        if(EP != Board::CoordToIndex(make_pair(x,y))) continue;
                        generatedMoves.push_back(Board::FormatMove(fromCoord, make_pair(x,y)));
                    }
                }
            }
        }


        // --- Castling Moves (Pseudo-Legal: checks flags and empty squares) ---
        // The full legality (King not in check, not passing through check) is checked in LegalMoves()
        if(matchColor == Piece::White){
            // Kingside Castling (e1g1)
            if (board.whiteCastleKingSide && 
                board.board[7][5] == Piece::None && 
                board.board[7][6] == Piece::None &&
                !board.IsSquareAttacked(make_pair(7,4), 0) &&
                !board.IsSquareAttacked(make_pair(7,5), 0) &&
                !board.IsSquareAttacked(make_pair(7,6), 0)) {


                generatedMoves.push_back("e1g1");
            }
            // Queenside Castling (e1c1)
            if (board.whiteCastleQueenSide &&
                board.board[7][1] == Piece::None &&
                board.board[7][2] == Piece::None &&
                board.board[7][3] == Piece::None &&
                !board.IsSquareAttacked(make_pair(7,1), 0) &&
                !board.IsSquareAttacked(make_pair(7,2), 0) &&
                !board.IsSquareAttacked(make_pair(7,3), 0) &&
                !board.IsSquareAttacked(make_pair(7,4), 0)) {


                generatedMoves.push_back("e1c1");
            }
        } 
        else{ // Black
            // Kingside Castling (e8g8)
            if(board.blackCastleKingSide && 
                board.board[0][5] == Piece::None && 
                board.board[0][6] == Piece::None &&
                !board.IsSquareAttacked(make_pair(0,4), 1) &&
                !board.IsSquareAttacked(make_pair(0,5), 1) &&
                !board.IsSquareAttacked(make_pair(0,6), 1)){


                generatedMoves.push_back("e8g8");
            }
            // Queenside Castling (e8c8)
            if(board.blackCastleQueenSide && 
                board.board[0][1] == Piece::None && 
                board.board[0][2] == Piece::None && 
                board.board[0][3] == Piece::None &&
                !board.IsSquareAttacked(make_pair(0,1), 1) &&
                !board.IsSquareAttacked(make_pair(0,2), 1) &&
                !board.IsSquareAttacked(make_pair(0,3), 1) &&
                !board.IsSquareAttacked(make_pair(0,4), 1)){


                generatedMoves.push_back("e8c8");
            }
        }


        

        return generatedMoves;

    }

    static vector<string> LegalMoves(Board& board){
        vector<string> pseudoLegalMoves = PseudoLegalMoves(board);
        vector<string> res;
        res.clear();
        for(string& move : pseudoLegalMoves){
            board.MakeMove(move);
            if(board.IsKingSafe()) res.push_back(move);
            board.UndoMove();
        }
        
        return res;

    }
};