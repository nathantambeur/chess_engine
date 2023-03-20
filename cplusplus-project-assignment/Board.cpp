#include "Board.hpp"
#include <ostream>
#include <cassert>
#include <iostream>
#include <cmath>
#include <optional>
#include <iosfwd>
#include <vector>

using Bitboard = std::bitset<64>;

Bitboard notAFileBoard = 0xfefefefefefefefe;
Bitboard notHFileBoard = 0x7f7f7f7f7f7f7f7f;
Bitboard notABFileBoard = 0xfcfcfcfcfcfcfcfc;
Bitboard notGHFileBoard = 0x3f3f3f3f3f3f3f3f;



void Board::startBoard()
{
    m_turn = PieceColor::White;
    WPawns = 0x000000000000ff00;
    BPawns = 0x00ff000000000000;

    WKnights = 0x0000000000000042;
    BKnights = 0x4200000000000000;

    WBishops = 0x0000000000000024;
    BBishops = 0x2400000000000000;

    WRooks = 0x0000000000000081;
    BRooks = 0x8100000000000000;

    WQueens = 0x0000000000000010;
    BQueens = 0x1000000000000000;

    WKings = 0x0000000000000008;
    BKings = 0x0800000000000000;

    
    Enpassant = 0;

    m_castlingRights = CastlingRights::All;

}
Board::Board(){
    m_turn = PieceColor::White;
    BPawns = 0;
    WPawns = 0;
    BKings = 0;
    WKings = 0;
    BQueens = 0;
    WQueens = 0;
    BBishops = 0;
    WBishops = 0;
    BKnights = 0;
    WKnights = 0;
    BRooks = 0;
    WRooks = 0;
    Enpassant = 0;
    m_castlingRights = CastlingRights::None;
}
Bitboard& Board::getBitboard(Piece piece) {
    switch (piece.color())
    {
    case PieceColor::White:
        switch (piece.type())
        {
        case PieceType::Pawn:
            return WPawns ; 
        case PieceType::Knight:
            return WKnights;
        case PieceType::Bishop:
            return WBishops;
        case PieceType::Rook:
            return WRooks;
        case PieceType::King:
            return WKings;
        case PieceType::Queen:
            return WQueens;
        }
        break;
    case PieceColor::Black:
        switch (piece.type())
        {
        case PieceType::Pawn:
            return BPawns;
        case PieceType::Knight:
            return BKnights;
        case PieceType::Bishop:
            return BBishops;
        case PieceType::Rook:
            return BRooks;
        case PieceType::King:
            return BKings;
        case PieceType::Queen:
            return BQueens;
        }
        break;
    }
    return Enpassant;
}

void Board::setPiece(const Square& square, const Piece::Optional& piece) {
    
        
        WPawns[square.index()] = false;
        BPawns[square.index()] = false;
        WKnights[square.index()] = false;
        BKnights[square.index()] = false;
        WBishops[square.index()] = false;
        BBishops[square.index()] = false;
        WRooks[square.index()] = false;
        BRooks[square.index()] = false;
        WKings[square.index()] = false;
        BKings[square.index()] = false;
        WQueens[square.index()] = false;
        BQueens[square.index()] = false;
        
        if (piece){
            getBitboard(*piece)[square.index()] = true;    
        }    //set all other bitboards to false
        
    
    
    }

Piece::Optional Board::piece(const Square& square) const {
    if (WPawns[square.index()]) {
        return Piece(PieceColor::White,PieceType::Pawn);
    }
    if (BPawns[square.index()]) {
        return Piece(PieceColor::Black, PieceType::Pawn);
    }
    if (WKnights[square.index()]) {
        return Piece(PieceColor::White, PieceType::Knight);
    }
    if (BKnights[square.index()]) {
        return Piece(PieceColor::Black, PieceType::Knight);
    }
    if (WBishops[square.index()]) {
        return Piece(PieceColor::White, PieceType::Bishop);
    }
    if (BBishops[square.index()]) {
        return Piece(PieceColor::Black, PieceType::Bishop);
    }
    if (WRooks[square.index()]) {
        return Piece(PieceColor::White, PieceType::Rook);
    }
    if (BRooks[square.index()]) {
        return Piece(PieceColor::Black, PieceType::Rook);
    }
    if (WKings[square.index()]) {
        return Piece(PieceColor::White, PieceType::King);
    }
    if (BKings[square.index()]) {
        return Piece(PieceColor::Black, PieceType::King);
    }
    if (WQueens[square.index()]) {
        return Piece(PieceColor::White, PieceType::Queen);
    }
    if (BQueens[square.index()]) {
        return Piece(PieceColor::Black, PieceType::Queen);
    }
    return std::nullopt;
}

