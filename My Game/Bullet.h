/// \file Bullet.h
/// \brief Interface for the bullet object class CBullet.

#ifndef __L4RC_GAME_BULLET_H__
#define __L4RC_GAME_BULLET_H__

#include "Object.h"

/// \brief The bullet object. 
///
/// The abstract representation of a bullet object. Bullet objects die when
/// they collide with anything and emit a cloud of smoke for anything except
/// an ant.

class CBullet: public CObject{
  protected:

    LEventTimer* m_pFireballEvent = nullptr; ///< Event timer for fireball animation

    void UpdateFramenumber();

    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

  public:
    CBullet(eSprite t, const Vector2& p); ///< Constructor.

    virtual void move();

    void SmokeFX();

    virtual ~CBullet();
}; //CBullet

#endif //__L4RC_GAME_BULLET_H__