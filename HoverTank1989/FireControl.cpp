#include "pch.h"
#include "FireControl.h"


void FireControl::AccumulateMissileForces(MissileData& aMissile, const float aTimeDelta)
{
    Utility::ForceAccum sumDrag;
    Utility::ForceAccum::ZeroValues(sumDrag);
    sumDrag += DragAccum(aMissile, aTimeDelta);

    Utility::ForceAccum sumForce;
    Utility::ForceAccum::ZeroValues(sumForce);

    Utility::ForceAccum boosterForce = BoosterAccum(aMissile);
    if (m_isDebugToggleTrue2 == false)
    {
        sumForce += boosterForce;
    }
    Utility::ForceAccum aeroAcc;
    Utility::ForceAccum::ZeroValues(aeroAcc);
    aeroAcc+= FinAccumSumTest(aMissile);
    
    if (m_missileConsts.isFinForceOn == true)
    {
        sumForce += aeroAcc;
    }

    Utility::ForceAccum bodyAeroAcc;
    Utility::ForceAccum::ZeroValues(bodyAeroAcc);
    bodyAeroAcc += BodyAeroAccum(aMissile);
    if (m_missileConsts.isBodyAeroOn == true)
    {
        sumForce += bodyAeroAcc;
    }

    if (m_missileConsts.isGravityOn == true)
    {
        Utility::ForceAccum gravityAcc;
        Utility::ForceAccum::ZeroValues(gravityAcc);
        auto gravVec = DirectX::SimpleMath::Vector3::UnitY;
        gravVec = DirectX::SimpleMath::Vector3::Transform(gravVec, aMissile.projectileData.inverseAlignmentQuat);
        gravVec *= m_environment->GetGravity() * m_missileConsts.mass;
        DirectX::SimpleMath::Vector3 forcePos = m_missileConsts.centerOfMassLocal;
        DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
        DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
        Utility::AddForceAtPoint(gravVec, forcePos, centerOfMass, forceAccum, torqueAccum);
        gravityAcc.linear = forceAccum;
        gravityAcc.torque = torqueAccum;
        sumForce += gravityAcc;
    }

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("boosterForce.linear = ", boosterForce.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("boosterForce.torque = ", boosterForce.torque.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("aeroAcc.linear = ", aeroAcc.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("aeroAcc.torque = ", aeroAcc.torque.Length(), "");
    m_debugData->ToggleDebugOff();

    Utility::ForceAccum::AlignLinear(sumForce, aMissile.projectileData.alignmentQuat);
    Utility::ForceAccum::AlignLinear(sumDrag, aMissile.projectileData.alignmentQuat);

    aMissile.guidance.linearForceSum = sumForce.linear;
    aMissile.guidance.linearDragSum = sumDrag.linear;
    aMissile.projectileData.angularForceSum = sumForce.torque;
    aMissile.projectileData.angularDragSum = sumDrag.torque;
}

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


Utility::ForceAccum FireControl::BodyAeroAccum(MissileData& aMissile)
{
    auto localAirVel = aMissile.guidance.localVel;
    auto localAirVelNorm = localAirVel;
    localAirVelNorm.Normalize();
    //DirectX::SimpleMath::Vector3 forcePos = m_missileConsts.centerOfPressureBasePosLocal;
    DirectX::SimpleMath::Vector3 forcePos = aMissile.guidance.centerOfPressureLocalPos;
    
    DirectX::SimpleMath::Vector3 forceDir = CalculateDragLinearForAccumulator(aMissile);
    //auto forceVal = forceDir.Length();
    //forceDir = localAirVelNorm * forceVal;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    //DebugPushDrawData(forcePos, accum.linear, DirectX::Colors::Lavender, false, true);
    //DebugPushDrawData(forcePos, accum.torque, DirectX::Colors::Tomato, false, true);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceAccum  = ", forceAccum.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("torqueAccum = ", torqueAccum.Length(), "");
    m_debugData->ToggleDebugOff();

    return accum;
}

Utility::ForceAccum FireControl::BoosterAccum(MissileData& aMissile)
{
    //DebugPushDrawData(aMissile.guidance.centerOfThrustLocalPos, DirectX::SimpleMath::Vector3::UnitY, DirectX::Colors::Red, false, true);

    DirectX::SimpleMath::Vector3 forcePos = aMissile.guidance.centerOfThrustLocalPos;
    DirectX::SimpleMath::Vector3 forceDir = CalculateBoostForceVec(aMissile);
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    //DebugPushDrawData(m_missileConsts.thrustPosLocal, forceDir, DirectX::Colors::Orange, false, true);

    //const float boreThrustPercent = 0.5f;
    //const float turnThrustPercent = 1.0f - boreThrustPercent;
    if (aMissile.guidance.isRocketFired == false)
    {
        forceDir = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
        testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile.guidance.steeringQuat);
        testLine = aMissile.guidance.conDat.thrustVecNorm;
        testLine = aMissile.guidance.nav.vecToTargLocal;

        testLine = aMissile.guidance.nav.thrustVec;
        forceDir = testLine * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage);
        DebugPushDrawData(m_missileConsts.thrustPosLocal, forceDir, DirectX::Colors::Lavender, false, true);
        DebugPushDrawData(m_missileConsts.thrustPosLocal, DirectX::SimpleMath::Vector3::UnitZ, DirectX::Colors::Yellow, false, true);
    }

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, accum.linear, DirectX::Colors::Red, false, true);
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, accum.torque, DirectX::Colors::Blue, false, true);
    
    //DebugPushDrawData(m_missileConsts.thrustPosLocal, accum.linear, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(m_missileConsts.thrustPosLocal, accum.torque, DirectX::Colors::Blue, false, true);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceAccum = ", forceAccum.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("torqueAccum = ", torqueAccum.Length(), "");
    m_debugData->ToggleDebugOff();
    return accum;
}

void FireControl::BoosterSteeringUpdate(MissileData& aMissile)
{
    auto steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    //steeringQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_manualThrustVecYaw, 0.0f, m_manualThrustVecPitch);
    steeringQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(aMissile.guidance.conDat.thrustYaw, 0.0f, aMissile.guidance.conDat.thrustPitch);
    auto steeringVec = DirectX::SimpleMath::Vector3::UnitX;
    steeringVec = DirectX::SimpleMath::Vector3::Transform(steeringVec, steeringQuat);
    steeringVec.Normalize();

    auto boosterQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, steeringVec);
    boosterQuat.Normalize();
    boosterQuat.Inverse(boosterQuat);

    aMissile.guidance.steeringQuat = boosterQuat;
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
        //if (aMissile.guidance.type == MissileType::TYPE_CANARDCONTROL || aMissile.guidance.type == MissileType::TYPE_TAILCONTRTROL)
        if (m_missileConsts.isThrustVecOn == false)
        {
            return DirectX::SimpleMath::Vector3::UnitX * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage);
        }
        else
        {
            auto thrust = DirectX::SimpleMath::Vector3::UnitX;
            thrust = DirectX::SimpleMath::Vector3::Transform(thrust, aMissile.guidance.steeringQuat);
            return thrust * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage);
        }
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


DirectX::SimpleMath::Vector3 FireControl::CalcDragLinearCurrent(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    if (m_isInRunge == false)
    {
        m_debugData->ToggleDebugOnOverRide();
    }

    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = velocityNorm;
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile->projectileData.inverseAlignmentQuat);
     
    float mainAreaPitch = CalculateFinDragArea(localVelocityNorm, aMissile->guidance.finPak.mainPitch.finDir, m_finLib.mainPitch);
    //m_debugData->DebugPushUILineDecimalNumber("mainPitch area = ", mainAreaPitch, "");
    float mainAreaYaw = CalculateFinDragArea(localVelocityNorm, aMissile->guidance.finPak.mainYaw.finDir, m_finLib.mainYaw);
    //m_debugData->DebugPushUILineDecimalNumber("mainYaw = ", mainAreaYaw, "");
    float tailAreaPitch = CalculateFinDragArea(localVelocityNorm, aMissile->guidance.finPak.tailPitch.finDir, m_finLib.tailPitch);
    //m_debugData->DebugPushUILineDecimalNumber("tailPitch area = ", tailAreaPitch, "");
    float tailAreaYaw = CalculateFinDragArea(localVelocityNorm, aMissile->guidance.finPak.tailYaw.finDir, m_finLib.tailYaw);
    //m_debugData->DebugPushUILineDecimalNumber("tailYaw = ", tailAreaYaw, "");

    const float finSurfaceSum = mainAreaPitch + mainAreaYaw + tailAreaPitch + tailAreaYaw;

    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float frontSurface = frontSurfaceArea *  abs(frontDot);
    float sideSurface = sideSurfaceArea * abs(sideDot);
    float topSurface = topSurfaceArea * abs(topDot);

    const float airSurfaceAreaSum = frontSurface + sideSurface + topSurface + finSurfaceSum;

    float sideSlipRatio = 1.0f - abs(localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX));

    float dragCoefficientBase = 0.17f;
    float dragCoefficientToAdd = 1.6f * sideSlipRatio;
    float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;
    dragCoefficient = m_missileConsts.dragCoefficientDebug;
    dragCoefficient += dragCoefficientToAdd;

    //////////////////////////////////////////////

    const float frontDragCoeff = 0.1f;
    const float sideDragCoeff = 1.2f;
    const float topDragCoeff = sideDragCoeff;

    float frontDrag = frontDragCoeff * abs(frontDot);
    float sideDrag = sideDragCoeff * abs(sideDot);
    float topDrag = topDragCoeff * abs(topDot);

    const float dragCoeffSum = frontDrag + sideDrag + topDrag;
    //////////////////////////////////////////////

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float velocity = aMissile->projectileData.q.velocity.Length();
    //float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    float dragResistance = 0.5f * airDensity * airSurfaceAreaSum * dragCoeffSum * velocity * velocity;
    //DirectX::SimpleMath::Vector3 airResistance = localVelocityNorm * (-dragResistance);
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    m_debugData->ToggleDebugOff();
    //DebugPushDrawData(m_missileConsts.tailPosLocal, aMissile->guidance.finPak.tailPitch.finDir);
    //DebugPushDrawData(m_missileConsts.tailPosLocal, aMissile->guidance.finPak.tailYaw.finDir);
    return drag;
}

DirectX::SimpleMath::Vector3 FireControl::CalcDragLinearCurrentOld(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    if (m_isInRunge == false)
    {
        m_debugData->ToggleDebugOnOverRide();
    }

    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = velocityNorm;
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, aMissile->projectileData.inverseAlignmentQuat);

    auto tailPitchDot = aMissile->guidance.finPak.tailPitch.finDir.Dot(localVelocityNorm);
    auto tailYawDot = aMissile->guidance.finPak.tailYaw.finDir.Dot(localVelocityNorm);

    /*
    float finArea = CalculateFinDragArea(localVelocityNorm, aMissile);
    float finAreaPitch = CalculateFinDragArea2(localVelocityNorm, aMissile->guidance.finPak.tailPitch.finDir, m_finLib.tailPitch);
    m_debugData->DebugPushUILineDecimalNumber("tailPitch area = ", finAreaPitch, "");
    float finAreaYaw = CalculateFinDragArea2(localVelocityNorm, aMissile->guidance.finPak.tailYaw.finDir, m_finLib.tailYaw);
    m_debugData->DebugPushUILineDecimalNumber("tailYaw = ", finAreaYaw, "");
    */

    //m_debugData->DebugPushUILineDecimalNumber("tailPitchDot = ", tailPitchDot, "");
    //m_debugData->DebugPushUILineDecimalNumber("tailYawDot   = ", tailYawDot, "");

    const float frontFinSurfaceSum = 2.0f * (m_finLib.tailPitch.frontArea + m_finLib.tailYaw.frontArea + m_finLib.mainPitch.frontArea + m_finLib.mainYaw.frontArea);
    const float sideFinSurfaceSum = 2.0f * (m_finLib.tailYaw.wingArea + m_finLib.mainYaw.wingArea);
    const float topFinSurfaceSum = 2.0f * (m_finLib.tailPitch.wingArea + m_finLib.mainPitch.wingArea);
    //const float frontFaceMod = 0.2f;
    //const float finMod = 0.5f;
    const float frontFaceMod = 0.0f;
    const float finMod = 0.0f;
    //float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z + frontFaceMod;
    //float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y + finMod;
    //float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z + finMod;

    float frontSurfaceArea2 = m_missileDimensions.y * m_missileDimensions.z;
    float sideSurfaceArea2 = m_missileDimensions.x * m_missileDimensions.y;
    float topSurfaceArea2 = m_missileDimensions.x * m_missileDimensions.z;

    float frontSurfaceArea3 = frontFinSurfaceSum;
    float sideSurfaceArea3 = sideFinSurfaceSum;
    float topSurfaceArea3 = topFinSurfaceSum;

    float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z + frontFinSurfaceSum;
    float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y + sideFinSurfaceSum;
    float topSurfaceArea = m_missileDimensions.x * m_missileDimensions.z + topFinSurfaceSum;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float frontSurface = frontSurfaceArea * abs(frontDot);
    float sideSurface = sideSurfaceArea * abs(sideDot);
    float topSurface = topSurfaceArea * abs(topDot);

    float airSurfaceArea = frontSurface + sideSurface + topSurface;
    const float airSurfaceAreaSum = frontSurface + sideSurface + topSurface;
    /*
    m_debugData->DebugPushUILineDecimalNumber("frontSurface   = ", frontSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("sideSurface    = ", sideSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("topSurface     = ", topSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea = ", airSurfaceArea, "");
    */
    const float surfaceSum = airSurfaceArea;
    const float surfacePercentage = surfaceSum / (frontSurfaceArea + sideSurfaceArea + topSurfaceArea);

    ////////////////////////////////////////////////////////////////////////////////////
    // 
    //const float angleOfAttackWrapped = Utility::WrapAngleOnePi(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    const float angleOfAttackWrapped2 = Utility::WrapAngle(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, localVelocityNorm));
    /*
    float sideSlipRatio2 = abs(angleOfAttackWrapped2 / DirectX::XM_PIDIV2);
    if (sideSlipRatio2 > 1.0f || sideSlipRatio2 < 0.0f)
    {
        sideSlipRatio2 = 1.0f;
        int testBreak = 0;
        testBreak++;
    }
    */

    auto sideSlipDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    auto invsRatio = 1.0f - abs(sideSlipDot);
    float sideSlipRatio = invsRatio;

    const float airSurfaceAreaToAdd = (m_missileDimensions.x * m_missileDimensions.y) * sideSlipRatio;
    const float radius = m_missileDimensions.y * 0.5f;
    //airSurfaceArea = Utility::GetPi() * radius * radius;
    //airSurfaceArea += airSurfaceAreaToAdd;

    //airSurfaceArea = m_missileConsts.dragAreaDebug;
    //airSurfaceArea = Utility::GetPi() * radius * radius;
    airSurfaceArea += airSurfaceAreaToAdd;

    float dragCoefficientBase = 0.17f;
    float dragCoefficientToAdd = 1.6f * sideSlipRatio;
    float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;
    dragCoefficient = m_missileConsts.dragCoefficientDebug;
    dragCoefficient += dragCoefficientToAdd;

    //////////////////////////////////////////////

    const float frontDragCoeff = 0.1f;
    const float sideDragCoeff = 1.2f;
    const float topDragCoeff = sideDragCoeff;

    float frontDrag = frontDragCoeff * abs(frontDot);
    float sideDrag = sideDragCoeff * abs(sideDot);
    float topDrag = topDragCoeff * abs(topDot);

    const float dragCoeffSum = frontDrag + sideDrag + topDrag;
    //////////////////////////////////////////////

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float velocity = aMissile->projectileData.q.velocity.Length();
    //float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    float dragResistance = 0.5f * airDensity * airSurfaceAreaSum * dragCoeffSum * velocity * velocity;
    //DirectX::SimpleMath::Vector3 airResistance = localVelocityNorm * (-dragResistance);
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    m_debugData->ToggleDebugOff();

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
    const float airSurfaceArea2 = frontSurface + sideSurface + topSurface;


    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea2 = ", airSurfaceArea2, "");
    m_debugData->DebugPushUILineDecimalNumber("frontDot = ", frontDot, "");
    m_debugData->DebugPushUILineDecimalNumber("sideDot = ", sideDot, "");
    m_debugData->DebugPushUILineDecimalNumber("topDot = ", topDot, "");
    m_debugData->DebugPushUILineDecimalNumber("frontSurface = ", frontSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("sideSurface = ", sideSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("topSurface = ", topSurface, "");

    //m_debugData->ToggleDebugOff();

    airSurfaceArea = 0.785f;

    ////////////////////////////////////////////////////////////////////////////////////

    DirectX::SimpleMath::Vector3 airVelocityLocalized = aMissile.projectileData.q.velocity;
    airVelocityLocalized = DirectX::SimpleMath::Vector3::Transform(airVelocityLocalized, aMissile.projectileData.inverseAlignmentQuat);
    // airVelocityLocalized = -DirectX::SimpleMath::Vector3::UnitX;
    //const float angleOfAttack = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized);
    const float angleOfAttackWrapped = Utility::WrapAngleOnePi(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    const float angleOfAttackWrapped2 = Utility::WrapAngle(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack        deg = ", Utility::ToDegrees(angleOfAttack), "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttackWrapped deg  = ", Utility::ToDegrees(angleOfAttackWrapped), "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttackWrapped2 deg = ", Utility::ToDegrees(angleOfAttackWrapped2), "");
    //float sideSlipRatio = angleOfAttack / DirectX::XM_PIDIV2;
    //float sideSlipRatio = angleOfAttackWrapped / DirectX::XM_PIDIV2;
    float sideSlipRatio = abs(angleOfAttackWrapped / DirectX::XM_PIDIV2);
    //float sideSlipRatio = abs(angleOfAttackWrapped2 / DirectX::XM_PI);
    float sideSlipRatio2 = abs(angleOfAttackWrapped2 / DirectX::XM_PI);
    //float sideSlipRatio =  DirectX::XM_PIDIV2;
    if (sideSlipRatio > 1.0f || sideSlipRatio < 0.0f)
    {
        //sideSlipRatio = 1.0f;
        int testBreak = 0;
        testBreak++;
    }

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("sideSlipRatio = ", sideSlipRatio, "");
    float sideSlipDeg = sideSlipRatio * 90.0f;
    //m_debugData->DebugPushUILineDecimalNumber("sideSlipDeg  = ", sideSlipDeg, "");

    float combinedDot = abs(topDot) * abs(sideDot);
    m_debugData->DebugPushUILineDecimalNumber("combinedDot = ", combinedDot, "");

    m_debugData->ToggleDebugOff();

    float airSurfaceAreaToAdd = (m_missileDimensions.x * m_missileDimensions.y) * sideSlipRatio2;
    float radius = 0.0635f;
    airSurfaceArea = Utility::GetPi() * radius * radius;
    airSurfaceArea = 1.0f;
    airSurfaceArea += airSurfaceAreaToAdd;

    float dragCoefficientBase = 0.17f;
    float dragCoefficientToAdd = 1.6f * sideSlipRatio2;
    //float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;
    float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;

    ////////////////////////////////////////////////////////////////////////////////////

    //airSurfaceArea += airSurfaceAreaToAdd;
    //dragCoefficient += dragCoefficientToAdd;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    //const float dragCoefficient = 0.17f;
    float velocity = aMissile.projectileData.q.velocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);
    DirectX::SimpleMath::Vector3 airResistance = localVelocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    //drag = DirectX::SimpleMath::Vector3::Transform(drag, aMissile.projectileData.inverseAlignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea  = ", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea2 = ", airSurfaceArea2, "");
    
    m_debugData->DebugPushUILineDecimalNumber("airResistance.Length() = ", airResistance.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("dragResistance = ", dragResistance, "");
    m_debugData->DebugPushUILineDecimalNumber("dragCoefficient = ", dragCoefficient, "");
    m_debugData->DebugPushUILineDecimalNumber("drag.Length() = ", drag.Length(), "");
 
    m_debugData->ToggleDebugOff();

    return drag;
}

DirectX::SimpleMath::Vector3 FireControl::CalculateDragLinearForAccumulatorOld(MissileData& aMissile)
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
    const float airSurfaceArea2 = frontSurface + sideSurface + topSurface;


    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea2 = ", airSurfaceArea2, "");
    m_debugData->DebugPushUILineDecimalNumber("frontDot = ", frontDot, "");
    m_debugData->DebugPushUILineDecimalNumber("sideDot = ", sideDot, "");
    m_debugData->DebugPushUILineDecimalNumber("topDot = ", topDot, "");
    m_debugData->DebugPushUILineDecimalNumber("frontSurface = ", frontSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("sideSurface = ", sideSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("topSurface = ", topSurface, "");

    //m_debugData->ToggleDebugOff();

    airSurfaceArea = 0.785f;

    ////////////////////////////////////////////////////////////////////////////////////

    DirectX::SimpleMath::Vector3 airVelocityLocalized = aMissile.projectileData.q.velocity;
    airVelocityLocalized = DirectX::SimpleMath::Vector3::Transform(airVelocityLocalized, aMissile.projectileData.inverseAlignmentQuat);
    // airVelocityLocalized = -DirectX::SimpleMath::Vector3::UnitX;
    //const float angleOfAttack = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized);
    const float angleOfAttackWrapped = Utility::WrapAngleOnePi(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    const float angleOfAttackWrapped2 = Utility::WrapAngle(Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, airVelocityLocalized));
    //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack        deg = ", Utility::ToDegrees(angleOfAttack), "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttackWrapped deg  = ", Utility::ToDegrees(angleOfAttackWrapped), "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttackWrapped2 deg = ", Utility::ToDegrees(angleOfAttackWrapped2), "");
    //float sideSlipRatio = angleOfAttack / DirectX::XM_PIDIV2;
    //float sideSlipRatio = angleOfAttackWrapped / DirectX::XM_PIDIV2;
    float sideSlipRatio = abs(angleOfAttackWrapped / DirectX::XM_PIDIV2);
    //float sideSlipRatio = abs(angleOfAttackWrapped2 / DirectX::XM_PI);
    float sideSlipRatio2 = abs(angleOfAttackWrapped2 / DirectX::XM_PI);
    //float sideSlipRatio =  DirectX::XM_PIDIV2;
    if (sideSlipRatio > 1.0f || sideSlipRatio < 0.0f)
    {
        //sideSlipRatio = 1.0f;
        int testBreak = 0;
        testBreak++;
    }

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("sideSlipRatio = ", sideSlipRatio, "");
    float sideSlipDeg = sideSlipRatio * 90.0f;
    //m_debugData->DebugPushUILineDecimalNumber("sideSlipDeg  = ", sideSlipDeg, "");

    float combinedDot = abs(topDot) * abs(sideDot);
    m_debugData->DebugPushUILineDecimalNumber("combinedDot = ", combinedDot, "");

    m_debugData->ToggleDebugOff();

    float airSurfaceAreaToAdd = (m_missileDimensions.x * m_missileDimensions.y) * sideSlipRatio2;
    float radius = 0.0635f;
    airSurfaceArea = Utility::GetPi() * radius * radius;
    airSurfaceArea += airSurfaceAreaToAdd;

    float dragCoefficientBase = 0.17f;
    float dragCoefficientToAdd = 1.6f * sideSlipRatio2;
    //float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;
    float dragCoefficient = dragCoefficientBase + dragCoefficientToAdd;

    ////////////////////////////////////////////////////////////////////////////////////

    //airSurfaceArea += airSurfaceAreaToAdd;
    //dragCoefficient += dragCoefficientToAdd;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    //const float dragCoefficient = 0.17f;
    float velocity = aMissile.projectileData.q.velocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);
    DirectX::SimpleMath::Vector3 airResistance = localVelocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    //drag = DirectX::SimpleMath::Vector3::Transform(drag, aMissile.projectileData.inverseAlignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea  = ", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea2 = ", airSurfaceArea2, "");
    /*
    m_debugData->DebugPushUILineDecimalNumber("drag.Length() = ", drag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.Length() = ", airResistance.Length(), "");
    /*
    m_debugData->DebugPushUILineDecimalNumber("drag.Length() = ", drag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("drag.Length() = ", drag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("dragResistance = ", dragResistance, "");
    m_debugData->DebugPushUILineDecimalNumber("dragCoefficient = ", dragCoefficient, "");
    m_debugData->DebugPushUILineDecimalNumber("velocity = ", velocity, "");
    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea = ", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("airDensity = ", airDensity, "");
    */
    m_debugData->DebugPushUILineDecimalNumber("aMissile.projectileData.q.velocity.Length() = ", aMissile.projectileData.q.velocity.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("XXXXXXXX = ", 0.0f, "XXXXXXXXX");

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, drag, 11.0f, 0.0f, DirectX::Colors::Teal);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, airResistance, 10.0f, 0.0f, DirectX::Colors::Lavender);
    //m_debugData->DebugPushUILineDecimalNumber(" = ", , "");


    m_debugData->ToggleDebugOff();

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

DirectX::SimpleMath::Vector3 FireControl::CalculateDragLinearSum(MissileData& aMissile, const float aTimeDelta)
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

float FireControl::CalculateFinDragArea(const DirectX::SimpleMath::Vector3 aVelocityNormLocal, const DirectX::SimpleMath::Vector3 aFinDir, const FinDataStatic& aFinDat)
{
    auto finDirDotVel = abs(aFinDir.Dot(aVelocityNormLocal));
    auto invsDot = 1.0f - finDirDotVel;

    float areaFront = aFinDat.frontArea * invsDot;
    float areaWing = aFinDat.wingArea * finDirDotVel;

    
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, aFinDir, DirectX::Colors::RoyalBlue, false, true);
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("areaWing = ", areaWing, "");
    m_debugData->DebugPushUILineDecimalNumber("areaFront = ", areaFront, "");
    m_debugData->DebugPushUILineDecimalNumber("invsDot = ", invsDot, "");
    m_debugData->DebugPushUILineDecimalNumber("finDirDotVel = ", finDirDotVel, "");

    m_debugData->DebugPushUILineDecimalNumber("aFinDat.frontArea    = ", aFinDat.frontArea, "");
    m_debugData->DebugPushUILineDecimalNumber("aFinDat.wingArea     = ", aFinDat.wingArea, "");
    m_debugData->DebugPushUILineDecimalNumber("Sum                  = ", aFinDat.frontArea + aFinDat.wingArea, "");
    m_debugData->DebugPushUILineDecimalNumber("areaFront + areaWing = ", areaFront + areaWing, "");
    m_debugData->ToggleDebugOff();
    

    return areaFront + areaWing;
}

float FireControl::CalculateFinLiftCoef(const float aAngleOfAttack)
{
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;

    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 0.15f;
    const float angKey1 = 5.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.0f;
    const float angKey2 = 10.0f;
    const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));

    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    else if (inputAngle < angKey2)
    {
        const float inputAngMod = inputAngle - angKey1;
        cl = posKey1 - (inputAngMod * deltaKey2);
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}

float FireControl::CalculateFinLiftCoefDebug(const float aAngleOfAttack)
{
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;

    float cl;
    float curveDeltaRate;
    float clTarget;
    const float inAngMax = 90.0f;

    cl = inputAngle / inAngMax;

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}

float FireControl::CalculateFinLiftCoefFlat(const float aAngleOfAttack)
{
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    //const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    const float inputAngleRaw = (Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;

    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 0.15f;
    const float angKey1 = 5.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.0f;
    const float angKey2 = 10.0f;
    //const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));
    const float deltaKey2 = -((posKey2 - posKey0) / (angKey2 - angKey0));

    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    /*
    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    */
    if (inputAngle < angKey2)
    {
        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);

        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);
        cl = inputAngle * -deltaKey2;
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}

float FireControl::CalculateFinLiftWholeBody(const float aAngleOfAttack)
{
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }

    //const float inputAngle = inputAngleMod;
    const float inputAngle = Utility::ToDegrees(aAngleOfAttack);

    const float posKey0 = 0.0f;
    const float angKey0 = -15.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = -0.4f;
    const float angKey1 = -10.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 0.0f;
    const float angKey2 = -5.0f;
    const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));

    const float posKey3 = 1.5f;
    const float angKey3 = 10.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.7f;
    const float angKey4 = 17.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 1.4f;
    const float angKey5 = 25.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    if (inputAngle < angKey1)
    {
        //cl = inputAngle * deltaKey1;
        const float inputAngMod = inputAngle - angKey0;
        //cl = inputAngle * deltaKey1;
        cl = posKey0 - (inputAngMod * deltaKey1);
    }
    else if (inputAngle < angKey2)
    {
        const float inputAngMod = inputAngle - angKey1;
        cl = posKey1 - (inputAngMod * deltaKey2);
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else
    {
        cl = 0.0f;
    }
    /*
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }
    */

    /*
    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }
    */
    clTarget = cl;
    /////////////////////////////////////////////////////////////

    if (aAngleOfAttack <= Utility::ToRadians(-5.0f) || aAngleOfAttack >= Utility::ToRadians(25.0f))
    {
        //clTarget = 0.0f;
    }
    else
    {

    }

    m_debugData->DebugPushUILineDecimalNumber("clTarget = ", clTarget, "");

    m_debugData->ToggleDebugOff();

    return clTarget;
}

float FireControl::CalculateFinLiftWholeBodySymmetric(const float aAngleOfAttack)
{
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;

    const float inputAngle = Utility::ToDegrees(abs(aAngleOfAttack));

    const float posKey0 = 0.0f;
    const float angKey0 = -15.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = -0.4f;
    const float angKey1 = -10.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 0.0f;
    const float angKey2 = -5.0f;
    const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));

    const float posKey3 = 1.5f;
    const float angKey3 = 10.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.7f;
    const float angKey4 = 17.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 1.4f;
    const float angKey5 = 25.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else
    {
        cl = 0.0f;
    }
    m_debugData->DebugPushUILineDecimalNumber("cl = ", cl, "");
    m_debugData->ToggleDebugOff();
    return cl;
}

float FireControl::CalculateFinLiftCoefTest(const float aAngleOfAttack)
{
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("aAngleOfAttack = ", aAngleOfAttack, "");

    //m_debugData->ToggleDebugOff();
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }

    m_debugData->DebugPushUILineWholeNumber("isAoAReversed", isAoAReversed, "");

    //m_debugData->ToggleDebugOff();
    const float inputAngle = inputAngleMod;

    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 1.6f;
    const float angKey1 = 15.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.2f;
    const float angKey2 = 20.0f;
    const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));






    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    else if (inputAngle < angKey2)
    {
        const float inputAngMod = inputAngle - angKey1;
        cl = posKey1 - (inputAngMod * deltaKey2);
    }
    else
    {
        cl = 0.0f;
    }

    /*
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }
    */

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }


    m_debugData->DebugPushUILineDecimalNumber("clTarget = ", clTarget, "");

    m_debugData->ToggleDebugOff();

    return clTarget;
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

