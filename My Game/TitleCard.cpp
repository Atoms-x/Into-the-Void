#include "TitleCard.h"
#include "Helpers.h"

TitleCard::TitleCard(const Vector2& pos) : CObject(eSprite::Titlecard, pos) {
	m_fRoll = 0.0f;
	m_nDone = false;
	objType = (UINT)eObjectType::UI;
}

TitleCard::~TitleCard() {

}

void TitleCard::NextFrame() {
	if (m_nDone) return;
	const UINT n_frames = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex);
	m_nCurrentFrame = Clamp(m_nCurrentFrame + 1, n_frames, 0);
	if (m_nCurrentFrame == n_frames) {
		m_nDone = true;
		m_bDead = true;
	}
}

bool TitleCard::Finished() {
	return m_nDone;
}