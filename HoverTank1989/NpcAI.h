#pragma once
#include "Environment.h"
//#include "Vehicle.h"

class NPCVehicle;
class Vehicle;

class NpcAI
{
public:
    NpcAI();
    NpcAI(const NPCVehicle* aOwner);
    ~NpcAI();

    float GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest);
    float GetThrottleInput();

    Utility::Waypoint GetCurrentWayPoint() { return m_currentWaypoint; }
    void InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer);

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
    BehaviorType m_behaviorType;

    void CreateWayPath();

    Utility::Waypoint m_currentWaypoint;
    Utility::WayPath m_currentWayPath;
    DirectX::SimpleMath::Vector3 m_currentDestination;
    //a pointer to the owner of this instance
    NPCVehicle const* m_npcOwner;
    Vehicle const* m_playerVehicle;
    Environment const* m_environment;

};

