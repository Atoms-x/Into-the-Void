/// \file Player.cpp
/// \brief Code for the player object class CPlayer.

#include "Player.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "SpriteRenderer.h"
#include "Obstacle.h"

 ///Create and initialize an player object given its initial position.
 ///\param p Initial position of player.
CPlayer::CPlayer(const Vector2& p) : CObject(eSprite::PlayerIdleDown, p) {
  objType = (UINT)eObjectType::PLAYER;

  m_bIsTarget = true; //Can be hit
  m_bStatic = false;

  m_fRoll = 0.0f;
  m_fXScale = m_fYScale = 2.0f; //Scale player sprite to look right

  m_pIdleEvent = new LEventTimer(0.2f);

  m_nStatusEff = -1;

  state = ePlayerAnimationState::IDLE;
} //Constructor

void CPlayer::SetPort(int port) {
    printf("Port updated. New port: %d\n", port);
    m_nPortAccept = port;
}

int CPlayer::GetPort() {
    return m_nPortAccept;
}

/// Destructor
CPlayer::~CPlayer() {
  if (m_pIdleEvent) {
    delete m_pIdleEvent;
    m_pIdleEvent = nullptr;
  }
  if (m_pDeathEvent) {
    delete m_pDeathEvent;
    m_pDeathEvent = nullptr;
  }
  if (m_pRunEvent) {
    delete m_pRunEvent;
    m_pRunEvent = nullptr;
  }
  if (m_pDashEvent) {
    delete m_pDashEvent;
    m_pDashEvent = nullptr;
  }
  if (m_pDamageEvent) {
    delete m_pDamageEvent;
    m_pDamageEvent = nullptr;
  }
} //Destructor

/// Move Player to device input. The amount of motion is proportional to the frame time
/// Additionally, is logic for whole animation pipeline and time dependent functions
/// Sets status effects, timer for stamina recovery, and shaketimer for camera shake
/// Logic (with other logic in other areas), helps set player control tightness
/// such as forced stoping when attacking
void CPlayer::move() {
  if (m_bDead)
    return;
  else {
    if (state != ePlayerAnimationState::DEATH) {
      StatusEffect();
      Timer();
      ShakeTimer();
      if (state == ePlayerAnimationState::HURT) {
        knockback();
      }
      else if (state == ePlayerAnimationState::ATTACK) {
        Attack();
      }
      else if (state == ePlayerAnimationState::DASH) {
        Dash();
      }
      else if ((m_bStrafeLeft || m_bStrafeRight || m_bStrafeDown || m_bStrafeUp) && !(state == ePlayerAnimationState::ATTACK || state == ePlayerAnimationState::DASH || state == ePlayerAnimationState::HURT)) {
        Run();
      }
      else {
        Idle();
      }
      UpdateFramenumber();
    }
    else {
      Death();
      UpdateFramenumber();
    }
  }
}

/// Timer function for stamina recovery. Sets an mptimer, then gives
/// an arbitrary constant. Works out to about 1 second = 100 units
/// Also resets stamina over time (3.5 second intervals)

void CPlayer::Timer() {
  if (m_nDashesLeft < 3) {
    const float t = m_pTimer->GetFrameTime();
    const float delta = 175.0f * t;
    m_nDashTimer += delta;
    if (m_nDashTimer > 350 && m_nDashesLeft < 3) {
      m_nDashesLeft++;
      m_nDashTimer = 0;
    }
  }
}

/// Timer function for camera shake when hurt. Also shifts/shakes camera. randomly shifts the position, then back to player position, then away again
/// does this for an arbitrarily set amount of time, then sets takingdamage to false

void CPlayer::ShakeTimer() {
  if (m_bIsTakingDamage) {
    const float t = m_pTimer->GetFrameTime();
    const float delta = 175.0f * t;
    m_nShakeTimer += delta;
    
    if (m_nShakeTimer != 0 && m_nShakeTimer % 10 == 0) {
      UINT dx = m_pRandom->randn(1, 10);
      UINT dy = m_pRandom->randn(1, 10);
      UINT choice = m_pRandom->randn(0, 4);
      switch (choice) {
      case 1: {
        m_pRenderer->SetCameraPos(Vector3(m_pRenderer->GetCameraPos().x + dx, m_pRenderer->GetCameraPos().y + dy, 0.0f));
      } break;
      case 2: {
        m_pRenderer->SetCameraPos(Vector3(m_pRenderer->GetCameraPos().x + dx, m_pRenderer->GetCameraPos().y - dy, 0.0f));
      } break;
      case 3: {
        m_pRenderer->SetCameraPos(Vector3(m_pRenderer->GetCameraPos().x - dx, m_pRenderer->GetCameraPos().y + dy, 0.0f));
      } break;
      case 4: {
        m_pRenderer->SetCameraPos(Vector3(m_pRenderer->GetCameraPos().x - dx, m_pRenderer->GetCameraPos().y - dy, 0.0f));
      } break;
      }
    }
    else if (m_nShakeTimer % 5 == 0) {
      m_pRenderer->SetCameraPos(Vector3(m_pPlayer->m_vPos.x, m_pPlayer->m_vPos.y, 0.0f));
    }
    else {
      m_pRenderer->SetCameraPos(Vector3(m_pPlayer->m_vPos.x, m_pPlayer->m_vPos.y, 0.0f));
    }
    if (m_nShakeTimer >= 21) {
      m_nShakeTimer = 0;
      m_bIsTakingDamage = false;
    }
  }
}

