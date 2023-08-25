/// \file Rabite.cpp
/// \brief Code for the Rabite object class CRabite.

#include "Rabite.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "SimpleMath.h"
#include "Helpers.h"
#include "ObjectManager.h"
#include "TileManager.h"
#include "Player.h"

/// Create and initialize a Rabite object given its initial position.
/// \param pos Initial position of Rabite.
/// \param maxHp starting hp of Rabite

CRabite::CRabite(eSprite t, const Vector2& pos) : CObject(t, pos) {
  objType = (UINT)eObjectType::RABITE; //set object type
  
  m_bIsTarget = true; //collides
  m_bStatic = false;  //not static

  m_fRoll = 0.0f; //sprite rotation
  m_vVelocity = Vector2(0.0f, 0.0f); //still until first hop
  m_fXScale = m_fYScale = 2.0f; //scale
  m_fRadius = 1.8f;

  m_pIdleEvent = new LEventTimer(0.3f); //period of frame events for slime
  m_pWanderEvent = new LEventTimer(2.0f, 1.0f);

  state = eRabiteState::IDLE; //initialize animation state to IDLE

} //constructor

/// Destructor.
CRabite::~CRabite() {
  if (m_pIdleEvent) {
    delete m_pIdleEvent;
    m_pIdleEvent = nullptr;
  }
  if (m_pHopEvent) {
    delete m_pHopEvent;
    m_pHopEvent = nullptr;
  }
  if (m_pDeathEvent) {
    delete m_pDeathEvent;
    m_pDeathEvent = nullptr;
  }
  if (m_pBiteEvent) {
    delete m_pBiteEvent;
    m_pBiteEvent = nullptr;
  }
  if (m_pDamageEvent) {
    delete m_pDamageEvent;
    m_pDamageEvent = nullptr;
  }
  if (m_pWanderEvent) {
    delete m_pWanderEvent;
    m_pWanderEvent = nullptr;
  }
} //destructor

/// Cycle through the animation/action pipeline based on the circumstances of the OakSeed enemy
/// First checks for hurt state, then faces player if applicable (and wanders), otherwise is idle
/// Updateframe afterwards
void CRabite::move() {
  if (m_bDead)
    return;
  else {
    if (state != eRabiteState::DEATH) {
      if (state == eRabiteState::HURT) {
        Damage();
      }
      else if (m_bBiting == false) {
        FacePlayer();
      }
      else if (state == eRabiteState::BITE && m_bBiting == true) {
        BiteDash();
      }
      else {
        Idle();
      }
      UpdateFramenumber();
    }
    else {
      UpdateFramenumber();
    }
  }
} //move

/// Idle function for Rabite. Sets the idleevent timer, deletes othere timers (for proper animation timing)
/// then states animation state to idle. Animation direction is based on the direction the sprite is facing
/// since it will try to face towards the player (a range is set for each)

