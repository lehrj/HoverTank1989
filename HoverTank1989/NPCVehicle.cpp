#include "pch.h"
#include "NPCVehicle.h"
#include "NPCController.h"

NPCVehicle::NPCVehicle()
{
    m_npcAI = std::make_unique<NpcAI>(this);
}

void NPCVehicle::ActivateAi()
{
    m_npcAI->ActivateAI();
}

bool NPCVehicle::ActivateJump()
{
    if (m_vehicleStruct00.vehicleData.jumpData.isJumpReady == true)
    {
        m_testMaxAlt = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.isJumpActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isJumpReady = false;
        m_vehicleStruct00.vehicleData.jumpData.jumpActiveTimer = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.isLaunchImpulseBurnActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive = true;

        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentTime = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.directionNorm = m_vehicleStruct00.vehicleData.up;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.isActive = true;

        float desiredAlt = m_jumpHeightTarget + m_vehicleStruct00.vehicleData.terrainHightAtPos;
        const float distanceToDesiredAlt = desiredAlt - m_vehicleStruct00.vehicleData.q.position.y;
        const float gravity = m_environment->GetGravity();
        const float initialVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;

 
        float burnTime = m_vehicleStruct00.vehicleData.jumpData.impulseBurnTimeTotal;
        float accel = 2.0f * (distanceToDesiredAlt - initialVelocity * (burnTime)) / (burnTime * burnTime);
        //////////////////
        //const float distanceToHoverRange = m_vehicleStruct00.vehicleData.q.position.y - m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper;
        //const float distanceToHoverRange = m_vehicleStruct00.vehicleData.q.position.y - (m_vehicleStruct00.vehicleData.terrainHightAtPos + m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper);
        const float distanceToHoverRange = desiredAlt;
        //const float gravity = m_environment->GetGravity();
        //const float initialVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;

        float quad = (-initialVelocity + sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
        if (quad <= 0.0f)
        {
            quad = (-initialVelocity - sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
        }
        if (quad <= 0.0f)
        {
            quad = 0.0001f;
        }

        float deceleration = (0.0f - initialVelocity) / quad;
        float decelerationForce = deceleration * m_vehicleStruct00.vehicleData.mass;
        //const float decelForceLimit = 26000.0f;
        const float decelForceLimit = m_jumpDeccelForceLimit;

        accel = 22.0f;
        //////////////////
        m_vehicleStruct00.vehicleData.jumpData.isLaunchImpulseBurnActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive = true;

        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentTime = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.directionNorm = -m_vehicleStruct00.vehicleData.up;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.isActive = true;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.maxMagnitude = (accel) * 2.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.totalTime = burnTime;
        return true;
    }
    else
    {
        return false;
    }
}


bool NPCVehicle::ActivateJumpOldFig8()
{
    if (m_vehicleStruct00.vehicleData.jumpData.isJumpReady == true)
    {
        m_testMaxAlt = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.isJumpActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isJumpReady = false;
        m_vehicleStruct00.vehicleData.jumpData.jumpActiveTimer = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.isLaunchImpulseBurnActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive = true;

        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentTime = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.directionNorm = m_vehicleStruct00.vehicleData.up;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.isActive = true;

        float desiredAlt = 42.0f;
        const float distanceToDesiredAlt = desiredAlt - m_vehicleStruct00.vehicleData.q.position.y;
        const float gravity = m_environment->GetGravity();
        const float initialVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;

        float burnTime = 1.5f;
        float accel = 2.0f * (distanceToDesiredAlt - initialVelocity * (burnTime)) / (burnTime * burnTime);

        m_vehicleStruct00.vehicleData.jumpData.isLaunchImpulseBurnActive = true;
        m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive = true;

        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentTime = 0.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.directionNorm = -m_vehicleStruct00.vehicleData.up;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.isActive = true;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.maxMagnitude = (accel) * 2.0f;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
        m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.totalTime = burnTime;
        return true;
    }
    else
    {
        return false;
    }
}

void NPCVehicle::ActivateJumpLanding()
{
    float altitude = m_vehicleStruct00.vehicleData.q.position.y;
    float verticalVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;
    float gravity = m_environment->GetGravity();

    float distanceToCutOff = altitude - m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper;

    float halfGravity = gravity * 0.5f;

    float rawTimeToDistance = (-verticalVelocity - sqrt((verticalVelocity * verticalVelocity) - (4.0f * halfGravity * distanceToCutOff))) / (2.0f * halfGravity);

    float burnTime = rawTimeToDistance * 2.0f;
    float burnMagMax = (-gravity * 1.5f);

    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude = 0.0f;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentTime = 0.0f;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.directionNorm = -m_vehicleStruct00.vehicleData.up;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.isActive = true;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.maxMagnitude = burnMagMax;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
    m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.totalTime = burnTime;

    Utility::ImpulseForce testLaunchImpulse = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce;
    Utility::ImpulseForce testLandImpulse = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce;
}

void NPCVehicle::ActivateNPC()
{
    m_vehicleStruct00.vehicleData.isActivated = true;
}

DirectX::SimpleMath::Vector3 NPCVehicle::CalculateDragAngular(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    DirectX::SimpleMath::Matrix inverseAlignment = m_vehicleStruct00.vehicleData.alignment;
    inverseAlignment = inverseAlignment.Invert();
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, inverseAlignment);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;

    const float length = m_vehicleStruct00.vehicleData.dimensions.x;
    const float width = m_vehicleStruct00.vehicleData.dimensions.z;
    const float height = m_vehicleStruct00.vehicleData.dimensions.y;

    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    //float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));
    float rollSurface = (length * abs(rollDot)) + (width * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    //m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    //m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    float angVelocityF = aAngVelocity.Length();
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float angFrontSurfaceArea = airSurfaceArea;
    float radius = 4.0f;
    angFrontSurfaceArea = 70.0f;
    //radius = radiusSum;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));

    //m_debugData->DebugPushUILineDecimalNumber("angularDrag.L = ", angularDrag.Length(), "");
    return angularDrag;
}

DirectX::SimpleMath::Vector3 NPCVehicle::CalculateDragAngular2(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    //DirectX::SimpleMath::Vector3 angVelocityNorm = m_vehicleStruct00.vehicleData.q.angularVelocityVec;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    DirectX::SimpleMath::Matrix inverseAlignment = m_vehicleStruct00.vehicleData.alignment;
    inverseAlignment = inverseAlignment.Invert();
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, inverseAlignment);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;

    const float length = m_vehicleStruct00.vehicleData.dimensions.x;
    const float width = m_vehicleStruct00.vehicleData.dimensions.z;
    const float height = m_vehicleStruct00.vehicleData.dimensions.y;

    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    //float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));
    float rollSurface = (length * abs(rollDot)) + (width * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    //m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    //m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    //float angVelocityF = aAngVelocity.Length();
    float angVelocityF = m_vehicleStruct00.vehicleData.q.angularVelocity.Length();
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float angFrontSurfaceArea = airSurfaceArea;
    float radius = 4.0f;
    angFrontSurfaceArea = airSurfaceArea;
    radius = radiusSum;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));

    //m_debugData->DebugPushUILineDecimalNumber("angularDrag.L = ", angularDrag.Length(), "");
    return angularDrag;
}

DirectX::SimpleMath::Vector3 NPCVehicle::CalculateDragAngular3(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    //DirectX::SimpleMath::Vector3 angVelocityNorm = m_vehicleStruct00.vehicleData.q.angularVelocityVec;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    DirectX::SimpleMath::Matrix inverseAlignment = m_vehicleStruct00.vehicleData.alignment;
    inverseAlignment = inverseAlignment.Invert();
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, inverseAlignment);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;

    const float length = m_vehicleStruct00.vehicleData.dimensions.x;
    const float width = m_vehicleStruct00.vehicleData.dimensions.z;
    const float height = m_vehicleStruct00.vehicleData.dimensions.y;

    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    //float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));
    float rollSurface = (length * abs(rollDot)) + (width * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    //m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    //m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    //float angVelocityF = aAngVelocity.Length();
    float angVelocityF = m_vehicleStruct00.vehicleData.q.angularVelocity.Length();
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float angFrontSurfaceArea = airSurfaceArea;
    float radius = 4.0f;
    angFrontSurfaceArea = airSurfaceArea;
    radius = radiusSum;

    DirectX::SimpleMath::Vector3 angVel = m_vehicleStruct00.vehicleData.q.angularVelocity;
    //DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));
    DirectX::SimpleMath::Vector3 angularDrag = (((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVel * angVel) * angFrontSurfaceArea * angAirDensity)));

    //m_debugData->DebugPushUILineDecimalNumber("angularDrag.L = ", angularDrag.Length(), "");
    return angularDrag;
}

DirectX::SimpleMath::Vector3 NPCVehicle::CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_vehicleStruct00.vehicleData.alignmentInverse);
    //angVelocityNorm = localAngVelocityNorm;

    const float length = m_vehicleStruct00.vehicleData.tensorDimensions.x;
    const float width = m_vehicleStruct00.vehicleData.tensorDimensions.z;
    const float height = m_vehicleStruct00.vehicleData.tensorDimensions.y;

    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    float rollSurface = (length * abs(rollDot)) + (width * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    float angVelocityF = aAngVelocity.Length();
    float angDragCoefficient = m_angDragCoefficient;
    float angAirDensity = m_environment->GetAirDensity();
    float angFrontSurfaceArea = airSurfaceArea;
    float radius = radiusSum;

    /*
    m_debugData->DebugPushUILineDecimalNumber("angDragCoefficient", angDragCoefficient, "");
    m_debugData->DebugPushUILineDecimalNumber("radius", radius, "");
    m_debugData->DebugPushUILineDecimalNumber("angVelocityF", angVelocityF, "");
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("angAirDensity", angAirDensity, "");
    m_debugData->DebugPushUILineDecimalNumber("angVelocityNorm", angVelocityNorm.Length(), "");
    */
    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));

    DirectX::SimpleMath::Vector3 angularDragNorm = angularDrag;
    angularDragNorm.Normalize();
    DirectX::SimpleMath::Vector3 aAngVelocityNorm = aAngVelocity;
    aAngVelocityNorm.Normalize();

    //angularDrag *= aTimeStep;
    float angularDragLength = angularDrag.Length();
    DirectX::SimpleMath::Vector3 avgVel = aAngVelocity;
    DirectX::SimpleMath::Vector3 testDrag = aAngVelocity * -0.2f;
    if (m_vehicleStruct00.vehicleData.impulseForceVec.size() > 0)
    {
        int testBreak = 0;
        ++testBreak;
    }

    //m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, angularDrag, 20.0f, 0.0f, DirectX::Colors::Red);
    //angularDrag = aAngVelocity * -0.2f;
    angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);

    return angularDrag;
}

void NPCVehicle::CalculateImpactForce(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    const float mass1 = aImpactForce.impactMass;
    const float mass2 = m_vehicleStruct00.vehicleData.mass;
    const float coefficientOfRestitution = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - coefficientOfRestitution * mass2) * vx1 * tmp +
        (1.0f + coefficientOfRestitution) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0f + coefficientOfRestitution) * mass1 * vx1 * tmp +
        (mass2 - coefficientOfRestitution * mass1) * vx2 * tmp;

    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;

    // test calc kinetic energy
    DirectX::SimpleMath::Vector3 kE = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;
    float kEVal = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity.Length() * aImpactForce.impactVelocity.Length();
    // end kinetic energy test

    DirectX::SimpleMath::Vector3 testV = aImpactForce.impactVelocity;
    testV.Normalize();
    testV *= 4000.0f;
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;
    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;
}

void NPCVehicle::CalculateImpactForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    const float mass1 = aImpactForce.impactMass;
    const float mass2 = m_vehicleStruct00.vehicleData.mass;
    const float coefficientOfRestitution = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    //DirectX::SimpleMath::Vector3 vx1 = aVehicleHit.q.velocity;
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - coefficientOfRestitution * mass2) * vx1 * tmp +
        (1.0f + coefficientOfRestitution) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0f + coefficientOfRestitution) * mass1 * vx1 * tmp +
        (mass2 - coefficientOfRestitution * mass1) * vx2 * tmp;

    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    //m_vehicleStruct00.vehicleData.impactForce.impactMass = aVehicleHit.mass;

    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    //impactForceToVec.impactMass = aVehicleHit.mass;
    impactForceToVec.impactMass = aImpactForce.impactMass;
    impactForceToVec.impactModifier = aImpactForce.impactModifier;
    impactForceToVec.kineticEnergy = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;

    // test calc kinetic energy
    /*
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    */
    // end kinetic energy test

    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    //::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;

    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    impactForceToVec.impactVelocity *= aImpactForce.impactModifier;
    PushImpactForce(impactForceToVec);

    impactTorque.magnitude *= aImpactForce.impactModifier + 1000.0f;
    PushImpactTorque(impactTorque);
}

void NPCVehicle::CalculateImpulseForce(const VehicleData& aVehicleHit, DirectX::SimpleMath::Vector3 aForceVec1, DirectX::SimpleMath::Vector3 aForceVec2, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    m_lastImpactPos = aImpactPos;

    DirectX::SimpleMath::Vector3 testVecUsed = aForceVec1;
    Utility::ImpulseForce impulseToVec;
    impulseToVec.currentMagnitude = 0.0f;
    impulseToVec.currentTime = 0.0f;
    //impulseToVec.directionNorm = aImpactForce.impactVelocity;
    impulseToVec.directionNorm = aVehicleHit.q.velocity;
    impulseToVec.directionNorm = testVecUsed;
    impulseToVec.directionNorm.Normalize();
    impulseToVec.isActive = true;
    //impulseToVec.maxMagnitude = (0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity).Length();
    impulseToVec.maxMagnitude = (0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity).Length();
    //impulseToVec.maxMagnitude *= 0.9f;
    //impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.torqueArm = aVehicleHit.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.torqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - aImpactPos;
    impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    //impulseToVec.torqueArm = DirectX::SimpleMath::Vector3::Zero;
    //impulseToVec.torqueArm = m_vehicleStruct00.vehicleData.up * 5.0f;
    //impulseToVec.torqueArm = aForceVec2;
    //impulseToVec.torqueArm.Normalize();
    float impactVelocity = (aVehicleHit.q.velocity - m_vehicleStruct00.vehicleData.q.velocity).Length();
    float impactTime = 1.0f / (impactVelocity + 0.00000000001f);
    impulseToVec.totalTime = impactTime;
    impulseToVec.totalTime = 0.1f;

    impulseToVec.maxMagnitude = aForceVec1.Length() * aVehicleHit.mass;
    impulseToVec.torqueForceNorm = impulseToVec.directionNorm;
    PushImpulseForce(impulseToVec);

    Utility::ImpulseForce repulsorToVec;
    DirectX::SimpleMath::Vector3 repulsionForce = GetRepulsionForce(aVehicleHit);
    repulsorToVec.currentMagnitude = 0.0f;
    repulsorToVec.currentTime = 0.0f;
    //repulsorToVec.directionNorm = aVehicleHit.q.velocity;
    //repulsorToVec.directionNorm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.q.position;
    //repulsorToVec.directionNorm = m_vehicleStruct00.vehicleData.q.position - aVehicleHit.q.position;
    repulsorToVec.directionNorm = repulsionForce;
    //repulsorToVec.directionNorm = aForceVec1;
    repulsorToVec.directionNorm.Normalize();
    repulsorToVec.isActive = true;
    //repulsorToVec.maxMagnitude = GetRepulsionForce(aVehicleHit).Length() * 10000.0f;
    repulsorToVec.maxMagnitude = repulsionForce.Length() * 10000.0f;
    repulsorToVec.torqueArm = DirectX::SimpleMath::Vector3::Zero;
    repulsorToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    repulsorToVec.totalTime = 0.1f;

    //PushImpulseForce(repulsorToVec);

    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;

    //repulsionForceToVec.impactVelocity *= 150.0f;
    repulsionForceToVec.impactVelocity *= 100.0f;
    repulsionForceToVec.kineticEnergy = repulsionForceToVec.impactVelocity;

    PushImpactForce(repulsionForceToVec);
}

void NPCVehicle::CalculateImpulseForceFromPlayer(const float aPlayerMass, const DirectX::SimpleMath::Vector3 aPlayerVelocity, const DirectX::SimpleMath::Vector3 aPlayerCenterOfMass, DirectX::SimpleMath::Vector3 aForceVec1, DirectX::SimpleMath::Vector3 aForceVec2, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    m_lastImpactPos = aImpactPos;

    DirectX::SimpleMath::Vector3 testVecUsed = aForceVec1;
    Utility::ImpulseForce impulseToVec;
    impulseToVec.currentMagnitude = 0.0f;
    impulseToVec.currentTime = 0.0f;
    //impulseToVec.directionNorm = aImpactForce.impactVelocity;
    impulseToVec.directionNorm = aPlayerVelocity;
    impulseToVec.directionNorm = testVecUsed;
    impulseToVec.directionNorm.Normalize();
    impulseToVec.isActive = true;
    //impulseToVec.maxMagnitude = (0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity).Length();
    impulseToVec.maxMagnitude = (0.5f * aPlayerMass * aPlayerVelocity * aPlayerVelocity).Length();
    //impulseToVec.maxMagnitude *= 0.9f;
    //impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.torqueArm = aPlayerCenterOfMass - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.torqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - aImpactPos;
    impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    //impulseToVec.torqueArm = DirectX::SimpleMath::Vector3::Zero;
    //impulseToVec.torqueArm = m_vehicleStruct00.vehicleData.up * 5.0f;
    //impulseToVec.torqueArm = aForceVec2;
    //impulseToVec.torqueArm.Normalize();
    impulseToVec.torqueForceNorm = aPlayerVelocity;
    impulseToVec.torqueForceNorm.Normalize();

    impulseToVec.torqueArm *= 1.0f;
    float impactVelocity = (aPlayerVelocity - m_vehicleStruct00.vehicleData.q.velocity).Length();
    float impactTime = 1.0f / (impactVelocity + 0.00000000001f);
    impulseToVec.totalTime = impactTime;
    impulseToVec.totalTime = 0.1f;

    impulseToVec.maxMagnitude = aForceVec1.Length() * aPlayerMass;

    PushImpulseForce(impulseToVec);

    /*
    Utility::ImpulseForce repulsorToVec;
    DirectX::SimpleMath::Vector3 repulsionForce = GetRepulsionForce(aVehicleHit);
    repulsorToVec.currentMagnitude = 0.0f;
    repulsorToVec.currentTime = 0.0f;
    //repulsorToVec.directionNorm = aVehicleHit.q.velocity;
    //repulsorToVec.directionNorm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.q.position;
    //repulsorToVec.directionNorm = m_vehicleStruct00.vehicleData.q.position - aVehicleHit.q.position;
    repulsorToVec.directionNorm = repulsionForce;
    //repulsorToVec.directionNorm = aForceVec1;
    repulsorToVec.directionNorm.Normalize();
    repulsorToVec.isActive = true;
    //repulsorToVec.maxMagnitude = GetRepulsionForce(aVehicleHit).Length() * 10000.0f;
    repulsorToVec.maxMagnitude = repulsionForce.Length() * 10000.0f;
    repulsorToVec.torqueArm = DirectX::SimpleMath::Vector3::Zero;
    repulsorToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    repulsorToVec.totalTime = 0.1f;

    //PushImpulseForce(repulsorToVec);

    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;

    repulsionForceToVec.impactVelocity *= 150.0f;
    repulsionForceToVec.kineticEnergy = repulsionForceToVec.impactVelocity;
    PushImpactForce(repulsionForceToVec);
    */
}

void NPCVehicle::CalculateImpulseForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    const float mass1 = aImpactForce.impactMass;
    const float mass2 = m_vehicleStruct00.vehicleData.mass;
    const float coefficientOfRestitution = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - coefficientOfRestitution * mass2) * vx1 * tmp +
        (1.0f + coefficientOfRestitution) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0f + coefficientOfRestitution) * mass1 * vx1 * tmp +
        (mass2 - coefficientOfRestitution * mass1) * vx2 * tmp;

    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    impactForceToVec.impactMass = aImpactForce.impactMass;
    impactForceToVec.impactModifier = aImpactForce.impactModifier;
    impactForceToVec.kineticEnergy = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;

    Utility::ImpulseForce impulseToVec;
    impulseToVec.currentMagnitude = 0.0f;
    impulseToVec.currentTime = 0.0f;
    impulseToVec.directionNorm = aImpactForce.impactVelocity;
    impulseToVec.directionNorm.Normalize();
    impulseToVec.isActive = true;
    impulseToVec.maxMagnitude = (0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity).Length();
    impulseToVec.maxMagnitude *= aImpactForce.impactModifier;
    impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    //impulseToVec.torqueArm = DirectX::SimpleMath::Vector3::Transform(impulseToVec.torqueArm, m_vehicleStruct00.vehicleData.alignmentInverseQuat);
    //impulseToVec.totalTime = 0.1f;
    impulseToVec.totalTime = 0.8f;
    PushImpulseForce(impulseToVec);

    // test calc kinetic energy
    /*
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    */
    // end kinetic energy test

    /*
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    //::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;

    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;
    impactForceToVec.impactVelocity *= aImpactForce.impactModifier;
    PushImpactForce(impactForceToVec);

    impactTorque.magnitude *= aImpactForce.impactModifier + 1000.0f;
    PushImpactTorque(impactTorque);
    */
}

void NPCVehicle::CalculateSelfRightingTorque()
{
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos;

    const float modVal = 0.0001f;
    DirectX::SimpleMath::Vector3 gravityForce = (DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f)) * modVal;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce = gravTorque;
}

void NPCVehicle::CalculateTopSpeed()
{
    /*
    float terminalVelocity = sqrt((2.0f * m_vehicleStruct00.vehicleData.mass * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax)
        / (m_environment->GetAirDensity() * m_vehicleStruct00.vehicleData.frontalArea * m_vehicleStruct00.vehicleData.dragCoefficient));
    */
    float observedTopSpeed = 50.0f; // using observed top speed for the moment while propulsion system design is still in flux, in meters per second?
    m_vehicleStruct00.vehicleData.topSpeedCalculated = observedTopSpeed;
}

void NPCVehicle::DebugToggleAI()
{
    m_npcAI->DebugToggle();
    /*
    if (m_isGoToggleTrue == true)
    {
        m_isGoToggleTrue = false;
    }
    else
    {
        m_isGoToggleTrue = true;
    }
    */
}

