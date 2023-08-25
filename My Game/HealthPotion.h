/// \file HealthPotion.h
/// \brief Interface for the HealthPotion object class CHealthPotion.

#ifndef __L4RC_GAME_HEALTHPOTION_H__
#define __L4RC_GAME_HEALTHPOTION_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The HealthPotion object. 
///
/// The abstract representation of the HealthPotion object. Imparts health to the player

class CHealthPotion : public CObject {

  protected:
    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision Response
    virtual void DeathFX(); ///< Death special effects.

  public:
    CHealthPotion(const Vector2& p); ///< Constructor.

    const Vector2& GetPos() const; ///< Get position.

    virtual ~CHealthPotion(); ///< Destructor.
  }; //CHealthPotion

#endif //__L4RC_GAME_HEALTHPOTION_H__