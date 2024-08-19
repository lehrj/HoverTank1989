#include "pch.h"
#include "NpcAI.h"
#include "NPCVehicle.h"
#include "Vehicle.h"


NpcAI::NpcAI(const NPCVehicle* aOwner)
{
    m_npcOwner = aOwner;
}

void NpcAI::AdjustHeadingForVelocity()
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination;
    DirectX::SimpleMath::Vector3 pos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 velocity = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vecToDest = dest - pos;
    float distanceToDest = vecToDest.Length();
    DirectX::SimpleMath::Vector3 velocityNorm = velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 headingNorm = vecToDest;
    headingNorm.Normalize();
    float velocityNormDotHeadingNorm = headingNorm.Dot(velocityNorm);
    float lookAheadTime = distanceToDest / (velocity.Length() + 50.0f);

    DirectX::SimpleMath::Vector3 adjustedDest = m_currentDestination;
    const float adjustmentMagnitude = 15.0f;
    adjustedDest -= velocityNorm * (1.0f + lookAheadTime) * (1.0f + (1.0f - velocityNormDotHeadingNorm)) * adjustmentMagnitude;
    m_currentDestination = adjustedDest;
}

void NpcAI::AvoidPosCurrent()
{
    if (m_isAvoidanceImpactChanceHigh == true && m_avoidanceTargetNpc->GetIsJumpActive() == false && m_npcOwner->GetIsJumpReady() == true)
    {
        m_isJumpTriggered = true;
    }
    else
    {
        DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
        DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

        DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
        DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();

        const float distanceBetween = (selfPos - targetPos).Length();
        DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
        const float velocityMag = (selfVelocity - targetVelocity).Length();
        const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
        const float lookAheadTime = impactTime;
        DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
        DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);

        DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
        DirectX::SimpleMath::Vector3 testAvoidanceVec = vecToTarget;
        testAvoidanceVec.Normalize();
  
        float distanceRatio = vecToTarget.Length();
        distanceRatio /= m_avoidanceBoxLength;
        const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
        const float frontOffset = 7.0f;

        float testRadius = m_avoidanceRadius;
        float distanceBetweenRatio = distanceBetween / testRadius;
        float distanceBetweenRatioInv = abs(1.0f - distanceBetweenRatio);
        const float avoidanceMag = 500.0f;
        testAvoidanceVec *= (1.0f + (distanceBetweenRatioInv * avoidanceMag));
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }
}

void NpcAI::CreateWayPath()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 85.0f;

    DirectX::SimpleMath::Vector3 pos;
    Utility::Waypoint wp;

    const float low = -10.0f;
    const float high = 10.0f;
    const float xOffSetMod = 300.0f;
    float xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;
    float zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    Utility::Waypoint wp1;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp1 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp2;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp2 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp3;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp3 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp4;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp4 = Utility::CreateWaypoint(pos, radius + 15.0f);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp5;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp5 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp6;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp6 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp7;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp7 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp8;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp8 = Utility::CreateWaypoint(pos, radius);

    Utility::PushWaypointToPath(m_currentWayPath, wp7);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);
    Utility::PushWaypointToPath(m_currentWayPath, wp4);
    Utility::PushWaypointToPath(m_currentWayPath, wp5);
    Utility::PushWaypointToPath(m_currentWayPath, wp6);
    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp3);
    Utility::PushWaypointToPath(m_currentWayPath, wp8);
 
    m_currentWayPath.targetNode = 0;
}

void NpcAI::CreateWayPathAlt()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 85.0f;

    DirectX::SimpleMath::Vector3 pos;
    Utility::Waypoint wp;

    const float low = -10.0f;
    const float high = 10.0f;
    const float xOffSetMod = 300.0f;
    float xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;
    float zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    Utility::Waypoint wp1;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp1 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp2;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp2 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp3;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp3 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp4;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp4 = Utility::CreateWaypoint(pos, radius + 15.0f);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp5;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp5 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp6;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp6 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp7;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp7 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    xOffset += xOffSetMod;

    Utility::Waypoint wp8;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp8 = Utility::CreateWaypoint(pos, radius);

    Utility::PushWaypointToPath(m_currentWayPath, wp6);
    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp3);
    Utility::PushWaypointToPath(m_currentWayPath, wp8);
    Utility::PushWaypointToPath(m_currentWayPath, wp7);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);
    Utility::PushWaypointToPath(m_currentWayPath, wp4);
    Utility::PushWaypointToPath(m_currentWayPath, wp5);

    m_currentWayPath.targetNode = 0;
}

