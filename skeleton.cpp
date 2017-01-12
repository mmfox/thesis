///////////////////////////////////
// Skeleton Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 4/13/15
//////////////////////////////////

/*
This is a skeleton module to show how to add more modules in the future.
*/

#ifndef SKELETON_CPP
#define SKELETON_CPP

#include <iostream>
using namespace std;

#include "baseclass.cpp"
#include "skeleton.h"

// Defines for liChannel locations in the multiChannel 
// Example would be 
// #define SKELETON_IN 0

// Constructor that assigns local multiChannel and other input variables 
Skeleton::Skeleton(multiChannel * channelsIn){
    this->channels = channelsIn;
};

// Schedule an instruction for your module - returns a Dynamic_Instruction (class defined in baseclass.h)
Dynamic_Instruction Skeleton::schedule(){
    // By default it should return a NOP
    Dynamic_Instruction d(NOP);
    return d;
};

// Execute a dynamic_instruction
Dynamic_Instruction Skeleton::execute(Dynamic_Instruction instruction){
    // Write any intermediate result to the uncommitted result vector in the Dynamic_instruction and then return it
    return instruction;
};

// Commit whatever command you executed
void Skeleton::commit(Dynamic_Instruction instruction){
};

#endif
