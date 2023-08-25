#pragma once
#include "Object.h"

class CMainMenuStart : public CObject {
	public:
		bool m_bIsSelected; ///< Used for sprite and functions
		CMainMenuStart(const Vector2& p); ///< Constructor
		const Vector2& GetPos() const; ///< Get position
		virtual ~CMainMenuStart(); ///< Destructor
		bool Select(); ///< Starts the menu
		void Highlight(); ///< Logic for highlighting the object
		void Normal(); ///< Logic for highlighting the object
};