void NpcAI::CreateWayPathTestFireRange()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 85.0f;

    DirectX::SimpleMath::Vector3 pos;
    Utility::Waypoint wp;

    pos = m_npcOwner->GetPos();
    wp = Utility::CreateWaypoint(pos, radius);

    Utility::PushWaypointToPath(m_currentWayPath, wp);
    m_currentWayPath.targetNode = 0;
}

void NpcAI::CreateWayPathTestMissileRange()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 85.0f;

    Utility::Waypoint wp1;
    DirectX::SimpleMath::Vector3 pos = m_missileRangeWP1;
    wp1 = Utility::CreateWaypoint(pos, radius);

    Utility::Waypoint wp2;
    pos = m_missileRangeWP2;
    wp2 = Utility::CreateWaypoint(pos, radius);

    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);
    
    m_currentWayPath.targetNode = 0;
}

void NpcAI::DebugToggle()
{
    if (m_debugToggle == true)
    {
        m_debugToggle = false;
    }
    else
    {
        m_debugToggle = true;
    }  
}

float NpcAI::GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest)
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination - m_npcOwner->GetPos();
    dest = aDest;

    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = m_npcOwner->GetForward();
    const DirectX::SimpleMath::Vector3 two = dest;
    const DirectX::SimpleMath::Vector3 up = m_npcOwner->GetUp();
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    const float det = one.x * two.y * up.z + two.x * up.y * one.z + up.x * one.y * two.z - one.z * two.y * up.x - two.z * up.y * one.x - up.z * one.y * two.x;
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

    if (m_behavior.none == true)
    {
        //return 0.0f;
    }
    else if (isFacingDest == false)
    {
        //return 0.0f;
    }
    else
    {
        //return 1.0f;
    }
    return m_aiControls.aiOutput.forwardThrust;
}

void NpcAI::InitializeAI(Environment const* aEnvironment, std::shared_ptr<Vehicle> aPlayer, std::shared_ptr<DebugData> aDebugPtr, std::shared_ptr<NPCController> aNpcController, const bool aUseAltAiTrue)
{
    m_debugData = aDebugPtr;
    m_environment = aEnvironment;
    m_playerVehicle = aPlayer;
    m_npcController = aNpcController;
    
    InitializeControlOutput();
    InitializeBehavior();
    InitializeDestinationTargets();
    m_currentWaypoint.waypointPos = DirectX::SimpleMath::Vector3::Zero;
    m_currentWaypoint.waypointRadius = 1.0f;

    m_desiredHeading = DirectX::SimpleMath::Vector3::Zero;
    m_desiredVelocity = 0.0f;

    Utility::ClearWayPath(m_currentWayPath);

    if (aUseAltAiTrue == false)
    {
        CreateWayPath();
    }
    else
    {
        CreateWayPathAlt();
    }
    
    m_currentDestination = DirectX::SimpleMath::Vector3::Zero;

    m_destinationSmoothing.clear();
    for (int i = 0; i < m_destinationSmoothingSize; ++i)
    {
        m_destinationSmoothing.push_back(m_currentDestination);
    }

    m_avoidanceTargetPos = DirectX::SimpleMath::Vector3::Zero;
    m_isAvoidanceTrue = false;
    UpdateAvoidanceBox();
}

void NpcAI::InitializeControlOutput()
{
    m_aiControls.aiOutput.angleToDestination = 0.0f;
    m_aiControls.aiOutput.steeringAngle = 0.0f;
    m_aiControls.aiOutput.steeringDirection = DirectX::SimpleMath::Vector3::Zero;
    m_aiControls.aiOutput.forwardThrust = 0.0f;
    m_aiControls.aiOutput.omniThrust = 0.0f;
    m_aiControls.aiOutput.omniDirection = DirectX::SimpleMath::Vector3::Zero;
}

