#include "GameOverObject.h"

GameOverObject::GameOverObject(const Vector2& pos) : CObject(eSprite::GameOver, pos) {
	m_bStatic = true;
	m_fRoll = 0.0f;
}

GameOverObject::~GameOverObject() {
}