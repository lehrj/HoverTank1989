#include "pch.h"
#include "FireControl.h"


void FireControl::ActivateMuzzleFlash(AmmoType aAmmoType)
{
    if (aAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.4f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.25f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.45f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.3f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else // default to cannon muzzle flash aAmmoType == AmmoType::AMMOTYPE_CANNON
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.35f;
        m_muzzleFlash.growthRate = 20.0f;
    }
}

void FireControl::AltitudeController(MissileData& aMissile, const float aTimeDelta)
{
    const float altBuffer = 50.0f;

    if (aMissile.guidance.targetDistance > m_missileConsts.terminalRange)
    {
        aMissile.guidance.targetDestination.y += altBuffer;
        aMissile.guidance.targetPosition.y += altBuffer;
    }
}

void FireControl::CalculateAirDragTorque(MissileData& aMissile, const float aTimeDelta)
{
    //m_debugData->DebugClearUI();
    //m_debugData->ToggleDebugOnOverRide();

    DirectX::SimpleMath::Vector3 airVelocityLocalized = aMissile.projectileData.q.velocity;
    airVelocityLocalized = DirectX::SimpleMath::Vector3::Transform(airVelocityLocalized, aMissile.projectileData.inverseAlignmentQuat);
   // airVelocityLocalized = -DirectX::SimpleMath::Vector3::UnitX;
    const float angleOfAttack = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized);
    const float angleOfAttackWrapped = Utility::WrapAngleOnePi(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttack        deg = ", Utility::ToDegrees(angleOfAttack), "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttackWrapped deg = ", Utility::ToDegrees(angleOfAttackWrapped), "");
    //float sideSlipRatio = angleOfAttack / DirectX::XM_PIDIV2;
    //float sideSlipRatio = angleOfAttackWrapped / DirectX::XM_PIDIV2;
    float sideSlipRatio = abs(angleOfAttackWrapped / DirectX::XM_PIDIV2);
    //float sideSlipRatio =  DirectX::XM_PIDIV2;
    if (sideSlipRatio > 1.0f || sideSlipRatio < 0.0f)
    {
        //sideSlipRatio = 1.0f;
        int testBreak = 0;
        testBreak++;
    }
    
    m_debugData->DebugPushUILineDecimalNumber("sideSlipRatio = ", sideSlipRatio, "");

    const float airSpeed = airVelocityLocalized.Length();
    const float frontalArea = m_missileDimensions.y * m_missileDimensions.z;
    const float sideArea = m_missileDimensions.x * m_missileDimensions.y;
    const float areaDelta = sideArea - frontalArea;
    const float area = frontalArea + (sideArea * sideSlipRatio);

    //const float coefficientDragBase = m_missileConsts.dragCoefficientAng; // 0.3f
    const float coefficientDragBase = 0.3;  
    const float coefficientDragModMax = 0.9f;
    const float coeffiecentDragSum = coefficientDragBase + (coefficientDragModMax * sideSlipRatio);
    //DirectX::SimpleMath::Vector3 centerOfMass = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 centerOfPressure = m_missileConsts.centerOfPressureBasePosLocal;
    centerOfPressure += m_missileConsts.centerOfPressureFullFinDeployOffset * aMissile.guidance.finDeployPercent;


    // F = A x P x Cd
    // P = 0.613 X V^2
    const float p = 0.613f * (airSpeed * airSpeed);
    DirectX::SimpleMath::Vector3 forcePoint = centerOfPressure;
    DirectX::SimpleMath::Vector3 forceVec = airVelocityLocalized;
    forceVec.Normalize();
    //forceVec *= airSpeed * area;
    forceVec *= p * area * coeffiecentDragSum;
    forceVec *= 0.3f;
    m_debugData->DebugPushUILineDecimalNumber("coeffiecentDragSum = ", coeffiecentDragSum, "");
    m_debugData->DebugPushUILineDecimalNumber("area               = ", area, "");
    m_debugData->DebugPushUILineDecimalNumber("p                  = ", p, "");
    m_debugData->DebugPushUILineDecimalNumber("forceVec.Length    = ", forceVec.Length(), "");
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceVec, forcePoint, centerOfMass, forceAccum, torqueAccum);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceAccum.Length() =", forceAccum.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("torqueAccum.Length() =", torqueAccum.Length(), "");
    m_debugData->ToggleDebugOff();
    //aMissile.guidance.airDragTorqueLocal = torqueAccum;
    aMissile.guidance.airDragTorqueLocalTest = torqueAccum;
  //  aMissile.guidance.airDragForceLocalTest = forceAccum;
    
    DirectX::SimpleMath::Vector3 centerOfPressureWorld = centerOfPressure;
    centerOfPressureWorld = DirectX::SimpleMath::Vector3::Transform(centerOfPressureWorld, aMissile.projectileData.alignmentQuat);
    centerOfPressureWorld += aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 localVelocityToWorld = airVelocityLocalized;
    localVelocityToWorld *= -1.0f;
    localVelocityToWorld = DirectX::SimpleMath::Vector3::Transform(localVelocityToWorld, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOn();
    //m_debugData->PushDebugLinePositionIndicator(centerOfPressureWorld, 10.0f, 0.0f, DirectX::Colors::Lime);
    m_debugData->PushDebugLine(centerOfPressureWorld, aMissile.projectileData.up, 5.0f, 0.0f, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(centerOfPressureWorld, aMissile.projectileData.right, 5.0f, 0.0f, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(centerOfPressureWorld, -aMissile.projectileData.up, 5.0f, 0.0f, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(centerOfPressureWorld, -aMissile.projectileData.right, 5.0f, 0.0f, DirectX::Colors::LightBlue);

    m_debugData->PushDebugLinePositionIndicatorAligned(aMissile.projectileData.q.position, 5.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicatorAligned(centerOfPressureWorld, 5.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(centerOfPressureWorld, localVelocityToWorld, 10.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->ToggleDebugOff();
}

DirectX::SimpleMath::Vector3 FireControl::CalculateBoostForceVec(MissileData& aMissile)
{
    if (aMissile.guidance.isRocketFired == false)
    {
        return DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.heading, 50.0f, 0.0f, DirectX::Colors::Lime);
        //return - aMissile.guidance.heading * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage);
        //return aMissile.guidance.steeringDirNormLocal * (m_missileConsts.rocketBoostForceMax * -aMissile.guidance.throttlePercentage);
        DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(aMissile.guidance.steeringQuat);
        testMat = testMat.Invert();

        DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, testMat);
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile.guidance.steeringQuat);
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, Utility::ToRadians(45.0f)));
        return testLine * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage);
    }
}

void FireControl::CalculateDragAngularSumLocal(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aMissile.projectileData.q.angularVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;
    // float length = m_inertiaModelX;
    //const float width = m_inertiaModelZ;
    //const float height = m_inertiaModelY;

    const float length = m_missileDimensions.x;
    const float width = m_missileDimensions.z;
    const float height = m_missileDimensions.y;

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
    m_debugData->ToggleDebugOn();
    m_debugData->DebugPushUILineDecimalNumber("radiusSum = ", radiusSum, "");

    m_debugData->DebugPushUILineDecimalNumber("yawRadius   = ", yawRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("pitchRadius = ", pitchRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("rollRadius  = ", rollRadius, "");

    m_debugData->ToggleDebugOff();
    float angVelocityF = aMissile.projectileData.q.angularVelocity.Length();

    //float angDragCoefficient = m_angDragCoefficient;
    float angDragCoefficient = 0.03f;

    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float radius = radiusSum;
    //float angFrontSurfaceArea = airSurfaceArea;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);

    aMissile.projectileData.angularDragSum = angularDrag;
    //return angularDrag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragAngularSumLocal(MissileData& aMissile)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aMissile.projectileData.q.angularVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;
    // float length = m_inertiaModelX;
    //const float width = m_inertiaModelZ;
    //const float height = m_inertiaModelY;

    const float length = m_missileDimensions.x;
    const float width = m_missileDimensions.z;
    const float height = m_missileDimensions.y;

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
    //m_debugData->ToggleDebugOn();
    m_debugData->DebugPushUILineDecimalNumber("radiusSum = ", radiusSum, "");

    m_debugData->DebugPushUILineDecimalNumber("yawRadius   = ", yawRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("pitchRadius = ", pitchRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("rollRadius  = ", rollRadius, "");

    //m_debugData->ToggleDebugOff();
    float angVelocityF = aMissile.projectileData.q.angularVelocity.Length();

    //float angDragCoefficient = 0.03f;
    float angDragCoefficient = m_missileConsts.dragCoefficientAng;

    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float radius = radiusSum;
    //float angFrontSurfaceArea = airSurfaceArea;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);
    //aMissile.projectileData.angularDragSum = angularDrag;

    return angularDrag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragAngularSumLocalTest(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aMissile.projectileData.q.angularVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;
    // float length = m_inertiaModelX;
    //const float width = m_inertiaModelZ;
    //const float height = m_inertiaModelY;

    const float length = m_missileDimensions.x;
    const float width = m_missileDimensions.z;
    const float height = m_missileDimensions.y;

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
    //m_debugData->ToggleDebugOn();
    m_debugData->DebugPushUILineDecimalNumber("radiusSum = ", radiusSum, "");

    m_debugData->DebugPushUILineDecimalNumber("yawRadius   = ", yawRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("pitchRadius = ", pitchRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("rollRadius  = ", rollRadius, "");

    //m_debugData->ToggleDebugOff();
    float angVelocityF = aMissile.projectileData.q.angularVelocity.Length();

    //float angDragCoefficient = 0.03f;
    float angDragCoefficient = m_missileConsts.dragCoefficientAng;

    float angAirDensity = m_environment->GetAirDensity();
    //float angFrontSurfaceArea = m_heli.area;
    float radius = radiusSum;
    //float angFrontSurfaceArea = airSurfaceArea;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);
    //aMissile.projectileData.angularDragSum = angularDrag;

    const float angularDragMod = m_missileConsts.angularDragMod;
    DirectX::SimpleMath::Vector3 angularDragPowTest = aMissile.projectileData.q.angularVelocity * -powf(angularDragMod, aTimeDelta);
    DirectX::SimpleMath::Vector3 angularDragPowTest2 = aMissile.projectileData.q.angularVelocity * -powf(0.2f, aTimeDelta);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("angularDrag        = ", angularDrag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("angularDragPowTest = ", angularDragPowTest.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("angularDragPowTest2 = ", angularDragPowTest2.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("Delta2             = ", angularDragPowTest.Length() - angularDragPowTest2.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("Delta3             = ", angularDragPowTest2.Length() - angularDragPowTest.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("Delta              = ", angularDrag.Length() - angularDragPowTest.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("Delta2             = ", angularDragPowTest.Length() - angularDrag.Length(), "");
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, angularDrag, 35.0, 0.0f, DirectX::Colors::Orange);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, angularDragPowTest, 20.0, 0.0f, DirectX::Colors::Teal);
    m_debugData->ToggleDebugOff();
    return angularDrag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragLinearForRunge(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aVelocity;
    localVelocityNorm.Normalize();
    //localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, m_heli.alignmentInverse);
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile->projectileData.inverseAlignmentQuat);
    /*
    float frontSurfaceArea = m_inertiaModelY * m_inertiaModelZ;
    float sideSurfaceArea = m_inertiaModelX * m_inertiaModelY;
    float topSurfaceArea = m_inertiaModelX * m_inertiaModelZ;
    */
    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);


    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    float airSurfaceArea = frontSurface + sideSurface + topSurface;
    //m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea = ", airSurfaceArea, "");


    airSurfaceArea = 0.785f;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    //float dragCoefficient = m_heli.dragCoefficient;
    const float dragCoefficient = 0.17f;
    float velocity = aVelocity.Length();
    //float velocity = aNewQVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;

    //DirectX::SimpleMath::Vector3 linearDrag = DirectX::SimpleMath::Vector3::Zero;
    //return linearDrag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragLinearForRungeTest(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aVelocity;
    localVelocityNorm.Normalize();
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile->projectileData.inverseAlignmentQuat);

    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    float airSurfaceArea = frontSurface + sideSurface + topSurface;
    airSurfaceArea = 0.785f;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    const float dragCoefficient = 0.17f;
    float velocity = aVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragLinearForAccumulator(MissileData& aMissile)
{
    //DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aMissile.projectileData.q.velocity;
    localVelocityNorm.Normalize();
    //localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile->projectileData.inverseAlignmentQuat);
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);

    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    float airSurfaceArea = frontSurface + sideSurface + topSurface;
    airSurfaceArea = 0.785f;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    const float dragCoefficient = 0.17f;
    float velocity = aMissile.projectileData.q.velocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    drag = DirectX::SimpleMath::Vector3::Transform(drag, aMissile.projectileData.inverseAlignmentQuat);

    return drag;
}

void FireControl::CalculateGimbaledThrust(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 angularMomentum = aMissile.projectileData.q.angularMomentum;
    DirectX::SimpleMath::Vector3 angularVelocity = aMissile.projectileData.q.angularVelocity;
    angularVelocity *= -1.0f;
    /*
    m_debugData->DebugPushUILineDecimalNumber("angularMomentum.x = ", angularMomentum.x, "");
    m_debugData->DebugPushUILineDecimalNumber("angularMomentum.y = ", angularMomentum.y, "");
    m_debugData->DebugPushUILineDecimalNumber("angularMomentum.z = ", angularMomentum.z, "");

    m_debugData->DebugPushUILineDecimalNumber("angularVelocity.x = ", angularVelocity.x, "");
    m_debugData->DebugPushUILineDecimalNumber("angularVelocity.y = ", angularVelocity.y, "");
    m_debugData->DebugPushUILineDecimalNumber("angularVelocity.z = ", angularVelocity.z, "");
    */

    DirectX::SimpleMath::Vector3 angularVelocityModified = angularVelocity;
    const float angularDamp = 1.0f;
    angularVelocityModified *= angularDamp;
    updateQuat.x = angularVelocityModified.x;
    updateQuat.y = angularVelocityModified.y;
    updateQuat.z = angularVelocityModified.z;
    updateQuat.w = 0.0f;
    DirectX::SimpleMath::Quaternion updateOutputQuat = aMissile.projectileData.alignmentQuat;
    updateOutputQuat.Normalize();
    updateOutputQuat += (0.5f * aTimeDelta) * (updateQuat * aMissile.projectileData.alignmentQuat);
    updateOutputQuat.Normalize();

    DirectX::SimpleMath::Quaternion updateOutputQuat2 = DirectX::SimpleMath::Quaternion::Identity;
    updateOutputQuat2.Normalize();
    updateOutputQuat2 += (0.5f * aTimeDelta) * (updateQuat * aMissile.projectileData.alignmentQuat);
    updateOutputQuat2.Normalize();


    //DirectX::SimpleMath::Vector3 updateHeading = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 updateHeading = aMissile.guidance.heading;
    updateHeading = DirectX::SimpleMath::Vector3::Transform(updateHeading, updateOutputQuat2);
    aMissile.guidance.heading = updateHeading;

    //aMissile.projectileData.alignmentQuat = updateOutputQuat;
    // m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 xLine = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 yLine = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 zLine = DirectX::SimpleMath::Vector3::UnitZ;

    xLine = DirectX::SimpleMath::Vector3::Transform(xLine, updateOutputQuat);
    yLine = DirectX::SimpleMath::Vector3::Transform(yLine, updateOutputQuat);
    zLine = DirectX::SimpleMath::Vector3::Transform(zLine, updateOutputQuat);

    /*
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, xLine, 200.0f, 0.5f, DirectX::Colors::SkyBlue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, yLine, 200.0f, 0.5f, DirectX::Colors::SkyBlue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, zLine, 200.0f, 0.5f, DirectX::Colors::SkyBlue);
    */

    xLine = DirectX::SimpleMath::Vector3::UnitX;
    yLine = DirectX::SimpleMath::Vector3::UnitY;
    zLine = DirectX::SimpleMath::Vector3::UnitZ;

    xLine = DirectX::SimpleMath::Vector3::Transform(xLine, updateOutputQuat2);
    yLine = DirectX::SimpleMath::Vector3::Transform(yLine, updateOutputQuat2);
    zLine = DirectX::SimpleMath::Vector3::Transform(zLine, updateOutputQuat2);

    /*
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, xLine, 200.0f, 0.5f, DirectX::Colors::Orange);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, yLine, 200.0f, 0.5f, DirectX::Colors::Orange);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, zLine, 200.0f, 0.5f, DirectX::Colors::Orange);
    */

    DirectX::SimpleMath::Vector3 testHeading = aMissile.guidance.heading;
    testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, aMissile.projectileData.alignmentQuat);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.heading, 200.0f, 0.0f, DirectX::Colors::Lime);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, testHeading, 200.0f, 0.5f, DirectX::Colors::SkyBlue);

    
    //aMissile.guidance.heading = aMissile.projectileData.right;
    //aMissile.guidance.headingLocal = DirectX::SimpleMath::Vector3::UnitZ;

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, DirectX::SimpleMath::Vector3::UnitY , 400.0f, 0.5f, DirectX::Colors::White);

    /*
    DirectX::SimpleMath::Vector3 postHeading = aMissile.guidance.heading;
    float angle = Utility::GetAngleBetweenVectors(preHeading, postHeading);
    m_debugData->DebugPushUILineDecimalNumber("angle rads = ", angle, "");
    m_debugData->DebugPushUILineDecimalNumber("angle degs = ", Utility::ToDegrees(angle), "");
    */
}

DirectX::SimpleMath::Vector3 FireControl::CalculeteDragLinearSum(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aMissile.projectileData.q.velocity;
    localVelocityNorm.Normalize();
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);

    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    float airSurfaceArea = frontSurface + sideSurface + topSurface;
    airSurfaceArea = 0.785f;
    airSurfaceArea = 1.0075f;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    const float dragCoefficient = 0.17f;
    float velocity = aMissile.projectileData.q.velocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    ///////////////////////////////////////////////////////////////////

    float airSurfaceArea2 = frontSurface + sideSurface + topSurface;
    float dragResistance2 = 0.5f * airDensity * airSurfaceArea2 * dragCoefficient * velocity * velocity;
    //DirectX::SimpleMath::Vector3 airResistance2 = velocityNorm * (-dragResistance2);
    DirectX::SimpleMath::Vector3 airResistance2 = localVelocityNorm * (-dragResistance2);
    DirectX::SimpleMath::Vector3 drag2 = airResistance2;

    /*
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea2  = ", airSurfaceArea2, "");
    m_debugData->DebugPushUILineDecimalNumber("drag  = ", drag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("drag2 = ", drag2.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("delta = ", drag.Length() - drag2.Length(), "");
    m_debugData->ToggleDebugOff();
    */

    return drag2;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateWindVaningTorqueForce(const MissileData& aMissile)
{
    DirectX::SimpleMath::Vector3 localizedAirVelocity = -aMissile.projectileData.q.velocity;
    DirectX::SimpleMath::Vector3 localizedAirVelocityTest1 = localizedAirVelocity;
    DirectX::SimpleMath::Vector3 localizedAirVelocityTest2 = localizedAirVelocity;
    DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(aMissile.projectileData.alignmentQuat);
    localizeMat = localizeMat.Invert();
    localizedAirVelocity = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocity, localizeMat);

    DirectX::SimpleMath::Vector3 localizedAirVelocityLateral = localizedAirVelocity;
    localizedAirVelocityLateral.x = 0.0f;
    DirectX::SimpleMath::Vector3 localizedAirVelocityLongitudinal = localizedAirVelocity;
    localizedAirVelocityLongitudinal.z = 0.0f;

    DirectX::SimpleMath::Vector3 airVelocityNormXZ = localizedAirVelocity;
    airVelocityNormXZ.y = 0.0f;
    airVelocityNormXZ.Normalize();
    float ratio = airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ);

    float v = aMissile.projectileData.q.velocity.Length();
    //float airDensity = aHeliData.airDensity;
    float airDensity = m_environment->GetAirDensity();
    //float dragCoefficient = aHeliData.dragCoefficient;
    float dragCoefficient = m_missileConsts.dragCoefficient;
    //float surfaceArea = aHeliData.area;
    float surfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float dragResistance = 0.5f * airDensity * surfaceArea * dragCoefficient * v * v;
    float windVaning = -ratio * dragResistance * 0.0000005f;

    const float dragVal = airDensity * surfaceArea * dragCoefficient;

    DirectX::SimpleMath::Vector3 lateralCross = localizedAirVelocity.Cross(DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 longitudunalCross = localizedAirVelocity.Cross(DirectX::SimpleMath::Vector3::UnitZ);

    bool isVelocityForward;
    if (longitudunalCross.y > 0.0)
    {
        isVelocityForward = true;
    }
    else
    {
        isVelocityForward = false;
    }

    bool isVelocityLeft;
    if (lateralCross.y > 0.0)
    {
        isVelocityLeft = true;
    }
    else
    {
        isVelocityLeft = false;
    }

    const DirectX::SimpleMath::Vector3 localCenterOfMass = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 localPhysicsPointRear = -DirectX::SimpleMath::Vector3::UnitX;
    const DirectX::SimpleMath::Vector3 localPhysicsPointFront = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 longitudeTorqueArm;
    if (isVelocityForward == true)
    {
        //longitudeTorqueArm = aHeliData.localPhysicsPointRear - aHeliData.localCenterOfMass;
        longitudeTorqueArm = localPhysicsPointRear - localCenterOfMass;
    }
    else
    {
        //longitudeTorqueArm = aHeliData.localPhysicsPointFront - aHeliData.localCenterOfMass;
        longitudeTorqueArm = localPhysicsPointFront - localCenterOfMass;
    }

    const DirectX::SimpleMath::Vector3 localPhysicsPointRight = DirectX::SimpleMath::Vector3::UnitZ;
    const DirectX::SimpleMath::Vector3 localPhysicsPointLeft = -DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 latTorqueArm;
    if (isVelocityLeft == true)
    {
        //latTorqueArm = aHeliData.localPhysicsPointRight - aHeliData.localCenterOfMass;
        latTorqueArm = localPhysicsPointRight - localCenterOfMass;
    }
    else
    {
        //latTorqueArm = aHeliData.localPhysicsPointLeft - aHeliData.localCenterOfMass;
        latTorqueArm = localPhysicsPointLeft - localCenterOfMass;
    }

    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aHeliData.right, aHeliData.up);
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aMissile.projectileData.right, aMissile.projectileData.up);

    const float windMod = 1.0f;
    float latDotMod = abs(airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ) * windMod);
    float longDotMod = abs(airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitX) * windMod);

    Utility::Torque lateralTorque = Utility::GetTorqueForce(latTorqueArm, (localizedAirVelocityLateral * latDotMod));
    Utility::Torque longitudeTorque = Utility::GetTorqueForce(longitudeTorqueArm, (localizedAirVelocityLongitudinal * longDotMod));

    DirectX::SimpleMath::Vector3 testLongTorque = longitudeTorque.axis * longitudeTorque.magnitude;
    DirectX::SimpleMath::Vector3 testLatTorque = lateralTorque.axis * lateralTorque.magnitude;
    testLongTorque = DirectX::SimpleMath::Vector3::Transform(testLongTorque, updateMat);
    testLatTorque = DirectX::SimpleMath::Vector3::Transform(testLatTorque, updateMat);

    localizedAirVelocityLateral = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocityLateral, updateMat);
    localizedAirVelocityLongitudinal = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocityLongitudinal, updateMat);

    DirectX::SimpleMath::Vector3 windVaningUpdateForce = DirectX::SimpleMath::Vector3::Zero;

    windVaningUpdateForce = (lateralTorque.axis * lateralTorque.magnitude) + (longitudeTorque.axis * longitudeTorque.magnitude);
    windVaningUpdateForce = (testLatTorque)+(testLongTorque);

    return windVaningUpdateForce;
}

void FireControl::CastRayLaser()
{
    DirectX::SimpleMath::Ray laserRay = DirectX::SimpleMath::Ray(m_playerVehicle->GetMuzzlePos(), m_playerVehicle->GetWeaponDirection());
    bool isTargetHit = false;
    float distanceToTarget = 0.0f;
    //int targetID = m_npcController->CheckTargetingLaser(laserRay, distanceToTarget);
    int targetID = -1;

    m_npcController->CheckTargetingLaser(laserRay, distanceToTarget, targetID, isTargetHit);
    m_playerLaser.distance = distanceToTarget;
    //m_debugData->DebugPushUILineWholeNumber("m_playerLaser.distance = ", m_playerLaser.distance, "");
    m_debugData->DebugPushUILineWholeNumber("targetID    = ", targetID, "");
    m_debugData->DebugPushUILineWholeNumber("isTargetHit = ", isTargetHit, "");
    m_currentTargetID = targetID;
    m_isTargetingLaserLockTrue = isTargetHit;
}