void NPCVehicle::DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    DirectX::SimpleMath::Vector4 mainBodyColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    DirectX::SimpleMath::Vector4 rearColor = DirectX::SimpleMath::Vector4(0.3f, 0.3f, 0.3f, 1.0f);
    DirectX::SimpleMath::Vector4 skirtColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    DirectX::SimpleMath::Vector4 steeringColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 jetHousingColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 jetHousingShellColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    DirectX::SimpleMath::Vector4 afterBurnColor = DirectX::SimpleMath::Vector4(1.000000000f, 0.270588249f, 0.000000000f, 1.40000000f);
    DirectX::SimpleMath::Vector4 ventColor = DirectX::SimpleMath::Vector4(0.3f, 0.3f, 0.3f, 1.0f);
    DirectX::SimpleMath::Vector4 ventColorAlt = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    ventColor = color;
    DirectX::SimpleMath::Vector4 testColorRed = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 testColorBlue = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 eyeColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    DirectX::SimpleMath::Vector4 testShadow = DirectX::SimpleMath::Vector4(-0.5f, -0.5f, -0.5f, 1.0f);
    DirectX::SimpleMath::Vector4 testHighlight = DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f);

    DirectX::SimpleMath::Vector4 targetColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 targetColorMissile = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    targetColorMissile = targetColor;

    DirectX::SimpleMath::Vector4 finColor = jetHousingShellColor;
    DirectX::SimpleMath::Vector4 jetBaseColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    //DirectX::SimpleMath::Vector4 noseColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 noseColor = jetHousingShellColor;
    //DirectX::SimpleMath::Vector4 forwardColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);     ////
    DirectX::SimpleMath::Vector4 forwardColor = jetHousingShellColor;

    //DirectX::SimpleMath::Vector4 interiorColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 interiorColor = jetHousingShellColor;

    DirectX::SimpleMath::Vector3 mainLightDirection0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
    DirectX::SimpleMath::Vector3 mainLightDirection1 = DirectX::SimpleMath::Vector3(0.7198464, 0.3420201, 0.6040227);
    DirectX::SimpleMath::Vector3 mainLightDirection2 = DirectX::SimpleMath::Vector3(0.4545195, -0.7660444, 0.4545195);
    //m_environment->GetLightDirectionalVectors(mainLightDirection0, mainLightDirection1, mainLightDirection2);

    DirectX::SimpleMath::Vector4 specular0 = DirectX::SimpleMath::Vector4(1.0f, 0.9607844f, 0.8078432f, 1.0f);
    DirectX::SimpleMath::Vector4 specular1 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 specular2 = DirectX::SimpleMath::Vector4(0.3231373f, 0.3607844f, 0.3937255f, 1.0f);

    DirectX::SimpleMath::Vector4 diffuse0 = DirectX::SimpleMath::Vector4(1.0f, 0.9607844f, 0.8078432f, 1.0f);
    DirectX::SimpleMath::Vector4 diffuse1 = DirectX::SimpleMath::Vector4(0.9647059f, 0.7607844f, 0.4078432f, 1.0f);
    DirectX::SimpleMath::Vector4 diffuse2 = DirectX::SimpleMath::Vector4(0.3231373f, 0.3607844f, 0.3937255f, 1.0f);

    DirectX::SimpleMath::Vector4 ambientColor = DirectX::SimpleMath::Vector4(0.05333332f, 0.09882354f, 0.1819608f, 1.0f);

    if (m_vehicleStruct00.vehicleData.isPlayerTargetedTrue == true || m_vehicleStruct00.vehicleData.isMissileTargetedTrue == true)
    {
        /*
        color = targetColor;
        mainBodyColor = targetColor;
        jetHousingColor = targetColor;
        ventColor = targetColor;
        eyeColor = targetColor;
        
        eyeColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        interiorColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        rearColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        jetHousingShellColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        steeringColor = targetColor;

        finColor = targetColor;
        finColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

        forwardColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        skirtColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

        jetBaseColor = DirectX::Colors::White;

        testShadow = DirectX::Colors::White;
        testHighlight = targetColor;

        testShadow = targetColor;
        testHighlight = DirectX::Colors::White;
        */
        //ambientColor = DirectX::SimpleMath::Vector4(0.8f, 0.0f, 0.0f, 1.0f);
    }

    if (m_vehicleStruct00.vehicleData.isMissileTargetedTrue == true)
    {
        /*
        color = targetColorMissile;
        mainBodyColor = targetColorMissile;
        jetHousingColor = targetColorMissile;
        ventColor = targetColorMissile;
        eyeColor = targetColorMissile;
        */
    }

   /*
    targetColor = DirectX::SimpleMath::Vector4(1.0f, 0.270588249f, 0.0f, 1.0f);
    ventColorAlt = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    color = targetColor;
    mainBodyColor = targetColor;
    jetHousingColor = targetColor;
    ventColor = targetColor;
    eyeColor = targetColor;
    */

    // override default colors with individual npc color schemes
    /*
    eyeColor = m_vehicleStruct00.npcModel.color1;
    forwardColor = m_vehicleStruct00.npcModel.color1;
    skirtColor = m_vehicleStruct00.npcModel.color1;
    jetHousingShellColor = m_vehicleStruct00.npcModel.color1;
    ventColor = m_vehicleStruct00.npcModel.color2;
    color = m_vehicleStruct00.npcModel.color2;
    ventColorAlt = m_vehicleStruct00.npcModel.color1;
    jetHousingColor = m_vehicleStruct00.npcModel.color2;
    */

    aEffect->EnableDefaultLighting();

    DirectX::SimpleMath::Vector4 diffuseAltColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    //DirectX::SimpleMath::Vector4 diffuseAltColor = DirectX::SimpleMath::Vector4(0.545098066f, 0.f, 0.f, 1.f);
    DirectX::SimpleMath::Vector4 specularAltColor = DirectX::SimpleMath::Vector4(0.7f, 0.3f, 0.3f, 1.0f);

    for (unsigned int i = 0; i < m_vehicleStruct00.npcModel.laserLightPosVec.size(); ++i)
    {
        auto laserDir = m_vehicleStruct00.vehicleData.q.position - m_vehicleStruct00.npcModel.laserLightPosVec[i];
        laserDir.Normalize();
        aEffect->SetSpecularPower(16.0f);
        //ambientColor = DirectX::SimpleMath::Vector4(0.8f, 0.0f, 0.0f, 1.0f);
        if (i == 2)
        {
            mainLightDirection0 = laserDir;
            specular0 = specularAltColor;
            diffuse0 = diffuseAltColor;
        }
        else if (i == 1)
        {
            mainLightDirection1 = laserDir;
            //specular1 = specularAltColor;
            //diffuse1 = diffuseAltColor;
        }
        else if (i == 0)
        {
            mainLightDirection2 = laserDir;
            specular2 = specularAltColor;
            diffuse2 = diffuseAltColor;
        }
        else
        {
            i = m_vehicleStruct00.npcModel.laserLightPosVec.size();
        }
    }


    //mainLightDirection0 *= -1.0f;
    //mainLightDirection1 *= -1.0f;
    //mainLightDirection2 *= -1.0f;

    //aEffect->SetAmbientLightColor(ambientColor);

    aEffect->SetLightSpecularColor(0, specular0);
    aEffect->SetLightDiffuseColor(0, diffuse0);
    aEffect->SetLightSpecularColor(1, specular1);
    aEffect->SetLightDiffuseColor(1, diffuse1);
    aEffect->SetLightSpecularColor(2, specular2);
    aEffect->SetLightDiffuseColor(2, diffuse2);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, mainLightDirection0, 20.0f, 0.0f, DirectX::Colors::Lavender);
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, mainLightDirection1, 20.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, mainLightDirection2, 20.0f, 0.0f, DirectX::Colors::Green);
    m_debugData->ToggleDebugOff();

    aEffect->SetLightDirection(0, mainLightDirection0);
    aEffect->SetLightDirection(1, mainLightDirection1);
    aEffect->SetLightDirection(2, mainLightDirection2);

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix);
    aEffect->SetColorAndAlpha(testHighlight);
    m_vehicleStruct00.npcModel.jetIntakeCoverShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2);
    aEffect->SetColorAndAlpha(testShadow);
    m_vehicleStruct00.npcModel.jetIntakeCoverShape2->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix);
    aEffect->SetColorAndAlpha(testHighlight);
    m_vehicleStruct00.npcModel.jetIntakeCoverShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2);
    aEffect->SetColorAndAlpha(testShadow);
    m_vehicleStruct00.npcModel.jetIntakeCoverShape2->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetMountMatrix);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.jetMountShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix);
    aEffect->SetColorAndAlpha(jetHousingColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix);
    aEffect->SetColorAndAlpha(jetHousingShellColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldBaseJetHousingMatrix);
    aEffect->SetColorAndAlpha(jetBaseColor);
    m_vehicleStruct00.npcModel.baseJetHousingShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldBaseJetShadowMatrix);
    aEffect->SetColorAndAlpha(testShadow);
    m_vehicleStruct00.npcModel.baseJetShadowShape->Draw(aEffect.get(), aInputLayout.Get());

    // Base jet afterburner lighting
    aEffect->SetNormalTexture(m_vehicleStruct00.npcModel.normalMapBlank.Get());
    aEffect->SetSpecularTexture(m_vehicleStruct00.npcModel.specularBlank.Get());

    DirectX::SimpleMath::Vector3 lightDirection0;
    DirectX::SimpleMath::Vector3 lightDirection1;
    DirectX::SimpleMath::Vector3 lightDirection2;

    const float baseRad = m_vehicleStruct00.npcModel.baseBurnRadius;
    const float baseLength = m_vehicleStruct00.npcModel.baseBurnScaledHeight;
    const float toa = baseLength / baseRad;
    const float baseAngle = atan(toa);
    const float baseAngleDegrees = Utility::ToDegrees(baseAngle);
    const float baseBurnRotation = m_vehicleStruct00.npcModel.baseBurnFlicker1;
    m_vehicleStruct00.npcModel.jetDirectionBase.Normalize();
    Utility::GetDispersedLightDirections(m_vehicleStruct00.npcModel.jetDirectionBase, Utility::ToRadians(baseAngleDegrees), lightDirection0, lightDirection1, lightDirection2);
    Utility::GetDispersedLightDirectionsRotation(m_vehicleStruct00.npcModel.jetDirectionBase, Utility::ToRadians(baseAngleDegrees), baseBurnRotation, lightDirection0, lightDirection1, lightDirection2);
    aEffect->SetLightDirection(0, lightDirection0);
    aEffect->SetLightDirection(1, lightDirection1);
    aEffect->SetLightDirection(2, lightDirection2);
    aEffect->SetAmbientLightColor(DirectX::Colors::White);

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldBaseBurnMatrix1);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.baseBurnShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldBaseBurnMatrix2);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.baseBurnShape->Draw(aEffect.get(), aInputLayout.Get());

    // left jet afterburner lighting
    const float leftRad = m_vehicleStruct00.npcModel.afterBurnRadius;
    const float leftLength = m_vehicleStruct00.npcModel.afterBurnLeftScaledHeight;
    const float leftToa = leftLength / leftRad;
    const float leftAngleDegrees2 = Utility::ToDegrees(leftToa);
    const float leftAngle = atan(leftToa);
    const float leftAngleDegrees = Utility::ToDegrees(leftAngle);
    const float toaTan = tan(toa);
    const float rotation = m_vehicleStruct00.npcModel.afterBurnLeftFlicker;

    Utility::GetDispersedLightDirectionsRotation(m_vehicleStruct00.npcModel.jetDirectionLeft, Utility::ToRadians(leftAngleDegrees), m_testLightRotation, lightDirection0, lightDirection1, lightDirection2);

    aEffect->SetLightDirection(0, lightDirection0);
    aEffect->SetLightDirection(1, lightDirection1);
    aEffect->SetLightDirection(2, lightDirection2);
    
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(aEffect.get(), aInputLayout.Get());

    // right jet afterburner lighting
    const float rightRad = m_vehicleStruct00.npcModel.afterBurnRadius;
    const float rightLength = m_vehicleStruct00.npcModel.afterBurnRightScaledHeight;
    const float rightToa = rightLength / rightRad;
    const float rightAngle = atan(rightToa);
    const float rightAngleDegrees = Utility::ToDegrees(rightAngle);

    Utility::GetDispersedLightDirections(m_vehicleStruct00.npcModel.jetDirectionRight, Utility::ToRadians(rightAngleDegrees), lightDirection0, lightDirection1, lightDirection2);
    Utility::GetDispersedLightDirectionsRotation(m_vehicleStruct00.npcModel.jetDirectionRight, Utility::ToRadians(rightAngleDegrees), m_testLightRotation, lightDirection0, lightDirection1, lightDirection2);

    aEffect->SetLightDirection(0, lightDirection0);
    aEffect->SetLightDirection(1, lightDirection1);
    aEffect->SetLightDirection(2, lightDirection2);

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2);
    aEffect->SetColorAndAlpha(afterBurnColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetNormalTexture(m_vehicleStruct00.npcModel.normalMapBlank.Get());
    aEffect->SetSpecularTexture(m_vehicleStruct00.npcModel.specularBlank.Get());

    aEffect->EnableDefaultLighting();

    aEffect->SetAmbientLightColor(ambientColor);
    aEffect->SetLightSpecularColor(0, specular0);
    aEffect->SetLightDiffuseColor(0, diffuse0);
    aEffect->SetLightSpecularColor(1, specular1);
    aEffect->SetLightDiffuseColor(1, diffuse1);
    aEffect->SetLightSpecularColor(2, specular2);
    aEffect->SetLightDiffuseColor(2, diffuse2);

    aEffect->SetLightDirection(0, mainLightDirection0);
    aEffect->SetLightDirection(1, mainLightDirection1);
    aEffect->SetLightDirection(2, mainLightDirection2);

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetHousingRightMatrix);
    aEffect->SetColorAndAlpha(jetHousingColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix);
    aEffect->SetColorAndAlpha(jetHousingShellColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldRearBodyMatrix);
    aEffect->SetColorAndAlpha(jetHousingShellColor);
    m_vehicleStruct00.npcModel.rearBodyShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldRearDeckMatrix);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.rearDeckShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetNormalTexture(m_vehicleStruct00.npcModel.normalMapTest1.Get());
    aEffect->SetSpecularTexture(m_vehicleStruct00.npcModel.specularTest1.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldSkirtMatrix);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.skirtShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldFrontAirDamMatrix);
    aEffect->SetColorAndAlpha(skirtColor);
    m_vehicleStruct00.npcModel.frontAirDamShape->Draw(aEffect.get(), aInputLayout.Get());

    // set textture and specular
    aEffect->SetNormalTexture(m_vehicleStruct00.npcModel.normalMapBlank.Get());
    aEffect->SetSpecularTexture(m_vehicleStruct00.npcModel.specularBlank.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldEyeLeftMatrix);
    aEffect->SetColorAndAlpha(eyeColor);
    m_vehicleStruct00.npcModel.eyeShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldEyeRightMatrix);
    aEffect->SetColorAndAlpha(eyeColor);
    m_vehicleStruct00.npcModel.eyeShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldGrillLeftMatrix);
    aEffect->SetColorAndAlpha(steeringColor);
    m_vehicleStruct00.npcModel.grillShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldGrillRightMatrix);
    aEffect->SetColorAndAlpha(steeringColor);
    m_vehicleStruct00.npcModel.grillShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldForwardMatrix);
    aEffect->SetColorAndAlpha(forwardColor);
    m_vehicleStruct00.npcModel.forwardShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldNoseConeMatrix);
    aEffect->SetColorAndAlpha(color);
    m_vehicleStruct00.npcModel.noseConeShape->Draw(aEffect.get(), aInputLayout.Get());


    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldOmniBaseMatrix);
    aEffect->SetColorAndAlpha(forwardColor);
    m_vehicleStruct00.npcModel.omniBaseShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldOmniDialMatrix);
    aEffect->SetColorAndAlpha(steeringColor);
    m_vehicleStruct00.npcModel.omniDialShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldWingArmMatrix);
    aEffect->SetColorAndAlpha(jetHousingShellColor);
    m_vehicleStruct00.npcModel.wingArmShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldWingMatrix);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.wingShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldWingFinLeftMatrix);
    aEffect->SetColorAndAlpha(finColor);
    m_vehicleStruct00.npcModel.wingFinShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldWingFinRightMatrix);
    aEffect->SetColorAndAlpha(finColor);
    m_vehicleStruct00.npcModel.wingFinShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldSteeringMatrix);
    aEffect->SetColorAndAlpha(steeringColor);
    m_vehicleStruct00.npcModel.steeringShape->Draw(aEffect.get(), aInputLayout.Get());

    // interior shadows
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldInteriorShadowLeftMatrix);
    aEffect->SetColorAndAlpha(interiorColor);
    m_vehicleStruct00.npcModel.interiorShadowShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldInteriorShadowRightMatrix);
    aEffect->SetColorAndAlpha(interiorColor);
    m_vehicleStruct00.npcModel.interiorShadowShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix1);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix2);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix3);
    aEffect->SetColorAndAlpha(ventColorAlt);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix4);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix5);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix6);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix7);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix8);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldVentMatrix9);
    aEffect->SetColorAndAlpha(ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(aEffect.get(), aInputLayout.Get());

    // Shadows
    auto lights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
    if (lights)
    {
        lights->SetLightEnabled(0, true);
        lights->SetLightEnabled(1, true);
        lights->SetLightEnabled(2, true);
        lights->SetAmbientLightColor(DirectX::Colors::Black);
        lights->SetLightDiffuseColor(0, DirectX::Colors::Black);
        lights->SetLightDiffuseColor(1, DirectX::Colors::Black);
        lights->SetLightDiffuseColor(2, DirectX::Colors::Black);
        lights->SetLightSpecularColor(0, DirectX::Colors::Black);
        lights->SetLightSpecularColor(1, DirectX::Colors::Black);
        lights->SetLightSpecularColor(2, DirectX::Colors::Black);
    }

    aEffect->SetWorld(m_vehicleStruct00.npcModel.shadowJetLeftMat);
    aEffect->SetColorAndAlpha(DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.shadowJetRightMat);
    aEffect->SetColorAndAlpha(DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldJetMountShadowMatrix);
    aEffect->SetColorAndAlpha(DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.jetMountShadowShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldSkirtShadowMatrix);
    aEffect->SetColorAndAlpha(DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.skirtShape->Draw(aEffect.get(), aInputLayout.Get());
    aEffect->SetWorld(m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix);
    aEffect->SetColorAndAlpha(DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.shadowBaseShape->Draw(aEffect.get(), aInputLayout.Get());

    m_vehicleStruct00.vehicleData.isPlayerTargetedTrue = false;
    m_vehicleStruct00.vehicleData.isMissileTargetedTrue = false;
}


void NPCVehicle::CheckIfInCameraFrustum(const DirectX::BoundingFrustum& aFrustum)
{
    if (aFrustum.Contains(m_vehicleStruct00.vehicleData.collisionBox) == true || aFrustum.Intersects(m_vehicleStruct00.vehicleData.collisionBox) == true)
    {
        m_vehicleStruct00.vehicleData.isVehicleInCameraFrustum = true;
    }
    else
    {
        m_vehicleStruct00.vehicleData.isVehicleInCameraFrustum = false;
    }
}

bool NPCVehicle::CheckVehiclePenetration(DirectX::SimpleMath::Vector3 aPos)
{
    DirectX::BoundingOrientedBox updatedCollision = m_vehicleStruct00.vehicleData.collisionBox;
    updatedCollision.Center = aPos;

    bool isPosPenetrating = false;
    std::vector<NPCVehicle*>& npcVec = m_npcController->GetVec();
    for (int i = 0; i < npcVec.size(); ++i)
    {
        if (m_vehicleStruct00.vehicleData.id != npcVec[i]->GetID())
        {
            if (updatedCollision.Intersects(npcVec[i]->GetCollisionData()) == true)
            {
                isPosPenetrating = true;
            }
        }
    }

    return isPosPenetrating;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetAntiGravGravityForce(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;
    float mainThrustMod = 1.0f;
    if (aVehicleData.altitude < lowerCurveBound)
    {
        float metersBelowLowerBound = aVehicleData.altitude - lowerCurveBound;
        float reboundMod = 0.1f;
        gravForce *= metersBelowLowerBound * reboundMod;

        const float currentCurvePos = (aVehicleData.altitude / upperCurveBound);
        mainThrustMod = 1.0f + currentCurvePos + (-metersBelowLowerBound * reboundMod);
        DirectX::SimpleMath::Vector3 stockGravForce = m_environment->GetGravityVec();
        DirectX::SimpleMath::Vector3 moddedGravForce = (stockGravForce * currentCurvePos) - stockGravForce;
        m_vehicleStruct00.npcModel.mainThrustLengthMod = moddedGravForce.y;
    }
    else if (aVehicleData.altitude > upperCurveBound)
    {
        gravForce = m_environment->GetGravityVec() * 1.0f;
        gravForce = DirectX::SimpleMath::Vector3::Zero;
        m_vehicleStruct00.npcModel.mainThrustLengthMod = 1.0f;
    }
    else
    {
        const float currentCurvePos = (aVehicleData.altitude / upperCurveBound);
        gravForce = gravForce * currentCurvePos;
        mainThrustMod = 1.0f + currentCurvePos;
        DirectX::SimpleMath::Vector3 stockGravForce = m_environment->GetGravityVec();
        DirectX::SimpleMath::Vector3 moddedGravForce = gravForce - stockGravForce;
        m_vehicleStruct00.npcModel.mainThrustLengthMod = moddedGravForce.y;

        float distanceToLowerBound = aVehicleData.altitude - lowerCurveBound;
        float verticalVelocity = aVehicleData.q.velocity.y;
        float t = 1.0f;
        float neededAccel = ((2.0f * distanceToLowerBound) / (t * t)) - ((2.0f * verticalVelocity) / t);
        gravForce = DirectX::SimpleMath::Vector3(0.0f, neededAccel, 0.0f);
    }

    DirectX::SimpleMath::Vector3 stockGravForce = m_environment->GetGravityVec();
    DirectX::SimpleMath::Vector3 moddedGravForce = gravForce - stockGravForce;
    m_vehicleStruct00.npcModel.mainThrustLengthMod *= 0.2f;
    return gravForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetBuoyancyForce(const VehicleData& aVehicleData)
{
    const DirectX::SimpleMath::Vector3 gravForce = -m_environment->GetGravityVec();
    float altitude = aVehicleData.altitude;
    bool breakToggle = false;

    float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;
    const float curveAdjustVal = lowerCurveBound;
    const float vehicleVolumeMax = aVehicleData.dimensions.x * aVehicleData.dimensions.y * aVehicleData.dimensions.z;
    const float immersedDensityNeutralAtHalfDepth = aVehicleData.mass / (vehicleVolumeMax * 0.5f);
    float immersedRange = upperCurveBound - lowerCurveBound;
    float immersedPos = altitude - curveAdjustVal;
    float immersedRatio;

    if (altitude >= upperCurveBound)
    {
        immersedRatio = 0.0f;
    }
    else if (altitude <= lowerCurveBound)
    {
        immersedRatio = 1.0f;
    }
    else
    {
        float testUpper = upperCurveBound - curveAdjustVal;
        float testLower = lowerCurveBound - curveAdjustVal;
        float testAlt = altitude - curveAdjustVal;
        float testRange = testUpper - testLower;

        float testRatio2 = testAlt / testRange;
        float immersedRatio2 = 1.0f - testRatio2;

        float testRatio = immersedPos / immersedRange;
        immersedRatio = 1.0f - testRatio;

        if (immersedRatio != immersedRatio2)
        {
            int testBreak = 0;
            testBreak++;
        }
    }

    float immersedVolume = abs(immersedRatio * vehicleVolumeMax);

    if (immersedPos > upperCurveBound)
    {
        //immersedVolume = 0.0f;
    }
    if (immersedVolume > vehicleVolumeMax)
    {
        //immersedVolume = vehicleVolumeMax;
    }
    else if (immersedVolume < 0.0f)
    {
        immersedVolume *= -1.0f;
    }
    float immersedVolumeRatio = immersedVolume / vehicleVolumeMax;

    const float penetrationVelocity = -10.0f;
    int stateVal = -1;
    if (aVehicleData.q.position.y > upperCurveBound)
    {
        stateVal = 0;
    }
    float testDensity = immersedDensityNeutralAtHalfDepth * (1.0f + immersedVolumeRatio);
    if (aVehicleData.q.velocity.y >= 0.0f && altitude > (lowerCurveBound))
    {
        testDensity = immersedDensityNeutralAtHalfDepth * (immersedVolumeRatio);
        stateVal = 1;
    }
    if (aVehicleData.q.velocity.y <= 0.0f && altitude < (upperCurveBound))
    {
        testDensity = immersedDensityNeutralAtHalfDepth * (1.0f + (immersedVolumeRatio));
        stateVal = 2;
        if (aVehicleData.q.velocity.y <= penetrationVelocity && altitude > midCurveBound)
        {
            const float overPen = aVehicleData.q.velocity.y / penetrationVelocity;

            testDensity = 90.0f * immersedVolumeRatio;
            testDensity = (immersedDensityNeutralAtHalfDepth * (3.0f + (immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio))) * overPen;
            stateVal = 6;
        }
        else if (altitude > midCurveBound)
        {
            breakToggle = true;
            stateVal = 9;
        }
        else if (aVehicleData.q.velocity.y >= penetrationVelocity)
        {
            stateVal = 10;
        }
        else
        {
            breakToggle = true;
            stateVal = 7;
        }
    }
    if (aVehicleData.q.velocity.y <= 0.0f && altitude < (midCurveBound))
    {
        testDensity = immersedDensityNeutralAtHalfDepth * (1.0f + (immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio));
        stateVal = 5;
        if (aVehicleData.q.velocity.y <= penetrationVelocity && altitude > (lowerCurveBound))
        {
            const float overPen = aVehicleData.q.velocity.y / penetrationVelocity;
            stateVal = 8;
            testDensity = immersedDensityNeutralAtHalfDepth * (4.0f + (immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio));
            testDensity *= overPen;
        }
    }
    if (aVehicleData.q.velocity.y <= 0.0f && altitude < (lowerCurveBound))
    {
        testDensity = immersedDensityNeutralAtHalfDepth * (1.0f + (immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio));
        stateVal = 3;
        if (aVehicleData.q.velocity.y <= penetrationVelocity && altitude < (lowerCurveBound))
        {
            stateVal = 11;
            const float overPen = aVehicleData.q.velocity.y / penetrationVelocity;

            testDensity = immersedDensityNeutralAtHalfDepth * (5.0f + (immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio + immersedVolumeRatio));
            testDensity *= overPen;
            float penetrationDistance = lowerCurveBound - altitude;
            testDensity *= (1.0f + (penetrationDistance * 0.1f));
        }
    }
    if (aVehicleData.q.velocity.y >= 0.0f && altitude < (lowerCurveBound))
    {
        testDensity = immersedDensityNeutralAtHalfDepth * (immersedVolumeRatio);
        stateVal = 4;
    }

    DirectX::SimpleMath::Vector3 buoyancyForce = testDensity * immersedVolume * gravForce;
    return buoyancyForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetDamperForce(const VehicleData& aVehicleData)
{
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverNeutralBoyantAlt;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;
    const float damperConstant = 19.1f;
    DirectX::SimpleMath::Vector3 currentVelocity = aVehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 damperForce = DirectX::SimpleMath::Vector3::Zero;
    if (aVehicleData.altitude > midCurveBound && aVehicleData.altitude < upperCurveBound)
    {
        if (currentVelocity.y < 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }
    else if (aVehicleData.altitude < midCurveBound && aVehicleData.altitude > lowerCurveBound)
    {
        if (currentVelocity.y > 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }

    return damperForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetForwardThrust(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 thrustUpdate = aVehicleData.forward * (aVehicleData.hoverData.forwardThrust);
    return thrustUpdate;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetHoverLift(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 liftForce = aVehicleData.up * -m_environment->GetGravity();
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;

    if (aVehicleData.altitude < lowerCurveBound)
    {
        liftForce *= 3.0f;
    }
    else if (aVehicleData.altitude > upperCurveBound)
    {
        liftForce = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        const float currentCurvePos = (aVehicleData.altitude / upperCurveBound);
        liftForce *= currentCurvePos;
    }

    return liftForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetImpactForceSum(const VehicleData& aVehicleData)
{
    const float timeStep = 1.0f;
    DirectX::SimpleMath::Vector3 impactForce = DirectX::SimpleMath::Vector3::Zero;
    for (int i = 0; i < aVehicleData.impactForceVec.size(); ++i)
    {
        //DirectX::SimpleMath::Vector3 force = (2.0 * aVehicleData.impactForceVec[i].impactMass * aVehicleData.impactForceVec[i].impactVelocity) / timeStep;
        //impactForce += aVehicleData.impactForceVec[i].impactVelocity;

        DirectX::SimpleMath::Vector3 force;

        // test calc kinetic energy
        DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleData.impactForceVec[i].impactMass * aVehicleData.impactForceVec[i].impactVelocity * aVehicleData.impactForceVec[i].impactVelocity;

        //float kEVal = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity.Length() * aImpactForce.impactVelocity.Length();
        //float newtonForce = kEVal * 10.0f;
        //float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
        // end kinetic energy test

        const float mass1 = aVehicleData.impactForceVec[i].impactMass;
        const float mass2 = m_vehicleStruct00.vehicleData.mass;
        const float coefficientOfRestitution = 0.1f;

        float tmp = 1.0f / (mass1 + mass2);
        DirectX::SimpleMath::Vector3 vx1 = aVehicleData.impactForceVec[i].impactVelocity;
        DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
        DirectX::SimpleMath::Vector3 newVx1 = (mass1 - coefficientOfRestitution * mass2) * vx1 * tmp +
            (1.0f + coefficientOfRestitution) * mass2 * vx2 * tmp;
        DirectX::SimpleMath::Vector3 newVx2 = (1.0f + coefficientOfRestitution) * mass1 * vx1 * tmp +
            (mass2 - coefficientOfRestitution * mass1) * vx2 * tmp;

        //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
        force = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
        //m_vehicleStruct00.vehicleData.impactForce.impactMass = aImpactForce.impactMass;

        //impactForce += force;
        //impactForce += aVehicleData.impactForceVec[i].impactVelocity;
        impactForce += aVehicleData.impactForceVec[i].kineticEnergy;
    }

    return impactForce;
}

Utility::Torque NPCVehicle::GetImpactTorqueSum(const VehicleData& aVehicleData)
{
    Utility::Torque impactTorque;
    impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    impactTorque.magnitude = 0.0f;

    for (int i = 0; i < aVehicleData.impactTorqueVec.size(); ++i)
    {
        impactTorque.axis += aVehicleData.impactTorqueVec[i].axis * aVehicleData.impactTorqueVec[i].magnitude;
        impactTorque.magnitude += aVehicleData.impactTorqueVec[i].magnitude;
    }
    impactTorque.axis.Normalize();
    return impactTorque;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetOmniDirectionalThrust(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 thrustDir = aVehicleData.forward;
    thrustDir = DirectX::SimpleMath::Vector3::Transform(thrustDir, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aVehicleData.up, aVehicleData.controlInput.angleToDestination));
    thrustDir.Normalize();
    thrustDir *= aVehicleData.hoverData.omniThrustMax;

    DirectX::SimpleMath::Vector3 omniThrust = aVehicleData.hoverData.omniThrustVec;
    return omniThrust;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetRepulsionForce(const VehicleData& aRepulsorVehicleData)
{
    DirectX::SimpleMath::Vector3 testSelfPos = m_vehicleStruct00.vehicleData.q.position;
    DirectX::SimpleMath::Vector3 testRepulsedPos = aRepulsorVehicleData.q.position;
    DirectX::SimpleMath::Vector3 repulsionForce = DirectX::SimpleMath::Vector3::Zero;
    const float repulsionForceMax = 1.0f * m_vehicleStruct00.vehicleData.mass;
    DirectX::SimpleMath::Vector3 repulsionForceNorm = testRepulsedPos - testSelfPos;
    const float distance = repulsionForceNorm.Length();
    repulsionForceNorm.Normalize();
    float ratio = (2.0f - (distance / (m_vehicleStruct00.vehicleData.maxCollisionDetectionRange + aRepulsorVehicleData.maxCollisionDetectionRange)));
    repulsionForce = -repulsionForceNorm * (ratio * repulsionForceMax);

    return repulsionForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetSlopeForce(const DirectX::SimpleMath::Vector3 aTerrainNorm, const float aAltitude, const float aGroundInteractionRange)
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    if (aAltitude > aGroundInteractionRange || aAltitude < 0.0f)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        const float forcePercentage = 1.0f - (aAltitude / aGroundInteractionRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = aTerrainNorm * (-m_environment->GetGravity());
        slopeForceUpdate = (terrrainForce * forcePercentage) * m_vehicleStruct00.vehicleData.mass;
    }
    slopeForceUpdate.y = 0.0f;
    return slopeForceUpdate;
}

void NPCVehicle::InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    NPCModel& aModel, VehicleHardPoints& aHardPoints, const DirectX::SimpleMath::Vector3 aDimensions)
{
    aModel.laserLightPosVec.clear();
    const float low = 0.0f;
    const float high = 1.0f;
    float x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    float y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    float z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    aModel.color1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
    x = 1.0f - x;
    y = 1.0f - y;
    z = 1.0f - z;
    aModel.color2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
    x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    aModel.color3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
    aModel.color3 = aModel.color2;
    aModel.color3 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    aModel.color4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);


    /*
    aModel.color1 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    aModel.color2 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    aModel.color3 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    aModel.color4 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    */

    const float zFightOffset = 0.02f;
    DirectX::SimpleMath::Matrix centerMassTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(aDimensions.x * 0.0f, -aDimensions.y * 0.0f, aDimensions.z * 0.0f);

    DirectX::SimpleMath::Vector3 baseShadowSize = aDimensions;
    baseShadowSize.y *= 0.5f;
    aModel.shadowBaseShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), baseShadowSize);
    aModel.localShadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    aModel.localShadowBaseMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(baseShadowSize.x * 0.04f, baseShadowSize.y * 0.45f, 0.0f));

    aModel.localBodyMainShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.bodyMainShadowTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(baseShadowSize.x * 0.04f, baseShadowSize.y * 0.45f, 0.0f));
    aModel.worldBodyMainShadowMatrix = aModel.localBodyMainShadowMatrix;

    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localModelMatrix *= centerMassTranslation;

    DirectX::SimpleMath::Vector3 mainBodySize = aDimensions;
    mainBodySize.x -= zFightOffset * 2.0f;
    mainBodySize.y -= (zFightOffset * 10.0f) * 2.0f;
    mainBodySize.z *= 0.45f;

    const float mainBodyOffset = 2.0f;
    mainBodySize.x -= mainBodyOffset;
    const float mainBodyCutOff = 3.0f;
    mainBodySize.x -= mainBodyCutOff;
    aModel.interiorShadowShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainBodySize);
    const float ventShadowAngle = Utility::ToRadians(-3.5f);
    DirectX::SimpleMath::Vector3 mainBodyTranslation(-(mainBodyOffset * 0.5f) + (mainBodyCutOff * 0.5f), 0.0f, 0.0f);
    mainBodyTranslation.z = -mainBodySize.z * 0.5f;

    const float intShadowScaleX = 0.95f;
    const float intShadowScaleY = 0.9f;
    const float intShadowScaleZ = 0.8f;

    DirectX::SimpleMath::Vector3 shadowTranslation = mainBodyTranslation;
    shadowTranslation.x += mainBodySize.z - (mainBodySize.z * cos(ventShadowAngle));
    shadowTranslation.x += -0.2f;
    shadowTranslation.z += -0.205f;

    aModel.localInteriorShadowLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localInteriorShadowLeftMatrix = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(intShadowScaleX, intShadowScaleY, intShadowScaleZ));
    aModel.localInteriorShadowLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(ventShadowAngle);
    //shadowTranslation.z *= -1.0f;
    aModel.localInteriorShadowLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(shadowTranslation);
    aModel.localInteriorShadowLeftMatrix *= centerMassTranslation;
    aModel.worldInteriorShadowLeftMatrix = aModel.localInteriorShadowLeftMatrix;

    shadowTranslation.z *= -1.0f;
    aModel.localInteriorShadowRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localInteriorShadowRightMatrix = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(intShadowScaleX, intShadowScaleY, intShadowScaleZ));
    aModel.localInteriorShadowRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(-ventShadowAngle);
   
    aModel.localInteriorShadowRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(shadowTranslation);
    aModel.localInteriorShadowRightMatrix *= centerMassTranslation;
    aModel.worldInteriorShadowRightMatrix = aModel.localInteriorShadowRightMatrix;

    // rear body
    DirectX::SimpleMath::Vector3 rearBodySize = aDimensions;
    //rearBodySize.x *= 0.3f;
    //rearBodySize.y *= 0.95f;
    //rearBodySize.z *= 1.03f;

    rearBodySize.x *= 0.3f;
    rearBodySize.y *= 0.95f;
    rearBodySize.z *= 1.1f;

    float rearBodyOffset = 2.0f;
    rearBodySize.x -= rearBodyOffset;
    rearBodyOffset = 4.0f;
    DirectX::SimpleMath::Vector3 rearBodyTranslation = DirectX::SimpleMath::Vector3(-rearBodyOffset * 0.5f, 0.0f, 0.0f);
    rearBodyTranslation.x += -3.0f;
    rearBodyTranslation.y += 0.3f;
    rearBodyTranslation.z += 0.0f;
    aModel.rearBodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearBodySize);
    aModel.localRearBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearBodyTranslation);
    aModel.localRearBodyMatrix *= centerMassTranslation;
    aModel.worldRearBodyMatrix = aModel.localRearBodyMatrix;

    // rear deck
    const float rearDeckOffset = 2.0f;
    const float rearDeckSize = 5.5f;
    DirectX::SimpleMath::Vector3 rearDeckScale(0.9f, 1.0f, 0.6f);
    DirectX::SimpleMath::Vector3 rearDeckTranslation = DirectX::SimpleMath::Vector3(-rearDeckOffset * 0.5f, 0.0f, 0.0f);
    rearDeckTranslation.x += -4.0f;
    rearDeckTranslation.x += -(rearDeckSize * rearDeckScale.x) * 0.1f;
    rearDeckTranslation.y += -1.8f;
    rearDeckTranslation.y += (rearDeckSize * rearDeckScale.y) * 0.5f;
    rearDeckTranslation.z += 0.0f;

    aModel.rearDeckShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), rearDeckSize);
    aModel.localRearDeckMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(55.0f));
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateScale(rearDeckScale);
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearDeckTranslation);
    aModel.localRearDeckMatrix *= centerMassTranslation;
    aModel.worldRearDeckMatrix = aModel.localRearDeckMatrix;

    // wing arm
    DirectX::SimpleMath::Vector3 wingArmSize(2.0f, 1.0f, 1.0f);
    wingArmSize.x *= 1.0f;
    wingArmSize.y *= 1.0f;
    wingArmSize.z *= 1.0f;
    aModel.wingArmShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingArmSize);
    DirectX::SimpleMath::Vector3 wingArmTranslation = rearDeckTranslation;
    wingArmTranslation.x += 1.6f;
    wingArmTranslation.x -= 0.61f;
    wingArmTranslation.y += rearDeckSize * 0.6f;
    wingArmTranslation.z = 0.0f;

    DirectX::SimpleMath::Vector3 wingArmLocalTranslation;
    wingArmLocalTranslation.x = -wingArmSize.x * 0.85f;
    wingArmLocalTranslation.y = 0.0f;
    wingArmLocalTranslation.z = 0.0f;

    aModel.localWingArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingArmLocalTranslation);
    aModel.localWingArmTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingArmTranslation);
    aModel.localWingArmMatrix *= centerMassTranslation;
    aModel.worldWingArmMatrix = aModel.localWingArmMatrix;

    // wing
    DirectX::SimpleMath::Vector3 wingSize(2.0f, 1.0f, 1.0f);
    wingSize.x *= 1.0f;
    wingSize.y *= 0.5f;
    wingSize.z *= aDimensions.z;
    aModel.wingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingSize);
    DirectX::SimpleMath::Vector3 wingTranslation = wingArmTranslation;
    wingTranslation.z += 0.0f;

    DirectX::SimpleMath::Vector3 wingLocalTranslation = wingArmLocalTranslation;
    wingLocalTranslation.x += -0.26 * 1.0f;
    wingLocalTranslation.y += wingSize.y * 1.5f;
    wingLocalTranslation.z += 0.0f;

    aModel.localWingMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingLocalTranslation);
    aModel.localWingTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingTranslation);
    aModel.localWingMatrix *= centerMassTranslation;
    aModel.worldWingMatrix = aModel.localWingMatrix;

    // wing fin left
    DirectX::SimpleMath::Vector3 wingFinSize(2.0f, 2.0f, 1.0f);
    wingFinSize.x *= 1.0f;
    wingFinSize.y *= 0.95f;
    wingFinSize.z *= 0.5f;
    const float wingFinOffSetX = rearBodySize.x - wingFinSize.x;
    wingFinSize.x += wingFinOffSetX;

    aModel.wingFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingFinSize);
    DirectX::SimpleMath::Vector3 wingFinLeftTranslation = wingTranslation;
    wingFinLeftTranslation.x += 0.0f;
    wingFinLeftTranslation.y += 0.0f;
    wingFinLeftTranslation.z += 0.0f;
    wingFinLeftTranslation.z += (-wingSize.z * 0.5f) + (-wingFinSize.z * 0.5f);

    DirectX::SimpleMath::Vector3 wingFinLeftLocalTranslation = wingLocalTranslation;
    wingFinLeftLocalTranslation.x += 0.0f;
    wingFinLeftLocalTranslation.x -= wingFinOffSetX;
    wingFinLeftLocalTranslation.y += 0.0f;

    aModel.localWingFinLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingFinLeftLocalTranslation);
    aModel.localWingFinLeftTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingFinLeftTranslation);
    aModel.localWingFinLeftMatrix *= centerMassTranslation;
    aModel.worldWingFinLeftMatrix = aModel.localWingFinLeftMatrix;

    // wing fin right
    DirectX::SimpleMath::Vector3 wingFinRightTranslation = wingTranslation;
    wingFinRightTranslation.x += 0.0f;
    wingFinRightTranslation.y += 0.0f;
    wingFinRightTranslation.z += 0.0f;

    DirectX::SimpleMath::Vector3 wingFinRightLocalTranslation = wingLocalTranslation;
    wingFinRightLocalTranslation.x += 0.0f;
    wingFinRightLocalTranslation.x -= wingFinOffSetX;
    wingFinRightLocalTranslation.y += 0.0f;
    wingFinRightLocalTranslation.z += (wingSize.z * 0.5f) + (wingFinSize.z * 0.5f);

    aModel.localWingFinRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingFinRightLocalTranslation);
    aModel.localWingFinRightTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingFinRightTranslation);
    aModel.localWingFinRightMatrix *= centerMassTranslation;
    aModel.worldWingFinRightMatrix = aModel.localWingFinRightMatrix;

    // Jet housing left
    const float jetHousingDiameter = aDimensions.y * 0.3f;
    const float jetHousingThickness = 0.5f;
    aModel.jetHousingShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), jetHousingDiameter, jetHousingThickness);
    DirectX::SimpleMath::Vector3 jetHousingLeftTranslation;
    jetHousingLeftTranslation.x = -aDimensions.x * 0.38f;
    jetHousingLeftTranslation.y = aDimensions.y * 0.15f;
    jetHousingLeftTranslation.z = -(aDimensions.z * 0.5f) - (jetHousingDiameter * 0.5f) + (jetHousingThickness * -1.5f);

    aModel.localJetHousingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    DirectX::SimpleMath::Matrix jetHousingScale = DirectX::SimpleMath::Matrix::CreateScale(11.0f, 1.0f, 1.0f);
    aModel.localJetHousingLeftMatrix *= jetHousingScale;
    aModel.jetHousingTranslationLeftMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);
    aModel.worldJetHousingLeftMatrix = aModel.localJetHousingLeftMatrix;

    aModel.shadowJetTranslationLeft = jetHousingLeftTranslation;
    aModel.shadowJetTranslationLeftMat = DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);

    aHardPoints.localLeftJetAxis = jetHousingLeftTranslation;
    aHardPoints.leftJetAxis = aHardPoints.localLeftJetAxis;

    // Jet housing shell left
    aModel.localJetHousingShellLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingShellLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingShellLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    DirectX::SimpleMath::Matrix jetHousingShellScale = DirectX::SimpleMath::Matrix::CreateScale(8.0f, 1.1f, 1.1f);
    aModel.localJetHousingShellLeftMatrix *= jetHousingShellScale;
    aModel.worldJetHousingShellLeftMatrix = aModel.localJetHousingShellLeftMatrix;

    // Jet housing right
    DirectX::SimpleMath::Vector3 jetHousingRightTranslation = jetHousingLeftTranslation;
    jetHousingRightTranslation.z = -jetHousingLeftTranslation.z;

    aModel.localJetHousingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    aModel.localJetHousingRightMatrix *= jetHousingScale;
    aModel.jetHousingTranslationRightMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingRightTranslation);
    aModel.worldJetHousingRightMatrix = aModel.localJetHousingRightMatrix;

    // Jet housing shell right
    aModel.localJetHousingShellRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingShellRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingShellRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    aModel.localJetHousingShellRightMatrix *= jetHousingShellScale;
    aModel.worldJetHousingShellRightMatrix = aModel.localJetHousingShellRightMatrix;

    aHardPoints.localRightJetAxis = jetHousingRightTranslation;
    aHardPoints.rightJetAxis = aHardPoints.localRightJetAxis;

    // Jet mounting arm
    DirectX::SimpleMath::Vector3 jetMountSize;
    jetMountSize.x = 1.1f;
    jetMountSize.y = 2.0f;
    jetMountSize.z = aDimensions.z * 1.2f;
    DirectX::SimpleMath::Vector3 jetMountTranslation = jetHousingLeftTranslation;
    jetMountTranslation.z = 0.0f;
    aModel.jetMountShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), jetMountSize.z, jetMountSize.y);
    aModel.localJetMountMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetMountMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aModel.localJetMountMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetMountTranslation);
    aModel.worldJetMountMatrix = aModel.localJetMountMatrix;

    aHardPoints.localJetArmCenterAxis = jetMountTranslation;
    aHardPoints.jetArmCenterAxis = aHardPoints.localJetArmCenterAxis;

    // Jet mounting arm shadow
    DirectX::SimpleMath::Vector3 jetMountShadowSize = jetMountSize;
    jetMountShadowSize.z *= 1.27f;
    aModel.jetMountShadowShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), jetMountShadowSize.z, jetMountShadowSize.y);
    aModel.localJetMountShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetMountShadowMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aModel.jetMountShadowTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(jetMountTranslation);
    aModel.worldJetMountShadowMatrix = aModel.localJetMountShadowMatrix;


    // jet intake
    const float jetIntakeSize = 1.0f;
    aModel.jetIntakeCoverShape = DirectX::GeometricPrimitive::CreateIcosahedron(aContext.Get(), jetIntakeSize);
    aModel.jetIntakeCoverShape2 = DirectX::GeometricPrimitive::CreateIcosahedron(aContext.Get(), jetIntakeSize * 0.99f);

    DirectX::SimpleMath::Vector3 jetIntakeLocalTranslation;
    jetIntakeLocalTranslation.x = 1.4f;
    jetIntakeLocalTranslation.y = 0.0f;
    jetIntakeLocalTranslation.z = 0.0f;
    const float jetIntakeAngle = 63.435f * 0.5f;
    aModel.localJetIntakeCoverLeftTranslationMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(jetIntakeAngle));
    aModel.localJetIntakeCoverLeftTranslationMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetIntakeLocalTranslation);
    aModel.localJetIntakeCoverLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetIntakeCoverLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);
    aModel.worldJetIntakeCoverLeftMatrix = aModel.localJetIntakeCoverLeftMatrix;

    aModel.localJetIntakeCoverLeftTranslationMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(jetIntakeAngle));
    aModel.localJetIntakeCoverLeftTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    aModel.localJetIntakeCoverLeftTranslationMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(jetIntakeLocalTranslation);
    aModel.localJetIntakeCoverLeftMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetIntakeCoverLeftMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);
    aModel.worldJetIntakeCoverLeftMatrix2 = aModel.localJetIntakeCoverLeftMatrix;

    // jet intake cover right
    aModel.localJetIntakeCoverRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetIntakeCoverRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingRightTranslation);
    aModel.worldJetIntakeCoverRightMatrix = aModel.localJetIntakeCoverRightMatrix;
    aModel.localJetIntakeCoverRightMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetIntakeCoverRightMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingRightTranslation);
    aModel.worldJetIntakeCoverRightMatrix2 = aModel.localJetIntakeCoverRightMatrix2;

    aModel.afterBurnLeftFlicker = 0.0f;
    aModel.afterBurnRightFlicker = 0.0f;
    aModel.afterBurnFlickerRate = 100.1f;
    //aModel.afterBurnFlickerRate = 0.0f;

    // Jet after burn left
    const float afterBurnDiameter = jetHousingDiameter * 0.9f;
    const float afterBurnHeight = 0.1f;
    aModel.afterBurnRadius = afterBurnDiameter * 0.5f;
    aModel.afterBurnHeight = afterBurnHeight;
    aModel.afterBurnShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), afterBurnDiameter, afterBurnHeight, 64);
    DirectX::SimpleMath::Vector3 afterBurnLowerLeftTranslation;
    afterBurnLowerLeftTranslation = jetHousingLeftTranslation;
    afterBurnLowerLeftTranslation.y += 0.0f;
    aModel.localAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.worldAfterBurnLeftMatrix = aModel.localAfterBurnLeftMatrix;

    // Jet after burn right
    DirectX::SimpleMath::Vector3 afterBurnRightTranslation;
    afterBurnRightTranslation = jetHousingRightTranslation;
    afterBurnRightTranslation.y += 0.0f;
    aModel.localAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localAfterBurnRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
    aModel.worldAfterBurnRightMatrix = aModel.localAfterBurnRightMatrix;
    // end

    DirectX::SimpleMath::Vector3 skirtSize(aDimensions);
    skirtSize.x *= 1.1f;
    skirtSize.y *= 0.5f;
    skirtSize.z *= 1.01f;
    DirectX::SimpleMath::Vector3 skirtTranslation;
    //skirtTranslation.x = -0.3f;
    skirtTranslation.x = 0.0f;
    skirtTranslation.y = (-aDimensions.y * 0.5f) + (skirtSize.y * 0.5f) - zFightOffset;
    skirtTranslation.z = 0.0f;
    aModel.skirtShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), skirtSize);
    aModel.skirtShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), skirtSize.z, 1.0f, 4);
    aModel.localSkirtMatrix = DirectX::SimpleMath::Matrix::Identity;
    const float skirtRot = Utility::ToRadians(-55.0f);
    aModel.localSkirtMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(skirtRot);
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    const float skirtTilt = Utility::ToRadians(3.0f);
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateScale(19.8f, 5.0f, 1.0f);
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(skirtTilt);
    aModel.localSkirtShadowMatrix = aModel.localSkirtMatrix;
    skirtTranslation.y = (-aDimensions.y * 0.15f);
    skirtTranslation.y = -1.85f;
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(skirtTranslation);
    aModel.localSkirtMatrix *= centerMassTranslation;
    aModel.worldSkirtMatrix = aModel.localSkirtMatrix;

    aModel.skirtShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.skirtShadowTranslationMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(skirtTranslation);
    aModel.skirtShadowTranslationMatrix *= centerMassTranslation;
    aModel.worldSkirtShadowMatrix = aModel.localSkirtShadowMatrix;

    DirectX::SimpleMath::Vector3 noseConeSize(aDimensions);
    noseConeSize.x *= 1.0f;
    noseConeSize.y *= 1.0f;
    noseConeSize.z *= 1.0f;
    DirectX::SimpleMath::Vector3 noseConeTranslation;
    noseConeTranslation.x = (aDimensions.x * 0.5f) - (mainBodyOffset * 1.0f);
    noseConeTranslation.x += -0.02f;
    noseConeTranslation.y = 0.0f;
    noseConeTranslation.z = 0.0f;
    const float noseSize = (aDimensions.z * 0.5f) * sqrt(2.0f);
    aModel.noseConeShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), noseSize);
    aModel.localNoseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    const float yScale = 1.0f / (aDimensions.y * 0.5f) * sqrt(6.0f);
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateScale(0.38f, yScale, 1.0f);
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    aModel.localNoseConeMatrix *= centerMassTranslation;
    aModel.worldNoseConeMatrix = aModel.localNoseConeMatrix;

    DirectX::SimpleMath::Matrix testEyeMat = DirectX::SimpleMath::Matrix::Identity;
    // eye left
    DirectX::SimpleMath::Vector3 eyeSize;
    eyeSize.x = 2.f;
    eyeSize.y = 1.0f;
    //eyeSize.z = 0.3f;
    eyeSize.z = 0.5f;
    DirectX::SimpleMath::Vector3 eyeLeftTranslation = noseConeTranslation;
    eyeLeftTranslation.x = aDimensions.x * 0.5f;
    eyeLeftTranslation.x += -1.055f;
    eyeLeftTranslation.y = aDimensions.y * 0.17f + 0.0f;
    eyeLeftTranslation.z = -aDimensions.z * 0.32f;
    DirectX::SimpleMath::Vector3 eyeRightTranslation2 = eyeLeftTranslation;
    DirectX::SimpleMath::Vector3 eyeLeftTranslation2 = eyeLeftTranslation;
    eyeLeftTranslation2.x = 0.0f;
    eyeLeftTranslation2.y = 0.0f;
    eyeLeftTranslation.z = 0.0f;
    aModel.eyeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), eyeSize);
    aModel.eyeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), eyeSize.z, eyeSize.x, 3);
    aModel.localEyeLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-20.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 3.0f);
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(27.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(28.5f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
    const float eyeInflection = 33.0f;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0));
    const float eyeRotation = 62.0f;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(eyeLeftTranslation);
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(eyeLeftTranslation2);
    aModel.localEyeLeftMatrix *= centerMassTranslation;
    aModel.worldEyeLeftMatrix = aModel.localEyeLeftMatrix;

    // eye right
    DirectX::SimpleMath::Vector3 eyeRightTranslation = eyeLeftTranslation;
    eyeRightTranslation.z *= -1.0f;
    eyeRightTranslation2.z *= -1.0f;
    aModel.localEyeRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-20.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 3.0f);
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(27.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-28.5f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(eyeRightTranslation2);
    aModel.localEyeRightMatrix *= centerMassTranslation;
    aModel.worldEyeRightMatrix = aModel.localEyeRightMatrix;

    // front air dam
    DirectX::SimpleMath::Vector3 frontAirDamSize(aDimensions);
    frontAirDamSize.x *= 0.2f;
    frontAirDamSize.y *= 0.4f;
    frontAirDamSize.z *= 0.9f;
    DirectX::SimpleMath::Vector3 frontAirDamTranslation;
    frontAirDamTranslation.x = aDimensions.x * 0.37f;
    frontAirDamTranslation.x = aDimensions.x * 0.45f;
    frontAirDamTranslation.y = -aDimensions.y * 0.4f;
    frontAirDamTranslation.z = 0.0f;
    aModel.frontAirDamShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), frontAirDamSize.z, frontAirDamSize.x, 3);
    aModel.localFrontAirDamMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(8.0f));
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(frontAirDamTranslation);
    aModel.localFrontAirDamMatrix *= centerMassTranslation;
    aModel.worldFrontAirDamMatrix = aModel.localFrontAirDamMatrix;

    // grill left
    DirectX::SimpleMath::Vector3 grillSize(aDimensions);
    grillSize.x = 1.0f;
    grillSize.y = 3.2f;
    grillSize.z = 2.0f;
    aModel.grillShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), grillSize.z, grillSize.y, 3);
    DirectX::SimpleMath::Vector3 grillLeftTranslation;
    grillLeftTranslation.x = aDimensions.x * 0.5f;
    grillLeftTranslation.y = -aDimensions.y * 0.2f;
    grillLeftTranslation.z = -3.5f;

    aModel.localGrillLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localGrillLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localGrillLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-30.0f));
    aModel.localGrillLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(grillLeftTranslation);
    aModel.localGrillLeftMatrix *= centerMassTranslation;
    aModel.worldGrillLeftMatrix = aModel.localGrillLeftMatrix;

    // grill right
    DirectX::SimpleMath::Vector3 grillRightTranslation = grillLeftTranslation;
    grillRightTranslation.z *= -1.0f;
    aModel.localGrillRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localGrillRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localGrillRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-30.0f));
    aModel.localGrillRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(grillRightTranslation);
    aModel.localGrillRightMatrix *= centerMassTranslation;
    aModel.worldGrillRightMatrix = aModel.localGrillRightMatrix;

    const float omniBaseDiameter = aDimensions.x * 0.32f;
    const float omniBaseHeight = aDimensions.y * 0.95f;
    aModel.omniBaseShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), omniBaseHeight, omniBaseDiameter);
    DirectX::SimpleMath::Vector3 omniBaseTranslation;
    omniBaseTranslation.x = -omniBaseDiameter * 0.21f;
    omniBaseTranslation.y = 0.35f;
    omniBaseTranslation.z = 0.0f;
    aModel.localOmniBaseMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localOmniBaseMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(omniBaseTranslation);
    aModel.localOmniBaseMatrix *= centerMassTranslation;
    aModel.worldForwardMatrix = aModel.localOmniBaseMatrix;

    const float omniDialDiameter = omniBaseDiameter * 0.3f;
    aModel.omniDialShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), omniDialDiameter);
    DirectX::SimpleMath::Vector3 omniDialTranslation = omniBaseTranslation;
    omniDialTranslation.y = aDimensions.y * 0.5f;
    aModel.localOmniDialMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localOmniDialMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(omniDialTranslation);
    aModel.localOmniDialMatrix *= centerMassTranslation;
    aModel.worldOmniDialMatrix = aModel.localOmniDialMatrix;
    aModel.omniDialRadius = (omniBaseDiameter * 0.5f) - (omniDialDiameter * 0.5f);

    const float baseJetHousingDiameter = aDimensions.z * 0.7f;
    const float baseJetHousingThickness = 1.0f;
    aModel.baseJetHousingShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), baseJetHousingDiameter, baseJetHousingThickness);
    DirectX::SimpleMath::Vector3 baseJetHousingTranslation = omniBaseTranslation;
    baseJetHousingTranslation.y = (aDimensions.y * -0.5f) - 0.2f;
    baseJetHousingTranslation.x += 0.75f;
    aModel.localBaseJetHousingMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localBaseJetHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(baseJetHousingTranslation);
    aModel.localBaseJetHousingMatrix *= centerMassTranslation;
    aModel.worldBaseJetHousingMatrix = aModel.localBaseJetHousingMatrix;

    // base jet shadow 
    const float baseJetShadowDiameter = baseJetHousingDiameter;
    const float baseJetShadowHeight = baseJetHousingThickness;
    aModel.baseJetShadowShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), baseJetShadowHeight, baseJetShadowDiameter);
    DirectX::SimpleMath::Vector3 baseJetShadowTranslation = baseJetHousingTranslation;
    baseJetShadowTranslation.y += baseJetShadowHeight * 0.5f;
    aModel.localBaseJetShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localBaseJetShadowMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(baseJetShadowTranslation);
    aModel.localBaseJetShadowMatrix *= centerMassTranslation;
    aModel.worldBaseJetShadowMatrix = aModel.localBaseJetShadowMatrix;

    // base burner start
    aModel.baseBurnFlicker1 = 0.0f;
    aModel.baseBurnFlicker2 = 0.0f;
    const float baseBurnDiameter = baseJetHousingDiameter - baseJetHousingThickness;
    const float baseBurnHeight = 0.1f;
    aModel.baseBurnRadius = baseBurnDiameter * 0.5f;
    aModel.baseBurnHeight = baseBurnHeight;
    aModel.baseBurnShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), baseBurnDiameter, baseBurnHeight, 64);
    DirectX::SimpleMath::Vector3 baseBurnTranslation = baseJetHousingTranslation;
    baseBurnTranslation.y -= 0.25f;
    aModel.localBaseBurnMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localBaseBurnMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    aModel.localBaseBurnMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(baseBurnTranslation);
    aModel.worldBaseBurnMatrix1 = aModel.localBaseBurnMatrix;
    aModel.worldBaseBurnMatrix2 = aModel.localBaseBurnMatrix;
    // base burner end


    // old wing
    DirectX::SimpleMath::Vector3 rearShapeSize = aDimensions;
    rearShapeSize.x *= 0.2f;
    rearShapeSize.y *= 0.1f;
    rearShapeSize.z *= 1.05f;
    aModel.rearShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearShapeSize);
    DirectX::SimpleMath::Vector3 rearShapeTranslation;
    rearShapeTranslation.x = -(aDimensions.x * 0.5f) + (rearShapeSize.x * 0.45f);
    rearShapeTranslation.x += -0.3f;
    rearShapeTranslation.y = aDimensions.y * 0.51f;
    rearShapeTranslation.y += .5f;
    rearShapeTranslation.z = 0.0f;
    aModel.localRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-0.0f));
    aModel.localRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearShapeTranslation);
    aModel.localRearMatrix *= centerMassTranslation;
    aModel.worldRearMatrix = aModel.localRearMatrix;

    DirectX::SimpleMath::Vector3 throttleShapeSize = rearShapeSize;
    throttleShapeSize.x *= 1.05f;
    throttleShapeSize.y *= 1.05f;
    throttleShapeSize.z *= 1.05f;
    aModel.throttleShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), throttleShapeSize);
    DirectX::SimpleMath::Vector3 throttleShapeTranslation;
    throttleShapeTranslation = rearShapeTranslation;
    aModel.localThrottleMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-15.0f));
    aModel.localThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(throttleShapeTranslation);
    aModel.localThrottleMatrix *= centerMassTranslation;
    aModel.worldThrottleMatrix = aModel.localThrottleMatrix;

    DirectX::SimpleMath::Vector3 forwardShapeSize = aDimensions;
    //forwardShapeSize.x *= 0.85f;
    //forwardShapeSize.y *= 0.52f;
    //forwardShapeSize.z *= 1.05f;

    forwardShapeSize.x *= 0.75f;
    forwardShapeSize.y *= 0.55f;
    forwardShapeSize.z *= 1.05f;

    aModel.forwardShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), forwardShapeSize.y, forwardShapeSize.x, 3);
    DirectX::SimpleMath::Vector3 forwardShapeTranslation;
    forwardShapeTranslation.x = (aDimensions.x * 0.5f) - (forwardShapeSize.x * 0.292f);
    //forwardShapeTranslation.y = aDimensions.y * 0.26f;
    forwardShapeTranslation.y = forwardShapeSize.y * 0.5f;
    forwardShapeTranslation.z = 0.0f;
    aModel.localForwardMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.7f, 1.0f, 0.9f));
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(forwardShapeTranslation);
    aModel.localForwardMatrix *= centerMassTranslation;
    aModel.worldForwardMatrix = aModel.localModelMatrix;

    DirectX::SimpleMath::Vector3 steeringShapeSize = forwardShapeSize;
    steeringShapeSize.x *= 0.85f;
    steeringShapeSize.y *= 1.05f;
    steeringShapeSize.z *= 1.05f;
    aModel.steeringShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), steeringShapeSize.y, steeringShapeSize.x, 3);
    DirectX::SimpleMath::Vector3 steeringShapeTranslation = forwardShapeTranslation;
    steeringShapeTranslation.x *= 1.1f;
    aModel.localSteeringMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.45f, 1.0f, 0.25f));
    aModel.translationSteeringMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(steeringShapeTranslation);
    aModel.translationSteeringMatrix *= centerMassTranslation;
    aModel.worldSteeringMatrix = aModel.localSteeringMatrix;

    DirectX::SimpleMath::Vector3 ventSize = aDimensions;
    ventSize.x = mainBodySize.x;
    ventSize.y *= 0.02f;
    ventSize.z *= 1.0f;
    aModel.ventShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), ventSize);

    const float ventOffsetY = 0.7f;
    const DirectX::SimpleMath::Vector3 ventOffSet(0.0f, -0.33f, 0.0f);
    DirectX::SimpleMath::Vector3 ventScale(1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector3 ventScaleOffSet(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 ventTranslation(-0.3f, 0.0f, 0.0f);
    ventTranslation.x = (-aDimensions.x * 0.5f) + (ventSize.x * 0.49f);
    ventTranslation.x = -mainBodyOffset * 1.0f;
    ventTranslation.x = mainBodyTranslation.x;
    ventTranslation.y = (aDimensions.y * 0.5f) - (ventSize.y * 0.5f) + zFightOffset;
    ventTranslation.z = 0.0f;

    const float topVentScale = 5.0f;
    DirectX::SimpleMath::Vector3 topVentTranslation = ventTranslation;
    const float topVentOffsetY = -((ventSize.y * 0.5f) * topVentScale) + (zFightOffset * 2.5f);
    topVentTranslation.y += topVentOffsetY;
    topVentTranslation.z += 0.0f;
    aModel.localVentMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix1 *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, topVentScale, 1.0f);
    aModel.localVentMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(topVentTranslation);
    aModel.worldVentMatrix1 = aModel.localVentMatrix1;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    ventTranslation.y -= -topVentOffsetY * 2.0f;
    aModel.localVentMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix2 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix2 = aModel.localVentMatrix2;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix3 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix3 = aModel.localVentMatrix3;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix4 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix4 = aModel.localVentMatrix4;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix5 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix5 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix5 = aModel.localVentMatrix5;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix6 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix6 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix6 = aModel.localVentMatrix6;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix7 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix7 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix7 = aModel.localVentMatrix7;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix8 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix8 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix8 = aModel.localVentMatrix8;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix9 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix9 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix9 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix9 = aModel.localVentMatrix9;

    aModel.avoidanceShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

    aModel.localCustomMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldCustomMatrix = aModel.localCustomMatrix;

    DirectX::GeometricPrimitive::VertexCollection vertices;
    DirectX::GeometricPrimitive::IndexCollection indices;
    DirectX::GeometricPrimitive::CreateBox(vertices, indices,
        DirectX::XMFLOAT3(1.f / 2.f, 2.f / 2.f, 3.f / 2.f));

    for (auto& it : vertices)
    {
        //it.textureCoordinate.x *= 5.f;
        //it.textureCoordinate.y *= 5.f;
    }

    DirectX::GeometricPrimitive::VertexCollection vertices2;
    DirectX::GeometricPrimitive::IndexCollection indices2;
    //DirectX::GeometricPrimitive::CreateBox(vertices2, indices2, DirectX::XMFLOAT3(3.f / 2.f, 2.f / 2.f, 1.f / 2.f));
    DirectX::GeometricPrimitive::CreateCone(vertices2, indices2, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector3 testTranslation = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    //DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    for (auto& it : vertices2)
    {
        it.position.x += testTranslation.x;
        it.position.y += testTranslation.y;
        it.position.z += testTranslation.z;
        //it.textureCoordinate.x *= 5.f;
        //it.textureCoordinate.y *= 5.f;
    }
    for (auto& it : indices2)
    {

        //it.position.x += testTranslation.x;
        //it.position.y += testTranslation.y;
        //it.position.z += testTranslation.z;
        //it.textureCoordinate.x *= 5.f;
        //it.textureCoordinate.y *= 5.f;
    }

    for (auto& it : vertices2)
    {
        //it.textureCoordinate.x *= 5.f;
        //it.textureCoordinate.y *= 5.f;
    }
    int vertSize1 = indices.size();
    for (auto& it : vertices2)
    {
        vertices.push_back(it);
    }
    for (auto& it : indices2)
    {
        indices.push_back(it);
    }
    int vertSize2 = indices.size();

    DirectX::GeometricPrimitive::VertexCollection vertices3;
    DirectX::GeometricPrimitive::IndexCollection indices3;
    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices3.push_back(vertices[i]);
    }
    for (int i = 0; i < indices.size(); ++i)
    {
        indices3.push_back(indices[i]);
    }

    for (int i = 0; i < vertices2.size(); ++i)
    {
        vertices3.push_back(vertices2[i]);
    }
    for (int i = 0; i < indices2.size(); ++i)
    {
        indices3.push_back(indices2[i]);
    }

    aModel.customShape = DirectX::GeometricPrimitive::CreateCustom(aContext.Get(), vertices3, indices3);
    vertSize2 = indices.size();
}