void NpcAI::InitializeBehavior()
{
    m_behavior.arrive = false;
    m_behavior.followWayPath = false;
    m_behavior.none = false;
    m_behavior.seek = false;
    m_behavior.separation = false;
    m_behavior.stop = true;
    m_behavior.wallAvoidance = false;
    m_behavior.wander = false;
}

void NpcAI::InitializeDestinationTargets()
{
    m_destinationTargets.separationTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.currentTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.seekTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.wanderTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.separationMagnitude = 15.0f;
    m_destinationTargets.separationRadius = 15.0f;
    m_destinationTargets.wanderDistance = 100.0f;
    m_destinationTargets.wanderJitter = 0.001f;
    m_destinationTargets.wanderRadius = 10.0f;
}

void NpcAI::PushAiAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    bool isPushTarget = true;
    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVelocity = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 targetWorldPos = aVehicle->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = aVehicle->GetVelocity();

    DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_avoidanceBoxBase, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeMat = localizeMat.Invert();
    DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeDirMat = localizeDirMat.Invert();

    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVelocity = DirectX::SimpleMath::Vector3::Transform(selfWorldVelocity, localizeDirMat);
    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeDirMat);

    DirectX::SimpleMath::Vector3 selfLocalVelocityNorm = selfLocalVelocity;
    selfLocalVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 targetLocalVelocityNorm = targetLocalVelocity;
    targetLocalVelocityNorm.Normalize();

    bool isCurrentTargetImpactChanceHigh = false;
    bool isPrevTargetImpactChanceHigh = m_isAvoidanceImpactChanceHigh;

    if (targetLocalVelocity.x >= selfLocalVelocity.x)
    {
        m_isAvoidanceImpactChanceLow = true;
        isPushTarget = false;
    }
    if (selfLocalVelocityNorm.Dot(targetLocalVelocityNorm) < 0.0f && targetLocalVelocity.x <= selfLocalVelocity.x)
    {
        isPushTarget = true;
        if ((targetLocalPos.z <= 0.0f && targetLocalVelocity.z >= 0.0f) || (targetLocalPos.z >= 0.0f && targetLocalVelocity.z <= 0.0f))
        {
            m_isAvoidanceImpactChanceHigh = true;
            isCurrentTargetImpactChanceHigh = true;
        }
    }

    if (isPushTarget == true)
    {
        if (m_isAvoidanceTrue == false)
        {
            m_isAvoidanceTrue = true;
            m_avoidanceTargetPos = aAvoidancePos;
            m_avoidanceTargetNpc = aVehicle;
        }
        else
        {
            const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTargetPos).Length();
            const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();
            if (isCurrentTargetImpactChanceHigh == true && isPrevTargetImpactChanceHigh == true)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (isCurrentTargetImpactChanceHigh == true && isPrevTargetImpactChanceHigh == false)
            {
                m_avoidanceTargetPos = aAvoidancePos;
                m_avoidanceTargetNpc = aVehicle;
            }
            else if (distanceToNewTarget < distanceToPriorTarget)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
        }
    }
}

void NpcAI::SetForwardThrustOutput()
{
    DirectX::SimpleMath::Vector3 directionToDest = m_currentDestination - m_npcOwner->GetPos();
    directionToDest.Normalize();
    bool isFacingDest;
    if (directionToDest.Cross(m_npcOwner->GetRight()).y >= 0.0f)
    {
        isFacingDest = false;
    }
    else
    {
        isFacingDest = true;
    }
    float forwardThrustVal;
    if (m_behavior.none == true)
    {
        forwardThrustVal = 0.0f;
    }
    else if (isFacingDest == true)
    {
        if (m_isAvoidanceTrue == true && m_isAvoidanceImpactChanceHigh == true)
        {
            forwardThrustVal = directionToDest.Dot(m_npcOwner->GetForward());
            forwardThrustVal *= m_aiControls.throttleOutputMax;
        }
        else
        {
            forwardThrustVal = directionToDest.Dot(m_npcOwner->GetForward());
            forwardThrustVal *= m_aiControls.throttleOutputMax;
        }
    }
    else
    {
        if (m_isAvoidanceTrue == true && m_isAvoidanceImpactChanceHigh == true)
        {
            forwardThrustVal = m_aiControls.throttleOutputMin;
        }
        else
        {
            forwardThrustVal = -directionToDest.Dot(-m_npcOwner->GetForward());
            forwardThrustVal *= -m_aiControls.throttleOutputMin;
        }
    }
    m_aiControls.aiOutput.forwardThrust = forwardThrustVal;
}

