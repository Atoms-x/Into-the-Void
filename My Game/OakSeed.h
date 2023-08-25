/// \file OakSeed.h
/// \brief Interface for the OakSeed object class COakSeed.

#ifndef __L4RC_GAME_OAKSEED_H__
#define __L4RC_GAME_OAKSEED_H__

#include "Object.h"
#include "Common.h"

/// \brief The Oakseed object. 
///
/// COakSeed is the abstract representation of an Oakseed object.

class COakSeed : public CObject {
  friend class CObjectManager; ///< Object manager is a friend.

protected:
  UINT m_nMaxHealth = 3; ///< Maximum health.
  UINT m_nHealth = m_nMaxHealth; ///< Current health.

  LEventTimer* m_pIdleEvent = nullptr; ///< Idle event timer.
  LEventTimer* m_pWalkEvent = nullptr; ///< Walk event timer
  LEventTimer* m_pDamageEvent = nullptr; ///< Damage event timer
  LEventTimer* m_pDeathEvent = nullptr; ///< Death Event timer
  LEventTimer* m_pWanderEvent = nullptr; ///< Wander Event timer

  bool m_bStrayParity = true; ///< Stray from path left or right.
  bool m_bPreferPosRot = false; ///< Prefer positive rotation.
  bool m_bPlayerDetected = false; ///< Whether or not player has been detected
  bool m_bShielded = false; ///< Whether or not player is behind shielded side of enemy

  eOakSeedState state{ eOakSeedState::NONE };

  virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

  void UpdateFramenumber(); ///< Update frame number.

public:
  COakSeed(eSprite t, const Vector2&); ///< Constructor. 
  ~COakSeed(); ///< Destructor. 

  void Idle();
  void Wander();
  void DamageTrigger();
  void Damage();
  void Death();
  void DeathFx();
  void FacePlayer();
  void RotateTowards(const Vector2& pos, float dsq);
  void MoveTowardsPlayer();
  virtual void move(); ///< Move OakSeed.
}; //COakSeed

#endif //__L4RC_GAME_OAKSEED_H__