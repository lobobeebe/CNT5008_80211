#include "Node.h"

#include "Ack.h"

Node::Node(uint32_t macAddress) :
	State(IDLE),
	TimeSlotsInStateCounter(0),
	SifsCounter(0),
	DifsCounter(DIFS),
	MacAddress(macAddress),
	BackoffCounter(0),
	ContentionWindow(CW_MIN)
{
}

Node::~Node()
{
}

void Node::AddListener(shared_ptr<Node> node)
{
	ListenerNodes.push_back(node);
}

/**
 * Each time slot, this will be called for each Node
 */
void Node::OnTimeSlot()
{
	// Switch on the current state of the Node State Machine
	switch (State)
	{
		// Continue sending current message
		case SENDING:
			OnTimeSlotSending();
			SendNextFrame();
			break;
		// Nothing to do in IDLE
		case IDLE:
			OnTimeSlotIdle();
		default:
			break;
	}

	TimeSlotsInStateCounter++;
}

void Node::SendNextFrame(shared_ptr<Frame> frame)
{
	for (size_t i = 0; i < ListenerNodes.size(); ++i)
	{
		if (shared_ptr<Node> listener = ListenerNodes[i].lock())
		{
			listener->ReceivedFrames.push(frame);
		}
	}
}

queue<weak_ptr<Frame> > Node::GetAndClearReceivedFrames()
{
	// Clear the queue
	queue<weak_ptr<Frame> > empty;
	swap(ReceivedFrames, empty);

	return empty;
}

void Node::OnTimeSlotFallingEdge()
{
	queue<weak_ptr<Frame> > receivedFrames = GetAndClearReceivedFrames();

	if (!receivedFrames.empty())
	{
		// Process received frames only if:
		// - Exactly one was received. Otherwise interference occurred.
		// - The destination of the frame was this node.
		// - The frame is valid
		if (receivedFrames.size() == 1)
		{
			if (shared_ptr<Frame> frame = receivedFrames.front.lock() && frame->DestinationMacAddress == MacAddress)
			{
				// Waiting on an ack
				if (frame->Type == Frame::FRAMETYPE_ACK && MessageToBeSent)
				{
					// If an ack was received and it matches the sent message, done!
					if (frame->Data == MessageToBeSent->TransactionId)
					{
						MessageToBeSent.reset();
					}
				}
				// Data that needs to be acked
				if (frame->Type == Frame::FRAMETYPE_DATA)
				{
					// Reset the SIFS counter to send an ack
					SifsCounter = SIFS;
				}
			}
		}

		// Reset the DIFS counter since the medium was no longer idle
		DifsCounter = DIFS;
	}
	// If nothing was received and messages are pending, wait for Backoff
	else if (receivedFrames.size() == 0)
	{

	}

}

size_t Node::GetRandomBackoff()
{
	size_t randomBackoff = DIFS;

	std::default_random_engine e1(RandomDevice());
	std::uniform_int_distribution<int> uniform_dist(0, ContentionWindow);
	int mean = uniform_dist(e1);
}

void Node::OnTimeSlotRisingEdge()
{
	size_t PreviousSifsCounter = 0;
	// Frames are still left to be sent
	if (MessageToBeSent && MessageToBeSent->NextFrameToSend < MessageToBeSent->Frames.size())
	{
		SendNextFrame(MessageToBeSent->Frames[MessageToBeSent->NextFrameToSend]);
	}

	// Decrement counters and send when needed
	if (PreviousSifsCounter > 0)
	{

	}


	// Lock on the Frame to access it
	weak_ptr<Frame> weakFrame = GetAndClearReceivedFrames();
	if (shared_ptr<Frame> frame = weakFrame.lock())
	{
		// Only process a frame if this is the destination
		if (frame->DestinationMacAddress == MacAddress)
		{
			// Waiting on an ack
			if (frame->Type == Frame::FRAMETYPE_ACK && MessageToBeSent)
			{
				// If an ack was received and it matches the sent message, done!
				if (frame->Data == MessageToBeSent->TransactionId)
				{
					MessageToBeSent.reset();
				}
			}
		}
	}
	// Idle medium for DIFS? Start sending next frame
	else if (TimeSlotsInStateCounter >= DIFS)
	{
		// Wait for Backoff
		SetState(WAITING_FOR_BACKOFF);

		if (!PendingMessages.empty())
		{
			MessageToBeSent = move(PendingMessages.front());
			PendingMessages.pop();

			SendNextFrame();
		}
	}
}

void Node::SetState(NodeState state)
{
	State = state;
	TimeSlotsInStateCounter = 0;
}