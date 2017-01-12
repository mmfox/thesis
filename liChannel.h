// Header File for the Latency Insensitive Channel
#ifndef LI_CHANNEL_H
#define LI_CHANNEL_H

#include<iostream>
#include<vector>
using namespace std;


// Define a packet - an integer along with a 1 bit tag.  All communication is done through this packet struct
struct packet {
    int data;
    std::bitset<1> tag;
};

class liChannel{
    public:
	liChannel(int maxInFlight);    // Constructor that takes the maximum packets in flight for this channel
	packet get();                  // Gets a packet from the channel
	packet peek();                 // Peeks at a packet in the channel
	void put(packet data);         // Puts a packet into the channel
	bool getCanPush();             // Checks if we can push to the channel
	bool getCanPop();              // Checks if we can pop from this channel
	void setCanPush(bool update);  // Method used by timing model to decide if we can push to a channel
	void setCanPop(bool update);   // Method used by timing model to decide if we can pop from a channel
	int numInFlight();             // Gets number of messages in flight
	int getMax();                  // Returns maxInChannel

    private:
	std::vector<packet> messages;  // Internal in order in flight messages 
	bool canPush;                  // Internal variable for if we can push right now
	bool canPop;                   // Internal variable for if we can pop right now
	int maxInChannel;              // Internal variable for the maximum number of packets in a channel
};

#endif
