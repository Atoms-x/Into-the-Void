/// \file ManaBar.h
/// \brief Interface for the ManaBar object class CManaBar.

#ifndef __L4RC_GAME_MANABAR_H__
#define __L4RC_GAME_MANABAR_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The Manabar object. 
///
/// The abstract representation of the Manabar object. Visually
/// presents the players mana amount

class CManaBar : public CObject {
protected:

public:
  CManaBar(const Vector2& p); ///< Constructor.

  const Vector2& GetPos() const; ///< Get position.

  virtual ~CManaBar(); ///< Destructor.
}; //CManaBar

#endif //__L4RC_GAME_MANABAR_H__