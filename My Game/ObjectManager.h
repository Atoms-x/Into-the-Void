/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#ifndef __L4RC_GAME_OBJECTMANAGER_H__
#define __L4RC_GAME_OBJECTMANAGER_H__

#include "BaseObjectManager.h"
#include "Object.h"
#include "Common.h"
#include "NextMapPorter.h"
#include "PrevMapPorter.h"
#include "FadeInObject.h"
#include "FadeOutObject.h"
#include "TitleCard.h"

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public LBaseObjectManager<CObject>,
  public CCommon
{
  private:
    void BroadPhase(); ///< Broad phase collision detection and response.
    void NarrowPhase(CObject*, CObject*); ///< Narrow phase collision detection and response.
    //bool AtWorldEdge(CObject*, Vector2&, float&) const; ///< Test whether at the edge of the world.

  public:
    CObject* create(eSprite, const Vector2&); ///< Create new object.

    virtual void draw(); ///< Draw all objects.

    void FireGun(CObject*, eSprite); ///< Fire object's gun.
    void SwingSword(CObject*, eSprite); ///< Swing object's Sword.
    //void FindClosest(const Vector2&, CObject*&, float&); ///< Find close objects.
    const size_t GetNumTurrets() const; ///< Get number of turrets in object list.
    const size_t GetEnemyCount() const; ///< Get number of enemies as size_t
    NextMapPorter* GetNextMap();
    PrevMapPorter* GetPreviousMap();
    FadeOutObject* GetFadeOut();
    FadeInObject* GetFadeIn();
    TitleCard* GetTitleCard();

}; //CObjectManager

#endif //__L4RC_GAME_OBJECTMANAGER_H__