void CRabite::Idle() {
  if (!m_pIdleEvent) {
    if (m_pHopEvent) {
      delete m_pHopEvent;
      m_pHopEvent = nullptr;
    }
    if (m_pBiteEvent) {
      delete m_pBiteEvent;
      m_pBiteEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pIdleEvent = new LEventTimer(0.3f);
  }
  state = eRabiteState::IDLE;
  //m_bBiting = false;

  if (m_fFacingVector > -(3 * XM_PIDIV4) && m_fFacingVector < -(XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteIdleDown) {
      m_nSpriteIndex = (UINT)eSprite::RabiteIdleDown;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > -(XM_PIDIV4) && m_fFacingVector < (XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteIdleRight) {
      m_nSpriteIndex = (UINT)eSprite::RabiteIdleRight;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (3* XM_PIDIV4) || m_fFacingVector < -(3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteIdleLeft) {
      m_nSpriteIndex = (UINT)eSprite::RabiteIdleLeft;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (XM_PIDIV4) && m_fFacingVector < (3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteIdleUp) {
      m_nSpriteIndex = (UINT)eSprite::RabiteIdleUp;
      m_nCurrentFrame = 0;
    }
  }
} //Idle

/// Wandering function for Rabite. Sets a timer and gives a new rotation vector with a direction
/// determined by the strayparity. Sets strayparity for next wandering event.

void CRabite::Wander() {
  const float t = m_pTimer->GetTime(); //current time

  if (m_pWanderEvent && m_pWanderEvent->Triggered()) { //enough time has passed
    UINT multiplier = m_pRandom->randn(1, 16);
    const float delta = (m_bStrayParity ? -1.3f : 1.3f) * 1.3f; //angle delta

    m_vVelocity = RotateVector(m_vVelocity, delta); //change direction by delta
    m_fFacingVector += delta; //rotate to face that direction

    m_bStrayParity = m_pRandom->randf() < 0.5f; //next stray is randomly left or right
  }
}

/// Damage Trigger for Rabite to delete other event timers, sent a damage event timer and set animation state to hurt

void CRabite::DamageTrigger() {
  if (!m_pDamageEvent) {
    if (m_pHopEvent) {
      delete m_pHopEvent;
      m_pHopEvent = nullptr;
    }
    if (m_pBiteEvent) {
      delete m_pBiteEvent;
      m_pBiteEvent = nullptr;
    }
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    m_pDamageEvent = new LEventTimer(0.2f);
  }
  state = eRabiteState::HURT;
}

/// Damage function to get a timer, set sprite index based on current animation sprite, then shift/knockback
/// based on the normal vector between the object collided with/hurt by and Oakseed

void CRabite::Damage() {
  const float t = m_pTimer->GetFrameTime();

  switch (m_nSpriteIndex) {
  case (UINT)eSprite::RabiteHopDown:
  case (UINT)eSprite::RabiteBiteDown:
  case (UINT)eSprite::RabiteHurtDown:
  case (UINT)eSprite::RabiteIdleDown: {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHurtDown) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHurtDown;
      m_nCurrentFrame = 0;
    }
  }break;
  case (UINT)eSprite::RabiteHopRight:
  case (UINT)eSprite::RabiteBiteRight:
  case (UINT)eSprite::RabiteHurtRight:
  case (UINT)eSprite::RabiteIdleRight: {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHurtRight) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHurtRight;
      m_nCurrentFrame = 0;
    }
  }break;
  case (UINT)eSprite::RabiteHopLeft:
  case (UINT)eSprite::RabiteBiteLeft:
  case (UINT)eSprite::RabiteHurtLeft:
  case (UINT)eSprite::RabiteIdleLeft: {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHurtLeft) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHurtLeft;
      m_nCurrentFrame = 0;
    }
  }break;
  case (UINT)eSprite::RabiteHopUp:
  case (UINT)eSprite::RabiteBiteUp:
  case (UINT)eSprite::RabiteHurtUp:
  case (UINT)eSprite::RabiteIdleUp: {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHurtUp) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHurtUp;
      m_nCurrentFrame = 0;
    }
  }break;
  }
  m_vPos += m_vVelocity * t;
}

/// Death function to delete other event timers and set a death event timer. Also sets animation state to death and sprite animation to death

void CRabite::Death() {
  if (!m_pDeathEvent) {
    if (m_pHopEvent) {
      delete m_pHopEvent;
      m_pHopEvent = nullptr;
    }
    if (m_pBiteEvent) {
      delete m_pBiteEvent;
      m_pBiteEvent = nullptr;
    }
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pDeathEvent = new LEventTimer(0.12f);
  }

  if (state == eRabiteState::DEATH) {
    return;
  }
  else {
    state = eRabiteState::DEATH;
    if (m_nSpriteIndex != (UINT)eSprite::RabiteDeath){
      m_nSpriteIndex = (UINT)eSprite::RabiteDeath;
      m_nCurrentFrame = 0;
    }
  }
}

/// Function to create blood splatter on the ground. creates multiple sprites
/// colors them read and then gives them a random position. Eventually fade out