void NPCVehicle::InitializeNPCStruct(VehicleStruct& aVehicleStruct,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition,
    const NPCType aNPCType, Environment const* aEnvironment)
{
    aVehicleStruct.vehicleData.npcType = aNPCType;
    aVehicleStruct.environment = aEnvironment;
    aVehicleStruct.vehicleData.q.position = aPosition;
    aVehicleStruct.vehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleStruct.vehicleData.q.angularMomentum = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.q.angularVelocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleStruct.vehicleData.dragCoefficient = 0.2f;

    DirectX::SimpleMath::Vector3 dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 10.0f);
    aVehicleStruct.vehicleData.dimensions = dimensions;
    DirectX::SimpleMath::Vector3 collisionDimensions = DirectX::SimpleMath::Vector3(16.0f, 8.0f, 10.0f);
    aVehicleStruct.vehicleData.collisionBox.Extents = collisionDimensions * 0.5f;
    
    aVehicleStruct.vehicleData.collisionBox.Center = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.collisionBox.Orientation = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 cornerVertex = aVehicleStruct.vehicleData.dimensions;
    cornerVertex.x *= 0.5f;
    cornerVertex.y *= 0.5f;
    cornerVertex.z *= 0.5f;
    // set max collision detection range slightly larger than box radius  
    aVehicleStruct.vehicleData.maxCollisionDetectionRange = (cornerVertex - aVehicleStruct.vehicleData.collisionBox.Center).Length() * 1.2f;

    aVehicleStruct.vehicleData.isCollisionTrue = false;

    aVehicleStruct.vehicleData.frontalArea = aVehicleStruct.vehicleData.dimensions.z * aVehicleStruct.vehicleData.dimensions.y;
    aVehicleStruct.vehicleData.hitPoints = 100.0f;

    aVehicleStruct.vehicleData.terrainHightAtPos = 0.0f;
    aVehicleStruct.vehicleData.altitude = 0.0f;
    aVehicleStruct.vehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.groundPlane.x = 0.0f;
    aVehicleStruct.vehicleData.groundPlane.y = -1.0f;
    aVehicleStruct.vehicleData.groundPlane.z = 0.0f;
    aVehicleStruct.vehicleData.groundPlane.w = 0.5f;

    aVehicleStruct.vehicleData.time = 0.0f;
    aVehicleStruct.vehicleData.forward = aHeading;
    aVehicleStruct.vehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.right = aVehicleStruct.vehicleData.forward.Cross(aVehicleStruct.vehicleData.up);
    aVehicleStruct.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicleStruct.vehicleData.right, aVehicleStruct.vehicleData.up);
    aVehicleStruct.vehicleData.alignmentInverse = aVehicleStruct.vehicleData.alignment;
    aVehicleStruct.vehicleData.alignmentInverse = aVehicleStruct.vehicleData.alignmentInverse.Invert();
    aVehicleStruct.vehicleData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(aVehicleStruct.vehicleData.alignment);
    aVehicleStruct.vehicleData.alignmentQuat.Normalize();
    aVehicleStruct.vehicleData.alignmentInverseQuat = aVehicleStruct.vehicleData.alignmentQuat;
    aVehicleStruct.vehicleData.alignmentInverseQuat.Inverse(aVehicleStruct.vehicleData.alignmentInverseQuat);

    // zero out impact force
    aVehicleStruct.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.impactForce.impactMass = 0.0f;
    aVehicleStruct.vehicleData.impactForceVec.clear();
    // zero out impact torque
    aVehicleStruct.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.impactTorque.magnitude = 0.0f;
    aVehicleStruct.vehicleData.impactTorqueVec.clear();
    aVehicleStruct.vehicleData.impulseForceVec.clear();

    aVehicleStruct.vehicleData.collisionImpulseTorqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.collisionImpulseTorqueSum.magnitude = 0.0f;

    aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.centerOfMassPos = aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos;
    aVehicleStruct.vehicleData.hardPoints.localWeaponDirection = DirectX::SimpleMath::Vector3::UnitX;
    aVehicleStruct.vehicleData.hardPoints.weaponDirection = aVehicleStruct.vehicleData.hardPoints.localWeaponDirection;
    aVehicleStruct.vehicleData.hardPoints.localWeaponPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.weaponPos = aVehicleStruct.vehicleData.hardPoints.localWeaponPos;

    aVehicleStruct.vehicleData.hardPoints.localVerticalStabilizerPos = DirectX::SimpleMath::Vector3(0.0f, aVehicleStruct.vehicleData.dimensions.y, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.verticalStabilizerPos = aVehicleStruct.vehicleData.hardPoints.localVerticalStabilizerPos;
    aVehicleStruct.vehicleData.hardPoints.localSteeringTorqueArmPos = DirectX::SimpleMath::Vector3(aVehicleStruct.vehicleData.dimensions.x * 0.5f, 0.0f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.steeringTorqueArmPos = aVehicleStruct.vehicleData.hardPoints.localSteeringTorqueArmPos;

    aVehicleStruct.vehicleData.hardPoints.localBasePos = DirectX::SimpleMath::Vector3(0.0f, -aVehicleStruct.vehicleData.dimensions.y * 0.5f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.basePos = aVehicleStruct.vehicleData.hardPoints.localBasePos;

    aVehicleStruct.vehicleData.hoverData.forwardThrust = 0.0f;
    aVehicleStruct.vehicleData.hoverData.omniThrust = 0.0f;
    aVehicleStruct.vehicleData.hoverData.omniThrustVec = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hoverData.hoverLiftNeutralWithGrav = -aEnvironment->GetGravityVec();
    aVehicleStruct.vehicleData.hoverData.hoverLiftMax = -aEnvironment->GetGravityVec() * 1.2f;
    aVehicleStruct.vehicleData.hoverData.hoverLiftCurrent = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hoverData.turnRateCurrent = 0.0f;
    aVehicleStruct.vehicleData.hoverData.turnRateMax = 3.0f;

    aVehicleStruct.vehicleData.controlInput.angleToDestination = 0.0f;
    aVehicleStruct.vehicleData.controlInput.omniDirection = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.controlInput.omniThrust = 0.0f;
    aVehicleStruct.vehicleData.controlInput.stearingIsPressed = false;
    aVehicleStruct.vehicleData.controlInput.steeringInput = 0.0f;
    aVehicleStruct.vehicleData.controlInput.steeringVec = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.controlInput.throttleInput = 0.0f;

    aVehicleStruct.vehicleData.playerPos = DirectX::SimpleMath::Vector3::Zero;

    const float xExtent = aVehicleStruct.vehicleData.tensorDimensions.x;
    const float yExtent = aVehicleStruct.vehicleData.tensorDimensions.y;
    const float zExtent = aVehicleStruct.vehicleData.tensorDimensions.z;
    const float mass = aVehicleStruct.vehicleData.tensorMass;
    // cuboid
    
    aVehicleStruct.vehicleData.localInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    aVehicleStruct.vehicleData.localInertiaMatrix._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    aVehicleStruct.vehicleData.localInertiaMatrix._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    aVehicleStruct.vehicleData.localInertiaMatrix._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));
    
    /*
    float radius = 10.0f;
    aVehicleStruct.vehicleData.localInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    aVehicleStruct.vehicleData.localInertiaMatrix._11 = (2.0f / 3.0f) * (mass) * (radius * radius);
    aVehicleStruct.vehicleData.localInertiaMatrix._22 = (2.0f / 3.0f) * (mass) * (radius * radius);
    aVehicleStruct.vehicleData.localInertiaMatrix._33 = (2.0f / 3.0f) * (mass) * (radius * radius);
    */
    aVehicleStruct.vehicleData.localInverseInertiaMatrix = aVehicleStruct.vehicleData.localInertiaMatrix;
    aVehicleStruct.vehicleData.localInverseInertiaMatrix = aVehicleStruct.vehicleData.localInverseInertiaMatrix.Invert();
}

