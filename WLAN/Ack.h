#pragma once
#include "Frame.h"

class Ack : public Frame
{
public:
	Ack();
	~Ack();

	size_t AckTransactionId;
};

