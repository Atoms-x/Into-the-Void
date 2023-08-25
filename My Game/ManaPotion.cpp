/// \file ManaPotion.cpp
/// \brief Code for the Manapotion object class CManaPotion.

#include "ManaPotion.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Player.h"

/// Create and initialize an Manapotion object given its initial position.
/// \param p Initial position of Manapotion.

CManaPotion::CManaPotion(const Vector2& p) : CObject(eSprite::ManaPotion, p) {
    objType = (UINT)eObjectType::MANAPOT;
    m_bIsTarget = true;
    m_bStatic = false;
    m_fXScale = m_fYScale = 0.035f;
    m_fRadius = m_fXScale;
    m_fRoll = 0.0f;
} //constructor

CManaPotion::~CManaPotion() {
} //destructor

/// Response to collision, which for a manapotion means playing a sound and dying
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CManaPotion::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (m_bDead)return; //already dead, bail out 

  if (pObj && pObj->isPlayer()) { //Collision with the player
    CPlayer* player = (CPlayer*) pObj;
    if (!player->m_bManaNeeded) {
        return;
    }
    m_bDead = true; //mark object for deletion
    m_pAudio->play(eSound::Acquire); //CHANGE OUT
    DeathFX();
  }
}

void CManaPotion::DeathFX() {
  // Left for potential use
}

/// Reader function for position.
/// \return Position.

const Vector2& CManaPotion::GetPos() const {
  return m_vPos;
} //GetPos