/// Function to delete all other event timers, set idle event timer, change animation state to idle, then change animation depending on last animation direction

void CPlayer::Idle() {

  if (!m_pIdleEvent) {
    if (m_pRunEvent) {
      delete m_pRunEvent;
      m_pRunEvent = nullptr;
    }
    if (m_pSwordAttackEvent) {
      delete m_pSwordAttackEvent;
      m_pSwordAttackEvent = nullptr;
    }
    if (m_pDashEvent) {
      delete m_pDashEvent;
      m_pDashEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pIdleEvent = new LEventTimer(0.2f);
  }

  if (state == ePlayerAnimationState::IDLE) {
    return;
  }
  else {
    state = ePlayerAnimationState::IDLE;

    switch (m_nSpriteIndex) {
    case (UINT)eSprite::PlayerRunDown:
    case (UINT)eSprite::PlayerAttackDown:
    case (UINT)eSprite::PlayerDashDown:
    case (UINT)eSprite::PlayerHitDown: {
      if (m_nSpriteIndex != (UINT)eSprite::PlayerIdleDown) {
        m_nSpriteIndex = (UINT)eSprite::PlayerIdleDown;
        m_nCurrentFrame = 0;
      }
    }break;
    case (UINT)eSprite::PlayerRunRight:
    case (UINT)eSprite::PlayerAttackRight:
    case (UINT)eSprite::PlayerDashRight:
    case (UINT)eSprite::PlayerHitRight: {
      if (m_nSpriteIndex != (UINT)eSprite::PlayerIdleRight) {
        m_nSpriteIndex = (UINT)eSprite::PlayerIdleRight;
        m_nCurrentFrame = 0;
      }
    }break;
    case (UINT)eSprite::PlayerRunLeft:
    case (UINT)eSprite::PlayerAttackLeft:
    case (UINT)eSprite::PlayerDashLeft:
    case (UINT)eSprite::PlayerHitLeft: {
      if (m_nSpriteIndex != (UINT)eSprite::PlayerIdleLeft) {
        m_nSpriteIndex = (UINT)eSprite::PlayerIdleLeft;
        m_nCurrentFrame = 0;
      }
    }break;
    case (UINT)eSprite::PlayerRunUp:
    case (UINT)eSprite::PlayerAttackUp:
    case (UINT)eSprite::PlayerDashUp:
    case (UINT)eSprite::PlayerHitUp: {
      if (m_nSpriteIndex != (UINT)eSprite::PlayerIdleUp) {
        m_nSpriteIndex = (UINT)eSprite::PlayerIdleUp;
        m_nCurrentFrame = 0;
      }
    }break;
    }
  }
} //Idle


/// sets animation to strafeleft and triggers its boolean

void CPlayer::StrafeLeft() {
  m_bStrafeLeft = true;

  if (state != ePlayerAnimationState::ATTACK && state != ePlayerAnimationState::DASH && state != ePlayerAnimationState::HURT && state != ePlayerAnimationState::DEATH) {
    state = ePlayerAnimationState::RUNLEFT;
  }
  else
    return;
  if (m_nSpriteIndex != (UINT)eSprite::PlayerRunLeft) {
    m_nSpriteIndex = (UINT)eSprite::PlayerRunLeft;
    m_nCurrentFrame = 0;
  }
  else
    m_nSpriteIndex;
} //StrafeLeft

/// sets animation to straferight and triggers its boolean

void CPlayer::StrafeRight() {
  m_bStrafeRight = true;

  if (state != ePlayerAnimationState::ATTACK && state != ePlayerAnimationState::DASH && state != ePlayerAnimationState::HURT && state != ePlayerAnimationState::DEATH) {
    state = ePlayerAnimationState::RUNRIGHT;
  }
  else
    return;
  if (m_nSpriteIndex != (UINT)eSprite::PlayerRunRight) {
    m_nSpriteIndex = (UINT)eSprite::PlayerRunRight;
    m_nCurrentFrame = 0;
  }
  else
    m_nSpriteIndex;
} //StrafeRight

/// sets animation to strafedown and triggers its boolean

void CPlayer::StrafeDown() {
  m_bStrafeDown = true;

  if (state == ePlayerAnimationState::RUNLEFT && m_pKeyboard->Down('A') || state == ePlayerAnimationState::RUNRIGHT && m_pKeyboard->Down('D')) {
    return;
  }
  else {
    if (state != ePlayerAnimationState::ATTACK && state != ePlayerAnimationState::DASH && state != ePlayerAnimationState::HURT && state != ePlayerAnimationState::DEATH) {
      state = ePlayerAnimationState::RUNDOWN;
    }
    else
      return;
    if (m_nSpriteIndex != (UINT)eSprite::PlayerRunDown) {
      m_nSpriteIndex = (UINT)eSprite::PlayerRunDown;
      m_nCurrentFrame = 0;
    }
    else
      m_nSpriteIndex;
  }
} //StrafeBack

/// sets animation to strafeup and triggers its boolean

void CPlayer::StrafeUp() {
  m_bStrafeUp = true;

  if (state == ePlayerAnimationState::RUNLEFT && m_pKeyboard->Down('A') || state == ePlayerAnimationState::RUNRIGHT && m_pKeyboard->Down('D')) {
    return;
  }
  else {
    if (state != ePlayerAnimationState::ATTACK && state != ePlayerAnimationState::DASH && state != ePlayerAnimationState::HURT && state != ePlayerAnimationState::DEATH) {
      state = ePlayerAnimationState::RUNUP;
    }
    else
      return;
    if (m_nSpriteIndex != (UINT)eSprite::PlayerRunUp) {
      m_nSpriteIndex = (UINT)eSprite::PlayerRunUp;
      m_nCurrentFrame = 0;
    }
    else
      m_nSpriteIndex;
  }
} //StrafeUp

/// Function to delete all other event timers, set run event timer, then calculate movement direction based on vectors. Also calls runFX to make dust

void CPlayer::Run() {
  if (!m_pRunEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pSwordAttackEvent) {
      delete m_pSwordAttackEvent;
      m_pSwordAttackEvent = nullptr;
    }
    if (m_pDashEvent) {
      delete m_pDashEvent;
      m_pDashEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pRunEvent = new LEventTimer(0.12f);
  }

  Vector2 input_dir = Vector2(m_bStrafeRight - m_bStrafeLeft, m_bStrafeUp - m_bStrafeDown);
  const float t = m_pTimer->GetFrameTime();
  const float delta = 175.0f * t;
  const float input_length = sqrt(input_dir.x * input_dir.x + input_dir.y * input_dir.y);

  if (m_bStrafeLeft || m_bStrafeRight || m_bStrafeDown || m_bStrafeUp)
    m_fFacingVector = atan2(input_dir.y, input_dir.x);

  if (input_length != 0.0f) {
    m_vVelocity = Vector2(input_dir.x / input_length, input_dir.y / input_length);
    m_vPos += delta * input_dir;
  }
  m_nDust = (m_nDust + 1) % 7;
  if (m_nDust == 0) {
    RunFx();
  }

  m_bStrafeLeft = m_bStrafeRight = m_bStrafeDown = m_bStrafeUp = false; //reset strafe flags
} //Run

/// Function called when space pressed. Sets animation state to attack, attack boolean to true, stops the player, deletes all other event timers
/// and sets the sword attack event timer

void CPlayer::AttackTrigger() {
  if (state != ePlayerAnimationState::DEATH) {
    state = ePlayerAnimationState::ATTACK;
    m_bIsAttacking = true;
    m_vVelocity = Vector2(0.0f, 0.0f); //Prevents bullet from gaining speed during dash
  }

  if (!m_pSwordAttackEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pRunEvent) {
      delete m_pRunEvent;
      m_pRunEvent = nullptr;
    }
    if (m_pDashEvent) {
      delete m_pDashEvent;
      m_pDashEvent = nullptr;
    }
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    m_pSwordAttackEvent = new LEventTimer(0.08f);
  }
} //AttackTrigger

