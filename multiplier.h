// Header File for the Multiply Module

#ifndef MULTIPLIER_H
#define MULTIPLIER_H

#include "liChannel.cpp"
#include "baseclass.cpp"
#include "multiChannel.cpp"


class Multiplier: public Module {
    public:
	Multiplier(multiChannel * channelsIn, int updateTag); // Constructor with multichannel input
	Dynamic_Instruction schedule();                // Schedule a multiply instruction
	Dynamic_Instruction execute(Dynamic_Instruction instruction); // Execute a multiply instruction
	void commit(Dynamic_Instruction instruction);  // Commit a multiply instruction

    private:
	multiChannel *c;        // Internal multichannel representation               
	int tagUpdate;
};

#endif
