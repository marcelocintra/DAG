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
	void print() {
		cout << "Abstract Vertex";
	}
	vector<Vertex *> getPredecessors() {
		return predecessors;
	}
	vector<Vertex *> getSuccessors() {
		return successors;
	}
	void addPredecessor(Vertex * pred) {
		predecessors.push_back(pred);
	}
	void addSuccessor(Vertex * succ) {
		successors.push_back(succ);
	}

	virtual int hashCode () const = 0;

protected:
	vector<Vertex *> predecessors;
	vector<Vertex *> successors;
};

// Defines a DAG non-leaf vertex
class OperatorVertex: public Vertex {
private:
	Operator op;
	vector<Instruction *> identifierList;

public:
	OperatorVertex() {
	}
	void print() {
		cout << "OperatorVertex:" << op;
	}
	virtual int hashCode () const;
};

// Defines a DAG leaf vertex
class LeafVertex: public Vertex {
private:
	Instruction *leaf;

public:
	LeafVertex() {
	}
	void print() {
		cout << "DAG Leaf: ";
		leaf->print();
	}
	virtual int hashCode () const;
};

namespace std {

  template <>
  struct hash<Vertex>
  {
    std::size_t operator()(const Vertex& k) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // uses hashCode()

      return ((hash<int>()(k.hashCode())));
    }
  };

}

// Defines a Direct Acyclic Graph (DAG) definition
class DAG {
public:
	template<typename T>
	using VertexMap = unordered_map<Vertex*, T>;
	using AdjacencyList = vector<Vertex*>;
	using TemporaryMap = unordered_map<LocalVariable*, Vertex *>;

	void addEdge(Vertex* u, Vertex* v);
	VertexMap<int> indegrees() const;
	int indegree(Vertex*) const;
	const AdjacencyList& adjacencyList() const;

private:
	AdjacencyList vertices;
};


#endif
