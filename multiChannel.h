// Header File for the Multi Channel

#ifndef MULTI_CHANNEL_H
#define MULTI_CHANNEL_H

#include<iostream>
#include<vector>
#include"liChannel.cpp"
using namespace std;

class multiChannel{
    public:
	multiChannel();                    // Empty Constructor
	std::vector<liChannel*> channels;  // Vector that holds our liChannel pointers
	void add(liChannel * newChan);     // Add a channel to the multiChannel
	int getNumChannels();              // Get the total number of channels available
};
#endif
