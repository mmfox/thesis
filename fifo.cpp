///////////////////////////////////
// Fifo Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

#ifndef FIFO_CPP
#define FIFO_CPP

#include"fifo.h"


// Here, we define the order that the channels have been inputted into the multichannel
// It is paramount that the timing model is consistent with the module in this definition
#define F_OUT 0
#define F_DATA 1


// Constructor takes in multiChannel pointer and size of fifo
Fifo::Fifo(multiChannel * channelsIn, int size){
    this->c = channelsIn;             // Set internal multichannel
    this->nextIndex = 0;              // Write pointer starts at beginning
    this->baseIndex = 0;              // Read pointer starts at begging
    this->endLooped = false;          // Writer pointer loop flag starts as false
    this->beginLooped = false;        // Read pointer loop flag starts as false
    this->data = new packet [size];   // Internal data storage
    memset(data, 0, size);            // Zero out initial fifo
};

// Method that checks if fifo is not full
bool Fifo::notFull(){
    // The fifo is full if the write pointer = read pointer, and they are on the same loop
    // If they have different loop flags, then the fifo is actually empty
    if(this->nextIndex == this->baseIndex && this->endLooped != this->beginLooped){
    	return false;
    }
    return true;
};

// Method that check if fifo is not empty
bool Fifo::notEmpty(){
    // The fifo is empty if the write pointer = read pointer, and they are on the same loop
    // If they have different loop flags, then the fifo is actually full
    if(this->nextIndex == this->baseIndex && this->endLooped == this->beginLooped){
        return false;
    }
    return true;
};

// Schedules a push or pop if possible
Dynamic_Instruction Fifo::schedule(){
    // If we have incoming data and space in our fifo, we can push
    if(this->c->channels[F_DATA]->getCanPop() && this->notFull()){
	Dynamic_Instruction d(PUSH);
	return d;
    }
    // If we can push to our output channel and we have data, we can pop
    else if(this->c->channels[F_OUT]->getCanPush() && this->notEmpty()){
	Dynamic_Instruction d(POP);
	return d;
    }
    // If we can do neither, issue a NOP 
    Dynamic_Instruction d(NOP);
    return d;
};

// Executes the push or pop
Dynamic_Instruction Fifo::execute(Dynamic_Instruction instruction){
    std::vector<int> uR;
    if(instruction.getInst() == PUSH){
	packet out = this->c->channels[F_DATA]->get();
        uR.push_back(out.data); // Grab the data but don't add it to the Fifo yet
        uR.push_back((int)out.tag.to_ulong()); 
    } else if(instruction.getInst() == POP){
        uR.push_back(this->data[baseIndex].data); // Grab the data but don't move the pointers yet
        uR.push_back((int)this->data[baseIndex].tag.to_ulong());
    }
    instruction.setUncommittedResult(uR);
    return instruction;
};

// Commits the push or pop that is in-flight
void Fifo::commit(Dynamic_Instruction instruction){
    if(instruction.getInst() == PUSH){
	packet out;
	out.data = instruction.getUncommittedResult()[0];
	out.tag = instruction.getUncommittedResult()[1];
        this->data[nextIndex] = out;  // Write to our circular buffer
	this->nextIndex++;                          // Increment write pointer

	// If we hit the end of the circular buffer, reset the write pointer and flip the loop flag
	if(this->nextIndex == sizeof(this->data)){  
	    this->nextIndex = 0;
	    this->endLooped = !this->endLooped;
	}
    }else if(instruction.getInst() == POP){
	packet out;
	out.data = instruction.getUncommittedResult()[0];
	out.tag = instruction.getUncommittedResult()[1];
        this->c->channels[F_OUT]->put(out); // Write to our output channel
	this->baseIndex++;                                 // Increment read pointer

        // If we hit the end of the circular buffer, reset the write pointer and flip the loop flag
	if(this->baseIndex == sizeof(this->data)){
	    this->baseIndex = 0;
	    this->beginLooped = !this->beginLooped;
	}
    }
};


#endif
