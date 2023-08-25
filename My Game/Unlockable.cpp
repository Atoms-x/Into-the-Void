#include "Unlockable.h"

CUnlockable::CUnlockable(const Vector2& pos, eSprite sprite) : CObject(sprite, pos) {
	objType = (UINT)eObjectType::LOCKBLOCK;
	m_bStatic = false;
	locked = true; //locked by default
} //Constructor

CUnlockable::CUnlockable(const Vector2& pos, eSprite sprite, bool unlocked) : CObject(sprite, pos) {
	objType = (UINT)eObjectType::LOCKBLOCK;
	m_bStatic = true;
	locked = unlocked; //locked by default
} //Constructor

CUnlockable::~CUnlockable() {} //Destructor

void CUnlockable::UpdateFrameNumber() {
	const size_t n = m_pRenderer->GetNumFrames(m_nSpriteIndex);
	if (this->m_nCurrentFrame == 0 /* && !m_bStatic*/) {	//not sure if the static check is needed
		if (m_nCurrentFrame++ >= n)
			HandleFrameLoop(n);
	}
} //UpdateFrameNumber

void CUnlockable::HandleFrameLoop(size_t n) {
	//override to handle frame or sprite changes
	m_nCurrentFrame = 0;
} //HandleFrameLoop


void CUnlockable::Unlock() {
	bool locked = false; 
	UnlockFx();
} //Unlock

void CUnlockable::Lock() {
	bool locked = true;
	LockFx();
} //Lock

void CUnlockable::UnlockFx() {
	//handle particle fx by overriding
} //UnlockFx

void CUnlockable::LockFx() {
	//handle particle fx by overriding
} //LockFx

