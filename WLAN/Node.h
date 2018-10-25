#pragma once
#include <memory>
#include <queue>
#include <random>
#include <vector>

#include "Message.h"
#include "Frame.h"

using namespace std;

class Node
{
public:
	enum NodeState
	{
		IDLE, // No messages to be sent
		SENDING, // Currently sending a message
		WAITING_FOR_DIFS,
		WAITING_FOR_BACKOFF, // Waiting for the backoff period to send a message
		WAITING_FOR_CTS // Waiting for the Clear to Send signal
	};

	Node(uint32_t macAddress);
	~Node();

	/** Observer Pattern **/

	// Adds a listener node to all sent messages. Simulates a device coming into range.
	void AddListener(shared_ptr<Node> node);

	// Gets the last received frame. Returns NULL if more than one frame was received in a single time slot.
	queue<weak_ptr<Frame> > GetAndClearReceivedFrames();

	vector<weak_ptr<Node> > ListenerNodes;
	queue<weak_ptr<Frame> > ReceivedFrames;

	/** State Machine **/
	void OnTimeSlotFallingEdge();
	void OnTimeSlotRisingEdge();

	void SetState(NodeState state);
	NodeState State;

	/** Time Slot Counters **/
	// The number of time slots that the medium has been idle
	size_t TimeSlotsInStateCounter;

	/** State: Sending **/
	void SendNextFrame(shared_ptr<Frame> frame);

	unique_ptr<Message> MessageToBeSent;
	queue<unique_ptr<Message> > PendingMessages;

	/** MAC Address for routing */
	uint32_t MacAddress;

private:
	// Constants for processing
	const size_t SIFS = 1;
	const size_t DIFS = 2;
	const size_t CW_MIN = 8;

	// The back off counter for the next message
	size_t SifsCounter;
	size_t DifsCounter;
	size_t BackoffCounter;
	size_t ContentionWindow;

	/** Helper function to get a random Backoff */
	random_device RandomDevice;
	size_t GetRandomBackoff();
};

