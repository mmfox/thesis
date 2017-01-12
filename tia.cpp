///////////////////////////////////
// TIA Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 1/25/14
//////////////////////////////////

/*
This is the module for the TIA processing element.  
*/

#ifndef TIA_CPP
#define TIA_CPP

#include <iostream>
#include <vector>
using namespace std;

#include "baseclass.cpp"
#include "tia.h"

#define T_WRITE 0

// Defining constructor for the TIA class
TIA::TIA(multiChannel * hChans, multiChannel * iChans, multiChannel * oChans, int numPReg, int numDReg){
    this->helpChans = hChans;        // Assign Helper Channels
    this->inputChans = iChans;       // Assign Input Channels
    this->outputChans = oChans;      // Assign Output Channels
    this->predRegs.addRegs(numPReg); // Add assigned variable number of predicate registers
    this->dataRegs.addRegs(numDReg); // Add assigned variable number of data registers
    for(int i=0;i<numPReg;i++){
        this->predScore.push_back(false);         // Initialize scoreboard to all false for pred regs
    }
    for(int i=0;i<iChans->getNumChannels();i++){
        this->inChanScore.push_back(false);       // Initialize scoreboard to all false for input chans
    }
    for(int i=0;i<numDReg;i++){
        this->dataRegScore.push_back(false);      // Initialize scoreboard to all false for data regs
    }
};

// Add Instruction to instruction bank
void TIA::addInstruction(TIAinstruction newInst){
    // Checks that all instructions added are legal - can be run
    // Set "legal" flag to start as true and then check it at the end
    bool legal = true;

    // Check that required predicate state is possible
    for(int i=0; i < newInst.getPredState().size(); i++){
        if(newInst.getPredState()[i].first >= this->predRegs.getNumRegs()){
	    legal = false;
	}
    }

    // Check that required input flags are possible
    for(int i=0; i < newInst.getInputFlags().size(); i++){
        if(newInst.getInputFlags()[i].first >= this->inputChans->getNumChannels()){
	    legal = false;
	}
    }

    // Check that inputs are valid
    for(int i=0; i < newInst.getInput().size(); i++){
        if(newInst.getInput()[i].first == 0){ // This input is from an input channel
            if(newInst.getInput()[i].second >= this->inputChans->getNumChannels()){
	        legal = false;
	    }
	}
	else if(newInst.getInput()[i].first == 1){ // This input is from a data register
	    if(newInst.getInput()[i].second >= this->dataRegs.getNumRegs()){
	        legal = false;
	    }
	}
    }

    // Check that outputs are valid
    for(int i=0; i < newInst.getOutput().size(); i++){
	if(newInst.getOutput()[i].first){ // This output is for an output channel
            if(newInst.getOutput()[i].second >= this->outputChans->getNumChannels()){
	        legal = false;
	    }
	}
	else{ // This output is for a data register
	    if(newInst.getOutput()[i].second >= this->dataRegs.getNumRegs()){
	        legal = false;
	    }
	}
    }


    // Only add instruction if it is actually legal
    if(legal){
        this->iMem.push_back(newInst);
    }
}

// Schedule and return a Dynamic Inst
Dynamic_TIAinstruction TIA::schedule(){
    if(this->helpChans->channels[T_WRITE]->getCanPop()){
        // Any updates of the state of TIA or to iMem
	TIAinstruction newInst = decode(this->helpChans->channels[T_WRITE]->get().data);
        this->addInstruction(newInst);	
    }

    std::vector<int> tList;
    std::vector<Dynamic_TIAinstruction> toTrig;

    // See which instructions can trigger right now
    tList = this->canTrigger();

    // If we can trigger any instructions, just trigger the first for now
    if(tList.size() > 0){
        Dynamic_TIAinstruction runInst(tList[0], static_cast<Instruction>(iMem[tList[0]].getCmd()));
	toTrig.push_back(runInst);
	this->trigger(toTrig);
	return runInst;
    }

    
    // If we can't trigger any instructions, just input a NOP
    Dynamic_TIAinstruction nop(-1, NOP);
    return nop;
};