void NPCVehicle::InitializeTextureMaps(NpcTextureMapType aTextureMapType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap)
{
    if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_BLANK)
    {
        m_vehicleStruct00.npcModel.textureBlank = aTexture;
        m_vehicleStruct00.npcModel.normalMapBlank = aNormalMap;
        m_vehicleStruct00.npcModel.specularBlank = aSpecularMap;
    }
    else if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_FLAME)
    {
        m_vehicleStruct00.npcModel.textureFlame = aTexture;
        m_vehicleStruct00.npcModel.normalMapFlame = aNormalMap;
        m_vehicleStruct00.npcModel.specularFlame = aSpecularMap;
    }
    else if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_TEST1)
    {
        m_vehicleStruct00.npcModel.textureTest1 = aTexture;
        m_vehicleStruct00.npcModel.normalMapTest1 = aNormalMap;
        m_vehicleStruct00.npcModel.specularTest1 = aSpecularMap;
    }
    else if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_TEST2)
    {
        m_vehicleStruct00.npcModel.textureTest2 = aTexture;
        m_vehicleStruct00.npcModel.normalMapTest2 = aNormalMap;
        m_vehicleStruct00.npcModel.specularTest2 = aSpecularMap;
    }
}

void NPCVehicle::InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment,
    std::shared_ptr<NPCController> aNpcController, std::shared_ptr<Vehicle> aPlayer, const unsigned int aID)
{
    m_context = aContext;
    m_environment = aEnvironment;
    m_npcController = aNpcController;
    m_vehicleStruct00.vehicleData.id = aID;
    
    const float low = -1.0f;
    const float high = 1.0f;
    m_testHoverFlutter = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    InitializeNPCStruct(m_vehicleStruct00, aHeading, aPosition, this->GetNPCType(), aEnvironment);
    CalculateTopSpeed();
    InitializeNPCModelStruct(aContext, m_vehicleStruct00.npcModel, m_vehicleStruct00.vehicleData.hardPoints, m_vehicleStruct00.vehicleData.dimensions);

    if (this->GetNPCType() == NPCType::NPCTYPE_SPAWNEDALT)
    {
        m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData, aNpcController, true);
    }
    else
    {
        m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData, aNpcController, false);
    }
}

