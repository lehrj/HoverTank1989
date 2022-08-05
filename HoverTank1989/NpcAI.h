#pragma once
#include "Environment.h"
#include "DebugData.h"

class NPCController;
class NPCVehicle;
class Vehicle;

struct AIOutput
{
    float       angleToDestination;
    float       forwardThrust;   
    DirectX::SimpleMath::Vector3 omniDirection;
    float       omniThrust;
    float       steeringAngle;
    DirectX::SimpleMath::Vector3 steeringDirection;
};

struct AIControls
{
    AIOutput aiOutput;
    const float throttleOutputMin = -1.0f;
    const float throttleOutputMax = 1.0f;
    const float steeringOutputMax = 1.0f;
    const float steeringOutputMin = -1.0f;
};

class NpcAI
{
public:
    NpcAI(const NPCVehicle* aOwner);
    void DebugToggle();
    float GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest);   
    DirectX::BoundingBox GetAiAvoidanceBox() const { return m_avoidanceBox; };
    DirectX::SimpleMath::Matrix GetAiAvoidanceBoxAlignment() const { return m_avoidanceAlignment; };
    AIOutput GetAiControlOutput() const { return m_aiControls.aiOutput; };
    
    bool GetIsAvoidanceTrue() const { return m_isAvoidanceTrue; };
    Utility::Waypoint GetCurrentWayPoint() { return m_currentWaypoint; }
    float GetThrottleInput();   
    DirectX::SimpleMath::Vector3 GetVecToDestination();

    void InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer, std::shared_ptr<DebugData> aDebugPtr, std::shared_ptr<NPCController> aNpcController);

    void UpdateAI(const float aTimeStep);
    void PushAiAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos);
    void PushAiAvoidanceTarget2(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle);
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

    struct DestinationTargets
    {
        DirectX::SimpleMath::Vector3 separationTarget;
        DirectX::SimpleMath::Vector3 currentTarget;
        DirectX::SimpleMath::Vector3 seekTarget;
        DirectX::SimpleMath::Vector3 wanderTarget;

        bool isSeparationTargetInRadius = false;
        float separationMagnitude;
        float separationRadius;
        float wanderDistance;
        float wanderJitter;
        float wanderRadius;
    };

    Behavior m_behavior;
    
    void AdjustHeadingForVelocity();
    void AdjustHeadingForVelocity2();
    void AvoidPos();
    void AvoidPos2();
    void AvoidPos3();
    void AvoidPos4();

    void CreateWayPath();

    void InitializeControlOutput();
    void InitializeDestinationTargets();
    void InitializeBehavior();
    
    void SetForwardThrustOutput();
    void SetOmniOutput();
    void SetSteeringOutput();

    void UpdateAvoidanceBox();
    void UpdateControlOutput();
    void UpdateDesiredHeading();
    void UpdateDestinationSmoothing();
    void UpdateSeparation();

    DirectX::SimpleMath::Vector3 Wander();

    DestinationTargets m_destinationTargets;

    //a pointer to the owner of this instance
    NPCVehicle const* m_npcOwner;
    Vehicle const* m_playerVehicle;
    Environment const* m_environment;
    std::shared_ptr<NPCController> m_npcController;

    Utility::Waypoint m_currentWaypoint;
    Utility::WayPath m_currentWayPath;
    DirectX::SimpleMath::Vector3 m_currentDestination;

    std::vector<DirectX::SimpleMath::Vector3> m_destinationSmoothing;
    const int m_destinationSmoothingSize = 4;


    DirectX::SimpleMath::Vector3 m_desiredHeading;
    float m_desiredVelocity;

    DirectX::SimpleMath::Matrix m_avoidanceAlignment = DirectX::SimpleMath::Matrix::Identity;
    DirectX::BoundingBox        m_avoidanceBox;
    const float                 m_avoidanceBoxLengthMin = 10.0f;
    float                       m_avoidanceBoxLength = m_avoidanceBoxLengthMin;
    float                       m_avoidanceBoxWidth;
    DirectX::SimpleMath::Vector3 m_avoidanceTarget = DirectX::SimpleMath::Vector3::Zero;
    NPCVehicle const*           m_avoidanceTarget2 = nullptr;
    bool                        m_isAvoidanceTrue = false;

    AIControls                  m_aiControls;

    std::shared_ptr<DebugData> m_debugData;
    bool m_debugToggle = false;
};

