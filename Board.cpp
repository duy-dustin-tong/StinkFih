// Board.cpp

#include "Board.h"
#include <bits/stdc++.h>

using namespace std;

// --- Helper Implementation ---

// /**
//  * @brief Converts (rank, file) coordinates to a 0-63 index.
//  * * @param x Pair of (rank, file), where rank 0 is 8th rank.
//  * @return int Index (0-63) or -1 if invalid.
//  */
// int Board::CoordToIndex(pair<int,int> x){
//     if(x.first == -1 || x.second == -1) return -1;
//     return x.first * 8 + x.second;
// }

// /**
//  * @brief Converts a 0-63 index to (rank, file) coordinates.
//  * * @param x Index (0-63).
//  * @return pair<int,int> Pair of (rank, file) or (-1, -1) if invalid.
//  */
// pair<int,int> Board::IndexToCoord(int x){
//     if(x == -1) return make_pair(-1,-1);
//     return make_pair(x / 8, x % 8);
// }



const char Board::fileID[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
const char Board::rankID[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};


// --- Constructor Implementation ---
Board::Board(bool initial_color) {
    InitBoard(startingFEN, initial_color); 
}

// --- Core Logic Implementation ---

/**
 * @brief Initializes the board state from a FEN string.
 * * @param fen The FEN string to parse.
 * @param x Boolean indicating if the bot is playing white.
 */
void Board::InitBoard(const string& fen, bool x){
    botIsWhite = x;
    
    // 1. Clear and reset the board
    for(int i = 0; i < 8; ++i) {
        memset(board[i], 0, sizeof(board[i]));
    }
    
    // Reset flags and state variables
    blackCastleKingSide = false;
    blackCastleQueenSide = false; 
    whiteCastleKingSide = false; 
    whiteCastleQueenSide = false; 
    isWhiteTurn = true;
    missingKing = false;
    enPassantTarget = -1;
    kingSquare.first = 0;
    kingSquare.second = 0;
    
    // Clear boardHistory
    while (!boardHistory.empty()) {
        boardHistory.pop();
    }
    
    int px = 0; // Rank index (0-7, 0 is rank 8)
    int py = 0; // File index (0-7, 0 is file a)
    int fen_idx = 0;

    // --- FEN Parsing ---
    
    // 1. Piece Placement (until the first space)
    for( ; fen_idx < fen.length(); fen_idx++){
        char c = fen[fen_idx];
        if (c == ' ') break;
        
        if(isdigit(c)) {
            py += c - '0'; // Skip empty squares
        } else {
            switch(c){
                case '/':
                    px++; 
                    py = 0;
                    break;
                // Black pieces
                case 'k': 
                    board[px][py] = Piece::Black | Piece::King; 
                    kingSquare.second = CoordToIndex(make_pair(px,py));
                    py++; 
                    break;

                case 'q': board[px][py] = Piece::Black | Piece::Queen; py++; break;
                case 'p': board[px][py] = Piece::Black | Piece::Pawn; py++; break;
                case 'n': board[px][py] = Piece::Black | Piece::Knight; py++; break;
                case 'b': board[px][py] = Piece::Black | Piece::Bishop; py++; break;
                case 'r': board[px][py] = Piece::Black | Piece::Rook; py++; break;
                // White pieces
                case 'K': 
                    board[px][py] = Piece::White | Piece::King; 
                    kingSquare.first = CoordToIndex(make_pair(px,py));
                    py++; 
                    break;
                case 'Q': board[px][py] = Piece::White | Piece::Queen; py++; break;
                case 'P': board[px][py] = Piece::White | Piece::Pawn; py++; break;
                case 'N': board[px][py] = Piece::White | Piece::Knight; py++; break;
                case 'B': board[px][py] = Piece::White | Piece::Bishop; py++; break;
                case 'R': board[px][py] = Piece::White | Piece::Rook; py++; break;
            }
        }
    }
    
    // 2. Active Color (w/b)
    fen_idx++; 
    if (fen_idx < fen.length()) {
        isWhiteTurn = (fen[fen_idx] == 'w');
    }
    fen_idx += 2; 

    // 3. Castling Availability (K, Q, k, q, or -)
    string castling_str = "";
    while(fen_idx < fen.length() && fen[fen_idx] != ' ') {
        castling_str += fen[fen_idx];
        fen_idx++;
    }
    
    whiteCastleKingSide = (castling_str.find('K') != string::npos);
    whiteCastleQueenSide = (castling_str.find('Q') != string::npos);
    blackCastleKingSide = (castling_str.find('k') != string::npos);
    blackCastleQueenSide = (castling_str.find('q') != string::npos);
    fen_idx++; 

    // 4. En Passant Target Square (e.g., e3 or -)
    string ep_target_str = "";
    while(fen_idx < fen.length() && fen[fen_idx] != ' ') {
        ep_target_str += fen[fen_idx];
        fen_idx++;
    }
    
    if (ep_target_str.length() == 2 && ep_target_str != "-") {
        // FEN file/rank (e.g., "e3") -> (file, rank)
        int file = ep_target_str[0] - 'a';
        int rank = '8' - ep_target_str[1]; 
        
        enPassantTarget = CoordToIndex(make_pair(rank, file));
    } else {
        enPassantTarget = -1; // Invalid or no target
    }
}

/**
 * @brief Executes a move (in UCI format) and records all changes for UndoMove.
 * * Handles standard moves, captures, promotions, castling, and en passant.
 * * @param move The move string (e.g., "e2e4", "e7e8q", "e1g1").
 */
void Board::MakeMove(const string& move){
    BoardChanges changes;
    changes.changedSquares.clear(); 

    // 1. Record CURRENT global flags for undo
    changes.blackCastleKingSide = blackCastleKingSide;
    changes.blackCastleQueenSide = blackCastleQueenSide;
    changes.whiteCastleKingSide = whiteCastleKingSide;
    changes.whiteCastleQueenSide = whiteCastleQueenSide;
    changes.isWhiteTurn = isWhiteTurn;
    changes.missingking = missingKing;
    changes.enPassantTarget = enPassantTarget;
    changes.kingSquare = kingSquare;
    
    // 2. Parse UCI coordinates and map to array indices (rank, file)
    int fromFile = move[0] - 'a'; 
    int fromRank = '8' - move[1]; 
    int toFile = move[2] - 'a';
    int toRank = '8' - move[3];

    pair<int,int> fromCoord = make_pair(fromRank, fromFile);
    pair<int,int> toCoord = make_pair(toRank, toFile);

    int fromIndex = CoordToIndex(fromCoord);
    int toIndex = CoordToIndex(toCoord);
    
    int &piece_at_from = board[fromRank][fromFile];
    int &piece_at_to = board[toRank][toFile];
    int moved_piece = piece_at_from;
    int captured_piece = piece_at_to; // Holds the piece at the 'to' square (0 if empty)

    if(piece_at_from == (Piece::White | Piece::King)) kingSquare.first = toIndex;
    if(piece_at_from == (Piece::Black | Piece::King)) kingSquare.second = toIndex;

    // 3. Record PRE-MOVE state of primary squares for undo
    changes.changedSquares.push_back(make_pair(fromIndex, piece_at_from));
    changes.changedSquares.push_back(make_pair(toIndex, piece_at_to));

    // --- Execute Move Logic ---

    // A. Handle En Passant CAPTURE
    if (Piece::Type(moved_piece) == Piece::Pawn && toIndex == enPassantTarget && Piece::Type(captured_piece) == Piece::None) {
        // This confirms it's an en passant move (pawn moves to EP target square, but target square is empty).
        // Determine the captured pawn's square (one rank behind the target square)
        int capturedRank = isWhiteTurn ? toRank + 1 : toRank - 1;
        int capturedFile = toFile;
        int capturedIndex = CoordToIndex(make_pair(capturedRank, capturedFile));
        
        // Record state of the captured pawn's square
        changes.changedSquares.push_back(make_pair(capturedIndex, board[capturedRank][capturedFile]));
        
        // Clear the captured pawn
        board[capturedRank][capturedFile] = Piece::None;
        captured_piece = Piece::Pawn | (isWhiteTurn ? Piece::Black : Piece::White); // Set the captured piece value for Castling logic check
    }
    
    // B. Handle Castling MOVE (Rook movement)
    if (Piece::Type(moved_piece) == Piece::King && abs(fromFile - toFile) == 2) {
        // Determine the rook's move coordinates
        int rookFromFile, rookToFile, rookRank = fromRank;
        if (toFile > fromFile) { // Kingside (e1g1 or e8g8)
            rookFromFile = 7;
            rookToFile = 5;
        } else { // Queenside (e1c1 or e8c8)
            rookFromFile = 0;
            rookToFile = 3;
        }
        
        // Record state of the rook's original and destination squares
        int rookFromIndex = CoordToIndex(make_pair(rookRank, rookFromFile));
        int rookToIndex = CoordToIndex(make_pair(rookRank, rookToFile));
        
        changes.changedSquares.push_back(make_pair(rookFromIndex, board[rookRank][rookFromFile]));
        changes.changedSquares.push_back(make_pair(rookToIndex, board[rookRank][rookToFile]));
        
        // Move the rook
        board[rookRank][rookToFile] = board[rookRank][rookFromFile];
        board[rookRank][rookFromFile] = Piece::None;
    }

    // C. Standard piece movement (King/Pawn/other piece)
    piece_at_to = piece_at_from;
    piece_at_from = Piece::None;
    
    // D. Handle Promotion
    if (move.size() == 5) {
        int new_piece_type = Piece::None;
        if(move[4] == 'q') new_piece_type = Piece::Queen;
        else if(move[4] == 'r') new_piece_type = Piece::Rook;
        else if(move[4] == 'n') new_piece_type = Piece::Knight;
        else if(move[4] == 'b') new_piece_type = Piece::Bishop;
        
        piece_at_to = Piece::Color(moved_piece) | new_piece_type;
    }

    // --- Update Flags ---
    
    // E. Update En Passant Target for next turn
    // Reset target first
    enPassantTarget = -1; 
    
    // Check for Pawn Double Push
    if (Piece::Type(moved_piece) == Piece::Pawn && abs(fromRank - toRank) == 2) {
        // Set the square BEHIND the pawn as the target
        int targetRank = (isWhiteTurn) ? toRank + 1 : toRank - 1;
        enPassantTarget = CoordToIndex(make_pair(targetRank, toFile));
    }

    // F. Revoke Castling Rights
    // 1. If King moved
    if (Piece::Type(moved_piece) == Piece::King) {
        if (isWhiteTurn) {
            whiteCastleKingSide = false;
            whiteCastleQueenSide = false;
        } else {
            blackCastleKingSide = false;
            blackCastleQueenSide = false;
        }
    }
    
    // 2. If Rook moved or captured (checking both 'from' and 'to' squares against rook start positions)
    bool checkRookFrom = Piece::Type(moved_piece) == Piece::Rook;
    bool checkRookTo = Piece::Type(captured_piece) == Piece::Rook;

    if (checkRookFrom || checkRookTo) {
        // White Rook original positions (a1/h1 -> rank 7)
        if (fromRank == 7 || toRank == 7) {
            // Check file 0 (a-file, Queen side)
            if (fromFile == 0 || toFile == 0) whiteCastleQueenSide = false; 
            // Check file 7 (h-file, King side)
            if (fromFile == 7 || toFile == 7) whiteCastleKingSide = false;  
        }
        // Black Rook original positions (a8/h8 -> rank 0)
        if (fromRank == 0 || toRank == 0) {
            // Check file 0 (a-file, Queen side)
            if (fromFile == 0 || toFile == 0) blackCastleQueenSide = false; 
            // Check file 7 (h-file, King side)
            if (fromFile == 7 || toFile == 7) blackCastleKingSide = false;  
        }
    }
    
    // G. Switch turn
    isWhiteTurn = !isWhiteTurn;

    // H. check missing king

    if(Piece::Type(captured_piece)==Piece::King) missingKing = true;

    // 4. Push the recorded state
    boardHistory.push(changes);
}

/**
 * @brief Reverts the board state to the position before the last MakeMove call.
 */
void Board::UndoMove(){
    if(boardHistory.empty()) return;

    BoardChanges lastChanges = boardHistory.top();
    boardHistory.pop();

    // 1. Revert global flags and state
    blackCastleKingSide = lastChanges.blackCastleKingSide;
    blackCastleQueenSide = lastChanges.blackCastleQueenSide;
    whiteCastleKingSide = lastChanges.whiteCastleKingSide;
    whiteCastleQueenSide = lastChanges.whiteCastleQueenSide;
    isWhiteTurn = lastChanges.isWhiteTurn;
    enPassantTarget = lastChanges.enPassantTarget;
    kingSquare = lastChanges.kingSquare;

    // 2. Revert piece positions on all changed squares (includes from, to, and potential EP/Rook squares)
    for(const auto& change : lastChanges.changedSquares){
        int index = change.first;
        int piece = change.second;

        if (index != -1) {
            pair<int, int> coord = IndexToCoord(index);
            board[coord.first][coord.second] = piece;
        }
    }
}

void Board::PrintBoard() {
    cout << "\n----------------------------------------" << endl;
    
    // Helper lambda to get the character symbol for a piece
    auto get_piece_symbol = [](int piece) -> char {
        if (piece == Piece::None) return '.';
        
        char symbol;
        int type = Piece::Type(piece);
        
        switch (type) {
            case Piece::King:   symbol = 'k'; break;
            case Piece::Pawn:   symbol = 'p'; break;
            case Piece::Knight: symbol = 'n'; break;
            case Piece::Bishop: symbol = 'b'; break;
            case Piece::Rook:   symbol = 'r'; break;
            case Piece::Queen:  symbol = 'q'; break;
            default: return '?'; 
        }
        
        // If white, convert to uppercase
        if (Piece::Color(piece) == Piece::White) {
            return toupper(symbol);
        }
        
        return symbol;
    };
    
    // Print the board contents
    for (int i = 0; i < 8; ++i) {
        cout << rankID[i] << " | ";
        for (int j = 0; j < 8; ++j) {
            cout << get_piece_symbol(board[i][j]) << " ";
        }
        cout << "| " << rankID[i] << endl;
    }
    
    // Print file labels at the bottom
    cout << "  |";
    for(int i = 0; i < 8; ++i) {
        cout << "--";
    }
    cout << " |" << endl;
    cout << "    ";
    for(int i = 0; i < 8; ++i) {
        cout << fileID[i] << " ";
    }
    cout << endl;

    // Print current state info
    cout << "Turn: " << (isWhiteTurn ? "White" : "Black") << endl;
    cout << "EP Target Index: " << enPassantTarget << endl;
    cout << "Castling: " 
         << (whiteCastleKingSide ? "K" : "")
         << (whiteCastleQueenSide ? "Q" : "")
         << (blackCastleKingSide ? "k" : "")
         << (blackCastleQueenSide ? "q" : "")
         << endl;
    cout << "----------------------------------------" << endl;
}

bool Board::IsSquareAttacked(pair<int,int> square, bool byWhite){
    int matchColor = byWhite? Piece::White : Piece::Black;
    
    //check if attacked by rook or queen
    for(int i = 0;i<4;i++){
        int x = square.first;
        int y = square.second;

        while(true){
            x += Piece::rookMoves[i].first;
            y += Piece::rookMoves[i].second;

            if(!InsideBoard(make_pair(x,y))) break;
            int pieceAtTarget = board[x][y];
            if(Piece::IsRookOrQueen(pieceAtTarget) && Piece::Color(pieceAtTarget) == matchColor) return true;
            if(pieceAtTarget!=Piece::None) break;
        }
    }

    //check if attacked by bishop or queen
    for(int i = 0;i<4;i++){
        int x = square.first;
        int y = square.second;

        while(true){
            x += Piece::bishopMoves[i].first;
            y += Piece::bishopMoves[i].second;

            if(!InsideBoard(make_pair(x,y))) break;
            int pieceAtTarget = board[x][y];
            if(Piece::IsBishopOrQueen(pieceAtTarget) && Piece::Color(pieceAtTarget) == matchColor) return true;
            if(pieceAtTarget!=Piece::None) break;
        }
    }

    // check if attacked by knight
    for(int i = 0;i<8;i++){
        int x = square.first + Piece::knightMoves[i].first;
        int y = square.second + Piece::knightMoves[i].second;

        if(!InsideBoard(make_pair(x,y))) continue;
        int pieceAtTarget = board[x][y];

        if(pieceAtTarget == (Piece::Knight | matchColor)) return true;
    }

    // check if attacked by king
    for(int i = 0;i<8;i++){
        int x = square.first + Piece::kingMoves[i].first;
        int y = square.second + Piece::kingMoves[i].second;

        if(!InsideBoard(make_pair(x,y))) continue;
        int pieceAtTarget = board[x][y];

        if(pieceAtTarget == (Piece::King | matchColor)) return true;
    }

    //check if attacked by pawn
    for(int i = -1; i<=1; i+=2){
        int x = square.first + ((matchColor==Piece::White)? 1 : -1);
        int y = square.second + i;

        if(!InsideBoard(make_pair(x,y))) continue;
        int pieceAtTarget = board[x][y];

        if(pieceAtTarget == (Piece::Pawn | matchColor)) return true;
    }

    return false;
}

bool Board::IsKingSafe(){
    int kingPos = isWhiteTurn? kingSquare.second : kingSquare.first;
    pair<int,int> square = IndexToCoord(kingPos);
    return !IsSquareAttacked(square, isWhiteTurn);
}
