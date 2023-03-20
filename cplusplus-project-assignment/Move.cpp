#include "Move.hpp"
#include "Piece.hpp"
#include "Square.hpp"

#include <iostream>
#include <ostream>




Move::Move(Square&& from, Square&& to,
           std::optional<PieceType>&& promotion)
    : ref_from(std::move(from)), ref_to(std::move(to)), ref_promotion(std::move(promotion)) {
        
}

Move::Move(const Square& from, const Square& to,
           const std::optional<PieceType>& promotion)
    : ref_from(from), ref_to(to), ref_promotion(promotion) {
    }

Move::Optional Move::fromUci(const std::string& uci) {
    if (uci.length() == 4 || uci.length() == 5){

        const Square::Optional from = Square::fromName(uci.substr(0, 2));
        const Square::Optional to = Square::fromName(uci.substr(2, 2));
        
       
       if (from && to) {
            if (uci.length() == 5) {
                const Piece::Optional promotion = Piece::fromSymbol(uci[4]);
                if (promotion) {
                    return Move(*from, *to, (*promotion).type());
                }
                else { return std::nullopt;
                } 
            }
            else {
                return Move(*from, *to);
            }
       }   
       else {
           return std::nullopt;
       }

    }
    else {
        return std::nullopt;
    }
}


Square Move::from() const {
    return ref_from;
}

Square Move::to() const {
    return ref_to;
}

std::optional<PieceType> Move::promotion() const {
    return ref_promotion;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.from() << move.to();
    if (move.promotion().has_value()) {
        //black so lower case
        os << Piece(PieceColor::Black,move.promotion().value());
    }
    return os;
}


bool operator<(const Move& lhs, const Move& rhs) {
    if (lhs.from() < rhs.from()) {
        return true;
    }
    else if (lhs.from() == rhs.from()) {
        if (lhs.to() < rhs.to()) {
            return true;
        }
        else if (lhs.to() == rhs.to()) {
            if (lhs.promotion() < rhs.promotion()) {
                return true;
            }
        }
    }
    return false;
}

bool operator==(const Move& lhs, const Move& rhs) {
    return lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion() == rhs.promotion();
}

void Move::swap(Move &other) {
    ref_from.swap(other.ref_from);
    ref_to.swap(other.ref_to);
    if (other.ref_promotion.has_value()) {
        ref_promotion = *other.ref_promotion;
    }
    else {
        ref_promotion = std::nullopt;
    }
    
}

int Move::getIndexFrom() const {
    return (int) ref_from.index();
}
int Move::getIndexTo() const {
    return (int) ref_to.index();
}
namespace std {
    void swap(Move &a, Move &b) {
        a.swap(b);
    }
}

