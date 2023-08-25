#ifndef __L4RC_GAME_OBSTACLE_H__
#define __L4RC_GAME_OBSTACLE_H__

#include "Object.h"
#include "Common.h"

class Obstacle : public CObject {
	friend class CObjectManager;

	protected:
		UINT m_nPersistenceTimer = 0; ///< Decides the interval for persistency. i.e. how long its awake vs sleeping
		UINT m_nPersistenceLoop = 0;
	public:
		Obstacle(const Vector2&, UINT, eSprite); ///< Constructor
		~Obstacle(); ///< Destructor
		void UpdateFrameNumber(); ///< Updates the frame number. 
		bool InDamageFrame();
		void FixBoundingBox(float); ///< Fixes the bounding box to a smaller circle.
};

#endif