/// Attack function that sets animation based on previous animation direction

void CPlayer::Attack() {
  switch (m_nSpriteIndex) {
  case (UINT)eSprite::PlayerRunDown:
  case (UINT)eSprite::PlayerIdleDown:
  case (UINT)eSprite::PlayerDashDown: {
    m_nSpriteIndex = (UINT)eSprite::PlayerAttackDown;
    m_nCurrentFrame = 0;
  }break;
  case (UINT)eSprite::PlayerRunRight:
  case (UINT)eSprite::PlayerIdleRight:
  case (UINT)eSprite::PlayerDashRight: {
    m_nSpriteIndex = (UINT)eSprite::PlayerAttackRight;
    m_nCurrentFrame = 0;
  }break;
  case (UINT)eSprite::PlayerRunLeft:
  case (UINT)eSprite::PlayerIdleLeft:
  case (UINT)eSprite::PlayerDashLeft: {
    m_nSpriteIndex = (UINT)eSprite::PlayerAttackLeft;
    m_nCurrentFrame = 0;
  }break;
  case (UINT)eSprite::PlayerRunUp:
  case (UINT)eSprite::PlayerIdleUp:
  case (UINT)eSprite::PlayerDashUp: {
    m_nSpriteIndex = (UINT)eSprite::PlayerAttackUp;
    m_nCurrentFrame = 0;
  }break;
  }
} //Attack