// Execute a dynamic instruction and pass it back
Dynamic_TIAinstruction TIA::execute(Dynamic_TIAinstruction inst){
    // Instruction is not a NOP - valid index into iMem
    if(inst.getIndex() != -1){
        // Set instruction to correct iMem static inst
	TIAinstruction instruction = iMem[inst.getIndex()];

        std::vector<int> args, uncommittedResult;
	std::vector<std::bitset<1> > tags;

        // Grab the arguments from the input locations and throw them into the "args": vector
        for(int i=0; i< instruction.getInput().size(); i++){
            if(instruction.getInput()[i].first == 0){ // Get input from input channel
                args.push_back(this->inputChans->channels[instruction.getInput()[i].second]->peek().data);
		tags.push_back(this->inputChans->channels[instruction.getInput()[i].second]->peek().tag);
            }
            else if(instruction.getInput()[i].first == 1){ // Get input from Data Reg
                args.push_back(this->dataRegs.getValue(instruction.getInput()[i].second));
		tags.push_back(0);
            }
	    else{   // Get input from PredState
		std::vector<pair<int, bool> > pState;
		int predStateInt;
		pState = instruction.getPredState();
		std::bitset<32> pBits;
		for(int s=0; s< pState.size(); s++){
	            if(s< 32){
		        if (pState[s].second){
		            pBits[s] = 1;
		        }
		        else{
		            pBits[s] = 0;
		        }
		    }
		}
		predStateInt = (int)pBits.to_ulong();
		args.push_back(predStateInt);
	    }
        }


        // Run the command specified by the instruction and store it in the instruction
        switch(instruction.getCmd()){
            case PLUSPLUS:
                uncommittedResult.push_back(args[0] + 1); 
                break;
            case MINUSMINUS:
                uncommittedResult.push_back(args[0] - 1);
                break;
            case ADD:
                uncommittedResult.push_back(args[0] + args[1]);
                break;
            case SUBTRACT:
                uncommittedResult.push_back(args[0] - args[1]);
                break;
            case LESS_THAN:
                uncommittedResult.push_back((args[0] < args[1]) ? 1 : 0);
                break;
            case GREATER_THAN:
                uncommittedResult.push_back((args[0] > args[1]) ? 1 : 0);
                break;
            case LESS_THAN_EQUAL:
                uncommittedResult.push_back((args[0] <= args[1]) ? 1 : 0);
                break;
            case GREATER_THAN_EQUAL:
                uncommittedResult.push_back((args[0] >= args[1]) ? 1 : 0);
                break;
	    case PASS_THROUGH:
		uncommittedResult.push_back(args[0]);
		uncommittedResult.push_back((int)tags[0].to_ulong());
		break;
	    case PASS_THROUGH_2:
		uncommittedResult.push_back(args[0]);
		uncommittedResult.push_back((int)tags[0].to_ulong());
		uncommittedResult.push_back(args[1]);
		uncommittedResult.push_back((int)tags[1].to_ulong());
		break;
	    case DEQ:
		uncommittedResult.push_back(args[0]);
		break;
	    case SHIFT_RIGHT:
		uncommittedResult.push_back(args[0] >> args[1]);
		break;
	    case SHIFT_LEFT:
		uncommittedResult.push_back(args[0] << args[1]);
		break;
        }

        inst.setUncommittedResult(uncommittedResult);


	// Set predicate updates	
        std::vector<pair<predUpdateType, pair<int, bool> > > predUp;
	std::vector<pair<int, bool> > newPredUp;
	predUp = instruction.getPredUpdates();
	for(int h=0; h<predUp.size(); h++){
	    switch(predUp[h].first){
		case ANY: // Do Nothing
		    break;
	        case ZERO:
		    predUp[h].second.second = uncommittedResult[0] == 0 ? true : false; 
	            break;
		case LOB:
		    predUp[h].second.second = (uncommittedResult[0] & 1) == 1 ? true : false;
		    break;
		case HOB:
		    unsigned int check;
		    check = (unsigned int)uncommittedResult[0];
		    predUp[h].second.second = (check >> 31) == 1 ? true: false;
		    break;
	    }
	    newPredUp.push_back(predUp[h].second);
	}
	inst.setPredUpdates(newPredUp);
    }

    // Return the instruction so that you have the uncommitted result
    return inst; 
};

