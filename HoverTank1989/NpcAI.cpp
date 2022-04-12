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