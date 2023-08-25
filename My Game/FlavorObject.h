
#ifndef __L4RC_GAME_FLAVOROBJECT_H__
#define __L4RC_GAME_FLAVOROBJECT_H__

#include "Object.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Helpers.h"
#include "ComponentIncludes.h"

class CFlavorObject : 
    public CObject{
    friend class CObjectManager;
    public: 
        CFlavorObject(const Vector2& p, eSprite); ///< Constructor.
        
        const Vector2& GetPos() const; ///< Get position.

        virtual ~CFlavorObject(); ///< Destructor.
}; //CFlavorObject

#endif //__L4RC_GAME_FLAVOROBJECT_H__

