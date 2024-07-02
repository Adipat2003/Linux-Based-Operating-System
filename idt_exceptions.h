#ifndef _IDT_EXCEPTIONS_H
#define _IDT_EXCEPTIONS_H
#include "x86_desc.h"
#include "idt_handlers.h"
#include "sys_call.h"

#define EXCEPTION_STATUS 69 //random value from 0-255

extern void Division_By_Zero(void);
extern void Single_Step_Interrupt(void);
extern void NMI(void);
extern void Breakpoint(void);
extern void Overflow(void);
extern void Bound_Range_Exceeded(void);
extern void Invalid_Opcode(void);
extern void Coprocessor_Not_Available(void);
extern void Double_Fault(void);
extern void Coprocessor_Segment_Overrun(void);
extern void Invalid_Task_State_Segment(void);
extern void Segment_Not_Present(void);
extern void Stack_Segment_Fault(void);
extern void General_Protection_Fault(void);
extern void Page_Fault(void);
extern void Reserved(void);
extern void Floating_Point_Exception(void);
extern void Alignment_Check(void);
extern void Machine_Check(void);
extern void SIMD_Floating_Point_Exception(void);

extern void System_Call();

extern int exception_flag;
extern uint8_t exception_status;

#endif
