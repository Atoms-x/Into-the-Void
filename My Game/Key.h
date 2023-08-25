/// \file Key.h

#ifndef __L4RC_GAME_KEY_H__
#define __L4RC_GAME_KEY_H__

#include "Object.h"
#include "EventTimer.h"
#include "Common.h"

/// <summary>
/// A key object to unlock things with
/// </summary>

class CKey : public CObject {

protected:
    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision Response
    virtual void DeathFX(); ///< Death special effects.

public:
    CKey(const Vector2& p); ///< Constructor.

    const Vector2& GetPos() const; ///< Get position.

    virtual ~CKey(); ///< Destructor.
}; //CKey

#endif //__L4RC_GAME_KEY_H__