void FireControl::CamMissileSelectNext()
{
    if (m_missileVec.size() > 1)
    {
        unsigned int j = 0;
        for (unsigned int i = 0; i < m_missileVec.size(); ++i)
        {
            if (m_missileVec[i].guidance.uniqueId == m_camTrackMissileId)
            {
                j = i;
            }
        }
        if (j >= m_missileVec.size() - 1)
        {
            m_camTrackMissileId = m_missileVec[0].guidance.uniqueId;
        }
        else
        {
            m_camTrackMissileId = m_missileVec[j + 1].guidance.uniqueId;
        }
    }
    else if (m_missileVec.size() > 0)
    {
        m_camTrackMissileId = m_missileVec[0].guidance.uniqueId;
    }
    else
    {
        m_camTrackMissileId = 0;
        // throw error
    }
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
    //m_debugData->DebugPushUILineWholeNumber("targetID    = ", targetID, "");
    //m_debugData->DebugPushUILineWholeNumber("isTargetHit = ", isTargetHit, "");
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
            isHitTrue = m_npcController->CheckProjectileCollisionsMissile(m_missileVec[i].projectileData.collisionData, vehicleHitId, true, 
                m_missileVec[i].guidance.targetID, m_missileConsts.detonationRange, isProximityDetonationTrue);

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

void FireControl::ControllerUpdate(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 dPosLocal;
    DirectX::SimpleMath::Vector3 dPosWorld;
    DirectX::SimpleMath::Vector3 dVelLocal;
    DirectX::SimpleMath::Vector3 dVelWorld;

    aMissile.guidance.controller.deltaQuat = aMissile.guidance.controller.currentQuat - aMissile.guidance.controller.prevQuat;
    aMissile.guidance.controller.deltaVec = aMissile.guidance.controller.currentVec - aMissile.guidance.controller.prevVec;
    aMissile.guidance.controller.deltaControlVal = aMissile.guidance.controller.currentControlVal - aMissile.guidance.controller.prevControlVal;

    DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
    DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;


    aMissile.guidance.controller.prevQuat = aMissile.guidance.controller.currentQuat;
    aMissile.guidance.controller.prevVec = aMissile.guidance.controller.currentVec;
    aMissile.guidance.controller.prevControlVal = aMissile.guidance.controller.currentControlVal;

    aMissile.guidance.controller.deltaQuatConed = aMissile.guidance.controller.currentQuatConed - aMissile.guidance.controller.prevQuatConed;
    aMissile.guidance.controller.deltaVecConed = aMissile.guidance.controller.currentVecConed - aMissile.guidance.controller.prevVecConed;

    aMissile.guidance.controller.deltaQuatRaw = aMissile.guidance.controller.currentQuatRaw - aMissile.guidance.controller.prevQuatRaw;
    aMissile.guidance.controller.deltaVecRaw = aMissile.guidance.controller.currentVecRaw - aMissile.guidance.controller.prevVecRaw;

    aMissile.guidance.controller.prevQuatConed = aMissile.guidance.controller.currentQuatConed;
    aMissile.guidance.controller.prevVecConed = aMissile.guidance.controller.currentVecConed;
    aMissile.guidance.controller.prevQuatRaw = aMissile.guidance.controller.currentQuatRaw;
    aMissile.guidance.controller.prevVecRaw = aMissile.guidance.controller.currentVecRaw;


    /////////////////
    // Instantanious angular offset

    DirectX::SimpleMath::Quaternion instQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();

    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = aMissile.projectileData.q.angularVelocity.x;
    updateAlignQuat.y = aMissile.projectileData.q.angularVelocity.y;
    updateAlignQuat.z = aMissile.projectileData.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    //DirectX::SimpleMath::Quaternion updateAlignOutputQuat = aMissile.projectileData.alignmentQuat;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = DirectX::SimpleMath::Quaternion::Identity;
    updateAlignOutputQuat.Normalize();

    //updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * aMissile.projectileData.alignmentQuat);
    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * DirectX::SimpleMath::Quaternion::Identity);
    updateAlignOutputQuat.Normalize();

    aMissile.guidance.angularStepQuat = updateAlignOutputQuat;
    //
    //////////////////

    //m_debugData->ToggleDebugOnOverRide();

    //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.controller.deltaVecRaw = ", aMissile.guidance.controller.deltaVecRaw.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.controller.deltaQuatRaw = ", aMissile.guidance.controller.deltaQuatRaw.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("", , "");

    m_debugData->ToggleDebugOff();
    DirectX::SimpleMath::Vector3 updateVec = aMissile.guidance.headingLocalVecTest;
    updateVec = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion updateQuat = aMissile.guidance.headingLocalQuatTest;
    updateQuat = DirectX::SimpleMath::Quaternion::Identity;
    const float t = 0.5f;
    //updateQuat = DirectX::SimpleMath::Quaternion::Slerp(aMissile.guidance.headingLocalQuatTest, -aMissile.guidance.controller.deltaQuatRaw, t);
    updateQuat = DirectX::SimpleMath::Quaternion::Slerp(aMissile.guidance.headingLocalQuatTest, -updateAlignOutputQuat, t);
    updateQuat.Normalize();
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, updateQuat);
    updateVec.Normalize();

    if (aMissile.guidance.type == MissileType::TYPE_ORANGE)
    {
        //aMissile.guidance.headingLocalVecTest = updateVec;
        //aMissile.guidance.headingLocalQuatTest = updateQuat;
    }

    if (m_isDebugAngularStabilityOn == true)
    {
        aMissile.guidance.headingLocalVecTest = updateVec;
        aMissile.guidance.headingLocalQuatTest = updateQuat;
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

void FireControl::CruiseGuidance(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::XMVECTORF32 color = { 1.0f, 1.0f, 1.f, 1.f };
    if (aMissile.guidance.type == MissileType::TYPE_BLUE)
    {
        color = DirectX::Colors::Blue;
    }
    else if (aMissile.guidance.type == MissileType::TYPE_ORANGE)
    {
        color = DirectX::Colors::Orange;
    }

    DirectX::SimpleMath::Vector3 prevHeadingLocalVecTest = aMissile.guidance.headingLocalVecTest;
    DirectX::SimpleMath::Quaternion prevHeadingLocalQuatTest = aMissile.guidance.headingLocalQuatTest;

    ////////////////////////////////
    // pos offset
    float timeToTarget = aMissile.guidance.targetDistance / aMissile.guidance.closureRate;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("timeToTarget = ", timeToTarget, "");
    m_debugData->ToggleDebugOff();
    float offSetMod = 0.1f;
    if (timeToTarget < 0.0f)
    {
        offSetMod = 0.0f;
    }
    else if (timeToTarget < 10.0f)
    {
        offSetMod = 0.3f;
    }
    else if (timeToTarget < 20.0f)
    {
        offSetMod = 0.2f;
    }

    float timeToTargetScaled = timeToTarget;
    if (timeToTargetScaled < 0.0f)
    {
        timeToTargetScaled = 0.0f;
    }
    else if (timeToTargetScaled > 20.0f)
    {
        timeToTargetScaled = 20.0f;
    }

    timeToTargetScaled *= 0.5f;

    auto targOffset = aMissile.guidance.targetVelocity * timeToTargetScaled;
    auto missileOffset = aMissile.projectileData.q.velocity * timeToTargetScaled;

    DirectX::SimpleMath::Vector3 targPosOffset = aMissile.guidance.targetVelocity * timeToTarget * offSetMod;
    //DirectX::SimpleMath::Vector3 missilePosOffset = aMissile.projectileData.q.velocity * abs(timeToTarget) * offSetMod;
    DirectX::SimpleMath::Vector3 missilePosOffset = aMissile.projectileData.q.velocity * timeToTarget * offSetMod;
    DirectX::SimpleMath::Vector3 missileVelocityStep = aMissile.projectileData.q.velocity * aTimeDelta;
    //
    ///////////////////////////////

    DirectX::SimpleMath::Vector3 targetDestinationRaw = aMissile.guidance.targetDestination;
    DirectX::SimpleMath::Vector3 positionRaw = aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;

    //destLocal = (aMissile.guidance.targetDestination - missilePosOffset) - (aMissile.projectileData.q.position);
    destLocal = (aMissile.guidance.targetDestination) - (aMissile.projectileData.q.position);
    destLocal = (aMissile.guidance.targetDestination + targOffset) - (aMissile.projectileData.q.position);

    float distToTargetTest = destLocal.Length();
    destLocal.Normalize();
    destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);

    bool isHeadingToDestTrue = false;
    if (destLocal.x >= 0.0f)
    {
        isHeadingToDestTrue = true;
    }

    aMissile.guidance.isFacingDestTrue = isHeadingToDestTrue;

    if (aMissile.guidance.type == MissileType::TYPE_ORANGE)
    {
        //destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, -aMissile.guidance.angularStepQuat);
    }

    DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    //destLocal = DirectX::SimpleMath::Vector3::UnitX;
    //destQuat = DirectX::SimpleMath::Quaternion::Identity;
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;

    InputNavData(aMissile, destQuat, destLocal, destLocal);
    DirectX::SimpleMath::Vector3 destWorld = destLocal;
    destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);
    //destWorld += aMissile.projectileData.q.position;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLinePositionIndicator(destWorld, 10.0f, 0.0f, color);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, destWorld, 3.0f, 0.0f, DirectX::Colors::Orange);

    m_debugData->DebugPushUILineDecimalNumber("destLocal.x = ", destLocal.x, "");
    m_debugData->DebugPushUILineDecimalNumber("destLocal.y = ", destLocal.y, "");
    m_debugData->DebugPushUILineDecimalNumber("destLocal.z = ", destLocal.z, "");
    m_debugData->ToggleDebugOff();

    /*
    /////////////////////////////////
    // test slerp
    //aMissile.guidance.headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
    //aMissile.guidance.headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;
    float slerpT = 0.5f;
    float angularQuatMag = aMissile.projectileData.q.angularVelocity.Length() * aTimeDelta;
    DirectX::SimpleMath::Quaternion angularQuat = DirectX::SimpleMath::Quaternion::Identity;
    if (aMissile.projectileData.q.angularVelocity.Length() > 0.01f)
    {
        angularQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(aMissile.projectileData.q.angularVelocity, -angularQuatMag);
    }
    DirectX::SimpleMath::Quaternion slerpQuat = DirectX::SimpleMath::Quaternion::Slerp(prevHeadingLocalQuatTest, destQuat, slerpT);
    DirectX::SimpleMath::Quaternion angSlerpQuat = DirectX::SimpleMath::Quaternion::Slerp(slerpQuat, angularQuat, 0.1f);
    slerpQuat = slerpQuat;
    DirectX::SimpleMath::Vector3 slerpedVec = DirectX::SimpleMath::Vector3::UnitX;
    slerpedVec = DirectX::SimpleMath::Vector3::Transform(slerpedVec, slerpQuat);
    */
}

void FireControl::CycleControlInputType()
{
    if (m_currentControlType == ControlInputType::INPUT_THRUSTVEC)
    {
        m_currentControlType = ControlInputType::INPUT_TAILFIN;
    }
    else if (m_currentControlType == ControlInputType::INPUT_TAILFIN)
    {
        m_currentControlType = ControlInputType::INPUT_THRUSTVEC;
    }
    else
    {
        m_currentControlType = ControlInputType::INPUT_TAILFIN;
    }
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

void FireControl::DebugGraphCurveData(MissileData& aMissile, const float aTimeDelta)
{
    std::vector<std::pair<float, float>> graph;

    std::vector<float> gAng;
    gAng.clear();
    std::vector<float> gCl;
    gCl.clear();
    //const float gap = Utility::ToRadians(1.0f);
    //const float start = Utility::ToRadians(0.0f);
    //const float end = Utility::ToRadians(180.0f);

    const float gap = Utility::ToRadians(1.0f);
    const float start = Utility::ToRadians(-90.0f);
    const float end = Utility::ToRadians(90.0f);

    //const float gap = Utility::ToRadians(4.0f);
    //const float start = Utility::ToRadians(-360.0f);
    //const float end = Utility::ToRadians(360.0f);

    float runfloat = (end - start) / gap;
    //const int max = static_cast<int>(runfloat);
    const int max = 180;
    float currentAng = start;
    for (int i = 0; i < max; ++i)
    {

        float testAng = currentAng;
        testAng = Utility::WrapAngle(testAng);
        //testAng = Utility::WrapAngleOnePi(testAng);


        //const float cl = CalculateFinLiftCoef(testAng);
        //const float cl = CalculateFinLiftCoefFlat(currentAng);
        //const float cl = CalculateFinLiftCoefFlatOld(currentAng);
        //const float cl = CalculateFinLiftCoefFlatOld2(testAng);
        //const float cl = CalculateFinLiftWholeBody(currentAng);
        //const float cl = CalculateFinLiftWholeBodySymmetric(currentAng);
        //const float cl = CalculateFinLiftCoefDebug(currentAng);
        
        //const float cl = CalculateFinLiftCoef(testAng);
        const float cl = CalculateFinLiftCoefUpdate(currentAng);
        //const float cl = CalculateFinLiftCoefFlat(currentAng);

        std::pair<float, float> dataPoint;
        dataPoint.first = Utility::ToDegrees(currentAng);
        dataPoint.second = cl;
        graph.push_back(dataPoint);

        //gAng.push_back(static_cast<int>((Utility::ToDegrees(currentAng))));
        gAng.push_back(Utility::ToDegrees(currentAng));
        //gAng.push_back(Utility::ToDegrees(testAng));
        gCl.push_back(cl);

        currentAng += gap;
        const int angInt = static_cast<int>(currentAng);
        //currentAng = static_cast<float>(angInt);
    }

    int testBreak = 0;
    testBreak++;
}

void FireControl::DebugIntputValUpdate(const float aInput)
{
    const float updatedTurretYaw = (aInput * m_debugValDeltaRate) + m_debugVal1;
    if (updatedTurretYaw > m_debugValMax)
    {
        m_debugVal1 = m_debugValMax;
    }
    else if (updatedTurretYaw < m_debugValMin)
    {
        m_debugVal1 = m_debugValMin;
    }

    else
    {
        m_debugVal1 = updatedTurretYaw;
    }
}

void FireControl::DebugIntputValUpdateStatic(const bool aIsTrue)
{
    if (aIsTrue == true)
    {
        m_debugVal1 = m_testDebugAng;
    }
    else
    {
        m_debugVal1 = -m_testDebugAng;
    }
}

void FireControl::DebugInputZero()
{
    m_debugVal1 = 0.0f;
}

void FireControl::DebugMissileFunc(MissileData* aMissile)
{
    auto dragForce = CalcDragLinearCurrent(aMissile, aMissile->projectileData.q.velocity);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("dragForce.Length() ", dragForce.Length(), "");
    m_debugData->PushDebugLine(aMissile->projectileData.q.position, dragForce, 5.0f, 0.0f, DirectX::Colors::DarkSeaGreen);
    m_debugData->ToggleDebugOff();
}

void FireControl::DebugSetMissileToPlayerPos(MissileData& aMissile)
{
    if (m_isDebugMissilePosToTankTrue == true)
    {
        aMissile.projectileData.q.position = m_playerVehicle->GetPos();
        aMissile.projectileData.q.velocity = m_playerVehicle->GetVelocity();
        aMissile.projectileData.q.angularVelocity = m_playerVehicle->GetAngularVelocity();

        auto weaponQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_playerVehicle->GetTurretYaw(), 0.0f, m_playerVehicle->GetWeaponPitch());
        aMissile.projectileData.alignmentQuat = weaponQuat;
        aMissile.projectileData.alignmentQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
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
            DirectX::SimpleMath::Matrix rotMatYaw = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.up,
                yawVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            DirectX::SimpleMath::Matrix rotMatPitch = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.right,
                pitchVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
      
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

    return accum;
}

void FireControl::DrawContrails(MissileData& aMissile)
{
    m_debugData->ToggleDebugOnOverRide();
    unsigned int itt = aMissile.contrails.iterator;
    unsigned int nextItt = itt + 1;
    for (unsigned int i = 0; i < m_ammoMissile.modelData.contrailDrawCountMax; ++i)
    {
        if (itt >= m_ammoMissile.modelData.contrailDrawCountMax)
        {
            itt = 0;
        }
        if (nextItt >= m_ammoMissile.modelData.contrailDrawCountMax)
        {
            nextItt = 0;
        }

        float ratio = static_cast<float>(i) / static_cast<float>(m_ammoMissile.modelData.contrailDrawCountMax);
        auto conColor = DirectX::Colors::White;
        // To prevent the contrails color going all the way to black, this cuts off color change at set value
        if (ratio >= m_ammoMissile.modelData.contrailColorMax)
        {
            conColor.f[0] = m_ammoMissile.modelData.contrailColorMax;
            conColor.f[1] = m_ammoMissile.modelData.contrailColorMax;
            conColor.f[2] = m_ammoMissile.modelData.contrailColorMax;
            conColor.f[3] = ratio;
        }
        else if (ratio >= m_ammoMissile.modelData.contrailColorMin)
        {
            conColor.f[0] = ratio;
            conColor.f[1] = ratio;
            conColor.f[2] = ratio;
            conColor.f[3] = ratio;
        }
        else
        {
            conColor.f[0] = m_ammoMissile.modelData.contrailColorMin;
            conColor.f[1] = m_ammoMissile.modelData.contrailColorMin;
            conColor.f[2] = m_ammoMissile.modelData.contrailColorMin;
            conColor.f[3] = ratio;
        }

        if (i == 0)
        {
            // do nothing
        }
        else
        {
            m_debugData->PushTestDebugBetweenPoints(aMissile.contrails.starboard.posVec[itt], aMissile.contrails.starboard.posVec[nextItt], conColor);
            m_debugData->PushTestDebugBetweenPoints(aMissile.contrails.bottom.posVec[itt], aMissile.contrails.bottom.posVec[nextItt], conColor);
            m_debugData->PushTestDebugBetweenPoints(aMissile.contrails.port.posVec[itt], aMissile.contrails.port.posVec[nextItt], conColor);
            m_debugData->PushTestDebugBetweenPoints(aMissile.contrails.top.posVec[itt], aMissile.contrails.top.posVec[nextItt], conColor);
        }
        ++itt;
        ++nextItt;
    }
    m_debugData->ToggleDebugOff();
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


                DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(
                    DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].projectileData.q.position - m_missileVec[i].guidance.targetPosition,
                    DirectX::SimpleMath::Vector3::UnitY);

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
        DirectX::SimpleMath::Vector4 finColor = m_ammoMissile.modelData.finColor2;
        if (m_missileVec[i].guidance.type == MissileType::TYPE_BLUE)
        {
            finColor = DirectX::Colors::Blue;
        }
        else if (m_missileVec[i].guidance.type == MissileType::TYPE_ORANGE)
        {
            finColor = DirectX::Colors::Orange;
        }

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
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle1);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finPak.tailYaw.finAngle);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);

        // tail fin 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle2);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finPak.tailYaw.finAngle);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, finColor);

        // tail fin 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle3);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finPak.tailPitch.finAngle);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, finColor);
        
        // tail fin 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localTailFinMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle4);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finPak.tailPitch.finAngle);
        updateMat *= m_ammoMissile.modelData.tailFinTransMat;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(270.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, finColor);

        // canard fins
        if (m_missileVec[i].guidance.type == MissileType::TYPE_CANARDCONTROL)
        {
            // 1
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat *= m_ammoMissile.modelData.localCanardFinMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
            //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finAngle1);
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finPak.canardYaw.finAngle);
            updateMat *= m_ammoMissile.modelData.canardFinTransMat;
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);

            // 2
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat *= m_ammoMissile.modelData.localCanardFinMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
            //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finAngle2);
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finPak.canardYaw.finAngle);
            updateMat *= m_ammoMissile.modelData.canardFinTransMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);

            // 3
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat *= m_ammoMissile.modelData.localCanardFinMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
            //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finAngle3);
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finPak.canardPitch.finAngle);
            updateMat *= m_ammoMissile.modelData.canardFinTransMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);

            // 4
            updateMat = DirectX::SimpleMath::Matrix::Identity;
            updateMat *= m_ammoMissile.modelData.localCanardFinMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
            //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finAngle4);
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.finPak.canardPitch.finAngle);
            updateMat *= m_ammoMissile.modelData.canardFinTransMat;
            updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(270.0f));
            updateMat *= alignRotMat;
            updateMat *= posTransMat;
            m_ammoMissile.modelData.tailFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        }

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
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle1);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.conDat.thrustYaw);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans1;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // thrust vector 2
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle2);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.conDat.thrustYaw);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans2;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // thrust vector 3
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle3);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.conDat.thrustPitch);
        updateMat *= m_ammoMissile.modelData.thrustVectorTrans3;
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.thrustVectorMainShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
        // thrust vector 4
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localThrustVectorMainMatrix;
        //updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(m_missileVec[i].guidance.finAngle4);
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileVec[i].guidance.conDat.thrustPitch);
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
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, finColor);
        // main wing lower
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, finColor);
        // main wing port
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, finColor);
        // main wing upper
        updateMat = DirectX::SimpleMath::Matrix::Identity;
        updateMat *= m_ammoMissile.modelData.localWingFinMatrix;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle + Utility::ToRadians(180.0f));
        updateMat *= m_ammoMissile.modelData.wingTranslation;
        updateMat *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
        updateMat *= alignRotMat;
        updateMat *= posTransMat;
        m_ammoMissile.modelData.wingFinShape->Draw(updateMat, aView, aProj, m_ammoMissile.modelData.finColor1);
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

Utility::ForceAccum FireControl::FinAccumSumTest(const MissileData& aMissile)
{
    Utility::ForceAccum mainPitchAccum;
    Utility::ForceAccum mainYawAccum;
    mainPitchAccum = FinForceAccum(m_finLib.mainPitch, aMissile.guidance.finPak.mainPitch, aMissile);
    mainYawAccum = FinForceAccum(m_finLib.mainYaw, aMissile.guidance.finPak.mainYaw, aMissile);
    Utility::ForceAccum mainSum;
    Utility::ForceAccum::ZeroValues(mainSum);
    mainSum += mainPitchAccum;
    mainSum += mainYawAccum;
    
    Utility::ForceAccum tailPitchAccum;
    Utility::ForceAccum tailYawAccum;

    tailPitchAccum = FinForceAccum(m_finLib.tailPitch, aMissile.guidance.finPak.tailPitch, aMissile);
    tailYawAccum = FinForceAccum(m_finLib.tailYaw, aMissile.guidance.finPak.tailYaw, aMissile);

    Utility::ForceAccum tailSum;
    Utility::ForceAccum::ZeroValues(tailSum);
    tailSum += tailPitchAccum;
    tailSum += tailYawAccum;

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum += mainSum;
    accum += tailSum;

    return accum;
}

Utility::ForceAccum FireControl::FinAccumSum(const MissileData& aMissile)
{
    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);

    //accum += FinForceAccum(m_finLib.canardPitch, aMissile.guidance.finPak.canardPitch, aMissile);
    //accum += FinForceAccum(m_finLib.canardYaw, aMissile.guidance.finPak.canardYaw, aMissile);

    Utility::ForceAccum mainPitchAccum;
    Utility::ForceAccum::ZeroValues(mainPitchAccum);
    Utility::ForceAccum mainYawAccum;
    Utility::ForceAccum::ZeroValues(mainYawAccum);

    //mainPitchAccum += FinForceAccum(m_finLib.mainPitch, aMissile.guidance.finPak.mainPitch, aMissile);
    //mainYawAccum += FinForceAccum(m_finLib.mainYaw, aMissile.guidance.finPak.mainYaw, aMissile);

    accum += mainPitchAccum;
    accum += mainYawAccum;

    Utility::ForceAccum tailPitchAccum;
    Utility::ForceAccum::ZeroValues(tailPitchAccum);
    Utility::ForceAccum tailYawAccum;
    Utility::ForceAccum::ZeroValues(tailYawAccum);

    tailPitchAccum += FinForceAccum(m_finLib.tailPitch, aMissile.guidance.finPak.tailPitch, aMissile);
    tailYawAccum += FinForceAccum(m_finLib.tailYaw, aMissile.guidance.finPak.tailYaw, aMissile);

    Utility::ForceAccum tailSum;
    Utility::ForceAccum::ZeroValues(tailSum);
    tailSum += tailPitchAccum;
    tailSum += tailYawAccum;

    accum += tailPitchAccum;
    accum += tailYawAccum;

    Utility::ForceAccum::ZeroValues(accum);
    accum += tailYawAccum;
    //Utility::ForceAccum::AlignLinearAndTorque(mainPitchAccum, aMissile.projectileData.alignmentQuat);
    //Utility::ForceAccum::AlignLinearAndTorque(mainYawAccum, aMissile.projectileData.alignmentQuat);

    Utility::ForceAccum mainSum;
    Utility::ForceAccum::ZeroValues(mainSum);
    mainSum += mainPitchAccum;
    mainSum += mainYawAccum;

    Utility::ForceAccum sum;
    Utility::ForceAccum::ZeroValues(sum);
    sum += accum;

    Utility::ForceAccum::AlignLinearAndTorque(sum, aMissile.projectileData.alignmentQuat);
    Utility::ForceAccum::AlignLinearAndTorque(mainSum, aMissile.projectileData.alignmentQuat);
    Utility::ForceAccum::AlignLinearAndTorque(tailSum, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    //m_debugData->DebugPushUILineDecimalNumber("accum.linear = ", accum.linear.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("Last accum.torque = ", accum.torque.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("sum.linear = ", sum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("sum.torque = ", sum.torque.Length(), "");

    //m_debugData->DebugPushUILineDecimalNumber("mainSum.linear = ", mainSum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("mainSum.torque = ", mainSum.torque.Length(), "");

    //m_debugData->DebugPushUILineDecimalNumber("tailSum.linear = ", tailSum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("tailSum.torque = ", tailSum.torque.Length(), "");

    //m_debugData->PushDebugLineScaled(aMissile.projectileData.q.position, mainPitchAccum.linear, 0.1f, 0.0f, 0.0f, DirectX::Colors::Lavender);
    //m_debugData->PushDebugLineScaled(aMissile.projectileData.q.position, mainYawAccum.linear, 0.1f, 0.0f, 0.0f, DirectX::Colors::Blue);
   // m_debugData->PushDebugLine(aMissile.projectileData.q.position, accum.linear, 1.5f, 0.0f, DirectX::Colors::Lime);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, accum.torque, 1.5f, 0.0f, DirectX::Colors::Teal);


    //  m_debugData->PushDebugLine(aMissile.projectileData.q.position, mainSum.linear, 1.3f, 0.05f, DirectX::Colors::White);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, mainSum.torque, 1.3f, 0.05f, DirectX::Colors::Red);

    //  m_debugData->PushDebugLine(aMissile.projectileData.q.position, tailSum.linear, 1.0f, 0.1f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, tailSum.torque, 1.0f, 0.1f, DirectX::Colors::Blue);

    m_debugData->ToggleDebugOff();

    return accum;
}

Utility::ForceAccum FireControl::FinForceAccum(const FinDataStatic& aFinLib, const FinDataDynamic& aFinDyn, const MissileData& aMissile)
{
    auto liftForce = aFinDyn.liftForce;
    auto dragForce = aFinDyn.dragForce;

    DirectX::SimpleMath::Vector3 forcePos = aFinLib.posLocal;
    DirectX::SimpleMath::Vector3 forceDir = aFinDyn.resultantForce;
    //DirectX::SimpleMath::Vector3 forceDir = dragForce;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    if (aFinLib.finType == FinType::TAIL_YAW && 1 == 0)
    {
        //DebugPushDrawData(forcePos, forceDir, DirectX::Colors::Red, false, true);
        DebugPushDrawData(forcePos, aFinDyn.resultantForce, DirectX::Colors::Red, false, true);
        DebugPushDrawData(forcePos, aFinDyn.liftForce, DirectX::Colors::LightBlue, false, true);
        DebugPushDrawData(forcePos, aFinDyn.dragForce, DirectX::Colors::DeepPink, false, true);
    }

    return accum;
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

    if (firedMissile.guidance.uniqueId % 2 == 0)
    {
        //firedMissile.guidance.type = MissileType::TYPE_BLUE;
    }
    else
    {
        //firedMissile.guidance.type = MissileType::TYPE_ORANGE;
    }

    firedMissile.guidance.type = m_currantMissileType;

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
    m_npcController->UpdateMissleGuidance(m_currentTargetID, firedMissile.guidance.targetPosition, firedMissile.guidance.targetVelocity, firedMissile.guidance.targetForward);
    firedMissile.guidance.targetDistance = (aLaunchPos - firedMissile.guidance.targetPosition).Length();

    InitializeContrails(firedMissile);
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
                if (m_selectMissileFire == 0)
                {
                    //FireMissile(m_playerVehicle->GetWeaponPos(), m_playerVehicle->GetWeaponDirection(), aLauncherVelocity, m_playerVehicle->GetVehicleUp(), 0.0f);
                    FireMissile(m_playerVehicle->GetWeaponPos(), aLaunchDirectionForward, DirectX::SimpleMath::Vector3::Zero, m_playerVehicle->GetVehicleUp(), 0.0f);
                    //FireMissile(m_playerVehicle->GetWeaponPos(), m_playerVehicle->GetWeaponDirection(), DirectX::SimpleMath::Vector3::Zero, m_playerVehicle->GetVehicleUp(), 0.0f);
                }
                else if (m_selectMissileFire == 1)
                {
                    DirectX::SimpleMath::Vector3 launchDir = m_playerVehicle->GetWeaponDirection();
                    DirectX::SimpleMath::Vector3 launchDirUp = m_playerVehicle->GetVehicleUp();      
                    DirectX::SimpleMath::Vector3 launchDirRight = launchDirUp.Cross(launchDir);
                    const float missileTubeVerticalRot = Utility::ToRadians(-40.0f);
                    launchDir = DirectX::SimpleMath::Vector3::Transform(launchDir, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(launchDirRight, missileTubeVerticalRot));
                    launchDirUp = DirectX::SimpleMath::Vector3::Transform(launchDirUp, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(launchDirRight, missileTubeVerticalRot));
                    FireMissile(m_playerVehicle->GetWeaponPos(), launchDir, aLauncherVelocity, launchDirUp, 0.0f);
                    //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetMissleTubeDirLeft(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpLeft(), 0.0f);
                }
                else if (m_selectMissileFire == 2)
                {
                    DirectX::SimpleMath::Vector3 launchDir = m_playerVehicle->GetWeaponDirection();
                    DirectX::SimpleMath::Vector3 launchDirUp = m_playerVehicle->GetVehicleUp();
                    DirectX::SimpleMath::Vector3 launchDirRight = launchDirUp.Cross(launchDir);
                    //const float missileTubeVerticalRot = Utility::ToRadians(-40.0f);
                    const float missileTubeVerticalRot = -m_playerVehicle->GetWeaponPitch();
                    launchDir = DirectX::SimpleMath::Vector3::Transform(launchDir, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(launchDirRight, missileTubeVerticalRot));
                    launchDirUp = DirectX::SimpleMath::Vector3::Transform(launchDirUp, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(launchDirRight, missileTubeVerticalRot));
                    FireMissile(m_playerVehicle->GetWeaponPos(), launchDir, aLauncherVelocity, launchDirUp, 0.0f);

                    //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetWeaponDirection(), aLauncherVelocity, m_playerVehicle->GetVehicleUp(), 0.0f);
                    //FireMissile(m_playerVehicle->GetMissleTubePosRight(), m_playerVehicle->GetWeaponDirection(), aLauncherVelocity, m_playerVehicle->GetVehicleUp(), 0.0f);
                }
                else if (m_selectMissileFire == 3)
                {
                    //FireMissile(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetMissleTubeDirLeft(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpLeft(), 0.0f);
                    const float fireTimeOffset = 0.0f;
                    FireMissile(m_playerVehicle->GetMissleTubePosRight(), m_playerVehicle->GetMissleTubeDirRight(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpRight(), fireTimeOffset);
                }

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

void FireControl::GetCameraMissileData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget) const
{
    if (m_missileVec.size() > 0)
    {
        // create list of valid missile IDs
        std::vector<unsigned int> validIds;
        unsigned int idIndex = 0;
        bool isIdFound = false;
        for (unsigned int i = 0; i < m_missileVec.size(); ++i)
        {
            if (m_missileVec[i].projectileData.isDeleteTrue == false && m_missileVec[i].guidance.uniqueId == m_camTrackMissileId)
            {
                idIndex = i;
                isIdFound = true;
                validIds.push_back(m_missileVec[i].guidance.uniqueId);
            }
        }
        
        unsigned int missileId = 0;
        if (isIdFound == true)
        {
            missileId = idIndex;
        }
        /*
        if (m_missileVec.size() >= 2)
        {
            missileId = 1;
        }
        */
        
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

void FireControl::GetCameraTrackAllData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget, DirectX::SimpleMath::Vector3& aUp) const
{
    if (m_missileVec.size() > 0)
    {
        DirectX::SimpleMath::Vector3 targetPos = m_npcController->GetNpcPos(m_currentTargetID);
        DirectX::SimpleMath::Vector3 avgMissilePos = DirectX::SimpleMath::Vector3::Zero;
        int avgCount = 0;
        float maxDistance = 0.0f;
        
        for (unsigned int i = 0; i < m_missileVec.size(); ++i)
        {
            if (m_missileVec[i].projectileData.isDeleteTrue == false)
            {
                avgMissilePos += m_missileVec[i].projectileData.q.position;
                ++avgCount;
                for (unsigned int j = 0; j < m_missileVec.size(); ++j)
                {
                    if (j != i)
                    {
                        float distance = DirectX::SimpleMath::Vector3::Distance(m_missileVec[i].projectileData.q.position, m_missileVec[j].projectileData.q.position);
                        if (distance > maxDistance)
                        {
                            maxDistance = distance;
                        }
                    }
                }
            }
        }

        const float minDistance = 1.0f;
        if (maxDistance < minDistance)
        {
            maxDistance = minDistance;
        }

        if (avgCount <= 0)
        {
            aAlignment = DirectX::SimpleMath::Quaternion::Identity;
            aPos = DirectX::SimpleMath::Vector3::Zero;
            aTarget = DirectX::SimpleMath::Vector3::UnitX;
            aUp = DirectX::SimpleMath::Vector3::UnitY;
        }
        else
        {
            avgMissilePos /= static_cast<float>(avgCount);

            DirectX::SimpleMath::Vector3 targToPosNorm = targetPos - avgMissilePos;
            targToPosNorm.Normalize();
            DirectX::SimpleMath::Vector3 toAddVec = targToPosNorm * maxDistance;
            avgMissilePos -= toAddVec;

            DirectX::SimpleMath::Quaternion missileAlignment = DirectX::SimpleMath::Quaternion::Identity;
            DirectX::SimpleMath::Vector3 missilePos = DirectX::SimpleMath::Vector3::Zero;

            aAlignment = missileAlignment;
            aPos = avgMissilePos;
            aTarget = targetPos;

            DirectX::SimpleMath::Vector3 targToPosNormRight = targToPosNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
            aUp = targToPosNorm.Cross(-targToPosNorm.Cross(DirectX::SimpleMath::Vector3::UnitY));
        }
    }
    else
    {
        aAlignment = DirectX::SimpleMath::Quaternion::Identity;
        aPos = DirectX::SimpleMath::Vector3::Zero;
        aTarget = DirectX::SimpleMath::Vector3::UnitX;
        aUp = DirectX::SimpleMath::Vector3::UnitY;
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

void FireControl::GetUIData(DirectX::SimpleMath::Vector3& aPosRaw, DirectX::SimpleMath::Vector3& aPosMod)
{
    if (m_missileVec.size() > 0)
    {
        aPosRaw = m_missileVec[0].guidance.uiTargPosRaw;
        aPosMod = m_missileVec[0].guidance.uiTargPosMod;
    }
    else
    {
        aPosRaw = DirectX::SimpleMath::Vector3::Zero;
        aPosMod = DirectX::SimpleMath::Vector3::Zero;
    }
}

void FireControl::GuidanceManual(MissileData& aMissile, const float aTimeDelta)
{
    auto steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    steeringQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_manualThrustVecYaw, 0.0f, m_manualThrustVecPitch);
    auto steeringVec = DirectX::SimpleMath::Vector3::UnitX;
    steeringVec = DirectX::SimpleMath::Vector3::Transform(steeringVec, steeringQuat);
    steeringVec.Normalize();

    auto boosterQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, steeringVec);
    boosterQuat.Normalize();
    boosterQuat.Inverse(boosterQuat);

    aMissile.guidance.nav.vecToTargLocal = steeringVec;
    aMissile.guidance.nav.quatToTarg = boosterQuat;
    aMissile.guidance.nav.targPosLocalized = steeringVec;
}

void FireControl::GuidanceManualVector(MissileData& aMissile, const float aTimeDelta)
{
    const float maxInputAng = m_missileConsts.steerAngMax;
    auto inputScaled = DirectX::SimpleMath::Vector2::Zero;
    inputScaled.x = m_manualControlInput.x * maxInputAng;
    inputScaled.y = m_manualControlInput.y * maxInputAng;

    auto steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
    //steeringQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_manualThrustVecYaw, 0.0f, m_manualThrustVecPitch);
    steeringQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(inputScaled.x, 0.0f, inputScaled.y);

    auto steeringVec = DirectX::SimpleMath::Vector3::UnitX;
    steeringVec = DirectX::SimpleMath::Vector3::Transform(steeringVec, steeringQuat);
    steeringVec.Normalize();

    auto boosterQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, steeringVec);
    boosterQuat.Normalize();
    boosterQuat.Inverse(boosterQuat);

    auto thrustVec = DirectX::SimpleMath::Vector3::UnitX;
    thrustVec = DirectX::SimpleMath::Vector3::Transform(thrustVec, boosterQuat);

    aMissile.guidance.nav.vecToTargLocal = steeringVec;
    aMissile.guidance.nav.quatToTarg = boosterQuat;
    aMissile.guidance.nav.targPosLocalized = steeringVec;
    aMissile.guidance.nav.thrustVec = thrustVec;
}

void FireControl::GuidanceBasic(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto targVecNorm = targPosL;
    targVecNorm.Normalize();

    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    aMissile.guidance.nav.vecToTargLocal = targVecNorm;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = targVecNorm;
}

void FireControl::GuidanceBasicGravity(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto targPosNormW = targPosW;
    targPosNormW.Normalize();
    auto targPosNormL = targPosL;
    targPosNormL.Normalize();

    auto upLocal = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.inverseAlignmentQuat);
    auto toUpQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, upLocal);
    upLocal.Normalize();
    auto invsToUpQuat = toUpQuat;
    invsToUpQuat.Inverse(invsToUpQuat);

    auto targPosUpNorm = targPosNormL;
    //targPosUpNorm = DirectX::SimpleMath::Vector3::Transform(targPosUpNorm, invsToUpQuat);
    targPosUpNorm = DirectX::SimpleMath::Vector3::Transform(targPosUpNorm, toUpQuat);
    auto targPosUpCord = targPosL;
    targPosUpCord.Normalize();
    //targPosUpCord = DirectX::SimpleMath::Vector3::Transform(targPosUpCord, toUpQuat);
    targPosUpCord = DirectX::SimpleMath::Vector3::Transform(targPosUpCord, invsToUpQuat);
    targPosUpCord = DirectX::SimpleMath::Vector3::Transform(targPosUpCord, aMissile.projectileData.alignmentQuat);

    auto targPosUpCordNorm = targPosUpCord;
    targPosUpCordNorm.Normalize();

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("targPosUpCord.x = ", targPosUpCord.x, "");
    m_debugData->DebugPushUILineDecimalNumber("targPosUpCord.y = ", targPosUpCord.y, "");
    m_debugData->DebugPushUILineDecimalNumber("targPosUpCord.z = ", targPosUpCord.z, "");
    m_debugData->ToggleDebugOff();

    bool isTargetBehindTrue;
    //if (targPosL.x < 0.0f)
    if (targPosUpCord.y > 0.0f)
    {
        isTargetBehindTrue = true;
    }
    else
    {
        isTargetBehindTrue = false;
    }

    bool isReverseQuatTrue;
    if (targPosUpCord.x < 0.0f)
    {
        isReverseQuatTrue = true;
    }
    else
    {
        isReverseQuatTrue = false;
    }

    const float navTime = (targPosL - selfPosL).Length() / selfVelL.Length();
    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());
    const float distToTarget = (targPosL - selfPosL).Length();

    //float timeToImpact = distToTarget / closingSpeed;
    float timeToImpact = abs(distToTarget / closingSpeed);
    const float timeToImpactMax = 2.0f;
    if (timeToImpact > timeToImpactMax)
    {
        timeToImpact = timeToImpactMax;
    }

    auto targVelNormW = targVelW;
    targVelNormW.Normalize();
    const float targetSpeed = targVelW.Length();
    auto updateVelW = targVelNormW * (targetSpeed * timeToImpact);

    m_debugData->ToggleDebugOnOverRide();
    float height = 0.5f * -m_environment->GetGravity() * (timeToImpact * timeToImpact);
    auto updateTargPosW = aMissile.guidance.targetPosition;
    m_debugData->PushDebugLinePositionIndicator(updateTargPosW, 25.0f, 0.0f, DirectX::Colors::Yellow);
    updateTargPosW += updateVelW;
    
    m_debugData->PushDebugLinePositionIndicator(updateTargPosW, 25.0f, 0.0f, DirectX::Colors::Orange);
    
    updateTargPosW.y += height;
    m_debugData->PushDebugLinePositionIndicator(updateTargPosW, 25.0f, 0.0f, DirectX::Colors::Lime);
    auto updateTargPosL = DirectX::SimpleMath::Vector3::Transform((updateTargPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    m_debugData->ToggleDebugOff();
    ////////

    //auto targVecNorm = targPosL;
    auto targVecNorm = updateTargPosL;
    targVecNorm.Normalize();

    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    auto targAng = DirectX::SimpleMath::Quaternion::Angle(DirectX::SimpleMath::Quaternion::Identity,
        DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm));
    auto invsAng = Utility::GetPi() - targAng;

    auto crossTargVec = targVecNorm.Cross(upLocal);
    if (isTargetBehindTrue == true)
    {
        if (isReverseQuatTrue == true)
        {
            auto reverseQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(crossTargVec, -invsAng);
            reverseQuat.Normalize();
            reverseQuat.Inverse(reverseQuat);
            toTargQuat = reverseQuat;
            int testBreak = 0;
            testBreak++;
        }


    }
    ///////////////
    auto testLine = DirectX::SimpleMath::Vector3::UnitX;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, toTargQuat);

    auto testLine2 = DirectX::SimpleMath::Vector3::UnitX;

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, crossTargVec, DirectX::Colors::Red, false, false);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, targVecNorm, DirectX::Colors::Blue, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, selfVelL, DirectX::Colors::Lime, false, true);
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, targPosUpNorm, DirectX::Colors::Blue, false, false);
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, targPosUpNorm, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, upLocal, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, upLocal, DirectX::Colors::Blue, false, false);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");
    m_debugData->DebugPushUILineDecimalNumber("height = ", height, "");
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");
    

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("targAng = ", Utility::ToDegrees(targAng), "");
    m_debugData->DebugPushUILineDecimalNumber("invsAng = ", Utility::ToDegrees(invsAng), "");
    m_debugData->DebugPushUILineWholeNumber("isTargetBehindTrue = ", static_cast<int>(isTargetBehindTrue), "");
    m_debugData->DebugPushUILineWholeNumber("isReverseQuatTrue = ", static_cast<int>(isReverseQuatTrue), "");
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");
    
    m_debugData->DebugPushUILineDecimalNumber("targetSpeed = ", targetSpeed, "");
    
    m_debugData->ToggleDebugOff();
    /////////////
    aMissile.guidance.nav.vecToTargLocal = targVecNorm;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = targVecNorm;
}

