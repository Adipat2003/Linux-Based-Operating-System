#include "idt_handlers.h"

/* IDT Initializer
 * 
 * Initializes the IDT by setting types and entry handlers
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void init_IDT()
{
    unsigned i;

    //initialize exceptions (indices x00 through x13 in IDT)
    //the Gate Type should be Interrupt, so 011100
    //therefore size, reserved 1, reserved 2 are 1
    for (i = 0; i < EXCEPTION_COUNT; i++)
    {
        idt[i].seg_selector = KERNEL_CS; //it's a code segment so CS
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].dpl = 0;  //should be 0 for interrupt handlers to run
        idt[i].present = 1; //set to 1 to make descriptor valid 
    }

    //initialize Sys Calls (index x80 in IDT)
    //pretty much the same thing as excpetions but different DPL value
    idt[SYS_CALL].seg_selector = KERNEL_CS;
    idt[SYS_CALL].reserved2 = 1;
    idt[SYS_CALL].reserved1 = 1;
    idt[SYS_CALL].size = 1;
    idt[SYS_CALL].dpl = 3;    //3 for Sys Calls
    idt[SYS_CALL].present = 1;

    //need to set PIT and keyboard and RTC interrupts as well
    //set PIT entry
    idt[PIT_INTR].seg_selector = KERNEL_CS;
    idt[PIT_INTR].reserved2 = 1;
    idt[PIT_INTR].reserved1 = 1;
    idt[PIT_INTR].size = 1;
    idt[PIT_INTR].dpl = 0;
    idt[PIT_INTR].present = 1;
    //set keyboard entry
    idt[KEYBOARD_INTR].seg_selector = KERNEL_CS;
    idt[KEYBOARD_INTR].reserved2 = 1;
    idt[KEYBOARD_INTR].reserved1 = 1;
    idt[KEYBOARD_INTR].size = 1;
    idt[KEYBOARD_INTR].dpl = 0;
    idt[KEYBOARD_INTR].present = 1;
    //set RTC entry
    idt[RTC_INTR].seg_selector = KERNEL_CS;
    idt[RTC_INTR].reserved2 = 1;
    idt[RTC_INTR].reserved1 = 1;
    idt[RTC_INTR].size = 1;
    idt[RTC_INTR].dpl = 0;
    idt[RTC_INTR].present = 1;

    //funciton call here to avoid seperate function call in boot.S
    //set IDT entries for all exception handlers
    SET_IDT_ENTRY(idt[0], Division_By_Zero);
    SET_IDT_ENTRY(idt[1], Single_Step_Interrupt);
    SET_IDT_ENTRY(idt[2], NMI);
    SET_IDT_ENTRY(idt[3], Breakpoint);
    SET_IDT_ENTRY(idt[4], Overflow);
    SET_IDT_ENTRY(idt[5], Bound_Range_Exceeded);
    SET_IDT_ENTRY(idt[6], Invalid_Opcode);
    SET_IDT_ENTRY(idt[7], Coprocessor_Not_Available);
    SET_IDT_ENTRY(idt[8], Double_Fault);
    SET_IDT_ENTRY(idt[9], Coprocessor_Segment_Overrun);
    SET_IDT_ENTRY(idt[10], Invalid_Task_State_Segment);
    SET_IDT_ENTRY(idt[11], Segment_Not_Present);
    SET_IDT_ENTRY(idt[12], Stack_Segment_Fault);
    SET_IDT_ENTRY(idt[13], General_Protection_Fault);
    SET_IDT_ENTRY(idt[14], Page_Fault);
    SET_IDT_ENTRY(idt[15], Reserved);
    SET_IDT_ENTRY(idt[16], Floating_Point_Exception);
    SET_IDT_ENTRY(idt[17], Alignment_Check);
    SET_IDT_ENTRY(idt[18], Machine_Check);
    SET_IDT_ENTRY(idt[19], SIMD_Floating_Point_Exception);
    SET_IDT_ENTRY(idt[SYS_CALL], sys_call_link);

    //set entry for RTC and keyboard
    SET_IDT_ENTRY(idt[PIT_INTR], pit_wrapper);
    SET_IDT_ENTRY(idt[KEYBOARD_INTR], keyboard_wrapper);
    SET_IDT_ENTRY(idt[RTC_INTR], rtc_wrapper);
}
