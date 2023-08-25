/// \file Explosion3.h
/// \brief Interface for the explosion3 object class CExplosion3.

#ifndef __L4RC_GAME_EXPLOSION3_H__
#define __L4RC_GAME_EXPLOSION3_H__

#include "Object.h"

/// \brief The explosion3 object. 
///
/// The abstract representation of an explosion3 object. Explosion3 objects die when
/// they finish their animation

class CExplosion3 : public CObject {
protected:

  LEventTimer* m_pExplosionEvent = nullptr;

  void UpdateFramenumber();

public:
  CExplosion3(eSprite t, const Vector2& p); ///< Constructor.

  virtual void move();

  virtual ~CExplosion3();
}; //CBullet

#endif //__L4RC_GAME_EXPLOSION3_H__