/*  bad6502 A Raspberry Pi-based backend to a 65C02 CPU
    Copyright (C) 2025  D.Herrendoerfer

    BSD 2-Clause License
*/

/* NOP TESTER - your 1st breadboard test */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "fake6502.h"

uint8_t read6502(uint16_t address, uint8_t bank)
{
  if (address == 0xEAEA)
    return 0x58; //Clear interrupt disable flag

  if (address == 0xEB00)
    return 0x00; //insert a break
  

  return 0xEA;
}

void write6502(uint16_t address, uint8_t bank, uint8_t data)
{
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

