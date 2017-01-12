///////////////////////////////////
// Multpilier Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

/*
This is the module for a basic multiplier.
It takes two inputs and outputs their product.
*/

#ifndef MULTIPLIER_CPP
#define MULTIPLIER_CPP

#include <iostream>
using namespace std;

#include "baseclass.cpp"
#include "multiplier.h"

// Defines for liChannel locations in the multiChannel 
#define M_OUT 0
#define M_MULT1 1
#define M_MULT2 2

// Construction that assings local multiChannel
Multiplier::Multiplier(multiChannel * channelsIn, int updateTag){
    this->c = channelsIn;
    this->tagUpdate = updateTag;
};

// Schedule a multiply command
Dynamic_Instruction Multiplier::schedule(){
    // We need to check if we can push to our output channel and grab data from both inputs
    if(this->c->channels[M_OUT]->getCanPush() && this->c->channels[M_MULT1]->getCanPop() && this->c->channels[M_MULT2]->getCanPop()){
        Dynamic_Instruction d(MULTIPLY);
	return d;
    }
    // Otherwise insert a NOP
    Dynamic_Instruction d(NOP);
    return d;
};

// Execute a multiply command
Dynamic_Instruction Multiplier::execute(Dynamic_Instruction instruction){
    // Calculate the product of the two inputs
    if(instruction.getInst() == MULTIPLY){
	std::vector<int> uR;
	struct packet p;
	p = this->c->channels[M_MULT1]->get();
	uR.push_back(p.data * this->c->channels[M_MULT2]->get().data);
	uR.push_back((int)p.tag.to_ulong());
        instruction.setUncommittedResult(uR);
    }
    return instruction;
};

// Create a packet and send it to the output
void Multiplier::commit(Dynamic_Instruction instruction){
    if(instruction.getInst() == MULTIPLY){
	struct packet product;
	product.data = instruction.getUncommittedResult()[0];
	product.tag = instruction.getUncommittedResult()[1];
	this->c->channels[M_OUT]->put(product);
    }
};

#endif
