#pragma once

#include "Object.h"
#include "Common.h"

class EndingScreen : public CObject {
public:
	EndingScreen(const Vector2&);
	~EndingScreen();
	virtual void CollisionResponse();
};

