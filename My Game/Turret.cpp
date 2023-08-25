/// \file Turret.cpp
/// \brief Code for the turret object class CTurret.

#include "Turret.h"
#include "ComponentIncludes.h"
#include "ObjectManager.h"
#include "TileManager.h"
#include "Player.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"

/// Create and initialize a turret object given its position.
/// \param p Position of turret.

CTurret::CTurret(eSprite t, const Vector2& p): CObject(t, p){
  objType = (UINT)eObjectType::TURRET;
  m_bStatic = true; //turrets are static
  m_fRoll = 0;
  m_pTowerIdle = new LEventTimer(0.12f);
  m_nCurrentFrame = 0;
} //constructor

CTurret::~CTurret() {
  if (m_pTowerIdle) {
    delete m_pTowerIdle;
    m_pTowerIdle = nullptr;
  }
}

/// Rotate the turret and fire the gun at at the closest available target if
/// there is one, and rotate the turret at a constant speed otherwise.

void CTurret::move() {
  if (m_pPlayer) { //safety
    const float r = ((CTurret*)m_pPlayer)->m_fRadius; //player radius

    if (m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r)) //player visible
      RotateTowards(m_pPlayer->m_vPos);
  } //if

  m_fFacingVector += 0.2f * m_fRotSpeed * XM_2PI * m_pTimer->GetFrameTime(); //rotate
  NormalizeAngle(m_fFacingVector); //normalize to [-pi, pi] for accuracy
  NormalizeAngle(m_fRoll); //normalize to [-pi, pi] for accuracy
  UpdateFrameNumber();
} //move

/// Rotate the turrent towards a point and file the gun if it is facing
/// sufficiently close to it.
/// \param pos Target point.

void CTurret::RotateTowards(const Vector2& pos) {
  const Vector2 v = pos - m_vPos; //vector from target to turret
  const float theta = atan2f(v.y, v.x); //orientation of that vector
  float diff = m_fFacingVector - theta; //difference with turret orientation
  NormalizeAngle(diff); //normalize to [-pi, pi]

  //set rotation speed from diff

  const float fAngleDelta = 0.02f; //allowable angle discrepancy
  const float fTrackingSpeed = 0.5f; //rotation speed when tracking

  //if (diff > fAngleDelta)m_fRotSpeed = m_fRoll = -fTrackingSpeed; //clockwise
  //else if (diff < -fAngleDelta)m_fRotSpeed = m_fRoll = fTrackingSpeed; //counterclockwise
  if (diff > fAngleDelta)m_fRotSpeed = -fTrackingSpeed; //clockwise
  else if (diff < -fAngleDelta)m_fRotSpeed =  fTrackingSpeed; //counterclockwise
  else m_fRotSpeed = 0; //stop rotating

  //fire gun if pointing approximately towards target

  if (fabsf(diff) < fAngleDelta && m_pGunFireEvent->Triggered())
    m_pObjectManager->FireGun(this, eSprite::Bullet2);
} //RotateTowards

void CTurret::UpdateFrameNumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (n > 1 && m_pTowerIdle && m_pTowerIdle->Triggered()) {
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
  }
}

/// Response to collision. If colliding with an object, play a sound.
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to `nullptr`,
/// which means collision with a wall).

void CTurret::CollisionResponse(const Vector2& norm, float d, CObject* pObj){
  if (m_bDead)return; //already dead, bail out 

  if (pObj && (pObj->GetType() == (UINT)eObjectType::BULLET || pObj->GetType() == (UINT)eObjectType::SWORD)) { //collision with bullet
    if (--m_nHealth == 0) { //health decrements to zero means death 
      m_pAudio->play(eSound::Thump); //explosion
      m_bDead = true; //flag for deletion from object list
      DeathFX(); //particle effects
    } //if

    else { //not a death blow
      m_pAudio->play(eSound::Thump); //impact sound
      const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
      m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the sprite to indicate damage
    } //else
  } //if
} //CollisionResponse

/// Perform a particle effect to mark the death of the turret.

void CTurret::DeathFX() {
  m_pObjectManager->create(eSprite::Explosion2, m_vPos);
} //DeathFX