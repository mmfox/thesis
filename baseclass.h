///////////////////////////////////
// Base Class Header for all Modules
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////

/*
This file holds a bunch of the basic classes that are used throughout the functional and timing models.
Here is a list with basic descriptions:
1. Instruction - enum that covers basic commands that are general modules
2. Module - base class from which all modules inherit
3. PredRegs - class for TIA predicate registers
4. DataRegs - class for TIA data registers
5. TIAinstruction - class for static TIA instruction
6. Dynamic_TIAinstruction - class that inherites TIAinstruction used for in-flight TIA instructions
 */


#ifndef BASECLASS_H
#define BASECLASS_H

#include<iostream>
using namespace std;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Instruction is the basic enum that tells us which command we are running on a module
// This is general across modules

enum Instruction{
    NOP = 0,
    PUSH = 1,
    POP = 2,
    START_STREAM = 3,
    STOP_STREAM = 4, 
    STREAM = 5,
    WRITE_MEM = 6,
    MULTIPLY = 7,
    PLUSPLUS = 8,
    MINUSMINUS = 9,
    ADD = 10,
    SUBTRACT = 11, 
    LESS_THAN = 12,
    GREATER_THAN = 13,
    LESS_THAN_EQUAL = 14,
    GREATER_THAN_EQUAL = 15,
    PASS_THROUGH = 16,
    PASS_THROUGH_2 = 17,
    DEQ = 18, 
    SHIFT_RIGHT = 19,
    SHIFT_LEFT = 20
};

// Different ways that you can interpret ALU results to update the predicate
enum predUpdateType{
    ANY = 0,
    ZERO = 1,
    LOB = 2,
    HOB = 3
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Beginning of Dynamic_Instruction class


class Dynamic_Instruction{
    public:
        Dynamic_Instruction(Instruction instruction);       // Construction that takes ALU instruction
	std::vector<int> getUncommittedResult();               // Get current in-flight result
	void setUncommittedResult(std::vector<int> result);    // Set current in-flight result
	Instruction getInst();                                 // Get ALU instruction
	void setInst(Instruction i);                           // Set ALU instruction

