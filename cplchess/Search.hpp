#ifndef CHESS_ENGINE_SEARCH_HPP
#define CHESS_ENGINE_SEARCH_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "Board.hpp"
#include "CastlingRights.hpp"
#include "Engine.hpp" 
#include "Node.hpp"

#include <bitset>
#include <optional>
#include <iosfwd>
#include <vector>

class Search :public Engine{
public:
    using Bitboard = std::bitset<64>;
    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;
    using Turn = PieceColor;


    Search();
    int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, Node& moves);

    std::string name() const override;
    std::string version() const override;
    std::string author() const override;

    void newGame() override;
    PrincipalVariation pv(
        const Board& board,
        const TimeInfo::Optional& timeInfo = std::nullopt
    ) override;

    std::optional<HashInfo> hashInfo() const override;
    void setHashSize(std::size_t size) override;
    int SearchAllCaptures(int depth,int alpha, int beta, bool isMaximizingPlayer);
    void OrderMoves(MoveVec& moves);
    int moveScore(const Move& move);
    int evaluate() const;
    bool moveScoreComparator(const Move& a, const Move& b);
    int negamax(int depth, int alpha, int beta, Node& node);
   int quiesce(int alpha, int beta, bool maximizingPlayer,int iteration);
    unsigned long long calculateHashCode();
    void initializeZobristHashTable();

private:
    Board m_board;
    Bitboard m_opponent_capture;
};

#endif
