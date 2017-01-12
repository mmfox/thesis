// Header File for the TIA Module

#ifndef TIA_H
#define TIA_H

#include "liChannel.cpp"
#include "baseClass.cpp"
#include "multiChannel.cpp"

class TIA: public Module {
    public:
	TIA(multiChannel * hChans, multiChannel * iChans, multiChannel * oChans, int numPReg, int numDReg); // Constructor 
	Dynamic_TIAinstruction schedule();                                  // Schedule and return an Dynamic Inst
	std::vector<int> canTrigger();                           // Return a vector of instructions that can trigger
	void trigger(std::vector<Dynamic_TIAinstruction> tList);            // Trigger instructions in tList, add all important info to scoreboards
	Dynamic_TIAinstruction execute(Dynamic_TIAinstruction instruction); // Execute a dynamic instruction and pass it back
	void commit(Dynamic_TIAinstruction instruction);                    // Commit dynamic instruction
	void addInstruction(TIAinstruction newInst);                        // Add Instruction to instruction bank
        bool canRun(TIAinstruction inst);                                   // Method to check if an instruction can trigger
	TIAinstruction decode(int encodedInst);                             // Return a TIAinstruction from common ones 

	multiChannel *helpChans;                // Helper channels to program and update TIA state
	multiChannel *inputChans;               // Input channels to the TIA module
	multiChannel *outputChans;              // Output channels from the TIA module
	PredRegs predRegs;                      // Predicate Registers
	DataRegs dataRegs;                      // Data Registers
	std::vector<TIAinstruction> iMem;       // Instruction bank for TIA
	std::vector<bool> predScore;            // Predicate Scoreboard
	std::vector<bool> inChanScore;          // Input Channel Scoreboard
	std::vector<bool> dataRegScore;         // Data Reg Scoreboard
};

#endif
