// Search.h

#include <bits/stdc++.h>
#include "Board.h"
#include "MoveGenerator.h"
using namespace std;



class Search{
public:
    static long long CountPositionsAtDepth(Board& board, int depth){
        if(depth==0) return 1;
        long long res = 0;
        vector<string> legalMoves = MoveGenerator::LegalMoves(board);
        for(string& move : legalMoves){
            board.MakeMove(move);
            res+=CountPositionsAtDepth(board, depth-1);
            board.UndoMove();
        }
        return res;
    }

};