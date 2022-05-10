#pragma once
#include "Environment.h"
#include "DebugData.h"

class NPCVehicle;
class Vehicle;

class NpcAI
{
public:
    NpcAI(const NPCVehicle* aOwner);

    float GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest);   
    Utility::Waypoint GetCurrentWayPoint() { return m_currentWaypoint; }
    float GetThrottleInput();   
    DirectX::SimpleMath::Vector3 GetVecToDestination();

    void InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer, std::shared_ptr<DebugData> aDebugPtr);

    void UpdateAI(const float aTimeStep);

    void SetCurrentWayPoint(const Utility::Waypoint aWayPoint) { m_currentWaypoint = aWayPoint; }

private:
    struct Behavior
    {
        bool none;
        bool seek;
        bool arrive;
        bool wander;
        bool separation;
        bool stop;
        bool wallAvoidance;
        bool followWayPath;

        const float wayPathWeight = 1.0f;
        const float seekWeight = 1.0f;
        const float arriveWeight = 1.0f;
        const float wanderWeight = 0.5f;
        const float separationWeight = 1.0f;
        const float stopWeight = 1.0f;
        const float wallAvoidanceWeight = 1.0f;
    };

    /*
    struct BehaviorWeights
    {

    };
    */

    struct DestinationTargets
    {
        DirectX::SimpleMath::Vector3 currentTarget;
        DirectX::SimpleMath::Vector3 seekTarget;
        DirectX::SimpleMath::Vector3 wanderTarget;
        float wanderDistance;
        float wanderJitter;
        float wanderRadius;
    };

    Behavior m_behavior;
    
    void CreateWayPath();

    void InitializeDestinationTargets();
    void InitializeBehavior();
    void UpdateDesiredHeading();

    DirectX::SimpleMath::Vector3 Wander();

    DestinationTargets m_destinationTargets;

    Utility::Waypoint m_currentWaypoint;
    Utility::WayPath m_currentWayPath;
    DirectX::SimpleMath::Vector3 m_currentDestination;

    DirectX::SimpleMath::Vector3 m_desiredHeading;
    float m_desiredVelocity;

    //a pointer to the owner of this instance
    NPCVehicle const* m_npcOwner;
    Vehicle const* m_playerVehicle;
    Environment const* m_environment;

    std::shared_ptr<DebugData> m_debugData;
};

