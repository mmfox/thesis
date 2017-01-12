///////////////////////////////////
// Timing Model
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 11/18/14
//////////////////////////////////
#ifndef TIMING_CPP
#define TIMING_CPP

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

    // Maps times for tia Execute into indexes into cTimes - will change if cTimes.csv changes!!
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
    liChannel in(10);

    // Channels used in the creation of a memory streamer

    tiaIn.add(&in);

    tiaOut.add(&tOut1);

    tiaHelp.add(&tHelp1);

    TIA t1(&tiaHelp, &tiaIn, &tiaOut, 3, 4); 

    // Push image and filter with proper tags to the TIA input channels
    
    packet i1, i2, i3, i4, i5, i6, i7, i8;
    //i1.data = 0;
    //i1.tag = 0;
    //i2.data = 0;
    //i2.tag = 0;
    //i3.data = 0;
    //i3.tag = 0;
    //i4.data = 0;
    //i4.tag = 0;
    i5.data = 42163;
    i5.tag = 1;

    
    //in.put(i1);
    //in.put(i2);
    //in.put(i3);
    //in.put(i4);
    in.put(i5);
    in.setCanPop(true);

//////////////////////////////////////////////////////////////////

    // Load 0 into data regs 0, 1, and 2

    std::vector<pair<int, bool> > flagsEmpty, flagsGT, flagsGT2;
    std::vector<pair<int, int> > inputLD, druEmpty, druLD, inputCM, inputGT, inputDQD, inputIC, inputSUB, inputGTE, inputQA;
    std::vector<pair<bool, int> > outputEmpty, outputGT, outputCMD, outputIC, outputF, outputQA;
    std::vector<int> dqEmpty, dqChan, dqGT;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpLD, pUpGT, pUpIC, pUpICD, pUpSUB, pUpGTE, pUpQA, pUpF, pUpGT2; 
    std::vector<pair<int, bool> > predStateLD, predStateGT, predStateIC, predStateICD, predStateSUB, predStateGTE, predStateQA, predStateF;
    
    
    std::pair<int, int> inputLDp;
    inputLDp.first = 0;
    inputLDp.second = 0;
    inputLD.push_back(inputLDp);

    std::pair<int, bool> pld0, pld1, pld2;
    pld0.first = 0;
    pld0.second = false;
    pld1.first = 1;
    pld1.second = false;
    pld2.first = 2;
    pld2.second = false;
    predStateLD.push_back(pld0);
    predStateLD.push_back(pld1);
    predStateLD.push_back(pld2);

    std::pair<int, bool> pUpLdp;
    std::pair<predUpdateType, pair<int, bool> > pLdp1;
    pUpLdp.first = 0;
    pUpLdp.second = true;
    pLdp1.first = ANY;
    pLdp1.second = pUpLdp;
    pUpLD.push_back(pLdp1);

   
    std::pair<int, int> dru0, dru1, dru2, dru3;
    dru0.first = 0;
    dru0.second = 0;
    dru1.first = 1;
    dru1.second = 0;
    dru2.first = 2;
    dru2.second = 0;
    dru3.first = 3;
    dru3.second = 0;
    druLD.push_back(dru0);
    druLD.push_back(dru1);
    druLD.push_back(dru2);
    druLD.push_back(dru3);

    TIAinstruction ld0(static_cast<Instruction>(DEQ), predStateLD, flagsEmpty, inputLD, outputEmpty, 0, dqEmpty, druLD);
    ld0.setPredUpdates(pUpLD);
    t1.addInstruction(ld0);



    // Add next value into running sum (but not the last value)


    std::pair<int, bool> pgt0, pgt1, pgt2;
    pgt0.first = 0;
    pgt0.second = true;
    pgt1.first = 1;
    pgt1.second = false;
    pgt2.first = 2;
    pgt2.second = false;
    predStateGT.push_back(pgt0);
    predStateGT.push_back(pgt1);
    predStateGT.push_back(pgt2);


    std::pair<int, int> inputGTp, inputGTp2;
    inputGTp.first = 0;
    inputGTp.second = 0;
    inputGTp2.first = 1;
    inputGTp2.second = 0;
    inputGT.push_back(inputGTp);
    inputGT.push_back(inputGTp2);

    std::pair<int, bool> pUpGTp1;
    std::pair<predUpdateType, pair<int, bool> > pGTp1;
    pUpGTp1.first = 1;
    pUpGTp1.second = true;
    pGTp1.first = ANY;
    pGTp1.second = pUpGTp1;
    pUpGT.push_back(pGTp1);

    std::pair<bool, int> outputGTp;
    outputGTp.first = false;
    outputGTp.second = 0;
    outputGT.push_back(outputGTp);

    dqGT.push_back(0);

    std::pair<int, bool> flagGT;
    flagGT.first = 0;
    flagGT.second = false;
    flagsGT.push_back(flagGT);


    TIAinstruction gt(static_cast<Instruction>(ADD), predStateGT, flagsGT, inputGT, outputGT, 0, dqGT, druEmpty);
    gt.setPredUpdates(pUpGT);
    t1.addInstruction(gt);



    // Increment counter - not last value
    

    std::pair<int, bool> pic0, pic1, pic2;
    pic0.first = 0;
    pic0.second = true;
    pic1.first = 1;
    pic1.second = true;
    pic2.first = 2;
    pic2.second = false;
    predStateIC.push_back(pic0);
    predStateIC.push_back(pic1);
    predStateIC.push_back(pic2);



    std::pair<int, int> inputICp;
    inputICp.first = 1;
    inputICp.second = 1;
    inputIC.push_back(inputICp);

    std::pair<bool, int> outputICp;
    outputICp.first = false;
    outputICp.second = 1;
    outputIC.push_back(outputICp);


    std::pair<int, bool> pUpICp1;
    std::pair<predUpdateType, pair<int, bool> > pICp1;
    pUpICp1.first = 1;
    pUpICp1.second = false;
    pICp1.first = ANY;
    pICp1.second = pUpICp1;
    pUpIC.push_back(pICp1);

    TIAinstruction incrCount(static_cast<Instruction>(PLUSPLUS), predStateIC, flagsEmpty, inputIC, outputIC, 0, dqEmpty, druEmpty);
    incrCount.setPredUpdates(pUpIC);
    t1.addInstruction(incrCount);



    // Add last value into running sum


    std::pair<int, bool> pUpGT2p1, pUpGT2p2;
    std::pair<predUpdateType, pair<int, bool> > pGT2p1, pGT2p2;
    pUpGT2p1.first = 0;
    pUpGT2p1.second = false;
    pUpGT2p2.first = 1;
    pUpGT2p2.second = true;
    pGT2p1.first = ANY;
    pGT2p1.second = pUpGT2p1;
    pGT2p2.first = ANY;
    pGT2p2.second = pUpGT2p2;
    pUpGT2.push_back(pGT2p1);
    pUpGT2.push_back(pGT2p2);

    std::pair<int, bool> flagGT2;
    flagGT2.first = 0u;
    flagGT2.second = true;
    flagsGT2.push_back(flagGT2);


    TIAinstruction gt2(static_cast<Instruction>(ADD), predStateGT, flagsGT2, inputGT, outputGT, 0, dqGT, druEmpty);
    gt2.setPredUpdates(pUpGT2);
    t1.addInstruction(gt2);




    // Increment counter - last value

    std::pair<int, bool> picd0, picd1, picd2;
    picd0.first = 0;
    picd0.second = false;
    picd1.first = 1;
    picd1.second = true;
    picd2.first = 2;
    picd2.second = false;
    predStateICD.push_back(picd0);
    predStateICD.push_back(picd1);
    predStateICD.push_back(picd2);


    std::pair<int, bool> pUpICDp1, pUpICDp2;
    std::pair<predUpdateType, pair<int, bool> > pICDp1, pICDp2;
    pUpICDp1.first = 1;
    pUpICDp1.second = false;
    pUpICDp2.first = 2;
    pUpICDp2.second = true;
    pICDp1.first = ANY;
    pICDp1.second = pUpICDp1;
    pICDp2.first = ANY;
    pICDp2.second = pUpICDp2;
    pUpICD.push_back(pICDp1);
    pUpICD.push_back(pICDp2);

    TIAinstruction incrCountDone(static_cast<Instruction>(PLUSPLUS), predStateICD, flagsEmpty, inputIC, outputIC, 0, dqEmpty, druEmpty);
    incrCountDone.setPredUpdates(pUpICD);
    t1.addInstruction(incrCountDone);


   // Subtract operation to start the end division
  

    std::pair<int, bool> psub0, psub1, psub2;
    psub0.first = 0;
    psub0.second = false;
    psub1.first = 1;
    psub1.second = false;
    psub2.first = 2;
    psub2.second = true;
    predStateSUB.push_back(psub0);
    predStateSUB.push_back(psub1);
    predStateSUB.push_back(psub2);


    std::pair<int, int> inputSUBp, inputSUBp2;
    inputSUBp.first = 1;
    inputSUBp.second = 0;
    inputSUBp2.first = 1;
    inputSUBp2.second = 1;
    inputSUB.push_back(inputSUBp);
    inputSUB.push_back(inputSUBp2);


    std::pair<int, bool> pUpSUBp1;
    std::pair<predUpdateType, pair<int, bool> > pSUBp1;
    pUpSUBp1.first = 0;
    pUpSUBp1.second = true;
    pSUBp1.first = ANY;
    pSUBp1.second = pUpSUBp1;
    pUpSUB.push_back(pSUBp1);

   
    TIAinstruction sub(static_cast<Instruction>(SUBTRACT), predStateSUB, flagsEmpty, inputSUB, outputGT, 0, dqEmpty, druEmpty);
    sub.setPredUpdates(pUpSUB);
    t1.addInstruction(sub);


    // Check if the running sum is still larger than


    std::pair<int, bool> pgte0, pgte1, pgte2;
    pgte0.first = 0;
    pgte0.second = true;
    pgte1.first = 1;
    pgte1.second = false;
    pgte2.first = 2;
    pgte2.second = true;
    predStateGTE.push_back(pgte0);
    predStateGTE.push_back(pgte1);
    predStateGTE.push_back(pgte2);


    std::pair<int, int> inputGTEp, inputGTEp2;
    inputGTEp.first = 1;
    inputGTEp.second = 0;
    inputGTEp2.first = 1;
    inputGTEp2.second = 3;
    inputGTE.push_back(inputGTEp);
    inputGTE.push_back(inputGTEp2);


    std::pair<int, bool> pUpGTEp1, pUpGTEp2;
    std::pair<predUpdateType, pair<int, bool> > pGTEp1, pGTEp2;
    pUpGTEp1.first = 0;
    pUpGTEp1.second = true;
    pGTEp1.first = ZERO;
    pUpGTEp2.first = 1;
    pUpGTEp2.second = true;
    pGTEp2.first = ANY;
    pGTEp1.second = pUpGTEp1;
    pGTEp2.second = pUpGTEp2;
    pUpGTE.push_back(pGTEp1);
    pUpGTE.push_back(pGTEp2);


    TIAinstruction gte(static_cast<Instruction>(GREATER_THAN_EQUAL), predStateGTE, flagsEmpty, inputGTE, outputEmpty, 0, dqEmpty, druEmpty);
    gte.setPredUpdates(pUpGTE);
    t1.addInstruction(gte);


    // Add to running quotient
    

    std::pair<int, bool> pqa0, pqa1, pqa2;
    pqa0.first = 0;
    pqa0.second = false;
    pqa1.first = 1;
    pqa1.second = true;
    pqa2.first = 2;
    pqa2.second = true;
    predStateQA.push_back(pqa0);
    predStateQA.push_back(pqa1);
    predStateQA.push_back(pqa2);


    std::pair<int, int> inputQAp;
    inputQAp.first = 1;
    inputQAp.second = 2;
    inputQA.push_back(inputQAp);

    std::pair<bool, int> outputQAp;
    outputQAp.first = false;
    outputQAp.second = 2;
    outputQA.push_back(outputQAp);


    std::pair<int, bool> pUpQAp1, pUpQAp2;
    std::pair<predUpdateType, pair<int, bool> > pQAp1, pQAp2;
    pUpQAp1.first = 0;
    pUpQAp1.second = false;
    pQAp1.first = ANY;
    pUpQAp2.first = 1;
    pUpQAp2.second = false;
    pQAp2.first = ANY;
    pQAp1.second = pUpQAp1;
    pQAp2.second = pUpQAp2;
    pUpQA.push_back(pQAp1);
    pUpQA.push_back(pQAp2);


    TIAinstruction qa(static_cast<Instruction>(PLUSPLUS), predStateQA, flagsEmpty, inputQA, outputQA, 0, dqEmpty, druEmpty);
    qa.setPredUpdates(pUpQA);
    t1.addInstruction(qa);


    // Output final answer

    std::pair<int, bool> pf0, pf1, pf2;
    pf0.first = 0;
    pf0.second = true;
    pf1.first = 1;
    pf1.second = true;
    pf2.first = 2;
    pf2.second = true;
    predStateF.push_back(pf0);
    predStateF.push_back(pf1);
    predStateF.push_back(pf2);


    std::pair<bool, int> outputFp;
    outputFp.first = true;
    outputFp.second = 0;
    outputF.push_back(outputFp);

    std::pair<int, bool> pUpFp1, pUpFp2, pUpFp3;
    std::pair<predUpdateType, pair<int, bool> > pFp1, pFp2, pFp3;
    pUpFp1.first = 0;
    pUpFp1.second = false;
    pFp1.first = ANY;
    pUpFp2.first = 1;
    pUpFp2.second = false;
    pFp2.first = ANY;
    pUpFp3.first = 2;
    pUpFp3.second = false;
    pFp3.first = ANY;
    pFp1.second = pUpFp1;
    pFp2.second = pUpFp2;
    pFp3.second = pUpFp3;
    pUpF.push_back(pFp1);
    pUpF.push_back(pFp2);
    pUpF.push_back(pFp3);

    TIAinstruction final(static_cast<Instruction>(PASS_THROUGH), predStateF, flagsEmpty, inputQA, outputF, 0, dqEmpty, druEmpty);
    final.setPredUpdates(pUpF);
    t1.addInstruction(final);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Create vectors of all modules so that we can run them in our timing model main loop


    std::vector<MemStream *> mems;
    std::vector<Multiplier *> mults;
    std::vector<Fifo *> fifos;
    std::vector<liChannel *> channels;
    std::vector<TIA *> tias;

    // Add all modules to the proper vectors

    tias.push_back(&t1);

    channels.push_back(&in);
    channels.push_back(&tOut1);

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



        if(tOut1.getCanPop()){
	        printf("Output: %i\n", tOut1.get().data);
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
	    //printf(t1.predRegs.getValue(0) ? "true\n" : "false\n");
	    //printf(t1.predRegs.getValue(1) ? "true\n" : "false\n");
	    //printf(t1.predRegs.getValue(2) ? "true\n" : "false\n");
	}



	// Add a cycle's worth of time to the total time
	totalTime += cycleTime;
    
    }

    return 0;
}



#endif
