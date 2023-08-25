/// \file HealthBar.cpp
/// \brief Code for the healthbar object class CHealthBar.

#include "HealthBar.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"

/// Create and initialize an HealthBar object given its initial position.
/// \param p Initial position of HealthBar.

CHealthBar::CHealthBar(const Vector2& p) : CObject(eSprite::HealthBarFull, p) {
  objType = (UINT)eObjectType::UI;
  m_bIsTarget = false;
  m_bStatic = false;
} //constructor

CHealthBar::~CHealthBar() {
} //destructor

/// Reader function for position.
/// \return Position.

const Vector2& CHealthBar::GetPos() const {
  return m_vPos;
} //GetPos