void NPCVehicle::InitializeNPCVehicleWithAudio(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment,
    std::shared_ptr<NPCController> aNpcController, std::shared_ptr<Vehicle> aPlayer, std::shared_ptr<Utility::SoundFx> aAudioFx, const unsigned int aID)
{
    m_context = aContext;
    m_environment = aEnvironment;
    m_npcController = aNpcController;
    m_vehicleStruct00.vehicleData.id = aID;

    m_vehicleStruct00.audioFx = aAudioFx;

    const float low = -1.0f;
    const float high = 1.0f;
    m_testHoverFlutter = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    InitializeNPCStruct(m_vehicleStruct00, aHeading, aPosition, this->GetNPCType(), aEnvironment);
    CalculateTopSpeed();
    InitializeNPCModelStruct(aContext, m_vehicleStruct00.npcModel, m_vehicleStruct00.vehicleData.hardPoints, m_vehicleStruct00.vehicleData.dimensions);

    if (this->GetNPCType() == NPCType::NPCTYPE_SPAWNEDALT)
    {
        m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData, aNpcController, true);
    }
    else
    {
        m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData, aNpcController, false);
    }
}

void NPCVehicle::PushImpactTorque(Utility::Torque aTorque)
{
    aTorque.axis.Normalize();
    m_vehicleStruct00.vehicleData.impactTorqueVec.push_back(aTorque);
}

void NPCVehicle::PushImpulseForce(Utility::ImpulseForce aImpulse)
{
    m_vehicleStruct00.vehicleData.impulseForceVec.push_back(aImpulse);
    if (aImpulse.impulseType == Utility::ImpulseType::IMPULSETYPE_FRONTLOADCURVE)
    {
        //m_isDebugPauseToggleTrue = true;
        //m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, DirectX::SimpleMath::Vector3::UnitY, 20.0f, 0.0f, DirectX::Colors::Red);
    }
}

void NPCVehicle::RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    //  Compute the total drag force
    const float v = newQ.velocity.Length();
    const float airDensity = m_environment->GetAirDensity();

    float dragCoefficient = aVehicle->dragCoefficient;
    float frontSurfaceArea = aVehicle->frontalArea;
    const float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;
    float mass = aVehicle->mass;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    /*
    DirectX::SimpleMath::Vector3 impactForceSum = m_vehicleStruct00.vehicleData.impactForceSum;

    DirectX::SimpleMath::Vector3 angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Zero;

    velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += impactForceSum;
    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        float jumpMag = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude;
        velocityUpdate += DirectX::SimpleMath::Vector3::UnitY * jumpMag * mass;
    }
    if (m_vehicleStruct00.vehicleData.jumpData.isJumpActive == false)
    {
        velocityUpdate += m_buoyancyTestForce;
    }
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    velocityUpdate += gravForce * (mass);

    DirectX::SimpleMath::Vector3 forwardThrustUpdate = GetForwardThrust(m_vehicleStruct00.vehicleData);
    forwardThrustUpdate.y = 0.0f;
    velocityUpdate += forwardThrustUpdate;

    airResistance = velocityNorm * (-frontDragResistance);
    airResistance.y = 0.0f;
    velocityUpdate += airResistance;

    // adds some floating flutter movement to vehicles when stationary 
    if (m_npcAI->GetAiToggle() == false)
    {
        DirectX::SimpleMath::Vector3 flutter = DirectX::SimpleMath::Vector3::Zero;
        float flutterMod = (((cos((static_cast<float>(m_testTimer) * 2.0f) + m_testHoverFlutter) * 0.5f) + 0.0f) * 1.0f) + 1.0f;
        flutter.y = flutterMod * 7.0f;
        //velocityUpdate += flutter * mass;
    }

    velocityUpdate += m_vehicleStruct00.vehicleData.collisionImpulseForceSum;
    */

    velocityUpdate = m_vehicleStruct00.vehicleData.vehicleLinearForcesSum;

    airResistance = velocityNorm * (-frontDragResistance);
    airResistance.y = 0.0f;
    velocityUpdate += airResistance;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    /////////////////////////////////////////
    // Angular
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;
    DirectX::SimpleMath::Vector3 torqueAccum = m_vehicleStruct00.vehicleData.vehicleAngularForcesSum;
    //torqueAccum = DirectX::SimpleMath::Vector3::UnitX * m_testTorque;
    torqueAccum += newQ.angularVelocity;
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_vehicleStruct00.vehicleData.localInverseInertiaMatrix);

    //torqueAccum += newQ.angularVelocity;
    torqueAccum += (m_vehicleStruct00.vehicleData.angularDrag);
    //torqueAccum *= 0.8f;
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
}

void NPCVehicle::RightHandSide2(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    //  Compute the total drag force
    const float v = newQ.velocity.Length();
    const float airDensity = m_environment->GetAirDensity();

    float dragCoefficient = aVehicle->dragCoefficient;
    float frontSurfaceArea = aVehicle->frontalArea;
    const float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;
    float mass = aVehicle->mass;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 impactForceSum = m_vehicleStruct00.vehicleData.impactForceSum;

    DirectX::SimpleMath::Vector3 angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Zero;

    velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += impactForceSum;
    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        float jumpMag = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude;
        velocityUpdate += DirectX::SimpleMath::Vector3::UnitY * jumpMag * mass;
    }
    if (m_vehicleStruct00.vehicleData.jumpData.isJumpActive == false)
    {
        velocityUpdate += m_buoyancyTestForce;
    }
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    velocityUpdate += gravForce * (mass);

    DirectX::SimpleMath::Vector3 forwardThrustUpdate = GetForwardThrust(m_vehicleStruct00.vehicleData);
    forwardThrustUpdate.y = 0.0f;
    velocityUpdate += forwardThrustUpdate;

    airResistance = velocityNorm * (-frontDragResistance);
    airResistance.y = 0.0f;
    velocityUpdate += airResistance;

    // adds some floating flutter movement to vehicles when stationary 
    if (m_npcAI->GetAiToggle() == false)
    {
        DirectX::SimpleMath::Vector3 flutter = DirectX::SimpleMath::Vector3::Zero;
        float flutterMod = (((cos((static_cast<float>(m_testTimer) * 2.0f) + m_testHoverFlutter) * 0.5f) + 0.0f) * 1.0f) + 1.0f;
        flutter.y = flutterMod * 7.0f;
        //velocityUpdate += flutter * mass;
    }

    velocityUpdate += m_vehicleStruct00.vehicleData.collisionImpulseForceSum;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    /////////////////////////////////////////
    // Angular
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;
    DirectX::SimpleMath::Vector3 torqueAccum = m_vehicleStruct00.vehicleData.vehicleAngularForcesSum;
    //torqueAccum = DirectX::SimpleMath::Vector3::UnitX * m_testTorque;
    torqueAccum += newQ.angularVelocity;
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_vehicleStruct00.vehicleData.localInverseInertiaMatrix);

    //torqueAccum += newQ.angularVelocity;
    torqueAccum += (m_vehicleStruct00.vehicleData.angularDrag);
    //torqueAccum *= 0.8f;
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
}

void NPCVehicle::RungeKutta4(struct VehicleData* aVehicle, double aTimeDelta)
{
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    MotionNPC q = aVehicle->q;
    MotionNPC dq1;
    MotionNPC dq2;
    MotionNPC dq3;
    MotionNPC dq4;

    DirectX::SimpleMath::Vector3 preAngVelocity = aVehicle->q.angularVelocity;
    DirectX::SimpleMath::Vector3 preAngPos = aVehicle->q.angularMomentum;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aVehicle, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aVehicle, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aVehicle, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aVehicle, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aVehicle->time = aVehicle->time + static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;

    q.angularMomentum += angularMomentumUpdate;
    q.angularVelocity += angularVelocityUpdate;

    m_prevYvelocityStep = velocityUpdate.y;
    m_prevYvelocityQ = q.velocity.y;

    aVehicle->q.velocity = q.velocity;
    aVehicle->q.position = q.position;
    aVehicle->q.angularVelocity = q.angularVelocity;
    aVehicle->q.angularMomentum = q.angularMomentum;
}

void NPCVehicle::SetCollisionVal(const bool aIsCollisionTrue)
{
    m_vehicleStruct00.vehicleData.isCollisionTrue = aIsCollisionTrue;
}

void NPCVehicle::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void NPCVehicle::SetIsPlayerTargetedTrue(const DirectX::SimpleMath::Vector3 aPos)
{
    m_vehicleStruct00.vehicleData.isPlayerTargetedTrue = true; 
    auto laserPos = aPos;
    m_vehicleStruct00.npcModel.laserLightPosVec.push_back(laserPos);
}

void NPCVehicle::SetIsMissileTargetedTrue(const DirectX::SimpleMath::Vector3 aPos)
{
    m_vehicleStruct00.vehicleData.isMissileTargetedTrue = true;
    auto laserPos = aPos;
    m_vehicleStruct00.npcModel.laserLightPosVec.push_back(laserPos);
}

void NPCVehicle::SetNpcType(NPCType aNPCType)
{
    m_vehicleStruct00.vehicleData.npcType = aNPCType;
}

void NPCVehicle::TerrainImpactHandling()
{
    DirectX::SimpleMath::Vector3 v = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 n = m_vehicleStruct00.vehicleData.terrainNormal;
    const float b = 0.9f;

    DirectX::SimpleMath::Vector3 postImpactVelocity = b * (-2.0f * (v.Dot(n)) * n + v);

    m_vehicleStruct00.vehicleData.q.velocity = postImpactVelocity;
}

void NPCVehicle::ToggleDebugBool()
{
    if (m_isDebugToggleTrue == true)
    {
        m_isDebugToggleTrue = false;
    }
    else
    {
        m_isDebugToggleTrue = true;
    }
}

void NPCVehicle::UpdateAlignmentNew(const float aTimeDelta)
{
    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = m_vehicleStruct00.vehicleData.q.angularVelocity.x;
    updateAlignQuat.y = m_vehicleStruct00.vehicleData.q.angularVelocity.y;
    updateAlignQuat.z = m_vehicleStruct00.vehicleData.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = m_vehicleStruct00.vehicleData.alignmentQuat;
    updateAlignOutputQuat.Normalize();
    //m_heli.q.angularQuatUpdateTest.Normalize();
    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * m_vehicleStruct00.vehicleData.alignmentQuat);
    updateAlignOutputQuat.Normalize();
    m_vehicleStruct00.vehicleData.alignmentQuat = updateAlignOutputQuat;

    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_vehicleStruct00.vehicleData.alignmentQuat);

    //////////////////////////////////
    /*
    DirectX::SimpleMath::Vector3 headingForward = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 headingUp = DirectX::SimpleMath::Vector3::UnitY;
    float testVal = cos(m_testTimer);
    float yaw = 0.0f;
    float pitch = testVal;
    float roll = 0.0f;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
    headingForward = DirectX::SimpleMath::Vector3::Transform(headingForward, rotMat);
    headingUp = DirectX::SimpleMath::Vector3::Transform(headingUp, rotMat);
    m_vehicleStruct00.vehicleData.forward = headingForward;
    m_vehicleStruct00.vehicleData.up = headingUp;
    m_vehicleStruct00.vehicleData.right = m_vehicleStruct00.vehicleData.forward.Cross(m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    */
    /////////////////////////////////

    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignment;
    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignmentInverse.Invert();

    m_vehicleStruct00.vehicleData.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_vehicleStruct00.vehicleData.alignment);

    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignment;
    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignmentInverse.Invert();
    m_vehicleStruct00.vehicleData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.alignmentQuat.Normalize();
    m_vehicleStruct00.vehicleData.alignmentInverseQuat = m_vehicleStruct00.vehicleData.alignmentQuat;
    m_vehicleStruct00.vehicleData.alignmentInverseQuat.Inverse(m_vehicleStruct00.vehicleData.alignmentInverseQuat);
    m_vehicleStruct00.vehicleData.alignmentInverseQuat.Normalize();

    m_vehicleStruct00.vehicleData.collisionBox.Center = m_vehicleStruct00.vehicleData.q.position;
    m_vehicleStruct00.vehicleData.collisionBox.Orientation = m_vehicleStruct00.vehicleData.alignmentQuat;
}


void NPCVehicle::UpdateAlignment()
{
    DirectX::SimpleMath::Matrix preAlignment = m_vehicleStruct00.vehicleData.alignment;
    DirectX::SimpleMath::Quaternion preAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    DirectX::SimpleMath::Matrix torqueMat;

    DirectX::SimpleMath::Vector3 testAngVec = m_vehicleStruct00.vehicleData.q.angularVelocity;

    if (testAngVec != DirectX::SimpleMath::Vector3::Zero)
    {
        //DirectX::SimpleMath::Vector3 axis = m_vehicleStruct00.vehicleData.q.angularVelocityVec;
        DirectX::SimpleMath::Vector3 axis = testAngVec;
        //DirectX::SimpleMath::Vector3 axis = m_vehicleStruct00.vehicleData.q.angPosVec;
        axis.Normalize();
        //DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, m_vehicleStruct00.vehicleData.q.angularVelocityVec.Length());
        DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, testAngVec.Length());
        //DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, m_vehicleStruct00.vehicleData.q.angPosVec.Length());
        testQuat.Normalize();
        m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::Transform(m_vehicleStruct00.vehicleData.alignment, testQuat);
        
    }

    m_vehicleStruct00.vehicleData.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.up.Normalize();
    m_vehicleStruct00.vehicleData.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.right.Normalize();
    m_vehicleStruct00.vehicleData.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.forward.Normalize();

    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignment;
    m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignmentInverse.Invert();
    m_vehicleStruct00.vehicleData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.alignmentQuat.Normalize();
    m_vehicleStruct00.vehicleData.alignmentInverseQuat = m_vehicleStruct00.vehicleData.alignmentQuat;
    m_vehicleStruct00.vehicleData.alignmentInverseQuat.Inverse(m_vehicleStruct00.vehicleData.alignmentInverseQuat);
    m_vehicleStruct00.vehicleData.alignmentInverseQuat.Normalize();

    DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    updateQuat.Normalize();
    m_vehicleStruct00.vehicleData.collisionBox.Orientation = updateQuat;
}

void NPCVehicle::UpdateAngularDrag(const float aTimeDelta)
{
    const float angAirDensity = m_environment->GetAirDensity();
    const float angDragCoefficient = m_vehicleStruct00.vehicleData.angularDragCoefficient;
    const float angFrontSurfaceArea = m_vehicleStruct00.vehicleData.frontalArea;
    //const float angVelocity = newQ.angularVelocityVec.Length();
    const float angVelocityF = m_vehicleStruct00.vehicleData.q.angularVelocity.Length();
    const float angFrontDragResistance = 0.5f * angAirDensity * angFrontSurfaceArea * angDragCoefficient * angVelocityF * angVelocityF;
    //DirectX::SimpleMath::Vector3 angVelNormVec = aVehicle->q.angularVelocityVec;
    //DirectX::SimpleMath::Vector3 angVelNormVec = newQ.angularVelocityVec;
    DirectX::SimpleMath::Vector3 angVelNormVec = m_vehicleStruct00.vehicleData.q.angularVelocity;
    angVelNormVec.Normalize();
    DirectX::SimpleMath::Vector3 angDampeningVec = angVelNormVec * (-angFrontDragResistance);

    DirectX::SimpleMath::Vector3 testAngDampVec1 = CalculateDragAngular(m_vehicleStruct00.vehicleData.q.angularVelocity);
    DirectX::SimpleMath::Vector3 testAngDampVec2 = CalculateDragAngular2(m_vehicleStruct00.vehicleData.q.angularVelocity);
    DirectX::SimpleMath::Vector3 testAngDampVec3 = CalculateDragAngular3(m_vehicleStruct00.vehicleData.q.angularVelocity);

    const float modVal = aTimeDelta;
    testAngDampVec1 *= modVal;
    testAngDampVec2 *= modVal;
    testAngDampVec3 *= modVal;

    /*
    m_debugData->DebugPushUILineDecimalNumber("angDampeningVec = ", angDampeningVec.Length(), "");
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, angDampeningVec, 20.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->DebugPushUILineDecimalNumber("testAngDampVec1 = ", testAngDampVec1.Length(), "");
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, testAngDampVec1, 18.0f, 0.0f, DirectX::Colors::Pink);
    m_debugData->DebugPushUILineDecimalNumber("testAngDampVec2 = ", testAngDampVec2.Length(), "");
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, testAngDampVec2, 16.0f, 0.0f, DirectX::Colors::Teal);
    m_debugData->DebugPushUILineDecimalNumber("testAngDampVec3 = ", testAngDampVec3.Length(), "");
    m_debugData->PushDebugLine(m_vehicleStruct00.vehicleData.q.position, testAngDampVec3, 14.0f, 0.0f, DirectX::Colors::Lavender);

    m_debugData->DebugPushUILineDecimalNumber("Currant ang Vel    = ", m_vehicleStruct00.vehicleData.q.angularVelocityVec.Length(), "");
    */
}

void NPCVehicle::UpdateAudio()
{
    m_vehicleStruct00.audioFx->volume = m_vehicleStruct00.vehicleData.audioThrottle;
    m_vehicleStruct00.audioFx->fx->SetVolume(m_vehicleStruct00.vehicleData.audioThrottle);
}

DirectX::SimpleMath::Vector3 NPCVehicle::UpdateBodyTorqueRungeLocalNew(const float aTimeStep)
{
    Utility::Torque impactTorque;
    impactTorque.axis = m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.axis;
    impactTorque.magnitude = m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.magnitude;
    impactTorque.axis = DirectX::SimpleMath::Vector3::Transform(impactTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);

    Utility::Torque gravTorque = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos, -m_vehicleStruct00.environment->GetGravityVec());
    Utility::Torque steeringTorque = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos, -m_vehicleStruct00.vehicleData.right * (m_vehicleStruct00.vehicleData.controlInput.steeringInput));
    gravTorque.axis = DirectX::SimpleMath::Vector3::Transform(gravTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);
    steeringTorque.axis = DirectX::SimpleMath::Vector3::Transform(steeringTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);
    steeringTorque.magnitude *= m_vehicleStruct00.vehicleData.controlInput.steeringForceMod;
    gravTorque.magnitude *= m_vehicleStruct00.vehicleData.controlInput.steeringForceMod;
    /*
    if (steeringTorque.magnitude > Utility::GetMaxAngularForce())
    {
        steeringTorque.magnitude = Utility::GetMaxAngularForce();
    }
    if (gravTorque.magnitude > Utility::GetMaxAngularForce())
    {
        gravTorque.magnitude = Utility::GetMaxAngularForce();
    }
    if (impactTorque.magnitude > Utility::GetMaxAngularForce())
    {
        impactTorque.magnitude = Utility::GetMaxAngularForce();
    }
    */

    DirectX::SimpleMath::Vector3 steeringVec = steeringTorque.axis * steeringTorque.magnitude;
    DirectX::SimpleMath::Vector3 gravVec = gravTorque.axis * gravTorque.magnitude;
    DirectX::SimpleMath::Vector3 impactVec = impactTorque.axis * impactTorque.magnitude;
    DirectX::SimpleMath::Vector3 torqueVec = steeringVec + gravVec + impactVec;
    //torqueVec = steeringVec + gravVec + impactVec + m_testTorqueVec;
    torqueVec = steeringVec + gravVec + impactVec;
    //torqueVec = steeringVec + gravVec;
    //torqueVec = steeringVec + gravVec + m_testTorqueVec;
    if (m_vehicleStruct00.vehicleData.isExploding == true)
    {
        torqueVec = steeringVec + impactVec;
        torqueVec = steeringVec + gravVec + impactVec;
        //torqueVec = steeringVec + m_testTorqueVec;
        //torqueVec = steeringVec + gravVec + impactVec + m_testTorqueVec;
        torqueVec = steeringVec + gravVec + impactVec;
    }

    //aAngVec = torqueVec;

    return torqueVec;
}

DirectX::SimpleMath::Vector3 NPCVehicle::UpdateBodyTorqueRungeLocalOld(const float aTimeStep)
{
    Utility::Torque impactTorque;
    impactTorque.axis = m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.axis;
    impactTorque.magnitude = m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.magnitude;
    impactTorque.axis = DirectX::SimpleMath::Vector3::Transform(impactTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);
    //impactTorque.magnitude *= .0000392f;
    Utility::Torque gravTorque = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos, -m_vehicleStruct00.environment->GetGravityVec());
    Utility::Torque steeringTorque = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos, -m_vehicleStruct00.vehicleData.right * (m_vehicleStruct00.vehicleData.controlInput.steeringInput));
    gravTorque.axis = DirectX::SimpleMath::Vector3::Transform(gravTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);
    steeringTorque.axis = DirectX::SimpleMath::Vector3::Transform(steeringTorque.axis, m_vehicleStruct00.vehicleData.alignmentInverse);
    steeringTorque.magnitude *= m_vehicleStruct00.vehicleData.controlInput.steeringForceMod;
    gravTorque.magnitude *= m_vehicleStruct00.vehicleData.controlInput.steeringForceMod;
    if (steeringTorque.magnitude > Utility::GetMaxAngularForce())
    {
        steeringTorque.magnitude = Utility::GetMaxAngularForce();
    }
    if (gravTorque.magnitude > Utility::GetMaxAngularForce())
    {
        gravTorque.magnitude = Utility::GetMaxAngularForce();
    }
    if (impactTorque.magnitude > Utility::GetMaxAngularForce())
    {
        impactTorque.magnitude = Utility::GetMaxAngularForce();
    }

    DirectX::SimpleMath::Vector3 steeringVec = steeringTorque.axis * steeringTorque.magnitude;
    DirectX::SimpleMath::Vector3 gravVec = gravTorque.axis * gravTorque.magnitude;
    DirectX::SimpleMath::Vector3 impactVec = impactTorque.axis * impactTorque.magnitude;
    DirectX::SimpleMath::Vector3 torqueVec = steeringVec + gravVec + impactVec;
    //torqueVec = steeringVec + gravVec + impactVec + m_testTorqueVec;
    torqueVec = steeringVec + gravVec + impactVec;
    //torqueVec = steeringVec + gravVec + m_testTorqueVec;
    if (m_vehicleStruct00.vehicleData.isExploding == true)
    {
        torqueVec = steeringVec + impactVec;
        torqueVec = steeringVec + gravVec + impactVec;
        //torqueVec = steeringVec + m_testTorqueVec;
        //torqueVec = steeringVec + gravVec + impactVec + m_testTorqueVec;
        torqueVec = steeringVec + gravVec + impactVec;
    }

    //aAngVec = torqueVec;

    return torqueVec;
}

