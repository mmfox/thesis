///////////////////////////////////
// Timing Model
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 11/18/14
//////////////////////////////////
#ifndef TIMING2_CPP
#define TIMING2_CPP

/*
This is the simple timing model that accompanies
the functional model.  It provides a skeleton
for a more complex timing model to be implemented
and changed in the future.  A global timing count
is kept and returned on completion of important 
tasks.
*/


#include"timingModel.h"
using namespace std;

// These defines are used to index into the parsed csv file to find cycle times for the different modules.

#define TIA_SCHEDULE_TIME 4
#define TIA_COMMIT_TIME 6
#define MULT_SCHEDULE_TIME 40
#define MULT_COMMIT_TIME 42
#define MEM_SCHEDULE_TIME 46
#define MEM_COMMIT_TIME 48
#define FIFO_SCHEDULE_TIME 61
#define FIFO_COMMIT_TIME 63

// Define size of main memory or memories
#define MAIN_MEM_SIZE 1024


int main(){

    // Define ifstream to parse the cycle times csv file	
    std::ifstream data("cTimes.csv");

    // Seek to the beginning of the csv file and create an internal vector to keep the cycle times
    data.seekg(0, data.beg);
    std::vector<int> cTimes, tempCSV;

    // Basic csv processing that stores the values in the cTimes vector
    std::string line;
    while(std::getline(data,line)){
        std::stringstream  lineStream(line);
	std::string        cell;
	while(std::getline(lineStream,cell,',')){
	        tempCSV.push_back(atoi(cell.c_str()));
        }
    }
 
    for(int w=0; w<tempCSV.size(); w++){
        //printf("index: %i :: value: %i\n", w, tempCSV[w]);   
    }
    cTimes = tempCSV;

    // Maps times for tia Execute into indexes into cTimes
    std::map<int, int> tiaTimeMap, multTimeMap, memStreamTimeMap, fifoTimeMap;
    tiaTimeMap[NOP] = 5;
    tiaTimeMap[PLUSPLUS] = 8;
    tiaTimeMap[MINUSMINUS] = 11;
    tiaTimeMap[ADD] = 14;
    tiaTimeMap[SUBTRACT] = 17;
    tiaTimeMap[LESS_THAN] = 20;
    tiaTimeMap[GREATER_THAN] = 23;
    tiaTimeMap[LESS_THAN_EQUAL] = 26;
    tiaTimeMap[GREATER_THAN_EQUAL] = 29;
    tiaTimeMap[PASS_THROUGH] = 32;
    tiaTimeMap[PASS_THROUGH_2] = 35;
    tiaTimeMap[DEQ] = 38;
    tiaTimeMap[SHIFT_RIGHT] = 71;
    tiaTimeMap[SHIFT_LEFT] = 74;

    multTimeMap[NOP] = 41;
    multTimeMap[MULTIPLY] = 44;

    memStreamTimeMap[NOP] = 47;
    memStreamTimeMap[START_STREAM] = 50;
    memStreamTimeMap[STOP_STREAM] = 53;
    memStreamTimeMap[STREAM] = 56;
    memStreamTimeMap[WRITE_MEM] = 59;

    fifoTimeMap[NOP] = 62;
    fifoTimeMap[PUSH] = 65;
    fifoTimeMap[POP] = 68;



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Initializing the TIA module

    multiChannel tiaHelp, tiaIn, tiaOut;
    liChannel tOut1(1), tHelp1(1), tiaToMult1(1), tiaToMult2(1);
    liChannel multOut(1), in(1), factors(1);

    // Channels used in the creation of a memory streamer

    tiaIn.add(&in);
    tiaIn.add(&factors);

    tiaOut.add(&tiaToMult1);
    tiaOut.add(&tiaToMult2);

    tiaHelp.add(&tHelp1);

    TIA t1(&tiaHelp, &tiaIn, &tiaOut, 3, 3); 

    // Push image and filter with proper tags to the TIA input channels
    
    packet i1, i2, i3, i4, i5, i6, i7, i8;
    i1.data = 3;
    i1.tag = 0;
    i2.data = 2;
    i2.tag = 0;
    i3.data = -2;
    i3.tag = 0;
    i4.data = -1;
    i4.tag = 0;
    i5.data = 0;
    i5.tag = 0;
    i6.data = 1;
    i6.tag = 0;
    i7.data = 2;
    i7.tag = 0;
    i8.data = 3;
    i8.tag = 0;

    
    factors.put(i1);
    factors.put(i2);
    in.put(i3);
    in.put(i4);
    in.put(i5);
    in.put(i6);
    in.put(i7);
    in.put(i8);
    factors.setCanPop(true);
    in.setCanPop(true);

//////////////////////////////////////////////////////////////////

    // Load factor 0 into data reg 0
    std::vector<pair<int, bool> > flagsEmpty;
    std::vector<pair<int, int> > inputF0, druEmpty, druF1;
    std::vector<pair<bool, int> > outputF0;
    std::vector<int> dqChan, dqFact;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpF0; 


    std::vector<pair<int, bool> > predStateF0, predStateF1, predStateLT, predStateG, predStateL;

    std::pair<int, bool> pf01, pf02, pf03;
    pf01.first = 0;
    pf01.second = false;
    pf02.first = 1;
    pf02.second = false;
    pf03.first = 2;
    pf03.second = false;
    predStateF0.push_back(pf01);
    predStateF0.push_back(pf02);
    predStateF0.push_back(pf03);

    std::pair<int, int> inputF0p;
    inputF0p.first = 0;
    inputF0p.second = 1;
    inputF0.push_back(inputF0p);

    std::pair<bool, int> outputF0p;
    outputF0p.first = false;
    outputF0p.second = 0;
    outputF0.push_back(outputF0p);

    dqFact.push_back(1);
    dqChan.push_back(0);

    std::pair<int, bool> pUpF0p;
    std::pair<predUpdateType, pair<int, bool> > pf0p1;
    pUpF0p.first = 0;
    pUpF0p.second = true;
    pf0p1.first = ANY;
    pf0p1.second = pUpF0p;
    pUpF0.push_back(pf0p1);

    TIAinstruction factor0(static_cast<Instruction>(PASS_THROUGH), predStateF0, flagsEmpty, inputF0, outputF0, 0, dqFact, druEmpty);
    factor0.setPredUpdates(pUpF0);
    t1.addInstruction(factor0);


    // Load factor 1 into data reg 1
    
    std::vector<pair<int, int> > inputF1, inputLT, inputG, inputL;
    std::vector<pair<bool, int> > outputF1, outputEmpty, outputG, outputL;
    std::vector<int> dqEmpty;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpF1, pUpLT, pUpG, pUpL; 

    std::pair<int, bool> pf11, pf12, pf13;
    pf11.first = 0;
    pf11.second = true;
    pf12.first = 1;
    pf12.second = false;
    pf13.first = 2;
    pf13.second = false;
    predStateF1.push_back(pf11);
    predStateF1.push_back(pf12);
    predStateF1.push_back(pf13);



    std::pair<int, int> inputF1p;
    inputF1p.first = 0;
    inputF1p.second = 1;
    inputF1.push_back(inputF1p);


    std::pair<bool, int> outputF1p;
    outputF1p.first = false;
    outputF1p.second = 1;
    outputF1.push_back(outputF1p);


    std::pair<int, bool> pUpF1p;
    std::pair<predUpdateType, pair<int, bool> > pf1p1;
    pUpF1p.first = 1;
    pUpF1p.second = true;
    pf1p1.first = ANY;
    pf1p1.second = pUpF1p;
    pUpF1.push_back(pf1p1);

    std::pair<int, int> dru0;
    dru0.first = 2;
    dru0.second = 0;
    druF1.push_back(dru0);


    TIAinstruction factor1(static_cast<Instruction>(PASS_THROUGH), predStateF1, flagsEmpty, inputF1, outputF1, 0, dqFact, druF1);
    factor1.setPredUpdates(pUpF1);
    t1.addInstruction(factor1);

    // Checks if it is less than 0
    
    std::pair<int, bool> plt1, plt2, plt3;
    plt1.first = 0;
    plt1.second = true;
    plt2.first = 1;
    plt2.second = true;
    plt3.first = 2;
    plt3.second = false;
    predStateLT.push_back(plt1);
    predStateLT.push_back(plt2);
    predStateLT.push_back(plt3);



    std::pair<int, int> inputLTp, inputLTp2;
    inputLTp.first = 0;
    inputLTp.second = 0;
    inputLTp2.first = 1;
    inputLTp2.second = 2;
    inputLT.push_back(inputLTp);
    inputLT.push_back(inputLTp2);


    std::pair<int, bool> pUpLTp, pUpLTp2;
    std::pair<predUpdateType, pair<int, bool> > pltp1, pltp2;
    pUpLTp.first = 2;
    pUpLTp.second = true;
    pltp1.first = ZERO;
    pltp1.second = pUpLTp;

    pUpLTp2.first = 0;
    pUpLTp2.second = true;
    pltp2.first = ZERO;
    pltp2.second = pUpLTp2;

    pUpLT.push_back(pltp1);
    pUpLT.push_back(pltp2);

    
    TIAinstruction choose(static_cast<Instruction>(LESS_THAN), predStateLT, flagsEmpty, inputLT, outputEmpty, 0, dqEmpty, druEmpty);
    choose.setPredUpdates(pUpLT);
    t1.addInstruction(choose);

    // Compute the answer if we are greater than zero

    std::pair<int, bool> pg1, pg2, pg3;
    pg1.first = 0;
    pg1.second = true;
    pg2.first = 1;
    pg2.second = true;
    pg3.first = 2;
    pg3.second = true;
    predStateG.push_back(pg1);
    predStateG.push_back(pg2);
    predStateG.push_back(pg3);

    std::pair<int, int> inputGp, inputGp2;
    inputGp.first = 0;
    inputGp.second = 0;
    inputGp2.first = 1;
    inputGp2.second = 1; // Grab F1
    inputG.push_back(inputGp);
    inputG.push_back(inputGp2);

    std::pair<bool, int> outputGp, outputGp2;
    outputGp.first = true;
    outputGp.second = 0;
    outputGp2.first = true;
    outputGp2.second = 1;
    outputG.push_back(outputGp);
    outputG.push_back(outputGp2);


    std::pair<int, bool> pUpGp;
    std::pair<predUpdateType, pair<int, bool> > pgp1;
    pUpGp.first = 2;
    pUpGp.second = false;
    pgp1.first = ANY;
    pgp1.second = pUpGp;
    pUpG.push_back(pgp1);


    TIAinstruction over(static_cast<Instruction>(PASS_THROUGH_2), predStateG, flagsEmpty, inputG, outputG, 0, dqChan, druEmpty);
    over.setPredUpdates(pUpG);
    t1.addInstruction(over);


    // Compute the answer if we are less than zero

    std::pair<int, bool> pl1, pl2, pl3;
    pl1.first = 0;
    pl1.second = false;
    pl2.first = 1;
    pl2.second = true;
    pl3.first = 2;
    pl3.second = false;
    predStateL.push_back(pl1);
    predStateL.push_back(pl2);
    predStateL.push_back(pl3);

    std::pair<int, int> inputLp, inputLp2;
    inputLp.first = 0;
    inputLp.second = 0;
    inputLp2.first = 1;
    inputLp2.second = 0; // Grab F0
    inputL.push_back(inputLp);
    inputL.push_back(inputLp2);

    std::pair<bool, int> outputLp, outputLp2;
    outputLp.first = true;
    outputLp.second = 0;
    outputLp2.first = true;
    outputLp2.second = 1;
    outputL.push_back(outputLp);
    outputL.push_back(outputLp2);


    std::pair<int, bool> pUpLp;
    std::pair<predUpdateType, pair<int, bool> > plp1;
    pUpLp.first = 0;
    pUpLp.second = true;
    plp1.first = ANY;
    plp1.second = pUpLp;
    pUpL.push_back(plp1);


    TIAinstruction under(static_cast<Instruction>(PASS_THROUGH_2), predStateL, flagsEmpty, inputL, outputL, 0, dqChan, druEmpty);
    under.setPredUpdates(pUpL);
    t1.addInstruction(under);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    //Initializing Multiplier module

    
    // Create and populate the multiChannel that will go into the multiplier
    multiChannel multIn;

    multIn.add(&multOut);
    multIn.add(&tiaToMult1);
    multIn.add(&tiaToMult2);


    Multiplier m(&multIn, 0);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Create vectors of all modules so that we can run them in our timing model main loop


    std::vector<MemStream *> mems;
    std::vector<Multiplier *> mults;
    std::vector<Fifo *> fifos;
    std::vector<liChannel *> channels;
    std::vector<TIA *> tias;

    // Add all modules to the proper vectors

    mults.push_back(&m);
    tias.push_back(&t1);


    channels.push_back(&in);
    channels.push_back(&factors);
    channels.push_back(&tiaToMult1);
    channels.push_back(&tiaToMult2);
    channels.push_back(&multOut);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Define temp variables and totalTime = the system time for the timing model 
    int totalTime = 0, cycleTime = 0, totalMemTime = 0, totalFifoTime = 0, totalMultTime = 0, totalTIAtime = 0, totalMemOverhead = 0, totalFifoOverhead = 0, totalMultOverhead = 0, totalTIAOverhead = 0;
   
    // Add up all the times to find our timing bottleneck to see our total cycle time 
    totalMemOverhead = cTimes[MEM_SCHEDULE_TIME] + cTimes[MEM_COMMIT_TIME];
    totalFifoOverhead = cTimes[FIFO_SCHEDULE_TIME] + cTimes[FIFO_COMMIT_TIME];
    totalMultOverhead = cTimes[MULT_SCHEDULE_TIME] + cTimes[MULT_COMMIT_TIME];
    totalTIAOverhead = cTimes[TIA_SCHEDULE_TIME] + cTimes[TIA_COMMIT_TIME];

    int maxMemTime = 0, maxFifoTime = 0, maxMultTime = 0, maxTIAtime = 0;

    // Main loop for the timing model - set off all modules in a loop like fashion
    while(1){
	// Correctly set all canPop and canPush variables for all channels based on channel properties and the last cycle
        for (unsigned i = 0; i < channels.size(); i++){
	    if(channels[i]->numInFlight() == 0){
	        channels[i]->setCanPop(false);
		channels[i]->setCanPush(true);
	    }
	    else if(channels[i]->numInFlight() == channels[i]->getMax()){
	        channels[i]->setCanPop(true);
		channels[i]->setCanPush(false);
	    }
	    else{
	        channels[i]->setCanPop(true);
		channels[i]->setCanPush(true);
	    }
	}
	maxMemTime = 0;
	maxFifoTime = 0;
	maxMultTime = 0;
	maxTIAtime = 0;

	Instruction whichMem = NOP, whichFifo = NOP, whichMult = NOP, whichTIA = NOP;

        // Set off any module that can schedule, execute, and commit

        for (unsigned i = 0; i < mems.size(); i++){
	    Dynamic_Instruction sResult = mems[i]->schedule();
	    Dynamic_Instruction eResult = mems[i]->execute(sResult);
            if(cTimes[memStreamTimeMap[eResult.getInst()]] > maxMemTime){
	        maxMemTime = cTimes[memStreamTimeMap[eResult.getInst()]];
		whichMem = eResult.getInst();
	    }
	    mems[i]->commit(eResult);
	}

	for(unsigned i = 0; i < fifos.size(); i++){
	    Dynamic_Instruction sResult = fifos[i]->schedule();
	    Dynamic_Instruction eResult = fifos[i]->execute(sResult);
            if(cTimes[fifoTimeMap[eResult.getInst()]] > maxFifoTime){
	        maxFifoTime = cTimes[fifoTimeMap[eResult.getInst()]];
		whichFifo = eResult.getInst();
	    }
	    fifos[i]->commit(eResult);
	}

	for(unsigned i=0; i < mults.size(); i++){
	    Dynamic_Instruction sResult = mults[i]->schedule();
            Dynamic_Instruction eResult = mults[i]->execute(sResult);
            if(cTimes[multTimeMap[eResult.getInst()]] > maxMultTime){
	        maxMultTime = cTimes[multTimeMap[eResult.getInst()]];
		whichMult = eResult.getInst();
	    }
	    mults[i]->commit(eResult);
	}

        for(unsigned i=0; i < tias.size(); i++){
	    Dynamic_TIAinstruction sResult = tias[i]->schedule();
            Dynamic_TIAinstruction eResult = tias[i]->execute(sResult);
	    if(cTimes[tiaTimeMap[eResult.getCmd()]] > maxTIAtime){
	        maxTIAtime = cTimes[tiaTimeMap[eResult.getCmd()]];
		whichTIA = eResult.getCmd();
	    }
	    tias[i]->commit(eResult);
	}
        if(multOut.getCanPop()){
	        printf("Output: %i\n", multOut.get().data);
		printf("Number of Cycles: %i\n", totalTime);
	}

	totalMemTime = totalMemOverhead + maxMemTime;
	totalFifoTime = totalFifoOverhead + maxFifoTime;
	totalMultTime = totalMultOverhead + maxMultTime;
	totalTIAtime = totalTIAOverhead + maxTIAtime;

	cycleTime = 0;
	// Calculate total time spent for cycle so that we can keep system time
        cycleTime = max(totalMemTime, totalFifoTime);
        cycleTime = max(cycleTime, totalMultTime);
        cycleTime = max(cycleTime, totalTIAtime);

	if(whichMem != NOP){
	    //printf("Mem Inst: %i\n", whichMem);
	}
	if(whichFifo != NOP){	
	    //printf("Fifo Inst: %i\n", whichFifo);
	}
	if(whichMult != NOP){
	    //printf("Mult Inst: %i\n", whichMult);
	}
	if(whichTIA != NOP){
	    //printf("TIA Inst: %i\n", whichTIA);
	}

	//printf("TIAtoMult1 can Pop: %d\n", tiaToMult1.getCanPop());
        //printf("TIAtoMult2 can Pop: %d\n", tiaToMult2.getCanPop());



	// Add a cycle's worth of time to the total time
	totalTime += cycleTime;
    
    }

    return 0;
}



#endif
