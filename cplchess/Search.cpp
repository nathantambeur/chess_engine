#include "Search.hpp"
#include <chrono>

#include <algorithm>
#include <iostream>
#include <functional>
#include <random>
#include <map>

using Optional = std::optional<Board>;
using MoveVec = std::vector<Move>;
using Bitboard = std::bitset<64>;
using Turn = PieceColor;
using namespace std::chrono;

std :: chrono ::microseconds time_per_turn;
std :: chrono ::microseconds bonus_time_next_turn;
std :: chrono ::microseconds my_total_time;
std::chrono::_V2::system_clock::time_point start_root;


MoveVec pvs = {Move(Square::A1, Square::A1)};
Move searchkillers[2] = {Move(Square::A1, Square::A1), Move(Square::A1, Square::A1)};
int searchHistoryW[6][64];
int searchHistoryB[6][64];
int bestScore;
bool root_call = true;
std::unordered_map<unsigned long long, int> uniquePositions;
unsigned long long zobristHashTable[6][64];
unsigned long long zobristHashEnpassant[64];
unsigned long long zobristHashCastle[4];
unsigned long long zobristHashTurn[2];
// int pieceToIndex(Piece piece){
//     if (piece.type() == PieceType::Pawn){
//         return 0;
//     }
//     if (piece.type() == PieceType::Knight){
//         return 1;
//     }
//     if (piece.type() == PieceType::Bishop){
//         return 2;
//     }
//     if (piece.type() == PieceType::Rook){
//         return 3;
//     }
//     if (piece.type() == PieceType::Queen){
//         return 4;
//     }
//     if (piece.type() == PieceType::King){
//         return 5;
//     }
//     return 0;
// }
Search::Search(){
    m_board = Board();
    //standard timing values to get to depth 7
    initializeZobristHashTable();
    time_per_turn= std :: chrono ::minutes(1);
    bonus_time_next_turn= std :: chrono ::minutes(0);
    my_total_time= std :: chrono ::minutes(1);
}
std::string Search::name() const{
    return "Nathan's Chess Engine";
}
std::string Search::version() const{
    return "0.0";
}
std::string Search::author() const {
    return "Nathan";
}
bool new_game = true;
void Search::newGame(){
    new_game = true;
    m_board = Board();
    m_board.startBoard();
    time_per_turn= std :: chrono ::minutes(1);
    bonus_time_next_turn= std :: chrono ::minutes(0);
    my_total_time= std :: chrono ::minutes(1);
    pvs = {Move(Square::A1, Square::A1)};
    searchkillers[0] = Move(Square::A1, Square::A1);
    searchkillers[1] = Move(Square::A1, Square::A1);
    for (int i = 0; i < 6; i++){
        for (int j = 0; j < 64; j++){
            searchHistoryW[i][j] = 0;
            searchHistoryB[i][j] = 0;
        }
    }
    bestScore = 0;
    uniquePositions.clear();
    root_call = true;
}
// The initializeZobristHashTable function initializes the zobristHashTable with random numbers
void Search::initializeZobristHashTable() {
  // Seed the random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<unsigned long long> dist;

  // Initialize the zobristHashTable with random numbers
  for (int piece = 0; piece < 6; ++piece) {
    for (int square = 0; square < 64; ++square) {
      zobristHashTable[piece][square] = dist(mt);

    }
  }
    for (int square = 0; square < 64; ++square) {
        zobristHashEnpassant[square] = dist(mt);
    }
    for (int castle = 0; castle < 4; ++castle) {
        zobristHashCastle[castle] = dist(mt);
    }
    for (int turn = 0; turn < 2; ++turn) {
        zobristHashTurn[turn] = dist(mt);
    }
}
microseconds total_pseudo_move_generation(0)  , total_ordering(0) , total_evalating(0) , total_minimax(0), total_king_checking(0),total_opponent_capture(0),total_undomove(0),total_move_score(0);
microseconds total_movecomparing_score(0);
microseconds total_movesorting(0);
microseconds total_hashing(0);
int total_pseudo_move_generation_count = 1;
int total_ordering_count = 1;
int total_evalating_count = 1;
int total_minimax_count = 1;
int total_king_checking_count = 1;
int total_opponent_capture_count = 1;
int total_undomove_count = 1;
int total_move_score_count = 1;
int toal_movecomparing_count = 1;
int total_movesorting_count = 1;
int total_hashing_count = 1;
// The calculateHashCode function calculates the hash code for a given position using Zobrist hashing
unsigned long long Search::calculateHashCode() {
  unsigned long long hash = 0;
  Bitboard occupied = m_board.occupied();
  // Iterate through the pieces on the board
  while (occupied != 0) {
        int square = occupied._Find_first();
        Piece piece_on =  *m_board.piece(*Square::fromIndex(square));
        hash ^= zobristHashTable[ (int) piece_on.type()][square];
        occupied[square] = false;
  } 
  if (m_board.turn() == PieceColor::White) {
    hash ^= zobristHashTurn[0];
  } else {
    hash ^= zobristHashTurn[1];
  }
    // XOR the hash with the random numbers for the castling rights
    if ((m_board.castlingRights() & CastlingRights::WhiteQueenside) != CastlingRights::None) {
    hash ^= zobristHashCastle[0];
    }
    if ((m_board.castlingRights() & CastlingRights::WhiteKingside) != CastlingRights::None) {
        hash ^= zobristHashCastle[1];
    }
    if ((m_board.castlingRights() & CastlingRights::BlackQueenside) != CastlingRights::None) {
        hash ^= zobristHashCastle[2];
    }
    if ((m_board.castlingRights() & CastlingRights::BlackKingside) != CastlingRights::None) {
        hash ^= zobristHashCastle[3];
    }

  // XOR the hash with the random number for the en passant square
  if (m_board.enPassantSquare().has_value()) {
    hash ^= zobristHashEnpassant[m_board.enPassantSquare().value().index()];
  }
            
  return hash;
}


