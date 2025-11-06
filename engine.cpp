// engine.cpp

#include <bits/stdc++.h>
#include "Board.h"
#include "MoveGenerator.h"
using namespace std;

Board board;

long long RandInt(long long l, long long r){
    long long res = 0;
    for(int i = 0;i<4;i++) res = (res<<15)+(rand()&((1<<15)-1));
    return l+res%(r-l+1);
}


void handle_opponent_move(const string& opponent_move, Board& board) {
    string move_to_play;
    if(opponent_move!="aaaa") board.MakeMove(opponent_move);
    // generate move here
    
    vector<string> legalMoves = MoveGenerator::LegalMoves(board);
    // string abc = Board::FormatMove({0,0},{0,0});
    move_to_play = legalMoves[RandInt(0, legalMoves.size()-1)];
    board.MakeMove(move_to_play);
    cout << move_to_play << endl; 

    // for(string& move : legalMoves) cout<<move<<' ';
    // cout<<endl;
    // cout<<"---------------------------------------------------"<<endl;

    // board.PrintBoard();
}


int main() {
    string line = "black";
    
    getline(cin,line);

    board.InitBoard(board.startingFEN, line == "white");

    // The persistent loop: The C++ engine waits for a command from Python
    while (getline(cin, line)) {
        
        handle_opponent_move(line, board);

        
    }




    // string abc;
    // int cnt = 21;
    // while(cnt--){
    //     cin>>abc;
    //     board.MakeMove(abc);
    // }

    // vector<string> v = MoveGenerator::LegalMoves(board);
    // for(string x : v) cout<<x<<' ';

    return 0; // Clean exit
}


// e2e3 e7e6 d1f3 g7g5 f1b5 f8c5 g1e2 h7h6 b2b3 b8c6 d2d4 
// c5b4 b1d2 c6e7 h2h4 a7a6 c2c3 f7f6 b5d3 e7d5 f3h5 b4f8