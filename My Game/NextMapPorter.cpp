#include "NextMapPorter.h"
#include "Player.h"

/// <summary>
/// Constructor for the next map porter object.
/// This object is similar to the PrevMapPorter except
/// it points to the next element in the map list instead
/// of the previous.
/// </summary>
/// <param name="pos"></param>

NextMapPorter::NextMapPorter(const Vector2& pos) : CObject(eSprite::NextPortal, pos) {
	m_nCurrentFrame = 0;
	m_nDirection = 1;
	m_fRoll = 0.0f;
	m_bIsActivated = false;
}

NextMapPorter::~NextMapPorter() {
}

/// <summary>
/// Returns the direction of this object, which should be 1
/// </summary>
/// <returns></returns>

int NextMapPorter::GetDirection() {
	return m_nDirection;
}


/// <summary>
/// Activates this object so it can be used for porting
/// the player to the next map.
/// </summary>

void NextMapPorter::Activate() {
	m_nCurrentFrame = 1;
	m_bIsActivated = true;
}


/// <summary>
/// If the player interacts with this object while its activated
/// it will attempt to send them to the next map in the list.
/// </summary>
/// <param name="norm"></param>
/// <param name="d"></param>
/// <param name="pObj"></param>

void NextMapPorter::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
	if (m_bDead) return;
	if (pObj && pObj->isPlayer()) {
		if (m_bIsActivated) {
			CPlayer* p = (CPlayer*)pObj;
			p->SetPort(m_nDirection);
			m_bDead = true;
		}
	}
}

