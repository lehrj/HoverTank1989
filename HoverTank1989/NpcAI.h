#pragma once
#include "Environment.h"
//#include "Vehicle.h"
#include "DebugData.h"

class NPCVehicle;
class Vehicle;

class NpcAI
{
public:
    NpcAI();
    NpcAI(const NPCVehicle* aOwner);
    ~NpcAI();

    float GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest);   
    Utility::Waypoint GetCurrentWayPoint() { return m_currentWaypoint; }
    float GetThrottleInput();   
    DirectX::SimpleMath::Vector3 GetVecToDestination();

    void InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer, std::shared_ptr<DebugData> aDebugPtr);

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

    enum class Behavior
    {
       
    };

    struct DestinationTargets
    {
        DirectX::SimpleMath::Vector3 currentTarget;
        DirectX::SimpleMath::Vector3 seekTarget;
        DirectX::SimpleMath::Vector3 wanderTarget;
        float wanderDistance;
        float wanderJitter;
        float wanderRadius;
    };

    //binary flags to indicate whether or not a behavior should be active
    int           m_iFlags;
    BehaviorType m_behaviorType;

    void CreateWayPath();

    void InitializeDestinationTargets();

    DirectX::SimpleMath::Vector3 Wander();

    DestinationTargets m_destinationTargets;

  

    Utility::Waypoint m_currentWaypoint;
    Utility::WayPath m_currentWayPath;
    DirectX::SimpleMath::Vector3 m_currentDestination;
    //a pointer to the owner of this instance
    NPCVehicle const* m_npcOwner;
    Vehicle const* m_playerVehicle;
    Environment const* m_environment;

    std::shared_ptr<DebugData> m_debugData;

};