/// Dash trigger function called by shift key press. Sets animation state to dash, deletes all other event timers and sets dash event timer

void CPlayer::DashTrigger() {
  if (state != ePlayerAnimationState::ATTACK && state != ePlayerAnimationState::DEATH && !m_bDead) {
    state = ePlayerAnimationState::DASH;
    m_bIsDashing = true;
  }
  else
    return;

  if (!m_pDashEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pRunEvent) {
      delete m_pRunEvent;
      m_pRunEvent = nullptr;
    }
    if (m_pSwordAttackEvent) {
      delete m_pSwordAttackEvent;
      m_pSwordAttackEvent = nullptr;
    }
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    m_pDashEvent = new LEventTimer(0.04f);
  }
} //DashTrigger

/// Dash function. Sets mptimer, calls dashFX for after image, then moves based on an arbitrary constant and the timer

void CPlayer::Dash() {
  const float t = m_pTimer->GetFrameTime();
  DashFX();
  m_vVelocity = Vector2(cosf(m_fFacingVector), sinf(m_fFacingVector)) * 800.0f;
  m_vPos += m_vVelocity * t;
} //Dash

/// Function to create an after image effect by drawing sprites as the player moves, tinted cyan, and transparent

void CPlayer::DashFX() {

  LParticleDesc2D d; //particle descriptor

  switch (m_nSpriteIndex) {
  case (UINT)eSprite::PlayerIdleDown:
  case (UINT)eSprite::PlayerAttackDown:
  case (UINT)eSprite::PlayerRunDown:
  case (UINT)eSprite::PlayerDashDown: {
    if (m_nSpriteIndex != (UINT)eSprite::PlayerDashDown) {
      m_nSpriteIndex = (UINT)eSprite::PlayerDashDown;
      m_nCurrentFrame = 0;
    }
    d.m_nSpriteIndex = (UINT)eSprite::PlayerStaticDashDown;
  } break;
  case (UINT)eSprite::PlayerIdleUp:
  case (UINT)eSprite::PlayerAttackUp:
  case (UINT)eSprite::PlayerRunUp:
  case (UINT)eSprite::PlayerDashUp: {
    if (m_nSpriteIndex != (UINT)eSprite::PlayerDashUp) {
      m_nSpriteIndex = (UINT)eSprite::PlayerDashUp;
      m_nCurrentFrame = 0;
    }
    d.m_nSpriteIndex = (UINT)eSprite::PlayerStaticDashUp;
  } break;
  case (UINT)eSprite::PlayerIdleRight:
  case (UINT)eSprite::PlayerAttackRight:
  case (UINT)eSprite::PlayerRunRight:
  case (UINT)eSprite::PlayerDashRight: {
    if (m_nSpriteIndex != (UINT)eSprite::PlayerDashRight) {
      m_nSpriteIndex = (UINT)eSprite::PlayerDashRight;
      m_nCurrentFrame = 0;
    }
    d.m_nSpriteIndex = (UINT)eSprite::PlayerStaticDashRight;
  }break;
  case (UINT)eSprite::PlayerIdleLeft:
  case (UINT)eSprite::PlayerAttackLeft:
  case (UINT)eSprite::PlayerRunLeft:
  case (UINT)eSprite::PlayerDashLeft: {
    if (m_nSpriteIndex != (UINT)eSprite::PlayerDashLeft) {
      m_nSpriteIndex = (UINT)eSprite::PlayerDashLeft;
      m_nCurrentFrame = 0;
    }
    d.m_nSpriteIndex = (UINT)eSprite::PlayerStaticDashLeft;
  }break;
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

} //DashFX

/// Function called when taking damage to set the player animation state to hurt and boolean taking damage to true
/// also deletes all other event timers and sets a damage event timer

void CPlayer::DamageTrigger() {
  if (state != ePlayerAnimationState::DEATH && !m_bDead) {
    if (state != ePlayerAnimationState::HURT) {
      state = ePlayerAnimationState::HURT;
      m_bIsTakingDamage = true;
    }
  }
  else
    return;

  if (!m_pDamageEvent) {
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pRunEvent) {
      delete m_pRunEvent;
      m_pRunEvent = nullptr;
    }
    if (m_pSwordAttackEvent) {
      delete m_pSwordAttackEvent;
      m_pSwordAttackEvent = nullptr;
    }
    if (m_pDashEvent) {
      delete m_pDashEvent;
      m_pDashEvent = nullptr;
    }
    m_pDamageEvent = new LEventTimer(0.06f);
  }
} //DamageTrigger

