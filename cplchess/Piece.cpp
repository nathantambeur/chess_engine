#include "Piece.hpp"

#include <ostream>

Piece::Piece(PieceColor color, PieceType type){
    m_color = color;
    m_type = type;
}

Piece::Optional Piece::fromSymbol(char symbol) {
    switch (symbol)
    {
    case 'P':
        return Piece(PieceColor::White,PieceType::Pawn);
    case 'N':
        return Piece(PieceColor::White,PieceType::Knight);
    case 'B':
        return Piece(PieceColor::White,PieceType::Bishop);
    case 'R':
        return Piece(PieceColor::White,PieceType::Rook);
    case 'K':
        return Piece(PieceColor::White,PieceType::King);
    case 'Q':
        return Piece(PieceColor::White,PieceType::Queen);
    case 'p':
        return Piece(PieceColor::Black,PieceType::Pawn);
    case 'n':
        return Piece(PieceColor::Black,PieceType::Knight);
    case 'b':
        return Piece(PieceColor::Black,PieceType::Bishop); 
    case 'r':
        return Piece(PieceColor::Black,PieceType::Rook);
    case 'k':
        return Piece(PieceColor::Black,PieceType::King);
    case 'q':
        return Piece(PieceColor::Black,PieceType::Queen);
    default:
        return std::nullopt;
        break;
    }
    
}

PieceColor Piece::color() const {
    return m_color;
}

PieceType Piece::type() const {
    return m_type;
}

bool operator==(const Piece& lhs, const Piece& rhs) {
    return lhs.color() == rhs.color() && lhs.type() == rhs.type();
}

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    switch (piece.color())
    {
    case PieceColor::White:
        switch (piece.type())
        {
        case PieceType::Pawn:
            os << 'P';
            break;
        case PieceType::Knight:
            os << 'N';
            break;
        case PieceType::Bishop:
            os << 'B';
            break;
        case PieceType::Rook:
            os << 'R';
            break;
        case PieceType::King:
            os << 'K';
            break;
        case PieceType::Queen:
            os << 'Q';
            break;
        }
        break;
    case PieceColor::Black:
        switch (piece.type())
        {
        case PieceType::Pawn:
            os << 'p';
            break;
        case PieceType::Knight:
            os << 'n';
            break;
        case PieceType::Bishop:
            os << 'b';
            break;
        case PieceType::Rook:
            os << 'r';
            break;
        case PieceType::King:
            os << 'k';
            break;
        case PieceType::Queen:
            os << 'q';
            break;
        }
        break;
    }
    return os;
}

PieceColor operator!(PieceColor color) {
    switch (color)
    {
        case PieceColor::White:
            return PieceColor::Black;
        case PieceColor::Black:
            return PieceColor::White;
    }
    return PieceColor::White;
}

bool operator<(const Piece& lhs,const Piece& rhs)
{
    if (lhs.type() < rhs.type()){
        return true; }
    else if (lhs.type() == rhs.type()){
        return lhs.color() < rhs.color();
    }
    else{
        return false;
    }
   
}