void Board::setTurn(PieceColor turn) {
    m_turn = turn;
}

PieceColor Board::turn() const {
    return m_turn;
}

void Board::setCastlingRights(CastlingRights cr) {
    m_castlingRights = cr;
}

CastlingRights Board::castlingRights() const {
    return m_castlingRights;
}

void Board::setEnPassantSquare(const Square::Optional& square) {
    if (square) {
        Enpassant[(*square).index()] = true;
    }
}

Square::Optional Board::enPassantSquare() const {
    if (Enpassant ==0){
        return std::nullopt;
    }  
    else{
        //careful int to unsigned int conversion
        return Square::fromIndex(Enpassant._Find_first());    
}
}

void Board::makeMove(const Move& move) {
    

    Piece piece = *(Board::piece(move.from()));
    //leave the from square empty, set the to square to true 
    setPiece(move.to(), piece);
    setPiece(move.from(), std::nullopt);
    
    if (move.to().index() == (*enPassantSquare()).index() && piece.type() == PieceType::Pawn) {
        //if the move is an enpassant move, remove the pawn from the square behind the to square
        if (piece.color() == PieceColor::White) {
            Board::getBitboard(Piece(PieceColor::Black, PieceType::Pawn))[move.to().index() - 8] = false;
        }
        else {
            Board::getBitboard(Piece(PieceColor::White, PieceType::Pawn))[move.to().index() + 8] = false;
        } 
    }
    
            
    //reset enpassant square (only works for one turn)
    Enpassant = 0;
    //if the move is a pawn move and the pawn is on the 2nd or 7th rank, set the enpassant square. (and it was not a capture move )
    if (piece.color() == PieceColor::White && piece.type() == PieceType::Pawn && move.from().rank() == 1 && move.to().rank() == 3) {
            Board::setEnPassantSquare(Square::fromIndex(move.to().index() - 8));
    }
    if (piece.color() == PieceColor::Black && piece.type() == PieceType::Pawn && move.from().rank() == 6 && move.to().rank() == 4) {
            Board::setEnPassantSquare(Square::fromIndex(move.to().index() + 8));
    }
    
    //castling rights
    //if the king moves lose all rights
    if (piece.type() == PieceType::King) {
        if (piece.color() == PieceColor::White) {
            m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::White);
        }
        else {
            m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::Black);
        }
    }
    //lose casteling rights if the rook is taken or moved 
    if (move.to().index() == 0 || move.from().index() == 0) {
        
        m_castlingRights = CastlingRights(m_castlingRights & ~ CastlingRights::WhiteQueenside);
    }
    if (move.to().index() == 7 || move.from().index() == 7) {
       
        m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::WhiteKingside);
    }
    if (move.to().index() == 56 || move.from().index() == 56) {
        
        m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::BlackQueenside);
    }
    if (move.to().index() == 63 || move.from().index() == 63) {
        
        m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::BlackKingside);
    }

    //if the piece is a king that moves 2 squares, it is a castling move. then also move the rook to the correct square
    if (piece.type() == PieceType::King && abs((int) move.from().index()- (int) move.to().index()) == 2) {
        if (piece.color() == PieceColor::White) {
            if (move.to().index() == 2) {
                WRooks[0] = false;
                WRooks[3] = true;
            }
            if (move.to().index() == 6) {
                WRooks[7] = false;
                WRooks[5] = true;
            }
        }
        else {
            if (move.to().index() == 58) {
                BRooks[56] = false;
                BRooks[59] = true;
            }
            if (move.to().index() == 62) {
                BRooks[63] = false;
                BRooks[61] = true;
            }
        }
    }
    //if piecetype is rook and it is on the starting square with casteling rights also move the king to the correct square
    if (piece.type() == PieceType::Rook) {
        if (piece.color() == PieceColor::White) {
            if ((move.from().index() == 0)&& ((m_castlingRights & CastlingRights::WhiteQueenside) != CastlingRights::None)) {
                WKings[4] = false;
                WKings[2] = true;
                m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::White);
            }
            if ((move.from().index() == 7) && ((m_castlingRights & CastlingRights::WhiteKingside) != CastlingRights::None)) {
                WKings[4] = false;
                WKings[6] = true;
                m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::White);
            }
            
        }
        else {
            if ((move.from().index() == 56) && ((m_castlingRights & CastlingRights::BlackQueenside) != CastlingRights::None)) {
                BKings[60] = false;
                BKings[58] = true;
                m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::Black);
            }
            if ((move.from().index() == 63) && ((m_castlingRights & CastlingRights::BlackKingside) != CastlingRights::None)) {
                BKings[60] = false;
                BKings[62] = true;
                m_castlingRights = CastlingRights(m_castlingRights & ~CastlingRights::Black);
            }
            
        }
    }
    //check promotion and set the piece to the correct type specified in the move
    if (piece.type() == PieceType::Pawn && move.promotion().has_value() && (move.to().rank() == 7 || move.to().rank() == 0)) {
       
        Piece promotionPiece = Piece(piece.color(), move.promotion().value());
        setPiece(move.to(), promotionPiece);
        setPiece(move.to(), promotionPiece);

    }
    //hand turn over to opponent
    m_turn = !m_turn;
    
}

