#include "pch.h"
#include "NpcAI.h"

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

float NpcAI::GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest)
{
    DirectX::SimpleMath::Vector3 dest = aDest - aPos;
    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = aForward;
    const DirectX::SimpleMath::Vector3 two = dest;
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    const float det = one.x * two.y * aUp.z + two.x * aUp.y * one.z + aUp.x * one.y * two.z - one.z * two.y * aUp.x - two.z * aUp.y * one.x - aUp.z * one.y * two.x;
    const float angle = atan2(det, dot);

    return angle;
}

void NpcAI::InitializeAI(Environment const* aEnvironment)
{
    m_environment = aEnvironment;
    m_currentWaypoint.waypointPos = DirectX::SimpleMath::Vector3::Zero;
    m_currentWaypoint.waypointRadius = 1.0f;
    Utility::ClearWayPath(m_currentWayPath);
}

void NpcAI::UpdateAI(const float aTimeStep)
{


}