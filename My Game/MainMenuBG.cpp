#include "MainMenuBG.h"

CMainMenuBG::CMainMenuBG(const Vector2& p) : CObject(eSprite::BG, p) {
	m_bStatic = true;
	m_fRoll = 0.0f;
}
CMainMenuBG::~CMainMenuBG() {
}

const Vector2& CMainMenuBG::GetPos() const {
	return m_vPos;
}
void CMainMenuBG::Begin() {

}