/// Function to set hit animation based on previous animation direction and to also move position based on an mptimer and velocity set in collision

void CPlayer::knockback() {
  const float t = m_pTimer->GetFrameTime();

  switch (m_nSpriteIndex) {
  case (UINT)eSprite::PlayerAttackDown:
  case (UINT)eSprite::PlayerIdleDown:
  case (UINT)eSprite::PlayerDashDown:
  case (UINT)eSprite::PlayerRunDown: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerHitDown) ? (UINT)eSprite::PlayerHitDown : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackRight:
  case (UINT)eSprite::PlayerIdleRight:
  case (UINT)eSprite::PlayerDashRight:
  case (UINT)eSprite::PlayerRunRight: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerHitRight) ? (UINT)eSprite::PlayerHitRight : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackLeft:
  case (UINT)eSprite::PlayerIdleLeft:
  case (UINT)eSprite::PlayerDashLeft:
  case (UINT)eSprite::PlayerRunLeft: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerHitLeft) ? (UINT)eSprite::PlayerHitLeft : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackUp:
  case (UINT)eSprite::PlayerIdleUp:
  case (UINT)eSprite::PlayerDashUp:
  case (UINT)eSprite::PlayerRunUp: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerHitUp) ? (UINT)eSprite::PlayerHitUp : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  }

  m_bIsAttacking = true;
  m_bIsDashing = true;
  m_vPos += m_vVelocity * t;
} //knockback

/// Death trigger function to set animation state to death, delete all other event timers, and set death event timer

void CPlayer::DeathTrigger() {
  if (state != ePlayerAnimationState::DEATH) {
    state = ePlayerAnimationState::DEATH;
  }
  else
    return;
  if (!m_pDeathEvent) {
    if (m_pDamageEvent) {
      delete m_pDamageEvent;
      m_pDamageEvent = nullptr;
    }
    if (m_pIdleEvent) {
      delete m_pIdleEvent;
      m_pIdleEvent = nullptr;
    }
    if (m_pRunEvent) {
      delete m_pRunEvent;
      m_pRunEvent = nullptr;
    }
    if (m_pSwordAttackEvent) {
      delete m_pSwordAttackEvent;
      m_pSwordAttackEvent = nullptr;
    }
    if (m_pDashEvent) {
      delete m_pDashEvent;
      m_pDashEvent = nullptr;
    }
    m_pDeathEvent = new LEventTimer(0.12f);
  }
} //death

/// Death function to set death animation based on direction of last animation

void CPlayer::Death() {
  switch (m_nSpriteIndex) {
  case (UINT)eSprite::PlayerAttackDown:
  case (UINT)eSprite::PlayerIdleDown:
  case (UINT)eSprite::PlayerHitDown:
  case (UINT)eSprite::PlayerDashDown:
  case (UINT)eSprite::PlayerRunDown: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerDeathDown) ? (UINT)eSprite::PlayerDeathDown : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackRight:
  case (UINT)eSprite::PlayerIdleRight:
  case (UINT)eSprite::PlayerHitRight:
  case (UINT)eSprite::PlayerDashRight:
  case (UINT)eSprite::PlayerRunRight: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerDeathRight) ? (UINT)eSprite::PlayerDeathRight : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackLeft:
  case (UINT)eSprite::PlayerIdleLeft:
  case (UINT)eSprite::PlayerHitLeft:
  case (UINT)eSprite::PlayerDashLeft:
  case (UINT)eSprite::PlayerRunLeft: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerDeathLeft) ? (UINT)eSprite::PlayerDeathLeft : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  case (UINT)eSprite::PlayerAttackUp:
  case (UINT)eSprite::PlayerIdleUp:
  case (UINT)eSprite::PlayerHitUp:
  case (UINT)eSprite::PlayerDashUp:
  case (UINT)eSprite::PlayerRunUp: {
    m_nSpriteIndex = (m_nSpriteIndex != (UINT)eSprite::PlayerDeathUp) ? (UINT)eSprite::PlayerDeathUp : m_nSpriteIndex;
    m_nCurrentFrame = 0;
  } break;
  }

  m_vVelocity = Vector2(0.0f, 0.0f);
  //m_bIsTarget = false;
}

/// Response to collision. If the object being collided with is an enemy or bullet, then
/// play a sound, otherwise call `CObject::CollisionResponse` for the default
/// collision response. If manapotion or healthpotion, gives respective boosts
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to `nullptr`,
/// which means collision with a wall).