//for benchmarking
void printTimes(){
    std::cout << "total_minimax: " << total_minimax.count() << std::endl;
    std ::cout << "seperated: "<< std::endl;
    std::cout << "total_pseudo_move_generation: " << total_pseudo_move_generation.count() << std::endl;
    std::cout << "total_ordering: " << total_ordering.count() << std::endl;
    std::cout << "total_evalating: " << total_evalating.count() << std::endl;
    std::cout << "total_king_checking: " << total_king_checking.count() << std::endl;
    std::cout << "total_opponent_capture: " << total_opponent_capture.count() << std::endl;
    std::cout << "total_undomove: " << total_undomove.count() << std::endl;
    std::cout << "total_move_score: " << total_move_score.count() << std::endl;
    std::cout << "total_movecomparing_score: " << total_movecomparing_score.count() << std::endl;
    std::cout << "total_movesorting: " << total_movesorting.count() << std::endl;

    //average time per call
    std::cout << "average time per call: " << std::endl;
    std::cout << "total_pseudo_move_generation: " << (float) total_pseudo_move_generation.count()/(float) total_pseudo_move_generation_count << std::endl;
    std::cout << "total_ordering: " << (float) total_ordering.count()/(float) total_ordering_count << std::endl;
    std::cout << "total_evalating: " << (float) total_evalating.count()/(float) total_evalating_count << std::endl;
    std::cout << "total_king_checking: " << (float) total_king_checking.count()/(float) total_king_checking_count << std::endl;
    std::cout << "total_opponent_capture: " << (float) total_opponent_capture.count()/(float) total_opponent_capture_count << std::endl;
    std::cout << "total_undomove: " << (float) total_undomove.count()/(float) total_undomove_count << std::endl;
    std::cout << "total_move_score: " << (float) total_move_score.count()/(float) total_move_score_count << std::endl;
    std::cout << "total_movecomparing_score: " << (float) total_movecomparing_score.count()/(float) toal_movecomparing_count << std::endl;
    std::cout << "total_movesorting: " << (float) total_movesorting.count()/ (float) total_movesorting_count << std::endl;
    //average time of total minmax time 
    std::cout << "average time of total minmax time: " << std::endl;
    std::cout << "total_minimax: " << (float) total_minimax.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_pseudo_move_generation: " << (float) total_pseudo_move_generation.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_ordering: " << (float) total_ordering.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_evalating: " << (float) total_evalating.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_king_checking: " << (float) total_king_checking.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_opponent_capture: " << (float) total_opponent_capture.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_undomove: " << (float) total_undomove.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_move_score: " << (float) total_move_score.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_movecomparing_score: " << (float) total_movecomparing_score.count()/(float) total_minimax.count() << std::endl;
    std::cout << "total_movesorting: " << (float) total_movesorting.count()/(float) total_minimax.count() << std::endl;
    //total call times 
    std::cout << "total call times: " << std::endl;
    std::cout << "total_pseudo_move_generation: " << total_pseudo_move_generation_count << std::endl;
    std::cout << "total_ordering: " << total_ordering_count << std::endl;
    std::cout << "total_evalating: " << total_evalating_count << std::endl;
    std::cout << "total_king_checking: " << total_king_checking_count << std::endl;
    std::cout << "total_opponent_capture: " << total_opponent_capture_count << std::endl;
    std::cout << "total_undomove: " << total_undomove_count << std::endl;
    std::cout << "total_move_score: " << total_move_score_count << std::endl;
    std::cout << "total_movecomparing_score: " << toal_movecomparing_count << std::endl;
    std::cout << "total_movesorting: " << total_movesorting_count << std::endl;
    

}


int pieceValue(Piece piece){
    if (piece.type() == PieceType::Pawn){
        return 1;
    }
    if (piece.type() == PieceType::Knight){
        return 3;
    }
    if (piece.type() == PieceType::Bishop){
        return 3;
    }
    if (piece.type() == PieceType::Rook){
        return 5;
    }
    if (piece.type() == PieceType::Queen){
        return 9;
    }
    if (piece.type() == PieceType::King){
        return 100;
    }
    return 0;
}
MoveVec bannedMoves;

