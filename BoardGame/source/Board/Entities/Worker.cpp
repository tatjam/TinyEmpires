#include "Worker.h"



void Worker::tick(float dt)
{
	if (hasOrder())
	{
		Order order = getActiveOrder();
		
		if(order.type == MOVE)
		{
			moveTo(order.target, getOwner()->game->sets->gSets.threadedPathfinding);
		}
		else if (order.type == MINE)
		{

		}

		finishOrder();
	}
}

Worker::~Worker()
{
}
