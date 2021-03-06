
#include <iostream>
#include <vector>
#include "Node.h"

using namespace std;


int main()
{
	size_t NumNodes = 5;
	size_t NumTimeSteps = 100;

	vector<shared_ptr<Node> > Nodes;

	// Create nodes
	for (size_t i = 0; i < NumNodes; ++i)
	{
		Nodes.push_back(shared_ptr<Node>(new Node()));

		unique_ptr<Message> msg(new Message());
		msg->Frames.push_back(shared_ptr<Frame>(new Frame()));

		Nodes[i]->PendingMessages.push(move(msg));
	}

	// Add all nodes as listeners
	for (size_t i = 0; i < Nodes.size(); ++i)
	{
		for (size_t j = 0; j < NumNodes; ++j)
		{
			Nodes[i]->AddListener(Nodes[j]);
		}
	}

	// Simulate time steps
	for (size_t i = 0; i < NumTimeSteps; ++i)
	{
		// Each Node
		for (size_t i = 0; i < Nodes.size(); ++i)
		{
			Nodes[i]->OnTimeSlot();
		}
	}
}