PrincipalVariation Search::pv(const Board& board,const TimeInfo::Optional& timeInfo){
    m_board = board;
    int current_depth = 0;
    int max_depth = 7; //if no time info is given, go to depth 6
    int score = 0;
    bool checked = false;
    bool isMaximizingPlayer = (m_board.turn() == PieceColor::White);
    start_root = std::chrono::high_resolution_clock::now();

    if (timeInfo.has_value()){
        max_depth = 64; //go as deep as possible within the time limit  
        std::chrono::milliseconds total_time_left;
        if (isMaximizingPlayer){
            total_time_left = timeInfo.value().white.timeLeft;
        }
        else{
            total_time_left = timeInfo.value().black.timeLeft;
        }

      
        if (total_time_left < std::chrono::seconds(20)){
            //failsafe incase time is too long 
            time_per_turn = std::chrono::seconds(30);
            bonus_time_next_turn= std :: chrono ::minutes(0);
            max_depth = 5;
            
        }
        else if (total_time_left < std::chrono::seconds(150)){
            //failsafe incase time is too long
            time_per_turn = std::chrono::seconds(30);
            bonus_time_next_turn= std :: chrono ::minutes(0);
            max_depth = 6;
        }
        else if (total_time_left < std::chrono::minutes(4)){
            //failsafe incase time is too long
            time_per_turn = std::chrono::seconds(30);
            bonus_time_next_turn= std :: chrono ::minutes(0);
            max_depth = 7;
            
        }  
        
        else if (new_game){
            //goal for each turn will always be a bit more 
            time_per_turn = total_time_left / 50;
            new_game = false;
        }
        my_total_time = time_per_turn + bonus_time_next_turn;
    }

    for (int i = 3; i < max_depth; i++){
        Node root = Node();
        current_depth = i;
        root_call = true;
        m_board = board;
        score = minimax(i, -20000, 20000, isMaximizingPlayer, root);
        if (score == 6666666 || score == -6666666){
            break;
        }
        pvs = root.getBestMoveSequence(isMaximizingPlayer);
        bestScore = score;
        if (score <= -10000 || score >= 10000){
            checked = true;
            score = 0;
            bestScore = score;
            break;
        }
    }
    m_board = board;
    auto stop_root = std::chrono::high_resolution_clock::now();
    auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop_root - start_root);
    if (duration_root < my_total_time){
        bonus_time_next_turn = my_total_time - duration_root;
    }
    else {
         bonus_time_next_turn = std::chrono::microseconds(0);
    }
   

    PrincipalVariation r_pv = PrincipalVariation(checked, bestScore, pvs);
    if (pvs.empty()){
        //just continue checkmate 
    }
    else{ 
        m_board.makeMove(pvs[0]);
        //dont repeat the same position so it doesn't end in draw 
        unsigned long long hash = calculateHashCode();
       
        if (uniquePositions.count(hash) == 0){
            uniquePositions[hash] = 1;  
        }
        else if (uniquePositions.count(hash) > 0){
        
            bannedMoves.push_back(pvs[0]);
            PrincipalVariation pv2 = pv(board,timeInfo);
            if (pv2.length()==0){
                return r_pv;
            }
            else {
                return pv2;
            }

        }
    }
    (void) current_depth;
    //printTimes();
    // printf("Nodes visited %d\n",total_minimax_count);
    bannedMoves.clear();
    pvs = {Move(Square::A1, Square::A1)}; 
    // std::cout << "Depth: " << current_depth << " Score: " << score << " PV: " << r_pv << std::endl;
    searchkillers[0] = Move(Square::A1, Square::A1);
    searchkillers[1] = Move(Square::A1, Square::A1);
    for (int i = 0; i < 6; i++){
        for (int j = 0; j < 64; j++){
            searchHistoryW[i][j] = 0;
            searchHistoryB[i][j] = 0;
        }
    }
    bestScore = 0;
    root_call = true;
    m_board = board;
    return r_pv;     
}
//function that returns the hash info of the engine right now no idea needed for tables in optimazation
std::optional<HashInfo> Search::hashInfo() const {
    return std::nullopt;
}
//function that sets the hash size of the engine
void Search::setHashSize(std::size_t size) {
    if (size == 0){
        return;
    }
    return;
}



