/// \file ManaPotion.h
/// \brief Interface for the ManaPotion object class CManaPotion.

#ifndef __L4RC_GAME_MANAPOTION_H__
#define __L4RC_GAME_MANAPOTION_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The ManaPotion object. 
///
/// The abstract representation of the ManaPotion object. Imparts Mana to the player

class CManaPotion : public CObject {

protected:
  virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision Response
  virtual void DeathFX(); ///< Death special effects.

public:
  CManaPotion(const Vector2& p); ///< Constructor.

  const Vector2& GetPos() const; ///< Get position.

  virtual ~CManaPotion(); ///< Destructor.
}; //CManaPotion

#endif //__L4RC_GAME_MANAPOTION_H__