///////////////////////////////////
// Fifo Module Header File
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

/*
This is the header file for the FIFO.  
It includes the other modules and defines the methods.
*/

#ifndef FIFO_H
#define FIFO_H


#include<iostream>
using namespace std;

#include "baseclass.cpp"
#include "baseclass.h"
#include "liChannel.cpp"
#include "multiChannel.cpp"

class Fifo : public Module {
    public:
        Fifo(multiChannel * channelsIn, int size); // Constructor takes in multiChannel pointer and size of fifo
	Dynamic_Instruction schedule();                    // Schedules a push or pop if possible
	Dynamic_Instruction execute(Dynamic_Instruction instruction);     // Executes the push or pop
	void commit(Dynamic_Instruction instruction);      // Commits the push or pop that is in-flight
	bool notFull();                            // Method that checks if fifo is not full
	bool notEmpty();                           // Method that check if fifo is not empty

    private:
	int nextIndex;        // Internal next available slot in circular buffer
	int baseIndex;        // Internal first element to push out
	bool endLooped;       // Flag for if the nextIndex has looped the circular buffer an even or odd number of times
	bool beginLooped;     // Flag for if the baseIndex has looped the circular buffer an even or odd number of times
	// Some internal packet representations
	packet * data;        
	multiChannel * c; // Pointer to the channels for the fifo module
};


#endif
