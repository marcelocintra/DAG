
1) Build
gradle build

To run the test under a Mac OS use:
chmod a+x ./build/exe/main/main
DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:build/libs/nativeAgent/shared ./build/exe/main/main

The code now is only printing the instructions... I was close to make it work. Will do if more time is given.

2) Additional comments:

Problem:
Construct the DAG and propose optimizations for the Code snippet:

int main
{
    //Assume Matrix type is a fully fleshed out 
    //Matrix implementation, also assume that the 
    //load command will load a remote file from HDFS
    //or other endpoint
    Matrix a = loadObj("faux-remote-0");
    Matrix b = loadObj("faux-remote-1");
​
    Matrix c = a + 5;
    Matrix d = b + a;
​
    a += 10;
    b = a + a + d;
​
    for (int i = 0; i < 2; i++)
    {
    a = b + 20; 
    d = (b + c) * i;
    }
​
    Matrix e = a + b + c + d
    print(sum(e))

    return 0;
}

I started working on this over the weekend.  I made the assumption that I could not use an existing compiler like LLVM. I almost followed that path, but decided not to since the purpose is to exercise my compiler knowledge. I could have used Clang to parse the input and produce the LLVM-IR. I could then create a compilation pass to transform the LLVM-IR into a DAG.

Even though I decided to use my own source, I do not want to parse the input file. So the input to my compiler is the code already in a Three-address IR. I defined it inside main.cpp.

* Control Flow Optimizations
DAG optimizations only apply to a single basic block. The code presented above includes a loop that executes a fixed amount of iterations (2). This could be easily unrolled. There is only one induction variable (i) and the number of iterations of the loop is a constant(2)

So the code could be optimized to:

int main
{
    //Assume Matrix type is a fully fleshed out 
    //Matrix implementation, also assume that the 
    //load command will load a remote file from HDFS
    //or other endpoint
    Matrix a = loadObj("faux-remote-0");
    Matrix b = loadObj("faux-remote-1");
​
    Matrix c = a + 5;
    Matrix d = b + a;
​
    a += 10;
    b = a + a + d;
​
    a = b + 20;   // this will be eliminated in the DAG
    d = (b + c) * 1; // this will be eliminated in the DAG
    a = b + 20; 
    d = (b + c) * 2;
​
    Matrix e = a + b + c + d
    print(sum(e))

    return 0;
}


Here are the next steps:

1) Finish the algorithm to build the DAG. I have everything what is needed.
	Each variable/constant will become a new leaf node.
	There will be a hash that maps a operation in a Instruction to a DAG node. If no mapping exists, a new node will be added to the DAG and the mapping will be created. If a mapping exists, the variable will be added to the list of variables that produce that operation.
	A map will also exist to map each temporary/variable to its most recent value.
	
2) Optimizations:

	Several optimizations can be performed in a DAG:
	a) Strength reduction
		remove operations like 2 * x with x << 1;
		
	b) Algebraic optimizations
	   Use commutative and associative properties of MUL and ADD for example. Here for example we could use a sequence of matrix multiplications and determine the optimal order on which they should be evaluated to minimize the number of operations. Assuming the matrices can have different rows and columns.
	
	c) Dead code eliminations
	    expressions that are not used can be eliminated. Variable liveness information can be used to remove dead code as well.
	  
	 d) parallelization
	    After the DAG is built, the nodes can be visited in a topological sort. At each pass over the remaining vertices of the DAG, all nodes that are resolved can be scheduled (sorted) to run in parallel. In the next pass, another set of nodes will be scheduled and so on ...
	    
	 e) Code reordering
	   Also, the order on which computations are executed can be optimized taking into consideration the number of resources needed to hold temp data (memory / registers)
	   
	  f) copy propagation
	   All nodes in the DAG with more than one variable/identifier can potentially be eliminated. They can be the result of instructions of type x := y. In this case, when converting the DAG back to IR or native code, avoid the assignment to x, and use y instead.
	   
	   g) remove redundant code outside the loop. 
	   If the loop could not be unrolled, we could still remove redundant code outside the loop. For example, the statement a = b + 20; is loop invariant and can be removed: both b and the constant 20 do not change (will map to its original value) after the DAG is constructed for the block representing the loop.
	   
	   h) common subexpression elimination
	   For example the computation of b+c will be done only once.
	   
I would like to add that, for correctness, alias analysis must be performed and can prevent many of the aforementioned aggressive DAG optimizations.


