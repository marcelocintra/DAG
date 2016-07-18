#include "ir/instruction.h"
#include <string.h>
#include <iostream>

using namespace std;

Instruction * Instruction::resolve() {
	Instruction *p = this;

	while (p->substitute != 0)
		p = p->substitute;
	return p;
}

BinaryInstruction::~BinaryInstruction() {
	// operands are deleted from the basic block instruction list
}

void BinaryInstruction::visitOperands(OperandVisitor &v) {
	v.visitOperand(*operand0);
	v.visitOperand(*operand1);
}

int BinaryInstruction::hashCode() const {
	return ((op << 4) ^ (operand0->hashCode() >> 16) ^ (operand1->hashCode() << 16));
}

void Add::accept(InstructionVisitor &v) {
	v.visit(*this);
}

void Mul::accept(InstructionVisitor &v) {
	v.visit(*this);
}

void Constant::visitOperands(OperandVisitor &v) {
	v.visitOperand(*this);
}

int Constant::hashCode() const {
	return value->valueNumber();
}

void Constant::accept(InstructionVisitor &v) {
	v.visit(*this);
}

void LocalVariable::accept(InstructionVisitor &v) {
	v.visit(*this);
}

void LocalVariable::visitOperands(OperandVisitor &v) {
	v.visitOperand(*this);
}

int LocalVariable::hashCode() const {
	return slotNumber;
}

void Move::visitOperands(OperandVisitor &v) {
	v.visitOperand(*rightValue);
	v.visitOperand(*variable);
}

void Move::accept(InstructionVisitor &v) {
	v.visit(*this);
}

int Move::hashCode() const {
	return (rightValue->hashCode() >> 16) ^ (variable->hashCode() << 16) ;
}