void FireControl::CheckCollisions()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        unsigned int vehicleHitId = 0;
        bool isHitTrue;
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE || m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId, true);
        }
        else
        {
            isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId, false);
        }

        //bool isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId);
        if (isHitTrue == true)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE || m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
            {
                CreateExplosion(m_projectileVec[i].q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE, static_cast<unsigned int>(vehicleHitId));
            }
            m_projectileVec[i].isCollisionTrue = true;
            m_projectileVec[i].liveTimeTick--;
            if (m_projectileVec[i].liveTimeTick <= 0)
            {
                m_projectileVec[i].isDeleteTrue = true;
            }
        }
        else if (m_projectileVec[i].time > m_projectileLifeTimeMax)
        {
            m_projectileVec[i].isDeleteTrue = true;
        }
        else if (m_environment->GetIsPosInPlay(m_projectileVec[i].q.position) == false)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
            {
                CreateExplosion(m_projectileVec[i].q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_NONVEHICLE, -1);
            }
            m_projectileVec[i].isDeleteTrue = true;
        }
    }
}

void FireControl::CheckCollisionsMissile()
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        if (m_missileVec[i].guidance.isExplodingTrue == false)
        {
            unsigned int vehicleHitId = -1;
            bool isHitTrue;
            bool isProximityDetonationTrue = false;
            isHitTrue = m_npcController->CheckProjectileCollisionsMissile(m_missileVec[i].projectileData.collisionData, vehicleHitId, true, m_missileVec[i].guidance.targetID, m_missileConsts.detonationRange, isProximityDetonationTrue);

            if (isProximityDetonationTrue == true)
            {
                CreateExplosion(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.q.velocity, ExplosionType::EXPLOSIONTYPE_DYNAMIC, -1);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (vehicleHitId != -1 && isHitTrue == true)
            {
                CreateExplosion(m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE, vehicleHitId);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (isHitTrue == true)
            {
                /*
                m_missileVec[i].projectileData.isCollisionTrue = true;
                m_missileVec[i].projectileData.liveTimeTick--;
                if (m_missileVec[i].projectileData.liveTimeTick <= 0)
                {
                    m_missileVec[i].projectileData.isDeleteTrue = true;
                }
                */
                CreateExplosion(m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_NONVEHICLE, -1);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_missileVec[i].projectileData.time > m_missileLifeTimeMax)
            {
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_environment->GetIsPosInPlay(m_missileVec[i].projectileData.q.position) == false)
            {
                //m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_missileVec[i].guidance.isSelfDestructTrue == true)
            {
                /*
                CreateExplosion(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.q.velocity, ExplosionType::EXPLOSIONTYPE_DYNAMIC, -1);
                //m_missileVec[i].projectileData.isDeleteTrue = true;

                m_missileVec[i].guidance.isExplodingTrue = true;
                m_missileVec[i].guidance.isRocketFired = false;
                m_missileVec[i].guidance.forwardThrust = 0.0f;
                m_missileVec[i].guidance.throttlePercentage = 0.0f;
                */
                m_missileVec[i].guidance.isRocketFired = false;
            }
        }
    }
}

void FireControl::CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aVelocity, ExplosionType aExplosionType, const int aVehicleId)
{
    ExplosionData createdExplosion;
    createdExplosion.position = aPos;
    createdExplosion.explosionStartColor = m_explosionStruct.explosionRefData.explosionStartColor;
    createdExplosion.explosionEndColor = m_explosionStruct.explosionRefData.explosionEndColor;
    createdExplosion.explosionCurrentColor = createdExplosion.explosionStartColor;
    createdExplosion.color0 = createdExplosion.explosionStartColor;
    createdExplosion.color1 = createdExplosion.explosionStartColor;
    createdExplosion.color2 = createdExplosion.explosionStartColor;
    createdExplosion.color3 = createdExplosion.explosionStartColor;
    createdExplosion.color4 = createdExplosion.explosionStartColor;
    createdExplosion.color5 = createdExplosion.explosionStartColor;
    createdExplosion.color6 = createdExplosion.explosionStartColor;
    createdExplosion.color7 = createdExplosion.explosionStartColor;
    createdExplosion.color8 = createdExplosion.explosionStartColor;

    createdExplosion.currentDuration = 0.0f;
    createdExplosion.currentRadius = m_explosionStruct.explosionRefData.currentRadius;

    createdExplosion.initialRadius = m_explosionStruct.explosionRefData.initialRadius;
    createdExplosion.collisionSphere = m_explosionStruct.explosionRefData.collisionSphere;
    createdExplosion.collisionSphere.Center = aPos;
    createdExplosion.collisionSphere.Radius = createdExplosion.initialRadius;
    createdExplosion.localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(aPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    createdExplosion.explosionMatrix0 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix1 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix2 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix3 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix4 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix5 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix6 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix7 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix8 = createdExplosion.localExplosionMatrix;

    createdExplosion.maxRadius = m_explosionStruct.explosionRefData.maxRadius;

    createdExplosion.totalDuration = m_explosionStruct.explosionRefData.totalDuration;
    createdExplosion.isLifeTimeExpired = false;

    const float randomRotationVariation = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Utility::GetPi())));
    createdExplosion.rotationVariationMatrix = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(randomRotationVariation, randomRotationVariation, randomRotationVariation);

    if (aExplosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL;
        createdExplosion.vehicleExplosionID = aVehicleId;
    }
    else if (aExplosionType == ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE;
        createdExplosion.vehicleExplosionID = aVehicleId;
        DirectX::SimpleMath::Matrix vehicleAlignment = m_npcController->GetNpcAlignment(aVehicleId);
        vehicleAlignment = vehicleAlignment.Invert();
        DirectX::SimpleMath::Vector3 posOnVehicleSurface = aPos - m_npcController->GetNpcPos(aVehicleId);
        posOnVehicleSurface = DirectX::SimpleMath::Vector3::Transform(posOnVehicleSurface, vehicleAlignment);
        createdExplosion.localizedSurfaceExplosionPos = posOnVehicleSurface;
    }
    else if (aExplosionType == ExplosionType::EXPLOSIONTYPE_DYNAMIC)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_DYNAMIC;
        createdExplosion.velocity = aVelocity;
        createdExplosion.vehicleExplosionID = -1;
    }
    else
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_NONVEHICLE;
        createdExplosion.vehicleExplosionID = -1;
    }
    m_explosionStruct.explosionVec.push_back(createdExplosion);
}

void FireControl::CycleLoadedAmmo()
{
    if (m_currentAmmoType == AmmoType::AMMOTYPE_CANNON)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_MACHINEGUN;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_SHOTGUN;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_EXPLOSIVE;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_MIRV;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_GUIDEDMISSILE)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_CANNON;
    }
}

void FireControl::DeleteMissileFromVec(const unsigned int aIndex)
{
    if (aIndex > m_missileVec.size())
    {
        // add error handling here
    }
    else
    {
        std::vector<MissileData>::iterator it;
        it = m_missileVec.begin() + aIndex;
        m_missileVec.erase(it);
    }
}

void FireControl::DeleteProjectileFromVec(const unsigned int aIndex)
{
    if (aIndex > m_projectileVec.size())
    {
        // add error handling here
    }
    else
    {
        std::vector<ProjectileData>::iterator it;
        it = m_projectileVec.begin() + aIndex;
        m_projectileVec.erase(it);
    }
}

void FireControl::DeployMirv(ProjectileData& aProjectile)
{
    AmmoData firedAmmo = m_ammoExplosive.ammoData;

    ProjectileData firedProjectile;
    firedProjectile.ammoData = firedAmmo;
    firedProjectile.q.position = aProjectile.q.position;
    firedProjectile.q.velocity = aProjectile.q.velocity;
    firedProjectile.forward = aProjectile.forward;
    firedProjectile.right = aProjectile.right;
    firedProjectile.up = aProjectile.up;
    firedProjectile.alignmentQuat = aProjectile.alignmentQuat;
    firedProjectile.inverseAlignmentQuat = aProjectile.inverseAlignmentQuat;

    firedProjectile.isCollisionTrue = false;
    firedProjectile.isDeleteTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
    firedProjectile.time = 0.0f;

    // collision data
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

    DirectX::SimpleMath::Vector3 velocityNorm = firedProjectile.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 right = velocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
    right = aProjectile.right;
    DirectX::SimpleMath::Vector3 up = velocityNorm.Cross(-right);
    right = velocityNorm.Cross(up);

    DirectX::SimpleMath::Vector3 forward = aProjectile.forward;
    right = aProjectile.right;
    up = aProjectile.up;

    const int rowCount = 8;
    const int columnCount = 5;

    float rowSpacingAngle = Utility::ToRadians(2.0f);
    float columnSpacingAngle = Utility::ToRadians(12.0f);
    float yawOrg = columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f);
    float pitchOrg = rowSpacingAngle * (static_cast<float>(rowCount) * 0.5f) - (rowSpacingAngle * 0.5f);
    float yawVal = yawOrg;
    float pitchVal = pitchOrg;

    DirectX::SimpleMath::Vector3 updateVelocity = firedProjectile.q.velocity;

    // calculate angle at deploy
    DirectX::SimpleMath::Vector3 flatForward = right.Cross(-DirectX::SimpleMath::Vector3::UnitY);
    const float flatDelta = Utility::GetAngleBetweenVectors(forward, flatForward);

    const float deployDownAngle = Utility::ToRadians(-30.0f) - flatDelta;
    float deployDownMod = Utility::ToRadians(-15.0f);
    DirectX::SimpleMath::Quaternion deployDownQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle);
    //updateVelocity = DirectX::SimpleMath::Vector3::Transform(updateVelocity, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle));
    updateVelocity = DirectX::SimpleMath::Vector3::Transform(updateVelocity, deployDownQuat);
    const float velocityBoostMod = 2.0f;
    updateVelocity *= velocityBoostMod;

    const float angleOffsetMin = Utility::ToRadians(-0.5f);
    const float angleOffSetMax = Utility::ToRadians(0.5f);
    float yawMod = 0.0f;
    float rowMod = 0.0f;
    float yawTestMod = Utility::ToRadians(2.0f);
    std::vector<float> testYawVec;
    testYawVec.clear();
    std::vector<float> testPitchVec;
    testPitchVec.clear();
    for (int i = 0; i < rowCount; ++i)
    {
        if (static_cast<float>(i) < static_cast<float>(rowCount * 0.5f))
        {
            columnSpacingAngle += yawTestMod;
            yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));
        }
        else
        {
            columnSpacingAngle -= yawTestMod;
            yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));
        }
        for (int j = 0; j < columnCount; ++j)
        {
            if (static_cast<float>(i) <= static_cast<float>(rowCount * 0.5f))
            {
                if (static_cast<float>(j) <= static_cast<float>(columnCount * 0.5f))
                {
                    //yawVal -= yawTestMod;
                }
                else
                {
                    //yawVal += yawTestMod;
                }
            }
            else
            {
                if (static_cast<float>(j) <= static_cast<float>(columnCount * 0.5f))
                {
                    //yawVal += yawTestMod;
                }
                else
                {
                    //yawVal -= yawTestMod;
                }
            }

            ProjectileData mirv = firedProjectile;
            DirectX::SimpleMath::Matrix rotMatYaw = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.up, yawVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            DirectX::SimpleMath::Matrix rotMatPitch = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.right, pitchVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            //DirectX::SimpleMath::Matrix rotMatYaw = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.up, yawVal);// +angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            //DirectX::SimpleMath::Matrix rotMatPitch = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.right, pitchVal);// +angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));

            DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::Identity;
            rotMat *= rotMatYaw;
            //rotMat *= rotMatPitch;
            if (j == 0)
            {
                testPitchVec.push_back(Utility::ToDegrees(pitchVal));
                testYawVec.push_back(Utility::ToDegrees(yawVal));
            }

            DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Identity;
            //updateQuat *= deployDownQuat;
            updateQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rotMat);
            updateQuat *= deployDownQuat;
            //mirv.alignmentQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rotMat);

            mirv.alignmentQuat *= updateQuat;
            mirv.up = DirectX::SimpleMath::Vector3::Transform(mirv.up, updateQuat);
            mirv.right = DirectX::SimpleMath::Vector3::Transform(mirv.right, updateQuat);
            mirv.forward = DirectX::SimpleMath::Vector3::Transform(mirv.forward, updateQuat);

            mirv.q.velocity = mirv.forward * updateVelocity.Length();
            if (j == 0)
            {
                //m_newProjectilePushVec.push_back(mirv);
            }

            m_newProjectilePushVec.push_back(mirv);
            yawVal -= columnSpacingAngle;

        }
        //yawVal = yawOrg;
        columnSpacingAngle += Utility::ToRadians(0.0f);// *static_cast<float>(i + 1);
        yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));// -((static_cast<float>(i) * Utility::ToRadians(5.0f)) * 0.5f);
        pitchVal -= rowSpacingAngle;
        deployDownQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle + (deployDownMod * static_cast<float>(i + 1)));
    }

    aProjectile.isDeleteTrue = true;
}

void FireControl::DetonateAllMissiles()
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        m_missileVec[i].guidance.isSelfDestructTrue = true;
        m_missileVec[i].projectileData.isDeleteTrue = true; // placeholder till detonation pipeline is built
    }
}

void FireControl::DrawExplosions(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix0, aView, aProj, m_explosionStruct.explosionVec[i].explosionCurrentColor);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix1, aView, aProj, m_explosionStruct.explosionVec[i].color1);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix2, aView, aProj, m_explosionStruct.explosionVec[i].color2);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix3, aView, aProj, m_explosionStruct.explosionVec[i].color3);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix4, aView, aProj, m_explosionStruct.explosionVec[i].color4);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix5, aView, aProj, m_explosionStruct.explosionVec[i].color5);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix6, aView, aProj, m_explosionStruct.explosionVec[i].color6);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix7, aView, aProj, m_explosionStruct.explosionVec[i].color7);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix8, aView, aProj, m_explosionStruct.explosionVec[i].color8);
    }
}

void FireControl::DrawExplosions2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        aEffect->EnableDefaultLighting();

        const float high = Utility::GetPi();
        const float low = -Utility::GetPi();
        float yaw = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        float pitch = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        float roll = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

        const float t = 0.9999f;
        DirectX::SimpleMath::Vector3 lightDir0 = m_explosionStruct.explosionVec[i].lightDir0;
        DirectX::SimpleMath::Matrix lightRot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
        lightDir0 = DirectX::SimpleMath::Vector3::Transform(lightDir0, lightRot);
        lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir0, lightDir0, t);
        m_explosionStruct.explosionVec[i].lightDir0 = lightDir0;

        DirectX::SimpleMath::Vector3 lightDir1 = m_explosionStruct.explosionVec[i].lightDir1;
        lightDir0 = DirectX::SimpleMath::Vector3::Transform(lightDir1, lightRot);
        lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir1, lightDir1, t);
        m_explosionStruct.explosionVec[i].lightDir1 = lightDir1;

        DirectX::SimpleMath::Vector3 lightDir2 = m_explosionStruct.explosionVec[i].lightDir2;
        lightDir2 = DirectX::SimpleMath::Vector3::Transform(lightDir2, lightRot);
        lightDir2 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir2, lightDir2, t);
        m_explosionStruct.explosionVec[i].lightDir2 = lightDir2;

        aEffect->SetLightDirection(0, m_explosionStruct.explosionVec[i].lightDir0);
        aEffect->SetLightDirection(1, m_explosionStruct.explosionVec[i].lightDir1);
        aEffect->SetLightDirection(2, m_explosionStruct.explosionVec[i].lightDir2);

        DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
        DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
        DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

        float durationMod = m_explosionStruct.explosionVec[i].currentDuration / m_explosionStruct.explosionVec[i].totalDuration;
        durationMod = 1.0f;
        DirectX::SimpleMath::Vector3 camPos = m_environment->GetCameraPos();
        DirectX::SimpleMath::Vector3 expPos = m_explosionStruct.explosionVec[i].position;
        DirectX::SimpleMath::Vector3 dirNormToExplosion = expPos - camPos;
        dirNormToExplosion.Normalize();
        float distanceToCenter = (expPos - camPos).Length();
        float distanceToFogStart = distanceToCenter - (m_explosionStruct.explosionVec[i].collisionSphere.Radius * durationMod);
        float distanceToFogEnd = distanceToCenter + (m_explosionStruct.explosionVec[i].collisionSphere.Radius * durationMod);
        //m_debugData->DebugPushUILineDecimalNumber("distanceToFogStart = ", distanceToFogStart, "");
        //m_debugData->DebugPushUILineDecimalNumber("distanceToFogEnd = ", distanceToFogEnd, "");

        if (m_isTestBoolTrue == true)
        {
            m_isTestBoolTrue = false;
        }
        else
        {
            m_isTestBoolTrue = true;
        }

        if (m_isTestBoolTrue == true)
        {
            aEffect->SetFogEnabled(true);
        }
        else
        {
            aEffect->SetFogEnabled(false);
        }
        aEffect->SetFogColor(DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.5f));
        aEffect->SetFogColor(DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        //aEffect->SetFogColor(m_explosionStruct.explosionVec[i].color1);
        aEffect->SetFogStart(distanceToFogStart - 5.0f);
        aEffect->SetFogEnd(distanceToFogEnd + 5.0f);

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix0);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color0);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix1);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color1);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix2);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color2);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix3);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color3);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix4);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color4);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix5);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color5);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix6);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color6);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix7);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color7);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix8);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color8);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetFogEnabled(false);
    }
}

void FireControl::DrawFireControlObjects(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DrawExplosions(aView, aProj);
    if (m_muzzleFlash.isFlashActive == true)
    {
        DrawMuzzleFlash(aView, aProj);
    }
    DrawProjectiles(aView, aProj);
}

void FireControl::DrawFireControlObjects2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    DrawExplosions2(aView, aProj, aEffect, aInputLayout);

    if (m_muzzleFlash.isFlashActive == true)
    {
        DrawMuzzleFlash2(aView, aProj, aEffect, aInputLayout);
    }
    DrawProjectiles(aView, aProj);
    DrawMissiles(aView, aProj, aEffect, aInputLayout);
    DrawLaser(aView, aProj, aEffect, aInputLayout);
}