void Board::storeAllBoars(std::vector<Bitboard>& boars){
    boars.push_back(WPawns);
    boars.push_back(BPawns);
    boars.push_back(WKings);
    boars.push_back(BKings);
    boars.push_back(WQueens);
    boars.push_back(BQueens);
    boars.push_back(WBishops);
    boars.push_back(BBishops);
    boars.push_back(WKnights);
    boars.push_back(BKnights);
    boars.push_back(WRooks);
    boars.push_back(BRooks);
    boars.push_back(Enpassant);
    
}
void Board::restoreAllBoars(const std::vector<Bitboard>& boars){
    WPawns = boars[0];
    BPawns = boars[1];
    WKings = boars[2];
    BKings = boars[3];
    WQueens = boars[4];
    BQueens = boars[5];
    WBishops = boars[6];
    BBishops = boars[7];
    WKnights = boars[8];
    BKnights = boars[9];
    WRooks = boars[10];
    BRooks = boars[11];
    Enpassant = boars[12];
}
Bitboard Board::occupied() const {
    return BPawns|WPawns|BKings|WKings|BQueens|WQueens|BBishops|WBishops|BKnights|WKnights|BRooks|WRooks;
}
Bitboard Board::occupied(PieceColor color) const {
    if (color == PieceColor::White) {
        return WPawns|WKings|WQueens|WBishops|WKnights|WRooks;
    }
    else {
        return BPawns|BKings|BQueens|BBishops|BKnights|BRooks;
    }
}
Bitboard Board::firstpawnPushMoves(PieceColor color) const{

    if (color == PieceColor::White) {
        auto firstpush = (WPawns << 8) & ~occupied();
       
        return firstpush;
    }
    else {
        auto firstpush = (BPawns >> 8) & ~occupied();
        return firstpush;
    }
}
Bitboard Board::secondpawnPushMoves(PieceColor color) const {
    if (color == PieceColor::White) {
        auto firstpush = (WPawns << 8) & ~occupied();
        const Bitboard rank4 = Bitboard(0x00000000FF000000);
        auto secondpush = ((firstpush << 8 & rank4) & ~occupied());
        return secondpush;
    }
    else {
        auto firstpush = (BPawns >> 8) & ~occupied();
        const Bitboard rank5 = Bitboard(0x000000FF00000000);
        auto secondpush = ((firstpush >> 8 & rank5) & ~occupied());
        return secondpush;
    }
}


