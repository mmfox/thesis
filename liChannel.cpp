///////////////////////////////////
// Latency Insensitive Channel Module
// Author - Matt Fox
// Email  - matthewmfox@gmail.com
// Date   - 10/6/14
//////////////////////////////////


/*
This is the module for the latency insensitve channel.
It is designed to allow for any variable number of packets in flight.
The timing model can decide arbitrary delays for when the channel can push or pop.
The channel also provides FIFO properties.
*/



#ifndef LICHANNEL_CPP
#define LICHANNEL_CPP

#include <iostream>
#include <vector>
using namespace std;

#include "liChannel.h"

// Constructor that takes the maximum packets in flight for this channel
liChannel::liChannel(int maxInFlight){ 
    this->canPush = true; // Every liChannel should accept at least one packet
    this->canPop = false; // We can't pop yet because no packets are in the system
    this->maxInChannel = maxInFlight;
};

// Peeks at a packet in the channel
packet liChannel::peek(){
    return this->messages.front();
}

// Gets a packet from the channel
packet liChannel::get(){
    packet value = this->messages.front();
    this->messages.erase(this->messages.begin()); // Erase from the front to keep Fifo properties
    return value;
};

// Gets number of messages in flight
int liChannel::numInFlight(){
    return this->messages.size();
}

// Puts a packet into the channel
void liChannel::put(packet data){
    this->messages.push_back(data);
};

// Checks if we can push to the channel
bool liChannel::getCanPush(){
    return this->canPush;
};

// Checks if we can pop from this channel
bool liChannel::getCanPop(){
    return this->canPop;
};

// Returns maxInChannel
int liChannel::getMax(){
    return this->maxInChannel;
}

// Method used by timing model to decide if we can push to a channel
void liChannel::setCanPush(bool update){
    this->canPush = update;
};

// Method used by timing model to decide if we can pop from a channel
void liChannel::setCanPop(bool update){
    this->canPop = update;
};


#endif
