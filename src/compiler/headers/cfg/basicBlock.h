#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "ir/instruction.h"
#include <vector>

class BasicBlock {
private:
	Instruction *firstInstruction;
	Instruction *lastInstruction;

	// add more fields like vectors for edges flowing in / out etc.
	// I will leave that out since the code exercise is not building a CFG

	BasicBlock *previous;
	BasicBlock *next;

public:
	BasicBlock(Instruction *first, Instruction *last) :
			firstInstruction(first), lastInstruction(last) {
	}
	void setFirst(Instruction *f) {
		firstInstruction = f;
	}
	Instruction * getFirst() {
		return firstInstruction;
	}
	void setLast(Instruction *l) {
		lastInstruction = l;
	}
	Instruction * getLast() {
		return lastInstruction;
	}
};

#endif
