#pragma once
#include <vector>

class Frame
{
public:
	enum FrameType
	{
		FRAMETYPE_ACK,
		FRAMETYPE_DATA
	};

	Frame();
	virtual ~Frame();

	FrameType Type;
	uint32_t Data;

	uint32_t DestinationMacAddress;
	uint32_t SourceMacAddress;
};

