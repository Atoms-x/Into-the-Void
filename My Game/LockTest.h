#ifndef __L4RC_GAME_LOCKTEST_H__
#define __L4RC_GAME_LOCKTEST_H__

#include "Unlockable.h"

class CObject;
/// <summary>
/// Test class for CUnlockable. It's a big pad lock that needs a key to unlock. 
/// Collision disabled after unlock. 
/// </summary>
class CLockTest :
    public virtual CUnlockable {
    friend class CObjectManager;
    public:
        CLockTest(const Vector2&, eSprite sprite);
        void virtual UnlockFx(); 
        void virtual CollisionResponse(const Vector2&, float, CObject*);
}; //CLockTest

#endif
