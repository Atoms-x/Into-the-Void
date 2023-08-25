/// \file Helpers.cpp
/// \brief Code for helper functions.

#include "Helpers.h"

/// Compute a unit vector at an angle measured in radians counterclockwise
/// from the positive X-axis. If \f$\vec{v} = [v_x, v_y]\f$
/// is a unit vector with tail at the Origin and \f$\theta\f$ is the angle
/// between \f$\vec{v}\f$ and the positive X-axis, then
/// \f$\sin \theta = v_y\f$ and \f$\cos \theta = v_x\f$. Hence,
/// \f$\vec{v} = [\cos \theta, \sin \theta]\f$, as shown below. Therefore,
/// this function returns `Vector2(cosf(theta), sinf(theta))`.
/// @image html AngleToVector.png width=220
/// \param theta Angle in radians.
/// \return Unit vector at angle theta counterclockwise from positive X.

const Vector2 AngleToVector(const float theta){
  return Vector2(cosf(theta), sinf(theta));
} //AngleToVector

/// Compute the counterclockwise unit perpendicular to a vector.
/// If \f$\vec{v} = [v_x, v_y]\f$, then both dot products 
/// \f$\vec{v} \cdot [-v_y, v_x]\f$ and \f$\vec{v} \cdot [v_y, -v_x]\f$ are
/// equal to zero, and therefore both \f$[-v_y, v_x]\f$ and \f$[v_y, -v_x]\f$
/// are perpendicular to \f$\vec{v}\f$. The former (drawn in green below) points
/// counterclockwise from \f$\vec{v}\f$ and the latter (drawn in purple below)
/// points clockwise from \f$\vec{v}\f$. Therefore, this function computes
/// `Vector2(-v.y, v.x)` and normalizes it before returning it.
/// @image html Perp.png width=200
/// \param v A vector, not necessarily normalized.
/// \return The counterclockwise unit perpendicular to v.

const Vector2 VectorNormalCC(const Vector2& v){
  Vector2 u(-v.y, v.x);
  u.Normalize();
  return u;
} //VectorNormalCC

/// Normalize an angle in radians to \f$\pm\pi\f$. If the angle is very large
/// or very small, then this function will be very slow. However, it will be
/// very fast if the angle is not too far out of range.
/// \param theta [in, out] An angle in radians.

void NormalizeAngle(float& theta){
  while(theta < -XM_PI)theta += XM_2PI;
  while(theta >  XM_PI)theta -= XM_2PI;
} //NormalizeAngle

/// Helper function to rotate a vector by ab angle in radians.
/// \param v [in] Vector to be rotated.
/// \param theta Rotation angle in radians counterclockwise.
/// \return Rotated vector.

const Vector2 RotateVector(const Vector2& v, float theta){
  const float sintheta = sinf(theta);
  const float costheta = cosf(theta);
  return Vector2(v.x*costheta - v.y*sintheta, v.x*sintheta + v.y*costheta);
} //Rotate

int Clamp(int a, int high, int low)
{
    if (a > high) {
        return high;
    }
    else if (a < low) {
        return low;
    }
    else {
        return a;
    }
}