    private:
	std::vector<int> uncommittedResult;
	Instruction inst;
};


// Constructor that takes the ALU instruction
Dynamic_Instruction::Dynamic_Instruction(Instruction instruction){ 
    std::vector<int> uR;
    this->uncommittedResult = uR; // Set some initial uncommitted result value
    this->inst = instruction;     // Set actual ALU instruction 
}

// Get current in-flight result
std::vector<int> Dynamic_Instruction::getUncommittedResult(){
    return this->uncommittedResult;
}

// Set current in-flight result
void Dynamic_Instruction::setUncommittedResult(std::vector<int> result){
    this->uncommittedResult = result;
}

// Get ALU instruction
Instruction Dynamic_Instruction::getInst(){
    return this->inst;
}

// Set ALU instruction
void Dynamic_Instruction::setInst(Instruction i){
    this->inst = i;
}

// End of Dynamic_Instruction class



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Base Module that is used across all modules

class Module{
    public:
	Instruction schedule();
	void execute();
	void commit();
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Beginning of Predicate Register Class

class PredRegs{
    private:
	int numRegs;                // Number of predicate Registers
	std::vector<bool> regs;     // Internal data struction to keep registers
    public:
	PredRegs();                                 // Empty constructor 
	bool getValue(int regID);                   // Lets you get the value of a predicate register indexed by the ID
	void setValue(int regID, bool value);       // Lets you set the value of a predicate register indexed by the ID
	void setState(std::vector<bool> newState);  // Lets you set the entire predicate register state 
	void addRegs(int num);                      // Lets you add registers (used when the TIA module's constructor is called)
	int getNumRegs();                           // Public method to see number of registers
	std::vector<bool> getState();               // Returns the entire state of the predicate state - useful for storing state in context switching
};

// Empty constructor that just sets the initial number or Registers at 0
PredRegs::PredRegs(){
    this->numRegs = 0;
}

// Lets you add registers (used when the TIA module's constructor is called)
void PredRegs::addRegs(int num){
    this->numRegs += num;
    for(int i=0; i<num; i++){
        this->regs.push_back(false); // Start all pred regs at False -- Can use setState or setValue if you don't want this
    }
}


// Lets you get the value of a predicate register indexed by the ID
bool PredRegs::getValue(int regID){
    return this->regs[regID];
}


// Lets you set the value of a predicate register indexed by the ID
void PredRegs::setValue(int regID, bool value){
    this->regs[regID] = value;
}


// Lets you set the entire predicate register state
void PredRegs::setState(std::vector<bool> newState){
    if(newState.size() == this->regs.size()){
        this->regs = newState;
    }
}


// Public method to see number of registers
int PredRegs::getNumRegs(){
    return this->numRegs;
}


// Returns the entire state of the predicate state - useful for storing state in context switching
std::vector<bool> PredRegs::getState(){
    return this->regs;
}


// End of Predicate Register Class



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Beginning of Data Register Class

class DataRegs{
    private:
	int numRegs;            // Number of Data Registers
	std::vector<int> regs;  // Internal Data struction for Data Regs
    public: 
	DataRegs();                               // Empty Constructor
	int getValue(int regID);                  // Get value of data reg by index
	void setValue(int regID, int value);      // Set value of data reg by index
	void setState(std::vector<int> newState); // Set value of entire state - good for reloading context
	void addRegs(int num);                    // Add data regs - used at TIA instantiation
	int getNumRegs();                         // Returns number of data regs
	std::vector<int> getState();              // Get entire state - useful for context switching
};


// Empty contructor used to set initial value of numRegs
DataRegs::DataRegs(){
    this->numRegs = 0;
}


// Add data regs - used at TIA instantiation
void DataRegs::addRegs(int num){
    this->numRegs += num;
    for(int i=0; i<num; i++){
        this->regs.push_back(0); // Start all data regs at 0
    }
}


// Get value of data reg by index
int DataRegs::getValue(int regID){
    return this->regs[regID];
}


// Set value of data reg by index
void DataRegs::setValue(int regID, int value){
    this->regs[regID] = value;
}


// Set value of entire state - good for reloading context
void DataRegs::setState(std::vector<int> newState){
    if(newState.size() == this->regs.size()){
        this->regs = newState;
    }
}


// Returns number of data regs
int DataRegs::getNumRegs(){
    return this->numRegs;
}


// Get entire state - useful for context switching
std::vector<int> DataRegs::getState(){
    return this->regs;
}


// End of Data Register Class



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// Beginning of TIA Instruction class

class TIAinstruction {
    public:
        TIAinstruction(Instruction command, std::vector<pair<int, bool> > pState, std::vector<pair<int, bool> > iFlags, std::vector<pair<int, int> > iChans, std::vector<pair<bool, int> >oChans, int tagUp, std::vector<int> deQ, std::vector<pair<int, int> > drUpdates);                                         // Constructor
        void setCmd(Instruction newCmd);                             // Allows you to set/change the ALU instruction that is computed in Execute
        void setPredState(std::vector<pair<int, bool> > pState);     // Sets the required predicate state for this instruction to be triggered
        void setInputFlags(std::vector<pair<int, bool> > iFlags);    // Sets the required flags of the input channels for this intruction to be triggered
        void setInput(std::vector<pair<int, int> > iChans);         // Sets which inputs the instruction uses
        void setOutput(std::vector<pair<bool, int> > oChans);        // Sets where the output from this instruction goes
        void setTagUpdate(int tag);                                  // Set the tag on outgoing messages 
        void setDequeue(int index);                                  // Add an input channel to dequeue upon finishing the instruction
        void setPredUpdates(std::vector<pair<predUpdateType, pair<int, bool> > > pUpdates); // Set updates to the Predicate Registers
        void setDataRegUpdates(std::vector<pair<int, int> > drUpdates); // Set updates to the Data Registers

