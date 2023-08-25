#include "LockTest.h"

//CLockTest::CLockTest(const Vector2& pos) : CObject((UINT)eSprite::LockTest, pos) {
//	CObject::objType = (UINT)eObjectType::LOCKBLOCK;
//	CObject::m_bStatic = true;
//	locked = true; //locked by default

//Constructor
CLockTest::CLockTest(const Vector2& pos, eSprite sprite) 
	: CUnlockable(pos, sprite) {
	m_fRoll = XM_1DIV2PI;
	m_fXScale = m_fYScale = 2.0f;
} //Constructor

void CLockTest::UnlockFx() {
	if (CObject::m_nCurrentFrame == 0)
		CObject::m_nCurrentFrame = 1;
	m_pAudio->play(eSound::Acquire); //CHANGE OUT
	CObject::objType = (UINT)eObjectType::UI;	//change type to not collide with player
} //UnlockFx

void CLockTest::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
	if (pObj && pObj->GetType() == (UINT)eObjectType::PLAYER) {
		if (m_pPlayer->HasKey() && m_bInteract) {
			m_pPlayer->UseKey();
			Unlock();
		} //if
	} //if
} //CollisionResponse