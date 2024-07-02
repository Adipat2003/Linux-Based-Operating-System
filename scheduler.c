#include "scheduler.h"

int curr_ter = 0;
int status_ter = 1;
//tempPID = 0;
int flag0 = 1;
int flag1 = 1;
int flag2 = 1;
int flag3 = 1;
// int bug_flag = 9;

void pit_init(){
    int divisor = 1193182 / 40;       /* Calculate our divisor */
    outb(0x36, CMD_REG);             /* Set our command byte 0x36 */
    outb(divisor & 0xFF, CH0);   /* Set low byte of divisor */
    outb(divisor >> 8, CH0);     /* Set high byte of divisor */

    enable_irq(0);
}

void pit_handler(){
    send_eoi(0);
    // if(flag3 == 0){
        // int PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1];
        // PCB_t* currentPCB = get_PCB(PID);
        // asm("\t movl %%esp, %0" : "=r"(currentPCB->current.exec_esp));
        // asm("\t movl %%ebp, %0" : "=r"(currentPCB->current.exec_ebp));
    //}
    
    scheduler();
    return;
}

void scheduler(){
    int PID;
    cli();
    // printf("%d", curr_ter);
    if(flag0 == 1){
        PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx];
        PCB_t* currentPCB = get_PCB(PID);
        // curr_ter++;
        switch_terminal(0);
        terminal_clear();
        flag0 = 0;
        asm("\t movl %%esp, %0" : "=r"(currentPCB->current.exec_esp));
        asm("\t movl %%ebp, %0" : "=r"(currentPCB->current.exec_ebp));
        execute((uint8_t*)"shell");
    }
    else if(flag1 == 1){
        curr_ter++;
        PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx];
        PCB_t* currentPCB = get_PCB(PID);
        //curr_ter++;
        switch_terminal(1);
        terminal_clear();
        asm("\t movl %%esp, %0" : "=r"(currentPCB->current.exec_esp));
        asm("\t movl %%ebp, %0" : "=r"(currentPCB->current.exec_ebp));
        execute((uint8_t*)"shell");
    }
    else if(flag2 == 1){
        curr_ter++;
        PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx];
        PCB_t* currentPCB = get_PCB(PID);
        // curr_ter = 0;
        switch_terminal(2);
        terminal_clear();
        asm("\t movl %%esp, %0" : "=r"(currentPCB->current.exec_esp));
        asm("\t movl %%ebp, %0" : "=r"(currentPCB->current.exec_ebp));
        execute((uint8_t*)"shell");
    }
    else if(flag3 == 1){
        // curr_ter = 0;
        flag3 = 0;
        switch_terminal(0);
    }
    
    PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1];
    PCB_t* currentPCB = get_PCB(PID);
        
    // terminal_update(curr_ter);
    //Get PCB of the terminal our curr_ter variable is on with the most recently running process
    // PCB_t* currentPCB = get_PCB(PID);
    
    curr_ter++;
    if (curr_ter >= 3)
        curr_ter = 0;
    
    //Take current assembly esp, ebp and store them into currently running pcb fields
    //Update the esp, ebp with the pcb fields of the next program to be run by scheduler
    
    PID = terminals[curr_ter].pids[terminals[curr_ter].pid_idx - 1];
    PCB_t* newPCB = get_PCB(PID);
    currentPID = PID;                               
    // update_terminal_screen(curr_ter);
    
    update_vidmap(curr_ter);

    flush_tlb();

    new_program(PID);

    flush_tlb();

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (eightMB - ((PID) * eightKB)) - sizeof(int32_t);

    // send_eoi(0);

    sti();

    asm("\t movl %%esp, %0" : "=r"(currentPCB->current.exec_esp));
    asm("\t movl %%ebp, %0" : "=r"(currentPCB->current.exec_ebp));
    scheduler_asm(newPCB->current.exec_esp, newPCB->current.exec_ebp);
    
    // asm volatile(
    //     "movl %0, %%esp       \n"
    //     "movl %1, %%ebp       \n"
    //     :
    //     : "r" (newPCB->current.exec_esp), "r" (newPCB->current.exec_ebp)
    //     : "esp" , "ebp"
    // );
    return;
}
