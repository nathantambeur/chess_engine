#ifndef CHESS_ENGINE_MOVE_HPP
#define CHESS_ENGINE_MOVE_HPP

#include "Square.hpp"
#include "Piece.hpp"


#include <iosfwd>
#include <optional>
#include <string>
#include <vector>



class Move {
public:

    using Optional = std::optional<Move>;
    Move()=default;
    Move(const Square& from, const Square& to,
         const std::optional<PieceType>& promotion = std::nullopt);

    Move(Square&&, Square&&, std::optional<PieceType>&& = std::nullopt);
    static Optional fromUci(const std::string& uci);
    void swap(Move &other);
    Square from() const;
    Square to() const;
    int getIndexFrom() const;
    int getIndexTo() const;
    std::optional<PieceType> promotion() const;
  
private:
    Square ref_from;
    Square ref_to;
    //to pass thest reference needs to be removed does this has any impact on performance?
    std::optional<PieceType> ref_promotion;
};    


std::ostream& operator<<(std::ostream& os, const Move& move);


bool operator<(const Move& lhs, const Move& rhs);
bool operator==(const Move& lhs, const Move& rhs);




#endif
