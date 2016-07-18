#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <iostream>

using namespace std;

// These are the operators for our instruction set
// I'm only defining what I need for the code exercise
typedef enum {
	ADD, MUL, MOVE, PRINT, CALL, RETURN, CONSTANT, LOCALVARIABLE, NUMBER_OF_OPERATORS
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
	virtual int valueNumber() const = 0;

	bool operator==(const Value &other) const {
		return valueNumber() == other.valueNumber();
	}
};

class Integer: public Value {
public:
	Integer(int v) :
			value(v) {
	}

	virtual int valueNumber() const {
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

	Value *getValue() {
		return value;
	}

	// Check if the instruction has a substitute
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
	virtual void print() = 0;
	virtual void accept(InstructionVisitor &v) = 0;
	virtual Operator getInstructionID() = 0;
	virtual void visitOperands(OperandVisitor &v) = 0;
	virtual int hashCode() const = 0 ;
};

class BinaryInstruction: public Instruction {
protected:
	Operator op;
	Instruction *operand0;
	Instruction *operand1;

public:
	BinaryInstruction(Operator opr, Value *value, Instruction *oper0, Instruction *oper1) :
			op(opr), operand0(oper0), operand1(oper1), Instruction(value, 0, 0) {
	}

	Instruction * getOperand0() {
		return operand0->resolve();
	}

	bool operator==(const BinaryInstruction &other) const {
		return operand0 == other.operand0 &&
				operand1 == other.operand1 &&
				op == other.op;
	}

	void setOperand0(Instruction *operand) {
		this->operand0 = operand;
	}

	Instruction *getFirstOperand() { return operand0; }

	Instruction * getOperand1() {
		return operand1->resolve();
	}

	Instruction *getSecondOperand() { return operand1; }

	void setOperand1(Instruction *operand) {
		this->operand1 = operand;
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
private:

public:
	Add(Value *value, Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(ADD, value, operand0, operand1) {
	}

	Add(Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(ADD, 0, operand0, operand1) {
	}

	virtual Operator getInstructionID() { return ADD; }

	virtual void accept(InstructionVisitor &v);
	virtual void print() {
		cout << " ADD ";
		operand0->print();
		operand1->print();
	}
};

class Mul: public BinaryInstruction {
private:
	Operator op;
public:
	Mul(Value *value, Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(MUL, value, operand0, operand1) {
	}

	Mul(Instruction *operand0, Instruction *operand1) :
			BinaryInstruction(MUL, 0, operand0, operand1) {
	}

	virtual Operator getInstructionID() { return MUL; }

	virtual void accept(InstructionVisitor &v);
	virtual void print() {
		cout << " MUL ";
		operand0->print();
		operand1->print();
	}
};

// Defines a instruction to represent a constant
class Constant: public Instruction {
public:
	Constant(Value *value) :
			Instruction(value, 0, 0) {
	}

	int valueNumber() {
		return getValue()->valueNumber();
	}

	bool operator==(const Constant &other) const {
		return value == other.value;
	}

	virtual Operator getInstructionID() { return CONSTANT; }

	virtual void visitOperands(OperandVisitor &v);
	virtual void accept(InstructionVisitor &v);
	virtual int hashCode() const;
	virtual void print() {
		cout << " CONSTANT(" << value->valueNumber() << ")";
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

	bool operator==(const LocalVariable &other) const {
		return value == other.value;
	}

	virtual Operator getInstructionID() { return LOCALVARIABLE; }

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

	void setRightValue(Instruction *rValue) {
		rightValue = rValue;
	}

	LocalVariable *getVariable() {
		return variable;
	}

	void setVariable(LocalVariable *var) {
		variable = var;
	}

	bool operator==(const Move &other) const {
		return rightValue == other.rightValue &&
				variable == other.variable;
	}

	virtual Operator getInstructionID() { return MOVE; }

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

// defines how to compute the hash for the Instruction class
namespace std {

  template <>
  struct hash<Instruction>
  {
    std::size_t operator()(const Instruction& i) const
    {
      using std::size_t;
      using std::hash;

      // uses hashCode()
      return ((hash<int>()(i.hashCode())));
    }
  };

}


#endif
