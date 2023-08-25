#pragma once

#include "Object.h"
#include "Common.h"

class PrevMapPorter : public CObject {
private:
	int m_nDirection;
	bool m_bIsActivated;
public:
	PrevMapPorter(const Vector2&);
	~PrevMapPorter();
	int GetDirection();
	void Activate();
	void CollisionResponse(const Vector2&, float, CObject* = nullptr);
};

