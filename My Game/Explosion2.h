/// \file Explosion2.h
/// \brief Interface for the explosion2 object class CExplosion2.

#ifndef __L4RC_GAME_EXPLOSION2_H__
#define __L4RC_GAME_EXPLOSION2_H__

#include "Object.h"

/// \brief The explosion2 object. 
///
/// The abstract representation of an explosion2 object. Explosion2 objects die when
/// they finish their animation

class CExplosion2 : public CObject {
protected:

  LEventTimer* m_pExplosionEvent = nullptr;

  void UpdateFramenumber();

public:
  CExplosion2(eSprite t, const Vector2& p); ///< Constructor.

  virtual void move();

  virtual ~CExplosion2();
}; //CBullet

#endif //__L4RC_GAME_EXPLOSION2_H__