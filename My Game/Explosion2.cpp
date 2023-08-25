/// \file Explosion2.cpp
/// \brief Code for the explosion2 object class CExplosion2.

#include "Explosion2.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"


/// Create and initialize an Explosions object given its initial position.
/// \param t Sprite type of explosion.
/// \param p Initial position of explosion.

CExplosion2::CExplosion2(eSprite t, const Vector2& p) : CObject(t, p) {
  objType = (UINT)eObjectType::EXPLOSION;
  m_bStatic = true;
  m_bIsTarget = false;

  m_fXScale = m_fYScale = 2.0f;
  m_fRadius = m_fXScale;

  m_fRoll = 0.0f;

  m_pExplosionEvent = new LEventTimer(0.06f);
} //constructor

CExplosion2::~CExplosion2() {
  if (m_pExplosionEvent) {
    delete m_pExplosionEvent;
    m_pExplosionEvent = nullptr;
  }
} //destructor

/// Move and advance current frame number. 

void CExplosion2::move() {
  if (m_bDead)
    return;
  else {
    UpdateFramenumber();
  }
}

/// Update the frame number in the animation sequence. Kill object after animation ends

void CExplosion2::UpdateFramenumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (n > 1 && m_pExplosionEvent && m_pExplosionEvent->Triggered()) {
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    if (m_nCurrentFrame == (n - 1)) {
      m_bDead = true;
    }
  }
}