// Commit dynamic instruction
void TIA::commit(Dynamic_TIAinstruction inst){
    // Check if the instruction is a nop
    if(inst.getIndex() != -1){
        // Grab correct iMem inst
	TIAinstruction instruction = iMem[inst.getIndex()];

	struct packet p, p2;

        // Switch on the command so that you can route a variable number of results
        switch(instruction.getCmd()){
            case PLUSPLUS:
            case MINUSMINUS:
            case ADD:
            case SUBTRACT:
            case LESS_THAN:
            case GREATER_THAN:
            case LESS_THAN_EQUAL:
            case GREATER_THAN_EQUAL:
	    case DEQ:
	    case SHIFT_RIGHT:
	    case SHIFT_LEFT:
		//printf("Main Inst: %i\n", inst.getIndex());
                p.data = inst.getUncommittedResult()[0];
                p.tag = instruction.getTagUpdate();
                for(int i=0; i< instruction.getOutput().size(); i++){
                    if(instruction.getOutput()[i].first){ // Output should go to channel
            	        this->outputChans->channels[instruction.getOutput()[i].second]->put(p);
            	    }
            	    else{ // Output should go to dataRegs
            	        this->dataRegs.setValue(instruction.getOutput()[i].second, p.data);
            	        this->dataRegScore[instruction.getOutput()[i].second] = false;
            	    }
                }
                break;
	    case PASS_THROUGH:
		//printf("Pass_through Inst: %i\n", inst.getIndex());
		p.data = inst.getUncommittedResult()[0];
                p.tag = inst.getUncommittedResult()[1];
                for(int i=0; i< instruction.getOutput().size(); i++){
                    if(instruction.getOutput()[i].first){ // Output should go to channel
            	        this->outputChans->channels[instruction.getOutput()[i].second]->put(p);
            	    }
            	    else{ // Output should go to dataRegs
            	        this->dataRegs.setValue(instruction.getOutput()[i].second, p.data);
            	        this->dataRegScore[instruction.getOutput()[i].second] = false;
            	    }
                }

		break;
            case PASS_THROUGH_2:
		//printf("Pass_through_2 Inst: %i\n", inst.getIndex());
	        p.data = inst.getUncommittedResult()[0];
		p.tag = inst.getUncommittedResult()[1];
		p2.data = inst.getUncommittedResult()[2];
		p2.tag = inst.getUncommittedResult()[3];
		if(instruction.getOutput().size() == 4){
		    this->outputChans->channels[instruction.getOutput()[0].second]->put(p);
            	    this->outputChans->channels[instruction.getOutput()[1].second]->put(p2);
            	    this->outputChans->channels[instruction.getOutput()[3].second]->put(p);
            	    this->outputChans->channels[instruction.getOutput()[2].second]->put(p2);
		}
		else if(instruction.getOutput().size() == 2){
                    if(instruction.getOutput()[0].first){ // Output should go to channel
            	        this->outputChans->channels[instruction.getOutput()[0].second]->put(p);
            	    }
            	    else{ // Output should go to dataRegs
            	        this->dataRegs.setValue(instruction.getOutput()[0].second, p.data);
            	        this->dataRegScore[instruction.getOutput()[0].second] = false;
            	    }
		    if(instruction.getOutput()[1].first){ // Output should go to channel
            	        this->outputChans->channels[instruction.getOutput()[1].second]->put(p2);
            	    }
            	    else{ // Output should go to dataRegs
            	        this->dataRegs.setValue(instruction.getOutput()[1].second, p2.data);
            	        this->dataRegScore[instruction.getOutput()[1].second] = false;
            	    }
		}
		break;
        }

        
        // Dequeue any channels that the instruction commands
        if(instruction.getDequeue().size() > 0){
            std::vector<int> dqList;
            dqList = instruction.getDequeue();
            for (int i=0;i< dqList.size(); i++){
                if(dqList[i] < inputChans->getNumChannels()){
                    this->inputChans->channels[dqList[i]]->get();
            	this->inChanScore[dqList[i]] = false;
                }
            }
        }

        // Update the predicate based on the instruction commands
        if(inst.getPredUpdates().size() > 0){
            std::vector<pair<int, bool> >predUpdates;
            predUpdates = inst.getPredUpdates();
            for (int i=0; i< predUpdates.size(); i++){
                if(predUpdates[i].first < this->predRegs.getNumRegs()){
                    this->predRegs.setValue(predUpdates[i].first, predUpdates[i].second);
                }
            }
        }

	// Update the predicate scoreboard from possible predicate updates
	if(instruction.getPredUpdates().size() > 0){
	    std::vector<pair<predUpdateType, pair<int, bool> > > predUp;
	    predUp = instruction.getPredUpdates();
	    for (int i=0; i< predUp.size(); i++){
                if(predUp[i].second.first < this->predRegs.getNumRegs()){
            	    this->predScore[predUp[i].second.first] = false;
                }
            }

	}

	// Update the data regs based on the instruction commands
	if(instruction.getDataRegUpdates().size() > 0){
            std::vector<pair<int, int> >drUpdates;
            drUpdates = instruction.getDataRegUpdates();
            for (int i=0; i< drUpdates.size(); i++){
                if(drUpdates[i].first < this->dataRegs.getNumRegs()){
                    this->dataRegs.setValue(drUpdates[i].first, drUpdates[i].second);
            	    this->dataRegScore[drUpdates[i].first] = false;
                }
            }
        }

    }
};


