#include "pch.h"
#include "NpcAI.h"
#include "NPCVehicle.h"
#include "Vehicle.h"

NpcAI::NpcAI()
{

}

NpcAI::NpcAI(const NPCVehicle* aOwner)
{
    m_npcOwner = aOwner;
    m_iFlags = 0;
    
}

NpcAI::~NpcAI()
{

}
void NpcAI::CreateWayPath()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 15.0f;
    DirectX::SimpleMath::Vector3 pos;
    Utility::Waypoint wp;

    /*
    pos = DirectX::SimpleMath::Vector3(200.0f, 3.0f, 0.0f);
    wp = Utility::CreateWaypoint(pos, radius);
    Utility::PushWaypointToPath(m_currentWayPath, wp);

    pos = DirectX::SimpleMath::Vector3(200.0f, 3.0f, 100.0f);
    wp = Utility::CreateWaypoint(pos, radius);
    Utility::PushWaypointToPath(m_currentWayPath, wp);

    pos = DirectX::SimpleMath::Vector3(50.0f, 3.0f, 100.0f);
    wp = Utility::CreateWaypoint(pos, radius);
    Utility::PushWaypointToPath(m_currentWayPath, wp);

    pos = DirectX::SimpleMath::Vector3(50.0f, 3.0f, 0.0f);
    wp = Utility::CreateWaypoint(pos, radius);
    Utility::PushWaypointToPath(m_currentWayPath, wp);
    */

    Utility::Waypoint wp1;
    pos = DirectX::SimpleMath::Vector3(300.0f, 3.0f, -100.0f);
    wp1 = Utility::CreateWaypoint(pos, radius);

    Utility::Waypoint wp2;
    pos = DirectX::SimpleMath::Vector3(300.0f, 3.0f, 100.0f);
    wp2 = Utility::CreateWaypoint(pos, radius);

    Utility::Waypoint wp3;
    pos = DirectX::SimpleMath::Vector3(75.0f, 3.0f, 100.0f);
    wp3 = Utility::CreateWaypoint(pos, radius);

    Utility::Waypoint wp4;
    pos = DirectX::SimpleMath::Vector3(75.0f, 3.0f, -100.0f);
    wp4 = Utility::CreateWaypoint(pos, radius);

    const int id = m_npcOwner->GetID();
    if (id % 2)
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
    }
    else
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
    }

    m_currentWayPath.targetNode = 0;
}

float NpcAI::GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest)
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination - m_npcOwner->GetPos();
    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = m_npcOwner->GetForward();
    const DirectX::SimpleMath::Vector3 two = dest;
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    const float det = one.x * two.y * aUp.z + two.x * aUp.y * one.z + aUp.x * one.y * two.z - one.z * two.y * aUp.x - two.z * aUp.y * one.x - aUp.z * one.y * two.x;
    const float angle = atan2(det, dot);
    
    return angle;
}

DirectX::SimpleMath::Vector3 NpcAI::GetVecToDestination()
{
    DirectX::SimpleMath::Vector3 vecToDestination = m_currentDestination - m_npcOwner->GetPos();
    vecToDestination.Normalize();
    return vecToDestination;
}

float NpcAI::GetThrottleInput()
{
    DirectX::SimpleMath::Vector3 pos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 directionToDest = m_currentDestination - pos;
    DirectX::SimpleMath::Vector3 directionNorm = directionToDest;
    directionNorm.Normalize();

    DirectX::SimpleMath::Vector3 crossProd = directionNorm.Cross(m_npcOwner->GetRight());
    bool isFacingDest;
    if (directionNorm.Cross(m_npcOwner->GetRight()).y >= 0.0f)
    {
        isFacingDest = false;
    }
    else
    {
        isFacingDest = true;
    }
    
    if (isFacingDest == false)
    {
        return 0.0f;
    }
    else
    {
        return 1.0f;
    }   
}

void NpcAI::InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer, std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
    m_environment = aEnvironment;
    m_playerVehicle = aPlayer;
    InitializeDestinationTargets();
    m_currentWaypoint.waypointPos = DirectX::SimpleMath::Vector3::Zero;
    m_currentWaypoint.waypointRadius = 1.0f;
    Utility::ClearWayPath(m_currentWayPath);

    CreateWayPath();

    m_currentDestination = DirectX::SimpleMath::Vector3::Zero;
}

void NpcAI::InitializeDestinationTargets()
{
    m_destinationTargets.currentTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.seekTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.wanderTarget = DirectX::SimpleMath::Vector3::Zero;

    m_destinationTargets.wanderDistance = 100.0f;
    m_destinationTargets.wanderJitter = 0.001f;
    m_destinationTargets.wanderRadius = 10.0f;
}

void NpcAI::UpdateAI(const float aTimeStep)
{
    //m_currentDestination = m_playerVehicle->GetPos();

    if ((m_npcOwner->GetPos() - m_currentWaypoint.waypointPos).Length() < m_currentWaypoint.waypointRadius)
    {
        Utility::IncrementWayPath(m_currentWayPath);
    }
    m_currentWaypoint = Utility::GetWaypointFromPath(m_currentWayPath);
    m_currentDestination = m_currentWaypoint.waypointPos;

    m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
}

DirectX::SimpleMath::Vector3 NpcAI::Wander()
{
    float xWander = Utility::RandomClamped() * m_destinationTargets.wanderJitter;
    float zWander = Utility::RandomClamped() * m_destinationTargets.wanderJitter; 
    m_destinationTargets.wanderTarget == DirectX::SimpleMath::Vector3(xWander, m_npcOwner->GetHeight(), zWander);
    m_destinationTargets.wanderTarget.Normalize();

    m_destinationTargets.wanderTarget *= m_destinationTargets.wanderRadius;
    DirectX::SimpleMath::Vector3 localTarget = m_destinationTargets.wanderTarget + DirectX::SimpleMath::Vector3(m_destinationTargets.wanderDistance, 0.0f, 0.0f);
    
    DirectX::SimpleMath::Matrix worldMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), m_npcOwner->GetForward(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Vector3 worldTarget = DirectX::SimpleMath::Vector3::Transform(localTarget, worldMat);

    return worldTarget - m_npcOwner->GetPos();
}