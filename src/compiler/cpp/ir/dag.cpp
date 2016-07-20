#include "ir/dag.h"
#include <string.h>
#include <iostream>
#include <assert.h>

using namespace std;

void Node::printLabel() const {
	switch (label) {
	case CONSTANT:
		cout << "CONSTANT";
		break;
	case LOCALVARIABLE:
		cout << "LOCALVARIABLE";
		break;
	case MUL:
		cout << "MUL";
		break;
	case ADD:
		cout << "ADD";
		break;
	case MOVE:
		cout << "MOVE";
		break;
	default:
		cout << "INVALID";
		break;
	}
}

int OperatorNode::hashCode() const {
	int hash = label;
	for(Node *i : successors) {
		hash ^= ((hash >> 4) ^ (i->hashCode() << 8));
	}
	return hash;
}

void OperatorNode::print() const{
	cout << "OperatorNode @" << this << ": ";
	cout << "@"<< successors[0];//->print();
	cout << " ";
	printLabel();
	cout << " ";
	cout << "@" << successors[1]; //->print();
	cout << " identifiers: ";
	for (unsigned i = 0; i < identifierList.size(); i++) {
		identifierList[i]->print();
		cout << " ";
	}
}

bool OperatorNode::operator==(const OperatorNode &other) const {
	if (label == other.label) {

		if (successors.size() != other.successors.size())
			return false;

		for (Node *i : successors) {
			if (find(other.successors.begin(), other.successors.end(), i)
					== other.successors.end()) {
				return false;
			}
		}

		if (predecessors.size() != other.predecessors.size())
			return false;

		for (Node *i : predecessors) {
			if (find(other.predecessors.begin(), other.predecessors.end(), i)
					== other.predecessors.end()) {
				return false;
			}
		}

		return true;
	}
	return false;
}

void OperatorNode::removeIdentifier (LocalVariable *localVariable) {
	vector<LocalVariable *>::iterator position = std::find(identifierList.begin(), identifierList.end(), localVariable);
	if (position != identifierList.end())
		identifierList.erase(position);
}


int LeafNode::hashCode() const {
	return leaf->hashCode();
}

// Add edge u->v to the DAG
void DAG::addEdge(Node* u, Node* v) {
	vertices.push_back(v);
	vertices.push_back(u);
	u->addSuccessor(v);
	v->addPredecessor(u);
}

// This we can use to compute the Topological Sort.
DAG::NodeMap<int> DAG::indegrees() const {
	NodeMap<int> indegrees;

	for (Node * v : vertices) {
		indegrees[v]; // initialize entry
		for (Node* neighbour : v->getSuccessors()) {
			++indegrees[neighbour];
		}
	}
	return indegrees;
}

// DAG: Constructor that builds a DAG for the
// basic block passed as parameter
DAG::DAG(BasicBlock *basicBlock) {

	operatorArray = (DAGNodes *) new DAGNodes[NUMBER_OF_OPERATORS];

	if (basicBlock) {
		Instruction *instruction = basicBlock->getFirst();

		while (instruction != 0) {
			this->addThreeAddressInstruction(instruction);
			instruction = instruction->getNext();
		}
	}
}

void DAG::addThreeAddressInstruction(Instruction *instruction) {
	if (instruction == 0) {
		return;
	}

	switch (instruction->getInstructionID()) {

	case LOCALVARIABLE:
		addNode((LocalVariable *) instruction);
		break;

	case MOVE:
		addNode((Move *) instruction);
		break;

	// binary instructions should always produce a value
    // adding assert to make sure the three-address input is
    // in a valid state
	case MUL:
	case ADD:
	case CONSTANT:
		assert(false);
		break;

	default:
		;
		// do nothing for the other cases
	}
}

Node * DAG::addOperatorNode(Instruction *instruction) {
	if (instruction == 0) {
		return 0;
	}

	Node *operatorNode = 0;

	switch (instruction->getInstructionID()) {

	case MUL:
	case ADD:
		operatorNode = addNode((BinaryInstruction *) instruction);
		break;

	case MOVE:
		operatorNode = addNode((Move *) instruction);
		break;

	default:
		assert(false && "Not an operator Node");
		break;

	}
	return operatorNode;
}

Node * DAG::addLeafNode(Instruction *instruction) {
	if (instruction == 0) {
		return 0;
	}

	Node *leafNode = 0;

	switch (instruction->getInstructionID()) {

	case LOCALVARIABLE:
		leafNode = addNode((LocalVariable *) instruction);
		break;

	case CONSTANT:
		leafNode = addNode((Constant *) instruction);
		break;

	default:
		assert(false && "Not a leaf Node");
	}

	return leafNode;
}


