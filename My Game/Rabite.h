/// \file Rabite.h
/// \brief Interface for the Rabite object class CRabite.

#ifndef __L4RC_GAME_RABITE_H__
#define __L4RC_GAME_RABITE_H__

#include "Object.h"
#include "Common.h"

/// \brief The rabite object. 
///
/// CRabite is the abstract representation of a Rabite object.

class CRabite : public CObject {
  friend class CObjectManager; ///< Object manager is a friend.

protected:
  UINT m_nMaxHealth = 3; ///< Maximum health.
  UINT m_nHealth = m_nMaxHealth; ///< Current health.

  LEventTimer* m_pIdleEvent = nullptr; ///< Idle event timer.
  LEventTimer* m_pHopEvent = nullptr; ///< Hop event timer
  LEventTimer* m_pDamageEvent = nullptr; ///< Damage event timer
  LEventTimer* m_pBiteEvent = nullptr; ///< Bite event timer
  LEventTimer* m_pDeathEvent = nullptr; ///< Death Event timer
  LEventTimer* m_pWanderEvent = nullptr; ///< Wander Event timer

  bool m_bStrayParity = true; ///< Stray from path left or right.
  bool m_bPreferPosRot = false; ///< Prefer positive rotation.
  bool m_bPlayerDetected = false; ///< Whether or not player has been detected
  bool m_bBiting = false; ///< Whether or not the rabite is dashbiting and shouldn't be turning

  eRabiteState state{ eRabiteState::NONE };

  virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

  void UpdateFramenumber(); ///< Update frame number.

public:
  CRabite(eSprite t, const Vector2&); ///< Constructor. 
  ~CRabite(); ///< Destructor. 

  void Idle();
  void Wander();
  void DamageTrigger();
  void Damage();
  void Death();
  void DeathFx();
  void FacePlayer();
  void RotateTowards(const Vector2& pos, float dsq);
  void MoveTowardsPlayer();
  void BiteDashTrigger();
  void BiteDash();
  void BiteDashFX();
  virtual void move(); ///< Move rabite.
}; //CRabite

#endif //__L4RC_GAME_Rabite_H__