void NPCVehicle::UpdateControlInput()
{
    m_vehicleStruct00.vehicleData.controlInput.steeringVec = m_npcAI->GetVecToDestination();
    m_vehicleStruct00.vehicleData.controlInput.angleToDestination = m_npcAI->GetAngleToDestination(m_vehicleStruct00.vehicleData.forward, m_vehicleStruct00.vehicleData.up, m_vehicleStruct00.vehicleData.playerPos);

    const float yawInput = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;
    const float updatedYaw = (yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    if (updatedYaw > m_vehicleStruct00.vehicleData.controlInput.steeringInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.steeringInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMin;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.inputDeadZone && updatedYaw > -m_vehicleStruct00.vehicleData.controlInput.inputDeadZone)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }

    // velocity
    float absAngleToDest = abs(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    float rawThrottleInput = m_npcAI->GetThrottleInput();
    float modThrottleInput = rawThrottleInput - absAngleToDest;
    if (modThrottleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = modThrottleInput;
    }

    m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
}

void NPCVehicle::UpdateControlInputFromAi()
{
    AIOutput aiInput = m_npcAI->GetAiControlOutput();

    if (aiInput.isTriggerJumpTrue == true)
    {
        bool testActivate = ActivateJump();
        if (testActivate == false)
        {
            //ToDo: error handle and check why jump failed
        }
    }

    m_vehicleStruct00.vehicleData.controlInput.steeringVec = aiInput.steeringDirection;
    m_vehicleStruct00.vehicleData.controlInput.angleToDestination = aiInput.angleToDestination;

    const float yawInput = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;
    float updatedYaw;
    if (abs(yawInput) < abs(m_vehicleStruct00.vehicleData.controlInput.steeringInput))
    {
        updatedYaw = yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate;
    }
    else
    {
        updatedYaw = (yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    }

    if (updatedYaw > m_vehicleStruct00.vehicleData.controlInput.steeringInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.steeringInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMin;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.inputDeadZone && updatedYaw > -m_vehicleStruct00.vehicleData.controlInput.inputDeadZone)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }

    float aiForwardThrottle = aiInput.forwardThrust;
    float updateThrottleDelta = aiForwardThrottle - m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    float updateThrottle = 0.0f;
    if (abs(updateThrottleDelta) > m_vehicleStruct00.vehicleData.controlInput.throttleInputRate)
    {
        if (updateThrottleDelta > 0.0f)
        {
            updateThrottle = m_vehicleStruct00.vehicleData.controlInput.throttleInputRate;
        }
        else
        {
            updateThrottle = -m_vehicleStruct00.vehicleData.controlInput.throttleInputRate;
        }
    }
    else
    {
        updateThrottle = updateThrottleDelta;
    }
    float throttleSum = m_vehicleStruct00.vehicleData.controlInput.throttleInput + updateThrottle;
    if (throttleSum > m_vehicleStruct00.vehicleData.controlInput.throttleInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_vehicleStruct00.vehicleData.controlInput.throttleInputMax;
    }
    else if (throttleSum < m_vehicleStruct00.vehicleData.controlInput.throttleInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_vehicleStruct00.vehicleData.controlInput.throttleInputMin;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput += updateThrottle;
    }

    if (m_vehicleStruct00.vehicleData.controlInput.throttleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.reverseThrustMax;
    }
    else
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
    }
   
    //m_debugData->DebugPushUILineDecimalNumber("m_vehicleStruct00.vehicleData.controlInput.throttleInput", m_vehicleStruct00.vehicleData.controlInput.throttleInput, "");
    //m_debugData->DebugPushUILineDecimalNumber("m_vehicleStruct00.vehicleData.controlInput.forwardThrust", m_vehicleStruct00.vehicleData.hoverData.forwardThrust, "");

    // omni trust
    m_vehicleStruct00.vehicleData.controlInput.omniDirection = aiInput.omniDirection;
    m_vehicleStruct00.vehicleData.controlInput.omniThrust = aiInput.omniThrust * m_vehicleStruct00.vehicleData.hoverData.omniThrustMax;
    m_vehicleStruct00.vehicleData.hoverData.omniThrustVec = m_vehicleStruct00.vehicleData.controlInput.omniDirection * m_vehicleStruct00.vehicleData.controlInput.omniThrust;
}

void NPCVehicle::UpdateForceTorqueVecs() // update when force and torque over time is implemented
{
    m_vehicleStruct00.vehicleData.impactForceVec.clear();
    m_vehicleStruct00.vehicleData.impactTorqueVec.clear();
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;
    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::UnitY;

    m_testTorqueVec = DirectX::SimpleMath::Vector3::Zero;
}

void NPCVehicle::UpdateHardPoints()
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    DirectX::SimpleMath::Matrix updateDirectionMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);

    m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localCenterOfMassPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponDirection = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.vehicleData.hardPoints.localWeaponDirection, updateDirectionMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localWeaponPos, updateMat);

    m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localVerticalStabilizerPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localSteeringTorqueArmPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.basePos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localBasePos, updateMat);

    m_vehicleStruct00.vehicleData.hardPoints.jetArmCenterAxis = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localJetArmCenterAxis, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.leftJetAxis = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localLeftJetAxis, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.rightJetAxis = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localRightJetAxis, updateMat);

    //DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    //m_vehicleStruct00.vehicleData.collisionBox.Orientation = updateQuat;
}

void NPCVehicle::UpdateImpulseForces(const float aTimeDelta)
{
    m_vehicleStruct00.vehicleData.collisionImpulseForceSum = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum.magnitude = 0.0f;

    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque torqueSum;
    torqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    torqueSum.magnitude = 0.0f;
    for (int i = 0; i < m_vehicleStruct00.vehicleData.impulseForceVec.size(); ++i)
    {
        Utility::ImpulseForce testImpulse2 = m_vehicleStruct00.vehicleData.impulseForceVec[i];
        Utility::UpdateImpulseForceBellCurve2(testImpulse2, static_cast<float>(aTimeDelta));

        //Utility::UpdateImpulseForceBellCurve2(m_vehicleStruct00.vehicleData.impulseForceVec[i], static_cast<float>(aTimeDelta));
        //Utility::ImpulseForce testImpulse = m_vehicleStruct00.vehicleData.impulseForceVec[i];

        //Utility::UpdateImpulseForceCurve(m_vehicleStruct00.vehicleData.impulseForceVec[i], static_cast<float>(aTimeDelta));
        Utility::UpdateImpulseForceBellCurve(m_vehicleStruct00.vehicleData.impulseForceVec[i], static_cast<float>(aTimeDelta));
        //Utility::UpdateImpulseForceCurveProjectile(m_vehicleStruct00.vehicleData.impulseForceVec[i], static_cast<float>(aTimeDelta));
        Utility::ImpulseForce testImpulse = m_vehicleStruct00.vehicleData.impulseForceVec[i];

        if (m_vehicleStruct00.vehicleData.impulseForceVec[i].isActive == true)
        {
            //m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude *= aTimeDelta;
            forceSum += (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm) * 1.0f;

            //Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm, (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm));
            //Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm, (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueForceNorm));
            Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm, (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentTorqueMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueForceNorm));

            torqueSum.axis += torqueImpulse.axis * torqueImpulse.magnitude;
            torqueSum.magnitude += torqueImpulse.magnitude;

            /////////////////////////////////////////////////

            DirectX::SimpleMath::Vector3 testAxis = torqueImpulse.axis;
            testAxis.Normalize();
            m_testTorqueVec += (testAxis * torqueImpulse.magnitude);
            //m_isDebugPauseToggleTrue = true;

            DirectX::SimpleMath::Vector3 directionNorm = m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm;
            DirectX::SimpleMath::Vector3 torqueArm = m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm;
            DirectX::SimpleMath::Vector3 forcePoint = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos + m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm;
            //m_debugData->PushDebugLineScaled(m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos, m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm, 1.0f, 1.0f, 0.0f, DirectX::Colors::DarkSeaGreen);
            //m_debugData->PushDebugLine(forcePoint, directionNorm, 20.0f, 0.0f, DirectX::Colors::White);
        }
    }

    /*
    if (torqueSum.axis.Length() > 1.1f || torqueSum.axis.Length() < 0.9f)
    {
        if (torqueSum.magnitude == 0.0f)
        {
            torqueSum.axis = DirectX::SimpleMath::Vector3::UnitY;
        }
        else
        {
            torqueSum.axis.Normalize();
            int testBreak = 0;
            testBreak++;
        }
    }
    */

    torqueSum.axis.Normalize();

    /*
    if (forceSum.Length() > 1600000.0f)
    {
        float forceSumLength = forceSum.Length();
        int testBreak = 0.0f;
        testBreak++;
    }
    if (torqueSum.magnitude > 1.0f)
    {
        int testBreak = 0.0f;
        testBreak++;
    }
    */

    m_vehicleStruct00.vehicleData.testProjectileImpulse = forceSum;
    m_vehicleStruct00.vehicleData.testProjectileTorque = torqueSum;
    m_vehicleStruct00.vehicleData.collisionImpulseForceSum = forceSum;
    m_vehicleStruct00.vehicleData.collisionImpulseTorqueSum = torqueSum;


    Utility::Torque zeroTorque;
    zeroTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    zeroTorque.magnitude = 0.0f;
    m_testTorqueVec = DirectX::SimpleMath::Vector3::Zero;
 
    m_vehicleStruct00.vehicleData.impulseForceVec.erase(
        std::remove_if(
            m_vehicleStruct00.vehicleData.impulseForceVec.begin(),
            m_vehicleStruct00.vehicleData.impulseForceVec.end(),
            [](Utility::ImpulseForce const& p) {return p.isActive == false; }
        ),
        m_vehicleStruct00.vehicleData.impulseForceVec.end()
    );
}

void NPCVehicle::UpdateJetCounterTorqueData(const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 eulerVec = DirectX::SimpleMath::Vector3::Zero;
    eulerVec = m_vehicleStruct00.vehicleData.alignmentQuat.ToEuler();
    DirectX::SimpleMath::Vector3 angularVelocityNorm = m_vehicleStruct00.vehicleData.q.angularVelocity;
    angularVelocityNorm.Normalize();

    DirectX::SimpleMath::Vector3 angDragNorm = m_vehicleStruct00.vehicleData.angularDrag;
    angDragNorm.Normalize();
    float angDragLength = m_vehicleStruct00.vehicleData.angularDrag.Length();
    float angDragMod = angDragLength;
    if(angDragLength > 1.0f)
    {
        angDragMod = 1.0f;
    }

    eulerVec = angDragNorm;
    float yawRot = angDragNorm.y;

    const float rotPitchMod = 1.0f * angDragLength;
    const float rotYawMod = 0.5f * angDragLength;
    float leftRotDot = angularVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rightRotDot = angularVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    
    leftRotDot = eulerVec.z * rotPitchMod;
    rightRotDot = eulerVec.z * rotPitchMod;

    leftRotDot += yawRot * rotYawMod;
    rightRotDot -= yawRot * rotYawMod;

    const float preRotLeft = m_vehicleStruct00.npcModel.jetAntiTorqueRotationLeft;
    const float preRotRight = m_vehicleStruct00.npcModel.jetAntiTorqueRotationRight;
    
    leftRotDot += m_vehicleStruct00.npcModel.jetAntiTorqueRotationLeft;
    rightRotDot += m_vehicleStruct00.npcModel.jetAntiTorqueRotationRight;
    leftRotDot *= 0.5f;
    rightRotDot *= 0.5f;
   
    const float postRotLeft = leftRotDot;
    const float postRotRight = rightRotDot;
    const float maxRotDelta = 3.0f * aTimeDelta;
    const float deltaRotLeft = postRotLeft - preRotLeft;
    const float deltaRotRight = postRotRight - preRotRight;
    if (abs(deltaRotLeft) > maxRotDelta)
    {
        if (deltaRotLeft > 0.0f)
        {
            leftRotDot = preRotLeft + maxRotDelta;
        }
        else
        {
            leftRotDot = preRotLeft - maxRotDelta;
        }
    }
    if (abs(deltaRotRight) > maxRotDelta)
    {
        if (deltaRotRight > 0.0f)
        {
            rightRotDot = preRotRight + maxRotDelta;
        }
        else
        {
            rightRotDot = preRotRight - maxRotDelta;
        }
    }
    m_vehicleStruct00.npcModel.jetAntiTorqueRotationLeft = leftRotDot;
    m_vehicleStruct00.npcModel.jetAntiTorqueRotationRight = rightRotDot;

    const float baseLengthPitchMod = 50.0f * angDragMod;
    const float pitchLengthhMod = 0.5f * angDragMod;
    const float rollLengthMod = 0.5f * angDragMod;
    const float yawLengthMod = 0.5f * angDragMod;
    float leftLength = m_vehicleStruct00.npcModel.jetAntiTorqueLengthLeft;
    float rightLength = m_vehicleStruct00.npcModel.jetAntiTorqueLengthRight;
    float baseLength = m_vehicleStruct00.npcModel.jetAntiTorqueLengthBase;

    leftLength += abs(yawRot) * yawLengthMod;
    rightLength += abs(yawRot) * yawLengthMod;

    if (eulerVec.z < 0.0f)
    {
        leftLength -= eulerVec.z * pitchLengthhMod;
        rightLength -= eulerVec.z * pitchLengthhMod;
    }
    else
    {
        baseLength += eulerVec.z * baseLengthPitchMod;
    }

    if (eulerVec.x < 0.0f)
    {
        rightLength -= eulerVec.x * rollLengthMod;
    }
    else
    {
        leftLength += eulerVec.x * rollLengthMod;
    }

    leftLength *= 0.5f;
    rightLength *= 0.5f;
    baseLength *= 0.5f;

    const float preLengthBase = m_vehicleStruct00.npcModel.jetAntiTorqueLengthBase;
    const float preLengthLeft = m_vehicleStruct00.npcModel.jetAntiTorqueLengthLeft;
    const float preLengthRight = m_vehicleStruct00.npcModel.jetAntiTorqueLengthRight;
    const float postLengthBase = baseLength;
    const float postLengthLeft = leftLength;
    const float postLengthRight = rightLength;
    const float maxSideLengthDelta = 2.0f * aTimeDelta;
    const float deltaLengthLeft = postLengthLeft - preLengthLeft;
    const float deltaLengthRight = postLengthRight - preLengthRight;

    if (abs(deltaLengthLeft) > maxSideLengthDelta)
    {
        if (deltaLengthLeft > 0.0f)
        {
            leftLength = preLengthLeft + maxSideLengthDelta;
        }
        else
        {
            leftLength = preLengthLeft - maxSideLengthDelta;
        }
    }
    if (abs(deltaLengthRight) > maxSideLengthDelta)
    {
        if (deltaLengthRight > 0.0f)
        {
            rightLength = preLengthRight + maxSideLengthDelta;
        }
        else
        {
            rightLength = preLengthRight - maxSideLengthDelta;
        }
    }

    const float maxBaseLengthDelta = 200.0f * aTimeDelta;
    const float deltaLengthBase = postLengthBase - preLengthBase;
    if (abs(deltaLengthBase) > maxBaseLengthDelta)
    {
        if (deltaLengthBase > 0.0f)
        {
            baseLength = preLengthBase + maxBaseLengthDelta;
        }
        else
        {
            baseLength = preLengthBase - maxBaseLengthDelta;
        }
    }

    m_vehicleStruct00.npcModel.jetAntiTorqueLengthLeft = leftLength;
    m_vehicleStruct00.npcModel.jetAntiTorqueLengthRight = rightLength;
    m_vehicleStruct00.npcModel.jetAntiTorqueLengthBase = baseLength;
}

