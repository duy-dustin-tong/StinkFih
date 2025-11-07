#include <bits/stdc++.h>
#include "Board.h"
#include "Piece.h"
#include "Search.h"
using namespace std;

Board board;

int main(){
    board.InitBoard("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ", true);
    
    string move;
    int n;
    cin>>n;
    for(int i = 0;i<=n;i++){
        auto start = chrono::steady_clock::now();
        long long cnt = Search::CountPositionsAtDepth(board, i);
        auto end = chrono::steady_clock::now();
        
    
        auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start);
        cout<<cnt<<' '<<elapsed_ms.count()<<endl;
    }

    // int d = 2;
    // auto start = chrono::steady_clock::now();
    // long long cnt = Search::CountPositionsAtDepth(board, d);
    // auto end = chrono::steady_clock::now();
    // auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // cout<<cnt<<' '<<elapsed_ms.count()<<endl;
    
    // while(true){
    //     cin>>move;
    //     if(move =="q") return 0;
    //     if(move == "z") board.UndoMove(), d++;
    //     else if(move != "a") board.MakeMove(move), d--;
        
    //     auto start = chrono::steady_clock::now();
    //     long long cnt = Search::CountPositionsAtDepth(board, d);
    //     auto end = chrono::steady_clock::now();
        
    
    //     auto elapsed_ms = chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //     cout<<cnt<<' '<<elapsed_ms.count()<<endl;
        
    // }
    
}