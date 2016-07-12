#include <iostream>
#include "ir/dag.h"
#include "ir/instruction.h"
#include "cfg/basicBlock.h"

using namespace std;

int main(int argc, char** argv) {

	// i0: Matrix a = loadObj("faux-remote-0");
	LocalVariable *a = new LocalVariable(0);

	// i1: Matrix b = loadObj("faux-remote-1");
	LocalVariable *b = new LocalVariable(1);

	a->link(b);

	// i2: Matrix c = a + 5;
	LocalVariable *c = new LocalVariable(2);
	Instruction *i2 = new Move(c, new Add(a, new Constant(new Integer(5))));

	b->link(i2);

	// i3: Matrix d = b + a;
	LocalVariable *d = new LocalVariable(3);
	Instruction *i3 = new Move(d, new Add(b, a));

	i2->link(i3);

	// i4: a += 10;

	Instruction *i4 = new Move(a, new Add(a, new Constant(new Integer(10))));
	i3->link(i4);

	// i5: b = a + a + d;

	LocalVariable *t1 = new LocalVariable(4);
	Instruction *i5a = new Move(t1, new Add(a, a));

	i4->link(t1)->link(i5a);

	Instruction *i5b = new Move(b, new Add(t1, d));
	i5a->link(i5b);

	// i6: a = b + 20;
	Instruction *i6 = new Move(a, new Add(b, new Constant(new Integer(20))));
	i5b->link(i6);

	// i7: d = (b + c) * 1;
	LocalVariable *t2 = new LocalVariable(5);
	Instruction *i7a = new Move(t2, new Add(b, c));

	i6->link(t2)->link(i7a);
	Instruction *i7b = new Move(d, new Mul(t2, new Constant(new Integer(1))));

	i7a->link(i7b);

	// i8: a = b + 20;
	Instruction *i8 = new Move(a, new Add(b, new Constant(new Integer(20))));
	i7b->link(i8);

	// i9: d = (b + c) * 2;
	LocalVariable *t3 = new LocalVariable(6);
	Instruction *i9a = new Move(t3, new Add(b, c));

	i8->link(t3)->link(i9a);
	Instruction *i9b = new Move(d, new Mul(t3, new Constant(new Integer(2))));

	i9a->link(i9b);

	// i10: Matrix e = a + b + c + d
	LocalVariable *e = new LocalVariable(9);
	LocalVariable *t4 = new LocalVariable(7);
	LocalVariable *t5 = new LocalVariable(8);
	Instruction *i10a = new Move(t4, new Add(a, b));
	i9b->link(e)->link(t4)->link(t5)->link(i10a);

	Instruction *i10b = new Move(t5, new Add(t4, c));
	Instruction *i10c = new Move(e, new Add(t5, d));
	i10a->link(i10b)->link(i10c);

	BasicBlock *codeSnippetBasicBlock = new BasicBlock(a, i10c);

	Instruction *instructionIter = codeSnippetBasicBlock->getFirst();

	while (instructionIter != 0) {
		instructionIter->print();
		cout << "\n";
		instructionIter = instructionIter->getNext();
	}

	// i11: return 0;
}
