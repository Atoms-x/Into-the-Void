/// \file TimeSpawn.h
/// Pass a CTimedSpawn object to the container in Common to have the particle created
/// at time m_fTriggerTime

#ifndef __L4RC_GAME_TIMEDSPAWN_H__
#define __L4RC_GAME_TIMEDSPAWN_H__

#include "Particle.h"
#include "Common.h"

/// CTimedSpawn stores a particle effect and time value. This object can be 
/// created by a game object, then passed to the container in common. This dissociates 
/// the particle effect from the game object, so if the game object is deleted
/// the particle effect will still play at the trigger time. This may help to 
/// simplify the logic needed to make particle effects as well

class CTimedSpawn{
protected:
    float m_fTriggerTime;     //time when particle should spawn
    LParticleDesc2D particle; //particle to spawn
public:
    CTimedSpawn(float time, LParticleDesc2D part);  //constructor
    ~CTimedSpawn(); //destructor
    //TODO expand to work with general sprites as well as particles

    //accessors
    float GetTime() const;
    LParticleDesc2D GetPart() const;

    //mutators
    void SetTime(float time);
    void SetPart(LParticleDesc2D);

    // comparator overload declarations- received help from this guide:
    // http://neutrofoton.github.io/blog/2016/12/29/c-plus-plus-priority-queue-with-comparator/
    // allows overload of std::less and std::greater for std::priority_queue
    friend bool operator > (const CTimedSpawn& first, const CTimedSpawn& second);   
    friend bool operator < (const CTimedSpawn& first, const CTimedSpawn& second);
}; //CTimedSpawn

#endif //__L4RC_GAME_TIMEDSPAWN_H__
