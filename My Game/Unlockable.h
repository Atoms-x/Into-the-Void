/// \file Unlockable.h
/// \brief Interface for the player object class CKey.

#ifndef __L4RC_GAME_UNLOCKABLE_H__
#define __L4RC_GAME_UNLOCKABLE_H__

#include "Object.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Helpers.h"
#include "ComponentIncludes.h"

/// <summary>
/// Inherit from this object to easily enable locking/unlocking behavior. See CLockTest for reference
/// </summary>
class CUnlockable : public CObject {
	friend class CObjectManager;
	protected:
		bool locked;
	public:
		CUnlockable(const Vector2&, eSprite); ///< Constructor
		CUnlockable(const Vector2&, eSprite, bool); ///< Constructor overload
		~CUnlockable(); ///< Destructor
		void virtual UpdateFrameNumber(); ///< Updates the frame number. 
		void virtual HandleFrameLoop(size_t n); //handles end of anim and sprite/frame changes

		void Unlock();	//unlock the object and play unlock fx
		void Lock();	//lock the object and play lock fx

		void virtual UnlockFx();	//called when unlocked
		void virtual LockFx();		//called when locked	
};  //CUnlockable

#endif //__L4RC_GAME_UNLOCKABLE_H__