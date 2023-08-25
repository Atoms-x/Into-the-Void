/// \file HealthBar.h
/// \brief Interface for the HealthBar object class CHealthBar.

#ifndef __L4RC_GAME_HEALTHBAR_H__
#define __L4RC_GAME_HEALTHBAR_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// \brief The Healthbar object. 
///
/// The abstract representation of the Healthbar object. Visually
/// presents the players health amount

class CHealthBar : public CObject {
protected:

public:
  CHealthBar(const Vector2& p); ///< Constructor.

  const Vector2& GetPos() const; ///< Get position.

  virtual ~CHealthBar(); ///< Destructor.
}; //CHealthBar

#endif //__L4RC_GAME_HEALTHBAR_H__