Node * DAG::addNode(BinaryInstruction *i) {
	return addNode(i->getInstructionID(), i->getFirstOperand(), i->getSecondOperand());
}

Node * DAG::addNode(Operator op, Instruction *left, Instruction *right) {
	Node *leftNode = addLeafNode(left);
	Node *rightNode = addLeafNode(right);

	// Search for an exiting inner node for operation  'left op right'
	Node * operatorNode = searchNode(op, leftNode, rightNode);

	if (operatorNode == 0){
		// if a operator node does not exit, create one and set the childs
		operatorNode = new OperatorNode(op, leftNode, rightNode);
		vertices.push_back(operatorNode);
		operatorArray[op].push_back(operatorNode);
	}

	return operatorNode;
}

Node * DAG::searchNode(Operator op, Node *left, Node *right) {
	// get  predecessor nodes for left Node;
	vector<Node *>  lpredecessors = left->getPredecessors();

	// get  predecessor nodes for right Node;
	vector<Node *>  rpredecessors = right->getPredecessors();

	// search a common predecessor node
	for (unsigned i = 0; i < lpredecessors.size(); i++) {
		Node * lpredecessor = lpredecessors[i];
		if (lpredecessor->getLabel() == op) {
			for (unsigned j = 0; j < rpredecessors.size(); j++) {
				Node *rpredecessor = rpredecessors[i];
				if (lpredecessor == rpredecessor &&
						(op == MUL || op == ADD || // for add and multiply does not matter the order
					    (i == 0 && j == 1))) {
					// node (left op right) already exists, return it
					return lpredecessor;
				}
			}
		}
	}
	return 0;
}

template<typename Base, typename T>
inline bool instanceof(const T *ptr)
{
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

Node * DAG::addNode(Move *i) {
	Instruction * rightValue = i->getRightValue();
	Node *resultNode = 0;

	// clear Move destination variable (localVariable) from previous DAG operator node
	Node *previousIdMapping = identifierMapper[i->getVariable()];
	if ( (previousIdMapping != 0) && instanceof<OperatorNode>(previousIdMapping)) {
		((OperatorNode *) previousIdMapping)->removeIdentifier(i->getVariable());
	}

	switch (rightValue->getInstructionID()) {

		case LOCALVARIABLE:
			resultNode = addNode((LocalVariable *) rightValue);
			identifierMapper[i->getVariable()] = resultNode;
			break;

		case CONSTANT: {
			Node *rightValueNode = addNode((Constant *) rightValue);
			Node *idNode = new LeafNode(i->getVariable());
			identifierMapper[i->getVariable()] = idNode;
			resultNode = new OperatorNode(i->getInstructionID(), idNode, rightValueNode);
			identifierMapper[i->getVariable()] = resultNode;
			operatorArray[i->getInstructionID()].push_back(resultNode);
			vertices.push_back(resultNode);
		}
			break;

		case MOVE:
			assert(false && "Invalid right value for Move instruction");
			break;

		// binary instructions should always produce a value
	    // adding assert to make sure the three-address input is
	    // in a valid state
		case MUL:
		case ADD:
			resultNode = addNode((BinaryInstruction *) rightValue);
			identifierMapper[i->getVariable()] = resultNode;
			((OperatorNode *) resultNode)->addIdentifier(i->getVariable());
			break;

		default:
			;
			// do nothing for the other cases
	}
	return resultNode;
}

Node * DAG::addNode(Constant *c) {
	DAGNodes constantNodes = operatorArray[c->getInstructionID()];
	for (unsigned i = 0; i < constantNodes.size(); ++i) {
		Constant *constantI = (Constant*) (((LeafNode *)constantNodes[i])->getLeaf());
		if (c == constantI) {
			return constantNodes[i];
		}
	}
	Node *leafNode = new LeafNode(c);
	operatorArray[c->getInstructionID()].push_back(leafNode);
	vertices.push_back(leafNode);
	return leafNode;
}

Node * DAG::addNode(LocalVariable *variable) {
	Node *leafNode = identifierMapper[variable];

	if (leafNode == 0) {
		leafNode = new LeafNode(variable);
		operatorArray[variable->getInstructionID()].push_back(leafNode);
		vertices.push_back(leafNode);
	}
	return leafNode;
}

const DAG::DAGNodes& DAG::getDAGNodes() const {
	return vertices;
}

DAG::~DAG() {
	for (auto *node : vertices) {
		delete node;
	}
}

void DAG::print() const {
	cout << endl << endl << "DAG: " << endl;
	for(Node *node : vertices) {
			node->print();
			cout << "\n \n";
	}
}
