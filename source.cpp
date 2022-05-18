// Ezekiel Barefoot
// CSCI470

#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

typedef void ThreadFun(int threadNum);

int currentThread;

struct ThreadControlBlock {
    int SP, IP, AX, BX, CX, DX;
    void(*program)(int threadNum);

    ThreadControlBlock(void(*func)(int threadNum))  {
        AX = 0;
        BX = 0;
        CX = 0;
        DX = 0;
        SP = 0;
        IP = 0;
        program = func;
    }
    
};

vector<ThreadControlBlock*> tcb;

void saveContext(ThreadControlBlock *thread) {
    asm("mov %rax, 16(%rdi)");
    asm("mov %rbx, 24(%rdi)");
    asm("mov %rcx, 32(%rdi)");
    asm("mov %rdx, 40(%rdi)");
    // rsp saved after other registers because RSP can't be reached directly
    asm("mov %rsp, %rax");
    asm("mov %rax, 0(%rdi)");

    // is this correct? i guess i'll find out
    asm("lea 0(%rip), %rax");
    asm("mov %rax, 8(%rdi)");
}

void loadContext(ThreadControlBlock *thread) {
    // rsp loaded before other registers because rsp can't be reached directly
    asm("mov 0(%rdi), %rax");
    asm("mov %rax, %rsp");
    asm("mov 16(%rdi), %rax");
    asm("mov 24(%rdi), %rbx");
    asm("mov 32(%rdi), %rcx");
    asm("mov 40(%rdi), %rdx");
    
    // set instruction pointer
    asm("mov 8(%rdi), %r9");
    asm("jmp *%r9");
}

void startThread(void (*func)(int thread), bool startMainThread=false) {
    // create a TCB for main if no threads have been started yet
    if(tcb.size() == 0) { 
        startThread((void*) main(3)); // question 1: is this possible? iv'e tried differect param delcarations but im not sure how to 
        saveContext(tcb[0]);          // get main to work with this function as it doesn't take an int as an arg. maybe just create
        currentThread = 0;            // a function specifically for main?
    }

    ThreadControlBlock *newTCB = new ThreadControlBlock(func);
    tcb.push_back(newTCB);

    func(1); // question 2:
             // how might i start the thread program? im not sure what to pass in as a parameter.
             // with my current code, it doesn't look like i need this parameter for anything (is this wrong?).
             // but passing in a magic number seems.. bad.
}


void contextSwitch(ThreadControlBlock *from, ThreadControlBlock *to) {
    saveContext(from);
    loadContext(to);
}

void sharecpu(int thread) {    
    int next = rand()%tcb.size();
    contextSwitch(tcb[currentThread], tcb[next]);
    currentThread = next;
}




/* Change nothing below this line. Get the program to execute the code
 * of main, main1, and main2
 * It does not matter the order that the code for main, main1, and
 * main2 gets executed, but it should execute a piece of the code
 * and then use sharecpu to give other threads a chance to execute.
 */ 

void main1(int threadNum) {
    for (int i=0;i<10;i++) {
        cout << "Main 1 says Hello" << endl;
        usleep(1000);
        sharecpu(threadNum);
    }
}

void main2(int thread) {
    for (int i=0;i<10;i++) {
        cout << "Main 2 says Hello" << endl;
        usleep(1000);
        sharecpu(thread);
    }
}

int main() {
    startThread(main1);
    cout << "Back to Main" << endl;
    startThread(main2);
    while(true) {
        cout << "Main says hello" << endl;
        usleep(100);
        sharecpu(0);
    }
    return 0;
}void saveContext(ThreadControlBlock *thread) {
    asm("mov %rax, 16(%rdi)");
    asm("mov %rbx, 24(%rdi)");
    asm("mov %rcx, 32(%rdi)");
    asm("mov %rdx, 40(%rdi)");
    // rsp saved after other registers because RSP can't be reached directly
    asm("mov %rsp, %rax");
    asm("mov %rax, 0(%rdi)");

    // 1
    asm("lea 0(%rip), %rax");
    asm("mov %rax, 8(%rdi)");
}

void loadContext(ThreadControlBlock *thread) {
    // rsp loaded before other registers because rsp can't be reached directly
    asm("mov 0(%rdi), %rax");
    asm("mov %rax, %rsp");
    asm("mov 16(%rdi), %rax");
    asm("mov 24(%rdi), %rbx");
    asm("mov 32(%rdi), %rcx");
    asm("mov 40(%rdi), %rdx");
    asm("mov 8(%rdi), %r9");
    asm("jmp *%r9");
}
