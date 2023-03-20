#include "PrincipalVariation.hpp"

#include <ostream>

PrincipalVariation::PrincipalVariation(bool isMate , int score , MoveVec moves){
    m_isMate= isMate;
    m_score = score;
    m_moves = moves;
}
bool PrincipalVariation::isMate() const {
    return m_isMate;
}

int PrincipalVariation::score() const {
    return m_score;
}

std::size_t PrincipalVariation::length() const {
    return m_moves.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return m_moves.begin();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
    return m_moves.end();
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
    if (pv.isMate()) {
        os << "mate in " << pv.length();
    } else {
        os << "score " << pv.score();
    }
    for (const auto& move : pv) {
        os << ' ' << move;
    }
    return os;
}