int Search::minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, Node& node){
   
    bool added = false;
    if (depth == 0){
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_root);
        // printf("Time taken: %lli\n", duration_root.count());
        // printf("Time left: %lli\n", my_total_time.count()-duration_root.count());
        if (duration_root> my_total_time){
            // printf("RETURNNNN TURN IS OVERRRRRR\n");
            return 6666666;
        }
        // printf("can continue\n");
        int score = evaluate();
        
        node.setScore(score);
        return score;
    }
   
    if (isMaximizingPlayer){
       
        int bestValue = -20000;
        MoveVec moves;
        
       
        m_board.pseudoLegalMoves(moves);   
        
     
    
        if (moves.empty()){
           
            if (m_board.isCheck()){
                
                node.setScore(bestValue);
                return -10000-depth;
            }
            else{ //stalemate
               
                node.setScore(bestValue);
                return 0;
            }
        }
       

       std::multimap<int, int, std::greater<int>> score_map;
        for (int i = 0; i < (int) moves.size();i++){
            
            score_map.insert({moveScore(moves[i]),i});               
        }
       

        std::vector<Bitboard> boards;
        m_board.storeAllBoars(boards);
        CastlingRights castlingRights = m_board.castlingRights();

        Move bestMove= moves[0];
        

        for (const auto& [key, val] : score_map) {
            
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_root);
            // printf("Time taken: %lli\n", duration_root.count());
            // printf("Time left: %lli\n", my_total_time.count()-duration_root.count());
            if (duration_root> my_total_time){
                // printf("RETURNNNN TURN IS OVERRRRRR\n");
                return -6666666;
            }
            // printf("can continue\n");
            
            Move move = moves[val];
            
            if (std::find(bannedMoves.begin(), bannedMoves.end(), move ) != bannedMoves.end()) {
                continue;
            }

          
            m_board.makeMove(move);

            

            //if king is under attack undo the move and continue
            //move makes color switch need to switch back 
            m_board.setTurn(PieceColor::White);
            
            if (m_board.isCheck()){
                // std :: cout << "CHECK FOR MOVE: "<< move << std::endl;
                

                //undo move switch color back back to white so again solves the problem
               
                m_board.restoreAllBoars(boards);
                m_board.setCastlingRights(castlingRights);

               

                //if this was the last move and the king is still under attack then return -10000 
                if (move == moves.back() && !added){
                    //it was altready in check at the start so we are mated 
                   
                    if (m_board.isCheck()){
    
                        bestValue = -10000-depth;
                    }
                    else{ 
                        
                        bestValue = 0;
                    }
                }
            }
            else{
                added = true;
                
                
                Node child = Node(move, 0, depth,0,0);
                m_board.setTurn(PieceColor::Black);
                
                int value = minimax( depth - 1, alpha, beta, false, child);
                bestValue = std::max(bestValue, value);
                
                m_board.restoreAllBoars(boards);
                m_board.setCastlingRights(castlingRights);
                m_board.setTurn(PieceColor::White);
                if (value >= alpha){
                    alpha = value;
                    if (key <  9999999){
                       searchHistoryW[(int) (*m_board.piece(move.from())).type()][move.to().index()] += depth;  
                    }
                    
                }
                if (bestValue == value){
                    
                    bestMove = Move(*Square::fromIndex(move.from().index()),*Square::fromIndex(move.to().index()),move.promotion());
                    
                }
                node.addChild(child);
               
               
                
                if (beta <= alpha){
                    //if it is not a capture that triggers the cutoff 
                    if (key <  9999999){
                        searchkillers[1] = searchkillers[0];
                        searchkillers[0] = move;
                    }

                    break;
                }
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_root);
        if (duration_root> my_total_time){
            
            return -6666666;
        }
        node.setScore(bestValue);
        return bestValue;
    }
    
    else{
       
        int bestValue = 20000;
         

        MoveVec moves;
        
        
        m_board.pseudoLegalMoves(moves);   
        
        
       

        std::multimap<int, int, std::greater<int>> score_map;
        for (int i = 0; i < (int) moves.size();i++){
           score_map.insert({moveScore(moves[i]),i});         
        }
       


        //OrderMoves(moves);

        std::vector<Bitboard> boards;
        m_board.storeAllBoars(boards);
        CastlingRights castlingRights = m_board.castlingRights();

        Move bestMove= moves[0];
        total_king_checking_count++;

        
        if (moves.empty()){
           
            if (m_board.isCheck()){
            
                node.setScore(bestValue);
                return 10000+depth;
            }
            else{ //stalemate
                node.setScore(bestValue);
                return 0;
            }
        }
        
       
       
       for (const auto& [key, val] : score_map) {
            
            
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_root);
            // printf("Time taken: %lli\n", duration_root.count());
            // printf("Time left: %lli\n", my_total_time.count()-duration_root.count());
            if (duration_root> my_total_time){
                // printf("RETURNNNN TURN IS OVERRRRRR\n");
                return 6666666;
            }
            // printf("can continue\n");
        
            Move move = moves[val];
            if (std::find(bannedMoves.begin(), bannedMoves.end(), move ) != bannedMoves.end()) {
                continue;
            }
            
            
            
            m_board.makeMove(move);
            
            m_board.setTurn(PieceColor::Black);
            
            if (m_board.isCheck()){
                
                m_board.restoreAllBoars(boards);
                m_board.setCastlingRights(castlingRights);
               
                if (move == moves.back() && !added){
                   
                    if (m_board.isCheck()){
                        

                        bestValue = 10000+depth;
                    }
                    else{ //stalemate
                        
                      
                        bestValue = 0;
                    }
                }
            }
            else{
                added = true;
               
                
                Node child = Node(move, 0, depth,0,0);
                m_board.setTurn(PieceColor::White);
                int value = minimax( depth - 1, alpha, beta, true, child);
                bestValue = std::min(bestValue, value);
                m_board.restoreAllBoars(boards);
                m_board.setCastlingRights(castlingRights);
                m_board.setTurn(PieceColor::Black);

                beta = std::min(beta, value);
                if (value <= beta){
                    beta = value;
                    if (key <   9999999){
                        searchHistoryB[(int) (*m_board.piece(move.from())).type()][move.to().index()] += depth;  
                    }
                    
                }
                if (bestValue == value){  
                    bestMove = Move(*Square::fromIndex(move.from().index()),*Square::fromIndex(move.to().index()),move.promotion());;
                }
                node.addChild(child);
                if (beta <= alpha){
                    if (key <   9999999){
                        searchkillers[1] = searchkillers[0];
                        searchkillers[0] = move;
                    }
                    break;
                }
            }
           
        } 
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_root = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_root);
        // printf("Time taken: %lli\n", duration_root.count());
        // printf("Time left: %lli\n", my_total_time.count()-duration_root.count());
        if (duration_root> my_total_time){
            // printf("RETURNNNN TURN IS OVERRRRRR\n");
            return 6666666;
        }
        // printf("can continue\n");
        node.setScore(bestValue);
        return bestValue;
    }

}