void NpcAI::SetOmniOutput()
{
    DirectX::SimpleMath::Vector3 omniThrustDirection = m_npcOwner->GetForward();
    omniThrustDirection = DirectX::SimpleMath::Vector3::Transform(omniThrustDirection, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_npcOwner->GetUp(), m_aiControls.aiOutput.angleToDestination));
    omniThrustDirection.Normalize();
    m_aiControls.aiOutput.omniDirection = omniThrustDirection;

    // Temp throttle settings till AI is built out to control thrust based on distance to destination
    const float distanceToDest = (m_npcOwner->GetPos() - m_currentDestination).Length();

    const float slowDownRange = 50.0f;
    if (distanceToDest > slowDownRange)
    {
        m_aiControls.aiOutput.omniThrust = 1.0f;
    }
    else
    {
        m_aiControls.aiOutput.omniThrust = distanceToDest / slowDownRange;
    }
}

void NpcAI::SetSteeringOutput()
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination - m_npcOwner->GetPos();
    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = m_npcOwner->GetForward();
    const DirectX::SimpleMath::Vector3 two = dest;
    const DirectX::SimpleMath::Vector3 up = m_npcOwner->GetUp();
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    //const float det = one.x * two.y * aUp.z + two.x * aUp.y * one.z + aUp.x * one.y * two.z - one.z * two.y * aUp.x - two.z * aUp.y * one.x - aUp.z * one.y * two.x;
    const float det = one.x * two.y * up.z + two.x * up.y * one.z + up.x * one.y * two.z - one.z * two.y * up.x - two.z * up.y * one.x - up.z * one.y * two.x;
    const float angle = atan2(det, dot);

    float steeringAngle;
    if (angle > m_npcOwner->GetMaxSteeringAngle())
    {
        steeringAngle = m_npcOwner->GetMaxSteeringAngle();
    }
    else if (angle < -m_npcOwner->GetMaxSteeringAngle())
    {
        steeringAngle = -m_npcOwner->GetMaxSteeringAngle();
    }
    else
    {
        steeringAngle = angle;
    }
    m_aiControls.aiOutput.steeringAngle = steeringAngle;
    DirectX::SimpleMath::Vector3 vecToDestination = m_currentDestination - m_npcOwner->GetPos();
    vecToDestination.Normalize();
    m_aiControls.aiOutput.steeringDirection = vecToDestination;
    m_aiControls.aiOutput.angleToDestination = angle;
}

void NpcAI::StopVehicleVelocity()
{
    const DirectX::SimpleMath::Vector3 currentVelocity = m_npcOwner->GetVelocity();
    const float speed = currentVelocity.Length();
    const DirectX::SimpleMath::Vector3 antiVelocity = currentVelocity * -1.0f;
    m_currentDestination = antiVelocity + m_npcOwner->GetPos();
    const float distanceToWaypoint = DirectX::SimpleMath::Vector3::Distance(m_currentDestination, m_npcOwner->GetPos());
    if (distanceToWaypoint < 5.0f && speed < 5.0f)
    {
        m_currentDestination = m_npcOwner->GetPos();
    }
}