void FireControl::DrawLaser(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    if (m_isTargetingLaserOn == true)
    {
        float scale = m_playerLaser.distance;
        DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
        updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

        const float testRot = Utility::WrapAngle(m_playerLaser.flickerRot + m_playerLaser.flickerRate);
        m_playerLaser.flickerRot = testRot;
        const float scaleTransOffset = (m_playerLaser.distance) * 0.5f;
        DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));

        float diameterScale = 1.0f;
        if (m_playerLaser.isFlickerTrue == false)
        {
            diameterScale = 0.5f;
        }

        DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(diameterScale, scale, diameterScale));
        DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

        m_playerLaser.worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
        //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
        m_playerLaser.worldBodyMatrix *= scaleMat;
        m_playerLaser.worldBodyMatrix *= scaleTransOffsetMat;
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
        m_playerLaser.worldBodyMatrix *= updateMat;

        DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
        DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
        DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

        m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

        DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
        DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
        DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

        //lightDir0 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
        //lightDir1 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
        //lightDir2 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);

        DirectX::SimpleMath::Vector4 laserColor = m_playerLaser.laserColor;
        if (m_isTargetingLaserLockTrue == true)
        {
            laserColor = m_playerLaser.laserColorLockTrue;
        }

        //DirectX::SimpleMath::Vector4 color1 = m_playerLaser.laserColor;
        //DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        DirectX::SimpleMath::Vector4 color1 = laserColor;
        DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        aEffect->SetEmissiveColor(color2);
        aEffect->SetLightSpecularColor(0, color2);
        aEffect->SetLightSpecularColor(1, color2);
        aEffect->SetLightSpecularColor(2, color2);

        aEffect->SetLightDiffuseColor(0, color1);
        aEffect->SetLightDiffuseColor(1, color1);
        aEffect->SetLightDiffuseColor(2, color1);

        aEffect->SetLightDirection(0, lightDir0);
        aEffect->SetLightDirection(1, lightDir1);
        aEffect->SetLightDirection(2, lightDir2);

        //aEffect->EnableDefaultLighting();
        aEffect->SetWorld(m_playerLaser.worldBodyMatrix);
        //aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
        //m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());

        if (m_playerLaser.isFlickerTrue == true)
        {
            m_playerLaser.isFlickerTrue = false;
            aEffect->SetColorAndAlpha(laserColor);
            m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());
        }
        else
        {
            m_playerLaser.isFlickerTrue = true;
            aEffect->SetColorAndAlpha(laserColor);
            m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
        }

    }

    if (m_missileConsts.isMissleTargetingLaserTrue == true)
    {
        DirectX::SimpleMath::Vector3 localTargPos = DirectX::SimpleMath::Vector3::Zero;
        for (unsigned int i = 0; i < m_missileVec.size(); ++i)
        {
            if (m_missileVec[i].guidance.isTargetingLaserOn == true && m_missileVec[i].guidance.isTargetLocked == true)
            {

                //float scale = m_playerLaser.distance;
                float scale = m_missileVec[i].guidance.targetDistance;
                //DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
                DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
                //updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());
                updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);

                const float testRot = Utility::WrapAngle(m_playerLaser.flickerRot + m_playerLaser.flickerRate);
                //m_playerLaser.flickerRot = testRot;
                //const float scaleTransOffset = (m_playerLaser.distance) * 0.5f;
                const float scaleTransOffset = (m_missileVec[i].guidance.targetDistance) * 0.5f;
                DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));

                //float diameterScale = 1.0f;
                float diameterScale = 3.0f;
                if (m_isLaserFlickerTrue == false)
                {
                    diameterScale *= 0.5f;
                }
                //diameterScale = 3.0f;

                DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(diameterScale, scale, diameterScale));
                DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

                DirectX::SimpleMath::Matrix worldBodyMatrix;
                worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
                //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;
                worldBodyMatrix *= scaleMat;
                worldBodyMatrix *= scaleTransOffsetMat;
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
                //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;
                //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
                //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

                //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());

                DirectX::SimpleMath::Matrix updateMatAlign = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
                DirectX::SimpleMath::Matrix updateMatPos = DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
                //worldBodyMatrix *= updateMatAlign;
                worldBodyMatrix *= updateMatPos;

                //worldBodyMatrix *= updateMatPos;
                //////////////////////////////////////////////////////////////////////////////
                worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                worldBodyMatrix *= scaleMat;
                worldBodyMatrix *= scaleTransOffsetMat;
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));


                //DirectX::SimpleMath::Matrix lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
                DirectX::SimpleMath::Matrix lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
                DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateRotationX(cos(m_testTimer));
                //worldBodyMatrix *= lookAtMat;
                worldBodyMatrix *= testMat;
                //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;

                DirectX::SimpleMath::Matrix createWorldMat = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
                worldBodyMatrix *= createWorldMat;
                ///////////////////////////////////////////////////////////////////////////////

                DirectX::SimpleMath::Vector3 localizedTargetPos = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
                localizedTargetPos = DirectX::SimpleMath::Vector3::Transform(localizedTargetPos, m_missileVec[i].projectileData.inverseAlignmentQuat);
                m_debugData->PushDebugLinePositionIndicator(localizedTargetPos, 200.0f, 0.0f, DirectX::Colors::Olive);
                DirectX::SimpleMath::Vector3 worldTargetPos = localizedTargetPos;
                worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, m_missileVec[i].projectileData.alignmentQuat);
                DirectX::SimpleMath::Matrix worldTargetMat = DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
                worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, worldTargetMat);
                //worldTargetPos = 

                //m_debugData->PushDebugLinePositionIndicator(m_missileVec[i].guidance.targetPosition, 320.0f, 0.0f, DirectX::Colors::Lime);
                //m_debugData->PushDebugLinePositionIndicator(worldTargetPos, 200.0f, 2.0f, DirectX::Colors::Orange);
                worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                worldBodyMatrix *= scaleMat;
                worldBodyMatrix *= scaleTransOffsetMat;
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));

                DirectX::SimpleMath::Vector3 lookAtVec = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
                lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
                lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, lookAtVec, DirectX::SimpleMath::Vector3::UnitY);
                lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, DirectX::SimpleMath::Vector3::UnitY);
                //lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, lookAtVec, m_missileVec[i].projectileData.up);
                worldBodyMatrix *= lookAtMat;
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);

                //m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, DirectX::Colors::Coral);
                lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
                DirectX::SimpleMath::Matrix testWorldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                testWorldBodyMatrix *= lookAtMat;

                DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat);
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 300.0f, 0.0f, DirectX::Colors::Fuchsia);

                testLine = DirectX::SimpleMath::Vector3::UnitY;
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat);
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 300.0f, 0.0f, DirectX::Colors::Yellow);

                DirectX::SimpleMath::Matrix createWorldMat2 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
                //DirectX::SimpleMath::Matrix createWorldMat2 = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
                DirectX::SimpleMath::Matrix lookAtMat2 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
                //DirectX::SimpleMath::Matrix lookAtMat2 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
                testLine = DirectX::SimpleMath::Vector3::UnitZ;
                testLine *= m_missileVec[i].guidance.targetDistance;
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat2);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.targetLaserAlignment);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, createWorldMat2);
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
                m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 400.0f, 0.0f, DirectX::Colors::SkyBlue);
                m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine, DirectX::Colors::Lime);
                m_debugData->DebugPushUILineDecimalNumber("testLine.x = ", testLine.x, "");
                m_debugData->DebugPushUILineDecimalNumber("testLine.y = ", testLine.y, "");
                m_debugData->DebugPushUILineDecimalNumber("testLine.z = ", testLine.z, "");
                m_debugData->PushDebugLinePositionIndicator(testLine, 100.0f, 0.0f, DirectX::Colors::Teal);

                //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.forward, 120.0f, 0.0f, DirectX::Colors::White);

                //DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
                //DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::UnitY);
                DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].projectileData.q.position - m_missileVec[i].guidance.targetPosition, DirectX::SimpleMath::Vector3::UnitY);

                testLine = -DirectX::SimpleMath::Vector3::UnitX;
                testLine *= m_missileVec[i].guidance.targetDistance;
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat3);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.targetLaserAlignment);
                testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, createWorldMat2);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
                //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
                //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 400.0f, 0.0f, DirectX::Colors::Red);
                //m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine, DirectX::Colors::Orange);
                //m_debugData->PushDebugLinePositionIndicator(testLine, 330.0f, 0.0f, DirectX::Colors::Coral);

                //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), DirectX::SimpleMath::Vector3::UnitZ, 100.0f, 0.0f, DirectX::Colors::White);

                //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);

                localizedTargetPos = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
                //localizedTargetPos = m_missileVec[i].projectileData.q.position - m_missileVec[i].guidance.targetPosition;
                //localizedTargetPos = DirectX::SimpleMath::Vector3::Transform(localizedTargetPos, m_missileVec[i].projectileData.inverseAlignmentQuat);

                //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::UnitY);
                DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
                //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(localizedTargetPos, DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);

                DirectX::SimpleMath::Vector3 testLine2 = DirectX::SimpleMath::Vector3::UnitX;
                testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, lookAtMat4);
                testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, m_missileVec[i].projectileData.alignmentQuat);
                //testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
                testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].guidance.targetPosition));
                //m_debugData->DebugClearUI();
                m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine2, DirectX::Colors::Orange);
                m_debugData->PushDebugLinePositionIndicator(testLine2, 330.0f, 0.0f, DirectX::Colors::Lime);
                m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine2, 100.0f, 0.0f, DirectX::Colors::White);
                m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.forward, 50.0f, 0.0f, DirectX::Colors::Red);

                worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                worldBodyMatrix *= scaleMat;
                worldBodyMatrix *= scaleTransOffsetMat;
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
                worldBodyMatrix *= lookAtMat4;
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].guidance.targetPosition);
                //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
                //////////////////////////////////////////////////////////////////////////////

                ///////////////////////////////////////////////////////////////
                // From tank prototype
                DirectX::SimpleMath::Vector3 toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Zero;
                toUseLocalMuzzlePos.z = -2.0f;
                //toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Transform(toUseLocalMuzzlePos, m_missileVec[i].projectileData.alignmentQuat);
                DirectX::SimpleMath::Vector3 toUseWorldMuzzlePos = toUseLocalMuzzlePos;
                toUseWorldMuzzlePos = DirectX::SimpleMath::Vector3::Zero;
                toUseWorldMuzzlePos.x = 2.0f;
                toUseWorldMuzzlePos = DirectX::SimpleMath::Vector3::Transform(toUseWorldMuzzlePos, m_missileVec[i].projectileData.alignmentQuat);


                //DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(5) - (m_playerVehicle->GetPos() + toUseLocalMuzzlePos);
                //DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(m_missileVec[i].guidance.targetID) - (m_missileVec[i].projectileData.q.position + toUseLocalMuzzlePos);
                DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(m_missileVec[i].guidance.targetID) - (m_missileVec[i].projectileData.q.position + toUseWorldMuzzlePos);
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::UnitX;
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::Transform(forwardToTargNorm, m_missileVec[i].projectileData.alignmentQuat);

                //forwardToTargNorm = DirectX::SimpleMath::Vector3::UnitX;
                //DirectX::SimpleMath::Quaternion alignQuat = DirectX::SimpleMath::Quaternion::Identity;
                //alignQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_missileVec[i].guidance.targetLaserAlignment);
                //alignQuat.Normalize();
                //alignQuat *= m_missileVec[i].projectileData.inverseAlignmentQuat;
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::Transform(forwardToTargNorm, m_missileVec[i].guidance.targetLaserAlignment);
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::Transform(forwardToTargNorm, m_missileVec[i].projectileData.alignmentQuat);
                //forwardToTargNorm = m_missileVec[i].guidance.heading;
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::Transform(forwardToTargNorm, alignQuat);
                //forwardToTargNorm = DirectX::SimpleMath::Vector3::Transform(forwardToTargNorm, m_missileVec[i].guidance.targetLaserAlignment);
                forwardToTargNorm.Normalize();
                DirectX::SimpleMath::Vector3 rightCross = forwardToTargNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
                DirectX::SimpleMath::Vector3 testUp = rightCross.Cross(forwardToTargNorm);

                //DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_playerVehicle->GetPos() + toUseLocalMuzzlePos, forwardToTargNorm, testUp);
                //DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position + toUseLocalMuzzlePos, forwardToTargNorm, testUp);
                //DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position + toUseLocalMuzzlePos, forwardToTargNorm, testUp);
                DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position + toUseWorldMuzzlePos, forwardToTargNorm, testUp);
                //updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position, forwardToTargNorm, testUp);
                posOffset = DirectX::SimpleMath::Vector3(0.0f, -2.0f, 4.0f);
                posOffset = DirectX::SimpleMath::Vector3(0.0f, 2.0f, 0.0f);
                worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
                worldBodyMatrix *= scaleMat;
                worldBodyMatrix *= scaleTransOffsetMat;
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));

                worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toUseLocalMuzzlePos);

                worldBodyMatrix *= updateMat6;

                //////////////////////////////////////////////////////////////

                DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
                DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
                DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

                m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

                DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
                DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
                DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

                DirectX::SimpleMath::Vector4 color1 = m_playerLaser.laserColor;
                DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
                //color1 = color2;
                aEffect->SetEmissiveColor(color2);
                aEffect->SetLightSpecularColor(0, color2);
                aEffect->SetLightSpecularColor(1, color2);
                aEffect->SetLightSpecularColor(2, color2);

                aEffect->SetLightDiffuseColor(0, color1);
                aEffect->SetLightDiffuseColor(1, color1);
                aEffect->SetLightDiffuseColor(2, color1);

                aEffect->SetLightDirection(0, lightDir0);
                aEffect->SetLightDirection(1, lightDir1);
                aEffect->SetLightDirection(2, lightDir2);
                aEffect->EnableDefaultLighting();
                aEffect->SetWorld(worldBodyMatrix);

                aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
                //aEffect->SetColorAndAlpha(DirectX::Colors::Purple);
                //m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());

                /////////////////////////////
                if (m_isLaserFlickerTrue == true)
                {
                    m_isLaserFlickerTrue = false;
                    aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
                    //m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());
                }
                else
                {
                    m_isLaserFlickerTrue = true;
                    aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
                    //m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
                }

                ////////////////////////////

                //m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
                aEffect->EnableDefaultLighting();
                //m_debugData->DebugClearUI();
                DirectX::SimpleMath::Vector3 decomTrans = DirectX::SimpleMath::Vector3::Zero;
                DirectX::SimpleMath::Vector3 decomScale = DirectX::SimpleMath::Vector3::Zero;
                DirectX::SimpleMath::Quaternion decomQuat = DirectX::SimpleMath::Quaternion::Identity;

                worldBodyMatrix.Decompose(decomScale, decomQuat, decomTrans);
                m_debugData->PushDebugLinePositionIndicator(decomTrans, 350.0f, 0.0f, DirectX::Colors::Aqua);

                DirectX::SimpleMath::Vector3 testAlignVec = DirectX::SimpleMath::Vector3::UnitX;
                testAlignVec = DirectX::SimpleMath::Vector3::Transform(testAlignVec, decomQuat);

                m_debugData->PushDebugLine(m_playerVehicle->GetPos(), testAlignVec, 250.0f, 0.0f, DirectX::Colors::BlueViolet);

                localTargPos = m_missileVec[i].guidance.targetPosition;
            }
        }
    }
}

void FireControl::DrawMissiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        const DirectX::SimpleMath::Matrix alignRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        const DirectX::SimpleMath::Matrix posTransMat = DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::Identity;

        // main body 
        updateMat = m_ammoMissile.modelData.localBodyMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.mainBodyShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // nose cone
        updateMat = m_ammoMissile.modelData.localNoseConeMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.noseConeShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // tailBlackVoid
        updateMat = m_ammoMissile.modelData.localBlackVoidMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailBlackVoidShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.voidBlackColor);
        // tailEndCap
        updateMat = m_ammoMissile.modelData.localTailEndCapMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailEndCapShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);

        // after burn plumes
        if (m_missileVec[i].guidance.isRocketFired == true)
        {
            // afterburn plume base sphere shape
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat *= m_missileVec[i].guidance.afterBurnPlumeSphereMat;
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.rocketPlumeBaseShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.plumeColor);

            // afterburn plume extended cone shape
            updateMat = m_missileVec[i].guidance.afterBurnPlumeConeMat;
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.rocketPlumeShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.plumeColor);
        }

        // tail fin 
        const float finDeployAngle = Utility::ToRadians(0.0f) + m_ammoMissile.modelData.tailFinDeployAngleMax * m_missileVec[i].guidance.finDeployPercent;
        // tail fin 1 
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle1);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, DirectX::Colors::Red);
        // tail fin 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle2);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor2);
        // tail fin 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle3);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor2);
        // tail fin 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle4);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(270.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor2);

        // fin axel 1
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        DirectX::SimpleMath::Vector3 finTransTest = m_ammoMissile.modelData.finAxelTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(finTransTest));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.finAxelShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.axelColor);
        // fin axel 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(finTransTest));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.finAxelShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.axelColor);

        // thrust vector 1
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle1);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans1;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, DirectX::Colors::Lime);
        // thrust vector 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle2);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans2;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // thrust vector 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle3);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans3;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // thrust vector 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle4);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans4;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);

        // thrust vector housing 1
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);

        // thrust vector housing end cap 1
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingEndCapMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingEndCapShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing end cap 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingEndCapMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingEndCapShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing end cap 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingEndCapMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingEndCapShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);
        // thrust vector housing end cap 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorHousingEndCapMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorHousingEndCapShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.bodyColor);

        // thrust vector axel 1
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
        updateMat *= m_ammoMissile.modelData.localThrustAxelMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustAxelShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.axelColor);
        // thrust vector axel 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= m_ammoMissile.modelData.localThrustAxelMatrix;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustAxelShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.axelColor);

        // main wing
        const float wingFinDeployAngle = Utility::ToRadians(90.0f) + (m_ammoMissile.modelData.wingFinDeployAngleMax * m_missileVec[i].guidance.finDeployPercent);
        // main wing starboard
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // main wing lower
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // main wing port
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // main wing upper
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, DirectX::Colors::Red);
    }
}

void FireControl::DrawMuzzleFlash(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_muzzleFlash.muzzleFlashConeShape2->Draw(m_muzzleFlash.worldTestMatrix, aView, aProj, m_muzzleFlash.currentColor);
    m_muzzleFlash.muzzleFlashConeShape->Draw(m_muzzleFlash.worldMuzzleFlashConeMatrix, aView, aProj, m_muzzleFlash.currentColor);
}

void FireControl::DrawMuzzleFlash2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    aEffect->EnableDefaultLighting();

    DirectX::SimpleMath::Vector3 weaponDir = -m_playerVehicle->GetWeaponDirection();
    DirectX::SimpleMath::Vector3 lightDir0 = weaponDir;
    DirectX::SimpleMath::Vector3 lightDir1 = weaponDir;
    DirectX::SimpleMath::Vector3 lightDir2 = weaponDir;

    const float flashDurationRatio = m_muzzleFlash.flashTimer / m_muzzleFlash.flashDuration;
    float coneSideAngle = atan((m_muzzleFlash.baseConeHeight / (m_muzzleFlash.baseConeDiameter * 0.5f)));
    const float coneSideAngleDegrees = Utility::ToDegrees(coneSideAngle);
    float lightAngle = flashDurationRatio * (Utility::GetPi() * -0.5f) + coneSideAngle;
    const float lightAngleDegrees = Utility::ToDegrees(lightAngle);

    const float low = 0.0f;
    const float high = DirectX::XM_2PI;
    float lightRotation = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    if (m_muzzleFlash.isFlickerTrue == true)
    {
        Utility::GetDispersedLightDirectionsRotation(weaponDir, lightAngle, lightRotation, lightDir0, lightDir1, lightDir2);
    }
    else
    {
        Utility::GetDispersedLightDirectionsRotation(-weaponDir, lightAngle, lightRotation, lightDir0, lightDir1, lightDir2);
    }

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);

    if (m_muzzleFlash.isFlickerTrue == true)
    {
        aEffect->SetLightDirection(0, lightDir0);
        aEffect->SetLightDirection(1, lightDir1);
        aEffect->SetLightDirection(2, lightDir2);
    }
    else
    {
        aEffect->SetLightDirection(0, -lightDir0);
        aEffect->SetLightDirection(1, -lightDir1);
        aEffect->SetLightDirection(2, -lightDir2);
    }

    aEffect->SetBiasedVertexNormals(true);

    aEffect->SetWorld(m_muzzleFlash.worldTestMatrix);
    aEffect->SetColorAndAlpha(m_muzzleFlash.currentColor);
    m_muzzleFlash.muzzleFlashConeShape2->Draw(aEffect.get(), aInputLayout.Get());

    //aEffect->SetWorld(m_muzzleFlash.worldMuzzleFlashConeMatrix);
    //m_muzzleFlash.muzzleFlashConeShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetBiasedVertexNormals(false);
}

void FireControl::DrawProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);

    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();

    DirectX::SimpleMath::Plane groundPlane;
    groundPlane.x = 0.0f;
    groundPlane.y = -1.0f;
    groundPlane.z = 0.0f;
    groundPlane.w = 0.5f;

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * 0.1f;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    planeTrans = planeTrans.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans);
    groundPlane.Normalize();
    DirectX::SimpleMath::Matrix shadowMat = DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    const float maxShadowRange = m_environment->GetMaxShadowCastRange();
    float shadowScale;
    float inverseShadowScale;

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat;
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            projMat = m_ammoExplosive.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            projMat = m_ammoMachineGun.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            projMat = m_ammoMirv.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            projMat = m_ammoShotgun.ammoModel.localProjectileMatrix;
        }
        else
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }

        const float altitude = m_projectileVec[i].q.position.y;
        if (altitude > maxShadowRange)
        {
            shadowScale = 0.0f;
            inverseShadowScale = 1.0f;
        }
        else
        {
            inverseShadowScale = altitude / maxShadowRange;
            shadowScale = 1.0f - inverseShadowScale;

            const float tol = 0.00001f;
            if (inverseShadowScale < tol)
            {
                inverseShadowScale = tol;
            }
        }
        DirectX::SimpleMath::Matrix shadowScaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(shadowScale, shadowScale, shadowScale));
        DirectX::SimpleMath::Matrix shadowDrawMat = projMat;

        DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_projectileVec[i].alignmentQuat);
        projMat *= alignMat;

        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);

        shadowDrawMat = shadowDrawMat;
        shadowDrawMat *= alignMat;
        shadowDrawMat *= inverseShadowScale;
        shadowDrawMat *= shadowScaleMat;
        shadowDrawMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);;
        shadowDrawMat *= shadowMat;

        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoCannon.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            m_ammoExplosive.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoExplosive.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            m_ammoMachineGun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoMachineGun.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            m_ammoMirv.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoMirv.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            m_ammoShotgun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoShotgun.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoCannon.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
    }
}

void FireControl::FireMissile(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp, const float aTimeOffSet)
{
    AmmoData firedAmmo = m_ammoMissile.ammoData;

    m_isCoolDownActive = true;
    m_coolDownTimer = firedAmmo.cooldown;

    MissileData firedMissile;
    firedMissile.projectileData.ammoData = firedAmmo;
    firedMissile.projectileData.q.position = aLaunchPos;
    firedMissile.projectileData.q.velocity = (firedMissile.projectileData.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedMissile.projectileData.isCollisionTrue = false;
    firedMissile.projectileData.isDeleteTrue = false;
    firedMissile.projectileData.liveTimeTick = firedAmmo.tickDownCounter;
    firedMissile.projectileData.forward = aLaunchDirectionForward;

    firedMissile.projectileData.forward.Normalize();
    firedMissile.projectileData.up = aUp;
    firedMissile.projectileData.up.Normalize();
    firedMissile.projectileData.right = -firedMissile.projectileData.up.Cross(firedMissile.projectileData.forward);
    firedMissile.projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedMissile.projectileData.right, firedMissile.projectileData.up));
    firedMissile.projectileData.alignmentQuat.Normalize();
    firedMissile.projectileData.inverseAlignmentQuat = firedMissile.projectileData.alignmentQuat;
    firedMissile.projectileData.inverseAlignmentQuat.Inverse(firedMissile.projectileData.inverseAlignmentQuat);
    firedMissile.projectileData.inverseAlignmentQuat.Normalize();

    // collision data
    firedMissile.projectileData.collisionData.collisionDurationMod = firedAmmo.impactDurration;
    firedMissile.projectileData.collisionData.collisionMagnitudeMod = firedMissile.projectileData.ammoData.impactModifier;
    firedMissile.projectileData.collisionData.collisionSphere.Center = firedMissile.projectileData.q.position;
    firedMissile.projectileData.collisionData.collisionSphere.Radius = firedAmmo.radius;
    firedMissile.projectileData.collisionData.velocity = firedMissile.projectileData.q.velocity;
    firedMissile.projectileData.collisionData.mass = firedAmmo.mass;
    firedMissile.projectileData.collisionData.isCollisionTrue = firedMissile.projectileData.isCollisionTrue;
    //firedMissile.projectileData.time = 0.0f;
    firedMissile.projectileData.time = 0.0f + aTimeOffSet;

    firedMissile.guidance.uniqueId = GetUniqueMissileID();
    firedMissile.guidance.heading = aLaunchDirectionForward;
    firedMissile.guidance.targetID = m_currentTargetID;
    if (m_currentTargetID != -1)
    {
        firedMissile.guidance.isTargetLocked = true;
    }
    else
    {
        firedMissile.guidance.isTargetLocked = false;
    }
    m_npcController->UpdateMissleGuidance(m_currentTargetID, firedMissile.guidance.targetPosition, firedMissile.guidance.targetVelocity);
    firedMissile.guidance.targetDistance = (aLaunchPos - firedMissile.guidance.targetPosition).Length();

    m_missileVec.push_back(firedMissile);
}

void FireControl::FireProjectileCannon(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoCannon.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;
        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireDefaultProjectile(const AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    if (m_isCoolDownActive == false)
    {
        ProjectileData firedProjectile;
        AmmoData firedAmmo;
        if (aAmmoType == AmmoType::AMMOTYPE_CANNON)
        {
            firedAmmo = m_ammoCannon.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_MIRV)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = true;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else
        {
            // ToDo - add error handling, ammo type missing
            firedAmmo = m_ammoCannon.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedAmmo.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoExplosive.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoExplosive.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        firedProjectile.isMidAirDeployAvailable = false;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileMachineGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoMachineGun.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoMirv.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoMirv.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        firedProjectile.isMidAirDeployAvailable = true;
        firedProjectile.isFuseTriggered = false;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoShotgun.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;

        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.time = 0.0f;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        //firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.right = aLaunchDirectionRight;
        firedProjectile.up = firedProjectile.right.Cross(firedProjectile.forward);
        firedProjectile.up.Normalize();
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        DirectX::SimpleMath::Vector3 up = aLaunchDirectionForward;
        up = aLaunchDirectionRight.Cross(aLaunchDirectionForward);

        const unsigned int shotCount = 10;
        const float shotRotationOffset = DirectX::XM_2PI / static_cast<float>(shotCount);
        float shotRotation = 0.0f;
        const float chokeAngle = Utility::ToRadians(5.0f);
        DirectX::SimpleMath::Vector3 shotCenterAim = m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward;
        for (unsigned int i = 0; i < shotCount; ++i)
        {
            const float chokeOffset = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));

            DirectX::SimpleMath::Matrix barrelRotationMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionForward, shotRotation);
            DirectX::SimpleMath::Matrix chokeVariationMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, chokeOffset);

            firedProjectile.q.velocity = shotCenterAim;
            firedProjectile.q.velocity = DirectX::SimpleMath::Vector3::Transform(firedProjectile.q.velocity, chokeVariationMat);
            firedProjectile.q.velocity = DirectX::SimpleMath::Vector3::Transform(firedProjectile.q.velocity, barrelRotationMat);
            firedProjectile.q.velocity += aLauncherVelocity;
            m_projectileVec.push_back(firedProjectile);

            shotRotation += shotRotationOffset;
        }
    }
}

