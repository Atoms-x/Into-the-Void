/// \file Player.h
/// \brief Interface for the player object class CPlayer.

#ifndef __L4RC_GAME_PLAYER_H__
#define __L4RC_GAME_PLAYER_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The player object. 
///
/// The abstract representation of the player object. The player differs from
/// the other objects in the game in that it moves in respond to device inputs.

class CPlayer : public CObject {
protected:
  const UINT m_nMaxHealth = 6; ///< Maximum health.
  const UINT m_nMaxMana = 6; ///< Maximum Mana

  const UINT m_nMaxDashes = 3; ///< Total number of dashes available
  UINT m_nDashTimer = 0;
  UINT m_nDashesLeft = m_nMaxDashes; ///< Currently used number of dashes
  int m_nPortAccept = 0;
  UINT m_nShakeTimer = 0;

  UINT m_nHealth = m_nMaxHealth; ///< Current health.
  UINT m_nMana = m_nMaxMana; ///< Current Mana
  UINT m_nStatusEff; ///< Indicates status effect for player

  UINT m_nDust = 0;

  float m_nFacingVectorAtShot = 0.0f;

  bool m_bStrafeLeft = false; ///< Strafe left.
  bool m_bStrafeRight = false; ///< Strafe right.
  bool m_bStrafeDown = false; ///< Strafe down.
  bool m_bStrafeUp = false; ///< Strafe up.
  bool m_bIdle = false; ///< FOR IDLE ANIMATION
  bool m_bHasKey = false; ///< player picked up key

  LEventTimer* m_pFrameEvent = nullptr; ///< Frame event timer.
  LEventTimer* m_pDamageEvent = nullptr; ///< Player damage event timer
  LEventTimer* m_pDeathEvent = nullptr; ///< Player death event timer
  LEventTimer* m_pDashEvent = nullptr; ///< Player Dash event timer
  LEventTimer* m_pRunEvent = nullptr; ///< Player Run event timer
  LEventTimer* m_pIdleEvent = nullptr; ///< Player Idle event timer

  ePlayerAnimationState state{ ePlayerAnimationState::NONE };

  void UpdateFramenumber(); ///< Update frame number.

  virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

public:
  int m_nNextLevel = 0;
  bool m_bManaNeeded = false;
  bool m_bHealthNeeded = false;
  bool m_bIsAttacking = false;
  bool m_bIsTakingDamage = false;
  bool m_bIsDashing = false;
  CPlayer(const Vector2& p); ///< Constructor.

  virtual void move(); ///< Move player object.

  void Idle(); ///< FOR IDLE ANIMATION

  void Run(); ///< FOR RUN ANIMATION
  void StrafeLeft(); ///< Strafe left.
  void StrafeRight(); ///< Strafe right.
  void StrafeDown(); ///< Strafe back.
  void StrafeUp(); ///< Strafe up.
  void SetPort(int);
  int GetPort();
  void knockback(); ///< knockback on hit
  void DamageTrigger(); ///< trigger

  void DeathTrigger(); ///< trigger for death
  void Death(); ///< FOR DEATH

  void DashTrigger(); ///< Dash trigger
  void Dash(); ///< Dash 
  void DashFX(); ///< Dash animations

  void HealFx(); ///< Heal visuals
  void ManaRegenFx(); ///< Mana regen visuals
  void DamageFx(); ///<spawn blood particles when damaged
  void RunFx(); ///< dust particles when running

  void Attack(); ///< Attack anim trigger
  void AttackTrigger(); ///< Attack anim

  void Timer(); ///< For anything timer
  void ShakeTimer(); ///< Camera shake timer

  void CollectKey(); ///< Player picks up key
  const bool HasKey(); ///< key bool accessor
  void UseKey(); //< reduce key count - set to false

  void SetMana(UINT t); ///< Setter function for mana
  void SetStamina(UINT t); ///< Setter function for mana
  void SetFacingVectorAtShot(float t); ///< Setter function for facing vector at shot

  const UINT GetHealth() const; ///< Getter function for health bar and health functions
  const UINT GetMana() const; ///< Getter function for mana bar and mana functions
  const UINT GetStamina() const; ///< Getter function for stamina bar and timed recovery
  const float GetFacingVectorAtShot() const; ///< Getter function for facing vector at shot
  const void StatusEffect(); ///< Applies status effect visuals based on m_nStatusEff value

  const Vector2& GetPos() const; ///< Get position.

  virtual ~CPlayer(); ///< Destructor.
}; //CPlayer

#endif //__L4RC_GAME_PLAYER_H__