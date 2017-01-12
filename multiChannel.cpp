///////////////////////////////////
// MULTI Channel Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 1/25/15
//////////////////////////////////

/*
This is the module for the multichannel module.
It is basically a wrapper around the liChannels so that
we can pass in only one multichannel into each module, 
instead of a bunch of individual liChannels.
*/

#ifndef MULTI_CHANNEL_CPP
#define MULTI_CHANNEL_CPP

#include <iostream>
#include <vector>
using namespace std;

#include"multiChannel.h"

// Empty Constructor
multiChannel::multiChannel(){
}


// Add a liChannel to the channel vector by pushing to the back
void multiChannel::add(liChannel * newChan){
    this->channels.push_back(newChan);
};

// Return the number of channels in the multiChannel
int multiChannel::getNumChannels(){
    return this->channels.size();
};


#endif