int gamePhaseScores[6] = {0,1,1,2,4,0};
int White_Pawn_Table_mg[64] = 
{  82,  82,  82,  82,  82,  82,  82,  82, 180, 216, 143, 177, 150, 208, 116,  71,  76,  89, 108, 113, 147, 138, 107,  62,  68,  95,  88, 103, 105,  94,  99,  59,  55,  80,  77,  94,  99,  88,  92,  57,  56,  78,  78,  72,  85,  85, 115,  70,  47,  81,  62,  59,  67, 106, 120,  60,  82,  82,  82,  82,  82,  82,  82,  82,};
int Black_Pawn_Table_mg[64] = 
{  82,  82,  82,  82,  82,  82,  82,  82,  47,  81,  62,  59,  67, 106, 120,  60,  56,  78,  78,  72,  85,  85, 115,  70,  55,  80,  77,  94,  99,  88,  92,  57,  68,  95,  88, 103, 105,  94,  99,  59,  76,  89, 108, 113, 147, 138, 107,  62, 180, 216, 143, 177, 150, 208, 116,  71,  82,  82,  82,  82,  82,  82,  82,  82,};
int White_Knight_Table_mg[64] = 
{ 170, 248, 303, 288, 398, 240, 322, 230, 264, 296, 409, 373, 360, 399, 344, 320, 290, 397, 374, 402, 421, 466, 410, 381, 328, 354, 356, 390, 374, 406, 355, 359, 324, 341, 353, 350, 365, 356, 358, 329, 314, 328, 349, 347, 356, 354, 362, 321, 308, 284, 325, 334, 336, 355, 323, 318, 232, 316, 279, 304, 320, 309, 318, 314,};
int Black_Knight_Table_mg[64] = 
{ 232, 316, 279, 304, 320, 309, 318, 314, 308, 284, 325, 334, 336, 355, 323, 318, 314, 328, 349, 347, 356, 354, 362, 321, 324, 341, 353, 350, 365, 356, 358, 329, 328, 354, 356, 390, 374, 406, 355, 359, 290, 397, 374, 402, 421, 466, 410, 381, 264, 296, 409, 373, 360, 399, 344, 320, 170, 248, 303, 288, 398, 240, 322, 230,};
int White_Bishop_Table_mg[64] = 
{ 336, 369, 283, 328, 340, 323, 372, 357, 339, 381, 347, 352, 395, 424, 383, 318, 349, 402, 408, 405, 400, 415, 402, 363, 361, 370, 384, 415, 402, 402, 372, 363, 359, 378, 378, 391, 399, 377, 375, 369, 365, 380, 380, 380, 379, 392, 383, 375, 369, 380, 381, 365, 372, 386, 398, 366, 332, 362, 351, 344, 352, 353, 326, 344,};
int Black_Bishop_Table_mg[64] = 
{ 332, 362, 351, 344, 352, 353, 326, 344, 369, 380, 381, 365, 372, 386, 398, 366, 365, 380, 380, 380, 379, 392, 383, 375, 359, 378, 378, 391, 399, 377, 375, 369, 361, 370, 384, 415, 402, 402, 372, 363, 349, 402, 408, 405, 400, 415, 402, 363, 339, 381, 347, 352, 395, 424, 383, 318, 336, 369, 283, 328, 340, 323, 372, 357,};
int White_Rook_Table_mg[64] = 
{ 509, 519, 509, 528, 540, 486, 508, 520, 504, 509, 535, 539, 557, 544, 503, 521, 472, 496, 503, 513, 494, 522, 538, 493, 453, 466, 484, 503, 501, 512, 469, 457, 441, 451, 465, 476, 486, 470, 483, 454, 432, 452, 461, 460, 480, 477, 472, 444, 433, 461, 457, 468, 476, 488, 471, 406, 458, 464, 478, 494, 493, 484, 440, 451,};
int Black_Rook_Table_mg[64] = 
{ 458, 464, 478, 494, 493, 484, 440, 451, 433, 461, 457, 468, 476, 488, 471, 406, 432, 452, 461, 460, 480, 477, 472, 444, 441, 451, 465, 476, 486, 470, 483, 454, 453, 466, 484, 503, 501, 512, 469, 457, 472, 496, 503, 513, 494, 522, 538, 493, 504, 509, 535, 539, 557, 544, 503, 521, 509, 519, 509, 528, 540, 486, 508, 520,};
int White_Queen_Table_mg[64] = 
{ 997,1025,1054,1037,1084,1069,1068,1070,1001, 986,1020,1026,1009,1082,1053,1079,1012,1008,1032,1033,1054,1081,1072,1082, 998, 998,1009,1009,1024,1042,1023,1026,1016, 999,1016,1015,1023,1021,1028,1022,1011,1027,1014,1023,1020,1027,1039,1030, 990,1017,1036,1027,1033,1040,1022,1026,1024,1007,1016,1035,1010,1000, 994, 975,};
int Black_Queen_Table_mg[64] = 
{1024,1007,1016,1035,1010,1000, 994, 975, 990,1017,1036,1027,1033,1040,1022,1026,1011,1027,1014,1023,1020,1027,1039,1030,1016, 999,1016,1015,1023,1021,1028,1022, 998, 998,1009,1009,1024,1042,1023,1026,1012,1008,1032,1033,1054,1081,1072,1082,1001, 986,1020,1026,1009,1082,1053,1079, 997,1025,1054,1037,1084,1069,1068,1070,};
int White_King_Table_mg[64] = 
{ -65,  23,  16, -15, -56, -34,   2,  13,  29,  -1, -20,  -7,  -8,  -4, -38, -29,  -9,  24,   2, -16, -20,   6,  22, -22, -17, -20, -12, -27, -30, -25, -14, -36, -49,  -1, -27, -39, -46, -44, -33, -51, -14, -14, -22, -46, -44, -30, -15, -27,   1,   7,  -8, -64, -43, -16,   9,   8, -15,  36,  12, -54,   8, -28,  24,  14,};
int Black_King_Table_mg[64] = { -15,  36,  12, -54,   8, -28,  24,  14,   1,   7,  -8, -64, -43, -16,   9,   8, -14, -14, -22, -46, -44, -30, -15, -27, -49,  -1, -27, -39, -46, -44, -33, -51, -17, -20, -12, -27, -30, -25, -14, -36,  -9,  24,   2, -16, -20,   6,  22, -22,  29,  -1, -20,  -7,  -8,  -4, -38, -29, -65,  23,  16, -15, -56, -34,   2,  13,};

