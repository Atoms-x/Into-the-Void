#include "EndingScreen.h"

EndingScreen::EndingScreen(const Vector2& pos) : CObject(eSprite::EndingScreen, pos) {
	objType = (UINT)eObjectType::UI;
	m_fRoll = 0.0f;
	m_bStatic = true;
}

EndingScreen::~EndingScreen() {

}

void EndingScreen::CollisionResponse() {

}
