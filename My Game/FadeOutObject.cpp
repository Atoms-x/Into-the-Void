#include "FadeOutObject.h"
#include "Helpers.h"
#include "Common.h"

FadeOutObject::FadeOutObject(const Vector2& pos) : CObject(eSprite::FadeOut, pos){
	m_fRoll = 0.0f;
	m_nCurrentFrame = 0;
	m_bDoneAnimating = false;
	m_bStatic = true;
	m_bIsTarget = false;
}

FadeOutObject::~FadeOutObject() {
}

void FadeOutObject::UpdateFrameNumber() {
	UINT t = m_nCurrentFrame;
	m_nCurrentFrame = Clamp(m_nCurrentFrame + 1, m_pRenderer->GetNumFrames(m_nSpriteIndex), 0);
	if (t == m_nCurrentFrame) {
		m_bDoneAnimating = true;
		m_bDead = true;
	}
}

bool FadeOutObject::IsDoneAnimating() {
	return m_bDoneAnimating;
}

void FadeOutObject::move() {
	UpdateFrameNumber();
}