/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
#include "i8259.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) 
{
    /* sending mask signals first for init */
    outb(master_mask, MASTER_8259_PORT_DATA);
    outb(slave_mask, SLAVE_8259_PORT_DATA);

    /* setting up for IR0-7 (0x20-0x27) and selecting 8086 mode for master */
    outb(ICW1, MASTER_8259_PORT_COMMAND);
    outb(ICW2_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW3_MASTER, MASTER_8259_PORT_DATA);
    outb(ICW4, MASTER_8259_PORT_DATA);

    /* setting up for IR0-7 (0x28-0x2F) and selecting 8086 mode for slave */
    outb(ICW1, SLAVE_8259_PORT_COMMAND);
    outb(ICW2_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);
    outb(ICW4, SLAVE_8259_PORT_DATA);

    /* enabling irq2 on master i.e. slave enabled */
    master_mask = ~(mask_shift_one << irq2_num);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) 
{
    //check if IRQ from master PIC and enable
    if (irq_num >= first_irq && irq_num <= master_end_irq) // master mask (IRQ 0-7)
    {
        master_mask &= ~(mask_shift_one << irq_num); // enabling the specific irq_num by bit shifting and notting with original mask
        outb(master_mask, MASTER_8259_PORT_DATA); // pushing out the new mask
        return;
    }

    //check if IRQ from slave PIC and enable
    else if (irq_num < final_irq) // slave mask (IRQ 8-15)
    {
        slave_mask &= ~(mask_shift_one << (irq_num - sub_master)); // enabling the specific irq_num by bit shifting and notting with original mask
        outb(slave_mask, SLAVE_8259_PORT_DATA); // pushing out the new mask
        return;
    }

    else
        return; // don't care state
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) 
{   
    //check if IRQ from master PIC and disable
    if (irq_num >= first_irq && irq_num <= master_end_irq) // master mask (IRQ 0-7)
    {
        master_mask |= (mask_shift_one << irq_num); // disabling the specific irq_num by bit shifting and notting with original mask
        outb(master_mask, MASTER_8259_PORT_DATA); // pushing out the new mask
        return;
    }

    //check if IRQ from slave PIC and disable
    else if (irq_num < final_irq) // slave mask (IRQ 8-15)
    {
        slave_mask |= (mask_shift_one << (irq_num - sub_master)); // disabling the specific irq_num by bit shifting and notting with original mask
        outb(slave_mask, SLAVE_8259_PORT_DATA); // pushing out the new mask
        return;
    }

    else
        return;
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) 
{   
    //check if EOI to master PIC (IRQ 0-7)
    if (irq_num >= first_irq && irq_num <= master_end_irq) // master mask
    {
        /* resetting master mask */
        outb(EOI|irq_num, MASTER_8259_PORT_COMMAND);
        return;
    }

    //check if EOI to slave PIC (IRQ 8-15)
    else if (irq_num < final_irq)
    {
        /* resetting masks for slave and irq2 on the master */
        outb(EOI|(irq_num - sub_master), SLAVE_8259_PORT_COMMAND);
        outb(EOI|irq2_num, MASTER_8259_PORT_COMMAND);
        return;
    }   

    else
    {
        return;
    }
}
