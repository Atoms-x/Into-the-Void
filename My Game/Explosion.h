/// \file Explosion.h
/// \brief Interface for the explosion object class CExplosion.

#ifndef __L4RC_GAME_EXPLOSION_H__
#define __L4RC_GAME_EXPLOSION_H__

#include "Object.h"

/// \brief The explosion object. 
///
/// The abstract representation of an explosion object. Explosion objects die when
/// they finish their animation

class CExplosion : public CObject {
protected:

  LEventTimer* m_pExplosionEvent = nullptr;

  void UpdateFramenumber();

public:
  CExplosion(eSprite t, const Vector2& p); ///< Constructor.

  virtual void move();

  virtual ~CExplosion();
}; //CBullet

#endif //__L4RC_GAME_EXPLOSION_H__