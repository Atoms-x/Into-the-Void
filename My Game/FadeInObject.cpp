#include "FadeInObject.h"
#include "Helpers.h"
#include "Common.h"

/// <summary>
/// The FadeInObject is just an object that
/// covers the screen, applying a fade from black->clear 
/// effect, and then leaves.
/// </summary>
/// <param name="pos"></param>

FadeInObject::FadeInObject(const Vector2& pos) : CObject(eSprite::FadeIn, pos) {
	m_fRoll = 0.0f;
	m_nCurrentFrame = 0;
	m_bDoneAnimating = false;
	m_bIsTarget = false;
	m_bStatic = true;
	m_fXScale = 3.0f;
	m_fYScale = 3.0f;
	objType = (UINT)eObjectType::UI;
}

FadeInObject::~FadeInObject() {

}

/// <summary>
/// Controls the animation for the object.
/// </summary>

void FadeInObject::UpdateFrameNumber() {
	UINT t = m_nCurrentFrame;
	m_nCurrentFrame = Clamp(m_nCurrentFrame + 1, m_pRenderer->GetNumFrames(m_nSpriteIndex), 0);
	printf("Sprite Index: %u", m_nSpriteIndex);
	if (t == m_nCurrentFrame) {
		printf("Done aniamting fadein\n");
		m_bDoneAnimating = true;
		m_bDead = true;
	}
}

/// <summary>
/// If this object is fully faded to clear, this will return
/// true, otherwise, false
/// </summary>
/// <returns></returns>

bool FadeInObject::IsDoneAnimating() {
	return m_bDoneAnimating;
}

/// <summary>
/// This is used to just update the frame number.
/// </summary>

void FadeInObject::move() {
	UpdateFrameNumber();
}

/// <summary>
/// Intentionally empty as it doesn't interact with anything
/// and shouldn't
/// </summary>

void FadeInObject::CollisionResponse() {
	return;
}
