/// \file SwordAttack.h
/// \Interface for the sword attack object class CSwordAttack

#ifndef _L4RC_GAME_SWORDATTACK_H
#define _L4RC_GAME_SWORDATTACK_H

#include "Object.h"

/// \brief the Sword Attack object
///
/// The abstract reperesentation of a sword attack object. Sword attack
/// objects die when they collide with anything (or reach end of lifespan) 
/// and emit hit particles for everything

class CSwordAttack : public CObject {
  protected:

    UINT m_nAttackTimer = 0;

    virtual void CollisionResponse(const Vector2&, float,
      CObject* = nullptr); ///< Collision response
    virtual void DeathFX(); ///<Death special effects

  public:
    CSwordAttack(eSprite t, const Vector2& p); ///< Constructor.

    virtual void move(); ///< Timed event

    UINT Timer(); ///< For lifetime of attack
}; //CSwordAttack

#endif // !_L4RC_GAME_BULLET_H