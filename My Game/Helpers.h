/// \file Helpers.h
/// \brief Interface for helper functions.

#ifndef __L4RC_GAME_HELPERS_H__
#define __L4RC_GAME_HELPERS_H__

#include "GameDefines.h"

const Vector2 AngleToVector(const float theta); ///< Convert angle to vector.
const Vector2 VectorNormalCC(const Vector2& v); ///< Counterclockwise normal.
const Vector2 RotateVector(const Vector2&, float); ///< Rotate a vector.
void NormalizeAngle(float& theta); ///< Normalize angle to \f$\pm\pi\f$.
int Clamp(int, int, int);

#endif //__L4RC_GAME_HELPERS_H__