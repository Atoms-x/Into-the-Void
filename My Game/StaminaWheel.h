/// \file StaminaWheel.h
/// \brief Interface for the StaminaWheel object class CStaminaWheel.

#ifndef __L4RC_GAME_STAMINAWHEEL_H__
#define __L4RC_GAME_STAMINAWHEEL_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The StaminaWheel object. 
///
/// The abstract representation of the StaminaWheel object. Visually
/// presents the players stamina amount

class CStaminaWheel : public CObject {
protected:

public:
  CStaminaWheel(const Vector2& p); ///< Constructor.

  const Vector2& GetPos() const; ///< Get position.

  virtual ~CStaminaWheel(); ///< Destructor.
}; //CStaminaWheel

#endif //__L4RC_GAME_STAMINAWHEEL_H__