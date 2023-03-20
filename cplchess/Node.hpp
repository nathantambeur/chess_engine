#ifndef CHESS_ENGINE_NODE_HPP
#define CHESS_ENGINE_NODE_HPP

#include "Square.hpp"
#include "Piece.hpp"
#include "Move.hpp"
#include "Board.hpp"

#include <iosfwd>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>

class Node {
public:

    using Optional = std::optional<Node>;
    Node () = default;
    Node(Move& move,const int& score,const int& hierarchy, const int& alpha,const int& beta);
    void addChild(Node& child);
    std::vector<Node>& getChildren();
    std::vector<Move> getBestMoveSequence(bool isMaximizingPlayer);
    std::vector<Move> getBestMoveSequence(bool isMaximizingPlayer, Move avoid_move);
    int getScore();
    int getHierarchy();
    int getAlpha();
    int getBeta();
    void setAlpha(int& alpha);
    void setBeta(int& beta);
    void setScore(int& score);
    void setAsbestChild(Node child);
    Move& getBestChildMove();
    std::vector<Move> getBestMoveSequenceWITHOPPONENT(bool isMaximizingPlayer);
    void swap(Node &other);
    Move& getMove();

    Move ref_move = Move(Square::A1,Square::A1);
    int ref_score = 0;
    int ref_hierarchy= 0;
    int ref_alpha   = 0;
    int ref_beta   = 0;
    std::vector<Node> ref_children;
    
    
};  

std::ostream& operator<<(std::ostream& os, const Node& move);
#endif