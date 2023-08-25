#pragma once

#include "Object.h"
#include "Common.h"

class FadeOutObject : public CObject {
private:
	bool m_bDoneAnimating;
public:
	FadeOutObject(const Vector2&);
	~FadeOutObject();
	void UpdateFrameNumber();
	bool IsDoneAnimating();
	virtual void move();
};

