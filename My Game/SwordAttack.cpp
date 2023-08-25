/// \file SwordAttack.cpp
/// \Code for the Sword attack object class CSwordAttack

#include "SwordAttack.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"

/// Create and initialize a sword attack object given its intial position
/// \param t Sprite type of sword attack
/// \param p Initial position of the sword attack

CSwordAttack::CSwordAttack(eSprite t, const Vector2& p) : CObject(t, p) {
  objType = (UINT)eObjectType::SWORD;
  m_fYScale = 2.0f;
  m_fXScale = 3.0f;
  m_bStatic = false;
  m_bIsTarget = false;
} //constructor

/// Response to collision, which for a sword means playing a sound and a
/// particle effect then dying.
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CSwordAttack::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (pObj == nullptr) //collide with edge of world
    m_pAudio->play(eSound::Impact);

  //bullets die on collision
  if (!m_bDead) {
    if (!pObj)
      m_bDead = true; //mark object for deletion
    else
      switch (pObj->GetType()) {
      case (UINT)eObjectType::SLIME:
      case (UINT)eObjectType::TURRET:
      case (UINT)eObjectType::LOCKBLOCK:
      case (UINT)eObjectType::BULLET:
        m_bDead = true;
        DeathFX();
        break;
      default:
        CObject::CollisionResponse(norm, d, pObj);
        //m_bDead = true;
        break;
      }
  } //if
} //CollisionResponse

//Kills sword attack after .2 seconds if it doesn't hit anything
void CSwordAttack::move() {
  if (Timer() > 20) {
    m_bDead = true;
  }
}

//Timer function for killing sword attack object if it doesn't hit anything
UINT CSwordAttack::Timer() {
  const float t = m_pTimer->GetFrameTime();
  const float delta = 175.0f * t;
  m_nAttackTimer += delta;
  return m_nAttackTimer;
}

/// Create a particle effect to mark an impact with the sword attack
void CSwordAttack::DeathFX() {
  LParticleDesc2D d; //particle descriptor

  d.m_nSpriteIndex = (UINT)eSprite::Smoke;
  d.m_vPos = m_vPos;
  d.m_fLifeSpan = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_fScaleInFrac = 0.2f;
  d.m_fFadeOutFrac = 0.8f;
  d.m_fScaleOutFrac = d.m_fFadeOutFrac;

  m_pParticleEngine->create(d); //create particle
} //DeathFX