int White_Pawn_Table_eg[64] = 
{  94,  94,  94,  94,  94,  94,  94,  94, 272, 267, 252, 228, 241, 226, 259, 281, 188, 194, 179, 161, 150, 147, 176, 178, 126, 118, 107,  99,  92,  98, 111, 111, 107, 103,  91,  87,  87,  86,  97,  93,  98, 101,  88,  95,  94,  89,  93,  86, 107, 102, 102, 104, 107,  94,  96,  87,  94,  94,  94,  94,  94,  94,  94,  94,};
int Black_Pawn_Table_eg[64] = 
{  94,  94,  94,  94,  94,  94,  94,  94, 107, 102, 102, 104, 107,  94,  96,  87,  98, 101,  88,  95,  94,  89,  93,  86, 107, 103,  91,  87,  87,  86,  97,  93, 126, 118, 107,  99,  92,  98, 111, 111, 188, 194, 179, 161, 150, 147, 176, 178, 272, 267, 252, 228, 241, 226, 259, 281,  94,  94,  94,  94,  94,  94,  94,  94,};
int White_Knight_Table_eg[64] = 
{ 223, 243, 268, 253, 250, 254, 218, 182, 256, 273, 256, 279, 272, 256, 257, 229, 257, 261, 291, 290, 280, 272, 262, 240, 264, 284, 303, 303, 303, 292, 289, 263, 263, 275, 297, 306, 297, 298, 285, 263, 258, 278, 280, 296, 291, 278, 261, 259, 239, 261, 271, 276, 279, 261, 258, 237, 252, 230, 258, 266, 259, 263, 231, 217,};
int Black_Knight_Table_eg[64] = 
{ 252, 230, 258, 266, 259, 263, 231, 217, 239, 261, 271, 276, 279, 261, 258, 237, 258, 278, 280, 296, 291, 278, 261, 259, 263, 275, 297, 306, 297, 298, 285, 263, 264, 284, 303, 303, 303, 292, 289, 263, 257, 261, 291, 290, 280, 272, 262, 240, 256, 273, 256, 279, 272, 256, 257, 229, 223, 243, 268, 253, 250, 254, 218, 182,};
int White_Bishop_Table_eg[64] = 
{ 283, 276, 286, 289, 290, 288, 280, 273, 289, 293, 304, 285, 294, 284, 293, 283, 299, 289, 297, 296, 295, 303, 297, 301, 294, 306, 309, 306, 311, 307, 300, 299, 291, 300, 310, 316, 304, 307, 294, 288, 285, 294, 305, 307, 310, 300, 290, 282, 283, 279, 290, 296, 301, 288, 282, 270, 274, 288, 274, 292, 288, 281, 292, 280,};
int Black_Bishop_Table_eg[64] = 
{ 274, 288, 274, 292, 288, 281, 292, 280, 283, 279, 290, 296, 301, 288, 282, 270, 285, 294, 305, 307, 310, 300, 290, 282, 291, 300, 310, 316, 304, 307, 294, 288, 294, 306, 309, 306, 311, 307, 300, 299, 299, 289, 297, 296, 295, 303, 297, 301, 289, 293, 304, 285, 294, 284, 293, 283, 283, 276, 286, 289, 290, 288, 280, 273,};
int White_Rook_Table_eg[64] = 
{ 525, 522, 530, 527, 524, 524, 520, 517, 523, 525, 525, 523, 509, 515, 520, 515, 519, 519, 519, 517, 516, 509, 507, 509, 516, 515, 525, 513, 514, 513, 511, 514, 515, 517, 520, 516, 507, 506, 504, 501, 508, 512, 507, 511, 505, 500, 504, 496, 506, 506, 512, 514, 503, 503, 501, 509, 503, 514, 515, 511, 507, 499, 516, 492,};
int Black_Rook_Table_eg[64] = 
{ 503, 514, 515, 511, 507, 499, 516, 492, 506, 506, 512, 514, 503, 503, 501, 509, 508, 512, 507, 511, 505, 500, 504, 496, 515, 517, 520, 516, 507, 506, 504, 501, 516, 515, 525, 513, 514, 513, 511, 514, 519, 519, 519, 517, 516, 509, 507, 509, 523, 525, 525, 523, 509, 515, 520, 515, 525, 522, 530, 527, 524, 524, 520, 517,};
int White_Queen_Table_eg[64] = 
{ 927, 958, 958, 963, 963, 955, 946, 956, 919, 956, 968, 977, 994, 961, 966, 936, 916, 942, 945, 985, 983, 971, 955, 945, 939, 958, 960, 981, 993, 976, 993, 972, 918, 964, 955, 983, 967, 970, 975, 959, 920, 909, 951, 942, 945, 953, 946, 941, 914, 913, 906, 920, 920, 913, 900, 904, 903, 908, 914, 893, 931, 904, 916, 895,};
int Black_Queen_Table_eg[64] = 
{ 903, 908, 914, 893, 931, 904, 916, 895, 914, 913, 906, 920, 920, 913, 900, 904, 920, 909, 951, 942, 945, 953, 946, 941, 918, 964, 955, 983, 967, 970, 975, 959, 939, 958, 960, 981, 993, 976, 993, 972, 916, 942, 945, 985, 983, 971, 955, 945, 919, 956, 968, 977, 994, 961, 966, 936, 927, 958, 958, 963, 963, 955, 946, 956,};
int White_King_Table_eg[64] = 
{ -74, -35, -18, -18, -11,  15,   4, -17, -12,  17,  14,  17,  17,  38,  23,  11,  10,  17,  23,  15,  20,  45,  44,  13,  -8,  22,  24,  27,  26,  33,  26,   3, -18,  -4,  21,  24,  27,  23,   9, -11, -19,  -3,  11,  21,  23,  16,   7,  -9, -27, -11,   4,  13,  14,   4,  -5, -17, -53, -34, -21, -11, -28, -14, -24, -43,};
int Black_King_Table_eg[64] = 
{ -53, -34, -21, -11, -28, -14, -24, -43, -27, -11,   4,  13,  14,   4,  -5, -17, -19,  -3,  11,  21,  23,  16,   7,  -9, -18,  -4,  21,  24,  27,  23,   9, -11,  -8,  22,  24,  27,  26,  33,  26,   3,  10,  17,  23,  15,  20,  45,  44,  13, -12,  17,  14,  17,  17,  38,  23,  11, -74, -35, -18, -18, -11,  15,   4, -17,};

