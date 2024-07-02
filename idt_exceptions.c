#include "idt_exceptions.h"
#include "x86_desc.h"
#include "lib.h"

int exception_flag = 0;         //global flag (for halt) used to indicate when exception has been raised 
uint8_t exception_status = EXCEPTION_STATUS;  //status arg passed in for halt call

/* Division by Zero Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Division_By_Zero(void)
{
    printf("Exception: Division by zero \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Single Step Interrupt Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Single_Step_Interrupt(void)
{
    printf("Exception: Single-step interrupt \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* NMI Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void NMI(void)
{
    printf("Exception: NMI \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Breakpoint Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Breakpoint(void)
{
    printf("Exception: Breakpoint \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Overflow Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Overflow(void)
{
    printf("Exception: Overflow \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Bound Range Exceeded Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Bound_Range_Exceeded(void)
{
    printf("Exception: Bound Range Exceeded \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Invalid Opcode Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Invalid_Opcode(void)
{
    printf("Exception: Invalid Opcode \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Coprocessor Not Availiable Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Coprocessor_Not_Available(void)
{
    printf("Exception: Coprocessor not available \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Double Fault Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Double_Fault(void)
{
    printf("Exception: Double Fault \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Corprocessor Segment Overrun Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Coprocessor_Segment_Overrun(void)
{
    printf("Exception: Coprocessor Segment Overrun \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Invalid Task State Segment Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Invalid_Task_State_Segment(void)
{
    printf("Exception: Invalid Task State Segment \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Segment Not Present Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Segment_Not_Present(void)
{
    printf("Exception: Segment not present \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Stack Segment Fault Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Stack_Segment_Fault(void)
{
    printf("Exception: Stack Segment Fault \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* General Protection Fault Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void General_Protection_Fault(void)
{
    printf("Exception: General Protection Fault \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Page Fault Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Page_Fault(void)
{
    printf("Exception: Page Fault \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Reserved Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Reserved(void)
{
    printf("Exception: Reserved \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Floating Point Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Floating_Point_Exception(void)
{
    printf("Exception: Floating Point Exception \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Alignment Check Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Alignment_Check(void)
{
    printf("Exception: Alignment Check \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* Machine Check Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void Machine_Check(void)
{
    printf("Exception: Machine Check \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* SIMD Floating Point Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: Halts program
 */
void SIMD_Floating_Point_Exception(void)
{
    printf("Exception: SIMD Floating-Point Exception \n");
    exception_flag = 1; //raise exception flag
    halt(exception_status); //halt program
    //while(1){};
}

/* System Call Exception Handler
 * 
 * Prints an exception error message
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void System_Call(void)
{
    printf("System Call invoked \n");
    //while(1){};
}
