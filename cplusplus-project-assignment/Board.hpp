#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"
#include "MagicBitboard.hpp"

#include <bitset>
#include <algorithm>
#include <vector>
#include <optional>




class Board {
public:
    using Optional = std::optional<Board>;
    
    using MoveVec = std::vector<Move>;
    using Bitboard = std::bitset<64>;
    using Turn = PieceColor;

    Board();
    void startBoard();
    void setPiece(const Square& square, const Piece::Optional& piece);
    Piece::Optional piece(const Square& square) const;
    void setTurn(PieceColor turn);
    PieceColor turn() const;
    void setCastlingRights(CastlingRights cr);
    CastlingRights castlingRights() const;
    void setEnPassantSquare(const Square::Optional& square);
    Square::Optional enPassantSquare() const;
    Bitboard& getBitboard(Piece piece);
    Bitboard occupied() const;
    Bitboard occupied(PieceColor color) const;

    Bitboard firstpawnPushMoves(PieceColor color) const;
    Bitboard secondpawnPushMoves(PieceColor color) const;
    Bitboard opponentCanCapture() const;
 

    std::vector<int> get_Indeces(Bitboard bitmap) const ;
    void makeMove(const Move& move);

    void storeAllBoars(std::vector<Bitboard>& boars);
    void restoreAllBoars(const std::vector<Bitboard>& boars);
    
    void pseudoLegalMoves(MoveVec& moves) const;
    void pseudoLegalMovesFrom(const Square& from, MoveVec& moves) const;

   
    void basicPseudoLegalMoves(MoveVec& moves, PieceColor& turn) const;
    
    void addCastelingMoves(MoveVec& moves,Bitboard& blockers) const;
    void opponentCanCaptureMoves(MoveVec& moves) const;
    void addAttacks(MoveVec& moves, Bitboard& attacks, const Square& from) const;

    bool isCheck() const;

    
private:
    Bitboard BPawns, WPawns, BKings, WKings, BQueens, WQueens, BBishops, WBishops, BKnights, WKnights, BRooks, WRooks,Enpassant;
    Turn m_turn;
    CastlingRights m_castlingRights;
    MagicBitboard mb;
};


std::ostream& operator<<(std::ostream& os, const Board& board);

#endif
