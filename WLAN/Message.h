#pragma once
#include <memory>
#include <vector>
#include "Frame.h"

using namespace std;

class Message
{
public:
	Message();
	~Message();

	size_t TransactionId;
	size_t NextFrameToSend;

	vector<shared_ptr<Frame> > Frames;
};

