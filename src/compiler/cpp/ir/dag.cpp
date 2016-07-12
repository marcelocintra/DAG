#include "ir/dag.h"
#include <string.h>
#include <iostream>

using namespace std;

void DAG::addEdge(Vertex* u, Vertex* v)
{

    vertices.push_back(v);  // Add edge u->v
    vertices.push_back(u);
    u->addSuccessor(v);
    v->addPredecessor(u);
}

DAG::VertexMap<int> DAG::indegrees() const
{
    VertexMap<int> indegrees;

    for (Vertex * v : vertices)
    {
        indegrees[v]; // initialize entry
        for (Vertex* neighbour : v->getSuccessors())
        {
            ++indegrees[neighbour];
        }
    }
    return indegrees;
}

const DAG::AdjacencyList& DAG::adjacencyList() const
{
    return vertices;
}