int* PestoWPiecesEG[6] = {White_Pawn_Table_eg, White_Knight_Table_eg, White_Bishop_Table_eg, White_Rook_Table_eg, White_Queen_Table_eg, White_King_Table_eg}; 
int* PestoBPiecesEG[6]  = {Black_Pawn_Table_eg, Black_Knight_Table_eg, Black_Bishop_Table_eg, Black_Rook_Table_eg, Black_Queen_Table_eg, Black_King_Table_eg};
int* PestoBPiecesMG[6] = {Black_Pawn_Table_mg, Black_Knight_Table_mg, Black_Bishop_Table_mg, Black_Rook_Table_mg, Black_Queen_Table_mg, Black_King_Table_mg}; 
int* PestoWPiecesMG[6] = {White_Pawn_Table_mg, White_Knight_Table_mg, White_Bishop_Table_mg, White_Rook_Table_mg, White_Queen_Table_mg, White_King_Table_mg};
//evalute the current m_board state with PESTO tables tables and adapted from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function (original table source: https://rofchade.nl/?p=307)
int Search::evaluate() const{ 
    int whiteScoreEG = 0;  
    int blackScoreEG = 0;
    int whiteScoreMG = 0;  
    int blackScoreMG = 0;
    int gamePhase = 0;
    Bitboard occupiedW = m_board.occupied(Turn::White);
    Bitboard occupiedB = m_board.occupied(Turn::Black);

    while (occupiedW.to_ullong() != 0ULL){
        int i = occupiedW._Find_first();
        Piece piece = *(m_board.piece(*(Square::fromIndex(i))));
        
        
        whiteScoreEG += PestoWPiecesEG[(int) piece.type()][i];
        whiteScoreMG += PestoWPiecesMG[(int) piece.type()][i];
        gamePhase += gamePhaseScores[(int) piece.type()];
        occupiedW[i] = false;
    }
    while (occupiedB.to_ullong() != 0ULL){
        int i = occupiedB._Find_first();
        Piece piece = *(m_board.piece(*(Square::fromIndex(i))));
        
        blackScoreEG += PestoBPiecesEG[(int) piece.type()][i];
        blackScoreMG += PestoBPiecesMG[(int) piece.type()][i];
        gamePhase += gamePhaseScores[(int) piece.type()];
        occupiedB[i] = false;
    }
    
    int mgScore =  whiteScoreMG - blackScoreMG;
    int egScore =  whiteScoreEG - blackScoreEG;
    int mgPhase = gamePhase;
    if (mgPhase > 24){
        mgPhase = 24;
    } 
    int egPhase = 24 - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / 24;
}

