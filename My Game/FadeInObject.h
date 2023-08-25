#pragma once

#include "Object.h"
#include "Common.h"

class FadeInObject : public CObject {
private:
	bool m_bDoneAnimating;
public:
	FadeInObject(const Vector2&);
	~FadeInObject();
	void UpdateFrameNumber();
	bool IsDoneAnimating();
	virtual void move();
	virtual void CollisionResponse();
};