void FireControl::GuidanceTest(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);

    const float navTime = (targPosL - selfPosL).Length() / selfVelL.Length();
    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());
    const float distToTarget = (targPosL - selfPosL).Length();

    float timeToImpact = distToTarget / closingSpeed;
    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();

    //auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL - selfPosL);
    auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL);
    losQuat.Normalize();

    auto invsLosQuat = losQuat;
    invsLosQuat.Inverse(invsLosQuat);
    auto targVelNormToLos = targVelL;
    targVelNormToLos = DirectX::SimpleMath::Vector3::Transform(targVelNormToLos, invsLosQuat);
    auto prevTargVelNormToLos = aMissile.guidance.targVelNormToLos;
    //aMissile.guidance.targVelNormToLos = targVelNormToLos;
    //aMissile.guidance.targAccelNormalToLOS = (aMissile.guidance.targVelNormToLos - prevTargVelNormToLos) / aTimeDelta;


    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");

    m_debugData->ToggleDebugOff();

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto targVelNormLocal = targVelL;
    const float targetVelocity = aMissile.guidance.targetVelocity.Length();
    targVelNormLocal.Normalize();

    auto tagVelOffset = targVelNormLocal * navTime;

    //
    auto targPosLocal = aMissile.guidance.targetPosition;
    targPosLocal -= aMissile.projectileData.q.position;
    targPosLocal = DirectX::SimpleMath::Vector3::Transform(targPosLocal, aMissile.projectileData.inverseAlignmentQuat);
    //
    targPosLocal = targPosL;
    //targPosLocal += tagVelOffset;

    auto targVecNorm = targPosLocal;
    targVecNorm.Normalize();

    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    aMissile.guidance.nav.vecToTargLocal = targVecNorm;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = targVecNorm;

    //aMissile.guidance.nav.vecToTargLocal = aMissile.guidance.navVecTest;
    //aMissile.guidance.nav.quatToTarg = aMissile.guidance.navQuatTest;
    //aMissile.guidance.nav.targPosLocalized = aMissile.guidance.navVecTest;

}

void FireControl::GuidanceTestOld(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);

    const float navTime = (targPosL - selfPosL).Length() / selfVelL.Length();
    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());
    const float distToTarget = (targPosL - selfPosL).Length();

    float timeToImpact = distToTarget / closingSpeed;
    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();

    //auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL - selfPosL);
    auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL);
    losQuat.Normalize();

    auto invsLosQuat = losQuat;
    invsLosQuat.Inverse(invsLosQuat);
    auto targVelNormToLos = targVelL;
    targVelNormToLos = DirectX::SimpleMath::Vector3::Transform(targVelNormToLos, invsLosQuat);
    auto prevTargVelNormToLos = aMissile.guidance.targVelNormToLos;
    //aMissile.guidance.targVelNormToLos = targVelNormToLos;
    //aMissile.guidance.targAccelNormalToLOS = (aMissile.guidance.targVelNormToLos - prevTargVelNormToLos) / aTimeDelta;

    auto vec = targVelNormToLos;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, targVelNormToLos, 10.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 10.0f, 0.0f, DirectX::Colors::Lime);

    vec = DirectX::SimpleMath::Vector3::UnitX;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, losQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 10.0f, 0.0f, DirectX::Colors::Lime);

    vec = targVelL;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 10.0f, 0.0f, DirectX::Colors::Orange);

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.closureRate = ", aMissile.guidance.closureRate, "");
    m_debugData->DebugPushUILineDecimalNumber("timeToImpact = ", timeToImpact, "");
    m_debugData->DebugPushUILineDecimalNumber("targVelW = ", targVelW.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("navTime = ", navTime, "");

    m_debugData->ToggleDebugOff();


    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto targVelNormLocal = targVelL;
    const float targetVelocity = aMissile.guidance.targetVelocity.Length();
    targVelNormLocal.Normalize();

    auto tagVelOffset = targVelNormLocal * navTime;


    //
    auto targPosLocal = aMissile.guidance.targetPosition;
    targPosLocal -= aMissile.projectileData.q.position;
    targPosLocal = DirectX::SimpleMath::Vector3::Transform(targPosLocal, aMissile.projectileData.inverseAlignmentQuat);
    //
    targPosLocal = targPosL;
    //targPosLocal += tagVelOffset;





    auto targVecNorm = targPosLocal;
    targVecNorm.Normalize();

    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    aMissile.guidance.nav.vecToTargLocal = targVecNorm;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = targVecNorm;

    //aMissile.guidance.nav.vecToTargLocal = aMissile.guidance.navVecTest;
    //aMissile.guidance.nav.quatToTarg = aMissile.guidance.navQuatTest;
    //aMissile.guidance.nav.targPosLocalized = aMissile.guidance.navVecTest;

    // world cords for draw calls
    auto targPosWorld = targPosLocal;
    targPosWorld = DirectX::SimpleMath::Vector3::Transform(targPosWorld, aMissile.projectileData.alignmentQuat);
    targPosWorld += aMissile.projectileData.q.position;

    auto targVecNormWorld = targVecNorm;
    targVecNormWorld = DirectX::SimpleMath::Vector3::Transform(targVecNormWorld, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, targVecNormWorld, 500.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(targPosWorld, 10.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->ToggleDebugOff();

    vec = DirectX::SimpleMath::Vector3::Zero;
    vec = aMissile.guidance.nav.vecToTargLocal;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 7.0f, 0.0f, DirectX::Colors::Blue);

    vec = aMissile.guidance.nav.targPosLocalized;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 8.0f, 0.0f, DirectX::Colors::Yellow);

    vec = DirectX::SimpleMath::Vector3::UnitX;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.guidance.nav.quatToTarg);
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 9.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->ToggleDebugOff();
}

void FireControl::GuidanceVelocitySteeringTest(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);

    const float navTime = (targPosL - selfPosL).Length() / selfVelL.Length();
    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());
    const float distToTarget = (targPosL - selfPosL).Length();

    float timeToImpact = distToTarget / closingSpeed;
    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();

    //auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL - selfPosL);
    auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL);
    losQuat.Normalize();

    auto invsLosQuat = losQuat;
    invsLosQuat.Inverse(invsLosQuat);
    auto targVelNormToLos = targVelL;
    targVelNormToLos = DirectX::SimpleMath::Vector3::Transform(targVelNormToLos, invsLosQuat);
    auto prevTargVelNormToLos = aMissile.guidance.targVelNormToLos;
    //aMissile.guidance.targVelNormToLos = targVelNormToLos;
    //aMissile.guidance.targAccelNormalToLOS = (aMissile.guidance.targVelNormToLos - prevTargVelNormToLos) / aTimeDelta;

    auto vec = targVelNormToLos;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto targVelNormLocal = targVelL;
    const float targetVelocity = aMissile.guidance.targetVelocity.Length();
    targVelNormLocal.Normalize();

    auto tagVelOffset = targVelNormLocal * navTime;

    //
    auto targPosLocal = aMissile.guidance.targetPosition;
    targPosLocal -= aMissile.projectileData.q.position;
    targPosLocal = DirectX::SimpleMath::Vector3::Transform(targPosLocal, aMissile.projectileData.inverseAlignmentQuat);
    //
    targPosLocal = targPosL;
    //targPosLocal += tagVelOffset;

    auto targVecNorm = targPosLocal;
    targVecNorm.Normalize();

    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    /*
    aMissile.guidance.nav.vecToTargLocal = targVecNorm;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = targVecNorm;

    //aMissile.guidance.nav.vecToTargLocal = aMissile.guidance.navVecTest;
    //aMissile.guidance.nav.quatToTarg = aMissile.guidance.navQuatTest;
    //aMissile.guidance.nav.targPosLocalized = aMissile.guidance.navVecTest;
    */

    auto velToTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(selfVelL, targPosL);
    auto velToTargVec = DirectX::SimpleMath::Vector3::UnitX;
    velToTargVec = DirectX::SimpleMath::Vector3::Transform(velToTargVec, velToTargQuat);
    velToTargVec.Normalize();
    //toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targVecNorm);
    toTargQuat = velToTargQuat;
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);

    aMissile.guidance.nav.vecToTargLocal = velToTargVec;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = velToTargVec;

    /////////////////////

    
    //vec = Seek(selfVelL, targPosL);
    vec = Pursuit(aMissile, selfVelL, targPosL, targVelL);
    toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, vec);
    toTargQuat.Normalize();
    toTargQuat.Inverse(toTargQuat);
    aMissile.guidance.nav.vecToTargLocal = vec;
    aMissile.guidance.nav.quatToTarg = toTargQuat;
    aMissile.guidance.nav.targPosLocalized = vec;

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, vec, DirectX::Colors::Red, false, true);

    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, toTargQuat), DirectX::Colors::Blue, false, true);
    
}

void FireControl::HardBurnModeActivator(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.isHardBurnModeTrue == false)
    {
        if (aMissile.guidance.isFacingDestTrue == false) // initialize vars needes
        {
            aMissile.guidance.isHardBurnModeTrue = true;

            aMissile.guidance.hardBurnVec = DirectX::SimpleMath::Vector3::UnitX;
            aMissile.guidance.hardBurnQuat = DirectX::SimpleMath::Quaternion::Identity;

            //aMissile.guidance.hardBurnVec = aMissile.projectileData.forward;
            aMissile.guidance.hardBurnVec = aMissile.projectileData.q.velocity;
            aMissile.guidance.hardBurnVec.Normalize();
            aMissile.guidance.hardBurnQuat = aMissile.projectileData.alignmentQuat;
        }
    }
    else if (aMissile.guidance.isHardBurnModeTrue == true)
    {
        if (aMissile.guidance.isFacingDestTrue == true)
        {
            aMissile.guidance.isHardBurnModeTrue = false;
        }
    }
}

void FireControl::HardBurnModeTest(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());

    auto velNormLocal = selfVelL;
    velNormLocal.Normalize();

    auto vecToTargWorld = targPosW - selfPosW;
    auto vecToTargLocal = vecToTargWorld;
    vecToTargLocal = DirectX::SimpleMath::Vector3::Transform(vecToTargLocal, aMissile.projectileData.inverseAlignmentQuat);
    auto vecToTargLocNorm = vecToTargLocal;
    vecToTargLocNorm.Normalize();
    auto velNormToTargLocalDot = velNormLocal.Dot(vecToTargLocNorm);
    auto forToVelNormLocalDot = DirectX::SimpleMath::Vector3::UnitX.Dot(velNormLocal);

    auto velQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, velNormLocal);
    auto toTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, vecToTargLocNorm);
    auto velToTargQuat = DirectX::SimpleMath::Quaternion::FromToRotation(velNormLocal, vecToTargLocNorm);
    velToTargQuat.Normalize();

    auto testQuat = DirectX::SimpleMath::Quaternion::Identity;
    testQuat.RotateTowards(velToTargQuat, (m_missileConsts.hardBurnRadPerSec * aTimeDelta));

    auto destLocal = DirectX::SimpleMath::Vector3::UnitX;
    destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, testQuat);

    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;

    auto destWorld = destLocal;
    destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("velNormToTargLocalDot ", velNormToTargLocalDot, "");
    m_debugData->DebugPushUILineDecimalNumber("forToVelNormLocalDot ", forToVelNormLocalDot, "");
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTargLocal, 4.0f, 0.0f, DirectX::Colors::Lavender);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTargWorld, 4.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, destWorld, 4.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->ToggleDebugOff();
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
    //aAmmo.ammoData.length = m_missileDimensions.x;
    //aAmmo.ammoData.radius = m_missileDimensions.z * 0.5f;

    aAmmo.ammoData.length = m_missileConsts.dimensions.x;
    aAmmo.ammoData.radius = m_missileConsts.dimensions.z * 0.5f;

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

void FireControl::InitializeContrails(MissileData& aMissile)
{
    aMissile.contrails.bottom.posLocal = DirectX::SimpleMath::Vector3(m_finLib.tailPitch.semiSpan * 0.5f, -m_finLib.tailYaw.chord * 0.25f, 0.0f);
    aMissile.contrails.starboard.posLocal = DirectX::SimpleMath::Vector3(m_finLib.tailPitch.semiSpan * 0.5f, -m_finLib.tailPitch.chord * 0.25f, 0.0f);
    aMissile.contrails.top.posLocal = DirectX::SimpleMath::Vector3(m_finLib.tailPitch.semiSpan * 0.5f, -m_finLib.tailYaw.chord * 0.25f, 0.0f);
    aMissile.contrails.port.posLocal = DirectX::SimpleMath::Vector3(m_finLib.tailPitch.semiSpan * 0.5f, -m_finLib.tailPitch.chord * 0.25f, 0.0f);

    aMissile.contrails.bottom.posVec.clear();
    aMissile.contrails.top.posVec.clear();
    aMissile.contrails.starboard.posVec.clear();
    aMissile.contrails.port.posVec.clear();

    aMissile.contrails.bottom.posVec.resize(m_ammoMissile.modelData.contrailDrawCountMax);
    aMissile.contrails.top.posVec.resize(m_ammoMissile.modelData.contrailDrawCountMax);
    aMissile.contrails.starboard.posVec.resize(m_ammoMissile.modelData.contrailDrawCountMax);
    aMissile.contrails.port.posVec.resize(m_ammoMissile.modelData.contrailDrawCountMax);

    aMissile.contrails.iterator = 0;

    for (unsigned int i = 0; i < m_ammoMissile.modelData.contrailDrawCountMax; ++i)
    {
        UpdateContrails(aMissile);
    }
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

void FireControl::InitializeFinLibrary(FinLibrary& aFinLib)
{
    float dragBase = 0.04f;
    float dragModMax = 1.17f;
    const float bodyDiameter = m_missileConsts.dimensions.z * 2.0f;
    FinDataStatic finDat;
    finDat.finType = FinType::CANARD_PITCH;
    finDat.name = "CANARD_PITCH";
    finDat.axis = DirectX::SimpleMath::Vector3::UnitZ;
    finDat.finNormal = DirectX::SimpleMath::Vector3::UnitY;
    finDat.posLocal = m_missileConsts.canardPosLocal;
    finDat.dragCoeffBase = dragBase;
    finDat.dragCoeefMod = dragModMax;
    finDat.isFinAngFixed = false;
    finDat.chord = m_missileConsts.canardChord;
    //finDat.span = m_missileConsts.canardSpan * 2.0f;
    finDat.span = m_missileConsts.canardSpan;
    finDat.semiSpan = finDat.span * 0.5f;
    finDat.wingSpanFull = finDat.span + bodyDiameter;
    finDat.thickness = m_missileConsts.canardThickness;
    finDat.wingArea = finDat.chord * finDat.span;
    finDat.frontArea = finDat.thickness * finDat.span;
    aFinLib.canardPitch = finDat;
   
    finDat.finType = FinType::CANARD_YAW;
    finDat.name = "CANARD_YAW";
    finDat.axis = -DirectX::SimpleMath::Vector3::UnitY;
    finDat.finNormal = DirectX::SimpleMath::Vector3::UnitZ;
    aFinLib.canardYaw = finDat;

    finDat.finType = FinType::TAIL_YAW;
    finDat.name = "TAIL_YAW";
    finDat.axis = DirectX::SimpleMath::Vector3::UnitY;
    finDat.finNormal = -DirectX::SimpleMath::Vector3::UnitZ;
    finDat.dragCoeffBase = dragBase;
    finDat.dragCoeefMod = dragModMax;
    finDat.posLocal = m_missileConsts.tailPosLocal;
    finDat.chord = m_missileConsts.tailChord;
    //finDat.span = m_missileConsts.tailSpan * 2.0f;
    finDat.span = m_missileConsts.tailSpan;
    finDat.semiSpan = finDat.span * 0.5f;
    finDat.wingSpanFull = finDat.span + bodyDiameter;
    finDat.thickness = m_missileConsts.tailThickness;
    finDat.wingArea = finDat.chord * finDat.span;
    finDat.frontArea = finDat.thickness * finDat.span;
    aFinLib.tailYaw = finDat;

    finDat.finType = FinType::TAIL_PITCH;
    finDat.name = "TAIL_PITCH";
    finDat.axis = DirectX::SimpleMath::Vector3::UnitZ;
    finDat.finNormal = DirectX::SimpleMath::Vector3::UnitY;
    aFinLib.tailPitch = finDat;

    finDat.finType = FinType::MAIN_PITCH;
    finDat.name = "MAIN_PITCH";
    finDat.isFinAngFixed = true;
    finDat.dragCoeffBase = dragBase;
    finDat.dragCoeefMod = dragModMax;
    finDat.posLocal = m_missileConsts.mainWingPosLocal;
    finDat.chord = m_missileConsts.mainChord;
    //finDat.span = m_missileConsts.mainSpan * 2.0f;
    finDat.span = m_missileConsts.mainSpan;
    finDat.semiSpan = finDat.span * 0.5f;
    finDat.wingSpanFull = finDat.span + bodyDiameter;
    finDat.thickness = m_missileConsts.mainThickness;
    finDat.wingArea = finDat.chord * finDat.span;
    finDat.frontArea = finDat.thickness * finDat.span;

    //////// setting dimensions equal to tail fin for testing
    finDat.chord = m_missileConsts.tailChord;
    //finDat.span = m_missileConsts.tailSpan * 2.0f;
    finDat.span = m_missileConsts.tailSpan;
    finDat.semiSpan = finDat.span * 0.5f;
    finDat.wingSpanFull = finDat.span + bodyDiameter;
    finDat.thickness = m_missileConsts.tailThickness;
    finDat.wingArea = finDat.chord * finDat.span;
    finDat.frontArea = finDat.thickness * finDat.span;
    ////////

    aFinLib.mainPitch = finDat;

    finDat.finType = FinType::MAIN_YAW;
    finDat.name = "MAIN_YAW";
    finDat.axis = DirectX::SimpleMath::Vector3::UnitY;
    finDat.finNormal = -DirectX::SimpleMath::Vector3::UnitZ;
    aFinLib.mainYaw = finDat;

    auto pitchQuat = DirectX::SimpleMath::Quaternion::Identity;
    auto yawQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, Utility::ToRadians(90.0f));

    InitializeFinAeroTensor(aFinLib.tailPitch, pitchQuat, m_missileConsts.tailFinAngMax);
    InitializeFinAeroTensor(aFinLib.tailYaw, yawQuat, m_missileConsts.tailFinAngMax);
}

void FireControl::InitializeFinAeroTensor(FinDataStatic& aFinData, const DirectX::SimpleMath::Quaternion aAlignment, const float aAngleMinMax)
{
    // cuboid
    const float xExtent = aFinData.chord;
    const float yExtent = aFinData.thickness;
    const float zExtent = aFinData.span;
    const float mass = 1.0f;

    auto baseTensor = DirectX::SimpleMath::Matrix::Identity;

    baseTensor._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    baseTensor._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    baseTensor._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    /*
    rudder(cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0),
        cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0.01f, 0, 0),
        cyclone::Matrix3(0, 0, 0, 0, 0, 0, -0.01f, 0, 0),
        cyclone::Vector3(2.0f, 0.5f, 0), &windspeed),


        tail(cyclone::Matrix3(0,0,0, -1,-0.5f,0, 0,0,-0.1f),
    */
    baseTensor = DirectX::SimpleMath::Matrix::Identity;
    baseTensor = DirectX::SimpleMath::Matrix::Transform(baseTensor, aAlignment);
    
    auto maxTensor = baseTensor;
    maxTensor._13 = -0.1f;
    auto minTensor = baseTensor;
    minTensor._13 = 0.1f;

    auto baseTensorInverse = baseTensor;
    baseTensorInverse = baseTensorInverse.Invert();

    aFinData.baseTensor = baseTensor;
    aFinData.maxTensor = maxTensor;
    aFinData.minTensor = minTensor;

}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection,
    Environment const* aEnvironment, std::shared_ptr<Vehicle> aVehicle)
{
    m_debugDrawVec.clear();

    m_playerVehicle = aVehicle;
    m_explosionStruct.explosionToPushVec.clear();
    m_projectileVec.clear();
    m_newProjectilePushVec.clear();
    m_missileVec.clear();
    m_environment = aEnvironment;
    m_currentAmmoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;

    InitializeAmmoCannon(m_ammoCannon);
    InitializeAmmoExplosive(m_ammoExplosive);
    InitializeAmmoMachineGun(m_ammoMachineGun);
    InitializeAmmoMirv(m_ammoMirv);
    InitializeAmmoShotgun(m_ammoShotgun);

    InitializeAmmoMissile(m_ammoMissile);

    InitializeExplosionData(aContext, m_explosionStruct.explosionRefData);
    InitializeMuzzleFlashModel(aContext, m_muzzleFlash);
    InitializeProjectileModelCannon(aContext, m_ammoCannon);
    InitializeProjectileModelExplosive(aContext, m_ammoExplosive);
    InitializeProjectileModelMachineGun(aContext, m_ammoMachineGun);
    InitializeProjectileModelMirv(aContext, m_ammoMirv);
    InitializeProjectileModelShotgun(aContext, m_ammoShotgun);
    InitializeLaserModel(aContext, m_playerLaser);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);

    InitializeFinLibrary(m_finLib);
    InitializeProjectileModelMissile(aContext, m_ammoMissile);
    InitializeMissileInertiaTensor();   
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeMissileInertiaTensor()
{
    m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;

    /*
    const float xExtent = m_missileDimensions.x;
    const float yExtent = m_missileDimensions.y;
    const float zExtent = m_missileDimensions.z;
    const float mass = m_missileConsts.mass;

    // cuboid
    m_missileInertiaTensorLocal._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));
    */

    // cylinder
    //const float m_missileMass = 10.0f;

    const float mass = m_missileConsts.mass;
    const float radius = m_missileDimensions.z * 0.5f;
    const float height = m_missileDimensions.x;

    m_missileInertiaTensorLocal._33 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._22 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._11 = (1.0f / 2.0f) * (mass) * (radius * radius);

    //m_missileInertiaTensorLocal *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, Utility::ToRadians(90.0f));

    // cuboid
    /*
    const float xExtent = m_missileDimensions.x;
    const float yExtent = m_missileDimensions.y;
    const float zExtent = m_missileDimensions.z;
    //const float mass = m_missileConsts.mass;

    m_missileInertiaTensorLocal._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_missileInertiaTensorLocal._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));
    */

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
    const float ammoDiameter = m_missileConsts.dimensions.z;
    const float ammoLength = m_missileConsts.dimensions.x;

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
    const float tailFinLength = m_finLib.tailYaw.semiSpan;
    const float tailFinWidth = m_finLib.tailYaw.chord * 0.5f;
    const DirectX::SimpleMath::Vector3 tailFinDimensions = DirectX::SimpleMath::Vector3(tailFinLength, tailFinWidth, m_finLib.tailYaw.thickness);
    aAmmo.modelData.tailFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinDimensions);

    const float tailOffset = m_missileConsts.tailPosLocal.x;
    // tail fin generic
    aAmmo.modelData.localTailFinMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.tailFinTransMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.tailFinTransMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailOffset * 1.0f, aAmmo.ammoData.radius * 0.9f, 0.0f));

    // tail fin axel
    const float finAxelLength = (aAmmo.ammoData.radius * 2.0f) * 1.05f;
    const float finAxelDiameter = tailFinDimensions.y;
    aAmmo.modelData.finAxelShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), finAxelLength, finAxelDiameter);
    const float tailFinAxelOffset = tailOffset;
    aAmmo.modelData.finAxelTranslation = DirectX::SimpleMath::Vector3(tailFinAxelOffset, 0.0f, 0.0f);

    // canards
    const float canardFinLength = aAmmo.ammoData.length * 0.3f;
    const float canardFinWidth = canardFinLength * 0.2f;
    const float canardOffset = ammoLength * 0.45f;
    const float canardFinLength2 = m_ammoMissile.ammoData.length * 0.3f;
    const float canardFinWidth2 = canardFinLength * 0.2f;
    aAmmo.modelData.localCanardFinMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localCanardFinMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(canardFinLength2 * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.canardFinTransMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.canardFinTransMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(canardOffset * 0.5f, aAmmo.ammoData.radius * 0.9f, 0.0f));
    aAmmo.modelData.canardFinTransMat = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.canardFinTransMat *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(canardOffset * 1.0f, aAmmo.ammoData.radius * 0.9f, 0.0f));
    const float canardFinAxelOffset = canardOffset;
    aAmmo.modelData.canardAxelTranslation = DirectX::SimpleMath::Vector3(canardFinAxelOffset, 0.0f, 0.0f);

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
    aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), 1.0f);
    DirectX::SimpleMath::Matrix wingScale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(m_finLib.mainYaw.chord * 0.5f,
        m_finLib.mainYaw.thickness, m_finLib.mainYaw.semiSpan));
    auto wingTrans = DirectX::SimpleMath::Vector3(m_finLib.mainYaw.chord * 0.333f, 0.0f, m_finLib.mainYaw.semiSpan * 0.333f);
    auto wingPosOffset = m_finLib.mainYaw.posLocal;
    wingPosOffset.z += aAmmo.ammoData.radius;

    aAmmo.modelData.localWingFinMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix *= wingScale;
    aAmmo.modelData.localWingFinMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingTrans);
    aAmmo.modelData.wingTranslation = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.wingTranslation *= DirectX::SimpleMath::Matrix::CreateTranslation(wingPosOffset);
 
    // thrust
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

void FireControl::InputNavData(MissileData& aMissile, DirectX::SimpleMath::Quaternion aQuatToTarg, const DirectX::SimpleMath::Vector3 aTargPosLocalized, const DirectX::SimpleMath::Vector3 aVecToTargLocal)
{
    aMissile.guidance.nav.vecToTargLocal = aVecToTargLocal;
    aMissile.guidance.nav.quatToTarg = aQuatToTarg;
    aMissile.guidance.nav.targPosLocalized = aTargPosLocalized;
}

void FireControl::IRSeekerTest(MissileData& aMissile, const float aTimeDelta)
{
    auto irPosLocal = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    irPosLocal = DirectX::SimpleMath::Vector3::Transform(irPosLocal, aMissile.projectileData.inverseAlignmentQuat);

    auto irPosWorld = irPosLocal;
    irPosWorld = DirectX::SimpleMath::Vector3::Transform(irPosWorld, aMissile.projectileData.alignmentQuat);
    irPosWorld += aMissile.projectileData.q.position;
}

void FireControl::ManualControlInput(FinType aFinType, const float aInput)
{
    if (aFinType == FinType::CANARD_PITCH)
    {
        m_manualCanardPitch = ManualInputUpdate(m_manualCanardPitch, aInput);
    }
    else if (aFinType == FinType::CANARD_YAW)
    {
        m_manualCanardYaw = ManualInputUpdate(m_manualCanardYaw, aInput);
    }
    else if (aFinType == FinType::TAIL_PITCH)
    {
        m_manualTailPitch = ManualInputUpdate(m_manualTailPitch, aInput);
    }
    else if (aFinType == FinType::TAIL_YAW)
    {
        m_manualTailYaw = ManualInputUpdate(m_manualTailYaw, aInput);
    }
    else
    {
        // error
    }
}

void FireControl::ManualControlInputPitch(const float aInput)
{
    m_isManualUpdatePitchTrue = true;

    const float inputMod = aInput * m_manualInputRate;

    if (inputMod + m_manualControlInput.y > 1.0f)
    {
        m_manualControlInput.y = 1.0f;
    }
    else if (inputMod + m_manualControlInput.y < -1.0f)
    {
        m_manualControlInput.y = -1.0f;
    }
    else
    {
        m_manualControlInput.y += inputMod;
    }


    if (m_currentControlType == ControlInputType::INPUT_CANARD)
    {
        m_manualCanardPitch = ManualInputUpdate(m_manualCanardPitch, aInput);
    }
    else if (m_currentControlType == ControlInputType::INPUT_TAILFIN)
    {
        m_manualTailPitch = ManualInputUpdate(m_manualTailPitch, -aInput);
        m_manualThrustVecPitch = ManualInputUpdate(m_manualThrustVecPitch, aInput);
    }
    else if (m_currentControlType == ControlInputType::INPUT_THRUSTVEC)
    {
        m_manualThrustVecPitch = ManualInputUpdate(m_manualThrustVecPitch, -aInput);
    }
    else
    {
        // error
    }
}

void FireControl::ManualControlInputYaw(const float aInput)
{
    m_isManualUpdateYawTrue = true;

    const float inputMod = aInput * m_manualInputRate;

    if (inputMod + m_manualControlInput.x > 1.0f)
    {
        m_manualControlInput.x = 1.0f;
    }
    else if (inputMod + m_manualControlInput.x < -1.0f)
    {
        m_manualControlInput.x = -1.0f;
    }
    else
    {
        m_manualControlInput.x += inputMod;
    }

    if (m_currentControlType == ControlInputType::INPUT_CANARD)
    {
        m_manualCanardYaw = ManualInputUpdate(m_manualCanardYaw, aInput);
    }
    else if (m_currentControlType == ControlInputType::INPUT_TAILFIN)
    {
        //m_manualTailYaw = ManualInputUpdate(m_manualTailYaw, -aInput);
        //m_manualThrustVecYaw = ManualInputUpdate(m_manualThrustVecYaw, aInput);
    }
    else if (m_currentControlType == ControlInputType::INPUT_THRUSTVEC)
    {
        m_manualThrustVecYaw = ManualInputUpdate(m_manualThrustVecYaw, -aInput);
    }
    else
    {
        // error
    }
}

