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
    liChannel in(10);

    // Channels used in the creation of a memory streamer

    tiaIn.add(&in);

    tiaOut.add(&tOut1);

    tiaHelp.add(&tHelp1);

    TIA t1(&tiaHelp, &tiaIn, &tiaOut, 3, 1); 

    // Push image and filter with proper tags to the TIA input channels
    
    packet i1, i2, i3, i4, i5, i6, i7, i8;
    i1.data = 600;
    i1.tag = 0;
    i2.data = 12;
    i2.tag = 0;
    i3.data = 0;
    i3.tag = 0;
    i4.data = -6;
    i4.tag = 0;
    i5.data = 60;
    i5.tag = 0;
    i6.data = 33;
    i6.tag = 0;
    i7.data = 1000;
    i7.tag = 0;
    i8.data = 0;
    i8.tag = 1;

    
    in.put(i1);
    in.put(i2);
    in.put(i3);
    in.put(i4);
    in.put(i5);
    in.put(i6);
   // in.put(i7);
    in.put(i8);
    in.setCanPop(true);

//////////////////////////////////////////////////////////////////

    // Load factor 0 into data reg 0
    std::vector<pair<int, bool> > flagsEmpty, flagsCM, flagsCMD, flagsGT;
    std::vector<pair<int, int> > inputLD, druEmpty, druLD, inputCM, inputGT, inputDQD;
    std::vector<pair<bool, int> > outputEmpty, outputCM, outputCMD;
    std::vector<int> dqEmpty, dqChan;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpLD, pUpGT, pUpCM; 
    std::vector<pair<int, bool> > predStateLD, predStateGT, predStateCM, predStateDQ;
    
    
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

   
    std::pair<int, int> dru0;
    dru0.first = 0;
    dru0.second = 0;
    druLD.push_back(dru0);

    TIAinstruction ld0(static_cast<Instruction>(DEQ), predStateLD, flagsEmpty, inputLD, outputEmpty, 0, dqEmpty, druLD);
    ld0.setPredUpdates(pUpLD);
    t1.addInstruction(ld0);



    // Check if greater than current max


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

    std::pair<int, bool> flagGT;
    flagGT.first = 0;
    flagGT.second = false;
    flagsGT.push_back(flagGT);

    std::pair<int, bool> pUpGTp1, pUpGTp2;
    std::pair<predUpdateType, pair<int, bool> > pGTp1, pGTp2;
    pUpGTp1.first = 1;
    pUpGTp1.second = false;
    pUpGTp2.first = 2;
    pUpGTp2.second = true;
    pGTp1.first = ZERO;
    pGTp1.second = pUpGTp1;
    pGTp2.first = ANY;
    pGTp2.second = pUpGTp2;
    pUpGT.push_back(pGTp1);
    pUpGT.push_back(pGTp2);


    TIAinstruction gt(static_cast<Instruction>(GREATER_THAN), predStateGT, flagsGT, inputGT, outputEmpty, 0, dqEmpty, druEmpty);
    gt.setPredUpdates(pUpGT);
    t1.addInstruction(gt);

// Change Max  
    std::pair<int, bool> pcm0, pcm1, pcm2;
    pcm0.first = 0;
    pcm0.second = true;
    pcm1.first = 1;
    pcm1.second = false;
    pcm2.first = 2;
    pcm2.second = true;
    predStateCM.push_back(pcm0);
    predStateCM.push_back(pcm1);
    predStateCM.push_back(pcm2);

    std::pair<int, int> inputCMp;
    inputCMp.first = 0;
    inputCMp.second = 0;
    inputCM.push_back(inputCMp);

    std::pair<int, bool> pUpCMp1, pUpCMp2;
    std::pair<predUpdateType, pair<int, bool> > pCMp1, pCMp2;
    pUpCMp1.first = 1;
    pUpCMp1.second = false;
    pUpCMp2.first = 2;
    pUpCMp1.second = false;
    pCMp1.first = ANY;
    pCMp1.second = pUpCMp1;
    pCMp2.first = ANY;
    pCMp2.second = pUpCMp2;
    pUpCM.push_back(pCMp1);
    pUpCM.push_back(pCMp2);

    dqChan.push_back(0);

    std::pair<bool, int> outputCMp;
    outputCMp.first = false;
    outputCMp.second = 0;
    outputCM.push_back(outputCMp);

    TIAinstruction cm(static_cast<Instruction>(PASS_THROUGH), predStateCM, flagsEmpty, inputCM, outputCM, 0, dqChan, druEmpty);
    cm.setPredUpdates(pUpCM);
    t1.addInstruction(cm);


// Change Max - final   

    std::pair<int, bool> flagCMD;
    flagCMD.first = 0;
    flagCMD.second = true;
    flagsCMD.push_back(flagCMD);

    std::pair<bool, int> outputCMDp;
    outputCMDp.first = true;
    outputCMDp.second = 0;
    outputCMD.push_back(outputCMDp);

    TIAinstruction cmd(static_cast<Instruction>(PASS_THROUGH), predStateCM, flagsCMD, inputCM, outputCMD, 0, dqChan, druEmpty);
    cmd.setPredUpdates(pUpCM);
    t1.addInstruction(cmd);

// DEQ

    std::pair<int, bool> pdq0, pdq1, pdq2;
    pdq0.first = 0;
    pdq0.second = true;
    pdq1.first = 1;
    pdq1.second = true;
    pdq2.first = 2;
    pdq2.second = true;
    predStateDQ.push_back(pdq0);
    predStateDQ.push_back(pdq1);
    predStateDQ.push_back(pdq2);


    TIAinstruction dq(static_cast<Instruction>(DEQ), predStateDQ, flagsCM, inputCM, outputEmpty, 0, dqChan, druEmpty);
    dq.setPredUpdates(pUpCM);
    t1.addInstruction(dq);

// DEQ - final

    std::pair<int, int> inputDQDp;
    inputDQDp.first = 1;
    inputDQDp.second = 0;
    inputDQD.push_back(inputDQDp);

    TIAinstruction dqd(static_cast<Instruction>(DEQ), predStateGT, flagsCMD, inputDQD, outputCMD, 0, dqChan, druEmpty);
    dqd.setPredUpdates(pUpGT);
    t1.addInstruction(dqd);


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
	}



	// Add a cycle's worth of time to the total time
	totalTime += cycleTime;
    
    }

    return 0;
}



#endif