void CRabite::DeathFx() {
  const int count = m_pRandom->randn(0, 10); //gen random num blood parts
  UINT randColor, randSize; //random vars
  LParticleDesc2D blood;
  for (int i = 0; i < count; i++) {
    blood.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;

    //fade properties
    blood.m_fFadeOutFrac = 0.8f;
    blood.m_fFadeInFrac = 0.2f;
    blood.m_fLifeSpan = 4.0f;

    //get random position within the slime sprite
    blood.m_vPos = GenRandomPosition(10.0f);

    //offset position to match sprite, account for scale
    //blood.m_vPos.x -= m_fRadius * 0.75f;
    //blood.m_vPos.y -= m_fRadius;

    randColor = m_pRandom->randn(0, 2);    //random color 
    randSize = m_pRandom->randn(0, 3);     //random size

    //select random color
    if (randColor == 0)
      blood.m_f4Tint = XMFLOAT4(Colors::Black);
    else if (randColor == 1)
      blood.m_f4Tint = XMFLOAT4(Colors::DarkRed);
    else
      blood.m_f4Tint = XMFLOAT4(Colors::Red);

    blood.m_f4Tint.w = 0.6f; //const alpha for all particles

    //select random size
    if (!randSize)
      blood.m_fMaxScale = 0.2f;
    else if (randSize == 1)
      blood.m_fMaxScale = 0.25f;
    else
      blood.m_fMaxScale = 0.3f;

    //spawn particle
    m_pParticleEngine->create(blood);
  }
}

/// Function that determines whether or not the player is detected, based on the position of the player
/// the position of the enemy, and whether player is within an arbitrary circle around the enemy. The circle
/// gets wider once player is detected. if Player is detected and also within LOS, enemy will then call 
/// rotateTowards() function to rotate towards the player. Otherwise, wanders randomly

void CRabite::FacePlayer() {
  if (m_pPlayer) {
    const float r = ((CRabite*)m_pPlayer)->m_fRadius; //player radius
    float dMin = 0.0f;
    if (m_bPlayerDetected) {
      dMin = 512.0f; //minimum distance at which target is invisible
    }
    else {
      dMin = 256.0f; //minimum distance at which target is invisible
    }
    
    const float dMinSq = dMin * dMin; //that squared

    Vector2 pTarget = m_pPlayer->m_vPos;
    Vector2 distance = pTarget - m_vPos;
    float mdistance = sqrt((distance.x * distance.x) + (distance.y * distance.y));
    float dsq = mdistance * mdistance; //distance squared to closest target
    //printf("min distance = %f  player distance = %f  ", dMinSq, dsq);

    if (m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r) && dsq < dMinSq) { //player visible
      RotateTowards(m_pPlayer->m_vPos, dsq); //rotate towards target
      m_bPlayerDetected = true;
    }
    else {
      m_fRotSpeed = 0.0f;
      m_bPlayerDetected = false;
      Wander();
      MoveTowardsPlayer();
    }
  }
  m_fFacingVector += 0.2f*m_fRotSpeed*XM_2PI*m_pTimer->GetFrameTime(); //rotate
  NormalizeAngle(m_fFacingVector); //normalize to [-pi, pi] for accuracy
}

/// Function to rotate facing vector to the player based on the difference in vectors
/// Rotates CC or C depending on which angle is least. Also sets shielded status if the 
/// position of the player, based on an angle from the enemy, is less than a certain amount
/// Moves towards the player if within a reasonable angle facing the player. Finds an inner circle
/// as well and dashes if rabite is within that distance of player
/// \param pos position of the player
/// \param dsq distance between the two initially

