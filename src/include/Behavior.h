#ifndef BEHAVIOR_H
#define BEHAVIOR_H

class IBehavior
{
public:
	//Return if you want to be removed or not
	virtual bool act(float dt) = 0;
};

#endif