void CPlayer::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
  if (m_bDead || m_pDeathEvent)
    return; //Already dead, bail out
  else if (pObj)
  {
    Vector2 kb;
    switch ((pObj->GetType())) {
    case (UINT)eObjectType::HEALTHPOT:
      if (m_nHealth == 6) {
        m_bHealthNeeded = false;
        break;
      }
      else if (m_nHealth <= 4) { //if not at max hp
        if (m_nStatusEff == -1) {
          m_nStatusEff = 0;
        }
        m_bHealthNeeded = true;
        m_nHealth += 2;
        const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
      } //if
      else {
        if (m_nStatusEff == -1) {
          m_nStatusEff = 0;
        }
        m_bHealthNeeded = true;
        m_nHealth = 6;
      } //else
      break;
    case (UINT)eObjectType::MANAPOT:
      if (m_nMana == 6) {
        m_bManaNeeded = false;
        break;
      }
      else if (m_nMana <= 4) {
        if (m_nStatusEff == -1) {
          m_nStatusEff = 128;
        }
        m_bManaNeeded = true;
        m_nMana += 2;
      }

      else {
        if (m_nStatusEff == -1) {
          m_nStatusEff = 128;
        }
        m_bManaNeeded = true;
        m_nMana = 6;
      }

      break;
    
    case (UINT)eObjectType::SLIME:
    case (UINT)eObjectType::BULLET:
    case (UINT)eObjectType::OAKSEED:
    case (UINT)eObjectType::RABITE: {
      kb = (m_vPos - pObj->m_vPos); // Difference of positions to determine knockback vector or normal
      if (m_bIsTarget == true && !pObj->GetIsDead()) {
        if (--m_nHealth == 0) { //health decrements to zero means death 
          m_bIsTarget = false;
          m_pAudio->play(eSound::Ow);
          DeathTrigger();
        } //if
        else { //not a death blow
          m_bIsTarget = false;
          DamageTrigger();
          DamageFx();
          m_vVelocity = kb * 10.0f;
          m_pAudio->play(eSound::Grunt); //impact sound
          const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
          m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the health indicator
        } //else
      }
    }
      break;
    default:
      CObject::CollisionResponse(norm, d, pObj); //default collision response
      break;
    }
  }
  else //pObj == nullptr for walls
    CObject::CollisionResponse(norm, d, pObj); //default collision response
} //CollisionResponse

/// Update the frame number in the animation sequence. Various circumstances for different frame event timers

void CPlayer::UpdateFramenumber() {
  const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
  if (state != ePlayerAnimationState::DEATH) {
    if (n > 1 && m_pRunEvent && m_pRunEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    }
    else if (n > 1 && m_pIdleEvent && m_pIdleEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    }
    else if (n > 1 && m_pSwordAttackEvent && m_pSwordAttackEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bIsAttacking = false;
        m_bIsDashing = false; //Prevents dash then attack midway keeping this flag up from the dash
        m_bIsTarget = true;
        Idle();
      }
    }
    else if (n > 1 && m_pDashEvent && m_pDashEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bIsDashing = false;
        m_bIsTarget = true;
        Idle();
      }
    }
    if (n > 1 && m_pDamageEvent && m_pDamageEvent->Triggered()) {
      m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
      if (m_nCurrentFrame == n - 1) {
        m_bIsDashing = false;
        m_bIsAttacking = false;
        m_bIsTarget = true;
        m_vVelocity = Vector2(0.0f, 0.0f);
        Idle();
      }
    }
  }
  else if (n > 1 && m_pDeathEvent && m_pDeathEvent->Triggered() && state == ePlayerAnimationState::DEATH) {
    m_nCurrentFrame = (m_nCurrentFrame + 1) % n;
    if (m_nCurrentFrame == n - 1) {
      m_bStatic = true;
      m_bDead = true;
      m_pPlayer = nullptr; //clear common player pointer
    }
  }
} //UpdateFramenumber

const Vector2& CPlayer::GetPos() const {
  return m_vPos;
} //GetPos

const UINT CPlayer::GetHealth() const {
  return m_nHealth;
} //GetHealth

const UINT CPlayer::GetMana() const {
  return m_nMana;
} //GetMana

void CPlayer::SetMana(UINT t) {
  m_nMana += t;
} //SetMana

void CPlayer::SetStamina(UINT t) {
  m_nDashesLeft += t;
} //SetStamina

void CPlayer::SetFacingVectorAtShot(float t) {
  m_nFacingVectorAtShot = t;
}

const UINT CPlayer::GetStamina() const {
  return m_nDashesLeft;
} //GetStamina

const float CPlayer::GetFacingVectorAtShot() const {
  return m_nFacingVectorAtShot;
} //GetFacingVectorAtShot