void FireControl::FireSelectedAmmo(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        ActivateMuzzleFlash(m_currentAmmoType);
        if (m_currentAmmoType == AmmoType::AMMOTYPE_CANNON)
        {
            FireProjectileCannon(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            FireProjectileExplosive(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            FireProjectileMachineGun(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
        {
            FireProjectileMirv(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            //DirectX::SimpleMath::Vector3 up = m_playerVehicle->GetVehicleUp();
            DirectX::SimpleMath::Vector3 up = aUp;
            DirectX::SimpleMath::Vector3 right = -up.Cross(aLaunchDirectionForward);
            FireProjectileShotGun(aLaunchPos, aLaunchDirectionForward, right, aLauncherVelocity);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_GUIDEDMISSILE)
        {
            if (m_currentTargetID != -1)
            {
                //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetMissleTubeDirLeft(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpLeft(), 0.0f);
                FireMissile(m_playerVehicle->GetWeaponPos(), m_playerVehicle->GetWeaponDirection(), aLauncherVelocity, m_playerVehicle->GetVehicleUp(), 0.0f);
                const float fireTimeOffset = 0.0f;
                //FireMissile(m_playerVehicle->GetMissleTubePosRight(), m_playerVehicle->GetMissleTubeDirRight(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpRight(), fireTimeOffset);

                //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), DirectX::SimpleMath::Vector3::UnitX, aLauncherVelocity, DirectX::SimpleMath::Vector3::UnitY);
                //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetMissleTubeDirLeft(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpLeft());
                //FireMissile(m_playerVehicle->GetMissleTubePosRight(), m_playerVehicle->GetMissleTubeDirRight(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpRight());

                m_isCoolDownActive = true;
                m_coolDownTimer = m_ammoMissile.ammoData.cooldown;
            }
        }
    }
}

bool FireControl::GetIsMissileActiveTrue() const
{
    if (m_missileVec.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void FireControl::GetCameraMissieData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget) const
{
    if (m_missileVec.size() > 0)
    {
        const int missileId = 0;
        //DirectX::SimpleMath::Matrix missileAlignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[missileId].projectileData.alignmentQuat);
        DirectX::SimpleMath::Quaternion missileAlignment = m_missileVec[missileId].projectileData.alignmentQuat;
        DirectX::SimpleMath::Vector3 missilePos = m_missileVec[missileId].projectileData.q.position;
        DirectX::SimpleMath::Vector3 targetPos = m_missileVec[missileId].guidance.targetPosition;
        aAlignment = missileAlignment;
        aPos = missilePos;
        aTarget = targetPos;
    }
    else
    {
        aAlignment = DirectX::SimpleMath::Quaternion::Identity;
        aPos = DirectX::SimpleMath::Vector3::Zero;
    }
}

Utility::ImpulseForce FireControl::GetRecoilImpulseForce(DirectX::SimpleMath::Vector3 aDirectionNorm)
{
    Utility::ImpulseForce recoilImpulseForce;

    if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.2f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 10000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.25f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else  // Default to cannon, AmmoType::AMMOTYPE_CANNON
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.6f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }

    return recoilImpulseForce;
}

unsigned int FireControl::GetUniqueMissileID()
{
    const unsigned int idVal = m_nextUniqueMissileID;
    m_nextUniqueMissileID++;
    return idVal;
}

void FireControl::InitializeAmmoCannon(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_CANNON;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.8f;
    //aAmmo.ammoData.dragCoefficient = 0.82f;
    aAmmo.ammoData.dragCoefficient = 0.042f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 395.0f;
    aAmmo.ammoData.length = 9.0f;
    aAmmo.ammoData.mass = 10.0f;
    aAmmo.ammoData.radius = 0.2f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoExplosive(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_EXPLOSIVE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.3f;
    aAmmo.ammoData.dragCoefficient = 0.2f;
    aAmmo.ammoData.impactDurration = 0.3f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 215.0f;
    aAmmo.ammoData.length = 25.0f;
    aAmmo.ammoData.mass = 55.0f;
    aAmmo.ammoData.radius = 0.22f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoMissile(MissileStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.9f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    //aAmmo.ammoData.launchVelocity = 25.0f;
    aAmmo.ammoData.launchVelocity = m_missileConsts.launchVelocity;
    //aAmmo.ammoData.length = 4.0f;
    //aAmmo.ammoData.radius = 0.75f;
    //aAmmo.ammoData.length = 4.0f;
    //aAmmo.ammoData.radius = 0.33f;
    //aAmmo.ammoData.length = 1.1f;
    //aAmmo.ammoData.radius = 0.127f * 0.5f;
    aAmmo.ammoData.length = m_missileDimensions.x;
    aAmmo.ammoData.radius = m_missileDimensions.z * 0.5f;
    aAmmo.ammoData.mass = m_missileConsts.mass;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = true;
}

void FireControl::InitializeAmmoMachineGun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MACHINEGUN;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.09f;
    aAmmo.ammoData.dragCoefficient = 0.25f;
    aAmmo.ammoData.impactDurration = 0.7f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 465.0f;
    aAmmo.ammoData.length = 0.4f;
    aAmmo.ammoData.mass = 1.4f;
    aAmmo.ammoData.radius = 0.14f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoMirv(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MIRV;
    aAmmo.ammoData.baseDamage = 1.0f;
    //aAmmo.ammoData.cooldown = 1.9f;
    aAmmo.ammoData.cooldown = 1.0f;
    //aAmmo.ammoData.dragCoefficient = 0.19f;
    aAmmo.ammoData.dragCoefficient = 0.5f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    //aAmmo.ammoData.launchVelocity = 165.0f;
    //aAmmo.ammoData.launchVelocity = 105.0f;
    aAmmo.ammoData.launchVelocity = 195.0f;
    aAmmo.ammoData.length = 24.5f;
    aAmmo.ammoData.mass = 65.0f;
    aAmmo.ammoData.radius = 0.22f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoShotgun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_SHOTGUN;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.5f;
    aAmmo.ammoData.dragCoefficient = 0.7f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 395.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 1.6f;
    aAmmo.ammoData.radius = 0.1f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeExplosionData(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, ExplosionData& aExplosionData)
{
    aExplosionData.explosionType = ExplosionType::EXPLOSIONTYPE_NONVEHICLE;
    aExplosionData.explosionStartColor = DirectX::Colors::DarkRed;
    aExplosionData.explosionEndColor = DirectX::Colors::OrangeRed;
    aExplosionData.explosionCurrentColor = aExplosionData.explosionStartColor;
    aExplosionData.color0 = aExplosionData.explosionStartColor;
    aExplosionData.color1 = aExplosionData.explosionStartColor;
    aExplosionData.color2 = aExplosionData.explosionStartColor;
    aExplosionData.color3 = aExplosionData.explosionStartColor;
    aExplosionData.color4 = aExplosionData.explosionStartColor;
    aExplosionData.color5 = aExplosionData.explosionStartColor;
    aExplosionData.color6 = aExplosionData.explosionStartColor;
    aExplosionData.color7 = aExplosionData.explosionStartColor;
    aExplosionData.color8 = aExplosionData.explosionStartColor;
    aExplosionData.initialRadius = m_ammoExplosive.ammoData.radius;
    aExplosionData.currentRadius = aExplosionData.initialRadius;
    aExplosionData.currentDuration = 0.0f;
    aExplosionData.totalDuration = 3.0f;
    aExplosionData.maxRadius = 15.f;
    aExplosionData.position = DirectX::SimpleMath::Vector3::Zero;
    m_explosionStruct.explosionShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), -1.0f);
    m_explosionStruct.explosionShape1 = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), -0.75f);
    aExplosionData.localExplosionMatrix = DirectX::SimpleMath::Matrix::Identity;
    aExplosionData.explosionMatrix0 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix1 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix2 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix3 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix4 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix5 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix6 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix7 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix8 = aExplosionData.localExplosionMatrix;
    aExplosionData.rotationVariationMatrix = DirectX::SimpleMath::Matrix::Identity;
    aExplosionData.collisionSphere.Center = aExplosionData.position;
    aExplosionData.collisionSphere.Radius = aExplosionData.initialRadius;
    aExplosionData.isLifeTimeExpired = false;

    //m_explosionStruct.maxExplosionForce = 10000000.0f;
    //m_explosionStruct.maxExplosionForce = 100.0f;
    //m_explosionStruct.maxExplosionForce = 1000000.0f;
    //m_explosionStruct.maxExplosionForce = 100000.0f;
    m_explosionStruct.maxExplosionForce = m_maxExplosiveForce;
    m_explosionStruct.maxExplosionImpactRadius = aExplosionData.maxRadius * 2.0f;

    m_explosionStruct.explosionVec.clear();
}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection,
    Environment const* aEnvironment, std::shared_ptr<Vehicle> aVehicle)
{
    m_playerVehicle = aVehicle;
    m_explosionStruct.explosionToPushVec.clear();
    m_projectileVec.clear();
    m_newProjectilePushVec.clear();
    m_missileVec.clear();
    m_environment = aEnvironment;
    //m_currentAmmoType = AmmoType::AMMOTYPE_CANNON;
    m_currentAmmoType = AmmoType::AMMOTYPE_MIRV;
    m_currentAmmoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;

    ////////////
    // inertia tensors
    m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;

    //const float xExtent = m_missileDimensions.x;
    //const float yExtent = m_missileDimensions.y;
    //const float zExtent = m_missileDimensions.z;
    //const float mass = m_missileMass;
    //const float mass = m_missileConsts.mass;

    // cuboid
    //m_missileInertiaTensorLocal._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    //m_missileInertiaTensorLocal._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    //m_missileInertiaTensorLocal._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    // cylinder
    //const float m_missileMass = 10.0f;
    
    //const float mass = m_missileConsts.mass;
    //const float radius = m_missileDimensions.z;
    //const float height = m_missileDimensions.x;

    //const float mass = 10.0f;
    //const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(4.0f, 1.0f, 1.0f);
    const float radius = 1.0f;
    const float height = 4.0f;
    /*
    m_missileInertiaTensorLocal._11 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._22 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._33 = (1.0f / 2.0f) * (mass) * (radius * radius);
    */

    // cuboid
    //const float xExtent = 1.0f;
    //const float yExtent = 1.0f;
    //const float zExtent = 1.0f;
    // cuboid
    const float xExtent = m_missileDimensions.x;
    const float yExtent = m_missileDimensions.y;
    const float zExtent = m_missileDimensions.z;
    const float mass = m_missileConsts.mass;
    
    m_missileInertiaTensorLocal._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));
    
    /*
    m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    m_missileInertiaTensorLocal._11 = (2.0f / 3.0f) * (mass) * (radius * radius);
    m_missileInertiaTensorLocal._22 = (2.0f / 3.0f) * (mass) * (radius * radius);
    m_missileInertiaTensorLocal._33 = (2.0f / 3.0f) * (mass) * (radius * radius);
    */

    m_missileInverseInertiaTensorLocal = m_missileInertiaTensorLocal;
    m_missileInverseInertiaTensorLocal = m_missileInverseInertiaTensorLocal.Invert();

    if (m_missileConsts.useAdvancedMoiTensorTrue == true)
    {
        const float altMass = 5.0f;
        const DirectX::SimpleMath::Vector3 altDimensions = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
        const DirectX::SimpleMath::Vector3 altOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
        DirectX::SimpleMath::Matrix advancedTensor = DirectX::SimpleMath::Matrix::Identity;

        advancedTensor._11 = (1.0f / 12.0f) * (altMass) * ((altDimensions.y * altDimensions.y) + (altDimensions.z * altDimensions.z));
        advancedTensor._22 = (1.0f / 12.0f) * (altMass) * ((altDimensions.x * altDimensions.x) + (altDimensions.z * altDimensions.z));
        advancedTensor._33 = (1.0f / 12.0f) * (altMass) * ((altDimensions.x * altDimensions.x) + (altDimensions.y * altDimensions.y));

        advancedTensor.Translation(altOffset);
        m_missileInertiaTensorLocal *= advancedTensor;
        m_missileInverseInertiaTensorLocal = m_missileInertiaTensorLocal;
        m_missileInverseInertiaTensorLocal = m_missileInverseInertiaTensorLocal.Invert();
    }
    ////////////

    InitializeAmmoCannon(m_ammoCannon);
    InitializeAmmoExplosive(m_ammoExplosive);
    InitializeAmmoMachineGun(m_ammoMachineGun);
    InitializeAmmoMirv(m_ammoMirv);
    InitializeAmmoShotgun(m_ammoShotgun);

    InitializeAmmoMissile(m_ammoMissile);
    InitializeProjectileModelMissile(aContext, m_ammoMissile);

    InitializeExplosionData(aContext, m_explosionStruct.explosionRefData);
    InitializeMuzzleFlashModel(aContext, m_muzzleFlash);
    InitializeProjectileModelCannon(aContext, m_ammoCannon);
    InitializeProjectileModelExplosive(aContext, m_ammoExplosive);
    InitializeProjectileModelMachineGun(aContext, m_ammoMachineGun);
    InitializeProjectileModelMirv(aContext, m_ammoMirv);
    InitializeProjectileModelShotgun(aContext, m_ammoShotgun);
    InitializeLaserModel(aContext, m_playerLaser);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeMuzzleFlashModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MuzzleFlash& aMuzzleFlash)
{
    aMuzzleFlash.muzzleFlashConeShape2 = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), aMuzzleFlash.baseConeDiameter, aMuzzleFlash.baseConeHeight);
    aMuzzleFlash.muzzleFlashConeShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), aMuzzleFlash.baseConeDiameter, 16Ui64);

    aMuzzleFlash.localMuzzleConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.worldMuzzleFlashConeMatrix = aMuzzleFlash.localMuzzleConeMatrix;

    aMuzzleFlash.worldTestMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.localTestMatrix = DirectX::SimpleMath::Matrix::Identity;
}

void FireControl::InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //const float ammoSize = 0.3f;
    //const float ammoLength = 10.0f;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, ammoLength, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMachineGun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoDiameter = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), ammoDiameter);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMirv(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, ammoLength, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMissile(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MissileStruct& aAmmo)
{
    const float zOffset = 0.001f;
    const float ammoDiameter = aAmmo.ammoData.radius * 2.0f;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.modelData.mainBodyShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoDiameter);
    aAmmo.modelData.worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localBodyMatrix = aAmmo.modelData.worldBodyMatrix;

    // tailEndCapShape
    const float tailEndCapThickness = aAmmo.ammoData.radius * 0.2f;
    const float tailEndCapDiameter = (aAmmo.ammoData.radius * 2.0f) - tailEndCapThickness;
    const float tailEndCapStretch = 1.5f;
    aAmmo.modelData.tailEndCapShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), tailEndCapDiameter, tailEndCapThickness);
    aAmmo.modelData.localTailEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localTailEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(tailEndCapStretch, 1.0f, 1.0f));
    aAmmo.modelData.localTailEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.worldTailEndCapMatrix = aAmmo.modelData.localTailEndCapMatrix;

    // tailBlackVoidShape
    const float tailBlackVoidDiameter = aAmmo.ammoData.radius * 1.9f;
    const float tailBlackVoidLength = zOffset * 2.0f;
    DirectX::SimpleMath::Vector3 tailBlackVoidTrans = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.02f, 0.0f, 0.0f);
    tailBlackVoidTrans.x = (-(ammoLength - tailBlackVoidLength) * 0.5f) - zOffset;
    tailBlackVoidTrans = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.5f, 0.0f, 0.0f);
    aAmmo.modelData.tailBlackVoidShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), tailBlackVoidLength, tailBlackVoidDiameter);
    aAmmo.modelData.localBlackVoidMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localBlackVoidMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localBlackVoidMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(tailBlackVoidTrans);
    aAmmo.modelData.worldBlackVoidMatrix = aAmmo.modelData.localBlackVoidMatrix;

    // nose cone
    aAmmo.modelData.noseConeShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoDiameter);
    aAmmo.modelData.localNoseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 noseConeTranslation = DirectX::SimpleMath::Vector3(ammoLength * 0.5f, 0.0f, 0.0f);
    aAmmo.modelData.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    aAmmo.modelData.worldNoseConeMatrix = aAmmo.modelData.localNoseConeMatrix;

    // rocket plume
    const float plumeLength = aAmmo.ammoData.length * 0.5f;
    aAmmo.modelData.afterBurnConeBaseLength = plumeLength;
    aAmmo.modelData.rocketPlumeShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoDiameter, plumeLength);
    DirectX::SimpleMath::Vector3 plumeTranslation = DirectX::SimpleMath::Vector3(-ammoLength, 0.0f, 0.0f);
    aAmmo.modelData.plumeTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(plumeTranslation);
    DirectX::SimpleMath::Vector3 rotationCenterTranslation = DirectX::SimpleMath::Vector3((-plumeLength * 0.5f), 0.0f, 0.0f);
    aAmmo.modelData.localPlumeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localPlumeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aAmmo.modelData.localPlumeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotationCenterTranslation);
    aAmmo.modelData.worldPlumeMatrix = aAmmo.modelData.localPlumeMatrix;

    // rocket plume base
    aAmmo.modelData.afterBurnBaseSize = ammoDiameter;
    aAmmo.modelData.rocketPlumeBaseShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoDiameter);
    DirectX::SimpleMath::Vector3 plumeBaseTranslation = DirectX::SimpleMath::Vector3(-ammoLength, 0.0f, 0.0f);
    aAmmo.modelData.plumeBaseTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(plumeBaseTranslation);
    aAmmo.modelData.localPlumeBaseMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localPlumeBaseMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aAmmo.modelData.worldPlumeBaseMatrix = aAmmo.modelData.localPlumeBaseMatrix;
    DirectX::SimpleMath::Vector3 testTransVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    testTransVec.x = -ammoDiameter * 0.5f;
    aAmmo.modelData.plumeTranslationTest = DirectX::SimpleMath::Matrix::CreateTranslation(testTransVec);

    // tail fins
    const float tailFinLength = aAmmo.ammoData.length * 0.3f;
    const float tailFinWidth = tailFinLength * 0.2f;
    const DirectX::SimpleMath::Vector3 tailFinDimensions = DirectX::SimpleMath::Vector3(tailFinLength, tailFinWidth, tailFinWidth * 0.3f);
    aAmmo.modelData.tailFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinDimensions);
    DirectX::SimpleMath::Vector3 tailFinTranslation1 = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);
    const float tailOffset = -ammoLength * 0.45f;

    // tail fin generic
    const float tailFinLength2 = m_ammoMissile.ammoData.length * 0.3f;
    const float tailFinWidth2 = tailFinLength * 0.2f;
    aAmmo.modelData.localTailFinMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength2 * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.tailFinTransMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.tailFinTransMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailOffset * 0.5f, aAmmo.ammoData.radius * 0.9f, 0.0f));
    aAmmo.modelData.tailFinTransMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.tailFinTransMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailOffset * 1.0f, aAmmo.ammoData.radius * 0.9f, 0.0f));

    // tail fin axel
    const float finAxelLength = (aAmmo.ammoData.radius * 2.0f) * 1.05f;
    const float finAxelDiameter = tailFinDimensions.y;
    aAmmo.modelData.finAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), finAxelLength, finAxelDiameter);
    const float tailFinAxelOffset = tailOffset;
    aAmmo.modelData.finAxelTranslation = DirectX::SimpleMath::Vector3(tailFinAxelOffset, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 testTrans = DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f);

    // thrust vector fin 1
    const float thrustPosZ = aAmmo.ammoData.radius * 0.5f;
    DirectX::SimpleMath::Vector3 thrustVectorMainDimensions = DirectX::SimpleMath::Vector3(tailFinLength * 0.25f, aAmmo.ammoData.radius * 0.6f, tailFinWidth * 0.15f);
    thrustVectorMainDimensions.x = (aAmmo.ammoData.length * 0.3f) * 0.125f;
    thrustVectorMainDimensions.y = aAmmo.ammoData.radius * 0.6f;
    thrustVectorMainDimensions.z = 0.05 * ammoDiameter;
    aAmmo.modelData.thrustVectorMainShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), thrustVectorMainDimensions);

    aAmmo.modelData.localThrustVectorMainMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localThrustVectorMainMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3((-thrustVectorMainDimensions.x * 0.5f) + (-thrustVectorMainDimensions.z * 1.0f), 0.0f, 0.0f));

    // thrust vector control rod 
    DirectX::SimpleMath::Vector3 trustRodDimensions = DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, tailFinWidth * 0.3f, tailFinWidth * 0.3f);
    DirectX::SimpleMath::Vector3 trustRodTrans3 = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.50f, aAmmo.ammoData.radius * 1.31f, 0.0f);
    aAmmo.modelData.thrustConnectingRodShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), trustRodDimensions);
    aAmmo.modelData.localThrustConnectingRodMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustConnectingRodTrans3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustConnectingRodTrans3 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustRodTrans3);
    aAmmo.modelData.thrustConnectingRodTrans3 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));

    // thrust vector control rod 2
    DirectX::SimpleMath::Vector3 trustRodTrans2 = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.50f, aAmmo.ammoData.radius * 1.26f, 0.0f);
    aAmmo.modelData.localThrustConnectingRodMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustConnectingRodTrans2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustConnectingRodTrans2 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustRodTrans2);
    aAmmo.modelData.thrustConnectingRodTrans2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));

    // thrust vector housing
    DirectX::SimpleMath::Vector3 thrustHousingDimensions = DirectX::SimpleMath::Vector3(thrustVectorMainDimensions.x * 1.1f, tailEndCapThickness * 0.4f, thrustVectorMainDimensions.z * 3.15f);
    thrustHousingDimensions.x = thrustVectorMainDimensions.x * 1.1f;
    thrustHousingDimensions.y = tailEndCapThickness * 0.6f;
    thrustHousingDimensions.z = thrustVectorMainDimensions.z * 3.15f;

    DirectX::SimpleMath::Vector3 thrustHousingTrans = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.5f - (thrustHousingDimensions.x * 0.5f), aAmmo.ammoData.radius, 0.0f);
    thrustHousingTrans.x += thrustHousingDimensions.x * 0.4f;
    thrustHousingTrans.y += (thrustVectorMainDimensions.y * 0.5f) + (thrustHousingDimensions.y * 0.5f);

    /// reshape housing
    float housingAddon = (ammoLength * 0.5f) + tailFinAxelOffset;

    thrustHousingDimensions.x += housingAddon;

    DirectX::SimpleMath::Vector3 thrustVectorBaseTrans = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.55f, 0.0f, 0.0f);
    thrustVectorBaseTrans = DirectX::SimpleMath::Vector3((-aAmmo.ammoData.length * 0.5f), 0.0f, 0.0f);
    thrustVectorBaseTrans.x -= ammoDiameter * 0.23f;
    thrustHousingTrans = DirectX::SimpleMath::Vector3(-aAmmo.ammoData.length * 0.5f - (thrustHousingDimensions.x * 0.5f), aAmmo.ammoData.radius, 0.0f);

    thrustHousingTrans = thrustVectorBaseTrans;
    thrustHousingTrans.x += thrustHousingDimensions.x * 0.5f;
    thrustHousingTrans.y = aAmmo.ammoData.radius;
    thrustHousingTrans.y += (thrustVectorMainDimensions.y * 0.5f) + (thrustHousingDimensions.y * 0.5f);
    thrustHousingTrans.y += thrustHousingDimensions.y;

    thrustHousingTrans = thrustVectorBaseTrans;
    thrustHousingTrans = aAmmo.modelData.finAxelTranslation;
    thrustHousingTrans.x -= thrustHousingDimensions.x * 0.5f;
    thrustHousingTrans.y += ammoDiameter * 0.53f;
    thrustHousingTrans.y = (finAxelLength * 0.5f) - (zOffset * 2.0f) - (thrustHousingDimensions.y * 0.5f);
    aAmmo.modelData.thrustVectorHousingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), thrustHousingDimensions);
    aAmmo.modelData.localThrustVectorHousingMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localThrustVectorHousingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(thrustHousingTrans);

    // thrust vector housing end cap
    DirectX::SimpleMath::Vector3 thrustHousingEndCapTrans = thrustHousingTrans;
    thrustHousingEndCapTrans.x -= thrustHousingDimensions.x * 0.5f;
    aAmmo.modelData.thrustVectorHousingEndCapShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), thrustHousingDimensions.y, thrustHousingDimensions.z);
    aAmmo.modelData.localThrustVectorHousingEndCapMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localThrustVectorHousingEndCapMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(thrustHousingEndCapTrans);

    // thrust Axel rod
    const float thrustAxelRodLength = finAxelLength;
    const float thrustAxelRodDiameter = thrustVectorMainDimensions.z;
    aAmmo.modelData.thrustAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), thrustAxelRodLength, thrustAxelRodDiameter);

    DirectX::SimpleMath::Vector3 testRodTrans = DirectX::SimpleMath::Vector3::Zero;
    testRodTrans.x = thrustHousingEndCapTrans.x;

    aAmmo.modelData.localThrustAxelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localThrustAxelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(testRodTrans);

    // wing fins
    float wingFinLength = aAmmo.ammoData.length * 0.3f;
    const float wingFinWidth = wingFinLength * 0.2f;
    const DirectX::SimpleMath::Vector3 wingFinDimensions = DirectX::SimpleMath::Vector3(wingFinLength, wingFinWidth, wingFinWidth * 0.3f);
    wingFinLength = 0.7f;
    aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), aAmmo.ammoData.length * 0.3f);

    DirectX::SimpleMath::Matrix wingScale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(10.0f, 1.0f, 1.0f));
    DirectX::SimpleMath::Matrix wingScale2 = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.3f, 0.05f, 1.0f));

    const float wingOffset = (2.0f * 1.1f) * 0.5f;
    const DirectX::SimpleMath::Matrix localWingTrans = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -0.5f, 0.0f));
    DirectX::SimpleMath::Vector3 wingFinTranslation1 = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);

    const float mainWingLongPos = 0.2f * aAmmo.ammoData.length;

    const float posOffset = -2.0f * aAmmo.ammoData.radius;

    DirectX::SimpleMath::Vector3 wingTrans = DirectX::SimpleMath::Vector3(mainWingLongPos, aAmmo.ammoData.radius + posOffset, 0.0f);

    //////////////////////////////////////////////
    float modelSizeVal = 1.0f;
    float x = 0.06f;
    float y = 0.015f;
    float z = 0.3f;
    float xMod = modelSizeVal * x;
    float yMod = modelSizeVal * y;
    float zMod = modelSizeVal * z;

    DirectX::SimpleMath::Matrix testWingScale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(x, y, z));

    wingTrans = DirectX::SimpleMath::Vector3(0.02f, 0.0f, 0.1f);
    float wingTransOffset = aAmmo.ammoData.radius * 1.0f;
    wingScale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(x, y, z));
    aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), modelSizeVal);

    aAmmo.modelData.localWingFinMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix *= testWingScale;
    aAmmo.modelData.localWingFinMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingTrans);
    aAmmo.modelData.wingTranslation = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.wingTranslation *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, wingTransOffset));

    ////////////////////////////////////////////

    const float thrustRodLength = tailFinLength;
    const float thrustRodWidth = tailFinWidth;
    const DirectX::SimpleMath::Vector3 thrustRodDimensions = tailFinDimensions;
    aAmmo.modelData.thrustRodShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), thrustRodDimensions);
    aAmmo.modelData.thustRodLocalPos = DirectX::SimpleMath::Vector3((-ammoLength * 0.5f) + (-thrustRodLength * 0.0f), aAmmo.ammoData.radius, 0.0f);

    thrustVectorBaseTrans = DirectX::SimpleMath::Vector3::Zero;
    thrustVectorBaseTrans.x = thrustHousingTrans.x;
    thrustVectorBaseTrans.x -= thrustVectorMainDimensions.x;
    thrustVectorBaseTrans.x -= thrustAxelRodDiameter * 0.5f;

    const float thrustVectorFinYOffset = thrustHousingTrans.y - (thrustVectorMainDimensions.y * 0.5f) - (thrustHousingDimensions.y * 0.5f) - (zOffset * 2.0f);
    // thrust vector fin 1
    DirectX::SimpleMath::Vector3 trustVectorTrans1 = thrustVectorBaseTrans;
    trustVectorTrans1.y += thrustVectorFinYOffset;
    aAmmo.modelData.localThrustVectorMainMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans1 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustVectorTrans1);
    aAmmo.modelData.thrustVectorTrans1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));

    // thrust vector fin 2
    DirectX::SimpleMath::Vector3 trustVectorTrans2 = thrustVectorBaseTrans;
    trustVectorTrans2.y += thrustVectorFinYOffset;
    aAmmo.modelData.localThrustVectorMainMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localThrustVectorMainMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans2 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustVectorTrans2);
    aAmmo.modelData.thrustVectorTrans2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));

    // thrust vector fin 3 
    DirectX::SimpleMath::Vector3 trustVectorTrans3 = thrustVectorBaseTrans;
    trustVectorTrans3.y += thrustVectorFinYOffset;
    aAmmo.modelData.localThrustVectorMainMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans3 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustVectorTrans3);
    aAmmo.modelData.thrustVectorTrans3 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));

    // thrust vector fin 4
    DirectX::SimpleMath::Vector3 trustVectorTrans4 = thrustVectorBaseTrans;
    trustVectorTrans4.y += thrustVectorFinYOffset;
    aAmmo.modelData.localThrustVectorMainMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans4 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.thrustVectorTrans4 *= DirectX::SimpleMath::Matrix::CreateTranslation(trustVectorTrans4);
    aAmmo.modelData.thrustVectorTrans4 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));

    // after burn plume base position
    DirectX::SimpleMath::Vector3 afterBurnPlumeBaseTrans = DirectX::SimpleMath::Vector3::Vector3::Zero;
    afterBurnPlumeBaseTrans.x = thrustVectorBaseTrans.x - (thrustVectorMainDimensions.x * 1.0f) - (thrustAxelRodDiameter * 1.5f);
    aAmmo.modelData.afterBurnPlumeBasePos = afterBurnPlumeBaseTrans;

    aAmmo.modelData.afterBurnPlumeThrottleModPos = DirectX::SimpleMath::Vector3::Zero;
    aAmmo.modelData.afterBurnPlumeThrottleModPos.x = -(thrustVectorMainDimensions.x * 2.0f);
}

