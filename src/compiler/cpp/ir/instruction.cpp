#include "ir/instruction.h"
#include <string.h>
#include <iostream>

using namespace std;

Instruction * Instruction::resolve(){
    Instruction *p = this;

    while (p->substitute != 0)
      p = p->substitute;
    return p;
}

void BinaryInstruction::visitOperands(OperandVisitor &v) {
  v.visitOperand(*operand0);
  v.visitOperand(*operand1);
}

void Add::accept(InstructionVisitor &v){
  v.visit(*this);
}

void Mul::accept(InstructionVisitor &v){
  v.visit(*this);
}

void Constant::visitOperands(OperandVisitor &v) {
  v.visitOperand(*this);
}

void Constant::accept(InstructionVisitor &v){
  v.visit(*this);
}

void LocalVariable::accept(InstructionVisitor &v){
  v.visit(*this);
}

void LocalVariable::visitOperands(OperandVisitor &v) {
  v.visitOperand(*this);
}

void Move::visitOperands(OperandVisitor &v) {
  v.visitOperand(*rightValue);
  v.visitOperand(*variable);
}

void Move::accept(InstructionVisitor &v){
  v.visit(*this);
}
