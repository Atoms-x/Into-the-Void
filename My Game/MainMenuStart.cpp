#include "MainMenuStart.h"
#include "ObjectManager.h"
#include "Game.h"

CMainMenuStart::CMainMenuStart(const Vector2& p) : CObject(eSprite::Start, p) {
	m_bIsSelected = true;
	m_bStatic = true;
	m_fRoll = 0.0f;
	m_nCurrentFrame = 1;
	m_fXScale = 0.5f;
	m_fYScale = 0.5f;
}

CMainMenuStart::~CMainMenuStart(){}

bool CMainMenuStart::Select() {
	return m_bIsSelected;
}

void CMainMenuStart::Highlight() {
	m_bIsSelected = true;
	m_nCurrentFrame = 1;
}

void CMainMenuStart::Normal() {
	m_bIsSelected = false;
	m_nCurrentFrame = 0;
}