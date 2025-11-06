// Piece.cpp

#include <bits/stdc++.h>
#include "Piece.h"
using namespace std;
// Define the static move arrays exactly once
const pair<int,int> Piece::knightMoves[8] = {{1,2},{2,1},{-1,2},{-2,1},{1,-2}, {2,-1}, {-2,-1}, {-1,-2}};
const pair<int,int> Piece::rookMoves[4] = {{0,1}, {0,-1}, {1,0}, {-1, 0}};
const pair<int,int> Piece::bishopMoves[4] = {{1,1}, {1,-1}, {-1,-1}, {-1, 1}}; // Corrected the repeated {1,-1} from original code
const pair<int,int> Piece::kingMoves[8] = {{0,1},{1,1},{-1,1},{0,-1}, {1,-1}, {-1,-1}, {-1,0},{1,0}};
