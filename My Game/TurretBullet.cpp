/// \file Bullet.cpp
/// \brief Code for the bullet object class CTurretBullet.

#include "TurretBullet.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"
#include "ObjectManager.h"

/// Create and initialize a bullet object given its initial position.
/// \param t Sprite type of bullet.
/// \param p Initial position of bullet.

CTurretBullet::CTurretBullet(eSprite t, const Vector2& p) : CObject(t, p) {
  objType = (UINT)eObjectType::BULLET;
  m_bStatic = false;
  m_bIsTarget = false;

  m_fXScale = m_fYScale = 0.75f;

  m_pFireballEvent = new LEventTimer(0.12f);
} //constructor

CTurretBullet::~CTurretBullet() {
  if (m_pFireballEvent) {
    delete m_pFireballEvent;
    m_pFireballEvent = nullptr;
  }
} //destructor

/// Move and advance current frame number. Create smoke effects as well

void CTurretBullet::move() {
  if (m_bDead)
    return;
  else {
    SmokeFX();
    const float t = m_pTimer->GetFrameTime();
    m_vPos += m_vVelocity * t;
    UpdateFramenumber();
  }
}

/// Creates a trailing smoke effect for the bullets 

void CTurretBullet::SmokeFX() {
  const int count = m_pRandom->randn(0, 10); //gen random amount of dust particles
  UINT randColor, randSize, randAlpha, randSprite; //random vars
  float colorVal, alphaVal;
  LParticleDesc2D dust; //particle descriptor
  for (int i = 0; i < count; i++) {
    dust.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;
    dust.m_fRoll = m_fFacingVector;

    //fade properties
    dust.m_fFadeOutFrac = 0.8f;
    dust.m_fFadeInFrac = 0.2f;

    //get random position within the player sprite
    dust.m_vPos = GenRandomPosition(0.1f);

    dust.m_vPos += -Vector2(20.0f * cos(m_fRoll), 20.0f * sin(m_fRoll));

    randColor = m_pRandom->randn(0, 5);     //random color 
    randAlpha = m_pRandom->randn(0, 12);    //random alpha
    randSize = m_pRandom->randn(1, 12);     //random size
    randSprite = m_pRandom->randn(0, 5);     //random size

    colorVal = 1.0f - (0.1f * (float)randColor); //calc value to use for color
    alphaVal = 0.05f * (float)randAlpha;         //calc value to use for alpha
    dust.m_f4Tint = XMFLOAT4(colorVal, colorVal, colorVal, alphaVal);

    dust.m_fLifeSpan = 0.07f * (float)randAlpha;

    dust.m_fMaxScale = 0.04f * (float)randSize; //apply random scale
    if (randSprite == 0) {
      dust.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;
      dust.m_fMaxScale = 0.05f * (float)randSize; //apply random scale
    }
    else {
      dust.m_nSpriteIndex = (UINT)eSprite::SmoothCircle;
      dust.m_fMaxScale = 0.02f * (float)randSize; //apply random scale
    }

    //spawn particle
    if (dust.m_nSpriteIndex)
      m_pParticleEngine->create(dust);
  } //for

}

/// Update the frame number in the animation sequence.

void CTurretBullet::UpdateFramenumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (n > 1 && m_pFireballEvent && m_pFireballEvent->Triggered()) {
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
  }
}

/// Response to collision, which for a bullet means playing a sound and a
/// explosion object and then dying.  
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CTurretBullet::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (pObj == nullptr) //collide with edge of world
    m_pAudio->play(eSound::Thump);

  //bullets die on collision
  if (!m_bDead) {
    if (!pObj) {
      m_pObjectManager->create(eSprite::Explosion3, m_vPos);
      m_bDeadCheck = true;
      m_bDead = true; //mark object for deletion
    }

    else
      switch (pObj->GetType()) {
      case (UINT)eObjectType::PLAYER:
      case (UINT)eObjectType::SLIME:
      case (UINT)eObjectType::RABITE:
      case (UINT)eObjectType::OAKSEED:
      case (UINT)eObjectType::TURRET:
      case (UINT)eObjectType::LOCKBLOCK:
      case (UINT)eObjectType::BULLET: {
        m_pObjectManager->create(eSprite::Explosion3, m_vPos);
        m_bDead = true;
      }break;
      default:
        //CObject::CollisionResponse(norm, d, pObj);
        break;
      }
  } //if
} //CollisionResponse
