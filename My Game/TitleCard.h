#pragma once

#include "Object.h"
#include "Common.h"

class TitleCard : public CObject {
private:
	bool m_nDone;
public:
	TitleCard(const Vector2&);
	~TitleCard();
	void NextFrame();
	bool Finished();
};

