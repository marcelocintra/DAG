#ifndef DAG_H
#define DAG_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include "ir/instruction.h"

using namespace std;

// Define the information to be stored at each node
class Vertex {
 public:
  void print() { cout << "Abstract Vertex"; }
  vector<Vertex *> getPredecessors() { return predecessors; }
  vector<Vertex *> getSuccessors() { return successors; }
  void addPredecessor(Vertex * pred) { predecessors.push_back(pred); }
  void addSuccessor(Vertex * succ) { successors.push_back(succ); }

 protected:
  vector<Vertex *> predecessors;
  vector<Vertex *> successors;
};

class OperatorVertex : public Vertex
{
 private:
  Operator             op;

  vector<Instruction *> identifierList;

 public :
  OperatorVertex() { }
  void print() { cout << "OperatorVertex:" << op; }
  
};

class LeafVertex : public Vertex
{
 private:
  Instruction  *leaf;

 public:
  LeafVertex() { }
  void print() { cout << "DAG Leaf:" << leaf; }
};

// Direct Acyclic Graph (DAG) definition
class DAG
{
public:
	template<typename T>
	using VertexMap     = unordered_map<Vertex*, T>;
    using AdjacencyList = vector<Vertex*>;

    void addEdge(Vertex* u, Vertex* v);

    VertexMap<int> indegrees() const;
    int indegree(Vertex*) const;

    const AdjacencyList& adjacencyList() const;
private:
    AdjacencyList vertices;
};


#endif