float FireControl::ManualInputDecay(const float aCurrentVal, const float aTimeStep)
{
    const float decayRate = m_missileConsts.steeringAngPerSecDeltaMax * m_manualDecayRateMod * aTimeStep;

    float updateVal = 0.0f;
    if (aCurrentVal == 0.0f)
    {
        return 0.0f;
    }
    else if (aCurrentVal > 0.0f)
    {
        updateVal = aCurrentVal - decayRate;
    }
    else
    {
        updateVal = aCurrentVal + decayRate;
    }

    if (aCurrentVal > 0.0f && updateVal < 0.0f)
    {
        updateVal = 0.0f;
    }
    else if (aCurrentVal < 0.0f && updateVal > 0.0f)
    {
        updateVal = 0.0f;
    }

    return updateVal;
}

void FireControl::ManualInputDecayVector(const float aTimeStep)
{
    const float decay = m_manualDecayRate * aTimeStep;
    if (m_manualControlInput.x != 0.0f && m_isManualUpdateYawTrue == false)
    {
        if (m_manualControlInput.x > 0.0f)
        {
            m_manualControlInput.x -= decay;
            if (m_manualControlInput.x < 0.0f)
            {
                m_manualControlInput.x = 0.0f;
            }
        }
        else if (m_manualControlInput.x < 0.0f)
        {
            m_manualControlInput.x += decay;
            if (m_manualControlInput.x > 0.0f)
            {
                m_manualControlInput.x = 0.0f;
            }
        }
    }

    if (m_manualControlInput.y != 0.0f && m_isManualUpdatePitchTrue == false)
    {
        if (m_manualControlInput.y > 0.0f)
        {
            m_manualControlInput.y -= decay;
            if (m_manualControlInput.y < 0.0f)
            {
                m_manualControlInput.y = 0.0f;
            }
        }
        else if (m_manualControlInput.y < 0.0f)
        {
            m_manualControlInput.y += decay;
            if (m_manualControlInput.y > 0.0f)
            {
                m_manualControlInput.y = 0.0f;
            }
        }
    }

    m_isManualUpdatePitchTrue = false;
    m_isManualUpdateYawTrue = false;
}

void FireControl::ManualInputReset(FinType aFinType, const bool aIsResetAllTrue)
{
    if (aIsResetAllTrue == true)
    {
        m_manualCanardPitch = 0.0f;
        m_manualCanardYaw = 0.0f;

        m_manualThrustVecPitch = 0.0f;
        m_manualThrustVecYaw = 0.0f;

        m_manualTailPitch = 0.0f;
        m_manualTailYaw = 0.0f;
    }
    else
    {
        if (aFinType == FinType::CANARD_PITCH)
        {
            m_manualCanardPitch = 0.0f;
        }
        else if (aFinType == FinType::CANARD_YAW)
        {
            m_manualCanardYaw = 0.0f;
        }
        else if (aFinType == FinType::TAIL_PITCH)
        {
            m_manualTailPitch = 0.0f;
        }
        else if (aFinType == FinType::TAIL_YAW)
        {
            m_manualTailYaw = 0.0f;
        }
        else
        {
            // error
        }
    }
}

float FireControl::ManualInputUpdate(const float aCurrentVal, const float aInput)
{
    const float updateRaw = (aInput * m_missileConsts.steeringAngPerSecDeltaMax) + aCurrentVal;

    float updateVal = 0.0f;
    if (updateRaw > m_missileConsts.steerAngMax)
    {
        updateVal = m_missileConsts.steerAngMax;
    }
    else if (updateRaw < -m_missileConsts.steerAngMax)
    {
        updateVal = -m_missileConsts.steerAngMax;
    }

    else
    {
        updateVal = updateRaw;
    }
    return updateVal;
}

void FireControl::PrintFinData(FinDataStatic& aFinStat, FinDataDynamic& aFinDyn, MissileData& aMissile)
{
    const DirectX::SimpleMath::Vector3 pos = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Quaternion quat = aMissile.projectileData.alignmentQuat;

    DirectX::SimpleMath::Vector3 finPos = aFinStat.posLocal;
    finPos = DirectX::SimpleMath::Vector3::Transform(finPos, quat);
    finPos += pos;

    DirectX::SimpleMath::Vector3 finNorm = aFinStat.finNormal;
    finNorm = DirectX::SimpleMath::Vector3::Transform(finNorm, quat);
    //finNorm += pos;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(finPos, finNorm, 1.1f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->PushDebugLinePositionIndicatorAligned(finPos, 0.4f, 0.0f, quat, DirectX::Colors::White);
    m_debugData->ToggleDebugOff();

    auto dragForce = aFinDyn.dragForce;
    dragForce = DirectX::SimpleMath::Vector3::Transform(dragForce, quat);

    auto liftForce = aFinDyn.liftForce;
    liftForce = DirectX::SimpleMath::Vector3::Transform(liftForce, quat);

    auto resultantForce = aFinDyn.resultantForce;
    resultantForce = DirectX::SimpleMath::Vector3::Transform(resultantForce, quat);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(finPos, dragForce, 2.1f, 0.0f, DirectX::Colors::DarkSalmon);
    m_debugData->PushDebugLine(finPos, liftForce, 1.1f, 0.0f, DirectX::Colors::Lavender);
    m_debugData->PushDebugLine(finPos, resultantForce, 1.1f, 0.0f, DirectX::Colors::RoyalBlue);
    m_debugData->DebugPushUILineDecimalNumber("aFinDyn.dragForce = ", aFinDyn.dragForce.Length(), "");

    m_debugData->PushDebugLinePositionIndicatorAligned(finPos, 0.4f, 0.0f, quat, DirectX::Colors::White);
    m_debugData->ToggleDebugOff();

}

void FireControl::PrintFlightStateData(MissileData& aMissile)
{
    auto velLocal = aMissile.projectileData.q.velocity;
    velLocal = DirectX::SimpleMath::Vector3::Transform(velLocal, aMissile.projectileData.inverseAlignmentQuat);
    //m_debugData->DebugClearUI();

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.lataxNeeded = ", aMissile.guidance.lataxNeeded, "");

    auto vec = aMissile.guidance.lataxAxis;
    vec = DirectX::SimpleMath::Vector3::Transform(vec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, vec, 2.0f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->ToggleDebugOnOverRide();
    
    if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_ATTACK ", 1, "");
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CLIMBOUT)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_CLIMBOUT ", 1, "");
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_CRUISE)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_CRUISE ", 1, "");
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_DEBUG)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_DEBUG ", 1, "");
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_EXPLODING)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_EXPLODING ", 1, "");
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_LAUNCH)
    {
        m_debugData->DebugPushUILineWholeNumber("FlightState::FLIGHTSTATE_LAUNCH ", 1, "");
    }
    

    m_debugData->ToggleDebugOff();
}

void FireControl::PrintMissileData(MissileData& aMissile, const float aTimeDelta)
{
    m_debugData->ToggleDebugOnOverRide();
    float speed = aMissile.projectileData.q.velocity.Length();
    float mph = speed * 2.237f;

    m_debugData->DebugPushUILineDecimalNumber("speed   = ", speed, "");
    m_debugData->DebugPushUILineWholeNumber("mph = ", static_cast<int>(mph), "");
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto heading = aMissile.projectileData.forward;
    heading.y = 0.0f;
    heading.Normalize();

    auto compassVec = DirectX::SimpleMath::Vector3::UnitX;
    float headingCompass = Utility::GetAngleBetweenVectors(heading, compassVec);

    if (heading.Dot(DirectX::SimpleMath::Vector3::UnitZ) < 0.0f)
    {
        headingCompass *= -1.0f;
        headingCompass += Utility::ToRadians(360.0f);
    }
    float headingAngle = Utility::GetAngleBetweenVectors(heading, DirectX::SimpleMath::Vector3::UnitX);
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto velocity = aMissile.projectileData.q.velocity;
    velocity.y = 0.0f;
    velocity.Normalize();

    float velCompass = Utility::GetAngleBetweenVectors(velocity, compassVec);

    if (velocity.Dot(DirectX::SimpleMath::Vector3::UnitZ) < 0.0f)
    {
        velCompass *= -1.0f;
        velCompass += Utility::ToRadians(360.0f);
    }
    float velAngle = Utility::GetAngleBetweenVectors(velocity, DirectX::SimpleMath::Vector3::UnitX);
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //m_debugData->DebugPushUILineDecimalNumber("headingCompass deg = ", Utility::ToDegrees(headingCompass), "");
    //m_debugData->DebugPushUILineDecimalNumber("velCompass deg = ", Utility::ToDegrees(velCompass), "");
    m_debugData->DebugPushUILineWholeNumber("ForwardCompass = ", static_cast<int>(Utility::ToDegrees(headingCompass)), "");
    m_debugData->DebugPushUILineWholeNumber("VelocityCompas = ", static_cast<int>(Utility::ToDegrees(velCompass)), "");
    m_debugData->ToggleDebugOff();
}

void FireControl::ProNav(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());

    auto prevLosNorm = aMissile.guidance.losNormTest;
    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();
    aMissile.guidance.losNormTest = losNormLocal;

    auto losAngDelta = (Utility::GetAngleBetweenVectors(losNormLocal, prevLosNorm) / aTimeDelta);
    auto losAng = Utility::GetAngleBetweenVectors(losNormLocal, DirectX::SimpleMath::Vector3::UnitX);
    const float N = 3.0f;
    auto trueProNav = N * closingSpeed * losAngDelta;

    ///////////////////////////////////////////////////

    // target velocity relative to missile
    //vec2 Vr = Vt - Vm;
    DirectX::SimpleMath::Vector3 Vr = targVelL - selfVelL;
    // range from missile to target
    //DirectX::SimpleMath::Vector3 R = Rt - Rm;
    DirectX::SimpleMath::Vector3 R = targPosL - selfPosL;
    //some constant
    //float N = ...;
    //rotation vector of line of sight
    //vec3 Omega = cross(R, Vr) / dot(R, R);
    DirectX::SimpleMath::Vector3 Omega = R.Cross(Vr) / R.Dot(R);
    DirectX::SimpleMath::Vector3 Omega2 = R.Cross(Vr) / R.Length();

    // the cross product of orthogonal vector to R and Vr from previous cross
    // product, and Vr again, will produce vector which lies on the plane of R 
    // and Vr again, hence we can ignore the last dimension of the cross product 
    // it will be zero anyway for 2d.
    //DirectX::SimpleMath::Vector3 a = N * cross(Vr, Omega);
    const DirectX::SimpleMath::Vector3 a = N * Vr.Cross(Omega2);
    const DirectX::SimpleMath::Vector3 a2 = N * Vr.Cross(Omega2);

    //////////////////////////////////////////////////////////////////////

    // los angle & los rate
    float lataxNeeded = m_missileConsts.mass * a.Length();

    const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;

    float tan = 0.0f;
    if (boostForce > 0.0f)
    {
        tan = atan(a.Length() / boostForce);
        //tan *= 1.0f / aTimeDelta;
    }
    //float tan = atan(2.0 / 500.0);

    auto updateVec = DirectX::SimpleMath::Vector3::UnitX;
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Omega, -tan));
    auto updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    //auto destLocal = updateVec;
    auto destLocal = -a;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    ///////////////////////////////////////////////////////

    aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();

    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    DirectX::SimpleMath::Vector3 currentLos = targPosL - selfPosL;
    aMissile.guidance.losCurrent = currentLos;
    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;

    aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losRate = losDelta.Length();
    aMissile.guidance.closingVelocityToTarget = closingSpeed;

    auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL - selfPosL);
    losQuat.Normalize();

    auto invsLosQuat = losQuat;
    invsLosQuat.Inverse(invsLosQuat);
    auto targVelNormToLos = targVelL;
    targVelNormToLos = DirectX::SimpleMath::Vector3::Transform(targVelNormToLos, invsLosQuat);
    auto prevTargVelNormToLos = aMissile.guidance.targVelNormToLos;
    //aMissile.guidance.targVelNormToLos = targVelNormToLos;
    //aMissile.guidance.targAccelNormalToLOS = (aMissile.guidance.targVelNormToLos - prevTargVelNormToLos) / aTimeDelta;

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta * Nt * (0.5 * N)
    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate;
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate + aMissile.guidance.losDelta;
    aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate
        + aMissile.guidance.losDelta * aMissile.guidance.targAccelNormalToLOS * (0.5 * m_missileConsts.navigationGain);

    auto losLocal = targPosL - selfPosL;
    losLocal.Normalize();
    auto lataxTestVec = DirectX::SimpleMath::Vector3::UnitX.Cross(losLocal);
    auto lataxTestVecWorld = lataxTestVec;
    lataxTestVecWorld = DirectX::SimpleMath::Vector3::Transform(lataxTestVecWorld, aMissile.projectileData.alignmentQuat);

    //aMissile.guidance.targAccelNormalToLOS.Normalize();
    auto lataxTest = lataxTestVec * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate
        + aMissile.guidance.losDelta * aMissile.guidance.targAccelNormalToLOS * (0.5 * m_missileConsts.navigationGain);

    auto lataxTestWorld = lataxTest;
    lataxTestWorld = DirectX::SimpleMath::Vector3::Transform(lataxTestWorld, aMissile.projectileData.alignmentQuat);

    //////////////////////
    lataxNeeded = m_missileConsts.mass * lataxTest.Length();
    tan = 0.0f;
    auto sin = 0.0f;
    if (boostForce > 0.0f)
    {
        //tan = atan(lataxTest.Length() / boostForce);
        //tan *= 1.0f / aTimeDelta;
        //tan = asin(lataxNeeded / boostForce);
        sin = asin(lataxNeeded / boostForce);
        //tan += Utility::ToRadians(-90.0f);
        if (targPosL.x >= 0.0f)
        {
            //tan += Utility::ToRadians(-90.0f);
        }
        else
        {
            //tan += Utility::ToRadians(180.0f);
        }
    }
    //float tan = atan(2.0 / 500.0);
    const float thrustAngle = sin;


    updateVec = DirectX::SimpleMath::Vector3::UnitX;
    //updateVec = losLocal;
    auto quatAxis = lataxTest;
    quatAxis.Normalize();
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(quatAxis, thrustAngle));
    updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    destLocal = updateVec;
    destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

  //  aMissile.guidance.headingLocalVecTest = destLocal;
  //  aMissile.guidance.headingLocalQuatTest = destQuat;
    InputNavData(aMissile, destQuat, destLocal, destLocal);
}

void FireControl::ProNav2(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;
    const DirectX::SimpleMath::Vector3 targPosAlt = aMissile.guidance.targetDestination;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.navigationTime", aMissile.guidance.navigationTime, "");
    m_debugData->PushDebugLinePositionIndicator(targPosW, 30.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(selfPosW, 10.0f, 0.0f, DirectX::Colors::Lime);
    m_debugData->PushDebugLinePositionIndicator(targPosAlt, 20.0f, 0.0f, DirectX::Colors::Violet);

    m_debugData->PushDebugLine(selfPosW, selfVelW, 6.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(targPosW, targVelW, 36.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->ToggleDebugOff();


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // target velocity relative to missile
    //vec2 Vr = Vt - Vm;
    DirectX::SimpleMath::Vector3 Vr = aMissile.guidance.targetVelocity - aMissile.projectileData.q.velocity;
    // range from missile to target
    //vec2 R = Rt - Rm;
    DirectX::SimpleMath::Vector3 R = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    //some constant
    //float N = ...;
    float N = 3.0f;
    //rotation vector of line of sight
    //vec3 Omega = cross(R, Vr) / dot(R, R);
    DirectX::SimpleMath::Vector3 Omega = R.Cross(Vr) / R.Dot(R);

    // the cross product of orthogonal vector to R and Vr from previous cross
    // product, and Vr again, will produce vector which lies on the plane of R 
    // and Vr again, hence we can ignore the last dimension of the cross product 
    // it will be zero anyway for 2d.
    //vec2 a = N * cross(Vr, Omega);
    DirectX::SimpleMath::Vector3 a = N * Vr.Cross(Omega);
    DirectX::SimpleMath::Vector3 aCrossForward = a.Cross(aMissile.projectileData.forward);
    DirectX::SimpleMath::Vector3 aCrossCross = aCrossForward.Cross(a);

    DirectX::SimpleMath::Vector3 aCrossCrossLocal = aCrossCross;
    aCrossCrossLocal = DirectX::SimpleMath::Vector3::Transform(aCrossCrossLocal, aMissile.projectileData.inverseAlignmentQuat);

    DirectX::SimpleMath::Vector3 aW = a;
    aW = DirectX::SimpleMath::Vector3::Transform(aW, aMissile.projectileData.alignmentQuat);

    DirectX::SimpleMath::Vector3 destLocal = aCrossCrossLocal;
    DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    DirectX::SimpleMath::Vector3 testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    DirectX::SimpleMath::Vector3 headingLocalVecTestWorld = aMissile.guidance.headingLocalVecTest;
    headingLocalVecTestWorld = DirectX::SimpleMath::Vector3::Transform(headingLocalVecTestWorld, aMissile.projectileData.alignmentQuat);
    //m_debugData->ToggleDebugOnOverRide();

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Lime);

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, a, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, selfVelL, 10.0f, 0.0f, DirectX::Colors::Green);
    /*
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, a, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aCrossForward, 10.0f, 0.0f, DirectX::Colors::Violet);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aCrossCross, 10.0f, 0.0f, DirectX::Colors::Lime);
    */
    m_debugData->DebugPushUILineDecimalNumber("a.x = ", a.x, "");
    m_debugData->DebugPushUILineDecimalNumber("a.y = ", a.y, "");
    m_debugData->DebugPushUILineDecimalNumber("a.z = ", a.z, "");
    m_debugData->DebugPushUILineDecimalNumber("a.l = ", a.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("a.D = ", Utility::ToDegrees(a.Length()), "");
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.headingLocalVecTest, 8.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingLocalVecTestWorld, 8.0f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->ToggleDebugOff();

    testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, Utility::ToRadians(10.0f)));

    // aMissile.guidance.headingLocalVecTest = testVec;


    DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-DirectX::SimpleMath::Vector3::UnitX, aMissile.guidance.headingLocalVecTest);
    testQuat.Normalize();
    //testQuat.Inverse(testQuat);
   // aMissile.guidance.headingLocalQuatTest = testQuat;

    DirectX::SimpleMath::Vector3 testTargPosW = targPosL;
    testTargPosW = DirectX::SimpleMath::Vector3::Transform(testTargPosW, aMissile.projectileData.alignmentQuat);
    testTargPosW += aMissile.projectileData.q.position;


    DirectX::SimpleMath::Vector3 testTargVelW = targVelL;
    testTargVelW = DirectX::SimpleMath::Vector3::Transform(testTargVelW, aMissile.projectileData.alignmentQuat);
    testTargVelW += aMissile.projectileData.q.position;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLinePositionIndicator(testTargPosW, 30.0f, 0.0f, DirectX::Colors::Blue);

    m_debugData->ToggleDebugOff();


    auto desiredVelocity = targPosW - selfPosW;
    desiredVelocity -= selfVelW;
    auto desiredVelocityNorm = desiredVelocity;
    desiredVelocityNorm.Normalize();
    desiredVelocityNorm = DirectX::SimpleMath::Vector3::Transform(desiredVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);

    auto dVelNormWorld = desiredVelocityNorm;
    dVelNormWorld = DirectX::SimpleMath::Vector3::Transform(dVelNormWorld, aMissile.projectileData.alignmentQuat);





    destLocal = desiredVelocityNorm;
    destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;


    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Orange);


    m_debugData->ToggleDebugOff();


}

void FireControl::ProNav3(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;
    //const DirectX::SimpleMath::Vector3 targPosAlt = aMissile.guidance.targetDestination;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto mVelLocal = aMissile.projectileData.q.velocity;
    mVelLocal = DirectX::SimpleMath::Vector3::Transform(mVelLocal, aMissile.projectileData.inverseAlignmentQuat);

    auto mVelWorld = mVelLocal;
    mVelWorld = DirectX::SimpleMath::Vector3::Transform(mVelWorld, aMissile.projectileData.alignmentQuat);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    const float navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    DirectX::SimpleMath::Vector3 currentLos = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    auto currentLosLocal = DirectX::SimpleMath::Vector3::Transform(currentLos, aMissile.projectileData.inverseAlignmentQuat);
    currentLosLocal.Normalize();
    //aMissile.guidance.losCurrent = currentLos;
    aMissile.guidance.losCurrent = currentLosLocal;

    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;
    DirectX::SimpleMath::Vector3 losDeltaLocal = currentLosLocal - prevLos;

    //aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losDelta = losDeltaLocal;
    //aMissile.guidance.losRate = losDelta.Length();
    aMissile.guidance.losRate = losDeltaLocal.Length();
    //aMissile.guidance.closingVelocityToTarget = -aMissile.guidance.losRate;
    aMissile.guidance.closingVelocityToTarget = aMissile.guidance.targetDistanceDelta / aTimeDelta;

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;

    const auto lataxCal = aMissile.guidance.losCurrent * 3.0f * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;
    const auto lataxCalWorld = DirectX::SimpleMath::Vector3::Transform(lataxCal, aMissile.projectileData.alignmentQuat);
    const auto lataxCalLocal = DirectX::SimpleMath::Vector3::Transform(lataxCal, aMissile.projectileData.inverseAlignmentQuat);

    aMissile.guidance.latax = lataxCal;

    aMissile.guidance.latax = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.latax, aMissile.projectileData.inverseAlignmentQuat);
    auto lataxTest = aMissile.guidance.latax;
    auto lataxTestNorm = lataxTest;
    lataxTestNorm.Normalize();

    const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;

    float tan = 0.0f;
    if (boostForce > 0.0f)
    {
        tan = atan(aMissile.guidance.latax.Length() / boostForce);
        tan *= 10000.0f;
    }
    //float tan = atan(2.0 / 500.0);
    auto lataxWorld = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.latax, aMissile.projectileData.alignmentQuat);

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto mVelLocalNorm = mVelLocal;
    mVelLocalNorm.Normalize();
    auto lataxCalNorm = lataxCal;
    lataxCalNorm.Normalize();

    //auto velLataxCrossLocal = mVelLocalNorm.Cross(lataxCalNorm);
    auto velLataxCrossLocal = lataxCalNorm.Cross(mVelLocalNorm);
    velLataxCrossLocal.Normalize();
    if (velLataxCrossLocal.Length() < 0.5f || velLataxCrossLocal.Length() > 2.0f)
    {
        velLataxCrossLocal = DirectX::SimpleMath::Vector3::UnitY;
    }


    auto updateVec = DirectX::SimpleMath::Vector3::UnitX;
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(velLataxCrossLocal, tan));
    auto updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    auto destLocal = updateVec;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;




    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto losWorld = aMissile.guidance.losCurrent;
    losWorld = DirectX::SimpleMath::Vector3::Transform(losWorld, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOnOverRide();

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, losWorld, 10.0f, 0.0f, DirectX::Colors::Orange);

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCal, 10.0f, 0.0f, DirectX::Colors::LightBlue);
  //  m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalWorld, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalLocal, 10.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
  //  m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax = ", aMissile.guidance.latax.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("tan = ", Utility::ToDegrees(tan), "");

    auto tanDeg = Utility::ToDegrees(tan);

    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.x ", lataxTest.x, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.y ", lataxTest.y, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.z ", lataxTest.z, "");

    m_debugData->DebugPushUILineDecimalNumber("lataxTest.x ", lataxTestNorm.x, "");
    m_debugData->DebugPushUILineDecimalNumber("lataxTest.y ", lataxTestNorm.y, "");
    m_debugData->DebugPushUILineDecimalNumber("lataxTest.z ", lataxTestNorm.z, "");

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, mVelWorld, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, mVelLocal, 10.0f, 0.0f, DirectX::Colors::Violet);

    float angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(velLataxCrossLocal, lataxCal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(lataxCal, mVelLocal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");


    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(mVelLocal, velLataxCrossLocal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    /////////////////////////

    auto testVec = velLataxCrossLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Blue);

    testVec = lataxCal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Red);

    testVec = mVelLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Lime);


    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.closingVelocityToTarget = ", aMissile.guidance.closingVelocityToTarget, "");


    m_debugData->ToggleDebugOff();
}

void FireControl::ProNav4(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;
    //const DirectX::SimpleMath::Vector3 targPosAlt = aMissile.guidance.targetDestination;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto mVelLocal = aMissile.projectileData.q.velocity;
    mVelLocal = DirectX::SimpleMath::Vector3::Transform(mVelLocal, aMissile.projectileData.inverseAlignmentQuat);

    auto mVelWorld = mVelLocal;
    mVelWorld = DirectX::SimpleMath::Vector3::Transform(mVelWorld, aMissile.projectileData.alignmentQuat);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    const float navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    DirectX::SimpleMath::Vector3 currentLos = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    auto currentLosLocal = DirectX::SimpleMath::Vector3::Transform(currentLos, aMissile.projectileData.inverseAlignmentQuat);
    currentLosLocal.Normalize();
    //aMissile.guidance.losCurrent = currentLos;
    aMissile.guidance.losCurrent = currentLosLocal;

    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;
    DirectX::SimpleMath::Vector3 losDeltaLocal = currentLosLocal - prevLos;

    //aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losDelta = losDeltaLocal;
    //aMissile.guidance.losRate = losDelta.Length();
    aMissile.guidance.losRate = losDeltaLocal.Length();
    //aMissile.guidance.closingVelocityToTarget = -aMissile.guidance.losRate;
    aMissile.guidance.closingVelocityToTarget = aMissile.guidance.targetDistanceDelta / aTimeDelta;

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;

    const auto lataxCal = aMissile.guidance.losCurrent * 3.0f * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;
    const auto lataxCalWorld = DirectX::SimpleMath::Vector3::Transform(lataxCal, aMissile.projectileData.alignmentQuat);
    const auto lataxCalLocal = DirectX::SimpleMath::Vector3::Transform(lataxCal, aMissile.projectileData.inverseAlignmentQuat);

    aMissile.guidance.latax = lataxCal;

    aMissile.guidance.latax = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.latax, aMissile.projectileData.inverseAlignmentQuat);
    auto lataxTest = aMissile.guidance.latax;
    auto lataxTestNorm = lataxTest;
    lataxTestNorm.Normalize();

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCal, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalLocal, 10.0f, 0.0f, DirectX::Colors::LightBlue);
    //m_debugData->ToggleDebugOff();


    const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;

    float tan = 0.0f;
    if (boostForce > 0.0f)
    {
        tan = atan(aMissile.guidance.latax.Length() / boostForce);
        tan *= 10000.0f;
    }
    //float tan = atan(2.0 / 500.0);
    auto lataxWorld = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.latax, aMissile.projectileData.alignmentQuat);

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    auto mVelLocalNorm = mVelLocal;
    mVelLocalNorm.Normalize();
    auto lataxCalNorm = lataxCal;
    lataxCalNorm.Normalize();

    //auto velLataxCrossLocal = mVelLocalNorm.Cross(lataxCalNorm);
    auto velLataxCrossLocal = lataxCalNorm.Cross(mVelLocalNorm);
    velLataxCrossLocal.Normalize();
    if (velLataxCrossLocal.Length() < 0.5f || velLataxCrossLocal.Length() > 2.0f)
    {
        velLataxCrossLocal = DirectX::SimpleMath::Vector3::UnitY;
    }

    /////////////
    // double cross method
    auto velLataxCrossCrossLocal = velLataxCrossLocal.Cross(lataxCalNorm);

    auto velQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, velLataxCrossCrossLocal);
    auto velQuatInverse = velQuat;
    velQuatInverse.Inverse(velQuatInverse);


    auto velLataxCrossCrossWorld = velLataxCrossCrossLocal;
    velLataxCrossCrossWorld = DirectX::SimpleMath::Vector3::Transform(velLataxCrossCrossWorld, aMissile.projectileData.alignmentQuat);

    auto velLataxCrossWorld = velLataxCrossLocal;
    velLataxCrossWorld = DirectX::SimpleMath::Vector3::Transform(velLataxCrossWorld, aMissile.projectileData.alignmentQuat);
    //
    /////////////

    auto updateVec = DirectX::SimpleMath::Vector3::UnitX;
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, velQuatInverse);

    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(velLataxCrossLocal, tan));
    auto updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    auto destLocal = updateVec;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->ToggleDebugOff();
    auto destWorld = destLocal;
    destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, destWorld, 10.0f, 0.0f, DirectX::Colors::DeepPink);

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, velLataxCrossCrossWorld, 10.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, velLataxCrossWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);



    auto testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Red);

    //m_debugData->ToggleDebugOnOverRide();

    testVec = aMissile.guidance.latax;
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Red);

    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Blue);

    m_debugData->ToggleDebugOff();
    testVec = mVelLocalNorm;
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Red);
    auto testVec1 = testVec;

    //testVec = velLataxCrossCrossLocal;
    testVec = velLataxCrossCrossLocal;
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Blue);
    auto testVec2 = testVec;

    testVec = velLataxCrossLocal;
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Yellow);
    auto testVec3 = testVec;


    float angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testVec1, testVec2));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testVec2, testVec3));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testVec3, testVec1));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");
    m_debugData->ToggleDebugOff();

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    auto losWorld = aMissile.guidance.losCurrent;
    losWorld = DirectX::SimpleMath::Vector3::Transform(losWorld, aMissile.projectileData.alignmentQuat);
    /*

   // m_debugData->PushDebugLine(aMissile.projectileData.q.position, losWorld, 10.0f, 0.0f, DirectX::Colors::Orange);

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCal, 10.0f, 0.0f, DirectX::Colors::LightBlue);
  //  m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalWorld, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxCalLocal, 10.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, lataxWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
  //  m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax = ", aMissile.guidance.latax.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("tan = ", Utility::ToDegrees(tan), "");

    auto tanDeg = Utility::ToDegrees(tan);

    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.x ", lataxTest.x, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.y ", lataxTest.y, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.z ", lataxTest.z, "");

    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.x ", lataxTestNorm.x, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.y ", lataxTestNorm.y, "");
    //m_debugData->DebugPushUILineDecimalNumber("lataxTest.z ", lataxTestNorm.z, "");

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, mVelWorld, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, mVelLocal, 10.0f, 0.0f, DirectX::Colors::Violet);
    */

    m_debugData->ToggleDebugOnOverRide();
    /*
    float angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(velLataxCrossLocal, lataxCal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(lataxCal, velLataxCrossCrossLocal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");

    angleDeg = Utility::ToDegrees(Utility::GetAngleBetweenVectors(mVelLocal, velLataxCrossLocal));
    m_debugData->DebugPushUILineDecimalNumber("angleDeg = ", angleDeg, "");
    */
    m_debugData->ToggleDebugOff();

    /////////////////////////
    /*
    auto testVec = velLataxCrossLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Blue);

    testVec = lataxCal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Red);

    testVec = mVelLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Lime);

    */
}