void NPCVehicle::UpdateJumpData(JumpData& aJumpData, const float aTimeDelta)
{
    aJumpData.landingStartAltitude = m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper + m_jumpLandingStartHeight;
    //aJumpData.landingStartAltitude = m_vehicleStruct00.vehicleData.terrainHightAtPos + m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper + m_jumpLandingStartHeight;
    if (aJumpData.isJumpActive == true)
    {
        aJumpData.jumpActiveTimer += aTimeDelta;
        if (aJumpData.isLaunchImpulseBurnActive == false)// && m_vehicleStruct00.vehicleData.q.velocity.y <= 0.0f && m_vehicleStruct00.vehicleData.q.position.y <= aJumpData.landingStartAltitude)
        {
            if (m_vehicleStruct00.vehicleData.q.velocity.y <= 0.0f && aJumpData.isLandImpulseBurnActive == false)
            {
                const float distanceToHoverRange = m_vehicleStruct00.vehicleData.q.position.y -  m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper;
                //const float distanceToHoverRange = m_vehicleStruct00.vehicleData.q.position.y - (m_vehicleStruct00.vehicleData.terrainHightAtPos + m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper);
                const float gravity = m_environment->GetGravity();
                const float initialVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;

                float quad = (-initialVelocity + sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
                if (quad <= 0.0f)
                {
                    quad = (-initialVelocity - sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
                }
                if (quad <= 0.0f)
                {
                    quad = 0.0001f;
                }

                float deceleration = (0.0f - initialVelocity) / quad;
                float decelerationForce = deceleration * m_vehicleStruct00.vehicleData.mass;
                //const float decelForceLimit = 26000.0f;
                const float decelForceLimit = m_jumpDeccelForceLimit;

                if (decelerationForce > decelForceLimit || quad < 1.0f)
                {
                    aJumpData.isLandImpulseBurnActive = true;
                    aJumpData.isImpulseBurnActive = true;
                    aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.currentTime = 0.0f;
                    aJumpData.impulseBurnForce.directionNorm = -m_vehicleStruct00.vehicleData.up;
                    aJumpData.impulseBurnForce.isActive = true;
                    aJumpData.impulseBurnForce.maxMagnitude = deceleration + -gravity;
                    aJumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
                    aJumpData.impulseBurnForce.totalTime = quad  * 1.5f;
                }
            }
            if (aJumpData.isLandImpulseBurnActive == true)
            {
                if (aJumpData.impulseBurnForce.currentTime >= aJumpData.impulseBurnForce.totalTime)
                {
                    aJumpData.isLandImpulseBurnActive = false;
                    aJumpData.isImpulseBurnActive = false;
                    aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.currentTime = 0.0f;
                    aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::Zero;
                    aJumpData.impulseBurnForce.isActive = false;
                    aJumpData.impulseBurnForce.maxMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.torqueArm = DirectX::SimpleMath::Vector3::Zero;
                    aJumpData.impulseBurnForce.totalTime = 0.0f;

                    m_vehicleStruct00.npcModel.jetRotationLeft = 0.0f;
                    m_vehicleStruct00.npcModel.jetRotationRight = 0.0f;

                    aJumpData.isJumpActive = false;
                    aJumpData.isJumpOnCoolDown = true;
                    aJumpData.jumpActiveTimer = 0.0f;
                }
                else
                {
                    aJumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
                    aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::UnitY;

                    Utility::UpdateImpulseForceBellCurve(aJumpData.impulseBurnForce, aTimeDelta);

                    //const float jetRotation = 225.0f;
                    const float jetRotation = 90.0f;
                    m_vehicleStruct00.npcModel.jetRotationLeft = Utility::ToRadians(jetRotation);
                    m_vehicleStruct00.npcModel.jetRotationRight = Utility::ToRadians(jetRotation);
                }
            }
        }
        if (aJumpData.isLaunchImpulseBurnActive == true)
        {
            aJumpData.impulseBurnTimer += aTimeDelta;

            if (aJumpData.impulseBurnTimer >= aJumpData.impulseBurnTimeTotal)
            {
                aJumpData.isLaunchImpulseBurnActive = false;
                aJumpData.isImpulseBurnActive = false;
                aJumpData.impulseBurnTimer = 0.0f;
                aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                aJumpData.impulseBurnForce.currentTime = 0.0f;
                aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::Zero;
                aJumpData.impulseBurnForce.isActive = false;
                aJumpData.impulseBurnForce.maxMagnitude = 0.0f;
                aJumpData.impulseBurnForce.torqueArm = DirectX::SimpleMath::Vector3::Zero;
                aJumpData.impulseBurnForce.totalTime = 0.0f;

                m_vehicleStruct00.npcModel.jetRotationLeft = 0.0f;
                m_vehicleStruct00.npcModel.jetRotationRight = 0.0f;
            }
            else
            {
                aJumpData.impulseBurnForce.torqueArm = -m_vehicleStruct00.vehicleData.right;
                aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::UnitY;

                Utility::UpdateImpulseForceBellCurve(aJumpData.impulseBurnForce, aTimeDelta);
                float jetRotationRatio = aJumpData.impulseBurnForce.currentMagnitude / aJumpData.impulseBurnForce.maxMagnitude;

                m_vehicleStruct00.npcModel.jetRotationLeft = jetRotationRatio * (Utility::GetPi() * 1.0f);
                m_vehicleStruct00.npcModel.jetRotationRight = jetRotationRatio * (Utility::GetPi() * 1.0f);

                //m_vehicleStruct00.npcModel.jetRotationLeft = Utility::ToRadians(112.0f);
                //m_vehicleStruct00.npcModel.jetRotationRight = Utility::ToRadians(112.0f);

                m_vehicleStruct00.npcModel.jetRotationLeft = Utility::ToRadians(75.0f);
                m_vehicleStruct00.npcModel.jetRotationRight = Utility::ToRadians(75.0f);
            }
        }
    }
    if (aJumpData.isJumpOnCoolDown == true)
    {
        aJumpData.jumpCoolDownTimer += aTimeDelta;
        if (aJumpData.jumpCoolDownTimer >= aJumpData.jumpCoolDownTotal)
        {
            aJumpData.isJumpOnCoolDown = false;
            aJumpData.jumpCoolDownTimer = 0.0f;
            aJumpData.isJumpReady = true;
        }
    }

    // control base burner length
    if (aJumpData.isImpulseBurnActive == true)
    {
        if (aJumpData.isLaunchImpulseBurnActive)
        {
            m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = aJumpData.impulseBurnForce.currentMagnitude / aJumpData.impulseBurnForce.maxMagnitude;
        }
        else
        {
            const float landingBurnLengthMod = 2.0f;
            m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = (aJumpData.impulseBurnForce.currentMagnitude * landingBurnLengthMod) / aJumpData.impulseBurnForce.maxMagnitude;
        }
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = 0.0f;
    }
}

void NPCVehicle::UpdateJumpDataOldFig8(JumpData& aJumpData, const float aTimeDelta)
{
    aJumpData.landingStartAltitude = m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper + 15.0f;
    if (aJumpData.isJumpActive == true)
    {
        aJumpData.jumpActiveTimer += aTimeDelta;
        if (aJumpData.isLaunchImpulseBurnActive == false)// && m_vehicleStruct00.vehicleData.q.velocity.y <= 0.0f && m_vehicleStruct00.vehicleData.q.position.y <= aJumpData.landingStartAltitude)
        {
            if (m_vehicleStruct00.vehicleData.q.velocity.y <= 0.0f && aJumpData.isLandImpulseBurnActive == false)
            {
                const float distanceToHoverRange = m_vehicleStruct00.vehicleData.q.position.y - m_vehicleStruct00.vehicleData.hoverData.hoverRangeUpper;
                const float gravity = m_environment->GetGravity();
                const float initialVelocity = m_vehicleStruct00.vehicleData.q.velocity.y;

                float quad = (-initialVelocity + sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
                if (quad <= 0.0f)
                {
                    quad = (-initialVelocity - sqrt((initialVelocity * initialVelocity) - 4.0f * (gravity * distanceToHoverRange))) / (2.0f * gravity);
                }
                if (quad <= 0.0f)
                {
                    quad = 0.0001f;
                }

                float deceleration = (0.0f - initialVelocity) / quad;
                float decelerationForce = deceleration * m_vehicleStruct00.vehicleData.mass;
                const float decelForceLimit = 26000.0f;

                if (decelerationForce > decelForceLimit || quad < 1.0f)
                {
                    aJumpData.isLandImpulseBurnActive = true;
                    aJumpData.isImpulseBurnActive = true;
                    aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.currentTime = 0.0f;
                    aJumpData.impulseBurnForce.directionNorm = -m_vehicleStruct00.vehicleData.up;
                    aJumpData.impulseBurnForce.isActive = true;
                    aJumpData.impulseBurnForce.maxMagnitude = deceleration + -gravity;
                    aJumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
                    aJumpData.impulseBurnForce.totalTime = quad;
                }
            }
            if (aJumpData.isLandImpulseBurnActive == true)
            {
                if (aJumpData.impulseBurnForce.currentTime >= aJumpData.impulseBurnForce.totalTime)
                {
                    aJumpData.isLandImpulseBurnActive = false;
                    aJumpData.isImpulseBurnActive = false;
                    aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.currentTime = 0.0f;
                    aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::Zero;
                    aJumpData.impulseBurnForce.isActive = false;
                    aJumpData.impulseBurnForce.maxMagnitude = 0.0f;
                    aJumpData.impulseBurnForce.torqueArm = DirectX::SimpleMath::Vector3::Zero;
                    aJumpData.impulseBurnForce.totalTime = 0.0f;

                    m_vehicleStruct00.npcModel.jetRotationLeft = 0.0f;
                    m_vehicleStruct00.npcModel.jetRotationRight = 0.0f;

                    aJumpData.isJumpActive = false;
                    aJumpData.isJumpOnCoolDown = true;
                    aJumpData.jumpActiveTimer = 0.0f;
                }
                else
                {
                    aJumpData.impulseBurnForce.torqueArm = m_vehicleStruct00.vehicleData.right;
                    aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::UnitY;

                    Utility::UpdateImpulseForceBellCurve(aJumpData.impulseBurnForce, aTimeDelta);

                    const float jetRotation = 225.0f;
                    m_vehicleStruct00.npcModel.jetRotationLeft = Utility::ToRadians(jetRotation);
                    m_vehicleStruct00.npcModel.jetRotationRight = Utility::ToRadians(jetRotation);
                }
            }
        }
        if (aJumpData.isLaunchImpulseBurnActive == true)
        {
            aJumpData.impulseBurnTimer += aTimeDelta;
            if (aJumpData.impulseBurnTimer >= aJumpData.impulseBurnTimeTotal)
            {
                aJumpData.isLaunchImpulseBurnActive = false;
                aJumpData.isImpulseBurnActive = false;
                aJumpData.impulseBurnTimer = 0.0f;
                aJumpData.impulseBurnForce.currentMagnitude = 0.0f;
                aJumpData.impulseBurnForce.currentTime = 0.0f;
                aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::Zero;
                aJumpData.impulseBurnForce.isActive = false;
                aJumpData.impulseBurnForce.maxMagnitude = 0.0f;
                aJumpData.impulseBurnForce.torqueArm = DirectX::SimpleMath::Vector3::Zero;
                aJumpData.impulseBurnForce.totalTime = 0.0f;

                m_vehicleStruct00.npcModel.jetRotationLeft = 0.0f;
                m_vehicleStruct00.npcModel.jetRotationRight = 0.0f;
            }
            else
            {
                aJumpData.impulseBurnForce.torqueArm = -m_vehicleStruct00.vehicleData.right;
                aJumpData.impulseBurnForce.directionNorm = DirectX::SimpleMath::Vector3::UnitY;

                Utility::UpdateImpulseForceBellCurve(aJumpData.impulseBurnForce, aTimeDelta);
                float jetRotationRatio = aJumpData.impulseBurnForce.currentMagnitude / aJumpData.impulseBurnForce.maxMagnitude;

                m_vehicleStruct00.npcModel.jetRotationLeft = jetRotationRatio * (Utility::GetPi() * 1.0f);
                m_vehicleStruct00.npcModel.jetRotationRight = jetRotationRatio * (Utility::GetPi() * 1.0f);

                m_vehicleStruct00.npcModel.jetRotationLeft = Utility::ToRadians(112.0f);
                m_vehicleStruct00.npcModel.jetRotationRight = Utility::ToRadians(112.0f);
            }
        }
    }
    if (aJumpData.isJumpOnCoolDown == true)
    {
        aJumpData.jumpCoolDownTimer += aTimeDelta;
        if (aJumpData.jumpCoolDownTimer >= aJumpData.jumpCoolDownTotal)
        {
            aJumpData.isJumpOnCoolDown = false;
            aJumpData.jumpCoolDownTimer = 0.0f;
            aJumpData.isJumpReady = true;
        }
    }

    // control base burner length
    if (aJumpData.isImpulseBurnActive == true)
    {
        if (aJumpData.isLaunchImpulseBurnActive)
        {
            m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = aJumpData.impulseBurnForce.currentMagnitude / aJumpData.impulseBurnForce.maxMagnitude;
        }
        else
        {
            const float landingBurnLengthMod = 2.0f;
            m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = (aJumpData.impulseBurnForce.currentMagnitude * landingBurnLengthMod) / aJumpData.impulseBurnForce.maxMagnitude;
        }
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput = 0.0f;
    }
}

void NPCVehicle::UpdateNPC(const double aTimeDelta)
{
    SpawnerJumpUpdate(static_cast<float>(aTimeDelta));

    if (m_vehicleStruct00.vehicleData.time > m_spawnerDelay)
    {
        this->DebugToggleAI();
    }

    m_testLightRotation = Utility::WrapAngle((m_testLightRotation + (aTimeDelta * -100.5f)));
    m_testShapeRotation = Utility::WrapAngle((m_testShapeRotation + (aTimeDelta * 200.5f)));

    if (m_vehicleStruct00.vehicleData.isDead == true)
    {
        m_vehicleStruct00.vehicleData.deleteCountDownTimer -= static_cast<float>(aTimeDelta);
        if (m_vehicleStruct00.vehicleData.deleteCountDownTimer <= 0.0f)
        {
            m_vehicleStruct00.vehicleData.isReadyToDelete = true;
        }
    }
    m_testTimer += aTimeDelta;
    m_avoidanceTargetIndex = -1;

    DirectX::SimpleMath::Vector3 preVelocity = m_vehicleStruct00.vehicleData.q.velocity;

    DirectX::SimpleMath::Vector3 preThrust = m_vehicleStruct00.vehicleData.controlInput.steeringVec * (m_vehicleStruct00.vehicleData.hoverData.forwardThrust);

    bool isVehicleInPlayUpdate = m_vehicleStruct00.environment->GetVehicleUpdateData(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.terrainNormal,
        m_vehicleStruct00.vehicleData.terrainHightAtPos, m_vehicleStruct00.vehicleData.groundPlane);
    if (isVehicleInPlayUpdate == false)
    {
        // to do: add error handling if out of play
    }

    m_vehicleStruct00.vehicleData.altitude = m_vehicleStruct00.vehicleData.q.position.y - m_vehicleStruct00.vehicleData.terrainHightAtPos;

    if (m_isAiOn == true)
    {
        m_npcAI->UpdateAI(static_cast<float>(aTimeDelta));
        UpdateControlInputFromAi();
    }

    if (m_vehicleStruct00.vehicleData.jumpData.isJumpUnlocked == true)
    {
        UpdateJumpData(m_vehicleStruct00.vehicleData.jumpData, static_cast<float>(aTimeDelta));
    }

    
    m_vehicleStruct00.vehicleData.impactForceSum = GetImpactForceSum(m_vehicleStruct00.vehicleData);
    UpdateImpulseForces(static_cast<float>(aTimeDelta));

    m_buoyancyTestForce = GetBuoyancyForce(m_vehicleStruct00.vehicleData);

    UpdateVehicleForces(static_cast<float>(aTimeDelta));

    RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);
    if (m_vehicleStruct00.vehicleData.id != 0 )
    {
        //RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);
    }
    else if (m_isDebugToggleTrue == true)
    {
        //RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);
    }

    if (m_vehicleStruct00.vehicleData.id == 0)
    {
        //m_debugData->DebugPushUILineDecimalNumber("m_vehicleStruct00.vehicleData.q.velocity.x ", m_vehicleStruct00.vehicleData.q.velocity.x, "");
        //m_debugData->DebugPushUILineDecimalNumber("m_vehicleStruct00.vehicleData.q.velocity.y ", m_vehicleStruct00.vehicleData.q.velocity.y, "");
        //m_debugData->DebugPushUILineDecimalNumber("m_vehicleStruct00.vehicleData.q.velocity.z ", m_vehicleStruct00.vehicleData.q.velocity.z, "");
    }

    
    UpdateAlignmentNew(static_cast<float>(aTimeDelta));
    
    /*
    if (m_vehicleStruct00.vehicleData.id != 0)
    {
        UpdateAlignmentNew(static_cast<float>(aTimeDelta));
    }
    else
    {
        m_vehicleStruct00.vehicleData.alignment = m_vehicleStruct00.vehicleData.playerAlignment;
        m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignment;
        m_vehicleStruct00.vehicleData.alignmentInverse = m_vehicleStruct00.vehicleData.alignmentInverse.Invert();
        m_vehicleStruct00.vehicleData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
        m_vehicleStruct00.vehicleData.alignmentInverseQuat = m_vehicleStruct00.vehicleData.alignmentQuat;
        m_vehicleStruct00.vehicleData.alignmentInverseQuat.Inverse(m_vehicleStruct00.vehicleData.alignmentInverseQuat);
        m_vehicleStruct00.vehicleData.alignmentInverseQuat.Normalize();

        m_vehicleStruct00.vehicleData.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_vehicleStruct00.vehicleData.alignment);
        m_vehicleStruct00.vehicleData.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_vehicleStruct00.vehicleData.alignment);
        m_vehicleStruct00.vehicleData.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_vehicleStruct00.vehicleData.alignment);
    }
    */

    UpdateJetCounterTorqueData(aTimeDelta);

    if (m_vehicleStruct00.vehicleData.isVehicleInCameraFrustum == true)
    {
        UpdateNPCModel(aTimeDelta);
    }

    UpdateHardPoints();
    //UpdateJetCounterTorqueData(aTimeDelta);
    if (m_vehicleStruct00.vehicleData.q.position.y > 1300.0f || m_vehicleStruct00.vehicleData.q.velocity.y > 500.0f || m_vehicleStruct00.vehicleData.q.position.Length() > 2000.0f)
    {
        m_vehicleStruct00.vehicleData.isDead = true;
        /*
        JumpData testData = m_vehicleStruct00.vehicleData.jumpData;

        m_vehicleStruct00.vehicleData.impulseForceVec.clear();
        m_vehicleStruct00.vehicleData.impactTorqueVec.clear();
        m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive = false;
        m_vehicleStruct00.vehicleData.jumpData.isJumpActive = false;
        m_vehicleStruct00.vehicleData.jumpData.isLaunchImpulseBurnActive = false;
        m_vehicleStruct00.vehicleData.jumpData.isJumpReady = false;
        m_vehicleStruct00.vehicleData.jumpData.isLandImpulseBurnActive = false;
        m_vehicleStruct00.vehicleData.jumpData.isJumpOnCoolDown = true;

        DirectX::SimpleMath::Vector3 testBuoyancy = m_buoyancyTestForce;
        m_buoyancyTestForce = DirectX::SimpleMath::Vector3::Zero;
        m_vehicleStruct00.vehicleData.q.position.x = 1000.0f;
        m_vehicleStruct00.vehicleData.q.position.y = 12.0f;
        m_vehicleStruct00.vehicleData.q.position.z = 0.0f;

        m_vehicleStruct00.vehicleData.q.velocity.x = 0.0f;
        m_vehicleStruct00.vehicleData.q.velocity.y = 0.0f;
        m_vehicleStruct00.vehicleData.q.velocity.z = 0.0f;
        */
    }
    m_testAccelVec = (m_testAccelVec + (m_vehicleStruct00.vehicleData.q.velocity - preVelocity) / static_cast<float>(aTimeDelta)) * 0.5f;

    m_vehicleStruct00.vehicleData.isCollisionTrue = false;

    m_vehicleStruct00.vehicleData.impactForce.impactMass = 0.0f;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;

    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;

    UpdateForceTorqueVecs();

    UpdateAudio();

    if (m_vehicleStruct00.audioFx->isDestroyTrue == false)
    {
        m_vehicleStruct00.audioFx->pos = m_vehicleStruct00.vehicleData.q.position;
        m_vehicleStruct00.audioFx->up = m_vehicleStruct00.vehicleData.up;
        m_vehicleStruct00.audioFx->forward = m_vehicleStruct00.vehicleData.forward;
    
        m_vehicleStruct00.audioFx->emitter->Position = m_vehicleStruct00.vehicleData.q.position;
        m_vehicleStruct00.audioFx->emitter->Velocity = m_vehicleStruct00.vehicleData.q.velocity;
        m_vehicleStruct00.audioFx->emitter->SetOrientation(m_vehicleStruct00.vehicleData.forward, m_vehicleStruct00.vehicleData.up);
        //m_vehicleStruct00.audioFx->fx->SetVolume(1.0f);  m_vehicleStruct00.vehicleData.hoverData.forwardThrust
        //m_vehicleStruct00.audioFx->fx->SetVolume(abs(m_vehicleStruct00.vehicleData.controlInput.throttleInput));
        //m_vehicleStruct00.audioFx->fx->SetPitch(abs(m_vehicleStruct00.vehicleData.controlInput.throttleInput));

        // /////////////////////////////////////////////////////////////////////////////
        //m_vehicleStruct00.audioFx->SetPos(m_vehicleStruct00.vehicleData.q.position);
        //m_vehicleStruct00.audioFx->emitter->SetPosition(m_vehicleStruct00.vehicleData.q.position);
        //m_vehicleStruct00.audioFx->emitter->SetVelocity(m_vehicleStruct00.vehicleData.q.velocity);
    }
    


    m_vehicleStruct00.npcModel.laserLightPosVec.clear();
}

void NPCVehicle::UpdateNPCModel(const double aTimeDelta)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);

    m_vehicleStruct00.npcModel.worldModelMatrix = m_vehicleStruct00.npcModel.localModelMatrix;
    m_vehicleStruct00.npcModel.worldModelMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldInteriorShadowLeftMatrix = m_vehicleStruct00.npcModel.localInteriorShadowLeftMatrix;
    m_vehicleStruct00.npcModel.worldInteriorShadowLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldInteriorShadowRightMatrix = m_vehicleStruct00.npcModel.localInteriorShadowRightMatrix;
    m_vehicleStruct00.npcModel.worldInteriorShadowRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldRearBodyMatrix = m_vehicleStruct00.npcModel.localRearBodyMatrix;
    m_vehicleStruct00.npcModel.worldRearBodyMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldFrontAirDamMatrix = m_vehicleStruct00.npcModel.localFrontAirDamMatrix;
    m_vehicleStruct00.npcModel.worldFrontAirDamMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldEyeLeftMatrix = m_vehicleStruct00.npcModel.localEyeLeftMatrix;
    m_vehicleStruct00.npcModel.worldEyeLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldEyeRightMatrix = m_vehicleStruct00.npcModel.localEyeRightMatrix;
    m_vehicleStruct00.npcModel.worldEyeRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldGrillLeftMatrix = m_vehicleStruct00.npcModel.localGrillLeftMatrix;
    m_vehicleStruct00.npcModel.worldGrillLeftMatrix *= updateMat;
    m_vehicleStruct00.npcModel.worldGrillRightMatrix = m_vehicleStruct00.npcModel.localGrillRightMatrix;
    m_vehicleStruct00.npcModel.worldGrillRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldRearDeckMatrix = m_vehicleStruct00.npcModel.localRearDeckMatrix;
    m_vehicleStruct00.npcModel.worldRearDeckMatrix *= updateMat;

    float throttleTest = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    float steeringTest = m_vehicleStruct00.vehicleData.controlInput.steeringInput / m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    throttleTest *= 1.0f;
    steeringTest *= 1.0f;
    float leftBurnLengthModTest = throttleTest - steeringTest;
    float rightBurnLengthModTest = throttleTest + steeringTest;
    if (leftBurnLengthModTest > 1.0f)
    {
        leftBurnLengthModTest = 1.0f;
    }
    if (leftBurnLengthModTest < -1.0f)
    {
        leftBurnLengthModTest = -1.0f;
    }
    if (rightBurnLengthModTest > 1.0f)
    {
        rightBurnLengthModTest = 1.0f;
    }
    if (rightBurnLengthModTest < -1.0f)
    {
        rightBurnLengthModTest = -1.0f;
    }

    float testThrottleRotLeft = (leftBurnLengthModTest * (Utility::GetPi() * -0.5f)) + (Utility::GetPi() * 0.5f);
    float testThrottleRotRight = (rightBurnLengthModTest * (Utility::GetPi() * -0.5f)) + (Utility::GetPi() * 0.5f);

    // counter body torque jet rotation
    testThrottleRotLeft += m_vehicleStruct00.npcModel.jetAntiTorqueRotationLeft;
    testThrottleRotRight += m_vehicleStruct00.npcModel.jetAntiTorqueRotationRight;
    if (testThrottleRotLeft > DirectX::XM_PI)
    {
        testThrottleRotLeft = DirectX::XM_PI;
    }
    else if (testThrottleRotLeft < 0.0f)
    {
        testThrottleRotLeft = 0.0f;
    }
    if (testThrottleRotRight > DirectX::XM_PI)
    {
        testThrottleRotRight = DirectX::XM_PI;
    }
    else if (testThrottleRotRight < 0.0f)
    {
        testThrottleRotRight = 0.0f;
    }

    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        testThrottleRotLeft = m_vehicleStruct00.npcModel.jetRotationLeft;
        testThrottleRotRight = m_vehicleStruct00.npcModel.jetRotationRight;
        if (m_vehicleStruct00.vehicleData.jumpData.isLandImpulseBurnActive == true)
        {
            testThrottleRotLeft = m_vehicleStruct00.npcModel.jetRotationLeft;
            testThrottleRotRight = m_vehicleStruct00.npcModel.jetRotationRight;
        }
    }

    testThrottleRotLeft = (testThrottleRotLeft + m_vehicleStruct00.npcModel.jetRotationLeftPrev + m_vehicleStruct00.npcModel.jetRotationLeftPrev2
        + m_vehicleStruct00.npcModel.jetRotationLeftPrev3) / 4.0f;
    m_vehicleStruct00.npcModel.jetRotationLeftPrev3 = m_vehicleStruct00.npcModel.jetRotationLeftPrev2;
    m_vehicleStruct00.npcModel.jetRotationLeftPrev2 = m_vehicleStruct00.npcModel.jetRotationLeftPrev;
    m_vehicleStruct00.npcModel.jetRotationLeftPrev = testThrottleRotLeft;
    testThrottleRotRight = (testThrottleRotRight + m_vehicleStruct00.npcModel.jetRotationRightPrev + m_vehicleStruct00.npcModel.jetRotationRightPrev2
        + m_vehicleStruct00.npcModel.jetRotationRightPrev3) / 4.0f;
    m_vehicleStruct00.npcModel.jetRotationRightPrev3 = m_vehicleStruct00.npcModel.jetRotationRightPrev2;
    m_vehicleStruct00.npcModel.jetRotationRightPrev2 = m_vehicleStruct00.npcModel.jetRotationRightPrev;
    m_vehicleStruct00.npcModel.jetRotationRightPrev = testThrottleRotRight;

    DirectX::SimpleMath::Matrix jetRotationMatLeft = DirectX::SimpleMath::Matrix::CreateRotationZ(testThrottleRotLeft);
    DirectX::SimpleMath::Matrix jetRotationMatRight = DirectX::SimpleMath::Matrix::CreateRotationZ(testThrottleRotRight);

    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix = m_vehicleStruct00.npcModel.localJetHousingLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix = m_vehicleStruct00.npcModel.localJetHousingShellLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationLeftMatrix;;
    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix = m_vehicleStruct00.npcModel.localJetHousingRightMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationRightMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix = m_vehicleStruct00.npcModel.localJetHousingShellRightMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationRightMatrix;;
    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix *= updateMat;

    // jet mount
    m_vehicleStruct00.npcModel.worldJetMountMatrix = m_vehicleStruct00.npcModel.localJetMountMatrix;
    m_vehicleStruct00.npcModel.worldJetMountMatrix *= updateMat;

    // base jet housing 
    m_vehicleStruct00.npcModel.worldBaseJetHousingMatrix = m_vehicleStruct00.npcModel.localBaseJetHousingMatrix;
    m_vehicleStruct00.npcModel.worldBaseJetHousingMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldBaseJetShadowMatrix = m_vehicleStruct00.npcModel.localBaseJetShadowMatrix;
    m_vehicleStruct00.npcModel.worldBaseJetShadowMatrix *= updateMat;
    //

    const float throttle = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    const float steering = m_vehicleStruct00.vehicleData.controlInput.steeringInput / m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    const float afterBurnIdleLengthMod = 0.25f;

    float leftBurnLengthMod = throttle - steering + afterBurnIdleLengthMod;
    float rightBurnLengthMod = throttle + steering + afterBurnIdleLengthMod;

    const float afterBurnLengthSum = 190.0f;
    float afterBurnLengthModLeft;
    float afterBurnLengthModRight;
    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        float burnRatio = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude / m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.maxMagnitude;
        leftBurnLengthMod = burnRatio;
        rightBurnLengthMod = burnRatio;
    }

    // Anti torque alignment jet length mod
    leftBurnLengthMod += m_vehicleStruct00.npcModel.jetAntiTorqueLengthLeft;
    rightBurnLengthMod += m_vehicleStruct00.npcModel.jetAntiTorqueLengthRight;


    float prevBurnLengthLeft = m_vehicleStruct00.npcModel.afterBurnLengthLeft;
    float prevBurnLengthRight = m_vehicleStruct00.npcModel.afterBurnLengthRight;
    float prevBurnLengthLeft1 = m_vehicleStruct00.npcModel.afterBurnLengthLeftPrev;
    float prevBurnLengthRight1 = m_vehicleStruct00.npcModel.afterBurnLengthRightPrev;
    float prevBurnLengthLeft2 = m_vehicleStruct00.npcModel.afterBurnLengthLeftPrev2;
    float prevBurnLengthRight2 = m_vehicleStruct00.npcModel.afterBurnLengthRightPrev2;
    m_vehicleStruct00.npcModel.afterBurnLengthLeftPrev = prevBurnLengthLeft;
    m_vehicleStruct00.npcModel.afterBurnLengthRightPrev = prevBurnLengthRight;
    m_vehicleStruct00.npcModel.afterBurnLengthLeftPrev2 = prevBurnLengthLeft1;
    m_vehicleStruct00.npcModel.afterBurnLengthRightPrev2 = prevBurnLengthRight1;
    m_vehicleStruct00.npcModel.afterBurnLengthLeftPrev3 = prevBurnLengthLeft2;
    m_vehicleStruct00.npcModel.afterBurnLengthRightPrev3 = prevBurnLengthRight2;

    leftBurnLengthMod = (leftBurnLengthMod + prevBurnLengthLeft + prevBurnLengthLeft1 + prevBurnLengthLeft2) / 4.0f;
    rightBurnLengthMod = (rightBurnLengthMod + prevBurnLengthRight + prevBurnLengthRight1 + prevBurnLengthRight2) / 4.0f;

    const float delta = leftBurnLengthMod - rightBurnLengthMod;
    if (abs(delta) > abs(m_vehicleStruct00.npcModel.maxDelta))
    {
        m_vehicleStruct00.npcModel.maxDelta = abs(delta);
    }

    afterBurnLengthModLeft = leftBurnLengthMod;
    afterBurnLengthModRight = rightBurnLengthMod;

    m_vehicleStruct00.npcModel.afterBurnLengthLeft = afterBurnLengthModLeft;
    m_vehicleStruct00.npcModel.afterBurnLengthRight = afterBurnLengthModRight;

    // hacking in audio volume data update
    float audioLeft = m_vehicleStruct00.npcModel.afterBurnLengthLeft - afterBurnIdleLengthMod;
    float audioRight = m_vehicleStruct00.npcModel.afterBurnLengthRight - afterBurnIdleLengthMod;
    float sum = (audioLeft + audioRight) / 2.0f;
    if (sum > 1.0f)
    {
        sum = 1.0f;
    }
    else if (sum < 0.0f)
    {
        sum = 0.0f;
    }
    m_vehicleStruct00.vehicleData.audioThrottle = sum;

    // Anti torque alignment jet length mod
    //m_vehicleStruct00.npcModel.afterBurnLengthLeft += m_vehicleStruct00.npcModel.jetAntiTorqueLengthLeft;
    //m_vehicleStruct00.npcModel.afterBurnLengthRight += m_vehicleStruct00.npcModel.jetAntiTorqueLengthRight;

    float testThrottle = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    float afterBurnLength = (afterBurnLengthModLeft * afterBurnLengthSum);
    m_vehicleStruct00.npcModel.burnFlickerLegth = Utility::WrapAnglePositive(m_vehicleStruct00.npcModel.burnFlickerLegth + m_vehicleStruct00.npcModel.burnFlickerFrequencyMod);
    afterBurnLength = afterBurnLength + (m_vehicleStruct00.npcModel.burnFlickerLegth * m_vehicleStruct00.npcModel.burnFlickerLengthMod);

    float afterBurnY = (afterBurnLength * 0.5f) * 0.1f;
    DirectX::SimpleMath::Matrix afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);
    if (testThrottle <= 0.0f)
    {
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);
    }
    const float idleFlicker = 0.5f;
    const float afterBurnFlickerModLeft = afterBurnLengthModLeft + idleFlicker;
    m_vehicleStruct00.npcModel.afterBurnLeftFlicker += (afterBurnFlickerModLeft * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnLeftFlicker = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    //m_vehicleStruct00.npcModel.afterBurnLeftFlicker = 0.0f;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    DirectX::SimpleMath::Matrix afterBurnScale = DirectX::SimpleMath::Matrix::CreateScale(1.0f, abs(afterBurnLength), 1.0f);
    m_vehicleStruct00.npcModel.afterBurnLeftScaledHeight = m_vehicleStruct00.npcModel.afterBurnHeight * abs(afterBurnLength);
    m_vehicleStruct00.npcModel.afterBurnLeftFlicker2 -= (afterBurnFlickerModLeft * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnLeftFlicker2 = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnLeftFlicker2);
    //m_vehicleStruct00.npcModel.afterBurnLeftFlicker2 = Utility::ToRadians(0.0f);
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnLeftFlicker2 + Utility::ToRadians(60.0f));

    if (afterBurnLengthModLeft <= 0.0f)
    {
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -afterBurnY, 0.0f);
    }

    DirectX::SimpleMath::Matrix afterBurnFlickerScaleLeft = DirectX::SimpleMath::Matrix::CreateScale(1.0f);
    if (m_vehicleStruct00.npcModel.isLeftPlumeFlickerTrue == true)
    {
        m_vehicleStruct00.npcModel.isLeftPlumeFlickerTrue = false;
        //afterBurnFlickerScale = DirectX::SimpleMath::Matrix::CreateScale(m_vehicleStruct00.npcModel.plumeScale);
        afterBurnFlickerScaleLeft = DirectX::SimpleMath::Matrix::CreateScale(1.0, m_vehicleStruct00.npcModel.plumeScale, 1.0);
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        if (afterBurnLengthModLeft <= 0.0f)
        {
            afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -afterBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        }
    }
    else
    {
        m_vehicleStruct00.npcModel.isLeftPlumeFlickerTrue = true;
    }
    
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnScale;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnFlickerScaleLeft;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= m_vehicleStruct00.npcModel.localAfterBurnLeftMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationLeftMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= afterBurnScale;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= afterBurnFlickerScaleLeft;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= m_vehicleStruct00.npcModel.localAfterBurnLeftMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= m_vehicleStruct00.npcModel.jetHousingTranslationLeftMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix2 *= updateMat;

    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, jetRotationMatLeft);
    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.npcModel.jetDirectionLeft, updateMat);

    //m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_vehicleStruct00.vehicleData.alignment);
    //DirectX::SimpleMath::Matrix lightDirLeft = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleStruct00.vehicleData.right, testThrottleRotLeft);

    DirectX::SimpleMath::Matrix lightDirLeft = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleStruct00.vehicleData.right, testThrottleRotLeft);
    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.npcModel.jetDirectionLeft, lightDirLeft);

    m_vehicleStruct00.npcModel.jetDirectionLeft.Normalize();
    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, jetRotationMatLeft);
    m_vehicleStruct00.npcModel.jetDirectionLeft = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.npcModel.jetDirectionLeft, updateMat);

    // right burner 
    afterBurnLength = (afterBurnLengthModRight * afterBurnLengthSum);
    m_vehicleStruct00.npcModel.burnFlickerLegth = Utility::WrapAnglePositive(m_vehicleStruct00.npcModel.burnFlickerLegth + m_vehicleStruct00.npcModel.burnFlickerFrequencyMod);
    afterBurnLength = afterBurnLength + (m_vehicleStruct00.npcModel.burnFlickerLegth * m_vehicleStruct00.npcModel.burnFlickerLengthMod);
    afterBurnY = (afterBurnLength * 0.5f) * 0.1f;
    afterBurnScale = DirectX::SimpleMath::Matrix::CreateScale(1.0f, abs(afterBurnLength), 1.0f);
    afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);
    m_vehicleStruct00.npcModel.afterBurnRightScaledHeight = m_vehicleStruct00.npcModel.afterBurnHeight * abs(afterBurnLength);
    const float afterBurnFlickerModRight = afterBurnLengthModRight + idleFlicker;
    m_vehicleStruct00.npcModel.afterBurnRightFlicker -= (afterBurnFlickerModRight * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnRightFlicker = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnRightFlicker);
    //m_vehicleStruct00.npcModel.afterBurnRightFlicker = 0.0f;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnRightFlicker);

    m_vehicleStruct00.npcModel.afterBurnRightFlicker2 += (afterBurnFlickerModRight * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnRightFlicker2 = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnRightFlicker2);
    //m_vehicleStruct00.npcModel.afterBurnRightFlicker2 = Utility::ToRadians(0.0f);
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnRightFlicker2 + Utility::ToRadians(60.0f));

    if (afterBurnLengthModRight <= 0.0f)
    {
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -afterBurnY, 0.0f);
    }

    DirectX::SimpleMath::Matrix afterBurnFlickerScaleRight = DirectX::SimpleMath::Matrix::CreateScale(1.0f);
    if (m_vehicleStruct00.npcModel.isRightPlumeFlickerTrue == true)
    {
        m_vehicleStruct00.npcModel.isRightPlumeFlickerTrue = false;
        afterBurnFlickerScaleRight = DirectX::SimpleMath::Matrix::CreateScale(1.0, m_vehicleStruct00.npcModel.plumeScale, 1.0);
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        if (afterBurnLengthModRight <= 0.0f)
        {
            afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -afterBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        }
    }
    else
    {
        m_vehicleStruct00.npcModel.isRightPlumeFlickerTrue = true;
    }

    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnScale;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnFlickerScaleRight;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= m_vehicleStruct00.npcModel.localAfterBurnRightMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= m_vehicleStruct00.npcModel.jetHousingTranslationRightMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= afterBurnScale;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= afterBurnFlickerScaleRight;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= m_vehicleStruct00.npcModel.localAfterBurnRightMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= m_vehicleStruct00.npcModel.jetHousingTranslationRightMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix2 *= updateMat;

    m_vehicleStruct00.npcModel.jetDirectionRight = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, -jetRotationMatRight);
    m_vehicleStruct00.npcModel.jetDirectionRight = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.npcModel.jetDirectionRight, updateMat);

    m_vehicleStruct00.npcModel.jetDirectionBase = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, updateMat);

    // base burner
    float baseBurnLength = ((10.0f) + (m_vehicleStruct00.vehicleData.controlInput.baseThrottleInput * 60.0f)) + ((1.0f + m_vehicleStruct00.npcModel.mainThrustLengthMod) * 10.0f);
    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        float burnRatio = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude / m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.maxMagnitude;
        baseBurnLength *= (1.0f + burnRatio);
    }

    m_vehicleStruct00.npcModel.burnFlickerLegth = Utility::WrapAnglePositive(m_vehicleStruct00.npcModel.burnFlickerLegth + m_vehicleStruct00.npcModel.burnFlickerFrequencyMod);
  baseBurnLength = baseBurnLength + (m_vehicleStruct00.npcModel.burnFlickerLegth * m_vehicleStruct00.npcModel.burnFlickerLengthMod);

    // counter body torque base jet length mod
    baseBurnLength += m_vehicleStruct00.npcModel.jetAntiTorqueLengthBase;

    baseBurnLength = (baseBurnLength + m_vehicleStruct00.npcModel.baseBurnLengthPrev1) * 0.5f;
    m_vehicleStruct00.npcModel.baseBurnLengthPrev1 = baseBurnLength;
    float baseBurnY = (baseBurnLength * 0.5f) * 0.1f;
  
    m_vehicleStruct00.npcModel.baseBurnLength = baseBurnY;
    m_vehicleStruct00.npcModel.baseBurnScaledHeight = m_vehicleStruct00.npcModel.baseBurnHeight * abs(baseBurnLength);
    DirectX::SimpleMath::Matrix baseBurnScale = DirectX::SimpleMath::Matrix::CreateScale(1.0f, abs(baseBurnLength), 1.0f);
    DirectX::SimpleMath::Matrix baseBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, baseBurnY, 0.0f);
    const float baseBurnFlickerMod = 0.6f;

    DirectX::SimpleMath::Matrix afterBurnFlickerScaleMain = DirectX::SimpleMath::Matrix::CreateScale(1.0f);
    if (m_vehicleStruct00.npcModel.isMainPlumeFlickerTrue == true)
    {
        m_vehicleStruct00.npcModel.isMainPlumeFlickerTrue = false;
        afterBurnFlickerScaleMain = DirectX::SimpleMath::Matrix::CreateScale(1.0, m_vehicleStruct00.npcModel.plumeScale, 1.0);
        baseBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, baseBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        if (baseBurnLength <= 0.0f)
        {
            baseBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -baseBurnY * m_vehicleStruct00.npcModel.plumeScale, 0.0f);
        }
    }
    else
    {
        m_vehicleStruct00.npcModel.isMainPlumeFlickerTrue = true;
    }

    m_vehicleStruct00.npcModel.baseBurnFlicker1 -= (baseBurnFlickerMod * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.baseBurnFlicker1 = Utility::WrapAngle(m_vehicleStruct00.npcModel.baseBurnFlicker1);
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.baseBurnFlicker1);
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 *= baseBurnScale;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 *= afterBurnFlickerScaleMain;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 *= baseBurnTranslation;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 *= m_vehicleStruct00.npcModel.localBaseBurnMatrix;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix1 *= updateMat;

    m_vehicleStruct00.npcModel.baseBurnFlicker2 += (baseBurnFlickerMod * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.baseBurnFlicker2 = Utility::WrapAngle(m_vehicleStruct00.npcModel.baseBurnFlicker2);
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.baseBurnFlicker2);
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 *= baseBurnScale;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 *= afterBurnFlickerScaleMain;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 *= baseBurnTranslation;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 *= m_vehicleStruct00.npcModel.localBaseBurnMatrix;
    m_vehicleStruct00.npcModel.worldBaseBurnMatrix2 *= updateMat;
    //

    //   
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix = m_vehicleStruct00.npcModel.localJetIntakeCoverLeftTranslationMatrix;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix *= m_vehicleStruct00.npcModel.localJetIntakeCoverLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2 = m_vehicleStruct00.npcModel.localJetIntakeCoverLeftTranslationMatrix2;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2 *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2 *= m_vehicleStruct00.npcModel.localJetIntakeCoverLeftMatrix2;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverLeftMatrix2 *= updateMat;
    //
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix = m_vehicleStruct00.npcModel.localJetIntakeCoverLeftTranslationMatrix;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(m_vehicleStruct00.npcModel.afterBurnRightFlicker);
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix *= m_vehicleStruct00.npcModel.localJetIntakeCoverRightMatrix;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2 = m_vehicleStruct00.npcModel.localJetIntakeCoverLeftTranslationMatrix2;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_vehicleStruct00.npcModel.afterBurnRightFlicker);
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2 *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2 *= m_vehicleStruct00.npcModel.localJetIntakeCoverRightMatrix2;
    m_vehicleStruct00.npcModel.worldJetIntakeCoverRightMatrix2 *= updateMat;

    const float speed = m_vehicleStruct00.vehicleData.q.velocity.Length();
    const float rotMod = 0.001f;
    bool isVelocityBackwards;
    if (m_vehicleStruct00.vehicleData.forward.Dot(m_vehicleStruct00.vehicleData.q.velocity) < 0.0)
    {
        isVelocityBackwards = true;
        m_vehicleStruct00.npcModel.skirtRotation += speed * rotMod;
    }
    else
    {
        isVelocityBackwards = false;
        m_vehicleStruct00.npcModel.skirtRotation -= speed * rotMod;
    }

    m_vehicleStruct00.npcModel.skirtRotation = Utility::WrapAngle(m_vehicleStruct00.npcModel.skirtRotation);

    m_vehicleStruct00.npcModel.worldSkirtMatrix = m_vehicleStruct00.npcModel.localSkirtMatrix;
    m_vehicleStruct00.npcModel.worldSkirtMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldForwardMatrix = m_vehicleStruct00.npcModel.localForwardMatrix;
    m_vehicleStruct00.npcModel.worldForwardMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldNoseConeMatrix = m_vehicleStruct00.npcModel.localNoseConeMatrix;
    m_vehicleStruct00.npcModel.worldNoseConeMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldOmniBaseMatrix = m_vehicleStruct00.npcModel.localOmniBaseMatrix;
    m_vehicleStruct00.npcModel.worldOmniBaseMatrix *= updateMat;

    DirectX::SimpleMath::Vector3 omniDialTranslation = m_vehicleStruct00.vehicleData.controlInput.omniDirection;
    omniDialTranslation.Normalize();
    omniDialTranslation *= m_vehicleStruct00.npcModel.omniDialRadius;
    m_vehicleStruct00.npcModel.worldOmniDialMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(omniDialTranslation);
    m_vehicleStruct00.npcModel.worldOmniDialMatrix *= m_vehicleStruct00.npcModel.localOmniDialMatrix;
    m_vehicleStruct00.npcModel.worldOmniDialMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(-1.4f, 0.0f, 0.0f);

    m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ((m_vehicleStruct00.vehicleData.controlInput.throttleInput - 1.0f) * 0.5f);
    m_vehicleStruct00.npcModel.worldRearMatrix *= m_vehicleStruct00.npcModel.localRearMatrix;
    m_vehicleStruct00.npcModel.worldThrottleMatrix = m_vehicleStruct00.npcModel.worldRearMatrix;
    m_vehicleStruct00.npcModel.worldRearMatrix *= updateMat;

    float wingRotationAngle = 0.0f;
    float testAngle = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    if (testAngle < 0.0f)
    {
        wingRotationAngle = (testAngle) * 1.6f;
    }
    m_vehicleStruct00.npcModel.wingRotPrev3 = m_vehicleStruct00.npcModel.wingRotPrev2;
    m_vehicleStruct00.npcModel.wingRotPrev2 = m_vehicleStruct00.npcModel.wingRotPrev1;
    m_vehicleStruct00.npcModel.wingRotPrev1 = wingRotationAngle;
    wingRotationAngle = (m_vehicleStruct00.npcModel.wingRotPrev1 + m_vehicleStruct00.npcModel.wingRotPrev2 + m_vehicleStruct00.npcModel.wingRotPrev3) / 3.0f;

    DirectX::SimpleMath::Matrix wingRotation = DirectX::SimpleMath::Matrix::CreateRotationZ(wingRotationAngle);

    m_vehicleStruct00.npcModel.worldWingArmMatrix = m_vehicleStruct00.npcModel.localWingArmMatrix;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= m_vehicleStruct00.npcModel.localWingArmTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingMatrix = m_vehicleStruct00.npcModel.localWingMatrix;
    m_vehicleStruct00.npcModel.worldWingMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingMatrix *= m_vehicleStruct00.npcModel.localWingTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix = m_vehicleStruct00.npcModel.localWingFinLeftMatrix;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= m_vehicleStruct00.npcModel.localWingFinLeftTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingFinRightMatrix = m_vehicleStruct00.npcModel.localWingFinRightMatrix;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= m_vehicleStruct00.npcModel.localWingFinRightTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 0.8f));
    m_vehicleStruct00.npcModel.worldThrottleMatrix *= updateMat;

    DirectX::SimpleMath::Matrix steeringRotation = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    m_vehicleStruct00.npcModel.worldSteeringMatrix = m_vehicleStruct00.npcModel.localSteeringMatrix;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= steeringRotation;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= m_vehicleStruct00.npcModel.translationSteeringMatrix;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldVentMatrix1 = m_vehicleStruct00.npcModel.localVentMatrix1;
    m_vehicleStruct00.npcModel.worldVentMatrix1 *= updateMat;

    m_vehicleStruct00.npcModel.worldVentMatrix2 = m_vehicleStruct00.npcModel.localVentMatrix2;
    m_vehicleStruct00.npcModel.worldVentMatrix2 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix3 = m_vehicleStruct00.npcModel.localVentMatrix3;
    m_vehicleStruct00.npcModel.worldVentMatrix3 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix4 = m_vehicleStruct00.npcModel.localVentMatrix4;
    m_vehicleStruct00.npcModel.worldVentMatrix4 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix5 = m_vehicleStruct00.npcModel.localVentMatrix5;
    m_vehicleStruct00.npcModel.worldVentMatrix5 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix6 = m_vehicleStruct00.npcModel.localVentMatrix6;
    m_vehicleStruct00.npcModel.worldVentMatrix6 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix7 = m_vehicleStruct00.npcModel.localVentMatrix7;
    m_vehicleStruct00.npcModel.worldVentMatrix7 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix8 = m_vehicleStruct00.npcModel.localVentMatrix8;
    m_vehicleStruct00.npcModel.worldVentMatrix8 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix9 = m_vehicleStruct00.npcModel.localVentMatrix9;
    m_vehicleStruct00.npcModel.worldVentMatrix9 *= updateMat;

    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();
    DirectX::SimpleMath::Plane groundPlane = m_vehicleStruct00.vehicleData.groundPlane;
    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * m_vehicleStruct00.npcModel.zOffsetShadowMod;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    DirectX::SimpleMath::Matrix planeTrans2 = planeTrans;
    planeTrans2 = planeTrans2.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans2);
    float alt = m_vehicleStruct00.vehicleData.altitude;
    DirectX::SimpleMath::Matrix shadowMat = DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    const float maxShadowRange = m_environment->GetMaxShadowCastRange();
    float shadowScale;
    float inverseShadowScale;
    if (m_vehicleStruct00.vehicleData.altitude > maxShadowRange)
    {
        shadowScale = 0.0f;
        inverseShadowScale = 1.0f;
    }
    else
    {
        inverseShadowScale = m_vehicleStruct00.vehicleData.altitude / maxShadowRange;
        shadowScale = 1.0f - inverseShadowScale;
    }
    DirectX::SimpleMath::Matrix shadowScaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(shadowScale, shadowScale, shadowScale));

    m_vehicleStruct00.npcModel.shadowJetLeftMat = m_vehicleStruct00.npcModel.localJetHousingLeftMatrix;
    m_vehicleStruct00.npcModel.shadowJetLeftMat *= jetRotationMatLeft;
    m_vehicleStruct00.npcModel.shadowJetLeftMat *= m_vehicleStruct00.npcModel.jetHousingTranslationLeftMatrix * inverseShadowScale;
    m_vehicleStruct00.npcModel.shadowJetLeftMat *= shadowScaleMat;
    m_vehicleStruct00.npcModel.shadowJetLeftMat *= updateMat;
    m_vehicleStruct00.npcModel.shadowJetLeftMat *= shadowMat;

    m_vehicleStruct00.npcModel.shadowJetRightMat = m_vehicleStruct00.npcModel.localJetHousingRightMatrix;
    m_vehicleStruct00.npcModel.shadowJetRightMat *= jetRotationMatRight;
    m_vehicleStruct00.npcModel.shadowJetRightMat *= m_vehicleStruct00.npcModel.jetHousingTranslationRightMatrix * inverseShadowScale;
    m_vehicleStruct00.npcModel.shadowJetRightMat *= shadowScaleMat;
    m_vehicleStruct00.npcModel.shadowJetRightMat *= updateMat;
    m_vehicleStruct00.npcModel.shadowJetRightMat *= shadowMat;

    m_vehicleStruct00.npcModel.worldJetMountShadowMatrix = m_vehicleStruct00.npcModel.localJetMountShadowMatrix;
    m_vehicleStruct00.npcModel.worldJetMountShadowMatrix *= m_vehicleStruct00.npcModel.jetMountShadowTranslationMatrix * inverseShadowScale;
    m_vehicleStruct00.npcModel.worldJetMountShadowMatrix *= shadowScaleMat;
    m_vehicleStruct00.npcModel.worldJetMountShadowMatrix *= updateMat;
    m_vehicleStruct00.npcModel.worldJetMountShadowMatrix *= shadowMat;

    m_vehicleStruct00.npcModel.worldSkirtShadowMatrix = m_vehicleStruct00.npcModel.localSkirtShadowMatrix;
    m_vehicleStruct00.npcModel.worldSkirtShadowMatrix *= m_vehicleStruct00.npcModel.skirtShadowTranslationMatrix * inverseShadowScale;
    m_vehicleStruct00.npcModel.worldSkirtShadowMatrix *= shadowScaleMat;
    m_vehicleStruct00.npcModel.worldSkirtShadowMatrix *= updateMat;
    m_vehicleStruct00.npcModel.worldSkirtShadowMatrix *= shadowMat;

    m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix = m_vehicleStruct00.npcModel.localBodyMainShadowMatrix;
    m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix *= m_vehicleStruct00.npcModel.bodyMainShadowTranslationMatrix * inverseShadowScale;
    m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix *= shadowScaleMat;
    m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix *= updateMat;
    m_vehicleStruct00.npcModel.worldBodyMainShadowMatrix *= shadowMat;

    m_vehicleStruct00.npcModel.worldCustomMatrix = m_vehicleStruct00.npcModel.localCustomMatrix;
    m_vehicleStruct00.npcModel.worldCustomMatrix *= updateMat;
}

