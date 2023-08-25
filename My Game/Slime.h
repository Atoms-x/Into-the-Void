/// \file Slime.h
/// \brief Interface for the slime object class CSlime.

#ifndef __L4RC_GAME_SLIME_H__
#define __L4RC_GAME_SLIME_H__

#include "Object.h"
#include "Common.h"

/// \brief The slime object. 
///
/// CSlime is the abstract representation of an ant object.

class CSlime : public CObject {
    friend class CObjectManager; ///< Object manager is a friend.

protected:
    UINT m_nMaxHealth; ///< Maximum health.
    UINT m_nHealth; ///< Current health.

    LEventTimer* m_pFrameEvent = nullptr; ///< Frame event timer.
    LEventTimer* m_pHopEvent = nullptr; ///< Hop event timer

    bool m_bStrayParity = true; ///< Stray from path left or right.
    bool m_bPreferPosRot = false; ///< Prefer positive rotation.

    eSlimeState state{ eSlimeState::NONE };

    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

    void StrayFromPath(); ///< Stray randomly from path.
    void SetRandHopDelay(float offset);
    void UpdateFramenumber(); ///< Update frame number.
    void HandleFrameLoop(size_t n);

public:
    CSlime(const Vector2&, const UINT maxHp); ///< Constructor. 
    ~CSlime(); ///< Destructor. 

    void idle();
    void hop();
    void hurt();
    void death();
    void deathFx();
    virtual void move(); ///< Move slime.
}; //CSlime

#endif //__L4RC_GAME_SLIME_H__