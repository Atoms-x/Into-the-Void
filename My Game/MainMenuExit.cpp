#include "MainMenuExit.h"
#include <cstdlib>

CMainMenuExit::CMainMenuExit(const Vector2& p) : CObject(eSprite::Exit, p) {
	m_bIsSelected = false;
	m_bStatic = true;
	m_fRoll = 0.0f;
	m_nCurrentFrame = 0;
	m_fXScale = 0.45f;
	m_fYScale = 0.45f;
}

CMainMenuExit::~CMainMenuExit(){}

void CMainMenuExit::Select() {
	exit(0);
}

const Vector2& CMainMenuExit::GetPos() const {
	return m_vPos;
}

void CMainMenuExit::Highlight() {
	m_bIsSelected = true;
	m_nCurrentFrame = 1;
}

void CMainMenuExit::Normal() {
	m_bIsSelected = false;
	m_nCurrentFrame = 0;
}