/// Applies status effect visuals to the player
const void CPlayer::StatusEffect() {
  if (m_nStatusEff == -1) //no status effect
    return;
  else if (m_nStatusEff < 128 && m_bHealthNeeded) {    //hp regen (0 - 128)
    if (m_nStatusEff % 32 == 0) {
      HealFx();   //create hp regen fx particles
    }
    if (m_nStatusEff % 6 == 0)  //flicker between colors
      m_f4Tint = XMFLOAT4(Colors::Gold);
    else
      m_f4Tint = XMFLOAT4(Colors::White);
    m_nStatusEff++;

    if (m_nStatusEff == 128) //if status now finished, set back to no status state
      m_nStatusEff = -1;
  }//elif
  else if (m_nStatusEff < 256 && m_bManaNeeded) {   //mana regen (128 - 256)
    if (m_nStatusEff % 16 == 0) {
      ManaRegenFx();  //spawn mana regen fx particles
    }
    if (m_nStatusEff % 6 == 0)   //flicker between colors
      m_f4Tint = XMFLOAT4(Colors::MediumPurple);
    else
      m_f4Tint = XMFLOAT4(Colors::White);
    m_nStatusEff++;

    if (m_nStatusEff == 256) //if status now finished, set back to no status state
      m_nStatusEff = -1;
  } //elif

  if (m_nStatusEff == -1) {  //status effect visual finished, change color to default value
    const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
    m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the health indicator
  } //if

} //StatusEffect

/// Applies heal effect visuals to the player
void CPlayer::HealFx() {
  const int count = m_pRandom->randn(2, 5); //gen 2-5 particles each call
  UINT randSize, randColor; //random vars
  LParticleDesc2D heal;   //particle descriptor
  for (int i = 0; i < count; i++) {
    heal.m_nSpriteIndex = (UINT)eSprite::Cross;

    //fade properties
    heal.m_fLifeSpan = 1.0f;
    heal.m_fFadeOutFrac = 0.4f;
    heal.m_fFadeInFrac = 0.1f;

    heal.m_f4Tint = XMFLOAT4(Colors::Green);

    //get random position within the player sprite
    heal.m_vPos = GenRandomPosition(0.6f);

    //randomize velocity direction and scale up
    heal.m_vVel = (m_pRandom->randv());
    heal.m_vVel.x *= 5.0f;
    heal.m_vVel.y *= 30.0f;

    //heal.m_vVel += this->m_vVelocity;   //add player velocity
    heal.m_vVel.y = abs(heal.m_vVel.y); //make y velocity positive

    randColor = m_pRandom->randn(0, 2); //random color
    randSize = m_pRandom->randn(4, 15);//random size multiplier

    //select random color
    if (randColor == 0)
      heal.m_f4Tint = XMFLOAT4(Colors::Green);
    else if (randColor == 1)
      heal.m_f4Tint = XMFLOAT4(Colors::GreenYellow);
    else
      heal.m_f4Tint = XMFLOAT4(Colors::ForestGreen);

    heal.m_f4Tint.w = 0.8f; //const alpha for all particles

    //apply random scale
    heal.m_fMaxScale = 0.1f * (float)randSize;

    //spawn particle
    if (heal.m_nSpriteIndex) {
      m_pParticleEngine->create(heal);
    }
  } //for

} //HealFx

/// Applies mana regen effect visuals to the player
void CPlayer::ManaRegenFx() {
  const int count = m_pRandom->randn(3, 8); //gen 3-8 particles each call
  UINT randSize, randColor; //random vars
  LParticleDesc2D mana;   //particle descriptor
  for (int i = 0; i < count; i++) {
    //select random sprite from ManaParticles
    mana.m_nSpriteIndex = (UINT)eSprite::ManaParticles;
    mana.m_nCurrentFrame = m_pRandom->randn(0, 7);

    //fade properties
    mana.m_fFadeOutFrac = 0.8f;
    mana.m_fFadeInFrac = 0.2f;
    mana.m_fLifeSpan = 1.2f;

    //get random spawn position on top of the player
    mana.m_vPos = this->GenRandomPosition(0.45f);

    //offset y, account for scale
    mana.m_vPos.y -= m_fRadius * 5.0f / 8.0f;

    //randomize velocity direction and scale up
    mana.m_vVel = (m_pRandom->randv());
    mana.m_vVel.x *= 20.0f;
    mana.m_vVel.y *= 100.0f;

    //mana.m_vVel += this->m_vVelocity;   //add player velocity
    mana.m_vVel.y = abs(mana.m_vVel.y); //make y velocity positive

    randColor = m_pRandom->randn(0, 2); //random color
    randSize = m_pRandom->randn(12, 22);//random size multiplier

    //select random color
    if (randColor == 0)
      mana.m_f4Tint = XMFLOAT4(Colors::Violet);
    else if (randColor == 1)
      mana.m_f4Tint = XMFLOAT4(Colors::DeepPink);
    else
      mana.m_f4Tint = XMFLOAT4(Colors::DarkGoldenrod);

    mana.m_f4Tint.w = 0.8f; //const alpha for all particles

    //apply random scale
    mana.m_fMaxScale = 0.1f * (float)randSize;

    //spawn particle
    if (mana.m_nSpriteIndex) {
      m_pParticleEngine->create(mana);
    }
  } //for

} //ManaRegenFx