void CRabite::RotateTowards(const Vector2& pos, float dsq) {
  const Vector2 v = pos - m_vPos; //vector from target to turret
  const float theta = atan2f(v.y, v.x); //orientation of that vector
  float diff = m_fFacingVector - theta; //difference with turret orientation
  NormalizeAngle(diff); //normalize to [-pi, pi]

  float dMin = 128.0f;
  const float dMinSq = dMin * dMin; //that squared

  //set rotation speed from diff

  const float fAngleDelta = 0.05f; //allowable angle discrepancy
  const float fTrackingSpeed = 2.0f; //rotation speed when tracking

  if (diff > fAngleDelta)
    m_fRotSpeed = -fTrackingSpeed; //clockwise
  else if (diff < -fAngleDelta)
    m_fRotSpeed = fTrackingSpeed; //counterclockwise
  else 
    m_fRotSpeed = 0; //stop rotating

  //move towards player if pointing approximately towards target
  if (fabsf(diff) < fAngleDelta && m_bBiting == false) {
    if (dsq < dMinSq) {
      BiteDashTrigger();
    }
    else {
      MoveTowardsPlayer();
    }
  }
} //RotateTowards

/// function to head towards player. Deletes any eventtimers that aren't walk and sets it if not already set
/// then gives animation based on facing vector and walks at a set speed. Also used for wandering, so
/// enemy walks twice as fast when playe detected.

void CRabite::MoveTowardsPlayer() {
  if (!m_pHopEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pBiteEvent) {
      delete m_pBiteEvent;
      m_pBiteEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pHopEvent = new LEventTimer(0.2f);
  }

  state = eRabiteState::HOP;

  if (m_fFacingVector > -(3 * XM_PIDIV4) && m_fFacingVector < -(XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHopDown) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHopDown;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > -(XM_PIDIV4) && m_fFacingVector < (XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHopRight) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHopRight;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (3 * XM_PIDIV4) || m_fFacingVector < -(3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHopLeft) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHopLeft;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (XM_PIDIV4) && m_fFacingVector < (3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteHopUp) {
      m_nSpriteIndex = (UINT)eSprite::RabiteHopUp;
      m_nCurrentFrame = 0;
    }
  }

  float delta = 0.0f;
  const float t = m_pTimer->GetFrameTime();
  if (m_bPlayerDetected) {
    delta = 100.0f * t;
  }
  else {
    delta = 50.0f * t;
  }
  
  m_vVelocity = GetFacingVector();
  m_vPos += m_vVelocity * delta;

} //MoveTowardsPlayer

/// Function that sets a bite event (deletes all other timers), sets rabite animation state to Bite, and biting to true

void CRabite::BiteDashTrigger() {
  if (!m_pBiteEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pHopEvent) {
      delete m_pHopEvent;
      m_pHopEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pBiteEvent = new LEventTimer(0.04f);
  }

  state = eRabiteState::BITE;
  m_bBiting = true;
}

/// Function for a bitedash attack. Sets animation direction based on facing direction, then moves at dash pace
/// utilizing an mptimer and arbitrary constant. Calls BiteDashFX() for after image effect