void NPCVehicle::UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos)
{
    m_vehicleStruct00.vehicleData.playerPos = aPlayerPos;
}

void NPCVehicle::UpdatePlayerVelocity(const DirectX::SimpleMath::Vector3 aPlayerVelocity)
{
    m_vehicleStruct00.vehicleData.playerVelocity = aPlayerVelocity;
}

void NPCVehicle::UpdatePlayerAlignment(const DirectX::SimpleMath::Matrix aAlignment)
{
    m_vehicleStruct00.vehicleData.playerAlignment = aAlignment;
}

void NPCVehicle::UpdateVehicleForces(const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 angularVec = UpdateBodyTorqueRungeLocalNew(aTimeStep);

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 impactForceSum = m_vehicleStruct00.vehicleData.impactForceSum;
    if (m_vehicleStruct00.vehicleData.jumpData.isImpulseBurnActive == true)
    {
        float jumpMag = m_vehicleStruct00.vehicleData.jumpData.impulseBurnForce.currentMagnitude;
        velocityUpdate += DirectX::SimpleMath::Vector3::UnitY * jumpMag * m_vehicleStruct00.vehicleData.mass;
    }
    if (m_vehicleStruct00.vehicleData.jumpData.isJumpActive == false)
    {
        velocityUpdate += m_buoyancyTestForce;
    }

    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    velocityUpdate += gravForce * (m_vehicleStruct00.vehicleData.mass);

    DirectX::SimpleMath::Vector3 slopeForce = GetSlopeForce(m_vehicleStruct00.vehicleData.terrainNormal, m_vehicleStruct00.vehicleData.altitude, m_vehicleStruct00.vehicleData.hoverData.groundNormalForceRange);
    velocityUpdate += slopeForce;
    DirectX::SimpleMath::Vector3 forwardThrustUpdate = GetForwardThrust(m_vehicleStruct00.vehicleData);
    forwardThrustUpdate.y = 0.0f;
    velocityUpdate += forwardThrustUpdate;

    // adds some floating flutter movement to vehicles when stationary 
    if (m_npcAI->GetAiToggle() == false)
    {
        DirectX::SimpleMath::Vector3 flutter = DirectX::SimpleMath::Vector3::Zero;
        float flutterMod = (((cos((static_cast<float>(m_testTimer) * 2.0f) + m_testHoverFlutter) * 0.5f) + 0.0f) * 1.0f) + 1.0f;
        flutter.y = flutterMod * 7.0f;
        //velocityUpdate += flutter * mass;
    }

    velocityUpdate += m_vehicleStruct00.vehicleData.collisionImpulseForceSum;

    if (m_vehicleStruct00.vehicleData.altitude < 0.0f)
    {
        
        CalculateGroundImpactForce(velocityUpdate, angularVec);
    }

    m_vehicleStruct00.vehicleData.vehicleAngularForcesSum = angularVec;
    m_vehicleStruct00.vehicleData.angularDrag = CalculateDragAngularLocal(m_vehicleStruct00.vehicleData.q.angularVelocity, aTimeStep);

    m_vehicleStruct00.vehicleData.vehicleLinearForcesSum = velocityUpdate;
}

void NPCVehicle::CalculateGroundImpactForce(DirectX::SimpleMath::Vector3& aForce, DirectX::SimpleMath::Vector3& aTorque)
{
    DirectX::SimpleMath::Vector3 groundNormal = m_vehicleStruct00.vehicleData.terrainNormal;
    DirectX::SimpleMath::Vector3 bounceDirection = groundNormal;
    float bounceMag = m_vehicleStruct00.vehicleData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 bounceForce = bounceDirection * bounceMag * m_vehicleStruct00.vehicleData.mass;

    DirectX::SimpleMath::Vector3 preVelocity = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 terrainNorm = m_vehicleStruct00.vehicleData.terrainNormal;
    const float b = 0.8f;
    DirectX::SimpleMath::Vector3 postVelocity = b * (-2.0f * (preVelocity.Dot(terrainNorm)) * terrainNorm + preVelocity);

    if (postVelocity.y < 0.0f)
    {
        //postVelocity.y = 5.0f;
        //m_vehicleStruct00.vehicleData.q.position.y += 30.0f;
    }
    m_vehicleStruct00.vehicleData.q.velocity = postVelocity;

    DirectX::SimpleMath::Vector3 velocityNorm = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 upGroundCross = m_vehicleStruct00.vehicleData.up.Cross(terrainNorm);
    upGroundCross.Normalize();
    DirectX::SimpleMath::Vector3 velocityGroundCross = velocityNorm.Cross(terrainNorm);
    velocityGroundCross.Normalize();
    DirectX::SimpleMath::Vector3 testTorque = upGroundCross * (m_vehicleStruct00.vehicleData.q.velocity.Length() * m_vehicleStruct00.vehicleData.mass);
    testTorque = DirectX::SimpleMath::Vector3::Transform(testTorque, m_vehicleStruct00.vehicleData.alignmentInverse);
    aTorque += testTorque;

}


void NPCVehicle::SpawnerJumpUpdate(const float aTimeDelta)
{
    if (m_isSpawnJumpTriggerTrue == false)
    {
        m_spawnJumpTimer += aTimeDelta;
        if (m_spawnJumpTimer >= m_spawnerJumpTimerMax)
        {
            m_isSpawnJumpTriggerTrue = true;
            //m_vehicleStruct00.vehicleData.jumpData.isJumpReady = true;
            //m_vehicleStruct00.vehicleData.jumpData.isJumpUnlocked = true;
            //bool testJump = ActivateJump();
        }
    }
}