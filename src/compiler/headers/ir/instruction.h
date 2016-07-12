#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <iostream>

using namespace std;

// These are the operators for our instruction set
// I'm only defining what I need for the code exercise
typedef enum {
	ADD, MULTIPLY, ASSIGNMENT, PRINT, CALL, RETURN
} Operator;

// define the data types
typedef enum {
	INT, FLOAT, DOUBLE, POINTER, UNKOWN
} Type;

// This represents a generic value during compilation
// TODO: I may refactor this out since this is static compilation
//  I'll leave it for now.
class Value {
public:
	virtual int valueNumber() {
		return 0;
	}
};

class Integer: public Value {
public:
	Integer(int v) :
			value(v) {
	}

	virtual int valueNumber() {
		return value;
	}

private:
	int value;
};

class InstructionVisitor;
class OperandVisitor;

// Instruction represents a three-address instruction
class Instruction {
protected:
	Value *value;
	Instruction *substitute;
	Instruction *previous;
	Instruction *next;

public:
	Instruction(Value *v, Instruction *p, Instruction *n) :
			value(v), previous(p), next(n), substitute(0) {
	}
	// Instruction(): value (0), previous(0), next(0), substitute(0) {}

	Value *getValue() {
		return value;
	}

	// Check if the instruction has a substitute */
	Instruction * resolve();

	// Replace the value of this instruction with other
	void redirectTo(Instruction *other) {
		substitute = other;
	}
	void setNext(Instruction *n) {
		next = n;
	}

	// accessors for next and previous
	Instruction * getNext() {
		return next;
	}
	void setPrevious(Instruction *p) {
		previous = p;
	}
	Instruction * getPrevious() {
		return previous;
	}

	Instruction * link(Instruction *b) {
		next = b;
		b->setPrevious(this);
		return b;
	}

	// virtual functions for instruction and operand visitors
	virtual void print() {
	}
	virtual void accept(InstructionVisitor &v) {
	}
	;
	virtual void visitOperands(OperandVisitor &v) {
	}
	;
	virtual int hashCode() const = 0 ;
};

class BinaryInstruction: public Instruction {
protected:
	Instruction *operand0;
	Instruction *operand1;

public:
	BinaryInstruction(Value *value, Instruction *oper0, Instruction *oper1) :
			operand0(oper0), operand1(oper1), Instruction(value, 0, 0) {
	}

	Instruction * getOperand0() {
		return operand0->resolve();
	}

	void setOperand0(Instruction *operand) {
		this->operand0 = operand;
		;
	}

	Instruction * getOperand1() {
		return operand1->resolve();
	}

	void setOperand1(Instruction *operand) {
		this->operand1 = operand;
		;
	}

	void swapOperands() {
		Instruction *temp = operand0;
		operand0 = operand1;
		operand1 = temp;
	}

	virtual void visitOperands(OperandVisitor &v);
	virtual int hashCode() const;
};

// Defining only the two Binary instructions used in the code snippet
class Add: public BinaryInstruction {
public:
	Add(Value *value, Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(value, operand0, operand1) {
	}

	Add(Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(0, operand0, operand1) {
	}

	virtual void accept(InstructionVisitor &v);
	virtual void print() {
		cout << " ADD ";
		operand0->print();
		operand1->print();
	}
};

class Mul: public BinaryInstruction {
public:
	Mul(Value *value, Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(value, operand0, operand1) {
	}

	Mul(Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(0, operand0, operand1) {
	}

	virtual void accept(InstructionVisitor &v);
	virtual void print() {
		cout << " MUL ";
		operand0->print();
		operand1->print();
	}
};

// Defines a instruction to represent an instruction
class Constant: public Instruction {
public:
	Constant(Value *value) :
			Instruction(value, 0, 0) {
	}

	int valueNumber() {
		return getValue()->valueNumber();
	}

	virtual void visitOperands(OperandVisitor &v);
	virtual void accept(InstructionVisitor &v);
	virtual int hashCode() const;
	virtual void print() {
		cout << " CONSTANT " << value->valueNumber() << " ";
	}
};

// Defines an instruction to represent a local variable
// TODO: this is in its minimal state, just for the code challenge
class LocalVariable: public Instruction {
private:
	int slotNumber;

public:
	LocalVariable(int slotNumber) :
			Instruction(0, 0, 0) {
		this->slotNumber = slotNumber;
	}

	LocalVariable(int slotNumber, Value *value) :
			Instruction(value, 0, 0) {
		this->slotNumber = slotNumber;
	}

	virtual void accept(InstructionVisitor &v);
	virtual void visitOperands(OperandVisitor &v);
	virtual int hashCode() const;
	virtual void print() {
		cout << " v" << slotNumber << " ";
	}
};

// Move: instruction to represent an assignment instruction
class Move: public Instruction {
private:
	Instruction *rightValue;
	LocalVariable *variable;

public:
	Move(LocalVariable *var, Instruction *rValue) :
			Instruction(0, 0, 0) {
		rightValue = rValue;
		variable = var;
	}

	Instruction *getRightValue() {
		return rightValue->resolve();
	}

	void setRValue(Instruction *rValue) {
		rightValue = rValue;
	}

	LocalVariable *getVariable() {
		return variable;
	}

	void setVariable(LocalVariable *var) {
		variable = var;
	}

	virtual void visitOperands(OperandVisitor &v);
	virtual void accept(InstructionVisitor &v);
	virtual int hashCode() const;
	virtual void print() {
		variable->print();
		cout << " <- ";
		rightValue->print();
	}
};

// Definition of instruction and operand visitors
// for the DAG construction
class InstructionVisitor {

public:

	// Default visitor implementation always delegate
	// to this generic call
	void visit(Instruction &i) {
		// do nothing
	}

	void visit(BinaryInstruction &i) {
		visit(static_cast<Instruction &>(i));
	}

	void visit(Move &i) {
		visit(static_cast<Instruction &>(i));
	}

	void visit(Constant &i) {
		visit(static_cast<Instruction &>(i));
	}

	void visit(Add &i) {
		visit(static_cast<BinaryInstruction &>(i));
	}

	void visit(Mul &i) {
		visit(static_cast<BinaryInstruction &>(i));
	}

	void visit(LocalVariable &i) {
		visit(static_cast<Instruction &>(i));
	}
};

class OperandVisitor {
public:
	void visitOperand(Instruction &operand) {
	}
};


#endif