void FireControl::ProNav5(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;
    //const DirectX::SimpleMath::Vector3 targPosAlt = aMissile.guidance.targetDestination;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto tmp = selfPosL - targPosL;
    //return -((a.velocity - b.velocity).dot(tmp) / tmp.length)
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("closingSpeed = ", closingSpeed, "");
    m_debugData->ToggleDebugOff();

    auto prevLosNorm = aMissile.guidance.losNormTest;
    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();
    aMissile.guidance.losNormTest = losNormLocal;

    auto losAngDelta = (Utility::GetAngleBetweenVectors(losNormLocal, prevLosNorm) / aTimeDelta);
    auto losAng = Utility::GetAngleBetweenVectors(losNormLocal, DirectX::SimpleMath::Vector3::UnitX);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("losAngDelta = ", Utility::ToDegrees(losAngDelta), "");
    m_debugData->DebugPushUILineDecimalNumber("losAng      = ", Utility::ToDegrees(losAng), "");
    m_debugData->ToggleDebugOff();


    const float N = 3.0f;
    auto trueProNav = N * closingSpeed * losAngDelta;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("trueProNav = ", trueProNav, "");
    //m_debugData->DebugPushUILineDecimalNumber("losAng      = ", Utility::ToDegrees(losAng), "");
    m_debugData->ToggleDebugOff();


    ///////////////////////////////////////////////////

    // target velocity relative to missile
    //vec2 Vr = Vt - Vm;
    DirectX::SimpleMath::Vector3 Vr = targVelL - selfVelL;
    // range from missile to target
    //DirectX::SimpleMath::Vector3 R = Rt - Rm;
    DirectX::SimpleMath::Vector3 R = targPosL - selfPosL;
    //some constant
    //float N = ...;
    //rotation vector of line of sight
    //vec3 Omega = cross(R, Vr) / dot(R, R);
    DirectX::SimpleMath::Vector3 Omega = R.Cross(Vr) / R.Dot(R);
    DirectX::SimpleMath::Vector3 Omega2 = R.Cross(Vr) / R.Length();

    // the cross product of orthogonal vector to R and Vr from previous cross
    // product, and Vr again, will produce vector which lies on the plane of R 
    // and Vr again, hence we can ignore the last dimension of the cross product 
    // it will be zero anyway for 2d.
    //DirectX::SimpleMath::Vector3 a = N * cross(Vr, Omega);
    const DirectX::SimpleMath::Vector3 a = N * Vr.Cross(Omega2);
    const DirectX::SimpleMath::Vector3 a2 = N * Vr.Cross(Omega2);

    auto aWorld = a;
    aWorld = DirectX::SimpleMath::Vector3::Transform(aWorld, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("a.Length() = ", a.Length(), "");
    //m_debugData->PushDebugLine(selfPosW, a, 5.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(selfPosW, a, 10.0f, 0.0f, DirectX::Colors::Lime);
    m_debugData->ToggleDebugOff();
    //////////////////////////////////////////////////////////////////////

    // los angle & los rate

    auto losNormWorld = losNormLocal;
    losNormWorld = DirectX::SimpleMath::Vector3::Transform(losNormWorld, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(selfPosW, losNormWorld, 10.0f, 0.0f, DirectX::Colors::Blue);


    auto omegaWorld = Omega;
    omegaWorld = DirectX::SimpleMath::Vector3::Transform(omegaWorld, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(selfPosW, omegaWorld, 7.0f, 0.0f, DirectX::Colors::Red);

    auto testVal = Utility::GetAngleBetweenVectors(losNormLocal, a);
    m_debugData->DebugPushUILineDecimalNumber("losNormLocal to a = ", Utility::ToDegrees(testVal), "");

    testVal = Utility::GetAngleBetweenVectors(Omega, a);
    m_debugData->DebugPushUILineDecimalNumber("Omega to a = ", Utility::ToDegrees(testVal), "");

    testVal = Utility::GetAngleBetweenVectors(Omega, losNormLocal);
    m_debugData->DebugPushUILineDecimalNumber("Omega to losNormLocal = ", Utility::ToDegrees(testVal), "");
    m_debugData->ToggleDebugOff();


    float lataxNeeded = m_missileConsts.mass * a.Length();

    const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;

    float tan = 0.0f;
    if (boostForce > 0.0f)
    {
        tan = atan(a.Length() / boostForce);
        //tan *= 1.0f / aTimeDelta;
    }
    //float tan = atan(2.0 / 500.0);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("tan = ", Utility::ToDegrees(tan), "");
    m_debugData->ToggleDebugOff();

    /////////auto destVec = DirectX::SimpleMath::Vector3::UnitX;


    auto updateVec = DirectX::SimpleMath::Vector3::UnitX;
    // updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, velQuatInverse);

    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(Omega, -tan));
    auto updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    //auto destLocal = updateVec;
    auto destLocal = -a;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    ///////////////////////////////////////////////////////


    aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    //DirectX::SimpleMath::Vector3 currentLos = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 currentLos = targPosL - selfPosL;
    aMissile.guidance.losCurrent = currentLos;
    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;

    aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losRate = losDelta.Length();
    //aMissile.guidance.closingVelocityToTarget = -aMissile.guidance.losRate;
    //aMissile.guidance.closingVelocityToTarget = aMissile.guidance.targetDistanceDelta / aTimeDelta;
    aMissile.guidance.closingVelocityToTarget = closingSpeed;



    auto losQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, targPosL - selfPosL);
    losQuat.Normalize();

    auto invsLosQuat = losQuat;
    invsLosQuat.Inverse(invsLosQuat);
    auto targVelNormToLos = targVelL;
    targVelNormToLos = DirectX::SimpleMath::Vector3::Transform(targVelNormToLos, invsLosQuat);
    auto prevTargVelNormToLos = aMissile.guidance.targVelNormToLos;
    aMissile.guidance.targVelNormToLos = targVelNormToLos;
    aMissile.guidance.targAccelNormalToLOS = (aMissile.guidance.targVelNormToLos - prevTargVelNormToLos) / aTimeDelta;

    //auto Nt = 

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta * Nt * (0.5 * N)
    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate;
    //aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate + aMissile.guidance.losDelta;
    aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate
        + aMissile.guidance.losDelta * aMissile.guidance.targAccelNormalToLOS * (0.5 * m_missileConsts.navigationGain);

    auto losLocal = targPosL - selfPosL;
    losLocal.Normalize();
    auto lataxTestVec = DirectX::SimpleMath::Vector3::UnitX.Cross(losLocal);
    auto lataxTestVecWorld = lataxTestVec;
    lataxTestVecWorld = DirectX::SimpleMath::Vector3::Transform(lataxTestVecWorld, aMissile.projectileData.alignmentQuat);

    aMissile.guidance.targAccelNormalToLOS.Normalize();
    auto lataxTest = lataxTestVec * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losRate
        + aMissile.guidance.losDelta * aMissile.guidance.targAccelNormalToLOS * (0.5 * m_missileConsts.navigationGain);

    auto lataxTestWorld = lataxTest;
    lataxTestWorld = DirectX::SimpleMath::Vector3::Transform(lataxTestWorld, aMissile.projectileData.alignmentQuat);

    //////////////////////
    lataxNeeded = m_missileConsts.mass * lataxTest.Length();
    //const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;
    tan = 0.0f;
    auto sin = 0.0f;
    if (boostForce > 0.0f)
    {
        //tan = atan(lataxTest.Length() / boostForce);
        //tan *= 1.0f / aTimeDelta;
        //tan = asin(lataxNeeded / boostForce);
        sin = asin(lataxNeeded / boostForce);
        //tan += Utility::ToRadians(-90.0f);
        if (targPosL.x >= 0.0f)
        {
            //tan += Utility::ToRadians(-90.0f);
        }
        else
        {
            //tan += Utility::ToRadians(180.0f);
        }
    }
    //float tan = atan(2.0 / 500.0);
    const float thrustAngle = sin;
    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("tan = ", Utility::ToDegrees(tan), "");
    m_debugData->DebugPushUILineDecimalNumber("lataxTest.Length() = ", lataxTest.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("boostForce = ", boostForce, "");
    m_debugData->DebugPushUILineDecimalNumber("lataxNeeded = ", lataxNeeded, "");
    m_debugData->ToggleDebugOff();


    updateVec = DirectX::SimpleMath::Vector3::UnitX;
    //updateVec = losLocal;
    auto quatAxis = lataxTest;
    quatAxis.Normalize();
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(quatAxis, thrustAngle));
    updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    destLocal = updateVec;
    destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    //destQuat.Inverse(destQuat);

    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;

    auto updateVecWorld = updateVec;
    updateVecWorld = DirectX::SimpleMath::Vector3::Transform(updateVecWorld, aMissile.projectileData.alignmentQuat);
    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(selfPosW, updateVecWorld, 16.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(selfPosW, aMissile.projectileData.forward, 12.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->ToggleDebugOff();
    ////////////////////

    auto lataxW = aMissile.guidance.latax;
    lataxW = DirectX::SimpleMath::Vector3::Transform(lataxW, aMissile.projectileData.alignmentQuat);
    auto lataxLocalized = aMissile.guidance.latax;
    lataxLocalized = DirectX::SimpleMath::Vector3::Transform(lataxLocalized, aMissile.projectileData.inverseAlignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax = ", aMissile.guidance.latax.Length(), "");
    m_debugData->PushDebugLine(selfPosW, aMissile.guidance.latax, 12.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(selfPosW, lataxW, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(selfPosW, lataxLocalized, 8.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(selfPosW, currentLos, 6.0f, 0.0f, DirectX::Colors::Orange);


    m_debugData->PushDebugLine(selfPosW, lataxTestVecWorld, 16.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->ToggleDebugOff();
    auto testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, losQuat);
    //testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    m_debugData->PushDebugLine(selfPosW, lataxTestWorld, 12.0f, 0.0f, DirectX::Colors::Blue);

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targAccelNormalToLOS = ", aMissile.guidance.targAccelNormalToLOS.Length(), "");



}

void FireControl::ProNavTest(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    auto tmp = selfPosL - targPosL;
    const float closingSpeed = -((selfVelL - targVelL).Dot(tmp) / tmp.Length());

    auto prevLosNorm = aMissile.guidance.losNormTest;
    prevLosNorm.Normalize();

    auto losNormWorld = targPosW - selfPosW;
    losNormWorld.Normalize();
    aMissile.guidance.losNormTest = losNormWorld;

    auto losAngDelta1 = (Utility::GetAngleBetweenVectors(losNormWorld, prevLosNorm));
    auto losAngDelta2 = (Utility::GetAngleBetweenVectors(losNormWorld, prevLosNorm) * aTimeDelta);
    auto losAngDelta3 = (Utility::GetAngleBetweenVectors(losNormWorld, prevLosNorm) / aTimeDelta);

    auto losNormLocal = targPosL - selfPosL;
    losNormLocal.Normalize();

    auto selfVelNormLocal = aMissile.projectileData.q.velocity;
    selfVelNormLocal = DirectX::SimpleMath::Vector3::Transform(selfVelNormLocal, aMissile.projectileData.inverseAlignmentQuat);
    selfVelNormLocal.Normalize();
    auto losVelDot = selfVelNormLocal.Dot(losNormLocal);

    bool isLosForward = true;
    if (losNormLocal.x < 0.0f)
    {
        isLosForward = false;
    }

    //auto losAngDelta = (Utility::GetAngleBetweenVectors(losNormLocal, prevLosNorm) / aTimeDelta);
    auto losAngDelta = (Utility::GetAngleBetweenVectors(losNormWorld, prevLosNorm) / aTimeDelta);
    //auto losAngDelta = Utility::GetAngleBetweenVectors(losNormLocal, prevLosNorm);
    auto losAng = Utility::GetAngleBetweenVectors(losNormLocal, DirectX::SimpleMath::Vector3::UnitX);
    const float N = 5.0f;
    //auto trueProNav = N * closingSpeed * losAngDelta;
    const auto trueProNav = (N * closingSpeed * losAngDelta) * aTimeDelta;
    if (isLosForward == false)
    {
        //   trueProNav = N * closingSpeed * -losAngDelta;
    }

    auto perpLosLocal = losNormLocal.Cross(DirectX::SimpleMath::Vector3::UnitX);
    //auto perpLosLocal = DirectX::SimpleMath::Vector3::UnitX.Cross(losNormLocal);
    auto perpLosWorld = perpLosLocal;
    perpLosWorld = DirectX::SimpleMath::Vector3::Transform(perpLosWorld, aMissile.projectileData.alignmentQuat);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->ToggleDebugOff();
    m_debugData->DebugPushUILineDecimalNumber("trueProNav ", trueProNav, "");
    m_debugData->DebugPushUILineDecimalNumber("closingSpeed ", closingSpeed, "");

    m_debugData->DebugPushUILineDecimalNumber("losAng ", losAng, "");
    m_debugData->DebugPushUILineDecimalNumber("closingSpeed", closingSpeed, "");
    m_debugData->DebugPushUILineDecimalNumber("losAngDelta ", losAngDelta, "");

    m_debugData->DebugPushUILineDecimalNumber("losAngDelta1 ", losAngDelta1, "");
    m_debugData->DebugPushUILineDecimalNumber("losAngDelta2 ", losAngDelta2, "");
    m_debugData->DebugPushUILineDecimalNumber("losAngDelta3 ", losAngDelta3, "");
    m_debugData->PushDebugLinePositionIndicator(targPosW, 10.0f, 0.0f, DirectX::Colors::White);

    auto testTargPosW = targPosL;
    testTargPosW = DirectX::SimpleMath::Vector3::Transform(testTargPosW, aMissile.projectileData.alignmentQuat);
    testTargPosW += aMissile.projectileData.q.position;
    m_debugData->PushDebugLinePositionIndicator(testTargPosW, 7.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->PushDebugLineScaled(aMissile.projectileData.q.position, perpLosWorld * trueProNav, 1.0, 1.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->ToggleDebugOff();

    //////////////////////////////////////////////////////////////////////////////////

        //////////////////////
    const float boostForce = aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax;
    //float boostForce = 30.0f;
    const float boostAccel = boostForce / m_missileConsts.mass;
    //boostForce = boostAccel;

    const float latAccelMax = (1.0f - sin(m_missileConsts.steerAngMax)) * boostAccel;
    const float latForceMax = (1.0f - sin(m_missileConsts.steerAngMax)) * boostForce;

    //const float boostAccel = (aMissile.guidance.throttlePercentage * m_missileConsts.rocketBoostForceMax) / m_missileConsts.mass;
   // trueProNav = 1.0f;

    //float lataxNeeded = m_missileConsts.mass * trueProNav;
    //float lataxNeeded = m_missileConsts.mass * (trueProNav * aTimeDelta);
    float lataxNeeded = trueProNav;
    float latForceNeeded = lataxNeeded * m_missileConsts.mass;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("lataxNeeded = ", lataxNeeded, "");
    m_debugData->DebugPushUILineDecimalNumber("latForceNeeded = ", latForceNeeded, "");
    m_debugData->DebugPushUILineDecimalNumber("boostAccel = ", boostAccel, "");
    m_debugData->DebugPushUILineDecimalNumber("boostForce = ", boostForce, "");
    m_debugData->DebugPushUILineDecimalNumber("ratioA  = ", lataxNeeded / boostAccel, "");
    m_debugData->DebugPushUILineDecimalNumber("ratioF  = ", latForceNeeded / boostForce, "");
    m_debugData->ToggleDebugOff();

    lataxNeeded = latForceNeeded;
    if (lataxNeeded > boostForce)
    {
        lataxNeeded = boostForce;
    }
    else if (lataxNeeded < -boostForce)
    {
        lataxNeeded = -boostForce;
    }
    else
    {

    }

    auto sin = 0.0f;
    if (boostForce > 0.0f)
    {
        sin = asin(lataxNeeded / boostForce);

        if (targPosL.x >= 0.0f)
        {
            //tan += Utility::ToRadians(-90.0f);
        }
        else
        {
            //tan += Utility::ToRadians(180.0f);
        }
    }

    const float thrustAngle = sin;
    const float thrustAngleDeg = Utility::ToDegrees(thrustAngle);
    auto trueProNavForce = trueProNav * m_missileConsts.mass;
    auto trueProNavForce2 = trueProNav / m_missileConsts.mass;

    auto updateVec = DirectX::SimpleMath::Vector3::UnitX;
    //updateVec = losLocal;
    auto quatAxis = perpLosLocal;
    if (aMissile.guidance.isFacingDestTrue == false)
    {
        //quatAxis *= -1.0f;
        //updateVec = -DirectX::SimpleMath::Vector3::UnitX;
    }
    if (losVelDot < 0.0f)
    {
        //quatAxis *= -1.0f;
        //updateVec = -DirectX::SimpleMath::Vector3::UnitX;
    }
    //quatAxis *= -1.0f;
    quatAxis.Normalize();
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(quatAxis, thrustAngle));
    //auto updateQuat = DirectX::SimpleMath::Quaternion::Identity;

    auto destLocal = updateVec;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;

    aMissile.guidance.navVecTest = destLocal;
    aMissile.guidance.navQuatTest = destQuat;
    aMissile.guidance.lataxNeeded = lataxNeeded;
    aMissile.guidance.lataxAxis = quatAxis;

    if (aMissile.guidance.isFacingDestTrue == true && aMissile.guidance.isVelocityForward == true)
    {
        //  aMissile.guidance.headingLocalVecTest = destLocal;
        //  aMissile.guidance.headingLocalQuatTest = destQuat;
    }

    auto destWorld = destLocal;
    destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);


    auto rotQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, m_debugVal1);
    //auto rotQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitZ, m_debugVal1);

    destLocal = DirectX::SimpleMath::Vector3::UnitX;
    destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, rotQuat);
    destLocal.Normalize();
    destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    InputNavData(aMissile, destQuat, destLocal, destLocal);

    destWorld = destLocal;
    destWorld = DirectX::SimpleMath::Vector3::Transform(destWorld, aMissile.projectileData.alignmentQuat);
    auto destWorld2 = DirectX::SimpleMath::Vector3::UnitX;
    destWorld2 = DirectX::SimpleMath::Vector3::Transform(destWorld2, destQuat);
    destWorld2 = DirectX::SimpleMath::Vector3::Transform(destWorld2, aMissile.projectileData.alignmentQuat);

    if (m_debugThrustAngMax < thrustAngle)
    {
        m_debugThrustAngMax = thrustAngle;
    }
    if (m_debugThrustAngMin > thrustAngle)
    {
        m_debugThrustAngMin = thrustAngle;
    }
}

void FireControl::ProNavOpenSteer(MissileData& aMissile, const float aTimeDelta)
{
    const DirectX::SimpleMath::Vector3 selfPosW = aMissile.projectileData.q.position;
    const DirectX::SimpleMath::Vector3 selfVelW = aMissile.projectileData.q.velocity;
    const DirectX::SimpleMath::Vector3 targPosW = aMissile.guidance.targetPosition;
    const DirectX::SimpleMath::Vector3 targVelW = aMissile.guidance.targetVelocity;
    //const DirectX::SimpleMath::Vector3 targPosAlt = aMissile.guidance.targetDestination;

    const DirectX::SimpleMath::Vector3 selfVelL = DirectX::SimpleMath::Vector3::Transform(selfVelW, aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 selfPosL = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 targPosL = DirectX::SimpleMath::Vector3::Transform((targPosW - selfPosW), aMissile.projectileData.inverseAlignmentQuat);
    const DirectX::SimpleMath::Vector3 targVelL = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.inverseAlignmentQuat);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const float maxPredictionTime = 45.0f;

    // offset from this to quarry, that distance, unit vector toward quarry
    const auto offset = targPosW - selfPosW;
    const float distance = offset.Length();
    const auto unitOffset = offset / distance;

    // how parallel are the paths of "this" and the quarry
    // (1 means parallel, 0 is pependicular, -1 is anti-parallel)
    //const float parallelness = aMissile.projectileData.forward.Dot(aMissile.guidance.targetForward);
    auto forwardTest = aMissile.projectileData.q.velocity;
    forwardTest.Normalize();
    const float parallelness = forwardTest.Dot(aMissile.guidance.targetForward);

    // how "forward" is the direction to the quarry
    // (1 means dead ahead, 0 is directly to the side, -1 is straight back)
    //const float forwardness = aMissile.projectileData.forward.Dot(unitOffset);
    const float forwardness = forwardTest.Dot(unitOffset);

    const float directTravelTime = distance / aMissile.projectileData.q.velocity.Length();
    const int f = Utility::IntervalComparison(forwardness, -0.707f, 0.707f);
    const int p = Utility::IntervalComparison(parallelness, -0.707f, 0.707f);
    float timeFactor = 0.0f;

    // Break the pursuit into nine cases, the cross product of the
    // quarry being [ahead, aside, or behind] us and heading
    // [parallel, perpendicular, or anti-parallel] to us.

    auto color = DirectX::Colors::White;

    switch (f)
    {
    case +1:
        switch (p)
        {
        case +1:          // ahead, parallel
            timeFactor = 4.0f;
            color = DirectX::Colors::Black;
            break;
        case 0:           // ahead, perpendicular
            timeFactor = 1.8f;
            color = DirectX::Colors::Gray;// 50;
            break;
        case -1:          // ahead, anti-parallel
            timeFactor = 0.85f;
            color = DirectX::Colors::White;
            break;
        }
        break;
    case 0:
        switch (p)
        {
        case +1:          // aside, parallel
            timeFactor = 1.0f;
            color = DirectX::Colors::Red;
            break;
        case 0:           // aside, perpendicular
            timeFactor = 0.8f;
            color = DirectX::Colors::Yellow;
            break;
        case -1:          // aside, anti-parallel
            timeFactor = 4.0f;
            color = DirectX::Colors::Green;
            break;
        }
        break;
    case -1:
        switch (p)
        {
        case +1:          // behind, parallel
            timeFactor = 0.5f;
            color = DirectX::Colors::Cyan;
            break;
        case 0:           // behind, perpendicular
            timeFactor = 2.0f;
            color = DirectX::Colors::Blue;
            break;
        case -1:          // behind, anti-parallel
            timeFactor = 2.0f;
            color = DirectX::Colors::Magenta;
            break;
        }
        break;
    }

    // estimated time until intercept of quarry
    const float et = directTravelTime * timeFactor;

    // xxx experiment, if kept, this limit should be an argument
    const float etl = (et > maxPredictionTime) ? maxPredictionTime : et;

    // estimated position of quarry at intercept
    const DirectX::SimpleMath::Vector3 target = aMissile.guidance.targetPosition + (aMissile.guidance.targetVelocity * etl);

    m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("etl  = ", etl, "");
    m_debugData->DebugPushUILineDecimalNumber("et  = ", et, "");
    m_debugData->ToggleDebugOff();


    /*
    OpenSteer::SimpleVehicle::predictFuturePosition(const float predictionTime) const
{
    return position() + (velocity() * predictionTime);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto desiredVelocity = targPosW - selfPosW;
    desiredVelocity -= selfVelW;

    desiredVelocity = target - selfPosW;

    auto desiredVelocityNorm = desiredVelocity;
    desiredVelocityNorm.Normalize();
    desiredVelocityNorm = DirectX::SimpleMath::Vector3::Transform(desiredVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);

    auto dVelNormWorld = desiredVelocityNorm;
    dVelNormWorld = DirectX::SimpleMath::Vector3::Transform(dVelNormWorld, aMissile.projectileData.alignmentQuat);

    auto destLocal = desiredVelocityNorm;
    auto destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    auto testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    aMissile.guidance.headingLocalVecTest = destLocal;
    aMissile.guidance.headingLocalQuatTest = destQuat;

    InputNavData(aMissile, destQuat, destLocal, destLocal);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, color);

    m_debugData->PushDebugLinePositionIndicator(target, 20.0f, 0.0f, color);

    m_debugData->DebugPushUILineDecimalNumber("missile Vel = ", aMissile.projectileData.q.velocity.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("target Vel  = ", aMissile.guidance.targetVelocity.Length(), "");

    m_debugData->ToggleDebugOff();
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

    Utility::ForceAccum aeroAccum = RHSAeroForceAccumulator(aProjectile, newQ.velocity);
    /*
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, aeroAccum.linear, DirectX::Colors::Lavender, false, false);
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, aeroAccum.torque, DirectX::Colors::Yellow, false, true);
    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("aeroAccum.linear = ", aeroAccum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("aeroAccum.torque = ", aeroAccum.torque.Length(), "");
    m_debugData->ToggleDebugOff();
    */
    ////////////////////////////////////////
    //float airSurfaceArea = 1.0f;
    float airSurfaceArea = m_missileConsts.dragAreaDebug;
    float airDensity = m_environment->GetAirDensity();
    //float dragCoefficient = m_missileConsts.dragCoefficient;
    float dragCoefficient = m_missileConsts.dragCoefficientDebug;
    float velocity = newQ.velocity.Length();
    auto velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    //float velocity = aNewQVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    //DirectX::SimpleMath::Vector3 drag = CalcDragLinearCurrent(aProjectile, newQ.velocity);
    DirectX::SimpleMath::Vector3 drag = airResistance;
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, drag, DirectX::Colors::Blue, false, false);
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("drag        = ", drag.Length(), "");

    m_debugData->ToggleDebugOff();
    ////////////////////////////////////////

    // linear
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    //velocityUpdate += newQ.velocity;
    //velocityUpdate += aProjectile->guidance.linearDragSum;
    //velocityUpdate += drag;

    velocityUpdate += aProjectile->guidance.linearForceSum;

    velocityUpdate += aeroAccum.linear;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_missileConsts.mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("angularForceSum = ", aProjectile->projectileData.angularForceSum.Length(), "");
    m_debugData->ToggleDebugOff();

    // angular
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    torqueAccum = newQ.angularVelocity;
    torqueAccum += aProjectile->projectileData.angularForceSum;
    torqueAccum += aeroAccum.torque;
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_missileInverseInertiaTensorLocal);
    //torqueAccum += aProjectile->projectileData.angularDragSum;

    DirectX::SimpleMath::Vector3 dragTorqueTest = aProjectile->projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, aTimeDelta);
    torqueAccum += dragTorqueTest;
    //torqueAccum += aProjectile->projectileData.angularDragSum;

    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("drag        = ", drag.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("linearForceSum = ", aProjectile->guidance.linearForceSum.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("newQ.velocity  = ", newQ.velocity.Length(), "");
    m_debugData->PushDebugLine(aProjectile->projectileData.q.position, drag, 7.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(aProjectile->projectileData.q.position, aProjectile->projectileData.q.velocity, 4.0f, 0.1f, DirectX::Colors::LimeGreen);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineWholeNumber("aProjectile->projectileData.rhsCounter ", aProjectile->projectileData.rhsCounter, "");
    m_debugData->DebugPushUILineWholeNumber("1 ", 1, "");
    m_debugData->ToggleDebugOff();

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, drag);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, dragNew);

    ++aProjectile->projectileData.rhsCounter;
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
    m_isInRunge = true;
    RightHandSideMissile(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSideMissile(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSideMissile(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSideMissile(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aProjectile->projectileData.rhsCounter = 0;
    m_isInRunge = false;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;

    //auto dragTest = CalcDragLinearCurrent(aProjectile, aProjectile->projectileData.q.velocity);

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
        m_isDebugMissilePosToTankTrue = false;
    }
    else
    {
        m_isDebugToggleTrue1 = true;
        m_isDebugMissilePosToTankTrue = true;

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
        //m_isUseProNavOn = false;
    }
    else
    {
        m_isDebugToggleTrue3 = true;
        //m_isUseProNavOn = true;
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

void FireControl::UpdateFinAngles(MissileData& aMissile)
{
    aMissile.guidance.finPak.canardPitch.finAngle = 0.0f;
    aMissile.guidance.finPak.canardYaw.finAngle = 0.0f;

    if (m_missileConsts.isDynamicFinOn == true)
    {
        m_debugData->ToggleDebugOnOverRide();

        aMissile.guidance.finPak.tailPitch.prevFinAngle = aMissile.guidance.finPak.tailPitch.finAngle;
        aMissile.guidance.finPak.tailYaw.prevFinAngle = aMissile.guidance.finPak.tailYaw.finAngle;

        aMissile.guidance.finPak.tailPitch.finAngle = aMissile.guidance.conDat.finPitch;
        aMissile.guidance.finPak.tailYaw.finAngle = aMissile.guidance.conDat.finYaw;
    }
    else
    {
        aMissile.guidance.finPak.tailPitch.prevFinAngle = aMissile.guidance.finPak.tailPitch.finAngle;
        aMissile.guidance.finPak.tailYaw.prevFinAngle = aMissile.guidance.finPak.tailYaw.finAngle;

        aMissile.guidance.finPak.tailPitch.finAngle = 0.0f;
        aMissile.guidance.finPak.tailYaw.finAngle = 0.0f;
    }

    aMissile.guidance.finPak.tailPitch.finDir = m_finLib.tailPitch.finNormal;
    aMissile.guidance.finPak.tailPitch.finDir = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.finPak.tailPitch.finDir,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_finLib.tailPitch.axis, aMissile.guidance.finPak.tailPitch.finAngle));

    aMissile.guidance.finPak.tailPitch.chordLine = DirectX::SimpleMath::Vector3::UnitX;
    aMissile.guidance.finPak.tailPitch.chordLine = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.finPak.tailPitch.chordLine,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_finLib.tailPitch.axis, aMissile.guidance.finPak.tailPitch.finAngle));

    aMissile.guidance.finPak.tailYaw.finDir = m_finLib.tailYaw.finNormal;
    aMissile.guidance.finPak.tailYaw.finDir = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.finPak.tailYaw.finDir,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_finLib.tailYaw.axis, aMissile.guidance.finPak.tailYaw.finAngle));

    aMissile.guidance.finPak.tailYaw.chordLine = DirectX::SimpleMath::Vector3::UnitX;
    aMissile.guidance.finPak.tailYaw.chordLine = DirectX::SimpleMath::Vector3::Transform(aMissile.guidance.finPak.tailYaw.chordLine,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_finLib.tailYaw.axis, aMissile.guidance.finPak.tailYaw.finAngle));

    aMissile.guidance.finPak.tailPitch.controlInput = GetAeroTensorControlInput(aMissile.guidance.finPak.tailPitch.finAngle, -m_missileConsts.steerAngMax, m_missileConsts.steerAngMax);
    aMissile.guidance.finPak.tailYaw.controlInput = GetAeroTensorControlInput(aMissile.guidance.finPak.tailYaw.finAngle, -m_missileConsts.steerAngMax, m_missileConsts.steerAngMax);

    aMissile.guidance.finPak.tailPitch.aeroTensor = GetAeroTensor(aMissile.guidance.finPak.tailPitch, m_finLib.tailPitch);
    aMissile.guidance.finPak.tailYaw.aeroTensor = GetAeroTensor(aMissile.guidance.finPak.tailYaw, m_finLib.tailYaw);

    aMissile.guidance.finPak.mainPitch.finAngle = 0.0f;
    aMissile.guidance.finPak.mainYaw.finAngle = 0.0f;

    aMissile.guidance.finPak.mainPitch.finDir = m_finLib.mainPitch.finNormal;
    aMissile.guidance.finPak.mainYaw.finDir = m_finLib.mainYaw.finNormal;

    aMissile.guidance.finPak.mainPitch.chordLine = DirectX::SimpleMath::Vector3::UnitX;
    aMissile.guidance.finPak.mainYaw.chordLine = DirectX::SimpleMath::Vector3::UnitX;
}

void FireControl::UpdateFinData(MissileData& aMissile)
{
    UpdateFinAngles(aMissile);

    //UpdateFinForces(m_finLib.canardPitch, aMissile.guidance.finPak.canardPitch, aMissile);
    //UpdateFinForces(m_finLib.canardYaw, aMissile.guidance.finPak.canardYaw, aMissile);

    //UpdateFinForces(m_finLib.tailPitch, aMissile.guidance.finPak.tailPitch, aMissile);
    UpdateFinForces(m_finLib.tailYaw, aMissile.guidance.finPak.tailYaw, aMissile);

    //UpdateFinForces(m_finLib.mainPitch, aMissile.guidance.finPak.mainPitch, aMissile);
    UpdateFinForces(m_finLib.mainYaw, aMissile.guidance.finPak.mainYaw, aMissile);

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::UnitX, aMissile.guidance.localVel, DirectX::Colors::Yellow, false, true);
}

