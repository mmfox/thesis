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
    // Create main memory

    int * memory = new int [MAIN_MEM_SIZE];


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Initializing the fifos
    //
    liChannel fFifoOut(1), iFifoOut(1), mFifoOut(1), tiaToIFifo(1), tiaToFFifo(1), multToFifo(1), fIn(20), iIn(20);

    // Defines fifo size
    int fifoSize = 80;

    multiChannel fifoIn1;

    fifoIn1.add(&iFifoOut);
    fifoIn1.add(&tiaToIFifo);

    Fifo f1(&fifoIn1, fifoSize);

    multiChannel fifoIn2;

    fifoIn2.add(&fFifoOut);
    fifoIn2.add(&tiaToFFifo);

    Fifo f2(&fifoIn2, fifoSize);

    //multiChannel mFin;

    //mFin.add(&mFifoOut);
    //mFin.add(&multToFifo);

    //Fifo multFifo(&mFin, fifoSize);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Loading vectors into main memory

    // Create vectors 
    int l1 [3] = {1,2,3};
    int l2 [7] = {1,2,3,4,5,6,7};

    std::vector<int> v1;
    v1.assign(l1, l1+3);

    std::vector<int> v2;
    v2.assign(l2, l2+7);


    // Load vectors into memory one after another
    for(int j=0; j<v1.size(); j++){
        memory[j] = v1[j];
    }


    for(int j=0; j<v2.size(); j++){
        memory[j+v1.size()] = v2[j];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Create and insert the packets which tell the streams that they can start streaming

    //struct packet startMem;
    //startMem.data = 3; // Command that tells the memory streamer to begin 
    //startMem.tag = 0;
    //memStart1.put(startMem);
    //memStart1.setCanPop(true);
    //memStart2.put(startMem);
    //memStart2.setCanPop(true);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Initializing the TIA module

    multiChannel tiaHelp, tiaIn, tiaOut;
    liChannel tOut1(1), tHelp1(1), tiaToMult1(1), tiaToMult2(1);
    liChannel multToTIA(1);

    // Channels used in the creation of a memory streamer
    liChannel memStart1(1), memOut1(1), memData1(1), base1(1), stride1(1), end1(1);
    liChannel memStart2(1), memOut2(1), memData2(1), base2(1), stride2(1), end2(1);

    tiaIn.add(&iIn);
    tiaIn.add(&fIn);
    tiaIn.add(&iFifoOut);
    tiaIn.add(&fFifoOut);
    tiaIn.add(&multToTIA);

    tiaOut.add(&tOut1);
    tiaOut.add(&tiaToIFifo);
    tiaOut.add(&tiaToFFifo);
    tiaOut.add(&tiaToMult1);
    tiaOut.add(&tiaToMult2);

    tiaHelp.add(&tHelp1);

    TIA t1(&tiaHelp, &tiaIn, &tiaOut, 2, 1); 

    // Push image and filter with proper tags to the TIA input channels
    
    packet i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16, i17, i18, i19, i20, filt1, filt2, filt3, filt4, filt5, filt6, filt7, filt8, filt9, filt10, filt11, filt12, filt13, filt14, filt15, filt16, filt17, filt18, filt19, filt20;
    i1.data = 1;
    i1.tag = 0;
    i2.data = 2;
    i2.tag = 0;
    i3.data = 3;
    i3.tag = 0;
    i4.data = 4;
    i4.tag = 0;
    i5.data = 5;
    i5.tag = 0;
    i6.data = 6;
    i6.tag = 0;
    i7.data = 7;
    i7.tag = 0;
    i8.data = 8;
    i8.tag = 0;
    i9.data = 9;
    i9.tag = 0;
    i10.data = 10;
    i10.tag = 0;
    i11.data = 11;
    i11.tag = 0;
    i12.data = 12;
    i12.tag = 0;
    i13.data = 13;
    i13.tag = 0;
    i14.data = 14;
    i14.tag = 0;
    i15.data = 15;
    i15.tag = 0;
    i16.data = 16;
    i16.tag = 0;
    i17.data = 17;
    i17.tag = 0;
    i18.data = 18;
    i18.tag = 0;
    i19.data = 19;
    i19.tag = 0; 
    i20.data = 20;
    i20.tag = 1;

    filt1.data = 1;
    filt1.tag = 0;
    filt2.data = 1;
    filt2.tag = 0;
    filt3.data = 1;
    filt3.tag = 0;
    filt4.data = 1;
    filt4.tag = 0;
    filt5.data = 1;
    filt5.tag = 0;
    filt6.data = 1;
    filt6.tag = 0;
    filt7.data = 1;
    filt7.tag = 0;
    filt8.data = 1;
    filt8.tag = 0;
    filt9.data = 1;
    filt9.tag = 0;
    filt10.data = 1;
    filt10.tag = 0;
    filt11.data = 1;
    filt11.tag = 0;
    filt12.data = 1;
    filt12.tag = 0;
    filt13.data = 1;
    filt13.tag = 0;
    filt14.data = 1;
    filt14.tag = 0;
    filt15.data = 1;
    filt15.tag = 0;
    filt16.data = 1;
    filt16.tag = 0;
    filt17.data = 1;
    filt17.tag = 0;
    filt18.data = 1;
    filt18.tag = 0;
    filt19.data = 1;
    filt19.tag = 0;
    filt20.data = 1;
    filt20.tag = 1;

    
    iIn.put(i1);
    iIn.put(i2);
    iIn.put(i3);
    iIn.put(i4);
    iIn.put(i5);
    iIn.put(i6);
    iIn.put(i7);
    iIn.put(i8);
    iIn.put(i9);
    iIn.put(i10);
    iIn.put(i11);
    iIn.put(i12);
    iIn.put(i13);
    iIn.put(i14);
    iIn.put(i15);
    iIn.put(i16);
    iIn.put(i17);
    iIn.put(i18);
    iIn.put(i19);
    iIn.put(i20);

    iIn.setCanPop(true);

    fIn.put(filt1);
    fIn.put(filt2);
    fIn.put(filt3);
    fIn.put(filt4);
    fIn.put(filt5);
    fIn.put(filt6);
    fIn.put(filt7);
    fIn.put(filt8);
    //fIn.put(filt9);
    //fIn.put(filt10);
    //fIn.put(filt11);
    //fIn.put(filt12);
    //fIn.put(filt13);
    //fIn.put(filt14);
    //fIn.put(filt15);
    //fIn.put(filt16);
   // fIn.put(filt17);
    //fIn.put(filt18);
   // fIn.put(filt19);
    fIn.put(filt20);

    fIn.setCanPop(true);


    // Let's put in the convolution instructions
    // Unload Multiplier not done with convolution
    std::vector<pair<int, bool> > predStateEmpty;
    std::vector<pair<int, bool> > flagsUM;
    std::vector<pair<int, int> > inputUM, druEmpty;
    std::vector<pair<bool, int> > outputUM;
    std::vector<int> dqUM;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpEmpty;
 
    std::pair<int, bool> flagUM;
    flagUM.first = 4;
    flagUM.second = false;
    flagsUM.push_back(flagUM);

    std::pair<int, int> inputMult, inputDR;
    inputMult.first = 0;
    inputMult.second = 4;
    inputDR.first = 1;
    inputDR.second = 0;
    inputUM.push_back(inputMult);
    inputUM.push_back(inputDR);

    std::pair<bool, int> outputUMp;
    outputUMp.first = false;
    outputUMp.second = 0;
    outputUM.push_back(outputUMp);

    dqUM.push_back(4);

    TIAinstruction unloadMult(static_cast<Instruction>(ADD), predStateEmpty, flagsUM, inputUM, outputUM, 0, dqUM, druEmpty);
    unloadMult.setPredUpdates(pUpEmpty);
    t1.addInstruction(unloadMult);



    // Unload Multiplier done with convolution
    std::vector<pair<int, bool> > flagsCD;
    std::vector<pair<int, int> > inputCD, druCD;
    std::vector<pair<bool, int> > outputCD;
    std::vector<int> dqCD;
 
    std::pair<int, bool> flagCD;
    flagCD.first = 4;
    flagCD.second = true;
    flagsCD.push_back(flagCD);

    inputCD.push_back(inputMult);
    inputCD.push_back(inputDR);

    std::pair<bool, int> outputCDp;
    outputCDp.first = true;
    outputCDp.second = 0;
    outputCD.push_back(outputCDp);

    dqCD.push_back(4);

    std::pair<int, int> dru0;
    dru0.first = 0;
    dru0.second = 0;
    druCD.push_back(dru0);

    TIAinstruction convolutionDone(static_cast<Instruction>(ADD), predStateEmpty, flagsCD, inputCD, outputCD, 0, dqCD, druCD);
    convolutionDone.setPredUpdates(pUpEmpty);
    t1.addInstruction(convolutionDone);


    // Load Image
    std::vector<pair<int, bool> > flagsEmpty;
    std::vector<pair<int, int> > inputLI;
    std::vector<pair<bool, int> > outputLI;
    std::vector<int> dqLI;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpLI;
    std::vector<pair<int, bool> > predStateLI;

    std::pair<int, bool> predStateLIp, predStateLIp2;
    predStateLIp.first = 0;
    predStateLIp.second = false;
    predStateLIp2.first = 1;
    predStateLIp2.second = true;
    predStateLI.push_back(predStateLIp);
    predStateLI.push_back(predStateLIp2);


    std::pair<int, bool> pUpLIp;
    std::pair<predUpdateType, pair<int, bool> > plip1;
    pUpLIp.first = 1;
    pUpLIp.second = false;
    plip1.first = ANY;
    plip1.second = pUpLIp;
    pUpLI.push_back(plip1);
 
    std::pair<int, int> inputLIp;
    inputLIp.first = 0;
    inputLIp.second = 0;
    inputLI.push_back(inputLIp);

    std::pair<bool, int> outputLIp;
    outputLIp.first = true;
    outputLIp.second = 1;
    outputLI.push_back(outputLIp);

    dqLI.push_back(0);

    TIAinstruction loadImage(static_cast<Instruction>(PASS_THROUGH), predStateLI, flagsEmpty, inputLI, outputLI, 0, dqLI, druEmpty);
    loadImage.setPredUpdates(pUpLI);
    t1.addInstruction(loadImage);

    // Load Filter
    std::vector<pair<int, int> > inputLF, druLF;
    std::vector<pair<bool, int> > outputLF;
    std::vector<int> dqLF;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpLF;

 
    std::pair<int, bool> pUpLFp;
    std::pair<predUpdateType, pair<int, bool> > plfp1;
    pUpLFp.first = 1;
    pUpLFp.second = true;
    plfp1.first = ANY;
    plfp1.second = pUpLFp;
    pUpLF.push_back(plfp1);


    std::pair<int, int> inputLFp;
    inputLFp.first = 0;
    inputLFp.second = 1;
    inputLF.push_back(inputLFp);

    std::pair<bool, int> outputLFp;
    outputLFp.first = true;
    outputLFp.second = 2;
    outputLF.push_back(outputLFp);

    dqLF.push_back(1);

    std::pair<int, int> druLFp;
    druLFp.first = 0;
    druLFp.second = 0;

    TIAinstruction loadFilter(static_cast<Instruction>(PASS_THROUGH), predStateEmpty, flagsEmpty, inputLF, outputLF, 0, dqLF, druLF);
    loadFilter.setPredUpdates(pUpLF);
    t1.addInstruction(loadFilter);



    // Pass arguments to multiplier not done with convolution step
    std::vector<pair<int, bool> > predStatePA;
    std::vector<pair<int, bool> > flagsPA;
    std::vector<pair<int, int> > inputPA;
    std::vector<pair<bool, int> > outputPA;
    std::vector<int> dqPA;

    std::pair<int, bool> predStatePAp, predStatePAp2;
    predStatePAp.first = 0;
    predStatePAp.second = false;
    predStatePAp2.first = 1;
    predStatePAp2.second = false;
    predStatePA.push_back(predStatePAp);
    predStatePA.push_back(predStatePAp);
 
    std::pair<int, bool> flagPA, flagPA2;
    flagPA.first = 3;
    flagPA.second = false;
    flagPA2.first = 2;
    flagPA2.second = false;
    flagsPA.push_back(flagPA);
    flagsPA.push_back(flagPA2);

    std::pair<int, int> inputImage, inputFilter;
    inputImage.first = 0;
    inputImage.second = 2;
    inputFilter.first = 0;
    inputFilter.second = 3;
    inputPA.push_back(inputImage);
    inputPA.push_back(inputFilter);

    std::pair<bool, int> outputPAp, outputPAp2, outputPAp3, outputPAp4;
    outputPAp.first = true;
    outputPAp.second = 1;
    outputPAp2.first = true;
    outputPAp2.second = 2;
    outputPAp3.first = true;
    outputPAp3.second = 3;
    outputPAp4.first = true;
    outputPAp4.second = 4;
    outputPA.push_back(outputPAp);
    outputPA.push_back(outputPAp2);
    outputPA.push_back(outputPAp3);
    outputPA.push_back(outputPAp4);


    dqPA.push_back(2);
    dqPA.push_back(3);

    TIAinstruction passArgs(static_cast<Instruction>(PASS_THROUGH_2), predStatePA, flagsPA, inputPA, outputPA, 0, dqPA, druEmpty);
    passArgs.setPredUpdates(pUpEmpty);
    t1.addInstruction(passArgs);


    // Pass arguments to multiplier done with convolution step
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpPAD;
    std::vector<pair<int, bool> > flagsPAD;

    std::pair<int, bool> flagPAD;
    flagPAD.first = 3;
    flagPAD.second = true;
    flagsPAD.push_back(flagPAD);

    std::pair<int, bool> pUpPADp, pUpPADp2;
    std::pair<predUpdateType, pair<int, bool> > padp1, padp2;
    pUpPADp.first = 0;
    pUpPADp.second = true;
    pUpPADp2.first = 1;
    pUpPADp2.second = true;
    padp1.first = ANY;
    padp1.second = pUpPADp;
    padp2.first = ANY;
    padp2.second = pUpPADp2;
    pUpPAD.push_back(padp1);
    pUpPAD.push_back(padp2);

    TIAinstruction passArgsDone(static_cast<Instruction>(PASS_THROUGH_2), predStatePA, flagsPAD, inputPA, outputPA, 1, dqPA, druEmpty);
    passArgsDone.setPredUpdates(pUpPAD);
    t1.addInstruction(passArgsDone);

/*
    // Cycle Image not at end
    std::vector<pair<int, bool> > predStateCI;
    std::vector<pair<int, bool> > flagsCI;
    std::vector<pair<int, int> > inputCI;
    std::vector<pair<bool, int> > outputCI;
    std::vector<int> dqCI;
 
    std::pair<int, bool> predStateCIp, predStateCIp2;
    predStateCIp.first = 0;
    predStateCIp.second = true;
    predStateCIp2.first = 1;
    predStateCIp2.second = false;
    predStateCI.push_back(predStateCIp);
    predStateCI.push_back(predStateCIp2);

    std::pair<int, int> inputCIp;
    inputCIp.first = 0;
    inputCIp.second = 2;
    inputCI.push_back(inputCIp);

    std::pair<bool, int> outputCIp;
    outputCIp.first = true;
    outputCIp.second = 1;
    outputCI.push_back(outputCIp);

    std::pair<int, bool> flagCI;
    flagCI.first = 2;
    flagCI.second = false;
    flagsCI.push_back(flagCI);


    dqCI.push_back(2);

    TIAinstruction cycleImage(static_cast<Instruction>(PASS_THROUGH), predStateCI, flagsCI, inputCI, outputCI, 0, dqCI, druEmpty);
    cycleImage.setPredUpdates(pUpEmpty);
    t1.addInstruction(cycleImage);


    // Cycle Image at end of image
     std::vector<pair<predUpdateType, pair<int, bool> > > pUpCID;
    std::vector<pair<int, bool> > flagsCID;

    std::pair<int, bool> flagCID;
    flagCID.first = 2;
    flagCID.second = true;
    flagsCID.push_back(flagCID);

    std::pair<int, bool> pUpCIDp;
    std::pair<predUpdateType, pair<int, bool> > cidp1;
    pUpCIDp.first = 1;
    pUpCIDp.second = true;
    cidp1.first = ANY;
    cidp1.second = pUpCIDp;
    pUpCID.push_back(cidp1);


    TIAinstruction cycleImageDone(static_cast<Instruction>(PASS_THROUGH), predStateCI, flagsCID, inputCI, outputCI, 0, dqCI, druEmpty);
    cycleImageDone.setPredUpdates(pUpCID);
    t1.addInstruction(cycleImageDone);

*/
  // Toss out one image value
    std::vector<pair<int, bool> > predStateTO;
    std::vector<pair<int, int> > inputTO;
    std::vector<pair<bool, int> > outputEmpty;
    std::vector<int> dqTO;
    std::vector<pair<predUpdateType, pair<int, bool> > > pUpTO;
 
    std::pair<int, bool> predStateTOp, predStateTOp2;
    predStateTOp.first = 0;
    predStateTOp.second = true;
    predStateTOp2.first = 1;
    predStateTOp2.second = true;
    predStateTO.push_back(predStateTOp);
    predStateTO.push_back(predStateTOp2);

    std::pair<int, int> inputTOp;
    inputTOp.first = 0;
    inputTOp.second = 2;
    inputTO.push_back(inputTOp);

    dqTO.push_back(2);

    std::pair<int, bool> pUpTOp, pUpTOp2;
    std::pair<predUpdateType, pair<int, bool> > top1, top2;
    pUpTOp.first = 0;
    pUpTOp.second = false;
    pUpTOp2.first = 1;
    pUpTOp2.second = true;
    top1.first = ANY;
    top1.second = pUpTOp;
    top2.first = ANY;
    top2.second = pUpTOp2;
    pUpTO.push_back(top1);
    pUpTO.push_back(top2);

    TIAinstruction tossOut(static_cast<Instruction>(DEQ), predStateTO, flagsEmpty, inputTO, outputEmpty, 0, dqTO, druEmpty);
    tossOut.setPredUpdates(pUpTO);
    t1.addInstruction(tossOut);







    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Initializing the memory streamers

    // Start at the beginning of memory, skipping no data and get a few values
    int beginAddr = 0;
    int beginStride = 1;
    int beginEndAddr = 3;

    // Create and populate the multiChannels that go to the streamers
    multiChannel memIn1, memIn2;

    memIn1.add(&memStart1);
    memIn1.add(&memOut1);
    memIn1.add(&memData1);
    memIn1.add(&base1);
    memIn1.add(&stride1);
    memIn1.add(&end1);

    memIn2.add(&memStart2);
    memIn2.add(&memOut2);
    memIn2.add(&memData2);
    memIn2.add(&base2);
    memIn2.add(&stride2);
    memIn2.add(&end2);

    //MemStream mem1(&memIn1, beginAddr, beginStride, beginEndAddr, false, 0, memory);
    //MemStream mem2(&memIn2, beginEndAddr, beginStride, beginEndAddr + 6, false, 0, memory);



    ////////////////////////////////////////////////////////////////////////////////////////////////

    //Initializing Multiplier module


    
    // Create and populate the multiChannel that will go into the multiplier
    multiChannel multIn;

    multIn.add(&multToTIA);
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

    //mems.push_back(&mem1);
    //mems.push_back(&mem2);
    fifos.push_back(&f1);
    fifos.push_back(&f2);
    //fifos.push_back(&mFin);
    mults.push_back(&m);
    tias.push_back(&t1);


    channels.push_back(&iIn);
    channels.push_back(&fIn);
    channels.push_back(&multToFifo);
    channels.push_back(&tiaToMult1);
    channels.push_back(&tiaToMult2);
    channels.push_back(&tiaToIFifo);
    channels.push_back(&tiaToFFifo);
    channels.push_back(&iFifoOut);
    channels.push_back(&fFifoOut);
    channels.push_back(&multToTIA);
    channels.push_back(&memStart1);
    channels.push_back(&memStart2);
    channels.push_back(&memOut1);
    channels.push_back(&memOut2);
    channels.push_back(&memData1);
    channels.push_back(&memData2);
    channels.push_back(&base1);
    channels.push_back(&base2);
    channels.push_back(&stride1);
    channels.push_back(&stride2);
    channels.push_back(&end1);
    channels.push_back(&end2);
    channels.push_back(&tHelp1);
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
	int whichIndex = 0;

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
		whichIndex = eResult.getIndex();
	    }
	    tias[i]->commit(eResult);
            if(tOut1.getCanPop()){
	        printf("Output: %i\n", tOut1.get().data);
		printf("Number of Cycles: %i\n", totalTime);
	    }
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
	    printf("TIA Inst: %i\n", whichTIA);
	    printf("TIA Index: %i\n", whichIndex);
	}


	// Add a cycle's worth of time to the total time
	totalTime += cycleTime;
    
    }

    return 0;
}



#endif
