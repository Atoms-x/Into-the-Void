/// \file MainMenuBG.h
/// \brief Interface for the MainMenuBG object class CMainMenuBG

#pragma once
#include "Object.h"

class CMainMenuBG : public CObject {
	public:
		CMainMenuBG(const Vector2& p); ///< Constructor
		const Vector2& GetPos() const; ///< Get position
		virtual ~CMainMenuBG(); ///< Destructor
		void Begin(); ///< Starts the menu
};

