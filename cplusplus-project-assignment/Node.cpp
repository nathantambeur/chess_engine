#include "Node.hpp"

#include <iostream>
#include <ostream>

Node::Node(Move& move,const int& score, const int& hierarchy, const int& alpha, const int& beta)
{
    ref_move = Move(*Square::fromIndex(move.from().index()),*Square::fromIndex(move.to().index()),move.promotion());
    ref_score = score;
    ref_hierarchy = hierarchy;
    ref_alpha = alpha;
    ref_beta = beta;
}

void Node::addChild(Node& child) {
    ref_children.push_back(child);
}

std::vector<Node>& Node::getChildren(){
    return ref_children;
}

int Node::getScore(){
    return ref_score;
}

int Node::getHierarchy(){
    return ref_hierarchy;
}
int Node::getAlpha()    {
    return ref_alpha;
}
int Node::getBeta()    {
    return ref_beta;
}
void Node::setAlpha(int& alpha)    {
    ref_alpha = alpha;
}
void Node::setBeta(int& beta)    {
    ref_beta = beta;
}
void Node::setScore(int& score)    {
    ref_score = score;
}
void Node::setAsbestChild(Node child) {
    ref_children.push_back(child);
}
Move& Node::getMove() {
    return ref_move;
}
Move& Node::getBestChildMove() {
    return ref_children[0].getMove();
}

void Node::swap(Node &other) {
    std::swap(ref_move, other.ref_move);
    std::swap(ref_score, other.ref_score);
    std::swap(ref_hierarchy, other.ref_hierarchy);
    std::swap(ref_alpha, other.ref_alpha);
    std::swap(ref_beta, other.ref_beta);
    std::swap(ref_children, other.ref_children);
}
//displays the tree stucture of the node starting from the root
std::ostream& operator<<(std::ostream& os, const Node& move) {
    if (move.ref_children.empty()) {
        return os; //<< "Leaf node: " << move.ref_score << std::endl;
    }
    if (move.ref_hierarchy == 0) {
        os << "Root node: " << std::endl;
    }
    else {
        os << "Node at hierarchy: " << move.ref_hierarchy << std::endl;
        os << move.ref_move << ", With score: " << move.ref_score << std::endl;
    }


    
    os << "CHILDEREN: " ;
    for (const Node& child : move.ref_children) {
         os << "[ "<< child.ref_move << ", With score: " << child.ref_score << "], ";
    }
    os << std::endl;
    for (const Node& child : move.ref_children) {
        os << "CHILDEREN  " << child.ref_move << " : ";
        for (const Node& childr : child.ref_children) {
         os << "[ "<< childr.ref_move << ", With score: " << childr.ref_score << "], ";
         }
    }
    return os;
   
}
namespace std {
    void swap(Node &a, Node &b) {
        a.swap(b);
    }
}
//only use for root node from the bottem up to the root
std::vector<Move> Node::getBestMoveSequence(bool isMaximizingPlayer) {
  std::vector<Move> sequence;
  Node current = *this;
  int i = 0;
  if (!isMaximizingPlayer){
        i = 1;
  }
  while (!current.getChildren().empty() ) {
    Node best_child = current.getChildren()[0];
    if (i%2 == 0){
         // Choose the child with the highest score
    for (const Node& child : current.getChildren()) {
      if (child.ref_score > best_child.ref_score) {
        best_child = child;
      }

    }
    // Add the move to the sequence and move to the best child
    if (isMaximizingPlayer){
        sequence.push_back(best_child.getMove());
    }
    
    }
    else {
        // Choose the child with the lowest score
    
    for (const Node& child : current.getChildren()) {
      if (child.ref_score < best_child.ref_score) {
        best_child = child;
      }
    }
    if (!isMaximizingPlayer){
        sequence.push_back(best_child.getMove());

    }
    }
    current = best_child;
    i++;
   }
    return sequence;
}
std::vector<Move> Node::getBestMoveSequence(bool isMaximizingPlayer, Move avoid_move) {
  std::vector<Move> sequence;
  Node current = *this;
  int i = 0;
  if (!isMaximizingPlayer){
        i = 1;
  }
  while (!current.getChildren().empty() ) {
    Node best_child = current.getChildren()[0];
    if (i==0 || i==1){ //first moves getting made 
        if (best_child.getMove() == avoid_move){
            continue;
        }
    }
    if (i%2 == 0){
         // Choose the child with the highest score
    for (const Node& child : current.getChildren()) {
      if (child.ref_score > best_child.ref_score) {
        best_child = child;
      }

    }
    // Add the move to the sequence and move to the best child
    if (isMaximizingPlayer){
        sequence.push_back(best_child.getMove());
    }
    
    }
    else {
        // Choose the child with the lowest score
    
    for (const Node& child : current.getChildren()) {
      if (child.ref_score < best_child.ref_score) {
        best_child = child;
      }
    }
    if (!isMaximizingPlayer){
        sequence.push_back(best_child.getMove());

    }
    }
    current = best_child;
    i++;
   }
    return sequence;
}
std::vector<Move> Node::getBestMoveSequenceWITHOPPONENT(bool isMaximizingPlayer) {
  std::vector<Move> sequence;
  Node current = *this;
  int i = 0;
  if (!isMaximizingPlayer){
        i = 1;
  }
  while (!current.getChildren().empty() ) {
    Node best_child = current.getChildren()[0];
    if (i%2 == 0){
         // Choose the child with the highest score
    for (const Node& child : current.getChildren()) {
      if (child.ref_score > best_child.ref_score) {
        best_child = child;
      }

    }
    // Add the move to the sequence and move to the best child
    
    sequence.push_back(best_child.getMove());
    
    
    }
    else {
        // Choose the child with the lowest score
    
    for (const Node& child : current.getChildren()) {
      if (child.ref_score < best_child.ref_score) {
        best_child = child;
      }
    }
   
    sequence.push_back(best_child.getMove());

    
    }
    current = best_child;
    i++;
   }
    return sequence;
}