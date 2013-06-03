////////////////////////////////////////////////////////////////////////////////
// hardware.h                                                                 //
// Various hardware-related & helper functions and defines                    //
//                                                                            //
// Copyright 2008-2009 Jakub Bednarski                                        //
// Copyright 2012-     Christian Vogelgsang, A.M. Robinson, Rok Krajnc        //
//                                                                            //
// This file is part of Minimig                                               //
//                                                                            //
// Minimig is free software; you can redistribute it and/or modify            //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation; either version 2 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// Minimig is distributed in the hope that it will be useful,                 //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// Changelog                                                                  //
//                                                                            //
// 2012-08-02 - rok.krajnc@gmail.com                                          //
// Updated with OR1200-specific functions and defines                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef __HARDWARE_H__
#define __HARDWARE_H__


#include <inttypes.h>
#include "spr_defs.h"
#include "or32_defs.h"


//// make sure NULL is defined ////
#ifndef NULL
#define NULL 0
#endif


//// debug output ////
//#define DEBUG

#ifdef DEBUG
#define DBGPRINT(...)       printf(__VA_ARGS__)
#else
#define DBGPRINT(...)
#endif

#define STR(x)              #x
#define XSTR(x)             STR(x)

#define DEBUG_FUNC_IN()     DBGPRINT("* DEBUG : FUNC IN  : %s(), file " __FILE__ ", line " XSTR(__LINE__) "\r", __FUNCTION__)
#define DEBUG_FUNC_OUT()    DBGPRINT("* DEBUG : FUNC OUT : %s()\r", __FUNCTION__)
#define DEBUG_MSG(x)        DBGPRINT("* DEBUG : " x "\r")


//// memory read/write ////
#define read8(adr)          (*((volatile uint8_t  *)(adr)))
#define read16(adr)         (*((volatile uint16_t *)(adr)))
#define read32(adr)         (*((volatile uint32_t *)(adr)))
#define write8(adr, data)   (*((volatile uint8_t  *)(adr)) = (data))
#define write16(adr, data)  (*((volatile uint16_t *)(adr)) = (data))
#define write32(adr, data)  (*((volatile uint32_t *)(adr)) = (data))


//// system ////
#define SYS_CLOCK           50000000  // system clock in Hz
#define SYS_PERIOD_NS       20        // system period in ns
#define ROM_START           0x000000
#define RAM_START           0x400000
#define REG_START           0x800000
#define REG_RST_ADR         0x800000  // reset reg  (bit 0 = ctrl reset, bit 1 = minimig reset)
#define REG_SYS_ADR         0x800004  // system reg (bits [3:0] = cfg input, bits [18:15] = status output)
#define REG_UART_ADR        0x800008  // uart transmit reg ([7:0] - transmit byte)
#define REG_TIMER_ADR       0x80000c  // timer reg ([15:0] - timer counter)
#define REG_SPI_DIV_ADR     0x800010  // SPI divider reg
#define REG_SPI_CS_ADR      0x800014  // SPI chip-select reg
#define REG_SPI_DAT_ADR     0x800018  // SPI data reg
#define REG_SPI_BLOCK_ADR   0x80001c  // SPI block transfer counter reg


//// minimig stuff ////

#define DISKLED_ON  // *AT91C_PIOA_SODR = DISKLED;
#define DISKLED_OFF // *AT91C_PIOA_CODR = DISKLED;

#define LEDS(x)             write16(REG_SYS_ADR, (x))

#define EnableCard()        write32(REG_SPI_CS_ADR, 0x11)
#define DisableCard()       write32(REG_SPI_CS_ADR, 0x10)
#define EnableFpga()        write32(REG_SPI_CS_ADR, 0x22)
#define DisableFpga()       write32(REG_SPI_CS_ADR, 0x20)
#define EnableOsd()         write32(REG_SPI_CS_ADR, 0x44)
#define DisableOsd()        write32(REG_SPI_CS_ADR, 0x40)
#define EnableDMode()       write32(REG_SPI_CS_ADR, 0x88)
#define DisableDMode()      write32(REG_SPI_CS_ADR, 0x80)

#define SPI_slow()          write32(REG_SPI_DIV_ADR, 0x3f)
#define SPI_fast()          write32(REG_SPI_DIV_ADR, 0x00)
#define SPI_write(x)        write32(REG_SPI_DAT_ADR, (x))
#define SPI_read()          (read32(REG_SPI_DAT_ADR))
#define SPI(x)              (SPI_write(x), SPI_read())
#define SPI_block(x)        write32(REG_SPI_BLOCK_ADR, (x))

#define RS232(x)            write32(REG_UART_ADR, (x))

#define TIMER_get()         (read32(REG_TIMER_ADR))
#define TIMER_set(x)        write32(REG_TIMER_ADR, (x))
#define TIMER_wait(x)       {TIMER_set(0); while (TIMER_get()<(x));}

// waste a few cycles to let the FPGA catch up
#define SPIN()              {read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR); read32(REG_SPI_DIV_ADR);}

#define RST_system()        write32(REG_RST_ADR, 0x1)
#define RST_minimig()       write32(REG_RST_ADR, 0x2)


//// system stuff ////
// align
#define ALIGN(addr,size)    ((addr + (size-1))&(~(size-1)))

// func pointer
#define FUNC(r,n,p...)      r _##n(p); r (*n)(p) = _##n; r _##n(p)

// atomic operation
#define ATOMIC(x...)        {unsigned val = disable_ints(); {x;} restore_ints(val);}

// read & write or1200 SPR
#define mtspr(spr, value)   { asm("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value)); }
#define mfspr(spr)          ({ unsigned long __val;                                \
                            asm("l.mfspr\t\t%0,%1,0" : "=r" (__val) : "r" (spr));  \
                            __val; })

// enable & disable ints
#define disable_ints()      ({ unsigned __x = mfspr(SPR_SR);   \
                            mtspr(SPR_SR, __x & ~SPR_SR_IEE);  \
                            __x; })
#define restore_ints(x)     mtspr(SPR_SR, x)
#define enable_ints()       mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_IEE)

#define report(val)         { asm("l.add r3,r0,%0": :"r" (val)); \
                            asm("l.nop\t2"); }


//// global variables ////
extern uint32_t fw_copy_routine[];

//// function declarations ////
unsigned long CheckButton(void);
void Timer_Init(void);
unsigned long GetTimer(unsigned long offset);
unsigned long CheckTimer(unsigned long time);
void WaitTimer(unsigned long time);

void *hmalloc(int size);
void sys_jump(unsigned long addr);
void sys_load(uint32_t * origin, uint32_t * dest, uint32_t size, uint32_t * routine);


#endif // __HARDWARE_H__

