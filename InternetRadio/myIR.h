#pragma once

// myIR.h
// handle InfraRed control


#define IRrecvPin 36   /* = VP */
#define LED_BUILTIN 2


void irSetup(void);
void irLoop(void);
void irCmd2act(uint8_t);
