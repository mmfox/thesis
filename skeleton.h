// Header File for the Skeleton Module

#ifndef SKELETON_H
#define SKELETON_H

#include "liChannel.cpp"
#include "baseclass.cpp"
#include "multiChannel.cpp"


class Skeleton: public Module { // Inherits from the standard module class
    public:
	Skeleton(multiChannel * channelsIn);           // Constructor with multichannel input
	Dynamic_Instruction schedule();                // Schedule an instruction
	Dynamic_Instruction execute(Dynamic_Instruction instruction); // Execute an instruction
	void commit(Dynamic_Instruction instruction);  // Commit an instruction

    private:
	multiChannel *channels;        // Internal multichannel representation               
};

#endif