void FireControl::UpdateFinForces(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const MissileData& aMissile)
{
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->ToggleDebugOff();
    m_debugData->DebugPushUILineDecimalNumber(aStaticDat.name, 0.0f, "");
    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.wingArea   = ", aStaticDat.wingArea, "");
    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.frontArea  = ", aStaticDat.frontArea, "");
    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.chord      = ", aStaticDat.chord, "");
    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.span       = ", aStaticDat.span, "");
    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.thickness  = ", aStaticDat.thickness, "");

    m_debugData->DebugPushUILineDecimalNumber("aStaticDat.thickness /aStaticDat.chord = ", aStaticDat.thickness  / aStaticDat.chord, "");
    m_debugData->ToggleDebugOff();

    auto airSpeedLocal = aMissile.guidance.localVel * -1.0f;
    auto airSpeedLocalNorm = airSpeedLocal;
    airSpeedLocalNorm.Normalize();

    auto finAxis = aStaticDat.axis;
    auto finAngle = aFinDyn.finAngle;

    auto testPos = aStaticDat.posLocal;
    float aoaToUse = Utility::ToRadians(0.0f);
    auto chordLineAirVec = airSpeedLocalNorm;
    if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW || aStaticDat.finType == FinType::CANARD_YAW)
    {
        chordLineAirVec.y = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.y < 0.0f)
        {
            aoaToUse *= -1.0f;
        }

        testPos.y += aStaticDat.span * 0.5f;
        //DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitZ, DirectX::Colors::Red, false, true);
        /*
        testPos.x += aStaticDat.chord * 0.5f;
        DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitZ, DirectX::Colors::Blue, false, true);
        testPos.x -= aStaticDat.chord;
        DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitZ, DirectX::Colors::Green, false, true);
        */
        testPos.z += aStaticDat.thickness * 0.5f;
        //DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitY, DirectX::Colors::Blue, false, true);
        testPos.z -= aStaticDat.thickness;
        //DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitY, DirectX::Colors::Green, false, true);


    }
    else
    {
        chordLineAirVec.z = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.z < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
        testPos.z += aStaticDat.span * 0.5f;
        //DebugPushDrawData(testPos, DirectX::SimpleMath::Vector3::UnitY, DirectX::Colors::Red, false, true);
    }






    float angleOfAttack = aoaToUse;
    
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttack    = ", Utility::ToDegrees(angleOfAttack), "");
    m_debugData->DebugPushUILineDecimalNumber("aFinDyn.finAngle = ", Utility::ToDegrees(aFinDyn.finAngle), "");
    m_debugData->ToggleDebugOff();

    const float cl = CalculateFinLiftCoefFlat(angleOfAttack);
    //const float cl = CalculateFinLiftCoefTest(angleOfAttack);
    //const float cl = CalculateFinLiftCoef(angleOfAttack);
    //const float cl = CalculateFinLiftCoefDebug(angleOfAttack);
    //const float cl = m_missileConsts.finClConst;
    
    const float surface = aStaticDat.span * aStaticDat.chord;
    const float rho = m_environment->GetAirDensity();

    // lift = coefficient * density * (vel^2 / two) * wing area
    auto lift = cl * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * surface;

    float liftLength = lift.Length();
    //auto liftTest = cl * rho * ((airSpeedLocal.Length() * airSpeedLocal.Length()) / 2.0f) * surface;

    auto axisTest = aStaticDat.axis;
    if (aFinDyn.finDir.Dot(airSpeedLocalNorm) > 0.0f)
    {
        axisTest = aStaticDat.axis;
    }
    else
    {
        axisTest = -aStaticDat.axis;
    }

    auto liftNormTest = airSpeedLocalNorm.Cross(axisTest);
    liftNormTest.Normalize();

    aFinDyn.liftForce = liftNormTest * lift.Length();

    auto airImpactDot = abs(aFinDyn.finDir.Dot(airSpeedLocalNorm));

    const float dragSurfaceBase = (surface * 0.5f) * 0.7f; // avg'ing data pulled from wiki
    const float dragSufaceAddMax = surface - dragSurfaceBase;

    //const float dragSurface = dragSurfaceBase + (dragSufaceAddMax * abs(airImpactDot));
    const float dragSurface = CalculateFinDragArea(airSpeedLocalNorm, aFinDyn.finDir, aStaticDat);;

    const float dragCoefBase = aStaticDat.dragCoeffBase;
    const float dragModMax = aStaticDat.dragCoeefMod;
    const float cd = dragCoefBase + (dragModMax * airImpactDot);
    auto finDrag = cd * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * dragSurface;
    //auto lift2   = cl * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * surface;
    aFinDyn.dragForce = airSpeedLocalNorm * finDrag.Length();

    aFinDyn.resultantForce = aFinDyn.liftForce + aFinDyn.dragForce;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("liftForce      = ", aFinDyn.liftForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("dragForce      = ", aFinDyn.dragForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("resultantForce = ", aFinDyn.resultantForce.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("cl = ", cl, "");
    m_debugData->DebugPushUILineDecimalNumber("cd = ", cd, "");
    m_debugData->DebugPushUILineDecimalNumber("cl/cd Ratio    = ",  cl / cd, "");
    m_debugData->DebugPushUILineDecimalNumber("L/D Ratio      = ", aFinDyn.liftForce.Length() / aFinDyn.dragForce.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("SUM Ratio      = ", (cl / cd) / (aFinDyn.liftForce.Length() / aFinDyn.dragForce.Length()), "");

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("dragForce      = ", aFinDyn.dragForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("cd = ", cd, "");
    m_debugData->DebugPushUILineDecimalNumber("dragSurface = ", dragSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("surface     = ", surface, "");
    m_debugData->ToggleDebugOff();

    /*
    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.liftForce, DirectX::Colors::Lime, false, true);
    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.dragForce, DirectX::Colors::DeepPink, false, true);
    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.resultantForce, DirectX::Colors::RoyalBlue, false, true);

    DebugPushDrawData(DirectX::SimpleMath::Vector3::UnitY, airSpeedLocalNorm, DirectX::Colors::RoyalBlue, false, true);
    */
}

void FireControl::UpdateFinForcesOld(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const MissileData& aMissile)
{
    auto airSpeedLocal = aMissile.guidance.localVel * -1.0f;
    auto airSpeedLocalNorm = airSpeedLocal;
    airSpeedLocalNorm.Normalize();

    auto finAxis = aStaticDat.axis;
    auto finAngle = aFinDyn.finAngle;

    float aoaToUse = Utility::ToRadians(0.0f);
    auto chordLineAirVec = airSpeedLocalNorm;
    if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW)
    {
        chordLineAirVec.y = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.y < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }
    else
    {
        chordLineAirVec.z = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.z < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }

    float angleOfAttack = aoaToUse;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttack = ", Utility::ToDegrees(angleOfAttack), "");
    m_debugData->DebugPushUILineDecimalNumber("aFinDyn.finAngle = ", Utility::ToDegrees(aFinDyn.finAngle), "");

    m_debugData->ToggleDebugOff();

    const float cl = CalculateFinLiftCoefFlat(angleOfAttack);
    //const float cl = CalculateFinLiftCoefTest(angleOfAttack);
    //const float cl = CalculateFinLiftCoef(angleOfAttack);
    //const float cl = CalculateFinLiftCoefDebug(angleOfAttack);
    //const float cl = m_missileConsts.finClConst;

    const float surface = aStaticDat.span * aStaticDat.chord;
    const float rho = m_environment->GetAirDensity();

    // lift = coefficient * density * (vel^2 / two) * wing area
    auto lift = cl * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * surface;

    float liftLength = lift.Length();
    //auto liftTest = cl * rho * ((airSpeedLocal.Length() * airSpeedLocal.Length()) / 2.0f) * surface;

    auto axisTest = aStaticDat.axis;
    if (aFinDyn.finDir.Dot(airSpeedLocalNorm) > 0.0f)
    {
        axisTest = aStaticDat.axis;
    }
    else
    {
        axisTest = -aStaticDat.axis;
    }

    auto liftNormTest = airSpeedLocalNorm.Cross(axisTest);
    liftNormTest.Normalize();
    //auto finLiftVec = finNorm * lift.Length();
    auto finLiftVec = liftNormTest * lift.Length();
    //auto finLiftVec = liftNorm * lift.Length();
    aFinDyn.liftForce = finLiftVec;

    float testArea = CalculateFinDragArea(airSpeedLocalNorm, aFinDyn.finDir, aStaticDat);

    auto airImpactDot = abs(aFinDyn.finDir.Dot(airSpeedLocalNorm));

    const float dragSurfaceBase = (surface * 0.5f) * 0.7f; // avg'ing data pulled from wiki
    const float dragSufaceAddMax = surface - dragSurfaceBase;

    //const float dragSurface = dragSurfaceBase + (dragSufaceAddMax * abs(airImpactDot));
    const float dragSurface = testArea;

    const float dragCoefBase = aStaticDat.dragCoeffBase;
    const float dragModMax = aStaticDat.dragCoeefMod;
    const float cd = dragCoefBase + (dragModMax * airImpactDot);
    auto finDrag = cd * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * dragSurface;

    auto finDragVec = finDrag;

    auto testFinDrag = airSpeedLocalNorm * finDrag.Length();
    aFinDyn.dragForce = testFinDrag;

    auto resultantForce = finLiftVec + testFinDrag;
    aFinDyn.resultantForce = aFinDyn.liftForce + aFinDyn.dragForce;


    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("liftForce      = ", aFinDyn.liftForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("dragForce      = ", aFinDyn.dragForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("resultantForce = ", aFinDyn.resultantForce.Length(), "");
    m_debugData->ToggleDebugOff();

    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.liftForce, DirectX::Colors::Lime, false, true);
    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.dragForce, DirectX::Colors::DeepPink, false, true);
    DebugPushDrawData(aStaticDat.posLocal, aFinDyn.resultantForce, DirectX::Colors::RoyalBlue, false, true);
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

    if (m_isManualInputDecayTrue == true)
    {
        m_manualThrustVecPitch = ManualInputDecay(m_manualThrustVecPitch, static_cast<float>(aTimeDelta));
        m_manualThrustVecYaw = ManualInputDecay(m_manualThrustVecYaw, static_cast<float>(aTimeDelta));

        m_manualTailPitch = ManualInputDecay(m_manualTailPitch, static_cast<float>(aTimeDelta));
        m_manualTailYaw = ManualInputDecay(m_manualTailYaw, static_cast<float>(aTimeDelta));

        m_manualCanardPitch = ManualInputDecay(m_manualCanardPitch, static_cast<float>(aTimeDelta));
        m_manualCanardYaw = ManualInputDecay(m_manualCanardYaw, static_cast<float>(aTimeDelta));

        ManualInputDecayVector(static_cast<float>(aTimeDelta));
    }
}

void FireControl::UpdateFlightStateData(MissileData& aMissile, const double aTimeDelta)
{
    aMissile.projectileData.time += static_cast<float>(aTimeDelta);
    aMissile.guidance.timeStepDelta = static_cast<float>(aTimeDelta);
    float terrainHeightAtPos = m_environment->GetTerrainHeightAtPos(aMissile.projectileData.q.position);
    aMissile.guidance.altitude = aMissile.projectileData.q.position.y - terrainHeightAtPos;

    m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity, aMissile.guidance.targetForward);
    float prevDistance = aMissile.guidance.targetDistance;
    aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();
    aMissile.guidance.targetDistanceDelta = prevDistance - aMissile.guidance.targetDistance;
    aMissile.guidance.closureRate = aMissile.guidance.targetDistanceDelta / aTimeDelta;
    aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;

    //AltitudeController(aMissile, aTimeDelta);

    auto localVelocity = aMissile.projectileData.q.velocity;
    localVelocity = DirectX::SimpleMath::Vector3::Transform(localVelocity, aMissile.projectileData.inverseAlignmentQuat);

    aMissile.guidance.localVel = localVelocity;

    if (localVelocity.x > 0.0f)
    {
        aMissile.guidance.isVelocityForward = true;
    }
    else
    {
        aMissile.guidance.isVelocityForward = false;
    }

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
                    aMissile.guidance.isFinsDeployEnd = true;
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
            /*
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CLIMBOUT;
            aMissile.guidance.isFinsDeployEnd = true;
            aMissile.guidance.finDeployPercent = 1.0f;
            */
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
        const DirectX::SimpleMath::Vector3 thrustPosOffset = m_missileConsts.thrustPosLocalOffset;
        DirectX::SimpleMath::Vector3 centerOfThrust = m_missileConsts.thrustPosLocal;
        //centerOfThrust += thrustPosOffset * thrustAnglePercent;
        aMissile.guidance.centerOfThrustLocalPos = centerOfThrust;

        DirectX::SimpleMath::Vector3 centerOfPressure = m_missileConsts.centerOfPressureBasePosLocal;
        centerOfPressure += m_missileConsts.centerOfPressureFullFinDeployOffset * aMissile.guidance.finDeployPercent;
        aMissile.guidance.centerOfPressureLocalPos = centerOfPressure;
    }
}

void FireControl::UpdateLOSData(MissileData& aMissile, const float aTimeDelta)
{
    aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    DirectX::SimpleMath::Vector3 currentLos = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    aMissile.guidance.losCurrent = currentLos;
    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;

    aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losRate = losDelta.Length();
    //aMissile.guidance.closingVelocityToTarget = -aMissile.guidance.losRate;
    aMissile.guidance.closingVelocityToTarget = aMissile.guidance.targetDistanceDelta / aTimeDelta;

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;

    //DirectX::SimpleMath::Vector3 los = (aMissile.guidance.targetPosition + aMissile.guidance.targetVelocity * aMissile.guidance.navigationTime) - aMissile.projectileData.q.position;
    //DirectX::SimpleMath::Vector3 los = (aMissile.guidance.targetPosition * aMissile.guidance.navigationTime) - aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 los = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;

    float angle = Utility::GetAngleBetweenVectors(aMissile.projectileData.q.velocity, los);
    DirectX::SimpleMath::Vector3 losNorm = los;
    losNorm.Normalize();
    DirectX::SimpleMath::Vector3 adjustment = m_missileConsts.navigationGain * angle * losNorm;
    adjustment = DirectX::SimpleMath::Vector3::Transform(adjustment, aMissile.projectileData.inverseAlignmentQuat);

    aMissile.guidance.pnAdjusted = adjustment;

    //////////////////
    // SimplifiedPN
    los = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    float navigationTime = los.Length() / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 targetRelativeInterceptPosition = los + (aMissile.guidance.targetVelocity * navigationTime);
    DirectX::SimpleMath::Vector3 desiredHeading = targetRelativeInterceptPosition;
    desiredHeading.Normalize();

    const float pValue = 3.0f;
    targetRelativeInterceptPosition *= pValue;

    DirectX::SimpleMath::Vector3 lookPos = (aMissile.guidance.targetPosition + targetRelativeInterceptPosition) - aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 lookPosLocal = lookPos;
    lookPosLocal = DirectX::SimpleMath::Vector3::Transform(lookPosLocal, aMissile.projectileData.inverseAlignmentQuat);

    DirectX::SimpleMath::Quaternion targetRot = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, lookPosLocal);
    DirectX::SimpleMath::Vector3 testVec = lookPosLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);


    DirectX::SimpleMath::Vector3 targPosLocalized = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    targPosLocalized = DirectX::SimpleMath::Vector3::Transform(targPosLocalized, aMissile.projectileData.inverseAlignmentQuat);

    auto testTarg = targPosLocalized;
    testTarg += aMissile.projectileData.q.velocity * std::min(aMissile.guidance.navigationTime, 10.0f);

    auto targPosWorldized = targPosLocalized;
    targPosWorldized = DirectX::SimpleMath::Vector3::Transform(targPosWorldized, aMissile.projectileData.alignmentQuat);
    targPosWorldized += aMissile.projectileData.q.position;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.navigationTime", aMissile.guidance.navigationTime, "");
    m_debugData->PushDebugLinePositionIndicator(targPosWorldized, 10.0f, 0.0f, DirectX::Colors::Lime);
    m_debugData->PushDebugLinePositionIndicator(targPosLocalized, 10.0f, 0.0f, DirectX::Colors::Violet);
    m_debugData->PushDebugLinePositionIndicator(targetRelativeInterceptPosition, 10.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->ToggleDebugOff();



    //destLocal = (aMissile.guidance.targetDestination - missilePosOffset) - (aMissile.projectileData.q.position);
    //auto destLocal = (aMissile.guidance.targetDestination) - (aMissile.projectileData.q.position);
    auto destLocal = testTarg;

    //float distToTargetTest = destLocal.Length();
    //destLocal.Normalize();
    //destLocal = DirectX::SimpleMath::Vector3::Transform(destLocal, aMissile.projectileData.inverseAlignmentQuat);

    DirectX::SimpleMath::Quaternion destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    destQuat.Inverse(destQuat);

    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    //m_debugData->ToggleDebugOnOverRide();
    testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);

    ////////////////////////////////////
    ////////////////////////////////////



    // target velocity relative to missile
    //vec2 Vr = Vt - Vm;
    DirectX::SimpleMath::Vector3 Vr = aMissile.guidance.targetVelocity - aMissile.projectileData.q.velocity;
    // range from missile to target
    //vec2 R = Rt - Rm;
    DirectX::SimpleMath::Vector3 R = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    //some constant
    //float N = ...;
    float N = 3.0f;
    //rotation vector of line of sight
    //vec3 Omega = cross(R, Vr) / dot(R, R);
    DirectX::SimpleMath::Vector3 Omega = R.Cross(Vr) / R.Dot(R);

    // the cross product of orthogonal vector to R and Vr from previous cross
    // product, and Vr again, will produce vector which lies on the plane of R 
    // and Vr again, hence we can ignore the last dimension of the cross product 
    // it will be zero anyway for 2d.
    //vec2 a = N * cross(Vr, Omega);
    DirectX::SimpleMath::Vector3 a = N * Vr.Cross(Omega);
    DirectX::SimpleMath::Vector3 aCrossForward = a.Cross(aMissile.projectileData.forward);
    DirectX::SimpleMath::Vector3 aCrossCross = aCrossForward.Cross(a);

    DirectX::SimpleMath::Vector3 aCrossCrossLocal = aCrossCross;
    aCrossCrossLocal = DirectX::SimpleMath::Vector3::Transform(aCrossCrossLocal, aMissile.projectileData.inverseAlignmentQuat);

    destLocal = aCrossCrossLocal;
    destQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, destLocal);
    destQuat.Normalize();
    // destQuat.Inverse(destQuat);

    testVec = DirectX::SimpleMath::Vector3::UnitX;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, destQuat);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);
    //aMissile.guidance.headingLocalVecTest = destLocal;
    //aMissile.guidance.headingLocalQuatTest = destQuat;

    DirectX::SimpleMath::Vector3 headingLocalVecTestWorld = aMissile.guidance.headingLocalVecTest;
    headingLocalVecTestWorld = DirectX::SimpleMath::Vector3::Transform(headingLocalVecTestWorld, aMissile.projectileData.alignmentQuat);
    m_debugData->ToggleDebugOnOverRide();

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 10.0f, 0.0f, DirectX::Colors::Lime);
    /*
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, a, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aCrossForward, 10.0f, 0.0f, DirectX::Colors::Violet);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aCrossCross, 10.0f, 0.0f, DirectX::Colors::Lime);
    */
    m_debugData->DebugPushUILineDecimalNumber("a.l = ", a.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("a.D = ", Utility::ToDegrees(a.Length()), "");
    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.headingLocalVecTest, 8.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingLocalVecTestWorld, 8.0f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->ToggleDebugOff();
}

void FireControl::UpdateLOSData2(MissileData& aMissile, const float aTimeDelta)
{
    aMissile.guidance.navigationTime = ((aMissile.guidance.targetPosition - aMissile.projectileData.q.position).Length()) / aMissile.projectileData.q.velocity.Length();
    DirectX::SimpleMath::Vector3 prevLos = aMissile.guidance.losCurrent;
    DirectX::SimpleMath::Vector3 currentLos = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    aMissile.guidance.losCurrent = currentLos;
    prevLos.Normalize();
    currentLos.Normalize();
    DirectX::SimpleMath::Vector3 losDelta = currentLos - prevLos;

    aMissile.guidance.losDelta = losDelta;
    aMissile.guidance.losRate = losDelta.Length();
    //aMissile.guidance.closingVelocityToTarget = -aMissile.guidance.losRate;
    aMissile.guidance.closingVelocityToTarget = aMissile.guidance.targetDistanceDelta / aTimeDelta;

    // latax = RTM_new * N * Vc * LOS_Rate + LOS_Delta
    aMissile.guidance.latax = aMissile.guidance.losCurrent * m_missileConsts.navigationGain * aMissile.guidance.closingVelocityToTarget * aMissile.guidance.losDelta;

    //m_debugData->ToggleDebugOnOverRide();

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.losCurrent, aMissile.guidance.losCurrent.Length(), 0.0f, DirectX::Colors::Lime);
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.losCurrent.latax = ", aMissile.guidance.losCurrent.Length(), "");

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.latax, 5.0f, 0.0f, DirectX::Colors::DarkOrange);
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax = ", aMissile.guidance.latax.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax.x = ", aMissile.guidance.latax.x, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax.y = ", aMissile.guidance.latax.y, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.latax.z = ", aMissile.guidance.latax.z, "");


    m_debugData->ToggleDebugOff();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.losDelta, 4.0f, 0.0f, DirectX::Colors::Violet);
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.losDelta = ", aMissile.guidance.losDelta.Length(), "");

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.closingVelocityToTarget = ", aMissile.guidance.closingVelocityToTarget, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.losRate = ", aMissile.guidance.losRate, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targetDistanceDelta = ", aMissile.guidance.targetDistanceDelta, "");

    float testRate = aMissile.guidance.targetDistanceDelta * aTimeDelta;
    //m_debugData->DebugPushUILineDecimalNumber("testRate = ", testRate, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targetDistanceDelta * aTimeDelta = ", aMissile.guidance.targetDistanceDelta * aTimeDelta, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targetDistanceDelta = ", aMissile.guidance.targetDistanceDelta, "");
    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targetDistanceDelta / aTimeDelta = ", aMissile.guidance.targetDistanceDelta / aTimeDelta, "");
    //m_debugData->DebugPushUILineDecimalNumber("aMissile.projectileData.q.velocity = ", aMissile.projectileData.q.velocity.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber(" = ", , "");



    //DirectX::SimpleMath::Vector3 los = (aMissile.guidance.targetPosition + aMissile.guidance.targetVelocity * aMissile.guidance.navigationTime) - aMissile.projectileData.q.position;
    //DirectX::SimpleMath::Vector3 los = (aMissile.guidance.targetPosition * aMissile.guidance.navigationTime) - aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 los = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;

    m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.targetVelocity.length() = ", aMissile.guidance.targetVelocity.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("los.Length() = ", los.Length(), "");
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, los, 7.0f, 0.0f, DirectX::Colors::Blue);
    float angle = Utility::GetAngleBetweenVectors(aMissile.projectileData.q.velocity, los);
    DirectX::SimpleMath::Vector3 losNorm = los;
    losNorm.Normalize();
    DirectX::SimpleMath::Vector3 adjustment = m_missileConsts.navigationGain * angle * losNorm;
    //adjustment = DirectX::SimpleMath::Vector3::Transform(adjustment, aMissile.projectileData.inverseAlignmentQuat);
    adjustment = DirectX::SimpleMath::Vector3::Transform(adjustment, aMissile.projectileData.inverseAlignmentQuat);


    m_debugData->PushDebugLine(aMissile.projectileData.q.position, adjustment, 5.0f, 0.0f, DirectX::Colors::DarkOrange);
    m_debugData->DebugPushUILineDecimalNumber("adjustment.L = ", adjustment.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("adjustment.x = ", adjustment.x, "");
    m_debugData->DebugPushUILineDecimalNumber("adjustment.y = ", adjustment.y, "");
    m_debugData->DebugPushUILineDecimalNumber("adjustment.z = ", adjustment.z, "");

    //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.steeringDirNormLocal, 4.0f, 0.05f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.headingLocalVecTest, 4.0f, 0.05f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 testVec1 = adjustment;
    testVec1.Normalize();
    DirectX::SimpleMath::Vector3 testVec2 = aMissile.guidance.headingLocalVecTest;
    testVec2.Normalize();

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec1, 6.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec2, 4.0f, 0.0f, DirectX::Colors::Blue);

    m_debugData->ToggleDebugOff();

    aMissile.guidance.pnAdjusted = adjustment;

    //////////////////
    // SimplifiedPN
    los = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
    float navigationTime = los.Length() / aMissile.projectileData.q.velocity.Length();

    //DirectX::SimpleMath::Vector3 targetRelativeInterceptPosition = los + (targetRb.velocity * navigationTime);
    DirectX::SimpleMath::Vector3 targetRelativeInterceptPosition = los + (aMissile.guidance.targetVelocity * navigationTime);
    DirectX::SimpleMath::Vector3 desiredHeading = targetRelativeInterceptPosition;
    desiredHeading.Normalize();

    float pValue = 3.0f;
    /*
    if (aMissile.guidance.uniqueId % 2 == 0)
    {
        if (aMissile.guidance.closureRate <= 0.0f)
        {
            pValue = 1.0f;
        }
        else if (aMissile.guidance.closureRate <= 2.0f)
        {
            pValue = 2.0f;
        }
        else if (aMissile.guidance.closureRate <= 10.0f)
        {
            pValue = 3.0f;
        }
        else if (aMissile.guidance.closureRate <= 40.0f)
        {
            pValue = 4.0f;
        }
    }
    */
    targetRelativeInterceptPosition *= pValue;

    DirectX::SimpleMath::Vector3 lookPos = (aMissile.guidance.targetPosition + targetRelativeInterceptPosition) - aMissile.projectileData.q.position;

    DirectX::SimpleMath::Vector3 lookPosLocal = lookPos;
    lookPosLocal = DirectX::SimpleMath::Vector3::Transform(lookPosLocal, aMissile.projectileData.inverseAlignmentQuat);

    DirectX::SimpleMath::Quaternion targetRot = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, lookPosLocal);

    //DirectX::SimpleMath::Vector3 testVec = aMissile.guidance.headingLocalVecTest;
    DirectX::SimpleMath::Vector3 testVec = lookPosLocal;
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOnOverRide();

    m_debugData->PushDebugLine(aMissile.projectileData.q.position, lookPos, 5.0f, 0.0f, DirectX::Colors::DarkOrange);
    m_debugData->DebugPushUILineDecimalNumber("lookPos.L = ", lookPos.Length(), "");
    /*
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, lookPosLocal, 5.0f, 0.0f, DirectX::Colors::DarkOrange);
    m_debugData->DebugPushUILineDecimalNumber("lookPosLocal.L = ", lookPosLocal.Length(), "");
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, testVec, 4.0f, 0.05f, DirectX::Colors::Blue);
    */
    m_debugData->ToggleDebugOff();

    //aMissile.guidance.headingLocalVecTest = lookPosLocal;
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
    auto prevQuat = aMissile.projectileData.alignmentQuat;

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


    auto testAngRads = DirectX::SimpleMath::Quaternion::Angle(prevQuat, aMissile.projectileData.alignmentQuat);
    auto testAngDegs = Utility::ToDegrees(testAngRads);

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("testAngDegs per sec", testAngDegs / aTimeDelta, "");

    m_debugData->ToggleDebugOff();
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


    DirectX::SimpleMath::Vector3 dragLinear = CalculateDragLinearSum(aMissile, aTimeDelta);
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
        CruiseGuidance(aMissile, aTimeDelta);
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_ATTACK)
    {
    }
    else if (aMissile.guidance.flightStateCurrent == FlightState::FLIGHTSTATE_EXPLODING)
    {
    }
}   

void FireControl::UpdateMissileModelData(MissileData& aMissile)
{
    if (aMissile.guidance.flickerBool == true)
    {
        aMissile.guidance.flickerBool = false;
    }
    else
    {
        aMissile.guidance.flickerBool = true;
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
    //const DirectX::SimpleMath::Vector3 toUseSteeringVecLocal = aMissile.guidance.steeringDirNormLocal;
    //const DirectX::SimpleMath::Quaternion toUseSteeringQuat = aMissile.guidance.steeringQuat;
    const DirectX::SimpleMath::Vector3 toUseSteeringVecLocal = aMissile.guidance.conDat.thrustVecNorm;
    const DirectX::SimpleMath::Quaternion toUseSteeringQuat = aMissile.guidance.conDat.thrustVecQuat;

    DirectX::SimpleMath::Matrix sphereMat = DirectX::SimpleMath::Matrix::Identity;
    float plumeBaseScale = 0.0f;
    float scale = 1.0f;
    float plumeConeScale = 0.0f;
    if (aMissile.guidance.flickerBool == true && 1 == 0)
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
        auto prevVelocity = m_missileVec[i].projectileData.q.velocity;

        //DebugGraphCurveData(m_missileVec[i], aTimeDelta);

        //UpdateFlightStateData(m_missileVec[i], aTimeDelta);
        UpdateFlightDataDependantVars(m_missileVec[i], aTimeDelta);
        UpdateFlightState(m_missileVec[i], aTimeDelta);
        UpdateFlightDataIndependentVars(m_missileVec[i], aTimeDelta);




        UpdateMissileGuidance(m_missileVec[i], static_cast<float>(aTimeDelta));
        //UpdateLOSData(m_missileVec[i], static_cast<float>(aTimeDelta));

        //ProNavTest(m_missileVec[i], static_cast<float>(aTimeDelta));

        //ControllerUpdate(m_missileVec[i], static_cast<float>(aTimeDelta));
        //UpdateSteeringDirNorm(m_missileVec[i], static_cast<float>(aTimeDelta));

        //ProNavOpenSteer(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateNavData(m_missileVec[i], static_cast<float>(aTimeDelta));
        //UpdateAngularStability(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateControlData(m_missileVec[i], static_cast<float>(aTimeDelta));

        UpdateThrustVector(m_missileVec[i]);
        UpdateFinData(m_missileVec[i]);
        //BoosterSteeringUpdate(m_missileVec[i]);
        AccumulateMissileForces(m_missileVec[i], static_cast<float>(aTimeDelta));
       
        //DebugMissileFunc(&m_missileVec[i]);

        RungeKutta4Missile(&m_missileVec[i], aTimeDelta);

        DebugSetMissileToPlayerPos(m_missileVec[i]);

        UpdateMissileAlignment(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateMissileModelData(m_missileVec[i]);

        if (m_missileConsts.isContrailsOn == true)
        {
            UpdateContrails(m_missileVec[i]);
            DrawContrails(m_missileVec[i]);
        }

        PrintFlightStateData(m_missileVec[i]);
        PrintMissileData(m_missileVec[i], static_cast<float>(aTimeDelta));
        ResetMissileForceAccumulators(m_missileVec[i]);

        auto postVelocity = m_missileVec[i].projectileData.q.velocity;
        auto deltaVelocity = (postVelocity - prevVelocity) / aTimeDelta;
        auto velocityNorm = m_missileVec[i].projectileData.q.velocity;
        velocityNorm.Normalize();

        //m_debugData->ToggleDebugOnOverRide();
        //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.q.velocity, 4.0f, 0.0f, DirectX::Colors::LimeGreen);
        m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, velocityNorm, 4.0f, 0.0f, DirectX::Colors::Coral);
        //m_debugData->DebugPushUILineDecimalNumber("deltaVelocity = ", deltaVelocity.Length(), "");
        m_debugData->ToggleDebugOff();

        //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, -m_missileVec[i].projectileData.q.velocity, DirectX::Colors::Yellow, false, false);


        //auto postVelocity = m_missileVec[i].projectileData.q.velocity;
        auto velDelta = prevVelocity - postVelocity;
        auto deltaVelocity2 = (postVelocity.Length() - prevVelocity.Length()) / aTimeDelta;
        float speed = m_missileVec[i].projectileData.q.velocity.Length();
        float mph = speed * 2.237f;

        m_debugData->ToggleDebugOnOverRide();
        //m_debugData->DebugPushUILineDecimalNumber("mph   = ", mph, "");
        m_debugData->DebugPushUILineWholeNumber("mph = ", static_cast<int>(mph), "");
        m_debugData->DebugPushUILineDecimalNumber("deltaVelocity ", deltaVelocity2, "");
        m_debugData->ToggleDebugOff();

        DebugDrawUpdate(m_missileVec[i]);

        m_debugDrawVec.clear();
    }

    //CheckCollisionsMissile();

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

void FireControl::UpdateNavData(MissileData& aMissile, const float aTimeDelta)
{
    if (m_missileConsts.isManualControlTrue == true)
    {
        GuidanceManualVector(aMissile, aTimeDelta);
    }
    else
    {
        //GuidanceBasicGravity(aMissile, aTimeDelta);
        //GuidanceBasic(aMissile, aTimeDelta);
        GuidanceTest(aMissile, aTimeDelta);
        //GuidanceVelocitySteeringTest(aMissile, aTimeDelta);
        
        
        // 
        //ProNavTest(aMissile, aTimeDelta);
        //ProNav(aMissile, aTimeDelta);
        //ProNav2(aMissile, aTimeDelta);
        //ProNav3(aMissile, aTimeDelta);
        //ProNav4(aMissile, aTimeDelta);
        //ProNav5(aMissile, aTimeDelta);
        //ProNavOpenSteer(aMissile, aTimeDelta);
        //CruiseGuidance(aMissile, aTimeDelta);
    }
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
        auto headingWorld = aMissile.guidance.headingLocalVecTest;
        headingWorld = DirectX::SimpleMath::Vector3::Transform(headingWorld, aMissile.projectileData.alignmentQuat);

        auto headingWorld2 = DirectX::SimpleMath::Vector3::UnitX;
        headingWorld2 = DirectX::SimpleMath::Vector3::Transform(headingWorld2, aMissile.guidance.headingLocalQuatTest);
        headingWorld2 = DirectX::SimpleMath::Vector3::Transform(headingWorld2, aMissile.projectileData.alignmentQuat);

        //m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.x ", aMissile.guidance.headingLocalVecTest.x, "");
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.y ", aMissile.guidance.headingLocalVecTest.y, "");
        m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.headingLocalVecTest.z ", aMissile.guidance.headingLocalVecTest.z, "");
        
        m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingWorld, 5.0f, 0.0f, DirectX::Colors::Lime);
        m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingWorld2, 4.0f, 0.2f, DirectX::Colors::SeaGreen);

        
        m_debugData->ToggleDebugOff();
        DirectX::SimpleMath::Vector3 destLocal = aMissile.guidance.headingLocalVecTest;
        DirectX::SimpleMath::Quaternion destQuat = aMissile.guidance.headingLocalQuatTest;

        DirectX::SimpleMath::Vector3 steeringLine = -DirectX::SimpleMath::Vector3::UnitX;
        steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, destQuat);
        //steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, aMissile.projectileData.alignmentQuat);

        aMissile.guidance.controller.currentQuatRaw = destQuat;
        aMissile.guidance.controller.currentVecRaw = steeringLine;

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

        aMissile.guidance.controller.currentQuat = updateQuat;
        aMissile.guidance.controller.currentVec = steeringUpdateVec;
        aMissile.guidance.controller.currentControlVal = aMissile.guidance.thrustAngle;

        aMissile.guidance.controller.currentQuatConed = updateQuat;
        aMissile.guidance.controller.currentVecConed = steeringUpdateVec;


        headingWorld = aMissile.guidance.steeringDirNormLocal;
        headingWorld = DirectX::SimpleMath::Vector3::Transform(headingWorld, aMissile.projectileData.alignmentQuat);

        headingWorld2 = DirectX::SimpleMath::Vector3::UnitX;
        headingWorld2 = DirectX::SimpleMath::Vector3::Transform(headingWorld2, aMissile.guidance.steeringQuat);
        float angle2 = Utility::GetAngleBetweenVectors(headingWorld, -DirectX::SimpleMath::Vector3::UnitX);
        headingWorld2 = DirectX::SimpleMath::Vector3::Transform(headingWorld2, aMissile.projectileData.alignmentQuat);

        //m_debugData->ToggleDebugOnOverRide();
        float angle1 = Utility::GetAngleBetweenVectors(aMissile.guidance.steeringDirNormLocal, -DirectX::SimpleMath::Vector3::UnitX);
        m_debugData->DebugPushUILineDecimalNumber("angle1 =", Utility::ToDegrees(angle1),"");
        m_debugData->DebugPushUILineDecimalNumber("angle2 = ", Utility::ToDegrees(angle2), "");

        m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingWorld, 5.0f, 0.0f, DirectX::Colors::Orange);
        m_debugData->PushDebugLine(aMissile.projectileData.q.position, headingWorld2, 4.0f, 0.4f, DirectX::Colors::White);


        float testVal1 = -powf(m_missileConsts.angularDragMod, 1.0f);
        float testVal2 = -powf(m_missileConsts.angularDragMod, aTimeDelta);

        testVal1 = -powf(0.8f, aTimeDelta);
        testVal2 = -powf(0.1f, aTimeDelta);

        //DirectX::SimpleMath::Vector3 dragTorqueTest = aProjectile->projectileData.q.angularVelocity * -powf(m_missileConsts.angularDragMod, 1.0f);
        //m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("testVal1 =", testVal1, "");
        m_debugData->DebugPushUILineDecimalNumber("testVal2 = ", testVal2, "");

        m_debugData->ToggleDebugOff();
    }
    //aMissile.guidance.steeringDirNormLocal = -DirectX::SimpleMath::Vector3::UnitX;
    //aMissile.guidance.steeringQuat = DirectX::SimpleMath::Quaternion::Identity;
}

void FireControl::ZeroMissileVelocities()
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        m_missileVec[i].projectileData.q.velocity = DirectX::SimpleMath::Vector3::Zero;
        m_missileVec[i].projectileData.q.angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    }
}

DirectX::SimpleMath::Vector3 FireControl::Seek(const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargPosLocal)
{
    auto desiredVelocity = aTargPosLocal;
    desiredVelocity.Normalize();
    desiredVelocity *= m_missileConsts.velMaxEst;
    auto len = m_missileConsts.velMaxEst;
    auto rbf = m_missileConsts.rocketBoostForceMax;

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("Seek On ", 0.0f, "");
    m_debugData->DebugPushUILineDecimalNumber("desiredVelocity = ", desiredVelocity.Length(), "");
    m_debugData->ToggleDebugOff();
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, desiredVelocity, DirectX::Colors::Orange, false, true);
    DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, desiredVelocity - aVelLocal, DirectX::Colors::Red, false, true);
    return (desiredVelocity - aVelLocal);
}