void CRabite::BiteDash() {
  if (m_fFacingVector > -(3 * XM_PIDIV4) && m_fFacingVector < -(XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteBiteDown) {
      m_nSpriteIndex = (UINT)eSprite::RabiteBiteDown;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > -(XM_PIDIV4) && m_fFacingVector < (XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteBiteRight) {
      m_nSpriteIndex = (UINT)eSprite::RabiteBiteRight;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (3 * XM_PIDIV4) || m_fFacingVector < -(3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteBiteLeft) {
      m_nSpriteIndex = (UINT)eSprite::RabiteBiteLeft;
      m_nCurrentFrame = 0;
    }
  }
  else if (m_fFacingVector > (XM_PIDIV4) && m_fFacingVector < (3 * XM_PIDIV4)) {
    if (m_nSpriteIndex != (UINT)eSprite::RabiteBiteUp) {
      m_nSpriteIndex = (UINT)eSprite::RabiteBiteUp;
      m_nCurrentFrame = 0;
    }
  }

  const float t = m_pTimer->GetFrameTime();
  const float delta = 800.0f * t;
  m_vVelocity = GetFacingVector();
  BiteDashFX();
  m_vPos += m_vVelocity * delta;
} //BiteDash

/// Function to create after image effect by spawning particles as enemy dashes

void CRabite::BiteDashFX() {
  LParticleDesc2D d; //particle descriptor

  if (m_fFacingVector > -(3 * XM_PIDIV4) && m_fFacingVector < -(XM_PIDIV4)) {
    d.m_nSpriteIndex = (UINT)eSprite::RabiteBiteDown;
  }
  else if (m_fFacingVector > -(XM_PIDIV4) && m_fFacingVector < (XM_PIDIV4)) {
    d.m_nSpriteIndex = (UINT)eSprite::RabiteBiteRight;
  }
  else if (m_fFacingVector > (3 * XM_PIDIV4) || m_fFacingVector < -(3 * XM_PIDIV4)) {
    d.m_nSpriteIndex = (UINT)eSprite::RabiteBiteLeft;
  }
  else if (m_fFacingVector > (XM_PIDIV4) && m_fFacingVector < (3 * XM_PIDIV4)) {
    d.m_nSpriteIndex = (UINT)eSprite::RabiteBiteUp;
  }

  d.m_fMaxScale = 2.0f;
  d.m_fLifeSpan = 0.1f;
  d.m_fFadeOutFrac = 0.3f;
  d.m_f4Tint = XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f);
  d.m_fAlpha = 0.1f;
  d.m_vPos = m_vPos; //center particle at player center
  if (d.m_nSpriteIndex) {
    m_pParticleEngine->create(d);
  }
}

/// Update the frame number in the animation sequence. Various circumstances for different frame event timers
/// Death gives chance to drop health or mana potion

void CRabite::UpdateFramenumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (state != eRabiteState::DEATH) {
    if (n > 1 && m_pIdleEvent && m_pIdleEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bBiting = false;
      }
    }
    else if (n > 1 && m_pDamageEvent && m_pDamageEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bIsTarget = true;
        Idle();
      }
    }
    else if (n > 1 && m_pBiteEvent && m_pBiteEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bBiting = false;
        Idle();
      }
    }
    else if (n > 1 && m_pHopEvent && m_pHopEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
      }
    }
  }
  else if (n > 1 && m_pDeathEvent && m_pDeathEvent->Triggered() && state == eRabiteState::DEATH) {
    DeathFx();
    m_bDeadCheck = true;
    m_bStatic = true;
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    if (m_nCurrentFrame == n - 1) {
      const UINT chance = m_pRandom->randn(1, 3);
      if (chance == 1)
        m_pObjectManager->create(eSprite::HealthPotion, m_vPos);
      else if (chance == 2)
        m_pObjectManager->create(eSprite::ManaPotion, m_vPos);
      m_bDead = true;
    }
  }
}

/// Response to collision, which for a Rabite means nearly always bounding backwards. This could also result in damage, depending on what is colliding
/// velocity set for knockback based on the difference in positions
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CRabite::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (m_bDead || state == eRabiteState::DEATH) 
    return; //already dead, bail out 
  else if (pObj) {
    Vector2 kb;
    kb = (m_vPos - pObj->m_vPos); // Difference of positions to determine knockback vector or normal
    if (pObj && (pObj->isBullet() || pObj->GetType() == (UINT)eObjectType::SWORD) && m_bIsTarget) { //collision with bullet
      m_bIsTarget = false; //intangibility while in damaged animation
      if (--m_nHealth == 0) { //health decrements to zero means death 
        m_pAudio->play(eSound::Thump); //hurt sound
        Death();
      } //if
      else { //not a death blow
        m_pAudio->play(eSound::Thump); //hurt sound
        DamageTrigger();
        m_vVelocity = kb * 10.0f;
      } //else
      const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
      m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the sprite to indicate damage
    } //if
    else if (pObj && pObj->GetType() == (UINT)eObjectType::PLAYER) {
      m_vVelocity = Vector2(0.0f, 0.0f);
      DamageTrigger();
      m_vVelocity = kb * 10.0f;
    }
  }
  else {
    CObject::CollisionResponse(norm, d, pObj); //wall collision
  }  
}