void NpcAI::UpdateAI(const float aTimeStep)
{
    UpdateAvoidanceBox();
    if ((m_npcOwner->GetPos() - m_currentWaypoint.waypointPos).Length() < m_currentWaypoint.waypointRadius)
    {
        Utility::IncrementWayPath(m_currentWayPath);
        ++m_lapCounter;
    }

    m_currentWaypoint = Utility::GetWaypointFromPath(m_currentWayPath);
    m_currentDestination = m_currentWaypoint.waypointPos;

    AdjustHeadingForVelocity();

    const float distanceToWaypoint = DirectX::SimpleMath::Vector3::Distance(m_currentWaypoint.waypointPos, m_npcOwner->GetPos());
    if (distanceToWaypoint < m_currentWaypoint.waypointRadius)
    {
        StopVehicleVelocity();
    }

    if (m_isAvoidanceTrue == true && m_isJumpTriggered == false)
    {
        AvoidPosCurrent();
    }
    else
    {
        UpdateSeparation();
        if (m_destinationTargets.isSeparationTargetInRadius == true)
        {
            m_currentDestination = m_destinationTargets.separationTarget;
        }
    }

    UpdateDestinationSmoothing();

    if (m_debugToggle == true)
    {
        UpdateControlOutput();
    }

    m_emergencyToggle = m_isAvoidanceImpactChanceHigh;
    m_isJumpTriggered = false;
    m_destinationTargets.isSeparationTargetInRadius = false;
    m_isAvoidanceTrue = false;
    m_isAvoidanceImpactChanceHigh = false;
    m_isAvoidanceImpactChanceLow = false;
    m_avoidanceTargetNpc = nullptr;
}

void NpcAI::UpdateAvoidanceBox()
{
    const DirectX::SimpleMath::Vector3 vehicleDimensions = m_npcOwner->GetDimensions();
    const float avoidanceWidthMod = 1.0f;
    const float avoidanceWidthMin = vehicleDimensions.z * avoidanceWidthMod;

    const float avoidanceWidth = avoidanceWidthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * avoidanceWidthMin;;
    m_avoidanceBoxWidth = avoidanceWidth;
    const float avoidanceHeight = vehicleDimensions.y * 0.5f;
    float avoidanceLength = m_avoidanceBoxLengthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * m_avoidanceBoxLengthMin;
    m_avoidanceBoxLength = avoidanceLength;
    m_avoidanceBox.Extents = DirectX::SimpleMath::Vector3(avoidanceLength, avoidanceHeight, avoidanceWidth);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3(vehicleDimensions.x + avoidanceLength, 0.0f, 0.0f);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 1.0f), -avoidanceHeight * 0.0f, 0.0f);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 0.5f), -avoidanceHeight * 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3 velocityForward = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vehicleForward = m_npcOwner->GetForward();
    DirectX::SimpleMath::Vector3  destination = m_currentDestination - m_npcOwner->GetPos();
    destination.Normalize();
    vehicleForward.Normalize();
    velocityForward.Normalize();

    velocityForward = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 velocityUp = m_npcOwner->GetUp();
    DirectX::SimpleMath::Vector3 velocityRight = velocityForward.Cross(velocityUp);
    velocityUp = velocityForward.Cross(-velocityRight);

    m_avoidanceBoxRight = velocityRight;
    m_avoidanceBoxRight.Normalize();
    m_avoidaneBoxUp = velocityUp;
    m_avoidaneBoxUp.Normalize();

    DirectX::SimpleMath::Matrix velocityAlignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -velocityRight, velocityUp);
    m_avoidanceBox.Center.x = avoidanceLength + vehicleDimensions.x * 0.5f;
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, velocityAlignment);
    m_avoidanceBox.Center = m_avoidanceBox.Center + m_npcOwner->GetPos();

    m_avoidanceBoxBase = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 0.5f), 0.0f, 0.0f);
    m_avoidanceBoxBase = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBoxBase, velocityAlignment);
    m_avoidanceBoxBase = m_avoidanceBoxBase + m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 testPos = m_avoidanceBox.Center;
    m_avoidanceAlignment = velocityAlignment;
    DirectX::SimpleMath::Quaternion velocityAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(velocityAlignment);
    m_avoidanceBox.Orientation = velocityAlignmentQuat;

    DirectX::SimpleMath::Vector3 testPos2 = m_npcOwner->GetPos();
    m_avoidanceRadius = (avoidanceLength + avoidanceLength + (m_avoidanceBoxWidth * 0.5f) + (vehicleDimensions.x * 0.5f));
}

void NpcAI::UpdateControlOutput()
{
    SetForwardThrustOutput();
    SetOmniOutput();
    SetSteeringOutput();
    if (m_isJumpTriggered == true)
    {
        m_aiControls.aiOutput.isTriggerJumpTrue = true;
    }
    else
    {
        m_aiControls.aiOutput.isTriggerJumpTrue = false;
    }
}