DirectX::SimpleMath::Vector3 FireControl::Pursuit(MissileData& aMissile, const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargLocalPos, const DirectX::SimpleMath::Vector3 aTargLocalVel)
{
    // PBA: using velocity instead of heading, val used in ai textbook
    auto toEvader = aTargLocalPos;

    float relativeHeading = aVelLocal.Dot(aTargLocalVel);

    if (toEvader.Dot(aVelLocal) > 0.0f && (relativeHeading < -0.95f)) //acos(0.95)=18 degs
    {
        return Seek(aVelLocal, aTargLocalPos);
    }

    float lookAheadTime = toEvader.Length() / (m_missileConsts.velMaxEst + aTargLocalVel.Length());
    //lookAheadTime = 1.0f;

    auto targPosW = aTargLocalPos;
    targPosW = DirectX::SimpleMath::Vector3::Transform(targPosW, aMissile.projectileData.alignmentQuat);
    targPosW += aMissile.projectileData.q.position;

    auto targVelW = aTargLocalVel;
    targVelW = DirectX::SimpleMath::Vector3::Transform(targVelW, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("Pursuit On ", 0.0f, "");
    m_debugData->DebugPushUILineDecimalNumber("lookAheadTime = ", lookAheadTime, "");

    m_debugData->PushDebugLinePositionIndicator(targPosW, 15.0f, 0.0f, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(targPosW, targVelW, 20.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->DebugPushUILineDecimalNumber("velocity = ", aMissile.projectileData.q.velocity.Length(), "");

    m_debugData->ToggleDebugOff();

    //return Seek(aTargLocalPos + aTargLocalVel * lookAheadTime, aTargLocalPos);
    return Seek(aVelLocal, aTargLocalPos + aTargLocalVel * lookAheadTime);
}


void FireControl::UpdateAngularStability(MissileData& aMissile, const float aTimeDelta)
{
    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = aMissile.projectileData.q.angularVelocity.x;
    updateAlignQuat.y = aMissile.projectileData.q.angularVelocity.y;
    updateAlignQuat.z = aMissile.projectileData.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    //DirectX::SimpleMath::Quaternion updateAlignOutputQuat = aMissile.projectileData.alignmentQuat;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = DirectX::SimpleMath::Quaternion::Identity;
    updateAlignOutputQuat.Normalize();

    //updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * aMissile.projectileData.alignmentQuat);
    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * DirectX::SimpleMath::Quaternion::Identity);
    updateAlignOutputQuat.Normalize();

    DirectX::SimpleMath::Vector3 updateVec = aMissile.guidance.nav.vecToTargLocal;
    //DirectX::SimpleMath::Quaternion updateQuat = aMissile.guidance.headingLocalQuatTest;
    DirectX::SimpleMath::Quaternion updateQuat = aMissile.guidance.nav.quatToTarg;
    //updateQuat = DirectX::SimpleMath::Quaternion::Identity;
    const float t = 0.5f;
    updateQuat = DirectX::SimpleMath::Quaternion::Slerp(aMissile.guidance.headingLocalQuatTest, -aMissile.guidance.controller.deltaQuatRaw, t);
    //updateQuat = DirectX::SimpleMath::Quaternion::Slerp(aMissile.guidance.headingLocalQuatTest, -updateAlignOutputQuat, t);
    updateQuat = DirectX::SimpleMath::Quaternion::Slerp(aMissile.guidance.nav.quatToTarg, -updateAlignOutputQuat, t);

    updateQuat.Normalize();
    updateVec = DirectX::SimpleMath::Vector3::Transform(updateVec, updateQuat);
    updateVec.Normalize();

    if (m_isDebugAngularStabilityOn == true)
    {
        aMissile.guidance.nav.vecToTargLocal = updateVec;
        aMissile.guidance.nav.targPosLocalized = updateVec;
        aMissile.guidance.nav.quatToTarg = updateQuat;
    }
}


void FireControl::UpdateControlData(MissileData& aMissile, const float aTimeDelta)
{
    auto navVec = aMissile.guidance.nav.vecToTargLocal;
    auto navQuat = aMissile.guidance.nav.quatToTarg;

    auto finVec = aMissile.guidance.conDat.tailFinVec;
    auto finQuat = aMissile.guidance.conDat.tailFinQuat;

    auto thrustVec = aMissile.guidance.conDat.thrustVecNorm;
    auto thrustQuat = aMissile.guidance.conDat.thrustVecQuat;

    ///////////////////////////////////////////////////////
    // Tail fin control
    DirectX::SimpleMath::Vector3 steeringLine = DirectX::SimpleMath::Vector3::UnitX;
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, navQuat);
    float steeringL = steeringLine.Length();
    const float maxAng = m_missileConsts.tailFinAngMax;
    DirectX::SimpleMath::Quaternion steeringConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion steeringConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, steeringLine);
    steeringConeLimitQuat.RotateTowards(steeringConeQuat, maxAng);

    const float maxAngPerTime = aTimeDelta * m_missileConsts.tailFinAngPerSecDeltaMax;

    //DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.nav.vecToTargLocal;
    DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.conDat.tailFinVec;
    //DirectX::SimpleMath::Vector3 preVec = aMissile.guidance.conDat.thrustVecNorm;
    //DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.nav.quatToTarg;
    //DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.conDat.thrustVecQuat;
    DirectX::SimpleMath::Quaternion preQuat = aMissile.guidance.conDat.tailFinQuat;
    DirectX::SimpleMath::Quaternion updateQuat = preQuat;
    updateQuat.RotateTowards(steeringConeLimitQuat, maxAngPerTime);

    DirectX::SimpleMath::Vector3 steeringUpdateVec = DirectX::SimpleMath::Vector3::UnitX;
    steeringUpdateVec = DirectX::SimpleMath::Vector3::Transform(steeringUpdateVec, updateQuat);
    
    aMissile.guidance.conDat.tailFinVec = steeringUpdateVec;
    aMissile.guidance.conDat.tailFinQuat = updateQuat;

    DirectX::SimpleMath::Vector3 finEulers = updateQuat.ToEuler();
    aMissile.guidance.conDat.finPitch = finEulers.z;
    aMissile.guidance.conDat.finYaw = finEulers.y;

    //m_debugData->ToggleDebugOnOverRide();
    //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.conDat.finPitch ", Utility::ToDegrees(aMissile.guidance.conDat.finPitch), "");
    //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.conDat.finYaw   ", Utility::ToDegrees(aMissile.guidance.conDat.finYaw), "");

    m_debugData->ToggleDebugOff();

    //////////////////////////////////////////////////////
    // Thrust Vector Control
    auto thrustSteering = DirectX::SimpleMath::Vector3::UnitX;
    thrustSteering = DirectX::SimpleMath::Vector3::Transform(thrustSteering, navQuat);

    const float thrustSteeringAngle = Utility::GetAngleBetweenVectors(thrustSteering, DirectX::SimpleMath::Vector3::UnitX);

    if (thrustSteeringAngle <= m_missileConsts.thrustVecDeadZoneAng)
    {
        thrustSteering = DirectX::SimpleMath::Vector3::UnitX;
    }
    else
    {
        //DirectX::SimpleMath::Quaternion::
        DirectX::SimpleMath::Quaternion deadZoneLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Quaternion deadZoneConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, thrustSteering);
        deadZoneLimitQuat.RotateTowards(deadZoneConeQuat, thrustSteeringAngle - m_missileConsts.thrustVecDeadZoneAng);

        thrustSteering = DirectX::SimpleMath::Vector3::UnitX;
        thrustSteering = DirectX::SimpleMath::Vector3::Transform(thrustSteering, deadZoneLimitQuat);
    }

    const float maxThrustAng = m_missileConsts.thrustVecAngMax;
    DirectX::SimpleMath::Quaternion thrustConeLimitQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion thrustConeQuat = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, thrustSteering);
    thrustConeLimitQuat.RotateTowards(thrustConeQuat, maxThrustAng);

    const float maxThrustAngPerTime = aTimeDelta * m_missileConsts.thrustVecAngPerSecDeltaMax;

    DirectX::SimpleMath::Vector3 preThrustVec = aMissile.guidance.conDat.thrustVecNorm;
    DirectX::SimpleMath::Quaternion preThrustQuat = aMissile.guidance.conDat.thrustVecQuat;
    DirectX::SimpleMath::Quaternion updateThrustQuat = preThrustQuat;
    updateThrustQuat.RotateTowards(thrustConeLimitQuat, maxThrustAngPerTime);

    DirectX::SimpleMath::Vector3 thrustUpdateVec = DirectX::SimpleMath::Vector3::UnitX;
    thrustUpdateVec = DirectX::SimpleMath::Vector3::Transform(thrustUpdateVec, updateThrustQuat);

    aMissile.guidance.conDat.thrustVecNorm = thrustUpdateVec;
    aMissile.guidance.conDat.thrustVecQuat = updateThrustQuat;

    DirectX::SimpleMath::Vector3 eulers = updateThrustQuat.ToEuler();
    aMissile.guidance.conDat.thrustPitch = eulers.z;
    aMissile.guidance.conDat.thrustYaw = eulers.y;


    //////////////    
    if (m_missileConsts.isThrustVecOn == false)
    {
        aMissile.guidance.conDat.thrustVecNorm = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.conDat.thrustVecQuat = DirectX::SimpleMath::Quaternion::Identity;
        aMissile.guidance.conDat.thrustPitch = 0.0f;
        aMissile.guidance.conDat.thrustYaw = 0.0f;
    }
}

void FireControl::UpdateContrails(MissileData& aMissile)
{
    ++aMissile.contrails.iterator;
    if (aMissile.contrails.iterator >= m_ammoMissile.modelData.contrailDrawCountMax)
    {
        aMissile.contrails.iterator = 0;
    }

    // tail fin position variables
    const float finDeployAngle = Utility::ToRadians(0.0f) + m_ammoMissile.modelData.tailFinDeployAngleMax * aMissile.guidance.finDeployPercent;

    auto updateMat = DirectX::SimpleMath::Matrix::Identity;
    updateMat *= m_ammoMissile.modelData.localTailFinMat;
    updateMat *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);

    // Starboard
    auto conPos = aMissile.contrails.starboard.posLocal;
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, updateMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationY(aMissile.guidance.finPak.tailPitch.finAngle));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, m_ammoMissile.modelData.tailFinTransMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f)));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, aMissile.projectileData.alignmentQuat);
    conPos += aMissile.projectileData.q.position;
    aMissile.contrails.starboard.posVec[aMissile.contrails.iterator] = conPos;

    // bottom
    conPos = aMissile.contrails.bottom.posLocal;
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, updateMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationY(-aMissile.guidance.finPak.tailYaw.finAngle));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, m_ammoMissile.modelData.tailFinTransMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(180.0f)));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, aMissile.projectileData.alignmentQuat);
    conPos += aMissile.projectileData.q.position;
    aMissile.contrails.bottom.posVec[aMissile.contrails.iterator] = conPos;

    // port 
    conPos = aMissile.contrails.port.posLocal;
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, updateMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationY(-aMissile.guidance.finPak.tailPitch.finAngle));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, m_ammoMissile.modelData.tailFinTransMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f)));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, aMissile.projectileData.alignmentQuat);
    conPos += aMissile.projectileData.q.position;
    aMissile.contrails.port.posVec[aMissile.contrails.iterator] = conPos;

    // top
    conPos = aMissile.contrails.top.posLocal;
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, updateMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationY(aMissile.guidance.finPak.tailYaw.finAngle));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, m_ammoMissile.modelData.tailFinTransMat);
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f)));
    conPos = DirectX::SimpleMath::Vector3::Transform(conPos, aMissile.projectileData.alignmentQuat);
    conPos += aMissile.projectileData.q.position;
    aMissile.contrails.top.posVec[aMissile.contrails.iterator] = conPos;
}

void FireControl::DebugPushDrawData(const DirectX::SimpleMath::Vector3 aPosLocal, const DirectX::SimpleMath::Vector3 aVecLocal, const DirectX::XMVECTORF32 aColor, const bool aIsDrawScaledTrue, const bool aIsRealignTrue)
{
    std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32, bool, bool> updateTuple;
    updateTuple = std::make_tuple(aPosLocal, aVecLocal, aColor, aIsDrawScaledTrue, aIsRealignTrue);
    m_debugDrawVec.push_back(updateTuple);
}

void FireControl::DebugDrawUpdate(MissileData& aMissile)
{
    auto posWorld = aMissile.projectileData.q.position;
    auto quatWorld = aMissile.projectileData.alignmentQuat;
    const float lineLength = 5.0f;
    const float offset = 0.0f;
    const float scaleLengthMin = 1.0f;
    const float scaleLengthMod = 1.0f;

    m_debugData->ToggleDebugOnOverRide();
    for (int i = 0; i < m_debugDrawVec.size(); ++i)
    {
        auto pos = std::get<0>(m_debugDrawVec[i]);

        pos = DirectX::SimpleMath::Vector3::Transform(pos, quatWorld);
        pos += posWorld;

        auto vec = std::get<1>(m_debugDrawVec[i]);
        if (std::get<4>(m_debugDrawVec[i]) == true)
        {
            vec = DirectX::SimpleMath::Vector3::Transform(vec, quatWorld);
        }
        if (std::get<3>(m_debugDrawVec[i]) == false)
        {
            m_debugData->PushDebugLine(pos, vec, lineLength, offset, std::get<2>(m_debugDrawVec[i]));

        }
        else
        {
            m_debugData->PushDebugLineScaled(pos, vec, scaleLengthMin, scaleLengthMod, offset, std::get<2>(m_debugDrawVec[i]));
        }

    }
    m_debugData->ToggleDebugOff();
}

float FireControl::GetAeroTensorControlInput(const float aAng, const float aMinAng, const float aMaxAng)
{
    float control = 0.0f;

    if (aAng <= aMinAng)
    {
        return -1.0f;
    }
    else if (aAng >= aMaxAng)
    {
        return 1.0f;
    }
    else if (aAng < 0.0f)
    {
        return aAng / aMinAng;
    }
    else if (aAng > 0.0f)
    {
        return aAng / aMaxAng;
    }
    else
    {
        return 0.0f;
    }
}

DirectX::SimpleMath::Matrix FireControl::GetAeroTensor(FinDataDynamic& aFin, FinDataStatic& aLib)
{
    auto tensor = aLib.baseTensor;
    return tensor;
    if (aFin.controlInput <= -1.0f)
    {
        return aLib.minTensor;
    }
    else if (aFin.controlInput >= 1.0f)
    {
        return aLib.maxTensor;
    }
    else if (aFin.controlInput < 0)
    {
        auto mat = DirectX::SimpleMath::Matrix::Identity;
        mat = DirectX::SimpleMath::Matrix::Lerp(aLib.minTensor, tensor, aFin.controlInput + 1.0f);
        return mat;

    }
    else if (aFin.controlInput > 0)
    {
        auto mat = DirectX::SimpleMath::Matrix::Identity;
        mat = DirectX::SimpleMath::Matrix::Lerp(aLib.maxTensor, tensor, aFin.controlInput);
        return mat;
    }
    else
    {
        return tensor;
    }
}

Utility::ForceAccum FireControl::GetAeroForceAccum(const MissileData& aMissile, const float aTimeDelta, FinDataDynamic& aFin, FinDataStatic& aLib)
{
    auto velocity = aMissile.projectileData.q.velocity;
    auto bodyVel = velocity;
    bodyVel = DirectX::SimpleMath::Vector3::Transform(bodyVel, aMissile.projectileData.inverseAlignmentQuat);
    auto tensor = aFin.aeroTensor;
    auto bodyForce = bodyVel;
    bodyForce = DirectX::SimpleMath::Vector3::Transform(bodyForce, aFin.aeroTensor);

    auto force = bodyForce;


    Utility::ForceAccum sumForce;
    Utility::ForceAccum::ZeroValues(sumForce);
    //sumForce = Utility::AddForceAtPoint(force, aLib.posLocal, DirectX::SimpleMath::Vector3::Zero, 
    
    
    DirectX::SimpleMath::Vector3 forcePos = aLib.posLocal;
    //DirectX::SimpleMath::Vector3 forceDir = aFinDyn.resultantForce;
    DirectX::SimpleMath::Vector3 forceDir = force;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    sumForce.linear = forceAccum;
    sumForce.torque = torqueAccum;

    Utility::ForceAccum worldForce = sumForce;
    worldForce.AlignLinearAndTorque(worldForce, aMissile.projectileData.alignmentQuat);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldForce.linear, 5.0f, 0.0f, DirectX::Colors::LightBlue);
    m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldForce.torque, 5.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->ToggleDebugOff();
    return sumForce;
}

Utility::ForceAccum FireControl::RHSAeroForceAccumulator(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    auto localVelocity = aVelocity;
    localVelocity = DirectX::SimpleMath::Vector3::Transform(localVelocity, aMissile->projectileData.inverseAlignmentQuat);

    Utility::ForceAccum sumDrag;
    Utility::ForceAccum::ZeroValues(sumDrag);
    //sumDrag += DragAccum(aMissile, aTimeDelta);

    Utility::ForceAccum aeroAcc;
    Utility::ForceAccum::ZeroValues(aeroAcc);
    aeroAcc += RHSFinAccumSum(aMissile, localVelocity);

    Utility::ForceAccum bodyAeroAcc;
    Utility::ForceAccum::ZeroValues(bodyAeroAcc);
    bodyAeroAcc += RHSBodyAero(aMissile, aVelocity);
    
    Utility::ForceAccum liftAcc;
    Utility::ForceAccum::ZeroValues(liftAcc);
    //liftAcc += RHSLiftForce(aMissile, localVelocity);
    //liftAcc += RHSLiftForceRebuild(aMissile, localVelocity);
    liftAcc += RHSLiftForceSymmetric(aMissile, localVelocity);

    Utility::ForceAccum sumForce;
    Utility::ForceAccum::ZeroValues(sumForce);
    sumForce += aeroAcc;
    //sumForce += sumDrag;
    sumForce += liftAcc;
    Utility::ForceAccum::AlignLinear(sumForce, aMissile->projectileData.alignmentQuat);
    return sumForce;
}


Utility::ForceAccum FireControl::RHSFinAccumSum(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    Utility::ForceAccum mainPitchAccum;
    Utility::ForceAccum mainYawAccum;
    //mainPitchAccum = FinForceAccum(m_finLib.mainPitch, aMissile.guidance.finPak.mainPitch, aMissile);
    mainPitchAccum = RHSFinCalc(m_finLib.mainPitch, aMissile->guidance.finPak.mainPitch, aMissile, aVelocity);

    //(m_finLib.mainPitch, aMissile.guidance.finPak.mainPitch, aMissile, aVelocity);
    //mainYawAccum = FinForceAccum(m_finLib.mainYaw, aMissile.guidance.finPak.mainYaw, aMissile);
    //mainYawAccum = RHSFinForce(m_finLib.mainYaw, aMissile.guidance.finPak.mainYaw, aMissile, aVelocity);
    mainYawAccum = RHSFinCalc(m_finLib.mainYaw, aMissile->guidance.finPak.mainYaw, aMissile, aVelocity);

    Utility::ForceAccum mainSum;
    Utility::ForceAccum::ZeroValues(mainSum);
    mainSum += mainPitchAccum;
    mainSum += mainYawAccum;

    Utility::ForceAccum tailPitchAccum;
    Utility::ForceAccum tailYawAccum;

    //tailPitchAccum = FinForceAccum(m_finLib.tailPitch, aMissile.guidance.finPak.tailPitch, aMissile);
    //tailYawAccum = FinForceAccum(m_finLib.tailYaw, aMissile.guidance.finPak.tailYaw, aMissile);

    tailPitchAccum = RHSFinCalc(m_finLib.tailPitch, aMissile->guidance.finPak.tailPitch, aMissile, aVelocity);
    tailYawAccum = RHSFinCalc(m_finLib.tailYaw, aMissile->guidance.finPak.tailYaw, aMissile, aVelocity);

    Utility::ForceAccum tailSum;
    Utility::ForceAccum::ZeroValues(tailSum);
    tailSum += tailPitchAccum;
    tailSum += tailYawAccum;

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum += mainSum;
    accum += tailSum;

    //return accum;
    m_testLiftVec = mainSum.linear;

    Utility::ForceAccum sumForce;
    sumForce = accum;
    return sumForce;
}


DirectX::SimpleMath::Vector3 FireControl::RHSFinForce(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    //auto airSpeedLocal = aMissile.guidance.localVel * -1.0f;
    auto airSpeedLocal = aVelocity * -1.0f;
    auto airSpeedLocalNorm = airSpeedLocal;
    airSpeedLocalNorm.Normalize();

    auto finAxis = aStaticDat.axis;
    auto finAngle = aFinDyn.finAngle;

    auto testPos = aStaticDat.posLocal;
    /*
    float aoaToUse = Utility::ToRadians(0.0f);
    auto chordLineAirVec = airSpeedLocalNorm;
    if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW || aStaticDat.finType == FinType::CANARD_YAW)
    {
        chordLineAirVec.y = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.y < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }
    else
    {
        chordLineAirVec.z = 0.0f;
        chordLineAirVec.Normalize();
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        if (crossVec.z < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }
    */

    auto testAngle = aFinDyn.prevFinAngle;
    float angDelta = aFinDyn.finAngle - aFinDyn.prevFinAngle;
    float quarterAngDelta = angDelta * 0.25f;
    if (aMissile->projectileData.rhsCounter == 0)
    {
        testAngle = aFinDyn.prevFinAngle;
    }
    else if (aMissile->projectileData.rhsCounter == 1 || aMissile->projectileData.rhsCounter == 2)
    {
        testAngle = (aFinDyn.prevFinAngle + aFinDyn.finAngle) * 0.5f;
    }
    else
    {
        testAngle = aFinDyn.finAngle;
    }

    //testAngle += quarterAngDelta * (static_cast<float>(aMissile->projectileData.rhsCounter ));

    auto testCord = DirectX::SimpleMath::Vector3::UnitX;
    testCord = DirectX::SimpleMath::Vector3::Transform(testCord,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(aStaticDat.axis, testAngle));

    auto testDir = aStaticDat.finNormal;
    testDir = DirectX::SimpleMath::Vector3::Transform(testDir,
        DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(aStaticDat.axis, testAngle));

    float aoaToUse = Utility::ToRadians(0.0f);
    auto chordLineAirVec = airSpeedLocalNorm;
    if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW || aStaticDat.finType == FinType::CANARD_YAW)
    {
        chordLineAirVec.y = 0.0f;
        chordLineAirVec.Normalize();
        //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -testCord);
        //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        auto crossVec = testCord.Cross(chordLineAirVec);
        if (crossVec.y < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }
    else
    {
        chordLineAirVec.z = 0.0f;
        chordLineAirVec.Normalize();
        //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
        aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -testCord);
        //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
        auto crossVec = testCord.Cross(chordLineAirVec);
        if (crossVec.z < 0.0f)
        {
            aoaToUse *= -1.0f;
        }
    }

    float angleOfAttack = aoaToUse;

    const float cl = CalculateFinLiftCoefFlat(angleOfAttack);
    //const float cl = CalculateFinLiftCoefTest(angleOfAttack);
    //const float cl = CalculateFinLiftCoef(angleOfAttack);
    //const float cl = CalculateFinLiftCoefDebug(angleOfAttack);
    //const float cl = m_missileConsts.finClConst;

    const float surface = aStaticDat.span * aStaticDat.chord;
    const float rho = m_environment->GetAirDensity();

    // lift = coefficient * density * (vel^2 / two) * wing area
    auto lift = cl * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * surface;

    const float liftLength = lift.Length();
    //auto liftTest = cl * rho * ((airSpeedLocal.Length() * airSpeedLocal.Length()) / 2.0f) * surface;

    auto axisTest = aStaticDat.axis;
    if (aFinDyn.finDir.Dot(airSpeedLocalNorm) > 0.0f)
    {
        axisTest = aStaticDat.axis;
    }
    else
    {
        axisTest = -aStaticDat.axis;
    }

    auto liftNormTest = airSpeedLocalNorm.Cross(axisTest);
    liftNormTest.Normalize();

    //////////////////////// aFinDyn.liftForce = liftNormTest * lift.Length();

    auto airImpactDot = abs(aFinDyn.finDir.Dot(airSpeedLocalNorm));

    const float dragSurfaceBase = (surface * 0.5f) * 0.7f; // avg'ing data pulled from wiki
    const float dragSufaceAddMax = surface - dragSurfaceBase;

    //const float dragSurface = dragSurfaceBase + (dragSufaceAddMax * abs(airImpactDot));
    //const float dragSurface = CalculateFinDragArea(airSpeedLocalNorm, aFinDyn.finDir, aStaticDat);
    const float dragSurface = CalculateFinDragArea(airSpeedLocalNorm, testDir, aStaticDat);

    const float dragCoefBase = aStaticDat.dragCoeffBase;
    const float dragModMax = aStaticDat.dragCoeefMod;
    const float cd = dragCoefBase + (dragModMax * airImpactDot);
    auto finDrag = cd * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * dragSurface;
    //auto lift2   = cl * rho * ((airSpeedLocal * airSpeedLocal) / 2.0f) * surface;

    //auto testDrag = (liftF * liftF) / (.5f * m_environment->GetAirDensity() * (aVelocity.Length() * aVelocity.Length()) * Utility::GetPi() * (m_finLib.mainYaw.span * m_finLib.mainYaw.span));
    auto testDrag = (lift.Length() * lift.Length()) 
        / (.5f * m_environment->GetAirDensity() * (aVelocity.Length() * aVelocity.Length()) * Utility::GetPi() * ( aStaticDat.span * aStaticDat.span));
    auto testDrag2 = CalculateLiftInducedDrag(cl, liftLength, aVelocity.Length(), aStaticDat.wingSpanFull, aStaticDat.chord, aStaticDat.wingArea);
    //return (liftNormTest * lift.Length()) + (airSpeedLocalNorm * finDrag.Length());
    return (liftNormTest * lift.Length()) + ((airSpeedLocalNorm * finDrag.Length()) + airSpeedLocalNorm * testDrag2);
}



Utility::ForceAccum FireControl::RHSFinCalc(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    //auto liftForce = aFinDyn.liftForce;
    //auto dragForce = aFinDyn.dragForce;

    DirectX::SimpleMath::Vector3 forcePos = aStaticDat.posLocal;
    //DirectX::SimpleMath::Vector3 forceDir = aFinDyn.resultantForce;
    //DirectX::SimpleMath::Vector3 forceDir = dragForce;
    DirectX::SimpleMath::Vector3 forceDir = RHSFinForce(aStaticDat, aFinDyn, aMissile, aVelocity);

    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;

    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    return accum;
}

Utility::ForceAccum FireControl::RHSBodyAero(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    auto worldVel = aVelocity;
    worldVel = DirectX::SimpleMath::Vector3::Transform(worldVel, aMissile->projectileData.alignmentQuat);

    auto localAirVel = aVelocity;
    auto localAirVelNorm = localAirVel;
    localAirVelNorm.Normalize();
  
    auto testNorm = aVelocity;
    testNorm = DirectX::SimpleMath::Vector3::Transform(testNorm, aMissile->projectileData.inverseAlignmentQuat);
    testNorm.Normalize();


    DirectX::SimpleMath::Vector3 forcePos = aMissile->guidance.centerOfPressureLocalPos;
    //DirectX::SimpleMath::Vector3 forceDir = CalculateDragLinearForAccumulator(aMissile);
    //DirectX::SimpleMath::Vector3 forceDir = CalculateDragLinearForAccumulator(aMissile);
    //DirectX::SimpleMath::Vector3 forceDir = CalcDragLinearCurrent(aMissile, aVelocity);
    DirectX::SimpleMath::Vector3 forceDir = CalcDragLinearCurrent(aMissile, worldVel);
    //forceDir = DirectX::SimpleMath::Vector3::Transform(forceDir, aMissile->projectileData.inverseAlignmentQuat);
    //forceDir = localAirVelNorm * forceDir.Length();
    //forceDir = testNorm * forceDir.Length();

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, forceDir, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, forceDir, DirectX::Colors::Blue, false, false);

    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum accum;
    Utility::ForceAccum::ZeroValues(accum);
    accum.linear = forceAccum;
    accum.torque = torqueAccum;

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, forceDir, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, localAirVelNorm, DirectX::Colors::White, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, aVelocity, DirectX::Colors::White, false, false);

    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, accum.linear, DirectX::Colors::Lime, false, true);
    //DebugPushDrawData(DirectX::SimpleMath::Vector3::Zero, accum.torque, DirectX::Colors::Teal, false, true);
    m_debugData->ToggleDebugOnOverRide();
    //m_debugData->DebugPushUILineDecimalNumber("forceDir = ", forceDir.Length(), "");
    m_debugData->ToggleDebugOff();

    return accum;
}


