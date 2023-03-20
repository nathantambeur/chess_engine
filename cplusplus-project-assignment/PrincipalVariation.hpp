#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
#define CHESS_ENGINE_PRINCIPALVARIATION_HPP

#include "Move.hpp"
#include "Piece.hpp"
#include "Board.hpp"


#include <iosfwd>
#include <cstddef>

//could also try with something that hashes so its easier to update and remove elements but fine for now 
class PrincipalVariation {
public:

    using MoveVec = std::vector<Move>;
    using MoveIter = MoveVec::const_iterator;
    PrincipalVariation(bool isMate , int score , MoveVec moves);
    bool isMate() const;
    int score() const;
    std::size_t length() const;
    MoveIter begin() const;
    MoveIter end() const;
private:
    bool m_isMate;
    int m_score;
    MoveVec m_moves;
};


std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);


#endif