//more efficient check of if the opponent can go to 
Bitboard Board::opponentCanCapture() const{
    Turn turn = (m_turn == PieceColor::White ? PieceColor::Black : PieceColor::White);
    Bitboard attacks = 0ULL;

   

    Bitboard pawns;
    Bitboard queens;
    Bitboard rooks;
    Bitboard bishops;
    Bitboard knights;
    Bitboard kings;
    Bitboard opponents;
    
    if (turn == PieceColor::White) {
        queens = WQueens;
        rooks = WRooks;
        bishops = WBishops;
        knights = WKnights;
        kings = WKings;
        pawns = WPawns;
        opponents = occupied(PieceColor::Black);
    }
    else {
        queens = BQueens;
        rooks = BRooks;
        bishops = BBishops;
        knights = BKnights;
        kings = BKings;
        pawns = BPawns;
        opponents = occupied(PieceColor::White);

    }
    Bitboard pawns_copy = pawns;
    Bitboard dummy = 0ULL;
    while (pawns_copy.to_ullong() != 0ULL) {
        int from = pawns_copy._Find_first();
        dummy = 0ULL;
        dummy[from] = true;
        
        if (turn == PieceColor::White) {
            attacks |= ((((dummy & notAFileBoard)  << 7) & (opponents|Enpassant)) | (((dummy & notHFileBoard) << 9) & (opponents|Enpassant)));
        }
        else {
            attacks |= ((((dummy& notHFileBoard)>> 7) & (opponents|Enpassant)) | (((dummy & notAFileBoard ) >> 9) & (opponents|Enpassant)));
        }
        pawns_copy[from] = false;
    }
    Bitboard blockers = (occupied());
    while (queens.to_ullong() != 0ULL) {
        const Square square = *(Square::fromIndex(queens._Find_first()));
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        queens[square.index()] = false;
    }
    while (rooks.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(rooks._Find_first()));
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        rooks[square.index()] = false;
    }
    while (bishops.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(bishops._Find_first()));
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        bishops[square.index()] = false;
    }
    while (knights.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(knights._Find_first()));
        attacks |= mb.Nmoves(square) & ~occupied(turn);  
        knights[square.index()] = false;
    }
    while (kings.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(kings._Find_first()));
        attacks |= mb.Kmoves(square) & ~occupied(turn);
        kings[square.index()] = false;
    }
    attacks &= opponents;
    return attacks;
}

void Board::addAttacks(MoveVec& moves, Bitboard& attacks, const Square& from) const{
    while (attacks.to_ullong() != 0ULL) {
        Square to = *(Square::fromIndex(attacks._Find_first()));
        moves.push_back(Move(from, to));
        attacks[to.index()] = false;
    }
}

//generates efficiently all possible capture moves THE CURRENT ONE can make 
void Board::opponentCanCaptureMoves(MoveVec& moves) const{
    Turn turn = (m_turn == PieceColor::White ? PieceColor::Black : PieceColor::White);
    turn = !turn;
    Bitboard attacks = 0ULL;
    Bitboard pawns;
    
    Bitboard queens;
    Bitboard rooks;
    Bitboard bishops;
    Bitboard knights;
    Bitboard kings;
    Bitboard opponents;
    if (turn == PieceColor::White) {
        queens = WQueens;
        rooks = WRooks;
        bishops = WBishops;
        knights = WKnights;
        kings = WKings;
        pawns = WPawns;
        opponents = occupied(PieceColor::Black);
    }
    else {
        queens = BQueens;
        rooks = BRooks;
        bishops = BBishops;
        knights = BKnights;
        kings = BKings;
        pawns = BPawns;
        opponents = occupied(PieceColor::White);

    }
    Bitboard pawns_copy = pawns;
    Bitboard dummy = 0ULL;
    attacks = 0ULL;
    while (pawns_copy.to_ullong() != 0ULL) {
        int from = pawns_copy._Find_first();
        dummy = 0ULL;
        dummy[from] = true;
        if (turn == PieceColor::White) {
            attacks |= (((dummy & notAFileBoard)  << 7 & (opponents|Enpassant)) | ((dummy & notHFileBoard) << 9 & (opponents|Enpassant)));
        }
        else {
            attacks |= (((dummy& notHFileBoard)>> 7 & (opponents|Enpassant)) | ((dummy & notAFileBoard ) >> 9 & (opponents|Enpassant)));
        }
        addAttacks(moves, attacks, *(Square::fromIndex(from)));
        pawns_copy[from] = false;
    }
    attacks = 0ULL;
    Bitboard blockers = (occupied());
    while (queens.to_ullong() != 0ULL) {
        const Square square = *(Square::fromIndex(queens._Find_first()));
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        attacks &= (opponents);
        addAttacks(moves, attacks, square);
        queens[square.index()] = false;
    }
    attacks = 0ULL;
    while (rooks.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(rooks._Find_first()));
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        attacks &= (opponents);
        addAttacks(moves, attacks, square);
        rooks[square.index()] = false;
    }
    attacks = 0ULL;
    while (bishops.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(bishops._Find_first()));
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        attacks &= (opponents);
        addAttacks(moves, attacks, square);
        bishops[square.index()] = false;
    }
    attacks = 0ULL;
    while (knights.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(knights._Find_first()));
        attacks |= mb.Nmoves(square) & ~occupied(turn);  
        attacks &= (opponents);
        addAttacks(moves, attacks, square);
        knights[square.index()] = false;
    }
    attacks = 0ULL;
    while (kings.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(kings._Find_first()));
        attacks |= mb.Kmoves(square) & ~occupied(turn);
        attacks &= (opponents);
        addAttacks(moves, attacks, square);
        kings[square.index()] = false;
    }
}