Utility::ForceAccum FireControl::RHSLiftForce(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    auto upLocal = DirectX::SimpleMath::Vector3::UnitY;
    upLocal = DirectX::SimpleMath::Vector3::Transform(upLocal, aMissile->projectileData.inverseAlignmentQuat);
    upLocal.Normalize();
    const auto gravVec = upLocal * (m_environment->GetGravity() * m_missileConsts.mass);
    DirectX::SimpleMath::Vector3 forceDir = DirectX::SimpleMath::Vector3::Zero;
    auto debugVec = DirectX::SimpleMath::Vector3::Zero;

    /////////////////////

    auto velocityNorm = aVelocity;
    velocityNorm.Normalize();

    auto aeroQuat2 = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, velocityNorm);
    auto invsAeroQuat2 = aeroQuat2;
    invsAeroQuat2.Inverse(invsAeroQuat2);

    auto upLocal2 = DirectX::SimpleMath::Vector3::UnitY;
    upLocal2 = DirectX::SimpleMath::Vector3::Transform(upLocal2, aMissile->projectileData.inverseAlignmentQuat);
    upLocal2 = DirectX::SimpleMath::Vector3::Transform(upLocal2, invsAeroQuat2);
    auto right2 = velocityNorm.Cross(upLocal2);
    right2.Normalize();
    auto bodyUp2 = right2.Cross(velocityNorm);
    bodyUp2.Normalize();

    auto velUp = DirectX::SimpleMath::Vector3::UnitY;
    velUp = DirectX::SimpleMath::Vector3::Transform(velUp, invsAeroQuat2);
    velUp = DirectX::SimpleMath::Vector3::Transform(velUp, aMissile->projectileData.inverseAlignmentQuat);

    auto velRight = -DirectX::SimpleMath::Vector3::UnitZ;
    velRight = DirectX::SimpleMath::Vector3::Transform(velRight, invsAeroQuat2);
    velRight = DirectX::SimpleMath::Vector3::Transform(velRight, aMissile->projectileData.inverseAlignmentQuat);

    auto bodyUp3 = velRight.Cross(velocityNorm);
    bodyUp3.Normalize();

    auto bodyUp4 = DirectX::SimpleMath::Vector3::UnitY;
    bodyUp4 = DirectX::SimpleMath::Vector3::Transform(bodyUp4, invsAeroQuat2);
    //bodyUp4 = DirectX::SimpleMath::Vector3::Transform(bodyUp4, aMissile->projectileData.inverseAlignmentQuat);
    bodyUp4.Normalize();

    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, velUp, DirectX::Colors::Blue, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, velRight, DirectX::Colors::Yellow, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, bodyUp3, DirectX::Colors::Red, false, true);
    auto aeroQuat = DirectX::SimpleMath::Quaternion::LookRotation(velocityNorm, upLocal2);
    auto invsAeroQuat = aeroQuat;
    invsAeroQuat.Inverse(invsAeroQuat);

    ///////////
    auto windRight = upLocal.Cross(velocityNorm);
    auto testUp = windRight.Cross(-DirectX::SimpleMath::Vector3::UnitX);

    DebugPushDrawData(m_missileConsts.centerOfMassLocal, windRight, DirectX::Colors::Red, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal, DirectX::Colors::Blue, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, testUp, DirectX::Colors::Yellow, false, true);

    ///////////

    /*
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, velocityNorm, DirectX::Colors::White, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, right2, DirectX::Colors::Red, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, bodyUp2, DirectX::Colors::Blue, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal2, DirectX::Colors::Yellow, false, true);
    */
    /////////////////////

    Utility::ForceAccum liftAccum;
    Utility::ForceAccum::ZeroValues(liftAccum);
    if (aMissile->guidance.isFinsDeployEnd == false || aMissile->guidance.isExplodingTrue == true)
    {
        //forceDir = gravVec;
        forceDir = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        float cL = CalculateFinLiftCoefFlat(aMissile->guidance.angleOfAttack);
        const float airDensity = m_environment->GetAirDensity();
        //const float wingArea = m_missileConsts.wingArea;
        const float wingArea = m_finLib.mainPitch.wingArea + m_finLib.tailPitch.wingArea;
 
        DirectX::SimpleMath::Vector3 worldVelocityNorm = aMissile->projectileData.q.velocity;
        worldVelocityNorm.Normalize();
        float velocityDotProd = worldVelocityNorm.Dot(aMissile->projectileData.forward);
        float velocityLength = aMissile->projectileData.q.velocity.Length();

        float liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftV = cL * ((airDensity * (aMissile->projectileData.q.velocity * aMissile->projectileData.q.velocity)) * 0.5f) * wingArea;
        float liftVLength = liftV.Length();
        float rawVelocityLength = aMissile->projectileData.q.velocity.Length();

        DirectX::SimpleMath::Vector3 fowardVelocity = aMissile->projectileData.forward * (velocityDotProd * aMissile->projectileData.q.velocity);
        DirectX::SimpleMath::Vector3 lineOfFlight = aMissile->projectileData.q.velocity;
        DirectX::SimpleMath::Vector3 liftVector = cL * ((airDensity * (fowardVelocity * fowardVelocity)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftVectorLoF = cL * ((airDensity * (lineOfFlight * lineOfFlight)) * 0.5f) * wingArea;
        float forwardVelocityLength = fowardVelocity.Length();
        float forwardLength = aMissile->projectileData.forward.Length();
        float liftVectorLength = liftVector.Length();

        DirectX::SimpleMath::Vector3 chordLine = aMissile->projectileData.right;
        DirectX::SimpleMath::Vector3 liftLine = chordLine.Cross(worldVelocityNorm);

        liftLine *= abs(liftF);

        //aMissile->guidance.liftForce = liftLine;
        //aMissile->guidance.liftForceFloat = liftF;

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
        localizedNormY = DirectX::SimpleMath::Vector3::Transform(localizedNormY, aMissile->projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 crossProdY = localizedNormY;
        crossProdY = crossProdY.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 worldCrossProdY = crossProdY;
        worldCrossProdY = DirectX::SimpleMath::Vector3::Transform(worldCrossProdY, aMissile->projectileData.alignmentQuat);

        DirectX::SimpleMath::Vector3 trueRightWorld = aMissile->projectileData.forward.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 trueRightLocal = trueRightWorld;
        trueRightLocal = DirectX::SimpleMath::Vector3::Transform(trueRightLocal, aMissile->projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 trueUpLocal = trueRightLocal.Cross(DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 trueUpWorld = trueUpLocal;
        //trueUpWorld = DirectX::SimpleMath::Vector3::Transform(trueUpWorld, aMissile->projectileData.alignmentQuat);

        trueUpWorld.Normalize();

        //aMissile->guidance.liftForce = testLift * trueUpWorld;
        //////////////////////////////
        auto airSpeedLocal = aMissile->guidance.localVel * -1.0f;
        auto airSpeedLocalNorm = airSpeedLocal;
        airSpeedLocalNorm.Normalize();

        //auto testLocalVel = aMissile->projectileData.q.velocity;
        //testLocalVel = DirectX::SimpleMath::Vector3::Transform(testLocalVel, aMissile->projectileData.inverseAlignmentQuat);
        auto testLocalVel = aVelocity;
        //testLocalVel = DirectX::SimpleMath::Vector3::Transform(testLocalVel, aMissile->projectileData.inverseAlignmentQuat);

        auto testLocalVelNorm = testLocalVel;
        testLocalVelNorm.Normalize();
        auto trueRightLocalTest = testLocalVelNorm.Cross(upLocal);
        auto trupUpLocalTest2 = trueRightLocalTest.Cross(DirectX::SimpleMath::Vector3::UnitX);
        trupUpLocalTest2.Normalize();
        const auto trueUpLocalTest = trupUpLocalTest2;
        const auto bodyUp = trupUpLocalTest2;
        
        auto bodyUpQuat = DirectX::SimpleMath::Quaternion::LookRotation(testLocalVelNorm, bodyUp);
        auto forwardQuat = DirectX::SimpleMath::Quaternion::LookRotation(DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
        auto angleTest = DirectX::SimpleMath::Quaternion::Angle(forwardQuat, bodyUpQuat);

        float aoaToUse = Utility::ToRadians(0.0f);
        auto chordLineAirVec = airSpeedLocalNorm;
        auto airSpeedLocalNormMod = airSpeedLocalNorm;
        airSpeedLocalNormMod.z = 0.0f;
        airSpeedLocalNormMod.Normalize();
        //if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW || aStaticDat.finType == FinType::CANARD_YAW)
        if (1 == 1)
        {
            //chordLineAirVec.y = 0.0f;
            chordLineAirVec.Normalize();
            //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
            //aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNorm, -DirectX::SimpleMath::Vector3::UnitX);
            aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNorm, bodyUp);
            //aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNormMod, bodyUp);
            aoaToUse -= Utility::ToRadians(90.0);
            //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
            auto crossVec = DirectX::SimpleMath::Vector3::UnitX.Cross(chordLineAirVec);
            if (crossVec.z < 0.0f)
            {
                //aoaToUse *= -1.0f;
                m_debugData->ToggleDebugOnOverRide();
               // m_debugData->DebugPushUILineDecimalNumber("aoaToUse   = ", aoaToUse, "");
          
                m_debugData->ToggleDebugOff();
            }
        }
        else
        {
            chordLineAirVec.z = 0.0f;
            chordLineAirVec.Normalize();
            //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
            aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -DirectX::SimpleMath::Vector3::UnitX);
            //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
            auto crossVec = DirectX::SimpleMath::Vector3::UnitX.Cross(chordLineAirVec);
            if (crossVec.z < 0.0f)
            {
                aoaToUse *= -1.0f;
            }
        }
        float angleOfAttack = -aoaToUse;


        //m_debugData->ToggleDebugOnOverRide();
        //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack   = ", angleOfAttack, "");
        m_debugData->DebugPushUILineDecimalNumber("angleOfAttack d = ", Utility::ToDegrees(angleOfAttack), "");
        //m_debugData->DebugPushUILineDecimalNumber("angleTest     d = ", Utility::ToDegrees(angleTest), "");
        //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack= ", liftAccum.torque.Length(), "");
        m_debugData->ToggleDebugOff();

        //cL = CalculateFinLiftWholeBody(angleOfAttack) * 1.0f;
        cL = CalculateFinLiftWholeBodySymmetric(angleOfAttack) * 1.0f;
        liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f)* wingArea;

        m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("cl   = ", cL, "");
        m_debugData->ToggleDebugOff();

        //DebugPushDrawData(m_missileConsts.centerOfMassLocal, trueRightLocalTest, DirectX::Colors::Red, false, true);
        //DebugPushDrawData(m_missileConsts.centerOfMassLocal, testLocalVel, DirectX::Colors::Yellow, false, true);
        /*
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, trupUpLocalTest, DirectX::Colors::White, false, true);
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal, DirectX::Colors::Red, false, true);
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, gravVec, DirectX::Colors::GreenYellow, false, true);
        */
        //
        //forceDir = liftF * upLocal;
        //forceDir = liftF * trueUpLocalTest;
        forceDir = liftF * bodyUp4;
        //forceDir = DirectX::SimpleMath::Vector3::Transform(forceDir, invsAeroQuat2);
        //forceDir = liftF * ;
        //forceDir += gravVec;
        debugVec = trueUpLocalTest;
    }
    
    DirectX::SimpleMath::Vector3 forcePos = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    liftAccum.linear = forceAccum;
    liftAccum.torque = torqueAccum;

    DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftAccum.linear, DirectX::Colors::Purple, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftAccum.torque, DirectX::Colors::Teal, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, debugVec, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, debugVec, DirectX::Colors::White, false, false);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceDir = ", forceDir.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.linear = ", liftAccum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.torque = ", liftAccum.torque.Length(), "");
    m_debugData->ToggleDebugOff();

    return liftAccum;
}

Utility::ForceAccum FireControl::RHSLiftForceRebuild(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    auto upLocal = DirectX::SimpleMath::Vector3::UnitY;
    upLocal = DirectX::SimpleMath::Vector3::Transform(upLocal, aMissile->projectileData.inverseAlignmentQuat);
    upLocal.Normalize();
    const auto gravVec = upLocal * (m_environment->GetGravity() * m_missileConsts.mass);
    DirectX::SimpleMath::Vector3 forceDir = DirectX::SimpleMath::Vector3::Zero;
    auto debugVec = DirectX::SimpleMath::Vector3::Zero;

    /////////////////////

    auto velocityNorm = aVelocity;
    velocityNorm.Normalize();

    auto aeroQuat2 = DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitX, velocityNorm);
    auto invsAeroQuat2 = aeroQuat2;
    invsAeroQuat2.Inverse(invsAeroQuat2);

    auto upLocal2 = DirectX::SimpleMath::Vector3::UnitY;
    upLocal2 = DirectX::SimpleMath::Vector3::Transform(upLocal2, aMissile->projectileData.inverseAlignmentQuat);
    upLocal2 = DirectX::SimpleMath::Vector3::Transform(upLocal2, invsAeroQuat2);
    auto right2 = velocityNorm.Cross(upLocal2);
    right2.Normalize();
    auto bodyUp2 = right2.Cross(velocityNorm);
    bodyUp2.Normalize();

    auto velUp = DirectX::SimpleMath::Vector3::UnitY;
    velUp = DirectX::SimpleMath::Vector3::Transform(velUp, invsAeroQuat2);
    velUp = DirectX::SimpleMath::Vector3::Transform(velUp, aMissile->projectileData.inverseAlignmentQuat);

    auto velRight = -DirectX::SimpleMath::Vector3::UnitZ;
    velRight = DirectX::SimpleMath::Vector3::Transform(velRight, invsAeroQuat2);
    velRight = DirectX::SimpleMath::Vector3::Transform(velRight, aMissile->projectileData.inverseAlignmentQuat);

    auto bodyUp3 = velRight.Cross(velocityNorm);
    bodyUp3.Normalize();

    auto bodyUp4 = DirectX::SimpleMath::Vector3::UnitY;
    bodyUp4 = DirectX::SimpleMath::Vector3::Transform(bodyUp4, invsAeroQuat2);
    //bodyUp4 = DirectX::SimpleMath::Vector3::Transform(bodyUp4, aMissile->projectileData.inverseAlignmentQuat);
    bodyUp4.Normalize();

    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, velUp, DirectX::Colors::Blue, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, velRight, DirectX::Colors::Yellow, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, bodyUp3, DirectX::Colors::Red, false, true);
    auto aeroQuat = DirectX::SimpleMath::Quaternion::LookRotation(velocityNorm, upLocal2);
    auto invsAeroQuat = aeroQuat;
    invsAeroQuat.Inverse(invsAeroQuat);

    ///////////
    auto windRight = upLocal.Cross(velocityNorm);
    auto testUp = windRight.Cross(-DirectX::SimpleMath::Vector3::UnitX);

    auto wVelNorm = aMissile->projectileData.q.velocity;
    wVelNorm.Normalize();
    auto wForward = aMissile->projectileData.forward;
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, wForward, DirectX::Colors::Red, false, false);

    auto aeroQuat3 = DirectX::SimpleMath::Quaternion::LookRotation(wVelNorm, DirectX::SimpleMath::Vector3::UnitY);
    aeroQuat3.Normalize();
    auto invsAeroQuat3 = aeroQuat3;
    invsAeroQuat3.Inverse(invsAeroQuat3);
    invsAeroQuat3.Normalize();

    auto testLine = DirectX::SimpleMath::Vector3::UnitY;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, invsAeroQuat3);
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, aMissile->projectileData.inverseAlignmentQuat);
    testLine = m_testLiftVec;
    testLine.Normalize();
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, testLine, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, testLine, DirectX::Colors::Blue, false, false);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, m_testLiftVec, DirectX::Colors::Blue, false, true);

    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, windRight, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal, DirectX::Colors::Blue, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, testUp, DirectX::Colors::Yellow, false, true);

    ///////////

    /*
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, velocityNorm, DirectX::Colors::White, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, right2, DirectX::Colors::Red, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, bodyUp2, DirectX::Colors::Blue, false, true);
    DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal2, DirectX::Colors::Yellow, false, true);
    */
    /////////////////////

    Utility::ForceAccum liftAccum;
    Utility::ForceAccum::ZeroValues(liftAccum);
    if (aMissile->guidance.isFinsDeployEnd == false || aMissile->guidance.isExplodingTrue == true)
    {
        //forceDir = gravVec;
        forceDir = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        float cL = CalculateFinLiftCoefFlat(aMissile->guidance.angleOfAttack);
        const float airDensity = m_environment->GetAirDensity();
        //const float wingArea = m_missileConsts.wingArea;
        const float wingArea = m_finLib.mainPitch.wingArea + m_finLib.tailPitch.wingArea;

        DirectX::SimpleMath::Vector3 worldVelocityNorm = aMissile->projectileData.q.velocity;
        worldVelocityNorm.Normalize();
        float velocityDotProd = worldVelocityNorm.Dot(aMissile->projectileData.forward);
        float velocityLength = aMissile->projectileData.q.velocity.Length();

        float liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftV = cL * ((airDensity * (aMissile->projectileData.q.velocity * aMissile->projectileData.q.velocity)) * 0.5f) * wingArea;
        float liftVLength = liftV.Length();
        float rawVelocityLength = aMissile->projectileData.q.velocity.Length();

        DirectX::SimpleMath::Vector3 fowardVelocity = aMissile->projectileData.forward * (velocityDotProd * aMissile->projectileData.q.velocity);
        DirectX::SimpleMath::Vector3 lineOfFlight = aMissile->projectileData.q.velocity;
        DirectX::SimpleMath::Vector3 liftVector = cL * ((airDensity * (fowardVelocity * fowardVelocity)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftVectorLoF = cL * ((airDensity * (lineOfFlight * lineOfFlight)) * 0.5f) * wingArea;
        float forwardVelocityLength = fowardVelocity.Length();
        float forwardLength = aMissile->projectileData.forward.Length();
        float liftVectorLength = liftVector.Length();

        DirectX::SimpleMath::Vector3 chordLine = aMissile->projectileData.right;
        DirectX::SimpleMath::Vector3 liftLine = chordLine.Cross(worldVelocityNorm);

        liftLine *= abs(liftF);

        //aMissile->guidance.liftForce = liftLine;
        //aMissile->guidance.liftForceFloat = liftF;

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
        localizedNormY = DirectX::SimpleMath::Vector3::Transform(localizedNormY, aMissile->projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 crossProdY = localizedNormY;
        crossProdY = crossProdY.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 worldCrossProdY = crossProdY;
        worldCrossProdY = DirectX::SimpleMath::Vector3::Transform(worldCrossProdY, aMissile->projectileData.alignmentQuat);

        DirectX::SimpleMath::Vector3 trueRightWorld = aMissile->projectileData.forward.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 trueRightLocal = trueRightWorld;
        trueRightLocal = DirectX::SimpleMath::Vector3::Transform(trueRightLocal, aMissile->projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 trueUpLocal = trueRightLocal.Cross(DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 trueUpWorld = trueUpLocal;
        //trueUpWorld = DirectX::SimpleMath::Vector3::Transform(trueUpWorld, aMissile->projectileData.alignmentQuat);

        trueUpWorld.Normalize();

        //aMissile->guidance.liftForce = testLift * trueUpWorld;
        //////////////////////////////
        auto airSpeedLocal = aMissile->guidance.localVel * -1.0f;
        auto airSpeedLocalNorm = airSpeedLocal;
        airSpeedLocalNorm.Normalize();

        //auto testLocalVel = aMissile->projectileData.q.velocity;
        //testLocalVel = DirectX::SimpleMath::Vector3::Transform(testLocalVel, aMissile->projectileData.inverseAlignmentQuat);
        auto testLocalVel = aVelocity;
        //testLocalVel = DirectX::SimpleMath::Vector3::Transform(testLocalVel, aMissile->projectileData.inverseAlignmentQuat);

        auto testLocalVelNorm = testLocalVel;
        testLocalVelNorm.Normalize();
        auto trueRightLocalTest = testLocalVelNorm.Cross(upLocal);
        auto trupUpLocalTest2 = trueRightLocalTest.Cross(DirectX::SimpleMath::Vector3::UnitX);
        trupUpLocalTest2.Normalize();
        const auto trueUpLocalTest = trupUpLocalTest2;
        const auto bodyUp = trupUpLocalTest2;

        auto bodyUpQuat = DirectX::SimpleMath::Quaternion::LookRotation(testLocalVelNorm, bodyUp);
        auto forwardQuat = DirectX::SimpleMath::Quaternion::LookRotation(DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
        auto angleTest = DirectX::SimpleMath::Quaternion::Angle(forwardQuat, bodyUpQuat);

        float aoaToUse = Utility::ToRadians(0.0f);
        auto chordLineAirVec = airSpeedLocalNorm;
        auto airSpeedLocalNormMod = airSpeedLocalNorm;
        airSpeedLocalNormMod.z = 0.0f;
        airSpeedLocalNormMod.Normalize();
        //if (aStaticDat.finType == FinType::MAIN_YAW || aStaticDat.finType == FinType::TAIL_YAW || aStaticDat.finType == FinType::CANARD_YAW)
        if (1 == 1)
        {
            //chordLineAirVec.y = 0.0f;
            chordLineAirVec.Normalize();
            //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
            //aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNorm, -DirectX::SimpleMath::Vector3::UnitX);
            aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNorm, bodyUp);
            //aoaToUse = Utility::GetAngleBetweenVectors(airSpeedLocalNormMod, bodyUp);
            aoaToUse -= Utility::ToRadians(90.0);
            //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
            auto crossVec = DirectX::SimpleMath::Vector3::UnitX.Cross(chordLineAirVec);
            if (crossVec.z < 0.0f)
            {
                //aoaToUse *= -1.0f;
                m_debugData->ToggleDebugOnOverRide();
                // m_debugData->DebugPushUILineDecimalNumber("aoaToUse   = ", aoaToUse, "");

                m_debugData->ToggleDebugOff();
            }
        }
        else
        {
            chordLineAirVec.z = 0.0f;
            chordLineAirVec.Normalize();
            //aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -aFinDyn.chordLine);
            aoaToUse = Utility::GetAngleBetweenVectors(chordLineAirVec, -DirectX::SimpleMath::Vector3::UnitX);
            //auto crossVec = aFinDyn.chordLine.Cross(chordLineAirVec);
            auto crossVec = DirectX::SimpleMath::Vector3::UnitX.Cross(chordLineAirVec);
            if (crossVec.z < 0.0f)
            {
                aoaToUse *= -1.0f;
            }
        }
        float angleOfAttack = -aoaToUse;


        //m_debugData->ToggleDebugOnOverRide();
        //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack   = ", angleOfAttack, "");
        m_debugData->DebugPushUILineDecimalNumber("angleOfAttack d = ", Utility::ToDegrees(angleOfAttack), "");
        //m_debugData->DebugPushUILineDecimalNumber("angleTest     d = ", Utility::ToDegrees(angleTest), "");
        //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack= ", liftAccum.torque.Length(), "");
        m_debugData->ToggleDebugOff();

        //cL = CalculateFinLiftWholeBody(angleOfAttack) * 1.0f;
        //angleOfAttack = abs(angleOfAttack);
        //cL = CalculateFinLiftWholeBodySymmetric(angleOfAttack) * 1.0f;
        cL = CalculateFinLiftCoefFlat(angleOfAttack) * 1.0f;
        liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;

        m_debugData->ToggleDebugOnOverRide();
        m_debugData->DebugPushUILineDecimalNumber("cl   = ", cL, "");
        m_debugData->DebugPushUILineDecimalNumber("angleOfAttack   = ", Utility::ToDegrees(angleOfAttack), "");
        m_debugData->ToggleDebugOff();

        //DebugPushDrawData(m_missileConsts.centerOfMassLocal, trueRightLocalTest, DirectX::Colors::Red, false, true);
        //DebugPushDrawData(m_missileConsts.centerOfMassLocal, testLocalVel, DirectX::Colors::Yellow, false, true);
        /*
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, trupUpLocalTest, DirectX::Colors::White, false, true);
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, upLocal, DirectX::Colors::Red, false, true);
        DebugPushDrawData(m_missileConsts.centerOfMassLocal, gravVec, DirectX::Colors::GreenYellow, false, true);
        */
        //
        //forceDir = liftF * upLocal;
        //forceDir = liftF * trueUpLocalTest;
        //forceDir = liftF * bodyUp4;
        forceDir = liftF * testLine;
        //forceDir = DirectX::SimpleMath::Vector3::Transform(forceDir, aeroQuat3);
        //forceDir = liftF * ;
        //forceDir += gravVec;
        debugVec = trueUpLocalTest;
    }

    DirectX::SimpleMath::Vector3 forcePos = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    liftAccum.linear = forceAccum;
    liftAccum.torque = torqueAccum;
    
    Utility::ForceAccum::AlignLinearAndTorque(liftAccum, invsAeroQuat3 );
    
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftAccum.linear, DirectX::Colors::White, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftAccum.torque, DirectX::Colors::Teal, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, debugVec, DirectX::Colors::Red, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, debugVec, DirectX::Colors::White, false, false);

    m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceDir = ", forceDir.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.linear = ", liftAccum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.torque = ", liftAccum.torque.Length(), "");
    m_debugData->ToggleDebugOff();

    return liftAccum;
}

Utility::ForceAccum FireControl::RHSLiftForceSymmetric(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    auto velocityNorm = aVelocity;
    velocityNorm.Normalize();
    auto liftNorm = velocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX).Cross(velocityNorm);
    liftNorm.Normalize();

    const float airDensity = m_environment->GetAirDensity();
    //const float wingArea = m_missileConsts.wingArea;
    const float wingArea = m_finLib.mainPitch.wingArea + m_finLib.tailPitch.wingArea;
    //float velocityLength = aMissile->projectileData.q.velocity.Length();
    float velocityLength = aVelocity.Length();
    float angleOfAttack = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, velocityNorm);
    float cL = CalculateFinLiftCoefUpdate(angleOfAttack) * 1.0f;
    float liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;
    const auto liftVec = liftF * liftNorm;
    const auto dragVec = RHSDragForceSymmetric(aMissile, aVelocity);

    //auto testDrag2 = CalculateLiftInducedDrag(cL, liftF, aVelocity.Length(), m_finLib.mainYaw.wingSpanFull, m_finLib.mainYaw.chord, m_finLib.mainYaw.wingArea);
    //auto testDrag = (liftF * liftF) / (.5f * m_environment->GetAirDensity() * (aVelocity.Length() * aVelocity.Length()) * Utility::GetPi() * (m_finLib.mainYaw.span * m_finLib.mainYaw.span));
    //auto testDrag = (liftF * liftF) / (.5f * m_environment->GetAirDensity() * (aVelocity.Length() * aVelocity.Length()) * Utility::GetPi() * (m_finLib.mainYaw.wingSpanFull * m_finLib.mainYaw.wingSpanFull));
    auto testDrag = CalculateLiftInducedDrag(cL, liftF, aVelocity.Length(), m_finLib.mainYaw.wingSpanFull, m_finLib.mainYaw.chord, m_finLib.mainYaw.wingArea);
    auto liftDrag = testDrag * -velocityNorm;

    //DirectX::SimpleMath::Vector3 forceDir = liftVec;
    //DirectX::SimpleMath::Vector3 forceDir = liftVec + dragVec;
    DirectX::SimpleMath::Vector3 forceDir = liftVec + dragVec + liftDrag;

    auto mDD = 0.87f - (0.1f * cL) - (m_finLib.mainPitch.thickness / m_finLib.mainPitch.chord);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftDrag, DirectX::Colors::Lime, false, true);


    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, liftVec, DirectX::Colors::Lime, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, dragVec, DirectX::Colors::DeepPink, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, forceDir, DirectX::Colors::Fuchsia, false, true);
    //DebugPushDrawData(m_missileConsts.centerOfMassLocal, velocityNorm, DirectX::Colors::White, false, true);


    //m_debugData->ToggleDebugOnOverRide();
    //m_debugData->DebugPushUILineDecimalNumber("forceDir = ", forceDir.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("liftVec = ", liftVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("mDD = ", mDD, "");
    m_debugData->DebugPushUILineDecimalNumber("testDrag = ", testDrag, "");
    m_debugData->DebugPushUILineDecimalNumber("dragVec  = ", dragVec.Length(), "");
    m_debugData->ToggleDebugOff();

    //DirectX::SimpleMath::Vector3 forcePos = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forcePos = aMissile->guidance.centerOfPressureLocalPos;
    //DirectX::SimpleMath::Vector3 forcePos = DirectX::SimpleMath::Vector3(-0.15f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 centerOfMass = m_missileConsts.centerOfMassLocal;
    DirectX::SimpleMath::Vector3 forceAccum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;

    Utility::AddForceAtPoint(forceDir, forcePos, centerOfMass, forceAccum, torqueAccum);

    Utility::ForceAccum liftAccum;
    Utility::ForceAccum::ZeroValues(liftAccum);
    liftAccum.linear = forceAccum;
    liftAccum.torque = torqueAccum;

    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("forceDir = ", forceDir.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.linear = ", liftAccum.linear.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("liftAccum.torque = ", liftAccum.torque.Length(), "");
    m_debugData->ToggleDebugOff();

    return liftAccum;
}

DirectX::SimpleMath::Vector3 FireControl::RHSDragForceSymmetric(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();

    auto crossNorm = velocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);;
    crossNorm.Normalize();
    auto liftNorm = velocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX).Cross(velocityNorm);
    liftNorm.Normalize();

    auto velDot = DirectX::SimpleMath::Vector3::UnitX.Dot(velocityNorm);
    auto invsDot = 1.0f - abs(velDot);
    float sideSlip = DirectX::SimpleMath::Vector3::UnitX.Dot(velocityNorm);
    //float sideSlip2 = DirectX::SimpleMath::Vector3::UnitX.Dot(crossNorm);
    float sideSlip2 = velocityNorm.Dot(liftNorm);

    //float frontSurfaceArea = m_missileDimensions.y * m_missileDimensions.z * 2.0f;
    //float sideSurfaceArea = m_missileDimensions.x * m_missileDimensions.y * 2.0f;
    float frontSurfaceArea = (m_missileDimensions.y * m_missileDimensions.z) + m_finLib.mainPitch.frontArea + m_finLib.tailPitch.frontArea;
    float sideSurfaceArea = (m_missileDimensions.x * m_missileDimensions.y) + m_finLib.mainPitch.wingArea + m_finLib.tailPitch.wingArea;

    //float frontSurface = frontSurfaceArea * abs(velDot);
    float frontSurface = frontSurfaceArea;
    float sideSurface = sideSurfaceArea * abs(invsDot);

    //const float airSurfaceArea = frontSurface + sideSurface;
    float airSurfaceArea = frontSurface;
    if (m_isTestLiftInducedDragOn == false)
    {
        airSurfaceArea += sideSurface;
    }

    //float dragCoeffFront = abs(sideSlip) * 0.2f;
    float dragCoeffFront = sideSlip * 0.2f;
    float dragCoeffSide = abs(invsDot) * 5.9f;
    //float dragCoeffSum = dragCoeffFront + dragCoeffSide;
    float dragCoeffSum = dragCoeffFront;
    if (m_isTestLiftInducedDragOn == false)
    {
        dragCoeffSum += dragCoeffSide;
    }
    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("dragCoeffSum = ", dragCoeffSum, "");
    m_debugData->ToggleDebugOff();

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    const float dragCoefficient = dragCoeffSum;
    float velocity = aVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;
}

float FireControl::CalculateFinLiftCoefFlatOld(const float aAngleOfAttack)
{

    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    //const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    const float inputAngleRaw = (Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;

    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 0.15f;
    const float angKey1 = 5.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.0f;
    const float angKey2 = 10.0f;
    //const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));
    const float deltaKey2 = -((posKey2 - posKey0) / (angKey2 - angKey0));

    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    /*
    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    */
    if (inputAngle < angKey2)
    {
        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);

        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);
        cl = inputAngle * -deltaKey2;
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}


float FireControl::CalculateFinLiftCoefFlatOld2(const float aAngleOfAttack)
{

    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    //const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    const float inputAngleRaw = (Utility::ToDegrees(aAngleOfAttack));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;
    //const float inputAngle = (Utility::ToDegrees(aAngleOfAttack));
    
    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 0.15f;
    const float angKey1 = 5.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.0f;
    const float angKey2 = 10.0f;
    //const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));
    const float deltaKey2 = -((posKey2 - posKey0) / (angKey2 - angKey0));

    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;
    /*
    //////////////////
    if (inputAngle < -angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else if (inputAngle < -angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < -angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < -angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < -angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < -angKey2)
    {
        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);

        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);
        cl = inputAngle * -deltaKey2;
    }
    */
    //////////////////
   
    /*
    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    */
    if (inputAngle < angKey2)
    {
        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);

        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);
        cl = inputAngle * -deltaKey2;
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}


float FireControl::CalculateFinLiftCoefUpdate(const float aAngleOfAttack)
{
    float wrapAng = aAngleOfAttack;
    wrapAng = Utility::WrapAngle(wrapAng);
    // converting to degrees since all available NACA airfoil maps seem to obstain from radians
    //const float inputAngleRaw = abs(Utility::ToDegrees(aAngleOfAttack));
    const float inputAngleRaw = abs(Utility::ToDegrees(wrapAng));
    float inputAngleMod = inputAngleRaw;
    bool isAoAReversed = false;
    if (inputAngleRaw > 90.0f)
    {
        float testVal = inputAngleRaw - 90.0f;
        float testVal2 = 90.0f - testVal;
        inputAngleMod = testVal2;
        inputAngleMod = 90.0f - (inputAngleRaw - 90.0f);
        isAoAReversed = true;
    }
    const float inputAngle = inputAngleMod;

    const float posKey0 = 0.0f;
    const float angKey0 = 0.0f;
    const float deltaKey0 = 0.0f;

    const float posKey1 = 0.15f;
    const float angKey1 = 5.0f;
    const float deltaKey1 = (posKey1 - posKey0) / (angKey1 - angKey0);

    const float posKey2 = 1.0f;
    const float angKey2 = 10.0f;
    //const float deltaKey2 = -((posKey2 - posKey1) / (angKey2 - angKey1));
    const float deltaKey2 = -((posKey2 - posKey0) / (angKey2 - angKey0));

    const float posKey3 = 1.2f;
    const float angKey3 = 14.0f;
    const float deltaKey3 = -((posKey3 - posKey2) / (angKey3 - angKey2));

    const float posKey4 = 1.0f;
    const float angKey4 = 18.0f;
    const float deltaKey4 = -((posKey4 - posKey3) / (angKey4 - angKey3));

    const float posKey5 = 0.6f;
    const float angKey5 = 24.0f;
    const float deltaKey5 = -((posKey5 - posKey4) / (angKey5 - angKey4));

    const float posKey6 = 0.5f;
    const float angKey6 = 45.0f;
    const float deltaKey6 = -((posKey6 - posKey5) / (angKey6 - angKey5));

    const float posKey7 = 0.0f;
    const float angKey7 = 90.0f;
    const float deltaKey7 = -((posKey7 - posKey6) / (angKey7 - angKey6));

    float cl;
    float curveDeltaRate;
    float clTarget;

    /*
    if (inputAngle < angKey1)
    {
        cl = inputAngle * deltaKey1;
    }
    */

    if (inputAngle < angKey2)
    {
        //const float inputAngMod = inputAngle - angKey1;
        //cl = posKey1 - (inputAngMod * deltaKey2);
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey2);
    }
    else if (inputAngle < angKey3)
    {
        const float inputAngMod = inputAngle - angKey2;
        cl = posKey2 - (inputAngMod * deltaKey3);
    }
    else if (inputAngle < angKey4)
    {
        const float inputAngMod = inputAngle - angKey3;
        cl = posKey3 - (inputAngMod * deltaKey4);
    }
    else if (inputAngle < angKey5)
    {
        const float inputAngMod = inputAngle - angKey4;
        cl = posKey4 - (inputAngMod * deltaKey5);
    }
    else if (inputAngle < angKey6)
    {
        const float inputAngMod = inputAngle - angKey5;
        cl = posKey5 - (inputAngMod * deltaKey6);
    }
    else if (inputAngle < angKey7)
    {
        const float inputAngMod = inputAngle - angKey6;
        cl = posKey6 - (inputAngMod * deltaKey7);
    }
    else
    {
        // throw error
        int testBreak = 0;
        testBreak++;
        cl = 0.0f;
    }

    if (isAoAReversed == true)
    {
        cl *= -1.0f;
    }

    if (aAngleOfAttack >= 0.0f)
    //if (wrapAng >= 0.0f)
    {
        clTarget = cl;
    }
    else
    {
        clTarget = -cl;
    }

    return clTarget;
}


float FireControl::CalculateLiftInducedDrag(const float aLiftCoeff, const float aLiftForce, const float aVelocity, const float aWingSpan, const float aChord, const float aWingArea)
{
    const auto airDensity = m_environment->GetAirDensity();
    const auto pi = Utility::GetPi();
    const auto effiFactor = 1.0f;
    //const auto 
    auto dI = (aLiftForce * aLiftForce) / (0.5f * airDensity * (aVelocity * aVelocity) * pi * (aWingSpan * aWingSpan));
    auto AR = (aWingSpan * aWingSpan) / aWingArea;
    auto cDi1 = dI / (0.5f * airDensity * (aVelocity * aVelocity) * aWingArea);
    auto cDi2 = (aLiftCoeff * aLiftCoeff) / (pi * AR * effiFactor);

    auto testDrag = (aLiftForce * aLiftForce) / (0.5f * m_environment->GetAirDensity() * (aVelocity * aVelocity) * Utility::GetPi() * (aWingSpan * aWingSpan));
    //auto liftDrag = testDrag * -velocityNorm;


    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->DebugPushUILineDecimalNumber("dI       = ", dI, "");
    m_debugData->DebugPushUILineDecimalNumber("cDi1     = ", cDi1, "");
    m_debugData->DebugPushUILineDecimalNumber("cDi2     = ", cDi2, "");
    m_debugData->DebugPushUILineDecimalNumber("testDrag = ", testDrag, "");
    //m_debugData->DebugPushUILineDecimalNumber("", , "");

    m_debugData->ToggleDebugOff();
    return testDrag;
}


void FireControl::UpdateThrustVector(MissileData& aMissile)
{
    //DebugPushDrawData(m_missileConsts.thrustPosLocal, dirVec, DirectX::Colors::Red, false, true);

    if (m_missileConsts.isThrustVecOn == true)
    {

        aMissile.guidance.nav.thrustQuat = aMissile.guidance.nav.quatToTarg;

        auto thrustVec = DirectX::SimpleMath::Vector3::UnitX;
        thrustVec = DirectX::SimpleMath::Vector3::Transform(thrustVec, aMissile.guidance.nav.thrustQuat);
        aMissile.guidance.nav.thrustVec = thrustVec;
    }
    else
    {
        aMissile.guidance.nav.thrustVec = DirectX::SimpleMath::Vector3::UnitX;
        aMissile.guidance.nav.thrustQuat = DirectX::SimpleMath::Quaternion::Identity;
    }
}



void FireControl::UpdateFlightState(MissileData& aMissile, const double aTimeDelta)
{
    // check for detonation / explosion
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
        if (aMissile.guidance.isRocketFired == true)
        {
            aMissile.guidance.flightStateCurrent = FlightState::FLIGHTSTATE_CLIMBOUT;
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
}

void FireControl::UpdateFlightDataDependantVars(MissileData& aMissile, const double aTimeDelta)
{
    // laser targeting
    if (aMissile.guidance.isTargetingLaserOn == false && aMissile.projectileData.time >= m_missileConsts.laserDepoyDelay)
    {
        aMissile.guidance.isTargetingLaserOn = true;
    }

    // rocket toggle
    if (aMissile.guidance.isRocketFired == false && aMissile.projectileData.time >= m_missileConsts.rocketFireDelay)
    {
        aMissile.guidance.isRocketFired = true;
    }
    // rocket center of thrust
    aMissile.guidance.centerOfThrustLocalPos = m_missileConsts.thrustPosLocal;
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
    aMissile.guidance.throttlePercentage = rocketThrottle;

    // fin deploy percent and center of pressure
    if (aMissile.guidance.isFinsDeployEnd == true)
    {
        aMissile.guidance.finDeployPercent = 1.0f;
        aMissile.guidance.centerOfThrustLocalPos = m_missileConsts.thrustPosLocal;
        aMissile.guidance.centerOfPressureLocalPos = m_missileConsts.centerOfPressureFullDeployLocal;
    }
    else
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
                    aMissile.guidance.isFinsDeployEnd = true;
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

        DirectX::SimpleMath::Vector3 centerOfPressure = m_missileConsts.centerOfPressureBasePosLocal;
        centerOfPressure += m_missileConsts.centerOfPressureFullFinDeployOffset * aMissile.guidance.finDeployPercent;
        aMissile.guidance.centerOfPressureLocalPos = centerOfPressure;
    }
}

void FireControl::UpdateFlightDataIndependentVars(MissileData& aMissile, const double aTimeDelta)
{
    aMissile.projectileData.time += static_cast<float>(aTimeDelta);
    aMissile.guidance.timeStepDelta = static_cast<float>(aTimeDelta);

    aMissile.guidance.altitude = aMissile.projectileData.q.position.y - (m_environment->GetTerrainHeightAtPos(aMissile.projectileData.q.position));

    m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity, aMissile.guidance.targetForward);
    float prevDistance = aMissile.guidance.targetDistance;
    aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();
    aMissile.guidance.targetDistanceDelta = prevDistance - aMissile.guidance.targetDistance;
    aMissile.guidance.closureRate = aMissile.guidance.targetDistanceDelta / aTimeDelta;
    aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;

    aMissile.guidance.localVel = DirectX::SimpleMath::Vector3::Transform(aMissile.projectileData.q.velocity, aMissile.projectileData.inverseAlignmentQuat);

    if (aMissile.guidance.localVel.x > 0.0f)
    {
        aMissile.guidance.isVelocityForward = true;
    }
    else
    {
        aMissile.guidance.isVelocityForward = false;
    }
}