        int getCmd();                                   // Get the ALU instruction that is computed in Execute 
        std::vector<pair<int, bool> > getPredState();   // Gets the required predicate state for this instruction to be triggered
        std::vector<pair<int, bool> > getInputFlags();  // Gets the required flags of the input channels for this instruction to be triggered
        std::vector<pair<int, int> > getInput();       // Gets which inputs the instruction uses
        std::vector<pair<bool, int> > getOutput();      // Gets where the output from this instruction goes 
	int getTagUpdate();                             // Get the tag for outgoing messages
	std::vector<int> getDequeue();                  // Get which channels should be dequeued upon intrunction commit
	std::vector<pair<predUpdateType, pair<int, bool> > > getPredUpdates(); // Get updates to the Predicated Registers
        std::vector<pair<int, int> > getDataRegUpdates(); // Get updates to the Data Registers

    private:
        Instruction cmd;                           // Internal ALU command
        std::vector<pair<int, bool> > predState;   // Internal predicate state
        std::vector<pair<int, bool> > inputFlags;  // Internal input Flags
        std::vector<pair<int, int> > input;       // Internal instruction input
        std::vector<pair<bool, int> > output;      // Internal instuction output
        int tagUpdate;                             // Internal instruction tag update
	std::vector<int> dequeue;                  // Internal dequeue list
	std::vector<pair<predUpdateType, pair<int, bool> > > predUpdates; // Internal list of predicate updates
	std::vector<pair<int, int> > dataRegUpdates; // Internal list of data reg updates
};


// Constructor
TIAinstruction::TIAinstruction(Instruction command, std::vector<pair<int, bool> > pState, std::vector<pair<int, bool> > iFlags, std::vector<pair<int, int> > iChans, std::vector<pair<bool,int> >oChans, int tagUp, std::vector<int> deQ, std::vector<pair<int, int> >drUpdates){
    this->cmd = command;        // Sets initial ALU command
    this->predState = pState;   // Sets initial predicate state required to trigger inst
    this->inputFlags = iFlags;  // Sets initial input flags required to trigger inst
    this->input = iChans;       // Sets input that is used for this instruction
    this->output = oChans;      // Sets output location for this instruction
    this->tagUpdate = tagUp;    // Sets tag update for this inst
    this->dequeue = deQ;        // Sets initial dequeue list
    this->dataRegUpdates = drUpdates; // Sets initial data register updates
} 

// Allows you to set/change the ALU instruction that is computed in Execute
void TIAinstruction::setCmd(Instruction newCmd){
    this->cmd = newCmd;
}

// Sets the required predicate state for this instruction to be triggered
void TIAinstruction::setPredState(std::vector<pair<int, bool> > pState){
    this->predState = pState;
}

// Sets the required flags of the input channels for this intruction to be triggered
void TIAinstruction::setInputFlags(std::vector<pair<int, bool> > iFlags){
    this->inputFlags = iFlags;
}

// Sets which inputs the instruction uses
void TIAinstruction::setInput(std::vector<pair<int, int> > iChans){
    this->input = iChans;
}

// Sets where the output from this instruction goes
void TIAinstruction::setOutput(std::vector<pair<bool, int> >oChans){
    this->output = oChans;
}

// Set the tag on outgoing messages
void TIAinstruction::setTagUpdate(int tag){
    this->tagUpdate = tag;
}

// Add an input channel to dequeue upon finishing the instruction
void TIAinstruction::setDequeue(int index){
    this->dequeue.push_back(index);
}

// Set updates to the Predicate Registers
void TIAinstruction::setPredUpdates(std::vector<pair<predUpdateType, pair<int, bool> > > pUpdates){
    this->predUpdates = pUpdates;
}

// Set updates to the Data Registers
void TIAinstruction::setDataRegUpdates(std::vector<pair<int, int> > drUpdates){
    this->dataRegUpdates = drUpdates;
}


// Get the ALU instruction that is computed in Execute
int TIAinstruction::getCmd(){
    return this->cmd;
}

// Gets the required predicate state for this instruction to be triggered
std::vector<pair<int, bool> > TIAinstruction::getPredState(){
    return this->predState;
}

// Gets the required flags of the input channels for this instruction to be triggered
std::vector<pair<int, bool> > TIAinstruction::getInputFlags(){
    return this->inputFlags;
}

// Gets which inputs the instruction uses
std::vector<pair<int, int> > TIAinstruction::getInput(){
    return this->input;
}

// Gets where the output from this instruction goes
std::vector<pair<bool, int> >TIAinstruction::getOutput(){
    return this->output;
}

// Get the tag for outgoing messages
int TIAinstruction::getTagUpdate(){
    return this->tagUpdate;
}

// Get which channels should be dequeued upon intrunction commit
std::vector<int> TIAinstruction::getDequeue(){
    return this->dequeue;
}

// Get updates to the Predicated Registers
std::vector<pair<predUpdateType, pair<int, bool> > > TIAinstruction::getPredUpdates(){
    return this->predUpdates;
}

// Get updates to the Predicated Registers
std::vector<pair<int, int> > TIAinstruction::getDataRegUpdates(){
    return this->dataRegUpdates;
}


// End of TIAinstruction Class


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Beginning of Dynamic_TIAinstruction class


class Dynamic_TIAinstruction{
    public:
        Dynamic_TIAinstruction(int index, Instruction command); // Construction that takes static TIAinstruction
	std::vector<int> getUncommittedResult();               // Get current in-flight result
	void setUncommittedResult(std::vector<int> result);    // Set current in-flight result
	std::vector<pair<int, bool> > getPredUpdates();        // Get predUpdates for this inst
	void setPredUpdates(std::vector<pair<int, bool> > pU); // Set predUpdates for this inst
	int getIndex();
	void setIndex(int in);
	Instruction getCmd();
	void setCmd(Instruction command);

