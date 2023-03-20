#ifndef CHESS_ENGINE_MAGICBITBOARD_HPP
#define CHESS_ENGINE_MAGICBITBOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"

#include <bitset>

#include <optional>
#include <iosfwd>
#include <vector>


class MagicBitboard {
public:
  
    using Bitboard = std::bitset<64>;
    using Turn = PieceColor;

    MagicBitboard();
    void regenerateMagicBitboard();
    void read_in_data();
    Bitboard Bmask_square(const Square& square) const;
    Bitboard Rmask_square(const Square& square) const;
    Bitboard Rattack_square(const Square& square, Bitboard occupied) const;
    Bitboard Battack_square(const Square& square, Bitboard occupied) const;
    Bitboard Bmoves(const Square& square, const Bitboard& Occ ) const;
    Bitboard Rmoves(const Square& square, const Bitboard& Occ ) const;
    Bitboard Nmoves(const Square& square) const;
    Bitboard Kmoves(const Square& square) const;
    Bitboard get_Bmagic(const Square& square) const;
    Bitboard get_Rmagic(const Square& square) const;
    
    Bitboard get_Rmask(const Square& square) const;
    Bitboard get_Bmask(const Square& square) const;

    
    std::vector<Bitboard> possibleOccupied(const Bitboard& mask) const;
    Bitboard add_1_to_indeces(const Bitboard b,const std::vector<int> indeces,const int index ) const;
    std::vector<int> get_indeces(const Bitboard b) const;

    void printBitboard(const Bitboard& b) const;

    
    
private:
    
    Bitboard RmoveDB[64][4096] ;
    Bitboard BmoveDB[64][512] ; 
    Bitboard KmoveDB[64];
    Bitboard NmoveDB[64]; 
    Bitboard Rmask [64] ;
    Bitboard Bmask [64] ;   
    Bitboard Rmagics[64] ;
    Bitboard Bmagics[64] ;
    
};

#endif