// Decides if a TIA instruction can run
bool TIA::canRun(TIAinstruction inst){
    // Check Predicate Registers
    int index = 0;
    for(int j=0; j < inst.getPredState().size(); j++){
        index = inst.getPredState()[j].first;
	// Checks that the predicate register values are correct for an instruction
	if(this->predRegs.getValue(index) != inst.getPredState()[j].second){
	    return false;
	}
	// Checks the predicate scoreboard to see if it is in use by an in-flight instruction
	else if(this->predScore[index] == true){
	    return false;
	}
    }
    // Check the input statuses 
    for(int j=0; j < inst.getInput().size(); j++){
        index = inst.getInput()[j].second;
	if(inst.getInput()[j].first == 0){ // Input is channel
	   // Check if there is available input in the channel
	    if(!this->inputChans->channels[index]->getCanPop()){
	        return false;
	    }
	    // Check if the input channel is being used by checking the scoreboard
            else if(this->inChanScore[index] == true){
	        return false;
	    }
	}
        else if(inst.getInput()[j].first == 1){ // Input is Data Reg - check the data reg scoreboard
	    if(this->dataRegScore[index]){
	        return false;
	    }
	}
        else{ // Input is Pred State - check if any pred register is currently being used
	    for(int r=0;r<this->predRegs.getNumRegs(); r++){
	        if(this->predScore[r]){
		    return false;
		}
	    }
	}	
    }

    // Check the input flags
    for(int j=0; j < inst.getInputFlags().size(); j++){
        index = inst.getInputFlags()[j].first;
	// Check if the tag matches the required tag
	if(this->inputChans->channels[index]->peek().tag != inst.getInputFlags()[j].second){
	    return false;
	}
	// Check if the channel is already in use - scoreboard
        else if(this->inChanScore[index] == true){
	    return false;
	}
    }

    // Check the output
    for(int j=0; j < inst.getOutput().size(); j++){
        index = inst.getOutput()[j].second;
	if(inst.getOutput()[j].first){ // We are looking at an output channel -> see if we can push to it
	    if(!this->outputChans->channels[index]->getCanPush()){
	        return false;
	    }
	}
	else{ // We are looking at an output to a data reg -> check the scoreboard
	    if(this->dataRegScore[index]){
	        return false;
	    }
	}
    }
    // Check predicate updates
    for(int j=0; j < inst.getPredUpdates().size(); j++){
        index = inst.getPredUpdates()[j].second.first;
	if(this->predScore[index]){
	    return false;
	}
    }
   // Check data reg updates
    for(int j=0; j < inst.getDataRegUpdates().size(); j++){
        index = inst.getDataRegUpdates()[j].first;
	if(this->dataRegScore[index]){
	    return false;
	}
    }


    // If we pass all of these tests, then we can run the instruction
    return true;
}

// Return a TIAinstruction from common ones
// Right now this just returns a NOP
TIAinstruction TIA::decode(int index){
    std::vector<pair<int, bool> > pred;
    std::vector<pair<int, bool> > flags;
    std::vector<pair<int, int> > inC, drU;
    std::vector<pair<bool, int> > oC;
    std::vector<int> dq;
    TIAinstruction n(static_cast<Instruction>(NOP), pred, flags, inC, oC, 0, dq, drU);
    return n;
}

// Return a vector of instructions that can trigger
std::vector<int> TIA::canTrigger(){
    std::vector<int> tList;
    // For each instruction in the iMem, check if it can be run, then add it to "tList" if it can
    for(int i=0; i < iMem.size(); i++){
        if(this->canRun(iMem[i])){
	    tList.push_back(i);
	}
    } 
    return tList;
}

// Trigger instructions in tList, add all important info to scoreboards
void TIA::trigger(std::vector<Dynamic_TIAinstruction> tList){
    for(int i=0; i<tList.size(); i++){
        // Add resources to scoreboard 
	
	// Add pred regs from the pred updates to the scoreboard
        std::vector<pair<predUpdateType, pair<int, bool> > > pUp = iMem[tList[i].getIndex()].getPredUpdates();
	for(int j=0; j< pUp.size(); j++){
	    this->predScore[pUp[j].second.first] = true;
	}

	// Add data regs from the data reg updates to the scoreboard
	std::vector<pair<int, int> > drUp = iMem[tList[i].getIndex()].getDataRegUpdates();
	for(int j=0; j< drUp.size(); j++){
	    this->dataRegScore[drUp[j].first] = true;
	}


	// Add input channels that you are dequeuing to the scoreboard
        std::vector<int> dQ = iMem[tList[i].getIndex()].getDequeue();
	for(int j=0; j< dQ.size(); j++){
	    this->inChanScore[dQ[j]] = true;
	}

	// Add output that goes to datt regs to the scoreboard
	std::vector<pair<bool, int> > oList = iMem[tList[i].getIndex()].getOutput();
	for(int j=0; j< oList.size(); j++){
	    if(oList[j].first == false){ // Output is going to a data reg - add to scoreboard
	        this->dataRegScore[oList[j].second] = true;
	    }
	}
    }
}

#endif
