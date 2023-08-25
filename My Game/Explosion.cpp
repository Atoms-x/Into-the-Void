/// \file Explosion.cpp
/// \brief Code for the explosion object class CExplosion.

#include "Explosion.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"
#include "Player.h"

/// Create and initialize an Explosion object given its initial position.
/// \param t Sprite type of explosion.
/// \param p Initial position of explosion.

CExplosion::CExplosion(eSprite t, const Vector2& p) : CObject(t, p) {
  objType = (UINT)eObjectType::EXPLOSION;
  m_bStatic = true;
  m_bIsTarget = false;

  m_fXScale = m_fYScale = 0.2f;
  m_fRadius = m_fXScale;

  m_fRoll = m_pPlayer->GetFacingVectorAtShot() + XM_PI/2;
  m_vPos += -Vector2(20.0f * cos(m_pPlayer->GetFacingVectorAtShot())), 20.0f * sin(m_pPlayer->GetFacingVectorAtShot());

  m_pExplosionEvent = new LEventTimer(0.06f);
} //constructor

CExplosion::~CExplosion() {
  if (m_pExplosionEvent) {
    delete m_pExplosionEvent;
    m_pExplosionEvent = nullptr;
  }
} //destructor

/// Move and advance current frame number. 

void CExplosion::move() {
  if (m_bDead)
    return;
  else {
    UpdateFramenumber();
  }
}

/// Update the frame number in the animation sequence. Kill Object after animation ends

void CExplosion::UpdateFramenumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (n > 1 && m_pExplosionEvent && m_pExplosionEvent->Triggered()) {
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    if (m_nCurrentFrame == (n - 1)) {
      m_bDead = true;
    }
  }
}