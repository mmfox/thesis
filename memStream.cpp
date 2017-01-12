/////////////////////////////////
// Memory Streaming Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

/*
This is the module for the memory streaming module.
It grabs a continuous stream of data starting at the currentBaseAddr
and ending at the currentEndAddr.  We can also set the stride between 
memory accesses using currentStride. Furthermore, we can set the 
streamer to either write or read at initialization using the write
flag and can set the size of the memory accessed using size.
*/

#ifndef MEMSTREAM_CPP
#define MEMSTREAM_CPP

#include"memstream.h"

// Defines for channel locations in the multichannel
#define MEM_START 0
#define MEM_OUT 1
#define MEM_DATA 2
#define MEM_BASE 3
#define MEM_STRIDE 4
#define MEM_END 5

//Constructor
MemStream::MemStream(multiChannel * channelsIn, int beginAddr, int beginStride, int beginEndAddr, bool write, int updateTag, int * memory){
    this->c = channelsIn;                // Multichannel for all liChannels in the module
    this->currentBaseAddr = beginAddr;   // Set the starting base addr
    this->currentAddr = beginAddr;       // Set the initial current addr, updates while running
    this->currentStride = beginStride;   // Set the initial stride
    this->currentEndAddr = beginEndAddr; // Set the current end addr
    this->running = false;               // The module must be prompted to start, so it begins in the not running state
    this->writeMem = write;              // Set whether we write or read
    this->tagUpdate = updateTag;         // Set what the updateTag is if reading from memory
    this->mainMemory = memory;           // Initialize main memory
};

// Schedule a push or pull from memory
Dynamic_Instruction MemStream::schedule(){
    // First, check if we have a command to start or stop our streamer
    if(this->c->channels[MEM_START]->getCanPop()){
	// Grab the command from the channel    
	Instruction command = static_cast<Instruction>(this->c->channels[MEM_START]->get().data);

	// Set our local variable "running" to reflect the command we were given
	if(command == START_STREAM){
	    this->running = true;
	}
	else if(command == STOP_STREAM){
	    this->running = false;
	}
    }

    // Check and set updates if we have updates to our base, stride, and end addresses -- all 3 must be updated together!
    if(this->c->channels[MEM_BASE]->getCanPop() && this->c->channels[MEM_STRIDE]->getCanPop() && this->c->channels[MEM_END]->getCanPop()){
	this->currentAddr = this->currentBaseAddr;
	this->currentBaseAddr = this->c->channels[MEM_BASE]->get().data;
	this->currentStride = this->c->channels[MEM_STRIDE]->get().data;
	this->currentEndAddr = this->c->channels[MEM_END]->get().data;

    }


    // If we are a reader, make sure we are running, haven't passed our end addres, and can write out the data we grab
    if(this->c->channels[MEM_OUT]->getCanPush() && (this->currentAddr < this->currentEndAddr) && this->running == true && this->writeMem == false){
        Dynamic_Instruction d(STREAM);
	return d;
    }
    // If we are a writer, make sure we are running, havend passed our end address, and we can grab the data we need to write
    if(this->c->channels[MEM_DATA]->getCanPop() && this->currentAddr < this->currentEndAddr && this->running == true && this->writeMem == true){
	Dynamic_Instruction d(WRITE_MEM);
        return d;
    }
    // If we can't read/write, return a NOP
    Dynamic_Instruction d(NOP);
    return d;
};

// Send another memory value or receive another memory value to write back
Dynamic_Instruction MemStream::execute(Dynamic_Instruction instruction){
    std::vector<int> uR;
    if(instruction.getInst() == STREAM){
        uR.push_back(this->mainMemory[this->currentAddr]);  // Grab our data from main memory
        instruction.setUncommittedResult(uR);
	this->currentAddr += this->currentStride;                   // Increment current address
    }
    else if(instruction.getInst() == WRITE_MEM){
	uR.push_back(this->c->channels[MEM_DATA]->get().data); // Get the data from the channel
        instruction.setUncommittedResult(uR);
	this->writeAddress = this->currentAddr;                        // Grab address to write to
	this->currentAddr += this->currentStride;                      // Increment current address
    }
    return instruction;
};

// Commit the push or pull from memory
void MemStream::commit(Dynamic_Instruction instruction){
    if(instruction.getInst() == STREAM){
	struct packet send;                    // Create a packet to send to the system
	send.data = instruction.getUncommittedResult()[0];       // Input the data from memory to the packet
	send.tag = this->tagUpdate;            // Input the correct tag to the packet
	this->c->channels[MEM_OUT]->put(send); // Send out the packet
    }
    else if(instruction.getInst() == WRITE_MEM){
        this->mainMemory[this->writeAddress] = instruction.getUncommittedResult()[0];  // Write our data to mainMemory
    }
};


#endif
