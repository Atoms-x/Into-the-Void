#include "PrevMapPorter.h"
#include "Player.h"

/// <summary>
/// Creates a new porter to the previous map.
/// This is just an object that depending on the state of the level
/// allows the player to go back to the previous map.
/// </summary>
/// <param name="pos"></param>

PrevMapPorter::PrevMapPorter(const Vector2& pos) : CObject(eSprite::PrevPortal, pos){
	m_nCurrentFrame = 0;
	m_nDirection = -1;
	m_fRoll = 0.0f;
	m_bIsActivated = false;
}

PrevMapPorter::~PrevMapPorter() {

}

/// <summary>
/// This direction variable is just where it traverses the list
/// of relevant map objects.
/// </summary>
/// <returns></returns>

int PrevMapPorter::GetDirection() {
	return m_nDirection;
}

/// <summary>
/// Activates the porter, updating its sprite and allowing it to
/// be interacted with.
/// </summary>

void PrevMapPorter::Activate() {
	m_nCurrentFrame = 2;
	m_bIsActivated = true;
}

/// <summary>
/// Controls the collision logic for this object. If the player interacts
/// with it while its activated, it will send the user to the previous map.
/// </summary>
/// <param name="norm"></param>
/// <param name="d"></param>
/// <param name="pObj"></param>

void PrevMapPorter::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
	if (m_bDead) return;
	if (pObj && pObj->isPlayer() && m_bIsActivated) {
		CPlayer* p = (CPlayer*)pObj;
		p->SetPort(m_nDirection);
		m_bDead = true;
	}
}