//generate all possible psuedo legal moves for the current board using all the attack functions form above
void Board::pseudoLegalMoves(MoveVec& moves) const {
    PieceColor this_turn = turn();
    basicPseudoLegalMoves(moves, this_turn);
    
    Bitboard opponents;
    Bitboard blockers = occupied();
    if (this_turn == PieceColor::White) {
        opponents = occupied(PieceColor::Black);
    }
    else {
        opponents = occupied(PieceColor::White);
    }
    //check wwather casteling rights are available then check weather the squares are empty then and only then generate the moves of the opponent otherwise needlessly coplicated also add opponent moves maybe 
    addCastelingMoves(moves, blockers);
}


void Board::pseudoLegalMovesFrom(const Square& from,
                                 Board::MoveVec& moves) const {
    MoveVec temp;
    pseudoLegalMoves(temp); 
    int size = temp.size();
    std::vector<int> vect;
    for (int i = 0; i < size; i++) {
        if (temp[i].from().index() == from.index()) {
            vect.push_back(i);
        }
    }
    size = vect.size();
    for (int i = 0; i < size; i++) {
        moves.push_back(temp[vect[i]]);
    }
}


void Board::basicPseudoLegalMoves(MoveVec& moves, PieceColor& turn) const {

    Bitboard firstpawnmoves = firstpawnPushMoves(turn);
    Bitboard pawns;
    PieceType type[4] = { PieceType::Knight, PieceType::Bishop, PieceType::Rook, PieceType::Queen};
    if (turn == PieceColor::White) {
        pawns = WPawns;
        while (firstpawnmoves.to_ullong() != 0ULL) {
            int to = firstpawnmoves._Find_first();
            int from = to - 8;
            const Square fromsquare = *(Square::fromIndex(from));
            const Square tosquare = *(Square::fromIndex(to));
            if (to > 55) {
                for (int i = 0; i < 4; i++) {
                    moves.push_back(Move(fromsquare, tosquare, type[i]));
                }
            }
            else {
            moves.push_back(Move(fromsquare, tosquare));
            }
            
            firstpawnmoves[to] = false;
        }
    
    }
    else {
        pawns = BPawns;
        while (firstpawnmoves.to_ullong() != 0ULL) {
            int to = firstpawnmoves._Find_first();
            int from = to + 8;

            const Square fromsquare = *(Square::fromIndex(from));
            const Square tosquare = *(Square::fromIndex(to));
            //check for promotion
            if (to < 8) {
                for (int i = 0; i < 4; i++) {
                    moves.push_back(Move(fromsquare, tosquare, type[i]));
                }
            }
            else {
                moves.push_back(Move(fromsquare, tosquare));
            }
            firstpawnmoves[to] = false;
        }
    }
    Bitboard secondpawnmoves = secondpawnPushMoves(turn);
    if (turn == PieceColor::White) {
        while (secondpawnmoves.to_ullong() != 0ULL) {
            int to = secondpawnmoves._Find_first();
            int from = to - 16;
            //only if its for the right square 
            const Square fromsquare = *(Square::fromIndex(from));
            const Square tosquare = *(Square::fromIndex(to));
            //check promotion
            
            moves.push_back(Move(fromsquare, tosquare));
            secondpawnmoves[to] = false;
        }
    }
    else {
        while (secondpawnmoves.to_ullong() != 0ULL) {
            int to = secondpawnmoves._Find_first();
            int from = to + 16;
            //only if its for the right square 

            const Square fromsquare = *(Square::fromIndex(from));
            const Square tosquare = *(Square::fromIndex(to));
            //check promotion
           
            moves.push_back(Move(fromsquare, tosquare));
            secondpawnmoves[to] = false;
        }
    }
    
    Bitboard pawns_copy = pawns;
    Bitboard queens;
    Bitboard rooks;
    Bitboard bishops;
    Bitboard knights;
    Bitboard kings;
    Bitboard opponents;
    
    if (turn == PieceColor::White) {
        queens = WQueens;
        rooks = WRooks;
        bishops = WBishops;
        knights = WKnights;
        kings = WKings;
        opponents = occupied(PieceColor::Black);
    }
    else {
        queens = BQueens;
        rooks = BRooks;
        bishops = BBishops;
        knights = BKnights;
        kings = BKings;
        opponents = occupied(PieceColor::White);

    }
    Bitboard attacks;
    Bitboard dummy = 0ULL;
    while (pawns_copy.to_ullong() != 0ULL) {
        int from = pawns_copy._Find_first();
        dummy = 0ULL;
        dummy[from] = true;
        attacks = 0ULL;
        
        if (turn == PieceColor::White) {
            attacks = (((dummy & notAFileBoard)  << 7 & (opponents|Enpassant)) | ((dummy & notHFileBoard) << 9 & (opponents|Enpassant)));
        }
        else {
            attacks = (((dummy& notHFileBoard)>> 7 & (opponents|Enpassant)) | ((dummy & notAFileBoard ) >> 9 & (opponents|Enpassant)));
        }
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square fromsquare = *(Square::fromIndex(from));
            const Square tosquare = *(Square::fromIndex(to));
            //check promotion
            if (to > 55 || to < 8) {
                for (int i = 0; i < 4; i++) {
                    moves.push_back(Move(fromsquare, tosquare, type[i]));
                }
            }
            else {
                moves.push_back(Move(fromsquare, tosquare));
            }
            attacks[to] = false;
        }
        pawns_copy[from] = false;
    }

    Bitboard blockers = (occupied());
    while (queens.to_ullong() != 0ULL) {
        const Square square = *(Square::fromIndex(queens._Find_first()));
        //only if its for the right square 
        Bitboard attacks = 0ULL;
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        queens[square.index()] = false;
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square tosquare = *(Square::fromIndex(to));
            moves.push_back(Move(square, tosquare));
            attacks[to] = false;
        }
    }
    while (rooks.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(rooks._Find_first()));
        //only if its for the right square 
        Bitboard attacks = 0ULL;
        attacks |= mb.Rmoves(square, (blockers& mb.get_Rmask(square)))& ~occupied(turn);
        rooks[square.index()] = false;
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square tosquare = *(Square::fromIndex(to));
            moves.push_back(Move(square, tosquare));
            attacks[to] = false;
        }
    }
    while (bishops.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(bishops._Find_first()));
        //only if its for the right square 
        Bitboard attacks = 0ULL;
        attacks |= mb.Bmoves(square, (blockers& mb.get_Bmask(square)))& ~occupied(turn);
        bishops[square.index()] = false;
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square tosquare = *(Square::fromIndex(to));
            moves.push_back(Move(square, tosquare));
            attacks[to] = false;
        }
    }
    while (knights.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(knights._Find_first()));
        Bitboard attacks = mb.Nmoves(square) & ~occupied(turn);  
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square tosquare = *(Square::fromIndex(to));
            
            moves.push_back(Move(square, tosquare));
            attacks[to] = false;
        } 
        knights[square.index()] = false;
    }
    while (kings.to_ullong() != 0ULL) {
        Square square = *(Square::fromIndex(kings._Find_first()));
        Bitboard attacks = mb.Kmoves(square) & ~occupied(turn);
        while (attacks.to_ullong() != 0ULL) {
            int to = attacks._Find_first();
            const Square tosquare = *(Square::fromIndex(to));
            moves.push_back(Move(square, tosquare));
            attacks[to] = false;
        } 
        kings[square.index()] = false;
    }    
}
void Board::addCastelingMoves(MoveVec& moves,Bitboard& blockers) const{
    Bitboard opponents_moves;
    bool opponent_moves_generated = false;
    if (m_turn == PieceColor::White){
        if (((m_castlingRights & CastlingRights::WhiteKingside) != CastlingRights::None)  ){
            if (  blockers[Square::F1.index()] == false && blockers[Square::G1.index()] == false && WRooks[Square::H1.index()] == true && WKings[Square::E1.index()] == true){
                opponents_moves = opponentCanCapture();
                opponent_moves_generated = true;
                
                if (!(opponents_moves[5] || opponents_moves[6] || opponents_moves[4])){
                    {
                        moves.push_back(Move(Square::E1, Square::G1));
                        moves.push_back(Move(Square::H1, Square::F1));
                    }
                }
            }
        }
        if (((m_castlingRights & CastlingRights::WhiteQueenside) != CastlingRights::None)){
            if ( blockers[Square::D1.index()] == false && blockers[Square::C1.index()] == false && blockers[Square::B1.index()] == false && WRooks[Square::A1.index()] == true && WKings[Square::E1.index()] == true){
                if (!opponent_moves_generated){
                    opponents_moves = opponentCanCapture();
                    opponent_moves_generated = true;
                    
                }
                if (!(opponents_moves[1] || opponents_moves[2] || opponents_moves[3] || opponents_moves[4])){
                    moves.push_back(Move(Square::E1, Square::C1));
                    moves.push_back(Move(Square::A1, Square::D1));
                }

            }
        }
    }
    else{
        if (((m_castlingRights & CastlingRights::BlackKingside) != CastlingRights::None)){
            if ( blockers[Square::F8.index()] == false && blockers[Square::G8.index()] == false && BRooks[Square::H8.index()] == true && BKings[Square::E8.index()] == true){
                opponents_moves = opponentCanCapture();
                opponent_moves_generated = true;
                if (!(opponents_moves[61] || opponents_moves[62] || opponents_moves[60])){
                    moves.push_back(Move(Square::E8, Square::G8));
                    moves.push_back(Move(Square::H8, Square::F8));
                }
            }   
        }
        
        if (((m_castlingRights & CastlingRights::BlackQueenside) != CastlingRights::None)){
            if (blockers[Square::D8.index()] == false && blockers[Square::C8.index()] == false && blockers[Square::B8.index()] == false && BRooks[Square::A8.index()] == true && BKings[Square::E8.index()] == true ){
                if (!opponent_moves_generated){
                    opponents_moves = opponentCanCapture();
                    opponent_moves_generated = true;
                }
                if (!(opponents_moves[58] || opponents_moves[59] || opponents_moves[60])){
                    moves.push_back(Move(Square::E8, Square::C8));
                    moves.push_back(Move(Square::A8, Square::D8));
                }
            }
        }
    }                         

}

bool Board::isCheck() const{
    Bitboard king = (m_turn == PieceColor::White ? WKings : BKings);
    Bitboard opponents_moves = opponentCanCapture();
    return ((king & opponents_moves).to_ullong() != 0ULL);
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    for (int i = 0; i <8; i++) {
        for (int j = 0; j < 8; j++) {
            std::optional<Square> square = Square::fromIndex((8*(7-i) + j));
            if (square) {
                Piece::Optional piece = board.piece(*square);
                if (piece) {
                    os << *piece;
                }
                else {
                os << ".";
                }
            } 
            os << " ";
        }
        os << std::endl;
    }
    return os;
}

//function that gets all indeces labeled as 1 in the bitmap and returns a vector of all the indeces
std::vector<int> Board::get_Indeces(Bitboard bitmap) const {
    std::vector<int> indeces;
    while (bitmap.to_ullong() != 0ULL) {
        int index = bitmap._Find_first();
        indeces.push_back(index);
        bitmap[index] = false;
    }
    return indeces;
}