void FireControl::InitializeLaserModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, LaserModel& aLazerModel)
{
    const float length = 1.0f;
    const float diameter = 0.2f;
    aLazerModel.laserShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), length, diameter);
    aLazerModel.laserShape2 = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), -length, -diameter);
    aLazerModel.localBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 transMat = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    aLazerModel.translationMatrix = DirectX::SimpleMath::Matrix::Identity;
    aLazerModel.localBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aLazerModel.localBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(transMat);
    aLazerModel.worldBodyMatrix = aLazerModel.localBodyMatrix;
}

void FireControl::InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateIcosahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeTextureMapsExplosion(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap)
{
    m_explosionStruct.textureExplosion = aTexture;
    m_explosionStruct.normalMapExplosion = aNormalMap;
    m_explosionStruct.specularExplosion = aSpecularMap;
}

void FireControl::PushVehicleExplosion(const DirectX::SimpleMath::Vector3 aPos, const int aVehicleId)
{
    std::tuple<DirectX::SimpleMath::Vector3, int> tupleToVec(aPos, aVehicleId);
    m_explosionStruct.explosionToPushVec.push_back(tupleToVec);
}

void FireControl::ResetMissileForceAccumulators(MissileData& aMissile)
{
    aMissile.projectileData.angularDragSum = DirectX::SimpleMath::Vector3::Zero;
    aMissile.projectileData.angularForceSum = DirectX::SimpleMath::Vector3::Zero;
    aMissile.guidance.linearDragSum = DirectX::SimpleMath::Vector3::Zero;
    aMissile.guidance.linearForceSum = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    const float mass = aProjectile->collisionData.mass;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = aProjectile->ammoData.dragCoefficient;
    float frontSurfaceArea = aProjectile->ammoData.frontSurfaceArea;
    float velocity = newQ.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 gravForce = (m_environment->GetGravityVec() * mass) * m_gravityMod;

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += airResistance;
    velocityUpdate += gravForce;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void FireControl::RightHandSideMissile(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the dependent variables.
    
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;

    // linear
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += newQ.velocity;
    velocityUpdate += aProjectile->guidance.linearDragSum;
    velocityUpdate += aProjectile->guidance.linearForceSum;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_missileConsts.mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    // angular
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    torqueAccum = newQ.angularVelocity;
    torqueAccum += aProjectile->projectileData.angularForceSum;
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_missileInverseInertiaTensorLocal);
    //torqueAccum += aProjectile->projectileData.angularDragSum;

    //DirectX::SimpleMath::Vector3 dragTorqueTest = aProjectile->projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, aTimeDelta);
    DirectX::SimpleMath::Vector3 dragTorqueTest = aProjectile->projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, 1.0f);
    torqueAccum += dragTorqueTest;
    //torqueAccum += aProjectile->projectileData.angularDragSum;

    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::RightHandSideMissileDebugTest(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the dependent variables.

    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
    //newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    //newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;

    // linear
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += newQ.velocity;
    velocityUpdate += aProjectile->guidance.linearDragSum;
    velocityUpdate += aProjectile->guidance.linearForceSum;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_missileConsts.mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    // angular
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    torqueAccum = newQ.angularVelocity;
    torqueAccum += aProjectile->projectileData.angularForceSum;
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_missileInverseInertiaTensorLocal);
    torqueAccum += aProjectile->projectileData.angularDragSum;
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    ProjectileMotion q = aProjectile->q;
    ProjectileMotion dq1;
    ProjectileMotion dq2;
    ProjectileMotion dq3;
    ProjectileMotion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aProjectile->time = aProjectile->time + static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aProjectile->q.velocity = q.velocity;
    aProjectile->q.position = q.position;
    aProjectile->collisionData.collisionSphere.Center = q.position;
    aProjectile->collisionData.velocity = q.velocity;
}

void FireControl::RungeKutta4Missile(struct MissileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    ProjectileMotion q = aProjectile->projectileData.q;
    ProjectileMotion dq1;
    ProjectileMotion dq2;
    ProjectileMotion dq3;
    ProjectileMotion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.

    RightHandSideMissile(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSideMissile(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSideMissile(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSideMissile(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);

    /*
    if (aProjectile->guidance.uniqueId % 2 == 0 || m_missileConsts.isUseDebugRG4True == false)
    {
        RightHandSideMissile(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
        RightHandSideMissile(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
        RightHandSideMissile(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
        RightHandSideMissile(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    }
    else
    {
        RightHandSideMissileDebugTest(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
        RightHandSideMissileDebugTest(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
        RightHandSideMissileDebugTest(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
        RightHandSideMissileDebugTest(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    }
    */

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;

    if (m_missileConsts.isMissileFreezeTrue == false)
    {
        q.velocity += velocityUpdate;
        q.position += posUpdate;
        q.angularMomentum += angularMomentumUpdate;
        q.angularVelocity += angularVelocityUpdate;
    }

    aProjectile->projectileData.q.velocity = q.velocity;
    aProjectile->projectileData.q.position = q.position;
    aProjectile->projectileData.collisionData.collisionSphere.Center = q.position;
    aProjectile->projectileData.collisionData.velocity = q.velocity;
    aProjectile->projectileData.q.angularVelocity = q.angularVelocity;
    aProjectile->projectileData.q.angularMomentum = q.angularMomentum;
}

void FireControl::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void FireControl::SetNPCController(std::shared_ptr<NPCController> aNPCController)
{
    m_npcController = aNPCController;
}

void FireControl::ToggleDebug1()
{
    if (m_isDebugToggleTrue1 == true)
    {
        m_isDebugToggleTrue1 = false;
    }
    else
    {
        m_isDebugToggleTrue1 = true;
    }
}

void FireControl::ToggleDebug2()
{
    if (m_isDebugToggleTrue2 == true)
    {
        m_isDebugToggleTrue2 = false;
    }
    else
    {
        m_isDebugToggleTrue2 = true;
    }
}

void FireControl::ToggleDebug3()
{
    if (m_isDebugToggleTrue3 == true)
    {
        m_isDebugToggleTrue3 = false;
    }
    else
    {
        m_isDebugToggleTrue3 = true;
    }
}

void FireControl::ToggleTargetingLaser()
{
    if (m_isTargetingLaserOn == false)
    {
        m_isTargetingLaserOn = true;
    }
    else
    {
        m_isTargetingLaserOn = false;
    }
}

void FireControl::TriggerMirvDeploy()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            if (m_projectileVec[i].isFuseTriggered == false)
            {
                m_projectileVec[i].isFuseTriggered = true;
            }
        }
    }
}

void FireControl::UpdateFireControl(double aTimeDelta)
{
    if (m_isTargetingLaserOn == true)
    {
        CastRayLaser();
    }
    if (m_isCoolDownActive == true)
    {
        m_coolDownTimer -= static_cast<float>(aTimeDelta);
        if (m_coolDownTimer <= 0.0f)
        {
            m_coolDownTimer = 0.0f;
            m_isCoolDownActive = false;
        }
    }

    if (m_muzzleFlash.isFlashActive == true)
    {
        UpdateMuzzleFlash(m_muzzleFlash, aTimeDelta);
    }

    m_testTimer += static_cast<float>(aTimeDelta);
    UpdateProjectileVec(aTimeDelta);
    UpdateMissileVec(aTimeDelta);
    UpdateExplosionVec(aTimeDelta);

    //m_debugData->DebugPushUILineWholeNumber("m_missileVec.size() = ", m_missileVec.size(), "");
    //m_debugData->DebugPushUILineWholeNumber("m_currentTargetID = ", m_currentTargetID, "");
}

void FireControl::UpdateDynamicExplosive(struct ExplosionData& aExplosion, const double aTimeDelta)
{
    Utility::MotionObj obj;
    obj.time = aExplosion.currentDuration;
    obj.q.angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    obj.q.position = aExplosion.position;
    obj.q.velocity = aExplosion.velocity;

    float durationRatio = aExplosion.currentDuration / aExplosion.totalDuration;
    float invsDurationRatio = 1.0f - durationRatio;

    if (invsDurationRatio <= 0.0f || invsDurationRatio >= 1.0f)
    {
        int testBreak = 0;
        testBreak++;
    }
    m_debugData->DebugPushUILineDecimalNumber("invsDurationRatio ", invsDurationRatio, "");
    float explosiveMass = 4.0f;
    //float explosiveMass = m_missileConsts.mass * invsDurationRatio;
    if (explosiveMass <= 0.0f)
    {
        const float minMass = 0.002f;
        explosiveMass = minMass;
    }
    obj.mass = explosiveMass;
    obj.mass = 10.0f;
    //sphere
    float radius = aExplosion.currentRadius;
    //radius = 3.0f;
    obj.inverseInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    obj.inverseInertiaMatrix._11 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix._22 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix._33 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix = obj.inverseInertiaMatrix.Invert();

    DirectX::SimpleMath::Vector3 linearForceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 gravForce = (m_environment->GetGravityVec() * obj.mass) * m_gravityMod;
    linearForceSum += gravForce;

    //  Compute the total drag force.
    const float airDensity = m_environment->GetAirDensity();
    //const float dragCoefficient = m_missileConsts.explosionDragCoefficientBase;
    //const float dragCoefficient = m_missileConsts.explosionDragCoefficientBase + (durationRatio * m_missileConsts.explosionDragCoefficientAddMax);
    const float dragCoefficient = 0.1f;
    //float frontSurfaceArea = radius * radius;
    float frontSurfaceArea = Utility::GetPi() * (radius * radius);
    float velocity = obj.q.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = obj.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 linearDragSum = DirectX::SimpleMath::Vector3::Zero;
    linearDragSum += airResistance;
    linearDragSum *= 0.1f;
    //m_debugData->DebugClearUI();
    m_debugData->DebugPushUILineDecimalNumber("airResistance.x ", airResistance.x, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.y ", airResistance.y, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.z ", airResistance.z, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.L ", airResistance.Length(), "");
    obj.linearAccumulated = linearForceSum;
    obj.linearDrag = linearDragSum;
    obj.torqueAccumulated = DirectX::SimpleMath::Vector3::Zero;
    obj.torqueDrag = DirectX::SimpleMath::Vector3::Zero;

    Utility::RungeKutta(&obj, aTimeDelta);
    aExplosion.position = obj.q.position;
    aExplosion.velocity = obj.q.velocity;

    const DirectX::SimpleMath::Vector3 updatedPos = obj.q.position;
    aExplosion.collisionSphere.Center = updatedPos;
    aExplosion.localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);

    DirectX::SimpleMath::Plane groundPlane;
    DirectX::SimpleMath::Vector3 groundNorm = DirectX::SimpleMath::Vector3::Zero;
    float height = 0.0f;
    m_environment->GetVehicleUpdateData(aExplosion.position, groundNorm, height, groundPlane);

    float altitude = aExplosion.position.y - height;

    if (altitude <= 0.0f)
    {
        DirectX::SimpleMath::Vector3 v = aExplosion.velocity;
        DirectX::SimpleMath::Vector3 n = groundNorm;
        const float b = m_environment->GetTerrainCoR();

        DirectX::SimpleMath::Vector3 postImpactVelocity = b * (-2.0f * (v.Dot(n)) * n + v);

        if (postImpactVelocity.y >= 0.0f)
        {
            aExplosion.velocity = postImpactVelocity;
        }
    }
}

