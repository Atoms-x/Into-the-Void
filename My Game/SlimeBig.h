/// \file Slime.h
/// \brief Interface for the ant object class CSlime.

#ifndef __L4RC_GAME_SLIMEBIG_H__
#define __L4RC_GAME_SLIMEBIG_H__

#include "Object.h"
#include "Common.h"

/// \brief The slime object. 
///
/// CSlimeBig is the abstract representation of a big slime object.

class CSlimeBig : public CObject {
    friend class CObjectManager; ///< Object manager is a friend.

protected:
    UINT m_nMaxHealth; ///< Maximum health.
    UINT m_nHealth; ///< Current health.
    UINT m_nJiggle;
    LEventTimer* m_pFrameEvent = nullptr; ///< Frame event timer.
    LEventTimer* m_pHopEvent = nullptr; ///< Hop event timer.

    bool m_bStrayParity = true; ///< Stray from path left or right.
    bool m_bPreferPosRot = false; ///< Prefer positive rotation.

    eSlimeState state{ eSlimeState::NONE }; ///< Slime animation state enum

    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.

    void StrayFromPath(); ///< Stray randomly from path.
    void SetRandHopDelay(float offset);
    void UpdateFramenumber(); ///< Update frame number.
    void HandleFrameLoop(int n); ///< Maintains slime state, called when a sprite reaches end of animation

    virtual void DeathFX(); ///< Death special effects.

public:
    CSlimeBig(const Vector2&, const UINT m_nMaxHp); ///< Constructor. 
    ~CSlimeBig(); ///< Destructor. 

    void idle();
    void hop();
    void hurt();
    void split();
    void splitFx();
    virtual void move(); ///< Move slime.
}; //CSlime

#endif //__L4RC_GAME_SLIMEBIG_H__