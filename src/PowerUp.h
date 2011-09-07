#ifndef POWERUP_H
#define POWERUP_H

class IPowerUp
{
public:
	virtual void start() = 0;
	virtual bool update(float dt) = 0;
	virtual void reset() = 0;
	virtual void finish() = 0;
	float getTimeRemaining() { return mTimeLeft; }
protected:
	float mTimeLeft;
};

#endif