void FireControl::UpdateExplosionVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        m_explosionStruct.explosionVec[i].currentDuration += aTimeDelta;
        if (m_explosionStruct.explosionVec[i].currentDuration > m_explosionStruct.explosionVec[i].totalDuration)
        {
            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
            {
                m_npcController->SetVehicleDeath(m_explosionStruct.explosionVec[i].vehicleExplosionID);
            }
            m_explosionStruct.explosionVec[i].isLifeTimeExpired = true;
        }
        else
        {
            float ratio = static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration / m_explosionStruct.explosionVec[i].totalDuration);
            float radius = m_explosionStruct.explosionVec[i].initialRadius + (ratio * m_explosionStruct.explosionVec[i].maxRadius);
            m_explosionStruct.explosionVec[i].currentRadius = radius;
            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
            {
                const DirectX::SimpleMath::Vector3 updatedPos = m_npcController->GetNpcPos(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                m_explosionStruct.explosionVec[i].position = updatedPos;
                m_explosionStruct.explosionVec[i].collisionSphere.Center = updatedPos;
                m_explosionStruct.explosionVec[i].localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
                radius *= m_explosionStruct.internalVehicleExplosionRadiusMod;
            }

            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE)
            {
                const DirectX::SimpleMath::Vector3 updatedVehiclePos = m_npcController->GetNpcPos(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                const DirectX::SimpleMath::Matrix updatedVehicleAlignment = m_npcController->GetNpcAlignment(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                DirectX::SimpleMath::Vector3 updatedExplosionPos = m_explosionStruct.explosionVec[i].localizedSurfaceExplosionPos;
                updatedExplosionPos = DirectX::SimpleMath::Vector3::Transform(updatedExplosionPos, updatedVehicleAlignment);
                updatedExplosionPos += updatedVehiclePos;
                m_explosionStruct.explosionVec[i].position = updatedExplosionPos;
                m_explosionStruct.explosionVec[i].collisionSphere.Center = updatedExplosionPos;
                m_explosionStruct.explosionVec[i].localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedExplosionPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
            }

            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_DYNAMIC)
            {
                UpdateDynamicExplosive(m_explosionStruct.explosionVec[i], aTimeDelta);
            }

            //upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
            const float rotMin = 0.0f;
            const float rotMax = DirectX::XM_2PI;
            //float yOffSet = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
            float yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            float pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            float rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat0 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat1 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat2 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat3 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat4 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat5 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat6 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat7 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat8 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            //m_explosionStruct.explosionVec[i].explosionMatrix0 = DirectX::SimpleMath::Matrix::CreateScale(radius);
            //m_explosionStruct.explosionVec[i].explosionMatrix0 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            const float scale = radius * 2.0f;
            m_explosionStruct.explosionVec[i].explosionMatrix0 = DirectX::SimpleMath::Matrix::CreateScale(scale);
            m_explosionStruct.explosionVec[i].explosionMatrix0 *= rotMat0;
            m_explosionStruct.explosionVec[i].explosionMatrix0 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            m_explosionStruct.explosionVec[i].collisionSphere.Radius = radius * 1.0f;

            float baseRedVal = 0.36f;
            float scatterVal0 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 9.0f);
            m_explosionStruct.explosionVec[i].color0 = DirectX::SimpleMath::Vector4((abs(scatterVal0 * 0.5f) + baseRedVal), (abs(scatterVal0 * 0.5f)), 0.0f, 1.0f);
            DirectX::SimpleMath::Vector4 currentColor = m_explosionStruct.explosionVec[i].explosionEndColor - m_explosionStruct.explosionVec[i].explosionStartColor;
            currentColor *= ratio;
            m_explosionStruct.explosionVec[i].explosionCurrentColor = currentColor + m_explosionStruct.explosionVec[i].explosionStartColor;

            //const float scatterDistanceMod = 0.15f * radius;
            const float scatterDistanceMod = 0.15f * scale;

            baseRedVal = 0.5f;
            float scatterVal1 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 10.0f);
            m_explosionStruct.explosionVec[i].color1 = DirectX::SimpleMath::Vector4((abs(scatterVal1 * 0.5f) + baseRedVal), (abs(scatterVal1 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance1 = scatterVal1 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans1(scatterDistance1, scatterDistance1, scatterDistance1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= rotMat1;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans1);
            //m_explosionStruct.explosionVec[i].explosionMatrix1 *= rotMat1;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal2 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 11.0f);
            m_explosionStruct.explosionVec[i].color2 = DirectX::SimpleMath::Vector4((abs(scatterVal2 * 0.5f) + baseRedVal), (abs(scatterVal2 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance2 = scatterVal2 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans2(-scatterDistance2, scatterDistance2, scatterDistance2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= rotMat2;
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal3 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 12.0f);
            m_explosionStruct.explosionVec[i].color3 = DirectX::SimpleMath::Vector4((abs(scatterVal3 * 0.5f) + baseRedVal), (abs(scatterVal3 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance3 = scatterVal3 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans3(scatterDistance3, -scatterDistance3, scatterDistance3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= rotMat3;
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal4 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 13.0f);
            m_explosionStruct.explosionVec[i].color4 = DirectX::SimpleMath::Vector4((abs(scatterVal4 * 0.5f) + baseRedVal), (abs(scatterVal4 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance4 = scatterVal4 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans4(scatterDistance4, scatterDistance4, -scatterDistance4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= rotMat4;
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal5 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 14.0f);
            m_explosionStruct.explosionVec[i].color5 = DirectX::SimpleMath::Vector4((abs(scatterVal5 * 0.5f) + baseRedVal), (abs(scatterVal5 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance5 = scatterVal5 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans5(-scatterDistance5, scatterDistance5, -scatterDistance5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= rotMat5;
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal6 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 15.0f);
            m_explosionStruct.explosionVec[i].color6 = DirectX::SimpleMath::Vector4((abs(scatterVal6 * 0.5f) + baseRedVal), (abs(scatterVal6 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance6 = scatterVal6 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans6(scatterDistance6, -scatterDistance6, -scatterDistance6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= rotMat6;
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal7 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 16.0f);
            m_explosionStruct.explosionVec[i].color7 = DirectX::SimpleMath::Vector4((abs(scatterVal7 * 0.5f) + baseRedVal), (abs(scatterVal7 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance7 = scatterVal7 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans7(-scatterDistance7, -scatterDistance7, scatterDistance7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= rotMat7;
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal8 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 17.0f);
            m_explosionStruct.explosionVec[i].color8 = DirectX::SimpleMath::Vector4((abs(scatterVal8 * 0.5f) + baseRedVal), (abs(scatterVal8 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance8 = scatterVal8 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans8(-scatterDistance8, -scatterDistance8, -scatterDistance8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= rotMat8;
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            const float remainingDuration = m_explosionStruct.explosionVec[i].totalDuration - m_explosionStruct.explosionVec[i].currentDuration;
            bool isNpcCollisionTrue = m_npcController->CheckExplosionCollisions(m_explosionStruct.explosionVec[i].collisionSphere, aTimeDelta, remainingDuration);
            if (isNpcCollisionTrue == true)
            {
                // to do : add collision detonation to vehicle
            }
        }
    }

    for (unsigned int i = 0; i < m_explosionStruct.explosionToPushVec.size(); ++i)
    {
        CreateExplosion(get<0>(m_explosionStruct.explosionToPushVec[i]), DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL, get<1>(m_explosionStruct.explosionToPushVec[i]));
    }
    m_explosionStruct.explosionToPushVec.clear();

    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        if (m_explosionStruct.explosionVec[i].isLifeTimeExpired == true)
        {
            std::vector<ExplosionData>::iterator it;
            it = m_explosionStruct.explosionVec.begin() + i;
            m_explosionStruct.explosionVec.erase(it);
        }
    }
}

void FireControl::UpdateFlightStateData(MissileData& aMissile, const double aTimeDelta)
{
    aMissile.projectileData.time += static_cast<float>(aTimeDelta);
    aMissile.guidance.timeStepDelta = static_cast<float>(aTimeDelta);
    float terrainHeightAtPos = m_environment->GetTerrainHeightAtPos(aMissile.projectileData.q.position);
    aMissile.guidance.altitude = aMissile.projectileData.q.position.y - terrainHeightAtPos;

    m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity);
    aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();
    aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;

    //AltitudeController(aMissile, aTimeDelta);

    // Update rocket throttle value
    float rocketThrottle;
    if (aMissile.projectileData.time >= m_missileConsts.rocketFireFullTime + m_missileConsts.rocketFireDelay)
    {
        rocketThrottle = 1.0f;
    }
    else if (aMissile.projectileData.time < m_missileConsts.rocketFireDelay)
    {
        rocketThrottle = 0.0f;
    }
    else
    {
        rocketThrottle = (aMissile.projectileData.time - m_missileConsts.rocketFireDelay) / (m_missileConsts.rocketFireFullTime);
    }
    //aMissile.guidance.throttlePercentage = (aMissile.guidance.throttlePercentage + rocketThrottle) * 0.5f;
    aMissile.guidance.throttlePercentage = rocketThrottle;

    if (aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_EXPLODING;
        aMissile.guidance.postExplosionDrawCountDown -= aTimeDelta;
        if (aMissile.guidance.postExplosionDrawCountDown <= 0.0f)
        {
            aMissile.projectileData.isDeleteTrue = true;
        }
    }

    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        if (aMissile.guidance.isFinsDeployStarted == false)
        {
            if (aMissile.projectileData.time <= m_missileConsts.finDeployDelay)
            {
                aMissile.guidance.finDeployPercent = 0.0f;
            }
            else
            {
                aMissile.guidance.isFinsDeployStarted = true;
            }
        }
        if (aMissile.guidance.isFinsDeployStarted == true)
        {
            if (aMissile.guidance.isFinsDeployEnd == false)
            {
                if (aMissile.projectileData.time >= m_missileConsts.finDeployTime + m_missileConsts.finDeployDelay)
                {
                    aMissile.guidance.isFinsDeployEnd == true;
                    aMissile.guidance.finDeployPercent = 1.0f;
                }
                else
                {
                    float finDeployPercent = (aMissile.projectileData.time - m_missileConsts.finDeployDelay) / (m_missileConsts.finDeployTime);
                    if (finDeployPercent > 1.0f || finDeployPercent < 0.0f)
                    {
                        int errorBreak = 0;
                        errorBreak++;
                        // throw error 
                        finDeployPercent = 1.0f;
                    }
                    aMissile.guidance.finDeployPercent = finDeployPercent;
                }
            }
        }

        if (aMissile.guidance.isTargetingLaserOn == false && aMissile.projectileData.time >= m_missileConsts.laserDepoyDelay)
        {
            aMissile.guidance.isTargetingLaserOn = true;
        }
        if (aMissile.guidance.isRocketFired == false && aMissile.projectileData.time >= m_missileConsts.rocketFireDelay)
        {
            aMissile.guidance.isRocketFired = true;
        }

        if (aMissile.guidance.isRocketFired == true)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CLIMBOUT;
            aMissile.guidance.isFinsDeployEnd = true;
            aMissile.guidance.finDeployPercent = 1.0f;
        }
    }
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        aMissile.guidance.climbOutTimer += static_cast<float>(aTimeDelta);
        if (aMissile.guidance.climbOutTimer >= m_missileConsts.climbOutDuration)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CRUISE;
        }
    }
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        if (aMissile.guidance.targetDistance <= m_missileConsts.terminalRange)
        {
            //aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_ATTACK;
        }
    }

    // Update center of pressure 
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH || aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT || 
        aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        float finDeployPercent = aMissile.guidance.finDeployPercent;
        if (finDeployPercent > 1.0f)
        {
            finDeployPercent = 1.0f;
        }
        else if (finDeployPercent < 0.0f)
        {
            finDeployPercent = 0.0f;
        }

        const float thrustAnglePercent = aMissile.guidance.thrustAngle / m_missileConsts.steerAngMax;
        const DirectX::SimpleMath::Vector3 thrustPosOffset = DirectX::SimpleMath::Vector3(0.3f, 0.0f, 0.0f);
        DirectX::SimpleMath::Vector3 centerOfThrust = m_missileConsts.thrustPosLocal;
        centerOfThrust += thrustPosOffset * thrustAnglePercent;
        aMissile.guidance.centerOfThrustLocalPos = centerOfThrust;

        DirectX::SimpleMath::Vector3 centerOfPressure = m_missileConsts.centerOfPressureBasePosLocal;
        centerOfPressure += m_missileConsts.centerOfPressureFullFinDeployOffset * aMissile.guidance.finDeployPercent;
        
        aMissile.guidance.centerOfPressureLocalPos = centerOfPressure;
    }
}

void FireControl::UpdateFlightStateOld(MissileData& aMissile, const double aTimeDelta)
{
    aMissile.projectileData.time += aTimeDelta;
    aMissile.guidance.timeStepDelta = static_cast<float>(aTimeDelta);
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        if (aMissile.guidance.isRocketFired == true)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CLIMBOUT;
        }
    }
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        if (aMissile.guidance.altitude >= m_missileConsts.climbOutAltMin)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CRUISE;
        }
    }
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        if (aMissile.guidance.targetDistance <= m_missileConsts.terminalRange)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_ATTACK;
        }
    }
}

void FireControl::UpdateMirv(ProjectileData& aProjectile, const double aTimeDelta)
{
    if (aProjectile.isFuseTriggered == true)
    {
        aProjectile.fuseTimer -= static_cast<float>(aTimeDelta);
        if (aProjectile.fuseTimer < 0.0f)
        {
            DeployMirv(aProjectile);
        }
    }

    if (aProjectile.isFuseTriggered == false && aProjectile.q.velocity.y < 0.0f)
    {
        aProjectile.isFuseTriggered = true;
        const float fuseDelay = m_mirvFuseDelayTime;
        aProjectile.fuseTimer = fuseDelay;
    }
}

void FireControl::UpdateMissileAlignment(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();

    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = aMissile.projectileData.q.angularVelocity.x;
    updateAlignQuat.y = aMissile.projectileData.q.angularVelocity.y;
    updateAlignQuat.z = aMissile.projectileData.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = aMissile.projectileData.alignmentQuat;
    updateAlignOutputQuat.Normalize();

    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * aMissile.projectileData.alignmentQuat);
    updateAlignOutputQuat.Normalize();

    aMissile.projectileData.alignmentQuat = updateAlignOutputQuat;

    aMissile.projectileData.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aMissile.projectileData.alignmentQuat);

    ////////////
    /*
    const float rotVal = Utility::ToRadians(1600.0f) * aTimeDelta;
    DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(aMissile.projectileData.forward, rotVal);
    aMissile.projectileData.alignmentQuat *= testQuat;
    aMissile.projectileData.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aMissile.projectileData.alignmentQuat);
    */
    ////////////

    aMissile.projectileData.inverseAlignmentQuat = aMissile.projectileData.alignmentQuat;
    aMissile.projectileData.inverseAlignmentQuat.Inverse(aMissile.projectileData.inverseAlignmentQuat);
    aMissile.projectileData.inverseAlignmentQuat.Normalize();

    aMissile.guidance.thrustPosWorldAligned = m_missileConsts.thrustPosLocal;
    aMissile.guidance.thrustPosWorldAligned = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.thrustPosWorldAligned, aMissile.projectileData.alignmentQuat);
}

void FireControl::UpdateMissileCoefficients(MissileData& aMissile, const float aTimeDelta)
{
    // calculate angle of attack
    DirectX::SimpleMath::Vector3 localizedVelocityNorm = aMissile.projectileData.q.velocity;
    localizedVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localizedVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);
    localizedVelocityNorm.z = 0.0f;
    localizedVelocityNorm.Normalize();
    float angleOfAttack = Utility::GetAngleBetweenVectors(localizedVelocityNorm, DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 localVelocityNormCross = localizedVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);

    aMissile.guidance.angleOfAttack = angleOfAttack;
    aMissile.guidance.angleOfAttack = Utility::WrapAngleRight(aMissile.guidance.angleOfAttack);
    //aMissile.guidance.angleOfAttack = Utility::ToDegrees(aMissile.guidance.angleOfAttack);
    aMissile.guidance.angleOfAttack = abs(aMissile.guidance.angleOfAttack);

    if (localVelocityNormCross.z < 0.0f)
    {
        angleOfAttack *= -1.0f;
    }

    const float angleMax = Utility::ToRadians(90.0f);
    const float angleMin = Utility::ToRadians(0.0f);
    const float currentCurvePos = (angleOfAttack / angleMax);
    //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack ", Utility::ToDegrees(angleOfAttack), "");
    //m_debugData->DebugPushUILineDecimalNumber("currentCurvePos ", currentCurvePos, "");
    float Cl;
    float curveDeltaRate;
    float ClTarget;
    if (currentCurvePos < 0.666f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else if (currentCurvePos < 0.80f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else
    {
        curveDeltaRate = -0.85f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    //m_debugData->DebugPushUILineDecimalNumber("Cl ******** ", Cl, "");
    // ////////////////////////////
    const float ClMax = 1.8;
    const float inflectionPoint = 0.99f;
    const float curvePos2 = currentCurvePos / inflectionPoint;
    if (currentCurvePos < inflectionPoint)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
        ClTarget = 1.5f;
        ClTarget = curvePos2 * ClMax;
    }
    else
    {
        float rightBound = 1.3f;
        float downCurve = rightBound - ClMax;

        const float curvePos4 = (currentCurvePos - inflectionPoint) / (1.0f - inflectionPoint);
        float ClRemove = curvePos4 * downCurve;
        ClTarget = ClMax;
        ClTarget += ClRemove;
    }
    //m_debugData->DebugPushUILineDecimalNumber("Cl ******** ", Cl, "");
    //m_debugData->DebugPushUILineDecimalNumber("ClTarget ******** ", ClTarget, "");


    ///////////////////////////////////////////////////
    // Testing lift coefficent calculations
    const float alphaClMax = 16.0f;
    float alpha = angleOfAttack;
    alpha = Utility::WrapAngleRight(alpha);
    alpha = Utility::ToDegrees(alpha);
    alpha = abs(alpha);
    const float clSlope0 = 0.0889f;
    const float clSlope1 = -0.1f;
    float cl0 = 0.178; // intercept of Cl-alpha curve
    float cl1 = 3.2;  // post-stall intercept of Cl-alpha curve
    float cl;

    if (alpha < alphaClMax)
    {
        cl = clSlope0 * alpha + cl0;
    }
    else
    {
        cl = clSlope1 * alpha + cl1;
    }

    /////////////////////////////////////////////////////////////////////
    // Drag coefficient
    //const float alphaDlMax = Utility::ToRadians(16.0f);
    const float alphaDlMax = 16.0f;
    float alphaD = angleOfAttack;
    float wrappedAlphaD = alphaD;
    wrappedAlphaD = Utility::WrapAngleRight(wrappedAlphaD);
    wrappedAlphaD = Utility::ToDegrees(wrappedAlphaD);
    m_debugData->DebugPushUILineDecimalNumber("wrappedAlphaD ", wrappedAlphaD, "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttack ", Utility::ToDegrees(angleOfAttack), "");
    alphaD = Utility::WrapAngleRight(alphaD);
    alphaD = Utility::ToDegrees(alphaD);
    alphaD = abs(alphaD);
    m_debugData->DebugPushUILineDecimalNumber("alphaD ", alphaD, "");
    const float dlSlope0 = 0.01f;
    const float dlSlope1 = 0.04f;
    float dl0 = 0.04f; // intercept of Cl-alpha curve
    float dl1 = 0.45;  // post-stall intercept of Cl-alpha curve
    float dl;

    // streamlined body coeff = 0.04f , full lock = ~ 2.0f, or 1.98f
    // 0.04 + 0.15

    if (alphaD < alphaDlMax)
    {
        dl = dlSlope0 * alphaD + dl0;
    }
    else
    {
        dl = dlSlope1 * alphaD + dl1;
    }
    aMissile.guidance.airFoilDragMod = dl;
    m_debugData->DebugPushUILineDecimalNumber("cl ", cl, "");
    m_debugData->DebugPushUILineDecimalNumber("dl ", dl, "");
    /////////////////////////////////////////////////////////////////////
    aMissile.guidance.liftCoefficient = cl;
    //return cl;
    ///////////////////////////////////////////////////
    if (ClTarget < 0.0f)
    {
        ClTarget = 0.0f;
        //ClTarget *= -1.0f;
    }
    ClTarget = 1.0f;
    //return ClTarget;
}

void FireControl::UpdateMissileDragLinear(MissileData& aMissile, const float aTimeDelta)
{
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = aMissile.projectileData.ammoData.dragCoefficient + aMissile.guidance.airFoilDragMod;
    if (aMissile.guidance.isExplodingTrue == true)
    {
        //dragCoefficient = m_missileConsts.postExplosionDragCoefficient;
        //mass = m_missileConsts.postExplosionMass;
    }

    float frontSurfaceArea = aMissile.projectileData.ammoData.frontSurfaceArea;
    float frontSurfaceAreaTest = (aMissile.projectileData.ammoData.radius * 2.0f) * 2.0f;
    float frontSurfaceAreatTest2 = Utility::GetPi() * (aMissile.projectileData.ammoData.radius * aMissile.projectileData.ammoData.radius);
    float sideSurfaceArea = aMissile.projectileData.ammoData.length * (aMissile.projectileData.ammoData.radius * 2.0f);
    float velocity = aMissile.projectileData.q.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);


    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += airResistance;


    float dragSurfaceArea = 0.0f;
    dragSurfaceArea += frontSurfaceArea * cos(aMissile.guidance.angleOfAttack);
    dragSurfaceArea += sideSurfaceArea * sin(aMissile.guidance.angleOfAttack);
    aMissile.guidance.dragSurfaceArea = dragSurfaceArea;

    float dragCoefficentSum = aMissile.projectileData.ammoData.dragCoefficient;
    const float sideProfileDragCoefficient = 0.82f; // long cylinder shape
    const float sideMinusFrontCl = sideProfileDragCoefficient - aMissile.projectileData.ammoData.dragCoefficient;

    dragCoefficentSum += abs(sideMinusFrontCl) * sin(aMissile.guidance.angleOfAttack);
    aMissile.guidance.dragCoefficientFull = abs(dragCoefficentSum + aMissile.guidance.airFoilDragMod);
}


void FireControl::UpdateMissileForcesOld(MissileData& aMissile, const float aTimeDelta)
{
    //m_debugData->ToggleDebugOnOverRide();
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forcePointThrust = m_missileConsts.thrustPosLocal;
    DirectX::SimpleMath::Vector3 boostForceLocal = CalculateBoostForceVec(aMissile);
    //boostForceLocal *= -1.0f;
    ///// debug start
    m_debugData->ToggleDebugOnOverRide();

    DirectX::SimpleMath::Vector3 forcePointThrustWorld = forcePointThrust;
    forcePointThrustWorld = DirectX::SimpleMath::Vector3::Transform(forcePointThrustWorld, aMissile.projectileData.alignmentQuat);
    forcePointThrustWorld += aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 boostForceWorld = boostForceLocal;
    boostForceWorld = DirectX::SimpleMath::Vector3::Transform(boostForceWorld, aMissile.projectileData.alignmentQuat);

    m_debugData->PushDebugLinePositionIndicatorAligned(forcePointThrustWorld, 1.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::Tan);
    m_debugData->PushDebugLine(forcePointThrustWorld, boostForceWorld, 2.0f, 0.0f, DirectX::Colors::Violet);

    //m_debugData->ToggleDebugOff();
    ///// debug stop
    Utility::AddForceAtPoint(boostForceLocal, forcePointThrust, centerOfMass, forceAccum, torqueAccum);

    DirectX::SimpleMath::Vector3 gravForceLocal = DirectX::SimpleMath::Vector3::Transform((m_environment->GetGravityVec() * m_missileConsts.mass), aMissile.projectileData.inverseAlignmentQuat);
    DirectX::SimpleMath::Vector3 forcePointGrav = centerOfMass;
    /////////
    DirectX::SimpleMath::Vector3 forceAccum2 = forceAccum;
    DirectX::SimpleMath::Vector3 torqueAccum2 = torqueAccum;


    DirectX::SimpleMath::Vector3 dragLinear = CalculeteDragLinearSum(aMissile, aTimeDelta);
    //DirectX::SimpleMath::Vector3 forcePointDrag = centerOfMass;
    DirectX::SimpleMath::Vector3 forcePointDrag = aMissile.guidance.centerOfPressureLocalPos;

    DirectX::SimpleMath::Vector3 forceAccumDrag = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccumDrag = DirectX::SimpleMath::Vector3::Zero;
    Utility::AddForceAtPoint(dragLinear, forcePointDrag, centerOfMass, forceAccumDrag, torqueAccumDrag);

    DirectX::SimpleMath::Vector3 forceSumLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceDragLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceSumAngular = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceDragAngular = DirectX::SimpleMath::Vector3::Zero;

    forceSumLinear = forceAccum;
    forceDragLinear = forceAccumDrag;
    forceSumLinear = DirectX::SimpleMath::Vector3::Transform(forceSumLinear, aMissile.projectileData.alignmentQuat);
    forceDragLinear = DirectX::SimpleMath::Vector3::Transform(forceDragLinear, aMissile.projectileData.alignmentQuat);

    forceSumAngular = torqueAccum;
    //forceDragAngular = torqueAccumDrag;
    forceDragAngular = aMissile.projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, aTimeDelta);
    //DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);
    //aMissile.projectileData.angularDragSum = angularDrag;

  //  forceSumAngular = DirectX::SimpleMath::Vector3::Transform(forceSumAngular, aMissile.projectileData.alignmentQuat);
  //  forceDragAngular = DirectX::SimpleMath::Vector3::Transform(forceDragAngular, aMissile.projectileData.alignmentQuat);

    DirectX::SimpleMath::Vector3 forceDragAngularTest1 = aMissile.projectileData.q.angularVelocity * -powf(0.3f, aTimeDelta);
    DirectX::SimpleMath::Vector3 forceDragAngularTest2 = aMissile.projectileData.q.angularVelocity * -powf(20.0f, aTimeDelta);
    //forceDragAngular = forceDragAngularTest2;
    float testVal1 = forceDragAngularTest1.Length();
    float testVal2 = forceDragAngularTest2.Length();
    float deltaVal = testVal2 - testVal1;
    m_debugData->DebugPushUILineDecimalNumber("testVal1 = ", testVal1, "");
    m_debugData->DebugPushUILineDecimalNumber("testVal2 = ", testVal2, "");
    m_debugData->DebugPushUILineDecimalNumber("deltaVal = ", deltaVal, "");

    if (deltaVal < 0.0f)
    {
        int testBreak = 0;
        testBreak++;
    }


    m_debugData->ToggleDebugOff();
    CalculateAirDragTorque(aMissile, aTimeDelta);

    ////// WindVaning start
    DirectX::SimpleMath::Vector3 windVaneTorque = CalculateWindVaningTorqueForce(aMissile);
    m_debugHeadingVec3 = windVaneTorque;
    m_debugVec1 = forceSumAngular;
    m_debugVec2 = forceDragAngular;
    ////// WindVaning end

    aMissile.guidance.linearDragSum = forceDragLinear;
    aMissile.guidance.linearForceSum = forceSumLinear;
    aMissile.projectileData.angularForceSum = forceSumAngular;
    aMissile.projectileData.angularDragSum = forceDragAngular;

    //m_debugHeadingVec3 = forceSumLinear;
}

void FireControl::UpdateMissileForcesLift(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.isFinsDeployEnd == false || aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.liftForce = DirectX::SimpleMath::Vector3::Zero;
        aMissile.guidance.liftForceFloat = 0.0f;
    }
    else
    {
        //const float cL = UpdateMissileLiftCoefficient(aMissile, aTimeDelta);
        UpdateMissileCoefficients(aMissile, aTimeDelta);
        const float cL = aMissile.guidance.liftCoefficient;
        const float airDensity = m_environment->GetAirDensity();
        const float wingArea = m_missileConsts.wingArea;

        DirectX::SimpleMath::Vector3 worldVelocityNorm = aMissile.projectileData.q.velocity;
        worldVelocityNorm.Normalize();
        float velocityDotProd = worldVelocityNorm.Dot(aMissile.projectileData.forward);
        float velocityLength = aMissile.projectileData.q.velocity.Length();

        float liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftV = cL * ((airDensity * (aMissile.projectileData.q.velocity * aMissile.projectileData.q.velocity)) * 0.5f) * wingArea;
        float liftVLength = liftV.Length();
        float rawVelocityLength = aMissile.projectileData.q.velocity.Length();

        DirectX::SimpleMath::Vector3 fowardVelocity = aMissile.projectileData.forward * (velocityDotProd * aMissile.projectileData.q.velocity);
        DirectX::SimpleMath::Vector3 lineOfFlight = aMissile.projectileData.q.velocity;
        DirectX::SimpleMath::Vector3 liftVector = cL * ((airDensity * (fowardVelocity * fowardVelocity)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftVectorLoF = cL * ((airDensity * (lineOfFlight * lineOfFlight)) * 0.5f) * wingArea;
        float forwardVelocityLength = fowardVelocity.Length();
        float forwardLength = aMissile.projectileData.forward.Length();
        float liftVectorLength = liftVector.Length();
        //m_debugData->DebugClearUI();
        //m_debugData->DebugPushUILineDecimalNumber("liftVectorLength ", liftVectorLength, "");
        //m_debugData->DebugPushUILineDecimalNumber("liftF             ", liftF, "");
        //m_debugData->DebugPushUILineDecimalNumber("liftVectorLoF.L   ", liftVectorLoF.Length(), "");
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftVector, 35.0f, 0.3f, DirectX::Colors::Orange);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftVectorLoF, 35.0f, 0.3f, DirectX::Colors::Lime);

        DirectX::SimpleMath::Vector3 chordLine = aMissile.projectileData.right;
        DirectX::SimpleMath::Vector3 liftLine = chordLine.Cross(worldVelocityNorm);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftLine, 35.0f, 0.3f, DirectX::Colors::Violet);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, chordLine, 35.0f, 0.3f, DirectX::Colors::Lime);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldVelocityNorm, 35.0f, 0.3f, DirectX::Colors::Orange);

        liftLine *= abs(liftF);

        //aMissile.guidance.liftForce = liftVector;
        aMissile.guidance.liftForce = liftLine;
        aMissile.guidance.liftForceFloat = liftF;


        float testLift = liftF;
        if (testLift < 0.0f)
        {
            testLift *= -1.0f;
        }
        if (testLift > 100.0f)
        {
            testLift = 100.0f;
        }

        DirectX::SimpleMath::Vector3 localizedNormY = DirectX::SimpleMath::Vector3::UnitY;
        localizedNormY = DirectX::SimpleMath::Vector3::Transform(localizedNormY, aMissile.projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 crossProdY = localizedNormY;
        crossProdY = crossProdY.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 worldCrossProdY = crossProdY;
        worldCrossProdY = DirectX::SimpleMath::Vector3::Transform(worldCrossProdY, aMissile.projectileData.alignmentQuat);

        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldCrossProdY, 100.0f, 0.0f, DirectX::Colors::Lavender);

        //DirectX::SimpleMath::Vector3 trueRightWorld = DirectX::SimpleMath::Vector3::UnitY.Cross(aMissile.projectileData.forward);
        DirectX::SimpleMath::Vector3 trueRightWorld = aMissile.projectileData.forward.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 trueRightLocal = trueRightWorld;
        trueRightLocal = DirectX::SimpleMath::Vector3::Transform(trueRightLocal, aMissile.projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 trueUpLocal = trueRightLocal.Cross(DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 trueUpWorld = trueUpLocal;
        trueUpWorld = DirectX::SimpleMath::Vector3::Transform(trueUpWorld, aMissile.projectileData.alignmentQuat);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, trueRightWorld, 100.0f, 0.0f, DirectX::Colors::White);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, trueUpWorld, 100.0f, 0.0f, DirectX::Colors::White);
        trueUpWorld.Normalize();

        //aMissile.guidance.liftForce = testLift * DirectX::SimpleMath::Vector3::UnitY;
        aMissile.guidance.liftForce = testLift * trueUpWorld;

        //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.liftForce.L   ", aMissile.guidance.liftForce.Length(), "");
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.liftForce, 200.0f, 0.0f, DirectX::Colors::Yellow);
        if (aMissile.guidance.liftForce.y < 0.0f)
        {
            int testBreak = 0;
            testBreak++;
        }
    }
}

void FireControl::UpdateMissileGuidance(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        /*
        aMissile.guidance.steeringDirNormLocal = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
        */

        aMissile.guidance.isFacingDestTrue = true;
        aMissile.guidance.headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        /*
        DirectX::SimpleMath::Vector3 destLocal = DirectX::SimpleMath::Vector3::UnitX;
        DirectX::SimpleMath::Vector3 destWorld = destLocal;
        destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);

        aMissile.guidance.steeringDirNormLocal = destLocal;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
        */
        aMissile.guidance.isFacingDestTrue = true;
        aMissile.guidance.headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;
        aMissile.guidance.headingLocalQuatTest.Inverse(aMissile.guidance.headingLocalQuatTest);
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
        //DirectX::SimpleMath::Vector3 destLocal = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
        float distToTargetTest = destLocal.Length();
        destLocal.Normalize();
        destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);

        bool isHeadingToDestTrue = false;
        if (destLocal.x >= 0.0f)
        {
            isHeadingToDestTrue = true;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }
        else
        {
            isHeadingToDestTrue = false;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }

        /*
        if (isHeadingToDestTrue == false)
        {
            destLocal = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            distToTargetTest = destLocal.Length();
            destLocal.Normalize();
            destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);
        }
        */

        aMissile.guidance.isFacingDestTrue = isHeadingToDestTrue;
        DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
        destQuat.Normalize();
        destQuat.Inverse(destQuat);

        aMissile.guidance.headingLocalVecTest = destLocal;
        aMissile.guidance.headingLocalQuatTest = destQuat;

        m_debugHeadingVec2 = destLocal;
        m_debugHeadingQuat2 = destQuat;
        m_debugDistanceToTarget2 = distToTargetTest;

        /*
        aMissile.guidance.isFacingDestTrue = true;
        aMissile.guidance.headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;
        */
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_EXPLODING)
    {
    }
}

