/// \file MainMenuExit.h
/// \brief Interface for the MainMenuExit object class CMainMenu

#pragma once
#include "Object.h"

class CMainMenuExit : public CObject {
	public:
		bool m_bIsSelected; ///< Used for sprite and functions
		CMainMenuExit(const Vector2& p); ///< Constructor
		const Vector2& GetPos() const; ///< Get position
		virtual ~CMainMenuExit(); ///< Destructor
		void Select(); ///< Logic for when this object is selected
		void Highlight(); ///< Logic for highlighting the object
		void Normal(); ///< Logic for unhighlighting the object
};

