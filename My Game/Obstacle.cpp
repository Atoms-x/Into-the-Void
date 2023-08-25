#include "Obstacle.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Helpers.h"
#include "ComponentIncludes.h"

Obstacle::Obstacle(const Vector2& pos, UINT persistence, eSprite sprite) : CObject(sprite, pos) {
	objType = (UINT)eObjectType::OBSTACLE;
	m_nPersistenceTimer = persistence;
	m_bStatic = true;
}

Obstacle::~Obstacle() {
	///< Just do destructor things
}

void Obstacle::FixBoundingBox(float radius) {
	m_fRadius = radius;
}

/// <summary>
/// This function updates the Obstacle's current animation frame
/// based off of the value in m_nPersistenceTimer.
/// Every time it is called, it increments m_nPersistenceLoop by 1
/// until it reaches the value of the timer. Then, it increments the
/// value of m_nCurrentFrame until it loops back around to frame 0.
/// This is all only done if m_nPersistenceTimer is greater than 0.
/// </summary>

void Obstacle::UpdateFrameNumber() {
	if (m_nPersistenceTimer == 0) {
		return;
	}
	else {
		const size_t n = m_pRenderer->GetNumFrames(m_nSpriteIndex);
		m_nPersistenceLoop = (m_nPersistenceLoop == m_nPersistenceTimer) ? 0 : m_nPersistenceLoop + 1;
		if (m_nPersistenceLoop == 0) {
			m_nCurrentFrame = (m_nCurrentFrame == n - 1) ? 0 : m_nCurrentFrame + 1;
		}
	}
}

bool Obstacle::InDamageFrame() {
	return m_nCurrentFrame == 0;
}