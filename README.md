
1) Build
gradle build

To run under a Mac OS use:
chmod a+x ./build/exe/main/main
DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:build/libs/nativeAgent/shared ./build/exe/main/main


2) Comments
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

I started working on this over the weekend.  I'll make the assumption that I cannot use an existing compiler like LLVM. I almost followed that path, but decided not to since the purpose is to exercise my compiler knowledge. I could have used Clang to parse the input and produce the LLVM-IR. I could then create a compilation pass to transform the LLVM-IR into a DAG.

Even though I decided to use my own source, I do not want to parse the input file. So the input to my compiler is the code already in a Three-address IR.

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
    a = b + 20; 
    d = (b + c) * 1;
    a = b + 20; 
    d = (b + c) * 2;
​
    Matrix e = a + b + c + d
    print(sum(e))

    return 0;
}


Here are the next steps:

1) Finish the algorithm to build the DAG. I have almost everything needed. The idea is to create a Map from a three-address operation to its correspondent node in



