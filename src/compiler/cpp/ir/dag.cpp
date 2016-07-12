#include "ir/dag.h"
#include <string.h>
#include <iostream>

using namespace std;

int OperatorVertex::hashCode() const {
	int hash = op;
	for(Vertex *i : successors) {
		hash ^= (i->hashCode() << 8);
	}
	return hash;
}

int LeafVertex::hashCode() const {
	return leaf->hashCode();
}

// Add edge u->v to the DAG
void DAG::addEdge(Vertex* u, Vertex* v) {
	vertices.push_back(v);
	vertices.push_back(u);
	u->addSuccessor(v);
	v->addPredecessor(u);
}

DAG::VertexMap<int> DAG::indegrees() const {
	VertexMap<int> indegrees;

	for (Vertex * v : vertices) {
		indegrees[v]; // initialize entry
		for (Vertex* neighbour : v->getSuccessors()) {
			++indegrees[neighbour];
		}
	}
	return indegrees;
}

const DAG::AdjacencyList& DAG::adjacencyList() const {
	return vertices;
}
