/// \file HealthPotion.cpp
/// \brief Code for the healthpotion object class CHealthPotion.

#include "HealthPotion.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Player.h"


/// Create and initialize an healthpotion object given its initial position.
/// \param p Initial position of healthpotion.

CHealthPotion::CHealthPotion(const Vector2& p) : CObject(eSprite::HealthPotion, p) {
    objType = (UINT)eObjectType::HEALTHPOT;
  m_bIsTarget = true;
  m_bStatic = false;
  m_fXScale = m_fYScale = 0.035f; 
  m_fRadius = m_fXScale;
  m_fRoll = 0.0f;
} //constructor

CHealthPotion::~CHealthPotion() {
} //destructor

/// Response to collision, which for a healthpotion means playing a sound and dying
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CHealthPotion::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (m_bDead)return; //already dead, bail out 
  CPlayer* player = (CPlayer*)pObj;
  if (!player->m_bHealthNeeded) {
      return;
  }
  if (pObj && pObj->isPlayer()) { //Collision with the player
    m_bDead = true; //mark object for deletion
    m_pAudio->play(eSound::Health); //CHANGE OUT
    DeathFX();
  }
}

void CHealthPotion::DeathFX() {
  // Left for potential use
}

/// Reader function for position.
/// \return Position.

const Vector2& CHealthPotion::GetPos() const {
  return m_vPos;
} //GetPos

