/// \file StaminaWheel.cpp
/// \brief Code for the StaminaWheel object class CStaminaWheel.

#include "StaminaWheel.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"

/// Create and initialize a StaminaWheel object given its initial position.
/// \param p Initial position of StaminaWheel.

CStaminaWheel::CStaminaWheel(const Vector2& p) : CObject(eSprite::StaminaWheelFull, p) {
  objType = (UINT)eObjectType::UI;
  m_bIsTarget = false;
  m_bStatic = false;
} //constructor

CStaminaWheel::~CStaminaWheel() {
} //destructor

/// Reader function for position.
/// \return Position.

const Vector2& CStaminaWheel::GetPos() const {
  return m_vPos;
} //GetPos

