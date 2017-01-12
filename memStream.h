/////////////////////////////////
// Memory Streaming Module Header File
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

#ifndef MEMSTREAM_H
#define MEMSTREAM_H

#include<iostream>
using namespace std;

#include"baseclass.cpp"
#include"liChannel.h"
#include"liChannel.cpp"
#include"multiChannel.cpp"

class MemStream : public Module {
    public:
	MemStream(multiChannel * channelsIn, int beginAddr, int beginStride, int beginEndAddr, bool write, int updateTag, int * memory);  // Constructor 
	multiChannel * c;                       // Multichannel for all liChannels in the module
	Dynamic_Instruction execute(Dynamic_Instruction instruction);  // Send another memory value or receive another memory value to write back
	Dynamic_Instruction schedule();                 // Schedule a push or pull from memory
	void commit(Dynamic_Instruction instruction);   // Commit the push or pull from memory
	int * mainMemory;                       // Create a memory to use for this streamer - Do we actually want to do this
	int writeAddress;                       // Keeps correct location to write 
	int currentAddr;                        // the next memory location we will read
	int currentBaseAddr;                    // the beginning of our current stream
	int currentStride;                      // the stride of our current stream
	int currentEndAddr;                     // the end of our current stream
	bool running;                           // bool which states if we are currently running or not
	bool writeMem;                          // bool whcih determines if we are a writing or reading memStreamer
	int tagUpdate;                          // If we are writing out, what should the tag be?
};

#endif