void NpcAI::UpdateDestinationSmoothing()
{
    std::vector<DirectX::SimpleMath::Vector3> prevSmoothingVec;
    prevSmoothingVec.clear();
    prevSmoothingVec.resize(m_destinationSmoothingSize);
    prevSmoothingVec = m_destinationSmoothing;
    m_destinationSmoothing[0] = m_currentDestination;
    for (int i = 1; i < m_destinationSmoothingSize - 1; ++i)
    {
        m_destinationSmoothing[i] = prevSmoothingVec[i - 1];
    }

    DirectX::SimpleMath::Vector3 smoothedDest = m_currentDestination;
    for (int i = 0; i < m_destinationSmoothingSize; ++i)
    {
        smoothedDest += m_destinationSmoothing[i];
    }
    m_currentDestination = smoothedDest / static_cast<float>(m_destinationSmoothingSize);
}

void NpcAI::UpdateDesiredHeading()
{
    DirectX::SimpleMath::Vector3 updateHeading = DirectX::SimpleMath::Vector3::Zero;
    float updateVelocity = 0.0f;
    if (m_behavior.followWayPath == true)
    {
        DirectX::SimpleMath::Vector3 headingToWaypoint = m_currentWaypoint.waypointPos - m_npcOwner->GetPos();
        headingToWaypoint.Normalize();
        headingToWaypoint *= m_behavior.wayPathWeight;
        updateHeading += headingToWaypoint;
    }
    if (m_behavior.wander == true)
    {
        DirectX::SimpleMath::Vector3 headingToWander = m_destinationTargets.wanderTarget - m_npcOwner->GetPos();
        headingToWander.Normalize();
        headingToWander *= m_behavior.wanderWeight;
        updateHeading += headingToWander;
    }
    if (m_behavior.stop == true)
    {
        DirectX::SimpleMath::Vector3 reverseHeading = m_npcOwner->GetVelocity() * -1.0f;
        reverseHeading.y = 0.0f;
        reverseHeading.Normalize();
        reverseHeading *= m_behavior.stopWeight;
        updateHeading += reverseHeading;
    }
    if (m_behavior.none == false)
    {
        updateHeading.Normalize();
        m_desiredHeading = updateHeading;
        m_desiredVelocity = updateVelocity;
    }
    else if (m_behavior.none == true)
    {
        m_desiredHeading = DirectX::SimpleMath::Vector3::Zero;
        m_desiredVelocity = 0.0f;
    }
}

void NpcAI::UpdateSeparation()
{
    std::vector<DirectX::SimpleMath::Vector3> npcPos = m_npcController->GetVecOfNpcPos(m_npcOwner->GetID());
    std::vector<DirectX::SimpleMath::Vector3> inDistanceVec;

    for (int i = 0; i < npcPos.size(); ++i)
    {
        float distance = DirectX::SimpleMath::Vector3::Distance(npcPos[i], m_npcOwner->GetPos());

        if (distance <= m_destinationTargets.separationRadius)
        {
            inDistanceVec.push_back(npcPos[i]);
        }
    }

    if (inDistanceVec.size() == 0)
    {
        m_destinationTargets.isSeparationTargetInRadius = false;
        m_destinationTargets.separationTarget = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        m_destinationTargets.isSeparationTargetInRadius = true;
        DirectX::SimpleMath::Vector3 testPos = DirectX::SimpleMath::Vector3::Zero;
        for (int i = 0; i < inDistanceVec.size(); ++i)
        {
            DirectX::SimpleMath::Vector3 vecToTarg = inDistanceVec[i] - m_npcOwner->GetPos();
            float distance = vecToTarg.Length();
            vecToTarg.Normalize();

            float ratio = distance / m_destinationTargets.separationRadius;
            ratio = 1.0f - ratio;
            testPos += vecToTarg * ratio;
        }
        testPos /= static_cast<float>(inDistanceVec.size());
        testPos *= -1.0f;
        testPos *= m_destinationTargets.separationMagnitude;
        testPos += m_npcOwner->GetPos();
        m_destinationTargets.separationTarget = testPos;
    }
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