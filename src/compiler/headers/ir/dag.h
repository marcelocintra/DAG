#ifndef DAG_H
#define DAG_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "ir/instruction.h"
#include "cfg/basicBlock.h"

using namespace std;

// Define the information to be stored at each node
class Node {
public:
	Node(Operator lbl) : label(lbl) { }

	vector<Node *> getPredecessors() {
		return predecessors;
	}
	vector<Node *> getSuccessors() {
		return successors;
	}
	void addPredecessor(Node * pred) {
		predecessors.push_back(pred);
	}
	void addSuccessor(Node * succ) {
		successors.push_back(succ);
	}

	virtual int hashCode () const = 0;

	// get the DAG label: constant/localVariable for leaf nodes
	//                    operator for interior nodes
    Operator getLabel () { return label; }

	virtual void print() const = 0;

protected:
	vector<Node *>  predecessors;
	vector<Node *>  successors;

	// A DAG label is: a constant/localVariable for leaf nodes
	//                 an operator for interior nodes
	Operator          label;

	void printLabel() const;
};

// Defines a DAG non-leaf Node
class OperatorNode: public Node {
private:
	vector<LocalVariable *> identifierList;

public:
	OperatorNode(Operator lbl, Node *left, Node *right): Node(lbl) {
		addSuccessor(left);
		addSuccessor(right);
		left->addPredecessor(this);
		right->addPredecessor(this);
	}

	virtual void print() const;

	virtual int hashCode() const;

	bool operator==(const OperatorNode &other) const;

	void addIdentifier(LocalVariable *localVariable) {
		identifierList.push_back(localVariable);
	}

	void removeIdentifier (LocalVariable *localVariable);
};

// Defines a DAG leaf Node
class LeafNode: public Node {
private:
	Instruction *leaf;

public:
	LeafNode(Instruction *lf): Node(lf->getInstructionID()), leaf(lf) {
	}

	LeafNode(Node *parent, Instruction *lf): Node(lf->getInstructionID()), leaf(lf) {
		addPredecessor(parent);
	}

	virtual void print() const{
		cout << "Leaf Node @" << this << "[";
		leaf->print();
		cout << "]";
	}

	virtual int hashCode () const;

	bool operator==(const LeafNode &other) const {
		return (leaf == other.leaf);
	}

	Instruction * getLeaf() { return leaf; }
};

// Defines a Direct Acyclic Graph (DAG)
class DAG {
public:
	template<typename T>
	using NodeMap = unordered_map<Node*, T>;
	using DAGNodes = vector<Node*>;
	using IdentifierMap = unordered_map<LocalVariable*, Node *>;

	DAG (BasicBlock *basicBlock);
	DAG (): DAG(0) { }

	~DAG();

	// Add a three address instruction to the DAG
	void addThreeAddressInstruction(Instruction *instruction);

	// Get the vector containing the DAG nodes
	const vector<Node*>& getDAGNodes() const;

	void print() const;

private:
	DAGNodes       *operatorArray;  // contains a list of DAG nodes where the operator occurs
	IdentifierMap  identifierMapper; // maps an identifier (LocalVariable) to latest Node producing it
	DAGNodes       vertices;         // contains all vertices of the DAG

	void addEdge(Node* u, Node* v);
	NodeMap<int> indegrees() const;
	int indegree(Node*) const;
	Node * addNode(BinaryInstruction *i);
	Node * addNode(Operator op, Instruction *left, Instruction *right);
	Node * searchNode(Operator op, Node *left, Node *right);
	Node * addNode(Move *i);
	Node * addNode(Constant *c);
	Node * addNode(LocalVariable *variable);
	Node * addOperatorNode(Instruction *instruction);
	Node * addLeafNode(Instruction *instruction);
};


#endif
