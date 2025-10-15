/*  bad6502 A Raspberry Pi-based backend to a 65C02 CPU
    Copyright (C) 2025  D.Herrendoerfer

    BSD 2-Clause License
*/

/* NOP TESTER - your 1st breadboard test 
 *
 * modified to have a real stack in memory
 * modified to enable interrupts 
 * modified to inject an interrupt everu 200 cycles
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "fake6502.h"

// the stack
uint8_t stack[255];

uint8_t read6502(uint16_t address, uint8_t bank)
{
  // Manual reset vector
  if (address == 0xFFFC)
    return 0x00; 
  if (address == 0xFFFD)
    return 0x02;
  
  // Manual stack read
  if (address >= 0x100 & address < 0x200)
    return stack[address-0x100];

  if (address == 0x0200)
    return 0x58; //Clear interrupt disable flag

  if (address == 0xEAF0)
    return 0x40; //RTI

  // Insert break test
  if (address == 0x210) {
    return 0x00; //insert a break
  }
  
  return 0xEA;
}

void write6502(uint16_t address, uint8_t bank, uint8_t data)
{
  // Manual stack write
  if (address >= 0x100 & address < 0x200)
    stack[address-0x100] = data;

  return;
}

uint16_t pc;
uint16_t sleep_val = 0;

// Main prog
int main(int argc, char **argv)
{
  uint16_t nextirq;

  reset6502(0);
  
  for (int i=0 ; i<32 ; i++) {
    step6502();
    usleep(100);
  }

  pc=bus_addr;

  nextirq = clockticks6502 + 200;

loop:  
  step6502();
  if (bus_addr != pc) {
    if (bus_addr != (uint16_t)(pc + 1)) {
      printf("pc mismatch (%04X,%04X)\n",bus_addr, (uint16_t)(pc+1));
      sleep_val++;
    }
  }
  pc=bus_addr;
  //usleep(sleep_val);
  
  #ifndef FAST
  usleep(100000);
  #endif

  if (clockticks6502 > nextirq ) {
    irq6502(1);
  }

  if ((clockticks6502-5) > nextirq ) {
    irq6502(0);
    nextirq += 200;
  }

goto loop;

  printf("\n\nExecution stopped. (but we never get here)");

  return 0;
}

