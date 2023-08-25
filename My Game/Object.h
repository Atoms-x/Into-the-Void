/// \file Object.h
/// \brief Interface for the game object class CObject.

#ifndef __L4RC_GAME_OBJECT_H__
#define __L4RC_GAME_OBJECT_H__

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "Common.h"
#include "Component.h"
#include "SpriteDesc.h"
#include "BaseObject.h"
#include "EventTimer.h"

/// \brief The game object. 
///
/// The abstract representation of an object. `CObjectManager` is a friend of
/// this class so that it can access any private members as needed to manage
/// the objects without the need for reader and set functions for each private
/// or protected member variable. This class must contain public member
/// functions `move()` and `draw()` to move and draw the object, respectively.

class CObject:
  public CCommon,
  public LBaseObject
{
  friend class CObjectManager; ///< Object manager needs access so it can manage.

  protected:
    float m_fRadius = 0; ///< Bounding circle radius.

    float m_fSpeed = 0; ///< Speed.
    float m_fRotSpeed = 0; ///< Rotational speed.
    Vector2 m_vVelocity; ///< Velocity.
    bool m_bStatic = true; ///< Is static (does not move).
    bool m_bIsTarget = true; ///< Is a target.
    bool m_bDeadCheck = false;

    UINT objType = 0;

    float m_fFacingVector = 0.0f; ///< For new direction vector

    LEventTimer* m_pGunFireEvent = nullptr; ///< Gun fire event.
    LEventTimer* m_pSwordAttackEvent = nullptr; ///< Sword Attack event
    
    virtual void CollisionResponse(const Vector2&, float,
    CObject* = nullptr); ///< Collision response.
    virtual void DeathFX(); ///< Death special effects.

  public:
    CObject(eSprite, const Vector2&); ///< Constructor.
    
    void move(); ///< Move object.
    void draw(); ///< Draw object.

    const UINT GetType() const; 

    const bool isBullet() const; ///< Is a bullet.
    const bool isSword() const; ///< Is a sword attack.

    const Vector2 GetViewVector() const; ///< Compute view vector.
    const Vector2 GetFacingVector() const; ///< Compute facing vector.
    const float GetFacingVectorRad() const; ///< Get facing vector in radians.
    const Vector2 GenRandomPosition(float radiusMultiplier) const; ///< Returns random position Vector2 that lies within the callers radius

    const bool CObject::isPlayer() const;
    const bool CObject::isHealthPotion() const;
    const bool CObject::isManaPotion() const;
    const bool CObject::isSlime() const;
    const bool CObject::GetIsDead() const;
    virtual ~CObject(); ///< Destructor.
}; //CObject

#endif //__L4RC_GAME_OBJECT_H__