    private:
	std::vector<int> uncommittedResult;
	int iMemIndex;
        std::vector<pair<int, bool> > predUpdates;
	Instruction cmd;
};


// Constructor inherits from TIAinstruction and uses its constructor
Dynamic_TIAinstruction::Dynamic_TIAinstruction(int index, Instruction command){ 
    std::vector<int> uR;
    this->uncommittedResult = uR;  // Set some initial uncommitted result value
    this->iMemIndex = index;       // Set correct pointer into iMem
    this->cmd = command;
}

// Set predicate updates for this inst
void Dynamic_TIAinstruction::setPredUpdates(std::vector<pair<int, bool> > pU){
    this->predUpdates = pU;
}

// Get updates to the Predicated Registers
std::vector<pair<int, bool> > Dynamic_TIAinstruction::getPredUpdates(){
    return this->predUpdates;
}


// Get current in-flight result
std::vector<int> Dynamic_TIAinstruction::getUncommittedResult(){
    return this->uncommittedResult;
}

// Set current in-flight result
void Dynamic_TIAinstruction::setUncommittedResult(std::vector<int> result){
    this->uncommittedResult = result;
}

// Get index of iMem inst
int Dynamic_TIAinstruction::getIndex(){
    return this->iMemIndex;
}

// Set index of iMem inst
void Dynamic_TIAinstruction::setIndex(int in){
    this->iMemIndex = in;
}

Instruction Dynamic_TIAinstruction::getCmd(){
    return this->cmd;
}

void Dynamic_TIAinstruction::setCmd(Instruction command){
    this->cmd = command;
}

// End of Dynamic_TIAinstruction class


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
