/// \file TimerSpawn.cpp

#include "TimedSpawn.h"
/// <summary>
/// Object used to spawn a particle at a given time. Collection stored in Common
/// </summary>
/// <param name="time">time to spawn. use m_pTimer->GetTime() and add an offset to it </param>
/// <param name="part">particle to spawn</param>
CTimedSpawn::CTimedSpawn(float time, LParticleDesc2D part) {
    m_fTriggerTime = time;
    particle = part;
} //constructor

CTimedSpawn::~CTimedSpawn() {} //destructor

//accessors
float CTimedSpawn::GetTime() const{ return m_fTriggerTime; }
LParticleDesc2D CTimedSpawn::GetPart() const { return particle; }

//mutators
void CTimedSpawn::SetTime(float time) { m_fTriggerTime = time; }
void CTimedSpawn::SetPart(LParticleDesc2D part) { particle = part; }

// comparator overload definitions - received help from this guide:
// http://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
//      - though this code seems to be pretty standard when using priority_queues
// These overloads allow CTimedSpawn objects to be sorted by their float(time) elements
    // greater than
bool operator > (const CTimedSpawn& first, const CTimedSpawn& second) { 
    return (first.GetTime() > second.GetTime());
}
    // less than
bool operator < (const CTimedSpawn& first, const CTimedSpawn& second) {
    return (first.GetTime() < second.GetTime());
}