/// Applies blood/damage effect visuals around the player
void CPlayer::DamageFx() {
  const int count = m_pRandom->randn(0, 12); //gen random amount of blood particles
  UINT randColor, randSize; //random vars
  LParticleDesc2D blood; //particle descriptor
  for (int i = 0; i < count; i++) {
    blood.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;

    //fade properties
    blood.m_fLifeSpan = 5.0f;
    blood.m_fFadeOutFrac = 0.9f;
    blood.m_fFadeInFrac = 0.1f;

    //get random position within the player sprite
    blood.m_vPos = GenRandomPosition(0.4f);

    //offset y position to match sprite, account for scale
    blood.m_vPos.y -= m_fRadius * 3.0f / 4.0f;

    randColor = m_pRandom->randn(0, 2);    //random color 
    randSize = m_pRandom->randn(0, 2);     //random size

    //select random color
    /*if (!randColor)
      blood.m_f4Tint = XMFLOAT4(Colors::Gold);
    else if (randColor == 1)
      blood.m_f4Tint = XMFLOAT4(Colors::AntiqueWhite);
    else
      blood.m_f4Tint = XMFLOAT4(Colors::LightGoldenrodYellow);*/
    blood.m_f4Tint = XMFLOAT4(0.651f, 0.063f, 0.118f, 0.0f); //blood red

    blood.m_f4Tint.w = 0.6f; //const alpha for all particles

    //select random size
    if (!randSize)
      blood.m_fMaxScale = 0.2f;
    else if (randSize == 1)
      blood.m_fMaxScale = 0.3f;
    else
      blood.m_fMaxScale = 0.4f;

    //spawn particle
    if (blood.m_nSpriteIndex) {
      m_pParticleEngine->create(blood);
    }
  } //for

} //DamageFx

/// Creates dust that trails behind the player
void CPlayer::RunFx() {
  UINT randColor, randSize, randAlpha, randSprite, randLife; //random vars
  LParticleDesc2D dust; //particle descriptor
  Vector2 input_dir = Vector2(m_bStrafeRight - m_bStrafeLeft, m_bStrafeUp - m_bStrafeDown);
  const float input_length = sqrt(input_dir.x * input_dir.x + input_dir.y * input_dir.y);
  dust.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;

  //fade properties
  dust.m_fFadeOutFrac = 0.8f;
  dust.m_fFadeInFrac = 0.01f;

  //get random position within the player sprite
  dust.m_vPos = GenRandomPosition(0.2f);

  //offset y position to spawn at feet, account for scale
  dust.m_vPos.y -= m_fRadius;

  //offset the position of the dust to spawn just behind the players feet from getting kicked
  dust.m_vPos += -Vector2(20.0f * cos(m_fFacingVector), 20.0f * sin(m_fFacingVector));

  //printf("%f\n%f\n\n", input_dir.x, input_dir.y); //TODO update after player refactor
  dust.m_vVel = input_dir * -1000.0f * m_pTimer->GetFrameTime();//TODO update after player refactor

  randAlpha = m_pRandom->randn(1, 8);    //random alpha
  randSize = m_pRandom->randn(1, 12);     //random size
  randSprite = m_pRandom->randn(0, 5);     //random size
  randLife = m_pRandom->randn(0, 100);
  randColor = m_pRandom->randn(0, 2);


  dust.m_f4Tint = XMFLOAT4(0.414f, 0.33f, 0.3012f, 0.0f);

  dust.m_fAlpha = 0.01f; // Alpha of the actual dust, should be quite see-through
  dust.m_fLifeSpan = 0.015f * (float)randLife;

  //TODO scale according to direction travelling
  dust.m_fMaxScale = 0.04f * (float)randSize; //apply random scale
  if (randSprite == 0) {
    dust.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;
    dust.m_fMaxScale = 0.05f * (float)randSize; //apply random scale
  }
  else {
    dust.m_nSpriteIndex = (UINT)eSprite::SmoothCircle;
    dust.m_fMaxScale = 0.02f * (float)randSize; //apply random scale
  }
  //TODO change dust color to match floor
  //TODO spawn secondary particles that travel slower after a short delay

  //spawn particle
  if (dust.m_nSpriteIndex) {
    m_pParticleEngine->create(dust);
  }
} //RunFx

void CPlayer::CollectKey() { m_bHasKey = true; }

void CPlayer::UseKey() { m_bHasKey = false; }

const bool CPlayer::HasKey() { return m_bHasKey; }