void FireControl::UpdateMissileGuidanceOld(MissileData& aMissile, const float aTimeDelta)
{
    float terrainHeightAtPos = m_environment->GetTerrainHeightAtPos(aMissile.projectileData.q.position);
    aMissile.guidance.altitude = aMissile.projectileData.q.position.y - terrainHeightAtPos;

    if (aMissile.guidance.isFinsDeployStarted == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay)
    {
    }
    if (aMissile.guidance.isFinsDeployEnd == false && aMissile.projectileData.time <= m_missileConsts.finDeployDelay)
    {
        aMissile.guidance.finDeployPercent = 0.0f;
    }
    if (aMissile.guidance.isFinsDeployEnd == false)
    {
        if (aMissile.guidance.isFinsDeployStarted == false && aMissile.projectileData.time <= m_missileConsts.finDeployDelay)
        {
            aMissile.guidance.finDeployPercent = 0.0f;
        }
        else if (aMissile.projectileData.time >= m_missileConsts.finDeployDelay && aMissile.projectileData.time <= (m_missileConsts.finDeployDelay + m_missileConsts.finDeployTime))
        {
            float finDeployPercent2 = (aMissile.projectileData.time - m_missileConsts.finDeployDelay) / (m_missileConsts.finDeployTime);
            aMissile.guidance.finDeployPercent = finDeployPercent2;
        }
    }
    else
    {
        aMissile.guidance.finDeployPercent = 1.0f;
    }

    if (aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.postExplosionDrawCountDown -= aTimeDelta;
        if (aMissile.guidance.postExplosionDrawCountDown <= 0.0f)
        {
            aMissile.projectileData.isDeleteTrue = true;
        }
    }
    if (aMissile.guidance.isTargetingLaserOn == false && aMissile.projectileData.time >= m_missileConsts.laserDepoyDelay)
    {
        aMissile.guidance.isTargetingLaserOn = true;
    }
    if (aMissile.guidance.isRocketFired == false && aMissile.projectileData.time >= m_missileConsts.rocketFireDelay)
    {
        aMissile.guidance.isRocketFired = true;
    }
    if (aMissile.guidance.isFinsDeployStarted == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay)
    {
        aMissile.guidance.isFinsDeployStarted = true;
    }
    if (aMissile.guidance.isFinsDeployEnd == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay + m_missileConsts.finDeployTime)
    {
        aMissile.guidance.isFinsDeployEnd = true;
    }
    if (aMissile.guidance.isRocketFired == true)
    {
        if (aMissile.guidance.isTargetLocked == true)
        {
            m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity);
            aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();
            aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;

            AltitudeController(aMissile, aTimeDelta);

            const float maxAngle = m_missileConsts.stearingAngleMax;

            m_debugData->ToggleDebugOn();

            DirectX::SimpleMath::Quaternion invsAlign = aMissile.projectileData.inverseAlignmentQuat;
            invsAlign.Normalize();
            DirectX::SimpleMath::Vector3 destination = aMissile.guidance.targetDestination;
            DirectX::SimpleMath::Vector3 missilePos = aMissile.projectileData.q.position;
            DirectX::SimpleMath::Vector3 destinationLocalized = destination - missilePos;
            //m_debugData->PushDebugLine(missilePos, destinationLocalized, 400.0f, 0.0f, DirectX::Colors::Orange);
            destinationLocalized = DirectX::SimpleMath::Vector3::Transform(destinationLocalized, invsAlign);

            //destinationLocalized = DirectX::SimpleMath::Vector3::UnitX;
            aMissile.guidance.localizedDestinationPos = destinationLocalized;

            DirectX::SimpleMath::Vector3 toDestLocalNorm = destinationLocalized;
            toDestLocalNorm.Normalize();
            DirectX::SimpleMath::Vector3 destLocalUp = toDestLocalNorm.Cross(-DirectX::SimpleMath::Vector3::UnitZ);
            DirectX::SimpleMath::Vector3 destLocalRight = destLocalUp.Cross(-toDestLocalNorm);

            bool isHeadingToDestTrue = false;
            if (toDestLocalNorm.x >= 0.0f)
            {
                isHeadingToDestTrue = true;
                //m_debugData->DebugPushUILineWholeNumber("isHeadingToDestTrue ", isHeadingToDestTrue, "");
            }
            else
            {
                isHeadingToDestTrue = false;
                //m_debugData->DebugPushUILineWholeNumber("isHeadingToDestTrue ", isHeadingToDestTrue, "");
            }

            DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, toDestLocalNorm, destLocalUp);
            DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(updateMat);
            updateQuat.Normalize();
            aMissile.guidance.headingQuat = updateQuat;

            DirectX::SimpleMath::Quaternion rotLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
            //rotLimitQuat.RotateTowards(updateQuat, maxAngle);

            float testAngleMod = cos(m_testTimer);
            float testAngle = maxAngle * testAngleMod;
            //m_debugData->DebugPushUILineDecimalNumber("testAngle ", Utility::ToDegrees(testAngle), "");

            DirectX::SimpleMath::Vector3 testHeading = -DirectX::SimpleMath::Vector3::UnitZ;
            testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, updateMat);
            //m_debugData->PushDebugLine(missilePos, testHeading, 300.0f, 0.1f, DirectX::Colors::Red);

            aMissile.guidance.localizedDestinationDir = testHeading;
            testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, aMissile.projectileData.alignmentQuat);
            //m_debugData->PushDebugLine(missilePos, testHeading, 300.0f, 0.1f, DirectX::Colors::Blue);

            aMissile.guidance.heading = testHeading;
            aMissile.guidance.targetLaserAlignment *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(DirectX::SimpleMath::Quaternion::Identity);

            //CalculateGimbaledThrust(aMissile, aTimeDelta);

            m_debugData->ToggleDebugOff();
        }
    }
}

void FireControl::UpdateMissileModelData(MissileData& aMissile)
{
    if (aMissile.guidance.testBool == true)
    {
        aMissile.guidance.testBool = false;
    }
    else
    {
        aMissile.guidance.testBool = true;
    }

    if (aMissile.guidance.isRocketPlumeFlickerTrue == true)
    {
        aMissile.guidance.isRocketPlumeFlickerTrue = false;
    }
    else
    {
        aMissile.guidance.isRocketPlumeFlickerTrue = true;
    }

    // after burn plume sphere
    const DirectX::SimpleMath::Vector3 toUseSteeringVecLocal = aMissile.guidance.steeringDirNormLocal;
    const DirectX::SimpleMath::Quaternion toUseSteeringQuat = aMissile.guidance.steeringQuat;

    DirectX::SimpleMath::Matrix sphereMat = DirectX::SimpleMath::Matrix::Identity;
    float plumeBaseScale = 0.0f;
    float scale = 1.0f;
    float plumeConeScale = 0.0f;
    if (aMissile.guidance.testBool == true)
    {
        const float plumeflickerScaleLength = 0.5f;
        const float plumeflickerScaleWidth = 0.8f;
        plumeBaseScale = plumeflickerScaleWidth;
        sphereMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(plumeflickerScaleWidth, plumeflickerScaleWidth, plumeflickerScaleWidth));

        plumeConeScale = plumeflickerScaleWidth;
    }
    else
    {
        plumeBaseScale = scale;
        sphereMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(scale, scale, scale));

        plumeConeScale = scale;
    }

    DirectX::SimpleMath::Vector3 plumeLocalPos = m_ammoMissile.modelData.afterBurnPlumeThrottleModPos * aMissile.guidance.throttlePercentage;
    sphereMat *= DirectX::SimpleMath::Matrix::CreateTranslation(plumeLocalPos);
    sphereMat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(toUseSteeringQuat);
    sphereMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_ammoMissile.modelData.afterBurnPlumeBasePos);
    aMissile.guidance.afterBurnPlumeSphereMat = sphereMat;

    //////////////////////////////

    DirectX::SimpleMath::Vector3 plumePos = DirectX::SimpleMath::Vector3::Zero;
    plumePos.x -= (m_ammoMissile.modelData.afterBurnBaseSize * 0.5f) * plumeBaseScale;

    DirectX::SimpleMath::Matrix coneMat = DirectX::SimpleMath::Matrix::Identity;
    coneMat = DirectX::SimpleMath::Matrix::Identity;
    coneMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(plumeBaseScale, plumeBaseScale, plumeBaseScale));
    coneMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));

    DirectX::SimpleMath::Vector3 plumeConeBaseTrans = DirectX::SimpleMath::Vector3::Zero;
    plumeConeBaseTrans.x -= (m_ammoMissile.modelData.afterBurnConeBaseLength * 0.5f) * plumeConeScale;

    coneMat *= DirectX::SimpleMath::Matrix::CreateTranslation(plumeConeBaseTrans);
    coneMat *= DirectX::SimpleMath::Matrix::CreateTranslation(plumeLocalPos);
    coneMat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(toUseSteeringQuat);
    coneMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_ammoMissile.modelData.afterBurnPlumeBasePos);
    aMissile.guidance.afterBurnPlumeConeMat = coneMat;

    /////////////////////////////////////////////

    // fin steering
    DirectX::SimpleMath::Vector3 localizedHeading = aMissile.guidance.heading;
    localizedHeading = DirectX::SimpleMath::Vector3::Transform(localizedHeading, aMissile.projectileData.inverseAlignmentQuat);
    localizedHeading = toUseSteeringVecLocal;
    DirectX::SimpleMath::Vector3 vertVec = localizedHeading;
    vertVec.z = 0.0f;
    vertVec.Normalize();
    float vertAng = Utility::GetAngleBetweenVectors(vertVec, -DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 vertCross = vertVec.Cross(DirectX::SimpleMath::Vector3::UnitX);

    if (vertCross.z < 0.0f)
    {
        vertAng *= -1.0f;
    }

    DirectX::SimpleMath::Vector3 horzVec = localizedHeading;
    horzVec.y = 0.0f;
    horzVec.Normalize();
    float horzAng = Utility::GetAngleBetweenVectors(horzVec, -DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 horzCross = horzVec.Cross(DirectX::SimpleMath::Vector3::UnitX);

    if (horzCross.y < 0.0f)
    {
        horzAng *= -1.0f;
    }

    DirectX::SimpleMath::Vector3 testHeadingWorld = localizedHeading;
    testHeadingWorld = DirectX::SimpleMath::Vector3::Transform(testHeadingWorld, aMissile.projectileData.alignmentQuat);

    aMissile.guidance.finAngle1 = (aMissile.guidance.finAngle1 + horzAng) * 0.5f;
    aMissile.guidance.finAngle2 = (aMissile.guidance.finAngle2 - horzAng) * 0.5f;
    aMissile.guidance.finAngle3 = (aMissile.guidance.finAngle3 + vertAng) * 0.5f;
    aMissile.guidance.finAngle4 = (aMissile.guidance.finAngle4 - vertAng) * 0.5f;
}

void FireControl::UpdateMissileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        //UpdateMissileForces(m_missileVec[i], static_cast<float>(aTimeDelta));
        //RungeKutta4Missile(&m_missileVec[i], aTimeDelta);
    }

    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        UpdateFlightStateData(m_missileVec[i], aTimeDelta);
        UpdateMissileGuidance(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateSteeringDirNorm(m_missileVec[i], static_cast<float>(aTimeDelta));

        //UpdateMissileForces(m_missileVec[i], static_cast<float>(aTimeDelta));
        AccumulateMissileForces(m_missileVec[i], static_cast<float>(aTimeDelta));

        RungeKutta4Missile(&m_missileVec[i], aTimeDelta);

        ////// debug set missle to player cords & align
        if (m_isDebugToggleTrue1 == true)
        {
            m_missileVec[i].projectileData.q.position = m_playerVehicle->GetPos();
            m_missileVec[i].projectileData.q.velocity = m_playerVehicle->GetVelocity();
            m_missileVec[i].projectileData.q.angularVelocity = m_playerVehicle->GetAngularVelocity();
            m_missileVec[i].projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
        }
        ////// end : debug set missle to player cords & align

        UpdateMissileAlignment(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateMissileModelData(m_missileVec[i]);

        ////// start debug drawing
        //m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("velocty = ", m_missileVec[i].projectileData.q.velocity.Length(), "");
        if (m_missileVec[i].guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
        {
            m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_LAUNCH ", 0, "");
        }
        else if (m_missileVec[i].guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
        {
            m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_CLIMBOUT ", 0, "");
        }
        else if (m_missileVec[i].guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
        {
            m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_CRUISE ", 0, "");
        }
        else if (m_missileVec[i].guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
        {
            m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_ATTACK ", 0, "");
        }
        else
        {
            m_debugData->DebugPushUILineWholeNumber("Error!!!!!!! ", 0, "");
        }

        /*
        m_debugData->DebugPushUILineDecimalNumber("linearDragSum   = ", m_missileVec[i].guidance.linearDragSum.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("linearForceSum  = ", m_missileVec[i].guidance.linearForceSum.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("angularDragSum  = ", m_missileVec[i].projectileData.angularDragSum.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("angularForceSum = ", m_missileVec[i].projectileData.angularForceSum.Length(), "");

        m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue1 ", m_isDebugToggleTrue1, "");
        m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue2 ", m_isDebugToggleTrue2, "");
        m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue3 ", m_isDebugToggleTrue3, "");
        */

        /*
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.angularForceSum, 25.0f, 0.5f, DirectX::Colors::Yellow);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.angularDragSum, 25.0f, 0.0f, DirectX::Colors::Orange);

        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.linearForceSum, 15.0f, 1.0f, DirectX::Colors::LightSkyBlue);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.linearDragSum, 15.0f, 1.0f, DirectX::Colors::Red);

        m_debugData->DebugPushUILineDecimalNumber("angularForceSum ", m_missileVec[i].projectileData.angularForceSum.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("angularDragSum  ", m_missileVec[i].projectileData.angularDragSum.Length(), "");
        */

        m_debugData->ToggleDebugOff();
        //m_debugData->ToggleDebugOnOverRide();

        DirectX::SimpleMath::Vector3 testLine = m_debugHeadingVec1;
        testLine.Normalize();
        //testLine *= m_debugDistanceToTarget;
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.steeringQuat);
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 14.0f, 0.0f, DirectX::Colors::CornflowerBlue);
        testLine *= -1.0f;
        //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 10.0f, 0.0f, DirectX::Colors::Red);
        m_debugData->PushDebugLinePositionIndicator(m_missileVec[i].guidance.targetDestination, 20.0f, 0.0f, DirectX::Colors::Teal);


        testLine = m_debugHeadingVec2;
        testLine.Normalize();
        testLine *= m_debugDistanceToTarget2;
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.steeringQuat);
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 12.0f, 0.0f, DirectX::Colors::Yellow);
        testLine *= -1.0f;
        //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 10.0f, 0.0f, DirectX::Colors::Orange);
        //m_debugData->PushDebugLinePositionIndicator(m_missileVec[i].guidance.targetDestination, 20.0f, 0.0f, DirectX::Colors::Teal);

        testLine = m_debugHeadingVec3;
        //testLine.Normalize();
        //testLine *= m_debugDistanceToTarget2;
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.steeringQuat);
        //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);

        //m_debugData->DebugClearUI();
        
        /*
        testLine = m_debugVec1;
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 14.0f, 0.0f, DirectX::Colors::Tan);
        testLine = m_debugVec2;
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 12.0f, 0.0f, DirectX::Colors::DeepSkyBlue);

        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 10.0f, 0.0f, DirectX::Colors::Red);
        m_debugData->DebugPushUILineDecimalNumber("Wind Vene Torque = ", testLine.Length(), "");

        m_debugData->DebugPushUILineDecimalNumber("forceSumAngular = ", m_debugVec1.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("forceDragAngular = ", m_debugVec2.Length(), "");
        */

        /*
        m_debugData->DebugClearUI();

        testLine = m_missileVec[i].guidance.linearForceSum;
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 14.0f, 0.3f, DirectX::Colors::Violet);

        testLine = m_missileVec[i].guidance.linearDragSum;
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 12.0f, 0.1f, DirectX::Colors::Orange);

        testLine = m_missileVec[i].projectileData.q.velocity;
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 10.0f, 0.2f, DirectX::Colors::Lime);

        m_debugData->DebugPushUILineDecimalNumber("m_missileVec[i].guidance.linearForceSum.Length() = ", m_missileVec[i].guidance.linearForceSum.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("m_missileVec[i].guidance.linearDragSum.Length() = ", m_missileVec[i].guidance.linearDragSum.Length(), "");
        */

        /*
        m_debugData->DebugPushUILineDecimalNumber("velocity.Length() = ", m_missileVec[i].projectileData.q.velocity.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("angular .Length() = ", m_missileVec[i].projectileData.q.angularVelocity.Length(), "");


        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.linearForceSum, 10.0f, 0.0f, DirectX::Colors::LimeGreen);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.linearDragSum, 8.0f, 0.0f, DirectX::Colors::Red);

        m_debugData->ToggleDebugOnOverRide();
        testLine = m_missileVec[i].projectileData.angularForceSum;
        m_debugData->DebugPushUILineDecimalNumber("angularForceSum.Length() = ", testLine.Length(), "");
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 12.0f, 0.0f, DirectX::Colors::Lime);

        testLine = m_missileVec[i].projectileData.angularDragSum;
        m_debugData->DebugPushUILineDecimalNumber("angularDragSum.Length() = ", testLine.Length(), "");
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 12.0f, 0.0f, DirectX::Colors::Red);
        */

        m_debugData->ToggleDebugOff();
        ResetMissileForceAccumulators(m_missileVec[i]);
    }

    CheckCollisionsMissile();

    int deleteCount = 0;
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        if (m_missileVec[i].projectileData.isDeleteTrue == true)
        {
            float timer = m_missileVec[i].projectileData.time;
            deleteCount++;
            DeleteMissileFromVec(i);
        }
        else
        {
            //m_debugData->DebugPushUILineDecimalNumber("m_missileVec[i].guidance.forwardThrust ", m_missileVec[i].guidance.forwardThrust, "");
        }
    }
}

void FireControl::UpdateMuzzleFlash(MuzzleFlash& aMuzzleFlash, const double aTimeDelta)
{
    aMuzzleFlash.flashTimer += static_cast<float>(aTimeDelta);
    if (aMuzzleFlash.flashTimer >= aMuzzleFlash.flashDuration)
    {
        aMuzzleFlash.isFlashActive = false;
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.sizeMod = 0.0f;
    }
    const float durationPercentage = aMuzzleFlash.flashTimer / aMuzzleFlash.flashDuration;
    const float inverseDurationPercentage = 1.0f - durationPercentage;
    if (durationPercentage <= 0.5f)
    {
        aMuzzleFlash.sizeMod += aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);
    }
    else
    {
        aMuzzleFlash.sizeMod -= aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);
    }
    //aMuzzleFlash.sizeMod += aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector4 colorTest = aMuzzleFlash.startColor - aMuzzleFlash.endColor;
    aMuzzleFlash.currentColor = aMuzzleFlash.startColor - (colorTest * durationPercentage);
    //aMuzzleFlash.currentColor.w = 1.0f;
    //aMuzzleFlash.currentColor.x = 1.0f;

    DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());
    const float maxModSize = aMuzzleFlash.flashDuration * aMuzzleFlash.growthRate;

    float scale = aMuzzleFlash.sizeMod;
    if (aMuzzleFlash.isFlickerTrue == true)
    {
        scale *= aMuzzleFlash.flickerScale;
        aMuzzleFlash.isFlickerTrue = false;
    }
    else
    {
        aMuzzleFlash.isFlickerTrue = true;
    }

    const float testRot = Utility::WrapAngle(m_testTimer * 345.2345f);
    //const float scaleTransOffset = (scale * 1.0f) * 0.5f;
    const float scaleTransOffset = ((maxModSize * durationPercentage) * 1.0f) * 0.5f;
    DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));
    DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(-scale, scale, -scale));
    DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);
    aMuzzleFlash.worldTestMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.1f * -scale, 0.0f));
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.2f, 0.0f));
    aMuzzleFlash.worldTestMatrix *= scaleMat;
    aMuzzleFlash.worldTestMatrix *= scaleTransOffsetMat;
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetMuzzlePos());
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
    aMuzzleFlash.worldTestMatrix *= updateMat;

    aMuzzleFlash.worldMuzzleFlashConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= scaleMat;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= scaleTransOffsetMat;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, (-0.5f * scale), 0.0f));
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetMuzzlePos());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= updateMat;
}

void FireControl::UpdateProjectileData(ProjectileData& aProjectile, const float aTimeDelta)
{
    aProjectile.time += aTimeDelta;
    DirectX::SimpleMath::Vector3 velocityNorm = aProjectile.q.velocity;
    velocityNorm.Normalize();
    const float t = 4.0f;
    aProjectile.forward = DirectX::SimpleMath::Vector3::SmoothStep(aProjectile.forward, velocityNorm, t * aTimeDelta);
    //aProjectile.forward = DirectX::SimpleMath::Vector3::Lerp(aProjectile.forward, velocityNorm, 0.01f);
    aProjectile.up = aProjectile.right.Cross(aProjectile.forward);
    aProjectile.right = aProjectile.forward.Cross(aProjectile.up);

    DirectX::SimpleMath::Vector3 pos2 = aProjectile.q.position;
    DirectX::SimpleMath::Vector3 up2 = aProjectile.up;
    DirectX::SimpleMath::Vector3 right2 = aProjectile.right;
    //m_debugData->PushDebugLine(pos2, right2, 10.0f, 0.0f, DirectX::Colors::Orange);

    //DirectX::SimpleMath::Vector3 forward = aProjectile.q.velocity;
    DirectX::SimpleMath::Vector3 forward = aProjectile.forward;
    if (forward.Length() < 0.00001f)
    {
        forward = DirectX::SimpleMath::Vector3::UnitX;
    }
    forward.Normalize();
    //DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitY.Cross(forward);
    DirectX::SimpleMath::Vector3 right = aProjectile.right;
    //DirectX::SimpleMath::Vector3 up = right.Cross(forward);
    DirectX::SimpleMath::Vector3 up = aProjectile.up;
    //m_debugData->PushDebugLine(pos2, up, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(pos2, right, 10.0f, 0.0f, DirectX::Colors::Orange);
    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -right, up);
    aProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(alignMat);
    //aProjectile.alignmentQuat.Normalize();
    aProjectile.inverseAlignmentQuat = aProjectile.alignmentQuat;
    aProjectile.inverseAlignmentQuat.Inverse(aProjectile.inverseAlignmentQuat);
    //aProjectile.inverseAlignmentQuat.Normalize();

    aProjectile.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aProjectile.alignmentQuat);
    aProjectile.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aProjectile.alignmentQuat);
    aProjectile.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aProjectile.alignmentQuat);
}

void FireControl::UpdateProjectileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        RungeKutta4(&m_projectileVec[i], aTimeDelta);
    }

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            UpdateMirv(m_projectileVec[i], aTimeDelta);
        }
    }

    if (m_newProjectilePushVec.size() > 0)
    {
        for (unsigned int i = 0; i < m_newProjectilePushVec.size(); ++i)
        {
            m_projectileVec.push_back(m_newProjectilePushVec[i]);
        }
        m_newProjectilePushVec.clear();
    }

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        UpdateProjectileData(m_projectileVec[i], static_cast<float>(aTimeDelta));
    }

    CheckCollisions();

    int deleteCount = 0;
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].isDeleteTrue == true)
        {
            deleteCount++;
            DeleteProjectileFromVec(i);
        }
    }
}

void FireControl::UpdateSteeringDirNorm(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE || aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
        //m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.x ", aMissile.guidance.headingLocalVecTest.x, "");
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.y ", aMissile.guidance.headingLocalVecTest.y, "");
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.z ", aMissile.guidance.headingLocalVecTest.z, "");
        m_debugData->ToggleDebugOff();
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
        DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;

        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        //steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        const float maxAng = m_missileConsts.steerAngMax;
        DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
        steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

        const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
        DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
        DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
        DirectX::SimpleMath::Quaternion updateQuat = preQuat;
        updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

        DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
        steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

        aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
        aMissile.guidance.steeringQuat = updateQuat;
        aMissile.guidance.thrustAngle = Utility::GetAngleBetweenVectors(steeringUpdateVec, -DirectX::SimpleMath::Vector3::UnitX);
        m_debugHeadingVec1 = steeringUpdateVec;
        m_debugHeadingQuat1 = updateQuat;
        m_debugDistanceToTarget = aMissile.guidance.targetDistance;
    }
    //aMissile.guidance.steeringDirNormLocal = -DirectX::SimpleMath::Vector3::UnitX;
    //aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;

    /*
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        aMissile.guidance.steeringDirNormLocal = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        DirectX::SimpleMath::Vector3 destLocal = DirectX::SimpleMath::Vector3::UnitX;
        DirectX::SimpleMath::Vector3 destWorld = destLocal;
        destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);

        aMissile.guidance.steeringDirNormLocal = destLocal;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
        destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);

        bool isHeadingToDestTrue = false;
        if (destLocal.x >= 0.0f)
        {
            isHeadingToDestTrue = true;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }
        else
        {
            isHeadingToDestTrue = false;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }

        if (isHeadingToDestTrue == false)
        {
            destLocal = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);
        }
        aMissile.guidance.isFacingDestTrue = isHeadingToDestTrue;

        DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
        destQuat.Normalize();
        destQuat.Inverse(destQuat);

        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        const float maxAng = m_missileConsts.steerAngMax;
        DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
        steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

        const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
        DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
        DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
        DirectX::SimpleMath::Quaternion updateQuat = preQuat;
        updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

        DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
        steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

        aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
        aMissile.guidance.steeringQuat = updateQuat;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
        DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;


        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        const float maxAng = m_missileConsts.steerAngMax;
        DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
        steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

        const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
        DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
        DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
        DirectX::SimpleMath::Quaternion updateQuat = preQuat;
        updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

        DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
        steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

        aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
        aMissile.guidance.steeringQuat = updateQuat;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_EXPLODING)
    {
    }
    */
}