static int MVVLVA[6][6] = {
 	10500, 20500, 30500, 40500, 50500, 60500,  
	10400, 20400, 30400, 40400, 50400, 60400,  
	10300, 20300, 30300, 40300, 50300, 60300,  
	10200, 20200, 30200, 40200, 50200, 60200,  
	10100, 20100, 30100, 40100, 50100, 60100,  
	10000, 20000, 30000, 40000, 50000, 60000,  
};
//function that returns the score of a move
int Search::moveScore(const Move& move){
    
    if(!pvs.empty()) {
        if (pvs[0] == move){
            return 99999999;
        }
        
    } 

    if(m_board.occupied(!m_board.turn()).test(move.to().index())){ 
                                                                                                       
        return MVVLVA[(int)(*m_board.piece(move.to())).type()][(int)(*m_board.piece(move.to())).type()] + 9999999; 
    }
    else {
        //if first killer move
        if (searchkillers[0] == move){
            return 888888;
        }
        else if (searchkillers[1] == move){
            return 777777;
        }
        else {
            if (m_board.turn() == Turn::White){
                
                return searchHistoryW[(int) (*m_board.piece(move.from())).type() ][move.to().index()] ;
            }
            else{
                
                return searchHistoryW[(int) (*m_board.piece(move.from())).type() ][move.to().index()] ;
            }
            
        }

        //if second killer move 

    }

    return 0;
}
//takes longer than sorting in a map not used anymore
bool Search::moveScoreComparator(const Move& a, const Move& b) {  
  
  // Assume that each move has a score associated with it
  int scoreA = moveScore(a);
  int scoreB = moveScore(b);
  
  // Sort moves in descending order of score
  return scoreA > scoreB;
}

//function that orders the moves in the vector based on the score of the move, now done seperatly can be build into the move generation class but that would require a lot of changes
void Search::OrderMoves(MoveVec& moves ){
  
   
    std::sort(moves.begin(), moves.end(), bind(&Search::moveScoreComparator,this,std::placeholders::_1, std::placeholders::_2));
    

}



//still gets stuck dont use only minor improvements 
int Search::quiesce(int alpha, int beta, bool maximizingPlayer,int iteration) { 
    
    
    std::vector<Move> moves;
    m_board.opponentCanCaptureMoves(moves);
    
    int score = evaluate();

    if (moves.size() == 0) {
        return score;
    }
    
    if (maximizingPlayer) {
        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    } else {
        if (score <= alpha) {
            return alpha;
        }
        if (score < beta) {
            beta = score;
        }
    }
    std::multimap<int, int, std::greater<int>> score_map;
    for (int i = 0; i < (int) moves.size();i++){
        score_map.insert({moveScore(moves[i]),i});               
    }


    std::vector<Bitboard> boards;
    m_board.storeAllBoars(boards);
    CastlingRights castlingRights = m_board.castlingRights();

    Turn turn = m_board.turn();
    

    for (const auto& [key, val] : score_map) {
        Move move = moves[val];
        m_board.makeMove(move);
        m_board.setTurn(turn);
        if (m_board.isCheck()){
            
            m_board.restoreAllBoars(boards);
            m_board.setCastlingRights(castlingRights);
            
        }
        else{
        m_board.setTurn(!turn);
        score = quiesce(alpha, beta, !maximizingPlayer,iteration+1);
        m_board.restoreAllBoars(boards);
        m_board.setCastlingRights(castlingRights);
        m_board.setTurn(turn);

        if (maximizingPlayer) {
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        } else {
            if (score <= alpha) {
                return alpha;
            }
            if (score < beta) {
                beta = score;
            }
        }
        }
    }

    if (maximizingPlayer) {
        return alpha;
    } else {
        return beta;
    }
}


