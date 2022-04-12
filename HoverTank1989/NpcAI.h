#pragma once
#include "Environment.h"

class NPCVehicle;

class NpcAI
{
public:
    NpcAI();
    NpcAI(const NPCVehicle* aOwner);
    ~NpcAI();

    Utility::Waypoint GetCurrentWayPoint() { return m_currentWaypoint; }
    void InitializeAI(Environment const* aEnvironment);
    void UpdateAI(const float aTimeStep);

    void SetCurrentWayPoint(const Utility::Waypoint aWayPoint) { m_currentWaypoint = aWayPoint; }

private:
    enum class BehaviorType
    {
        BEHAVIOR_NONE = 0x00000,
        BEHAVIOR_SEEK = 0x00002,
        BEHAVIOR_ARRIVE = 0x00008,
        BEHAVIOR_WANDER = 0x00010,
        BEHAVIOR_SEPARATION = 0x00040,
        BEHAVIOR_WALL_AVOIDANCE = 0x00200,
    };
    //binary flags to indicate whether or not a behavior should be active
    int           m_iFlags;

    Utility::Waypoint m_currentWaypoint;
    Utility::WayPath m_currentWayPath;
    
    //a pointer to the owner of this instance
    NPCVehicle const* m_npcOwner;
    Environment const* m_environment;
};