void FireControl::UpdateSteeringDirNormOld(MissileData& aMissile, const float aTimeDelta)
{
    //m_debugData->ToggleDebugOn();
    //m_debugData->DebugClearUI();
    m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, aMissile.guidance.targetDestination, DirectX::Colors::White);

    float reverseMod = 1.0f;
    if (m_isDebugToggleTrue2 == true)
    {
        reverseMod = -1.0f;
    }

    float yawDelta = Utility::ToRadians(20.0f) * reverseMod;
    float pitchDelta = Utility::ToRadians(0.0f) * reverseMod;
    float rollDelta = Utility::ToRadians(0.0f) * reverseMod;

    if (m_isDebugToggleTrueTestConst1 == false || 1 == 1)
    {
        yawDelta = Utility::ToRadians(0.0f) * reverseMod;
        pitchDelta = Utility::ToRadians(0.0f) * reverseMod;
        rollDelta = Utility::ToRadians(0.0f) * reverseMod;
    }

    m_testRotYaw = Utility::WrapAngle(m_testRotYaw + (yawDelta * aTimeDelta));
    m_testRotPitch = Utility::WrapAngle(m_testRotPitch + (pitchDelta * aTimeDelta));
    m_testRotRoll = Utility::WrapAngle(m_testRotRoll + (rollDelta * aTimeDelta));
    const float yaw = m_testRotYaw + Utility::ToRadians(0.0f);
    const float pitch = m_testRotPitch + Utility::ToRadians(0.0f);
    const float roll = m_testRotRoll + Utility::ToRadians(0.0f);

    /*
    DirectX::SimpleMath::Vector3 testDestWorld = DirectX::SimpleMath::Vector3::UnitX * 50.0f;
    testDestWorld = DirectX::SimpleMath::Vector3::Transform(testDestWorld, DirectX::SimpleMath::
        Quaternion::CreateFromYawPitchRoll(yaw, roll, pitch));
    testDestWorld = DirectX::SimpleMath::Vector3::Transform(testDestWorld, aMissile.projectileData.alignmentQuat);
    testDestWorld += aMissile.projectileData.q.position;
    */
    DirectX::SimpleMath::Vector3 testDestLocal = DirectX::SimpleMath::Vector3::UnitX * 50.0f;
    testDestLocal = DirectX::SimpleMath::Vector3::Transform(testDestLocal, DirectX::SimpleMath::
        Quaternion::CreateFromYawPitchRoll(yaw, roll, pitch));
    //testDestWorld = DirectX::SimpleMath::Vector3::Transform(testDestWorld, aMissile.projectileData.alignmentQuat);
    //testDestWorld += aMissile.projectileData.q.position;

    ////////////////////////
    ///////////////////////

    testDestLocal = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
    testDestLocal = DirectX::SimpleMath::Vector3::Transform(testDestLocal, aMissile.projectileData.inverseAlignmentQuat);


    DirectX::SimpleMath::Vector3 toDestLocalNorm = testDestLocal;
    m_debugData->ToggleDebugOn();
    m_debugData->DebugPushUILineDecimalNumber("toDestLocalNorm.x = ", toDestLocalNorm.x, "");
    m_debugData->DebugPushUILineDecimalNumber("toDestLocalNorm.y = ", toDestLocalNorm.y, "");
    m_debugData->DebugPushUILineDecimalNumber("toDestLocalNorm.z = ", toDestLocalNorm.z, "");
    m_debugData->DebugPushUILineDecimalNumber("toDestLocalNorm.l = ", toDestLocalNorm.Length(), "");


    bool isHeadingToDestTrue = false;
    if (toDestLocalNorm.x >= 0.0f)
    {
        isHeadingToDestTrue = true;
        m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
    }
    else
    {
        isHeadingToDestTrue = false;
        m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
    }

    if (isHeadingToDestTrue == false)
    {
        //destLocal.x *= -1.0f;
        testDestLocal = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
        testDestLocal = DirectX::SimpleMath::Vector3::Transform(testDestLocal, aMissile.projectileData.inverseAlignmentQuat);
    }
    m_debugData->ToggleDebugOff();

    aMissile.guidance.isFacingDestTrue = isHeadingToDestTrue;

    ////////////////////////
    ///////////////////////

    DirectX::SimpleMath::Vector3 compassVec = DirectX::SimpleMath::Vector3::UnitX;
    compassVec = DirectX::SimpleMath::Vector3::Transform(compassVec, DirectX::SimpleMath::
        Quaternion::CreateFromYawPitchRoll(yaw, roll, pitch));

    DirectX::SimpleMath::Vector3 testDestWorld = testDestLocal;
    testDestWorld = DirectX::SimpleMath::Vector3::Transform(testDestWorld, aMissile.projectileData.alignmentQuat);
    testDestWorld += aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 testDestLocalFromWorld = testDestWorld;
    testDestLocalFromWorld -= aMissile.projectileData.q.position;
    // testDestLocalFromWorld = DirectX::SimpleMath::Vector3::Transform(testDestLocalFromWorld, aMissile.projectileData.inverseAlignmentQuat);

    DirectX::SimpleMath::Vector3 normToTargetWorld = testDestWorld - aMissile.projectileData.q.position;
    normToTargetWorld.Normalize();
    DirectX::SimpleMath::Vector3 normToTargetLocal = normToTargetWorld;
    normToTargetLocal = DirectX::SimpleMath::Vector3::Transform(normToTargetLocal, aMissile.projectileData.inverseAlignmentQuat);
    DirectX::SimpleMath::Vector3 normToTargetLocalToWorld = normToTargetLocal;
    normToTargetLocalToWorld = DirectX::SimpleMath::Vector3::Transform(normToTargetLocalToWorld, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOn();
    m_debugData->PushDebugLinePositionIndicator(testDestWorld, 25.0f, 0.0f, DirectX::Colors::White);

    //DirectX::SimpleMath::Vector3 compassPos = m_playerVehicle->GetPos() + DirectX::SimpleMath::Vector3(-10.0f, 10.0f, 10.0f);
    DirectX::SimpleMath::Vector3 compassPos = aMissile.projectileData.q.position;// +DirectX::SimpleMath::Vector3(-10.0f, 10.0f, 10.0f);
    m_debugData->PushDebugLinePositionIndicatorAligned(compassPos, 5.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::White);

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testDestLocalFromWorld, 40.0f, 0.0f, DirectX::Colors::White);
    DirectX::SimpleMath::Vector3 testDestWorldFromLocal = testDestLocalFromWorld;
    testDestWorldFromLocal = DirectX::SimpleMath::Vector3::Transform(testDestWorldFromLocal, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testDestWorldFromLocal, 50.0f, 0.5f, DirectX::Colors::HotPink);
    //m_debugData->DebugPushUILineDecimalNumber("$$testDestWorldFromLocal.Length() = ", testDestWorldFromLocal.Length(), "");

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, testDestWorld - aMissile.projectileData.q.position, 50.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, testDestWorld, 50.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, normToTargetWorld, 50.0f, 0.0f, DirectX::Colors::Violet);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, normToTargetLocal, 50.0f, 1.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, normToTargetLocalToWorld, 50.0f, 1.0f, DirectX::Colors::Orange);
    ///////////////////////////////////////////////////
    DirectX::SimpleMath::Vector3 testDestWorld2 = DirectX::SimpleMath::Vector3::UnitX * 50.0f;
    testDestWorld2 = DirectX::SimpleMath::Vector3::Transform(testDestWorld2, DirectX::SimpleMath::
        Quaternion::CreateFromYawPitchRoll(yaw, roll, pitch));
    //testDestWorld2 = DirectX::SimpleMath::Vector3::Transform(testDestWorld2, aMissile.projectileData.alignmentQuat);
    testDestWorld2 += aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 normToTargetWorld2 = testDestWorld2 - aMissile.projectileData.q.position;
    normToTargetWorld2.Normalize();
    DirectX::SimpleMath::Vector3 normToTargetLocal2 = normToTargetWorld2;
    normToTargetLocal2 = DirectX::SimpleMath::Vector3::Transform(normToTargetLocal2, aMissile.projectileData.inverseAlignmentQuat);
    DirectX::SimpleMath::Vector3 normToTargetLocalToWorld2 = normToTargetLocal2;
    normToTargetLocalToWorld2 = DirectX::SimpleMath::Vector3::Transform(normToTargetLocalToWorld2, aMissile.projectileData.alignmentQuat);

    m_debugData->PushDebugLinePositionIndicator(testDestWorld2, 15.0f, 0.0f, DirectX::Colors::Wheat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, normToTargetLocalToWorld2, 50.0f, 2.0f, DirectX::Colors::OrangeRed);
    //m_debugData->ToggleDebugOff();

    //DirectX::SimpleMath::Vector3 destWorld = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 destWorld = m_testTargetPos - aMissile.projectileData.q.position;
    destWorld = testDestWorld - aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 destLocal = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.inverseAlignmentQuat);
    //const DirectX::SimpleMath::Vector3 localUp = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.inverseAlignmentQuat);

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, destWorld, 130.0f, 0.0f, DirectX::Colors::Green);
    m_debugData->ToggleDebugOff();


    DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    DirectX::SimpleMath::Matrix destMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(destQuat);
    destMat = destMat.Invert();
    //destQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(destMat);
    destQuat.Normalize();

    destQuat.Inverse(destQuat);

    DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOn();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, steeringLine, 50.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->ToggleDebugOff();

    //const float maxAng = Utility::ToRadians(15.0f);
    const float maxAng = m_missileConsts.steerAngMax;
    DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
    steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);
    //   steeringConeLimitQuat = steeringConeQuat;   /// WTF??

    DirectX::SimpleMath::Vector3 testLineSteeringRaw = -DirectX::SimpleMath::Vector3::UnitX;
    testLineSteeringRaw = DirectX::SimpleMath::Vector3::Transform(testLineSteeringRaw, steeringConeQuat);
    testLineSteeringRaw = DirectX::SimpleMath::Vector3::Transform(testLineSteeringRaw, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOn();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testLineSteeringRaw, 50.0f, 1.0f, DirectX::Colors::Yellow);
    m_debugData->ToggleDebugOff();

    const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
    DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
    DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
    DirectX::SimpleMath::Quaternion updateQuat = preQuat;
    //updateQuat.RotateTowards(testQuat2, maxAngPerTime);
    updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);


    DirectX::SimpleMath::Vector3 testLineConeRaw = -DirectX::SimpleMath::Vector3::UnitX;
    testLineConeRaw = DirectX::SimpleMath::Vector3::Transform(testLineConeRaw, steeringConeLimitQuat);
    testLineConeRaw = DirectX::SimpleMath::Vector3::Transform(testLineConeRaw, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOn();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testLineConeRaw, 50.0f, 2.0f, DirectX::Colors::Violet);
    m_debugData->ToggleDebugOff();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
    steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);
    //steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, testQuat2);


    aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
    aMissile.guidance.steeringQuat = updateQuat;
    steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, aMissile.projectileData.alignmentQuat);
    m_debugData->ToggleDebugOn();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, steeringUpdateVec, 25.0f, 0.0f, DirectX::Colors::LimeGreen);

    m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue1", m_isDebugToggleTrue1, "");
    m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue2", m_isDebugToggleTrue2, "");
    m_debugData->DebugPushUILineWholeNumber("m_isDebugToggleTrue3", m_isDebugToggleTrue3, "");
    m_debugData->ToggleDebugOff();

    m_debugData->ToggleDebugOff();
}

void FireControl::UpdateSteeringDirNormOld2(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
    DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;

    DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

    const float maxAng = m_missileConsts.steerAngMax;
    DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
    steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

    const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
    DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
    DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
    DirectX::SimpleMath::Quaternion updateQuat = preQuat;
    updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

    DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
    steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

    aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
    aMissile.guidance.steeringQuat = updateQuat;
    /*
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        aMissile.guidance.steeringDirNormLocal = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        DirectX::SimpleMath::Vector3 destLocal = DirectX::SimpleMath::Vector3::UnitX;
        DirectX::SimpleMath::Vector3 destWorld = destLocal;
        destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);

        aMissile.guidance.steeringDirNormLocal = destLocal;
        aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
        destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);

        bool isHeadingToDestTrue = false;
        if (destLocal.x >= 0.0f)
        {
            isHeadingToDestTrue = true;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }
        else
        {
            isHeadingToDestTrue = false;
            m_debugData->DebugPushUILineWholeNumber("!$!isHeadingToDestTrue ", isHeadingToDestTrue, "");
        }

        if (isHeadingToDestTrue == false)
        {
            destLocal = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);
        }
        aMissile.guidance.isFacingDestTrue = isHeadingToDestTrue;

        DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
        destQuat.Normalize();
        destQuat.Inverse(destQuat);

        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        const float maxAng = m_missileConsts.steerAngMax;
        DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
        steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

        const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
        DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
        DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
        DirectX::SimpleMath::Quaternion updateQuat = preQuat;
        updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

        DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
        steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

        aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
        aMissile.guidance.steeringQuat = updateQuat;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
        DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;


        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        const float maxAng = m_missileConsts.steerAngMax;
        DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, steeringLine);
        steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

        const float maxAngPerTime = aTimeDelta * m_missileConsts.steeringAngPerSecDeltaMax;
        DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.steeringDirNormLocal;
        DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.steeringQuat;
        DirectX::SimpleMath::Quaternion updateQuat = preQuat;
        updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

        DirectX::SimpleMath::Vector3 steeringUpdateVec = -DirectX::SimpleMath::Vector3::UnitX;
        steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);

        aMissile.guidance.steeringDirNormLocal = steeringUpdateVec;
        aMissile.guidance.steeringQuat = updateQuat;
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_EXPLODING)
    {
    }
    */
}




void FireControl::UpdateMissileForces(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forcePointThrust = m_missileConsts.thrustPosLocal;
    DirectX::SimpleMath::Vector3 boostForceLocal = CalculateBoostForceVec(aMissile);
    //boostForceLocal *= -1.0f;
    ///// debug start
    //m_debugData->ToggleDebugOnOverRide();

    DirectX::SimpleMath::Vector3 forcePointThrustWorld = forcePointThrust;
    forcePointThrustWorld = DirectX::SimpleMath::Vector3::Transform(forcePointThrustWorld, aMissile.projectileData.alignmentQuat);
    forcePointThrustWorld += aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 boostForceWorld = boostForceLocal;
    boostForceWorld = DirectX::SimpleMath::Vector3::Transform(boostForceWorld, aMissile.projectileData.alignmentQuat);

    m_debugData->PushDebugLinePositionIndicatorAligned(forcePointThrustWorld, 1.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::Tan);
    m_debugData->PushDebugLine(forcePointThrustWorld, boostForceWorld, 2.0f, 0.0f, DirectX::Colors::Violet);

    //m_debugData->ToggleDebugOff();
    ///// debug stop
    Utility::AddForceAtPoint(boostForceLocal, forcePointThrust, centerOfMass, forceAccum, torqueAccum);

    DirectX::SimpleMath::Vector3 gravForceLocal = DirectX::SimpleMath::Vector3::Transform((m_environment->GetGravityVec() * m_missileConsts.mass), aMissile.projectileData.inverseAlignmentQuat);
    DirectX::SimpleMath::Vector3 forcePointGrav = centerOfMass;
    /////////
    DirectX::SimpleMath::Vector3 forceAccum2 = forceAccum;
    DirectX::SimpleMath::Vector3 torqueAccum2 = torqueAccum;


    DirectX::SimpleMath::Vector3 dragLinear = CalculeteDragLinearSum(aMissile, aTimeDelta);
    //DirectX::SimpleMath::Vector3 forcePointDrag = centerOfMass;
    DirectX::SimpleMath::Vector3 forcePointDrag = aMissile.guidance.centerOfPressureLocalPos;

    DirectX::SimpleMath::Vector3 forceAccumDrag = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccumDrag = DirectX::SimpleMath::Vector3::Zero;
    Utility::AddForceAtPoint(dragLinear, forcePointDrag, centerOfMass, forceAccumDrag, torqueAccumDrag);

    DirectX::SimpleMath::Vector3 forceSumLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceDragLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceSumAngular = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forceDragAngular = DirectX::SimpleMath::Vector3::Zero;

    forceSumLinear = forceAccum;
    forceDragLinear = forceAccumDrag;
    forceSumLinear = DirectX::SimpleMath::Vector3::Transform(forceSumLinear, aMissile.projectileData.alignmentQuat);
    forceDragLinear = DirectX::SimpleMath::Vector3::Transform(forceDragLinear, aMissile.projectileData.alignmentQuat);

    forceSumAngular = torqueAccum;
    //forceDragAngular = torqueAccumDrag;
    forceDragAngular = aMissile.projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, aTimeDelta);
    //DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);
    //aMissile.projectileData.angularDragSum = angularDrag;

  //  forceSumAngular = DirectX::SimpleMath::Vector3::Transform(forceSumAngular, aMissile.projectileData.alignmentQuat);
  //  forceDragAngular = DirectX::SimpleMath::Vector3::Transform(forceDragAngular, aMissile.projectileData.alignmentQuat);

    DirectX::SimpleMath::Vector3 forceDragAngularTest1 = aMissile.projectileData.q.angularVelocity * -powf(0.3f, aTimeDelta);
    DirectX::SimpleMath::Vector3 forceDragAngularTest2 = aMissile.projectileData.q.angularVelocity * -powf(0.8f, aTimeDelta) * 2.0f;
    //forceDragAngular = forceDragAngularTest2;
    float testVal1 = forceDragAngularTest1.Length();
    float testVal2 = forceDragAngularTest2.Length();
    float deltaVal = testVal2 - testVal1;
    m_debugData->DebugPushUILineDecimalNumber("testVal1 = ", testVal1, "");
    m_debugData->DebugPushUILineDecimalNumber("testVal2 = ", testVal2, "");
    m_debugData->DebugPushUILineDecimalNumber("deltaVal = ", deltaVal, "");
    if (deltaVal < 0.0f)
    {
        int testBreak = 0;
        testBreak++;
    }

    m_debugData->ToggleDebugOff();
    CalculateAirDragTorque(aMissile, aTimeDelta);

    ////// WindVaning start
    DirectX::SimpleMath::Vector3 windVaneTorque = CalculateWindVaningTorqueForce(aMissile);
    //forceSumAngular += windVaneTorque;
    m_debugHeadingVec3 = windVaneTorque;
    m_debugVec1 = forceSumAngular;
    m_debugVec2 = forceDragAngular;
    ////// WindVaning end

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceSumAngular  Pre   = ", forceSumAngular.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("airDragTorqueLocalTest = ", aMissile.guidance.airDragTorqueLocalTest.Length(), "");

    DirectX::SimpleMath::Vector3 testLine = forceSumAngular;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testLine, 8.0f, 0.0f, DirectX::Colors::Blue);

    testLine = aMissile.guidance.airDragTorqueLocalTest;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testLine, 6.0f, 0.0f, DirectX::Colors::DarkOrange);

    forceSumAngular += aMissile.guidance.airDragTorqueLocalTest;

    testLine = forceSumAngular;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testLine, 6.0f, 0.0f, DirectX::Colors::White);
    m_debugData->DebugPushUILineDecimalNumber("forceSumAngular  Post  = ", forceSumAngular.Length(), "");

    m_debugData->ToggleDebugOff();


    DirectX::SimpleMath::Vector3 sumLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 sumDragLinear = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 sumAngular = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 sumDragAngular = DirectX::SimpleMath::Vector3::Zero;
    
    aMissile.guidance.linearDragSum = forceDragLinear;
    aMissile.guidance.linearForceSum = forceSumLinear;
    aMissile.projectileData.angularForceSum = forceSumAngular;
    aMissile.projectileData.angularDragSum = forceDragAngular;

}


void FireControl::AccumulateMissileForces(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        int testBreak = 0;
        testBreak++;
    }

    Utility::ForceAccum sumForce;
    Utility::ForceAccum::ZeroValues(sumForce);
    sumForce += BoosterAccum(aMissile);
    Utility::ForceAccum sumDrag;
    Utility::ForceAccum::ZeroValues(sumDrag);
    sumDrag += DragAccum(aMissile, aTimeDelta);


    m_debugData->ToggleDebugOnOverRide();

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, sumDrag.linear, 8.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->DebugPushUILineDecimalNumber("sumDrag.linear.Length() = ", sumDrag.linear.Length(), "");

    DirectX::SimpleMath::Vector3 thrustPosWorld = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.centerOfThrustLocalPos, aMissile.projectileData.alignmentQuat) + aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 centerOfPressurePosWorld = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.centerOfPressureLocalPos, aMissile.projectileData.alignmentQuat) + aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 centerOfMassWorld = DirectX::SimpleMath::Vector3::Transform(m_missileConsts.centerOfMassLocal, aMissile.projectileData.alignmentQuat) + aMissile.projectileData.q.position;

    //DirectX::SimpleMath::Vector3 boostForcesWorld = DirectX::SimpleMath::Vector3::Transform(sumForce.linear, aMissile.projectileData.alignmentQuat) + aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 boostForcesWorld = DirectX::SimpleMath::Vector3::Transform(sumForce.linear, aMissile.projectileData.alignmentQuat);
    DirectX::SimpleMath::Vector3 dragBoostForceWorld = DirectX::SimpleMath::Vector3::Transform(sumDrag.linear, aMissile.projectileData.alignmentQuat);

    DirectX::SimpleMath::Vector3 boostTorqueWorld = DirectX::SimpleMath::Vector3::Transform(sumForce.torque, aMissile.projectileData.alignmentQuat);
    DirectX::SimpleMath::Vector3 dragBoostTorqueWorld = DirectX::SimpleMath::Vector3::Transform(sumDrag.torque, aMissile.projectileData.alignmentQuat);

    //m_debugData->PushDebugLinePositionIndicatorAligned(thrustPosWorld, 2.0f, 0.1f, aMissile.projectileData.alignmentQuat, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicatorAligned(centerOfPressurePosWorld, 2.0f, 0.0f, aMissile.projectileData.alignmentQuat, DirectX::Colors::Teal);
    //m_debugData->PushDebugLinePositionIndicatorAligned(centerOfMassWorld, 2.0f, 0.2f, aMissile.projectileData.alignmentQuat, DirectX::Colors::White);
    
    m_debugData->PushDebugLinePositionIndicator(thrustPosWorld, 2.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(centerOfPressurePosWorld, 2.0f, 0.0f,  DirectX::Colors::Teal);
    m_debugData->PushDebugLinePositionIndicator(centerOfMassWorld, 2.0f, 0.0f, DirectX::Colors::White);

    m_debugData->PushDebugLine(thrustPosWorld, boostForcesWorld, 3.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->PushDebugLine(centerOfMassWorld, dragBoostForceWorld, 3.0f, 0.0f, DirectX::Colors::Blue);

    //m_debugData->PushDebugLine(centerOfMassWorld, boostTorqueWorld, 3.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(centerOfMassWorld, dragBoostTorqueWorld, 3.0f, 0.0f, DirectX::Colors::LimeGreen);

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.thrustAngle = ", Utility::ToDegrees(aMissile.guidance.thrustAngle), "");

    m_debugData->ToggleDebugOff();

    sumForce.torque += sumDrag.torque;

    Utility::ForceAccum::AlignLinear(sumForce, aMissile.projectileData.alignmentQuat);
    Utility::ForceAccum::AlignLinear(sumDrag, aMissile.projectileData.alignmentQuat);

    aMissile.guidance.linearForceSum = sumForce.linear;
    aMissile.guidance.linearDragSum = sumDrag.linear;
    aMissile.projectileData.angularForceSum = sumForce.torque;
    aMissile.projectileData.angularDragSum = sumDrag.torque;
}

Utility::ForceAccum FireControl::BoosterAccum(MissileData& aMissile)
{
    DirectX::SimpleMath::Vector3 forcePos = aMissile.guidance.centerOfThrustLocalPos;
    DirectX::SimpleMath::Vector3 forceDir = CalculateBoostForceVec(aMissile);
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

   // Utility::ForceAccum::AlignLinear(accum, aMissile.projectileData.alignmentQuat);

    return accum;
}

Utility::ForceAccum FireControl::DragAccum(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 forcePos = aMissile.guidance.centerOfPressureLocalPos;
    DirectX::SimpleMath::Vector3 forceDir = CalculateDragLinearForAccumulator(aMissile);
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);

    /*
    forceAccum = CalculateDragLinearForAccumulator(aMissile);
    torqueAccum = aMissile.projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, aTimeDelta);
    */

    //accum.linear = forceDir;
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    //Utility::ForceAccum::AlignLinear(accum, aMissile.projectileData.alignmentQuat);

    return accum;
}
