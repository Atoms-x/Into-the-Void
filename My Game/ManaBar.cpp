/// \file ManaBar.cpp
/// \brief Code for the healthbar object class CManaBar.

#include "ManaBar.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"

/// Create and initialize an manabar object given its initial position.
/// \param p Initial position of manabar.

CManaBar::CManaBar(const Vector2& p) : CObject(eSprite::ManaBarFull, p) {
  objType = (UINT)eObjectType::UI;
  m_bIsTarget = false;
  m_bStatic = false;
} //constructor

CManaBar::~CManaBar() {
} //destructor

/// Reader function for position.
/// \return Position.

const Vector2& CManaBar::GetPos() const {
  return m_vPos;
} //GetPos