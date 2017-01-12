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

    multiChannel tiaHelp, tiaIn, tiaOut, tiaHelp2, tiaIn2, tiaOut2;
    liChannel tOut1(1), tSumOut1(1), tCountOut1(1), tHelp1(1), tiaToMult1(1), tiaToMult2(1), tHelp2(1);
    liChannel in(10);

    // Channels used in the creation of a memory streamer

    tiaIn.add(&in);

    tiaOut.add(&tSumOut1);
    tiaOut.add(&tCountOut1);

    tiaHelp.add(&tHelp1);

    TIA t1(&tiaHelp, &tiaIn, &tiaOut, 3, 4); 

    tiaIn2.add(&tSumOut1);
    tiaIn2.add(&tCountOut1);

    tiaOut2.add(&tOut1);

    tiaHelp2.add(&tHelp2);

    TIA t2(&tiaHelp2, &tiaIn2, &tiaOut2, 5, 7);

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
    i5.data = 100000000;
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
    std::vector<pair<int, int> > inputLD, druEmpty, druLD, inputCM, inputGT, inputDQD, inputIC, inputSUB, inputGTE, inputQA, inputFin;
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
    flagGT2.first = 0;
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

   // Push sum and count to TIA2

    std::pair<int, bool> pf0, pf1, pf2;
    pf0.first = 0;
    pf0.second = false;
    pf1.first = 1;
    pf1.second = false;
    pf2.first = 2;
    pf2.second = true;
    predStateF.push_back(pf0);
    predStateF.push_back(pf1);
    predStateF.push_back(pf2);


    std::pair<bool, int> outputFp, outputFp2;
    outputFp.first = true;
    outputFp.second = 0;
    outputFp2.first = true;
    outputFp2.second = 1;
    outputF.push_back(outputFp);
    outputF.push_back(outputFp2);

    std::pair<int, int> inputFinp, inputFinp2;
    inputFinp.first = 1;
    inputFinp.second = 0;
    inputFinp2.first = 1;
    inputFinp2.second = 1;
    inputFin.push_back(inputFinp);
    inputFin.push_back(inputFinp2);

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

    TIAinstruction final(static_cast<Instruction>(PASS_THROUGH_2), predStateF, flagsEmpty, inputFin, outputF, 0, dqEmpty, druEmpty);
    final.setPredUpdates(pUpF);
    t1.addInstruction(final);



    ///////////////////////////
    // TIA 2 instructions
    ///////////////////////////

    // Input
    std::vector<pair<int, int> > inputBegin, druBegin, inputCS, inputSS, inputLG, inputAQ, inputUS, inputSC, inputSF, inputFO;
    std::vector<pair<bool, int> > outputBegin, outputCS, outputSS, outputLG, outputAQ, outputUS, outputSC, outputSF, outputFO;
    std::vector<int> dqBegin;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpBegin, pUpCS, pUpSS, pUpLG, pUpAQ, pUpUS, pUpSC, pUpSF, pUpTS, pUpNC, pUpSU, pUpFO; 
    std::vector<pair<int, bool> > predStateBegin, predStateCS, predStateSS, predStateLG, predStateAQ, predStateUS, predStateSC, predStateSF, predStateTS, predStateNC, predStateAC, predStateSU, predStateFO;



    // Prepare T2 for operation
  

    std::pair<int, bool> pBG0, pBG1, pBG2, pBG3;
    pBG0.first = 0;
    pBG0.second = false;
    pBG1.first = 1;
    pBG1.second = false;
    pBG2.first = 2;
    pBG2.second = false;
    pBG3.first = 3;
    pBG3.second = false;
    predStateBegin.push_back(pBG0);
    predStateBegin.push_back(pBG1);
    predStateBegin.push_back(pBG2);
    predStateBegin.push_back(pBG3);
    
    std::pair<int, int> inputBeginp, inputBeginp2;
    inputBeginp.first = 0;
    inputBeginp.second = 0;
    inputBeginp2.first = 0;
    inputBeginp2.second = 1;
    inputBegin.push_back(inputBeginp);
    inputBegin.push_back(inputBeginp2);

    
    std::pair<bool, int> outputBeginp, outputBeginp2;
    outputBeginp.first = false;
    outputBeginp.second = 0;
    outputBeginp2.first = false;
    outputBeginp2.second = 1;
    outputBegin.push_back(outputBeginp);
    outputBegin.push_back(outputBeginp2);

    dqBegin.push_back(0);
    dqBegin.push_back(1);

    std::pair<int, int> druB0, druB1, druB2, druB3, druB4;
    druB0.first = 2;
    druB0.second = 65536;  // This is 1 << 16
    druB1.first = 3;
    druB1.second = 0;
    druB2.first = 4;
    druB2.second = 16;
    druB3.first = 5;
    druB3.second = 0;
    druB4.first = 6;
    druB4.second = 1;
    druBegin.push_back(druB0);
    druBegin.push_back(druB1);
    druBegin.push_back(druB2);
    druBegin.push_back(druB3);
    druBegin.push_back(druB4);

    std::pair<int, bool> pUpBGp;
    std::pair<predUpdateType, pair<int, bool> > pBGp;
    pUpBGp.first = 0;
    pUpBGp.second = true;
    pBGp.first = ANY;
    pBGp.second = pUpBGp;
    pUpBegin.push_back(pBGp);
    
    
    TIAinstruction beginT2(static_cast<Instruction>(PASS_THROUGH_2), predStateBegin, flagsEmpty, inputBegin, outputBegin, 0, dqBegin, druBegin);
    beginT2.setPredUpdates(pUpBegin);
    t2.addInstruction(beginT2);


    // Shift count by 16 bits to start subtraction mode
    
    std::pair<int, bool> pCS0, pCS1, pCS2, pCS3;
    pCS0.first = 0;
    pCS0.second = true;
    pCS1.first = 1;
    pCS1.second = false;
    pCS2.first = 2;
    pCS2.second = false;
    pCS3.first = 3;
    pCS3.second = false;
    predStateCS.push_back(pCS0);
    predStateCS.push_back(pCS1);
    predStateCS.push_back(pCS2);
    predStateCS.push_back(pCS3); 

    std::pair<int, int> inputCSp, inputCSp2;
    inputCSp.first = 1;
    inputCSp.second = 1;
    inputCSp2.first = 1;
    inputCSp2.second = 4;
    inputCS.push_back(inputCSp);
    inputCS.push_back(inputCSp2);

    std::pair<bool, int> outputCSp;
    outputCSp.first = false;
    outputCSp.second = 1;
    outputCS.push_back(outputCSp);


    std::pair<int, bool> pUpCSp;
    std::pair<predUpdateType, pair<int, bool> > pCSp;
    pUpCSp.first = 1;
    pUpCSp.second = true;
    pCSp.first = ANY;
    pCSp.second = pUpCSp;
    pUpCS.push_back(pCSp);


    TIAinstruction csT2(static_cast<Instruction>(SHIFT_LEFT), predStateCS, flagsEmpty, inputCS, outputCS, 0, dqEmpty, druEmpty);
    csT2.setPredUpdates(pUpCS);
    t2.addInstruction(csT2);


    // Try subtracting largest shift
    
    std::pair<int, bool> pSS0, pSS1, pSS2, pSS3;
    pSS0.first = 0;
    pSS0.second = true;
    pSS1.first = 1;
    pSS1.second = true;
    pSS2.first = 2;
    pSS2.second = false;
    pSS3.first = 3;
    pSS3.second = false;
    predStateSS.push_back(pSS0);
    predStateSS.push_back(pSS1);
    predStateSS.push_back(pSS2);
    predStateSS.push_back(pSS3); 

    std::pair<int, int> inputSSp, inputSSp2;
    inputSSp.first = 1;
    inputSSp.second = 0;
    inputSSp2.first = 1;
    inputSSp2.second = 1;
    inputSS.push_back(inputSSp);
    inputSS.push_back(inputSSp2);

    std::pair<bool, int> outputSSp;
    outputSSp.first = false;
    outputSSp.second = 4;
    outputSS.push_back(outputSSp);


    std::pair<int, bool> pUpSSp, pUpSSp2;
    std::pair<predUpdateType, pair<int, bool> > pSSp, pSSp2;
    pUpSSp.first = 1;
    pUpSSp.second = false;
    pSSp.first = ANY;
    pUpSSp2.first = 2;
    pUpSSp2.second = true;
    pSSp2.first = ANY;
    pSSp.second = pUpSSp;
    pSSp2.second = pUpSSp2;
    pUpSS.push_back(pSSp);
    pUpSS.push_back(pSSp2);
    
    
    
    TIAinstruction ssT2(static_cast<Instruction>(SUBTRACT), predStateSS, flagsEmpty, inputSS, outputSS, 0, dqEmpty, druEmpty);
    ssT2.setPredUpdates(pUpSS);
    t2.addInstruction(ssT2);


    // Check if we are still greater than 0

    std::pair<int, bool> pLG0, pLG1, pLG2, pLG3;
    pLG0.first = 0;
    pLG0.second = true;
    pLG1.first = 1;
    pLG1.second = false;
    pLG2.first = 2;
    pLG2.second = true;
    pLG3.first = 3;
    pLG3.second = false;
    predStateLG.push_back(pLG0);
    predStateLG.push_back(pLG1);
    predStateLG.push_back(pLG2);
    predStateLG.push_back(pLG3); 

    std::pair<int, int> inputLGp, inputLGp2;
    inputLGp.first = 1;
    inputLGp.second = 4;
    inputLGp2.first = 1;
    inputLGp2.second = 5;
    inputLG.push_back(inputLGp);
    inputLG.push_back(inputLGp2);


    std::pair<int, bool> pUpLGp, pUpLGp2;
    std::pair<predUpdateType, pair<int, bool> > pLGp, pLGp2;
    pUpLGp.first = 1;
    pUpLGp.second = true;
    pLGp.first = ANY;
    pUpLGp2.first = 3;
    pUpLGp2.second = true;
    pLGp2.first = ZERO;
    pLGp.second = pUpLGp;
    pLGp2.second = pUpLGp2;
    pUpLG.push_back(pLGp);
    pUpLG.push_back(pLGp2);
    

    TIAinstruction lgT2(static_cast<Instruction>(GREATER_THAN_EQUAL), predStateLG, flagsEmpty, inputLG, outputEmpty, 0, dqEmpty, druEmpty);
    lgT2.setPredUpdates(pUpLG);
    t2.addInstruction(lgT2);

  
    // Add to our quotient - still first iteration!


    std::pair<int, bool> pAQ0, pAQ1, pAQ2, pAQ3, pAQ4;
    pAQ0.first = 0;
    pAQ0.second = true;
    pAQ1.first = 1;
    pAQ1.second = true;
    pAQ2.first = 2;
    pAQ2.second = true;
    pAQ3.first = 3;
    pAQ3.second = false;
    pAQ4.first = 4;
    pAQ4.second = false;
    predStateAQ.push_back(pAQ0);
    predStateAQ.push_back(pAQ1);
    predStateAQ.push_back(pAQ2);
    predStateAQ.push_back(pAQ3); 
    predStateAQ.push_back(pAQ4);

    std::pair<int, int> inputAQp, inputAQp2;
    inputAQp.first = 1;
    inputAQp.second = 2;
    inputAQp2.first = 1;
    inputAQp2.second = 3;
    inputAQ.push_back(inputAQp);
    inputAQ.push_back(inputAQp2);

    std::pair<bool, int> outputAQp;
    outputAQp.first = false;
    outputAQp.second = 3;
    outputAQ.push_back(outputAQp);


    std::pair<int, bool> pUpAQp;
    std::pair<predUpdateType, pair<int, bool> > pAQp;
    pUpAQp.first = 4;
    pUpAQp.second = true;
    pAQp.first = ANY;
    pAQp.second = pUpAQp;
    pUpAQ.push_back(pAQp);


    TIAinstruction aqT2(static_cast<Instruction>(ADD), predStateAQ, flagsEmpty, inputAQ, outputAQ, 0, dqEmpty, druEmpty);
    aqT2.setPredUpdates(pUpAQ);
    t2.addInstruction(aqT2);


    // Update Sum to reflect subtraction - initial round


    std::pair<int, bool> pUS0, pUS1, pUS2, pUS3, pUS4;
    pUS0.first = 0;
    pUS0.second = true;
    pUS1.first = 1;
    pUS1.second = true;
    pUS2.first = 2;
    pUS2.second = true;
    pUS3.first = 3;
    pUS3.second = false;
    pUS4.first = 4;
    pUS4.second = true;
    predStateUS.push_back(pUS0);
    predStateUS.push_back(pUS1);
    predStateUS.push_back(pUS2);
    predStateUS.push_back(pUS3); 
    predStateUS.push_back(pUS4);

    std::pair<int, int> inputUSp;
    inputUSp.first = 1;
    inputUSp.second = 4;
    inputUS.push_back(inputUSp);

    std::pair<bool, int> outputUSp;
    outputUSp.first = false;
    outputUSp.second = 0;
    outputUS.push_back(outputUSp);


    std::pair<int, bool> pUpUSp, pUpUSp2;
    std::pair<predUpdateType, pair<int, bool> > pUSp, pUSp2;
    pUpUSp.first = 2;
    pUpUSp.second = false;
    pUSp.first = ANY;
    pUpUSp2.first = 4;
    pUpUSp2.second = false;
    pUSp2.first = ANY;
    pUSp.second = pUpUSp;
    pUSp2.second = pUpUSp2;
    pUpUS.push_back(pUSp);
    pUpUS.push_back(pUSp2);


    TIAinstruction usT2(static_cast<Instruction>(PASS_THROUGH), predStateUS, flagsEmpty, inputUS, outputUS, 0, dqEmpty, druEmpty);
    usT2.setPredUpdates(pUpUS);
    t2.addInstruction(usT2);


    // Shift the count subtractor to the right by 1
    
    std::pair<int, bool> pSC0, pSC1, pSC2, pSC3;
    pSC0.first = 0;
    pSC0.second = true;
    pSC1.first = 1;
    pSC1.second = true;
    pSC2.first = 2;
    pSC2.second = true;
    pSC3.first = 3;
    pSC3.second = true;
    predStateSC.push_back(pSC0);
    predStateSC.push_back(pSC1);
    predStateSC.push_back(pSC2);
    predStateSC.push_back(pSC3); 

    std::pair<int, int> inputSCp, inputSCp2;
    inputSCp.first = 1;
    inputSCp.second = 1;
    inputSCp2.first = 1;
    inputSCp2.second = 6;
    inputSC.push_back(inputSCp);
    inputSC.push_back(inputSCp2);

    std::pair<bool, int> outputSCp;
    outputSCp.first = false;
    outputSCp.second = 1;
    outputSC.push_back(outputSCp);


    std::pair<int, bool> pUpSCp, pUpSCp2, pUpSCp3;
    std::pair<predUpdateType, pair<int, bool> > pSCp, pSCp2, pSCp3;
    pUpSCp.first = 0;
    pUpSCp.second = false;
    pSCp.first = ANY;
    pUpSCp2.first = 2;
    pUpSCp2.second = false;
    pSCp2.first = ANY;
    pUpSCp3.first = 3;
    pUpSCp3.second = false;
    pSCp3.first = ANY;
    pSCp.second = pUpSCp;
    pSCp2.second = pUpSCp2;
    pSCp3.second = pUpSCp3;
    pUpSC.push_back(pSCp);
    pUpSC.push_back(pSCp2);
    pUpSC.push_back(pSCp3);


    TIAinstruction scT2(static_cast<Instruction>(SHIFT_RIGHT), predStateSC, flagsEmpty, inputSC, outputSC, 0, dqEmpty, druEmpty);
    scT2.setPredUpdates(pUpSC);
    t2.addInstruction(scT2);


    // Shift our additive factor and check if it is equal to 1 yet
    
    std::pair<int, bool> pSF0, pSF1, pSF2, pSF3;
    pSF0.first = 0;
    pSF0.second = false;
    pSF1.first = 1;
    pSF1.second = true;
    pSF2.first = 2;
    pSF2.second = false;
    pSF3.first = 3;
    pSF3.second = false;
    predStateSF.push_back(pSF0);
    predStateSF.push_back(pSF1);
    predStateSF.push_back(pSF2);
    predStateSF.push_back(pSF3); 

    std::pair<int, int> inputSFp, inputSFp2;
    inputSFp.first = 1;
    inputSFp.second = 2;
    inputSFp2.first = 1;
    inputSFp2.second = 6;
    inputSF.push_back(inputSFp);
    inputSF.push_back(inputSFp2);

    std::pair<bool, int> outputSFp;
    outputSFp.first = false;
    outputSFp.second = 2;
    outputSF.push_back(outputSFp);


    std::pair<int, bool> pUpSFp, pUpSFp2;
    std::pair<predUpdateType, pair<int, bool> > pSFp, pSFp2;
    pUpSFp.first = 2;
    pUpSFp.second = true;
    pSFp.first = ANY;
    pUpSFp2.first = 3;
    pUpSFp2.second = false;
    pSFp2.first = ZERO;
    pSFp.second = pUpSFp;
    pSFp2.second = pUpSFp2;
    pUpSF.push_back(pSFp);
    pUpSF.push_back(pSFp2);


    TIAinstruction sfT2(static_cast<Instruction>(SHIFT_RIGHT), predStateSF, flagsEmpty, inputSF, outputSF, 0, dqEmpty, druEmpty);
    sfT2.setPredUpdates(pUpSF);
    t2.addInstruction(sfT2);


    // Try subtracting count from leftover sum

    std::pair<int, bool> pTS0, pTS1, pTS2, pTS3;
    pTS0.first = 0;
    pTS0.second = false;
    pTS1.first = 1;
    pTS1.second = true;
    pTS2.first = 2;
    pTS2.second = true;
    pTS3.first = 3;
    pTS3.second = false;
    predStateTS.push_back(pTS0);
    predStateTS.push_back(pTS1);
    predStateTS.push_back(pTS2);
    predStateTS.push_back(pTS3); 

    

    std::pair<int, bool> pUpTSp, pUpTSp2;
    std::pair<predUpdateType, pair<int, bool> > pTSp, pTSp2;
    pUpTSp.first = 2;
    pUpTSp.second = false;
    pTSp.first = ANY;
    pUpTSp2.first = 3;
    pUpTSp2.second = true;
    pTSp2.first = ANY;
    pTSp.second = pUpTSp;
    pTSp2.second = pUpTSp2;
    pUpTS.push_back(pTSp);
    pUpTS.push_back(pTSp2);
    
    
    TIAinstruction tsT2(static_cast<Instruction>(SUBTRACT), predStateTS, flagsEmpty, inputSS, outputSS, 0, dqEmpty, druEmpty);
    tsT2.setPredUpdates(pUpTS);
    t2.addInstruction(tsT2);


    // Check if we are still over zero for our running sum

    std::pair<int, bool> pNC0, pNC1, pNC2, pNC3;
    pNC0.first = 0;
    pNC0.second = false;
    pNC1.first = 1;
    pNC1.second = true;
    pNC2.first = 2;
    pNC2.second = false;
    pNC3.first = 3;
    pNC3.second = true;
    predStateNC.push_back(pNC0);
    predStateNC.push_back(pNC1);
    predStateNC.push_back(pNC2);
    predStateNC.push_back(pNC3); 


    std::pair<int, bool> pUpNCp, pUpNCp2;
    std::pair<predUpdateType, pair<int, bool> > pNCp, pNCp2;
    pUpNCp.first = 0;
    pUpNCp.second = true;
    pNCp.first = ANY;
    pUpNCp2.first = 2;
    pUpNCp2.second = true;
    pNCp2.first = ZERO;
    pNCp.second = pUpNCp;
    pNCp2.second = pUpNCp2;
    pUpNC.push_back(pNCp);
    pUpNC.push_back(pNCp2);
    

    TIAinstruction ncT2(static_cast<Instruction>(GREATER_THAN_EQUAL), predStateNC, flagsEmpty, inputLG, outputEmpty, 0, dqEmpty, druEmpty);
    ncT2.setPredUpdates(pUpNC);
    t2.addInstruction(ncT2);


    // Add the correct amount to our running quotient
    

    std::pair<int, bool> pAC0, pAC1, pAC2, pAC3, pAC4;
    pAC0.first = 0;
    pAC0.second = true;
    pAC1.first = 1;
    pAC1.second = true;
    pAC2.first = 2;
    pAC2.second = false;
    pAC3.first = 3;
    pAC3.second = true;
    pAC4.first = 4;
    pAC4.second = false;
    predStateAC.push_back(pAC0);
    predStateAC.push_back(pAC1);
    predStateAC.push_back(pAC2);
    predStateAC.push_back(pAC3); 
    predStateAC.push_back(pAC4);


    TIAinstruction acT2(static_cast<Instruction>(ADD), predStateAC, flagsEmpty, inputAQ, outputAQ, 0, dqEmpty, druEmpty);
    acT2.setPredUpdates(pUpAQ);
    t2.addInstruction(acT2);

    
    // Update running sum to reflect our correct operation
    

    std::pair<int, bool> pSU0, pSU1, pSU2, pSU3, pSU4;
    pSU0.first = 0;
    pSU0.second = true;
    pSU1.first = 1;
    pSU1.second = true;
    pSU2.first = 2;
    pSU2.second = false;
    pSU3.first = 3;
    pSU3.second = true;
    pSU4.first = 4;
    pSU4.second = true;
    predStateSU.push_back(pSU0);
    predStateSU.push_back(pSU1);
    predStateSU.push_back(pSU2);
    predStateSU.push_back(pSU3); 
    predStateSU.push_back(pSU4);



    std::pair<int, bool> pUpSUp, pUpSUp2;
    std::pair<predUpdateType, pair<int, bool> > pSUp, pSUp2;
    pUpSUp.first = 2;
    pUpSUp.second = true;
    pSUp.first = ANY;
    pUpSUp2.first = 4;
    pUpSUp2.second = false;
    pSUp2.first = ANY;
    pSUp.second = pUpSUp;
    pSUp2.second = pUpSUp2;
    pUpSU.push_back(pSUp);
    pUpSU.push_back(pSUp2);


    TIAinstruction suT2(static_cast<Instruction>(PASS_THROUGH), predStateSU, flagsEmpty, inputUS, outputUS, 0, dqEmpty, druEmpty);
    suT2.setPredUpdates(pUpSU);
    t2.addInstruction(suT2);


    // Done with division, output answer
    
    std::pair<int, bool> pFO0, pFO1, pFO2, pFO3;
    pFO0.first = 0;
    pFO0.second = false;
    pFO1.first = 1;
    pFO1.second = true;
    pFO2.first = 2;
    pFO2.second = true;
    pFO3.first = 3;
    pFO3.second = true;
    predStateFO.push_back(pFO0);
    predStateFO.push_back(pFO1);
    predStateFO.push_back(pFO2);
    predStateFO.push_back(pFO3); 

    std::pair<int, int> inputFOp;
    inputFOp.first = 1;
    inputFOp.second = 3;
    inputFO.push_back(inputFOp);

    std::pair<bool, int> outputFOp;
    outputFOp.first = true;
    outputFOp.second = 0;
    outputFO.push_back(outputFOp);


    std::pair<int, bool> pUpFOp, pUpFOp2, pUpFOp3;
    std::pair<predUpdateType, pair<int, bool> > pFOp, pFOp2, pFOp3;
    pUpFOp.first = 1;
    pUpFOp.second = false;
    pFOp.first = ANY;
    pUpFOp2.first = 2;
    pUpFOp2.second = false;
    pFOp2.first = ANY;
    pUpFOp3.first = 3;
    pUpFOp3.second = false;
    pFOp3.first = ANY;
    pFOp.second = pUpFOp;
    pFOp2.second = pUpFOp2;
    pFOp3.second = pUpFOp3;
    pUpFO.push_back(pFOp);
    pUpFO.push_back(pFOp2);
    pUpFO.push_back(pFOp3);


    TIAinstruction foT2(static_cast<Instruction>(PASS_THROUGH), predStateFO, flagsEmpty, inputFO, outputFO, 0, dqEmpty, druEmpty);
    foT2.setPredUpdates(pUpFO);
    t2.addInstruction(foT2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Create vectors of all modules so that we can run them in our timing model main loop


    std::vector<MemStream *> mems;
    std::vector<Multiplier *> mults;
    std::vector<Fifo *> fifos;
    std::vector<liChannel *> channels;
    std::vector<TIA *> tias;

    // Add all modules to the proper vectors

    tias.push_back(&t1);
    tias.push_back(&t2);

    channels.push_back(&in);
    channels.push_back(&tOut1);
    channels.push_back(&tSumOut1);
    channels.push_back(&tCountOut1);

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
	int whichIndex = 0;

        // Set off any module that can schedule, execute, and commit
	

	if(tSumOut1.getCanPop()){
	    printf("Sum: %i\n", tSumOut1.peek().data);
	}
	if(tCountOut1.getCanPop()){
	    printf("Count: %i\n", tCountOut1.peek().data);
	}
	

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
	    if(cTimes[tiaTimeMap[eResult.getCmd()]] >= maxTIAtime){
	        maxTIAtime = cTimes[tiaTimeMap[eResult.getCmd()]];
		whichTIA = eResult.getCmd();
		whichIndex = sResult.getIndex();
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
	    //printf("TIA Index: %i\n", whichIndex);
	    //printf("Reg0: %i\n", t2.dataRegs.getValue(0));
	    //printf("Reg1: %i\n", t2.dataRegs.getValue(1));
	    //printf("Reg2: %i\n", t2.dataRegs.getValue(2));
	    //printf("Reg3: %i\n", t2.dataRegs.getValue(3));
	    //printf("Reg4: %i\n", t2.dataRegs.getValue(4));
	    //printf("Reg5: %i\n", t2.dataRegs.getValue(5));
	}



	// Add a cycle's worth of time to the total time
	totalTime += cycleTime;
    
    }

    return 0;
}



#endif
