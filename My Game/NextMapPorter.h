#pragma once

#include "Object.h"
#include "Common.h"

class NextMapPorter : public CObject {
private:
	int m_nDirection;
	bool m_bIsActivated;
public:
	NextMapPorter(const Vector2&);
	~NextMapPorter();
	int GetDirection();
	void Activate();
	void CollisionResponse(const Vector2&, float, CObject* = nullptr);
};

