#include "pch.h"
#include "Vehicle.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

Vehicle::~Vehicle()
{
    //delete m_fireControl;
}

void Vehicle::CycleFireControlAmmo()
{
    m_fireControl->CycleLoadedAmmo();
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateBuoyancyForce(const HeliData& aVehicleData)
{
    const DirectX::SimpleMath::Vector3 gravForce = -m_environment->GetGravityVec();
    float altitude = aVehicleData.altitude;
    bool breakToggle = false;

    float midCurveBound = aVehicleData.hoverRangeMid;
    const float lowerCurveBound = aVehicleData.hoverRangeLower;
    const float upperCurveBound = aVehicleData.hoverRangeUpper;
    const float curveAdjustVal = lowerCurveBound;
    DirectX::SimpleMath::Vector3 testDimensions = DirectX::SimpleMath::Vector3(8.0f, 4.0f, 4.0f);
    const float vehicleVolumeMax = testDimensions.x * testDimensions.y * testDimensions.z;
    const float immersedDensityNeutralAtHalfDepth = aVehicleData.mass / (vehicleVolumeMax * 0.5f);
    float immersedRange = upperCurveBound - lowerCurveBound;
    float immersedPos = altitude - curveAdjustVal;
    float immersedRatio;

    if (m_heli.buoyancyForce.y >= 33319.0f)
    {
        int testBreak = 0;
        testBreak++;
    }

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
        int testBreak2 = 0;
        //testDensity = immersedDensityNeutralAtHalfDepth * (immersedVolumeRatio);
        stateVal = 4;
    }

    DirectX::SimpleMath::Vector3 buoyancyForce = testDensity * immersedVolume * gravForce;

    m_debugData->ToggleDebugOnOverRide();

    m_debugData->DebugPushUILineDecimalNumber("immersedRatio = ", immersedRatio, "");
    m_debugData->ToggleDebugOff();
    

    /*
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.x = ", buoyancyForce.x, "");
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.y = ", buoyancyForce.y, "");
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.z = ", buoyancyForce.z, "");
    */

    return buoyancyForce;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    //localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    //const float length = 8.0f;
    //const float width = 4.0f;
    //const float height = 2.0f;
    const float length = m_inertiaModelX;
    const float width = m_inertiaModelZ;
    const float height = m_inertiaModelY;

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

    float angVelocityF = aAngVelocity.Length();
    float angDragCoefficient = m_angDragCoefficient;
    float angAirDensity = m_environment->GetAirDensity() * m_airDensityDragMod;
    float angFrontSurfaceArea = m_heli.area;
    float radius = radiusSum;
    angFrontSurfaceArea = airSurfaceArea;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * airSurfaceArea * angAirDensity)));
    //angularDrag = aAngVelocity * -powf(m_angularDragMod, aTimeStep);

    return angularDrag;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateDragLinear(const DirectX::SimpleMath::Vector3 aVelocity)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aVelocity;
    localVelocityNorm.Normalize();
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, m_heli.alignmentInverse);

    float frontSurfaceArea = m_inertiaModelY * m_inertiaModelZ;
    float sideSurfaceArea = m_inertiaModelX * m_inertiaModelY;
    float topSurfaceArea = m_inertiaModelX * m_inertiaModelZ;

    /*
    frontSurfaceArea = m_heli.area;
    sideSurfaceArea = m_heli.area;
    topSurfaceArea = m_heli.area;
    */

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);


    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    float airSurfaceArea = frontSurface + sideSurface + topSurface;


    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = m_heli.dragCoefficient;
    float velocity = aVelocity.Length();
    //float velocity = aNewQVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateHoverDriveForce(const struct HeliData& aHeli)
{
    float zForce = -aHeli.controlInput.cyclicInputRoll;
    zForce = zForce / aHeli.controlInput.cyclicInputMax;
    float xForce = aHeli.controlInput.cyclicInputPitch;
    xForce = xForce / aHeli.controlInput.cyclicInputMax;
    DirectX::SimpleMath::Vector3 driveForce = DirectX::SimpleMath::Vector3::Zero;
    driveForce.x = xForce * aHeli.hoverDriveMagMax;
    driveForce.z = zForce * aHeli.hoverDriveMagMax;
    driveForce = DirectX::SimpleMath::Vector3::Transform(driveForce, aHeli.alignment);
    driveForce.y = 0.0f;

    return driveForce;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateHoverTorqueForce(const struct HeliData& aHeli, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 torqueForceSum = DirectX::SimpleMath::Vector3::Zero;
    const float liftForce = 50.0f;
    return torqueForceSum;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateImpactLinearForceSum(const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 impactForceSum = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 impulseForce = m_heli.testCollisionImpulseForce.directionNorm * m_heli.testCollisionImpulseForce.currentMagnitude;
    impactForceSum += impulseForce;
    impactForceSum += m_heli.testCollisionVelocityUpdate;
    return impactForceSum;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateImpactTorqueSum(const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 impactTorqueSum = DirectX::SimpleMath::Vector3::Zero;

    Utility::Torque torqueSum;
    torqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    torqueSum.magnitude = 0.0f;

    //Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm, (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm));
    Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_heli.testCollisionImpulseForce.torqueArm, (m_heli.testCollisionImpulseForce.currentMagnitude * m_heli.testCollisionImpulseForce.directionNorm));

    //torqueSum.axis += torqueImpulse.axis * torqueImpulse.magnitude;
    //torqueSum.magnitude += torqueImpulse.magnitude;
    torqueSum.axis = torqueImpulse.axis;
    torqueSum.magnitude = torqueImpulse.magnitude;

    impactTorqueSum = torqueSum.axis * torqueSum.magnitude;
    return impactTorqueSum;
}

float Vehicle::CalculateLiftCoefficient(const float aAngle)
{
    //const float angleMax = m_heli.mainRotor.pitchAngleMax;
    const float angleMax = Utility::ToRadians(24.0f);
    const float angleMin = m_heli.mainRotor.pitchAngleMin;
    float inputAngle = aAngle;
    if (inputAngle > angleMax)
    {
        inputAngle = angleMax;
    }
    if (inputAngle < angleMin)
    {
        inputAngle = angleMin;
    }

    const float currentCurvePos = (inputAngle / angleMax);

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

    return ClTarget;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateSelfRightingForce(const HeliData& aHeliData)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        //updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);

    const float torqueMax = m_stabilityTorqueForceMax;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;

    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;

    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicNormLocal.z * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicNormLocal.x * torqueMax) * m_heli.up;

    lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    longitudinalForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up;

    //lateralForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up;
    //longitudinalForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;

    //lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //longitudinalForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;

    //m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch
    //lateralForce = (m_heli.controlInput.cyclicNormWorld.z * torqueMax) * m_heli.up;
    //longitudinalForce = (-m_heli.controlInput.cyclicNormWorld.x * torqueMax) * m_heli.up;

    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);

    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    ///////////

    ///////////
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);

    Utility::Torque torqueSum1;
    torqueSum1.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSum1.axis.Normalize();
    torqueSum1.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    if (m_debugToggle == true && 1 == 1)
    {
        if (m_heli.controlInput.cyclicInputRoll > 0.0f) // left
        {
            if (m_heli.physicsPointLeft.y < m_heli.physicsPointRight.y)
            {
                lateralTorque.magnitude *= (1.0f - lateralDot);
                lateralTorqueLocal.magnitude *= (1.0f - lateralDot);
            }
        }
        else if (m_heli.controlInput.cyclicInputRoll < 0.0f) // right
        {
            if (m_heli.physicsPointLeft.y > m_heli.physicsPointRight.y)
            {
                lateralTorque.magnitude *= (1.0f + lateralDot);
                lateralTorqueLocal.magnitude *= (1.0f + lateralDot);
            }
        }

        if (m_heli.controlInput.cyclicInputPitch > 0.0f) // forward
        {
            if (m_heli.physicsPointFront.y < m_heli.physicsPointRear.y)
            {
                longitudinalTorque.magnitude *= (1.0f - longitudinalDot);
                longitudinalTorqueLocal.magnitude *= (1.0f - longitudinalDot);
            }
        }
        else if (m_heli.controlInput.cyclicInputPitch < 0.0f) // back
        {
            if (m_heli.physicsPointFront.y > m_heli.physicsPointRear.y)
            {
                longitudinalTorque.magnitude *= (1.0f + longitudinalDot);
                longitudinalTorqueLocal.magnitude *= (1.0f + longitudinalDot);
            }
        }
    }

    /*
    if (m_debugToggle == true)
    {
        //lateralTorque.magnitude = lateralTorque.magnitude * (1.0f - abs(lateralDot));
    }

    DirectX::SimpleMath::Vector3 torqueArmLateralPos = m_heli.q.position + torqueArmLateral;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalPos = m_heli.q.position + torqueArmLongitudinal;

    DirectX::SimpleMath::Vector3 forceSumTest = DirectX::SimpleMath::Vector3::Zero;
    forceSumTest.x = lateralForce.x + longitudinalForce.x;
    forceSumTest.y = lateralForce.y + longitudinalForce.y;
    forceSumTest.z = lateralForce.z + longitudinalForce.z;

    DirectX::SimpleMath::Vector3 momentSumTest = DirectX::SimpleMath::Vector3::Zero;
    momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(longitudinalForce));
    float momentSumTestLength = (torqueArmLateral.Cross(lateralForce)).Length() + (torqueArmLongitudinal.Cross(longitudinalForce)).Length();

    DirectX::SimpleMath::Vector3 latLongForceSum = lateralForce + longitudinalForce;
    */

    Utility::Torque torqueSum;
    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    DirectX::SimpleMath::Vector3 torqVec = DirectX::SimpleMath::Vector3::Zero;
    torqVec = torqueSum.axis * torqueSum.magnitude;

    torqVec = DirectX::SimpleMath::Vector3::Transform(torqVec, m_heli.alignmentInverse);
    return torqVec;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateStabilityTorqueLocal(const HeliData& aHeliData, const float aTimeStep)
{ 
    
    DirectX::SimpleMath::Vector3 testArm = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 testForce = DirectX::SimpleMath::Vector3::UnitZ;
    Utility::Torque testTorque = Utility::GetTorqueForce(DirectX::SimpleMath::Vector3::UnitY, DirectX::SimpleMath::Vector3::UnitZ);

    DirectX::SimpleMath::Vector3 testTorqueVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.3, 0.0, 0.3);
    DirectX::SimpleMath::Vector3 point = DirectX::SimpleMath::Vector3(0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector3 pt = point;
    pt -= position;

    DirectX::SimpleMath::Vector3 torqueAccum = DirectX::SimpleMath::Vector3::Zero;
    testTorqueVec = pt; 
    testTorqueVec = testTorqueVec.Cross(testForce);
    //% testForce;

    torqueAccum += testTorqueVec;
    //DirectX::SimpleMath::Quaternion testQuat = aHeliData.q.angularQuat;
    DirectX::SimpleMath::Quaternion testQuat = aHeliData.alignmentQuat;
    //m_debugData->DebugPushUILineDecimalNumber("testQuat.x ", testQuat.x, "");
    //m_debugData->DebugPushUILineDecimalNumber("testQuat.y ", testQuat.y, "");
    //m_debugData->DebugPushUILineDecimalNumber("testQuat.z ", testQuat.z, "");
    //m_debugData->DebugPushUILineDecimalNumber("testQuat.w ", testQuat.w, "");

    //aHeliData.localGravityTorqueArmPos == (0.0. 1.0, 0.0)
    DirectX::SimpleMath::Vector3 cogArm = (aHeliData.localCenterOfMass - aHeliData.localGravityTorqueArmPos) * 3.0f;
    DirectX::SimpleMath::Vector3 cogForce = aHeliData.gravity;
    cogForce = DirectX::SimpleMath::Vector3::Transform(cogForce, aHeliData.alignment);
    cogForce *= m_gravTorqueModTest;
    cogForce = (aHeliData.gravity * m_gravTorqueModTest) * (1.0f * abs(testQuat.w));
    DirectX::SimpleMath::Vector3 cogForceWorld = cogForce;
    cogForceWorld = DirectX::SimpleMath::Vector3::Transform(cogForceWorld, aHeliData.alignmentInverse);

    DirectX::SimpleMath::Vector3 cogTorqueArmWorld = cogArm;
    cogTorqueArmWorld = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld, aHeliData.alignment);
    Utility::Torque cogTorque = Utility::GetTorqueForce(cogTorqueArmWorld, cogForce);


    DirectX::SimpleMath::Vector3 armHinge = cogTorqueArmWorld;
    armHinge *= 1.0f;
    armHinge += aHeliData.q.position;
    //m_debugData->PushDebugLineScaled(armHinge, cogForce, 0.0f, 0.03f, 0.0f, DirectX::Colors::Lavender);
    //m_debugData->PushDebugLine(aHeliData.q.position, cogTorqueArmWorld, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLineScaled(aHeliData.q.position, cogTorqueArmWorld, 0.0f, 1.0f, 0.0f, DirectX::Colors::Red);


    DirectX::SimpleMath::Vector3 cogArm2 = (DirectX::SimpleMath::Vector3::UnitY * 3.0f) * (1.1f - abs(testQuat.w));
    //m_debugData->DebugPushUILineDecimalNumber("cogArm2.Length() ", cogArm2.Length(), "");
    DirectX::SimpleMath::Vector3 cogForce2 = aHeliData.gravity;
    cogForce2 = DirectX::SimpleMath::Vector3::Transform(cogForce2, aHeliData.alignment);
    cogForce2 *= m_gravTorqueModTest;
    cogForce2 = -aHeliData.gravity * m_gravTorqueModTest;
    DirectX::SimpleMath::Vector3 cogForceWorld2 = cogForce2;
    cogForceWorld2 = DirectX::SimpleMath::Vector3::Transform(cogForceWorld2, aHeliData.alignmentInverse);
    DirectX::SimpleMath::Vector3 cogTorqueArmWorld2 = cogArm2;
    //cogTorqueArmWorld2 = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld2, aHeliData.alignment);
    cogTorqueArmWorld2 = DirectX::SimpleMath::Vector3::Transform(cogTorqueArmWorld2, aHeliData.alignmentInverse);
    Utility::Torque cogTorque2 = Utility::GetTorqueForce(cogTorqueArmWorld2, cogForce2);
    DirectX::SimpleMath::Vector3 stabilityTorque2 = cogTorque2.axis * cogTorque2.magnitude;


    DirectX::SimpleMath::Vector3 armHinge2 = cogTorqueArmWorld2;
    armHinge2 *= 1.0f;
    armHinge2 += aHeliData.q.position;
    //m_debugData->PushDebugLine(aHeliData.q.position, cogTorqueArmWorld2, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLineScaled(aHeliData.q.position, cogTorqueArmWorld2, 0.0f, 1.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLineScaled(armHinge2, cogForce2, 0.0f, 0.03f, 0.0f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 stabilityTorque = DirectX::SimpleMath::Vector3::Zero;
    stabilityTorque = cogTorque.axis * cogTorque.magnitude;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    stabilityTorque = DirectX::SimpleMath::Vector3::Zero;
    stabilityTorque2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityForce = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityForce2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityForce3 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityTorque3 = DirectX::SimpleMath::Vector3::Zero;
    Utility::AddForceAtPoint(cogForceWorld, (aHeliData.localGravityTorqueArmPos * 3.0f), aHeliData.localCenterOfMass, stabilityForce, stabilityTorque);
    Utility::AddForceAtPoint(cogForceWorld2, cogArm2, aHeliData.localCenterOfMass, stabilityForce2, stabilityTorque2);
    //stabilityTorque *= -1.0f;
    //stabilityTorque2 *= -1.0f;
    //m_debugData->DebugPushUILineDecimalNumber("stabilityTorque.Length()  ", stabilityTorque.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("stabilityTorque2.Length() ", stabilityTorque2.Length(), "");

    //DirectX::SimpleMath::Vector3 localizedGravity = -m_heli.gravity * m_gravTorqueModTest;
    DirectX::SimpleMath::Vector3 localizedGravity = -m_heli.gravity * m_stabilityTorqueForceMax2;

    localizedGravity = DirectX::SimpleMath::Vector3::Transform(localizedGravity, m_heli.alignmentInverse);
    //localizedGravity = DirectX::SimpleMath::Vector3::Transform(localizedGravity, m_heli.alignment);
    Utility::AddForceAtPoint(localizedGravity, (aHeliData.localGravityTorqueArmPos * m_gravStabilityArmMod), aHeliData.localCenterOfMass, stabilityForce3, stabilityTorque3);
    //return stabilityTorque + stabilityTorque2;
    m_debugData->DebugPushUILineDecimalNumber("stabilityTorque3.Length() ", stabilityTorque3.Length(), "");
    DirectX::SimpleMath::Vector3 worldStabilityTorque = stabilityTorque3;
    worldStabilityTorque = DirectX::SimpleMath::Vector3::Transform(worldStabilityTorque, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, worldStabilityTorque, 10.0f, 0.0f, DirectX::Colors::Gray);
    return stabilityTorque3;
}

float Vehicle::CalculateWindVaningVal(const HeliData& aHeliData)
{
    DirectX::SimpleMath::Vector3 localizedAirVelocity = -aHeliData.q.velocity;
    DirectX::SimpleMath::Matrix localizeMat = aHeliData.alignment;
    localizeMat = localizeMat.Invert();
    localizedAirVelocity = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocity, localizeMat);

    DirectX::SimpleMath::Vector3 airVelocityNormXZ = localizedAirVelocity;
    airVelocityNormXZ.y = 0.0f;
    airVelocityNormXZ.Normalize();
    float ratio = airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ);

    float v = aHeliData.q.velocity.Length();
    float airDensity = aHeliData.airDensity;
    float dragCoefficient = aHeliData.dragCoefficient;
    float surfaceArea = aHeliData.area;
    float dragResistance = 0.5f * airDensity * surfaceArea * dragCoefficient * v * v;
    float windVaning = -ratio * dragResistance * 0.0000005f;
    return windVaning;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateWindVaningTorqueForce(const HeliData& aHeliData)
{
    DirectX::SimpleMath::Vector3 localizedAirVelocity = -aHeliData.q.velocity;
    DirectX::SimpleMath::Vector3 localizedAirVelocityTest1 = localizedAirVelocity;
    DirectX::SimpleMath::Vector3 localizedAirVelocityTest2 = localizedAirVelocity;
    DirectX::SimpleMath::Matrix localizeMat = aHeliData.alignment;
    localizeMat = localizeMat.Invert();
    localizedAirVelocity = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocity, localizeMat);

    DirectX::SimpleMath::Vector3 localizedAirVelocityLateral = localizedAirVelocity;
    localizedAirVelocityLateral.x = 0.0f;
    //localizedAirVelocityLateral.y = 0.0f;
    DirectX::SimpleMath::Vector3 localizedAirVelocityLongitudinal = localizedAirVelocity;
    localizedAirVelocityLongitudinal.z = 0.0f;
    //localizedAirVelocityLongitudinal.y = 0.0f;

    DirectX::SimpleMath::Vector3 airVelocityNormXZ = localizedAirVelocity;
    airVelocityNormXZ.y = 0.0f;
    airVelocityNormXZ.Normalize();
    float ratio = airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ);

    float v = aHeliData.q.velocity.Length();
    float airDensity = aHeliData.airDensity;
    float dragCoefficient = aHeliData.dragCoefficient;
    float surfaceArea = aHeliData.area;
    float dragResistance = 0.5f * airDensity * surfaceArea * dragCoefficient * v * v;
    float windVaning = -ratio * dragResistance * 0.0000005f;

    //m_debugData->DebugPushUILineDecimalNumber("windVaning = ", windVaning, "");
    const float dragVal = airDensity * surfaceArea * dragCoefficient;
    //m_debugData->DebugPushUILineDecimalNumber("dragVal = ", dragVal, "");

    DirectX::SimpleMath::Vector3 lateralCross = localizedAirVelocity.Cross(DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 longitudunalCross = localizedAirVelocity.Cross(DirectX::SimpleMath::Vector3::UnitZ);
    //m_debugData->PushDebugLine(m_heli.q.position, lateralCross, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, longitudunalCross, 10.0f, 0.0f, DirectX::Colors::Blue);

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

    //m_debugData->DebugPushUILineDecimalNumber("isVelocityForward = ", static_cast<float>(isVelocityForward), "");
    //m_debugData->DebugPushUILineDecimalNumber("isVelocityLeft = ", static_cast<float>(isVelocityLeft), "");



    DirectX::SimpleMath::Vector3 longitudeTorqueArm;
    if (isVelocityForward == true)
    {
        longitudeTorqueArm = aHeliData.localPhysicsPointRear - aHeliData.localCenterOfMass;
        //longitudeTorqueArm = aHeliData.physicsPointRear - aHeliData.centerOfMass;
    }
    else
    {
        longitudeTorqueArm = aHeliData.localPhysicsPointFront - aHeliData.localCenterOfMass;
        //longitudeTorqueArm = aHeliData.physicsPointFront - aHeliData.centerOfMass;
    }

    DirectX::SimpleMath::Vector3 latTorqueArm;
    if (isVelocityLeft == true)
    {
        latTorqueArm = aHeliData.localPhysicsPointRight - aHeliData.localCenterOfMass;
        //latTorqueArm = aHeliData.physicsPointRight - aHeliData.centerOfMass;
    }
    else
    {
        latTorqueArm = aHeliData.localPhysicsPointLeft - aHeliData.localCenterOfMass;
        //latTorqueArm = aHeliData.physicsPointLeft - aHeliData.centerOfMass;
    }
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aHeliData.right, aHeliData.up);

    const float windMod = 1.0f;
    float latDotMod = abs(airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitZ) * windMod);
    float longDotMod = abs(airVelocityNormXZ.Dot(DirectX::SimpleMath::Vector3::UnitX) * windMod);

    //m_debugData->DebugPushUILineDecimalNumber("latDotMod = ", latDotMod, "");
    //m_debugData->DebugPushUILineDecimalNumber("longDotMod = ", longDotMod, "");


    //Utility::Torque lateralTorque = Utility::GetTorqueForce(latTorqueArm, (localizedAirVelocity * latDotMod));
    //Utility::Torque longitudeTorque = Utility::GetTorqueForce(longitudeTorqueArm, (localizedAirVelocity * longDotMod));
    Utility::Torque lateralTorque = Utility::GetTorqueForce(latTorqueArm, (localizedAirVelocityLateral * latDotMod));
    Utility::Torque longitudeTorque = Utility::GetTorqueForce(longitudeTorqueArm, (localizedAirVelocityLongitudinal * longDotMod));
    //Utility::Torque lateralTorque = Utility::GetTorqueForce(latTorqueArm, (-aHeliData.q.velocity * latDotMod));
    //Utility::Torque longitudeTorque = Utility::GetTorqueForce(longitudeTorqueArm, (-aHeliData.q.velocity * longDotMod));

    //m_debugData->PushDebugLine(m_heli.q.position, latTorqueArm, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, longitudeTorqueArm, 10.0f, 0.0f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 testLongTorque = longitudeTorque.axis * longitudeTorque.magnitude;
    DirectX::SimpleMath::Vector3 testLatTorque = lateralTorque.axis * lateralTorque.magnitude;
    testLongTorque = DirectX::SimpleMath::Vector3::Transform(testLongTorque, updateMat);
    testLatTorque = DirectX::SimpleMath::Vector3::Transform(testLatTorque, updateMat);

    localizedAirVelocityLateral = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocityLateral, updateMat);
    localizedAirVelocityLongitudinal = DirectX::SimpleMath::Vector3::Transform(localizedAirVelocityLongitudinal, updateMat);

    /*
    m_debugData->PushDebugLine(m_heli.q.position, localizedAirVelocityLateral, 7.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, localizedAirVelocityLongitudinal, 7.0f, 0.0f, DirectX::Colors::Orchid);
    m_debugData->DebugPushUILineDecimalNumber("localizedAirVelocityLateral.Length() = ", localizedAirVelocityLateral.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("localizedAirVelocityLongitudinal.Length() = ", localizedAirVelocityLongitudinal.Length(), "");

    m_debugData->PushDebugLine(m_heli.q.position, testLatTorque, 10.0f, 0.0f, DirectX::Colors::Aqua);
    m_debugData->PushDebugLine(m_heli.q.position, testLongTorque, 10.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->DebugPushUILineDecimalNumber("testLatTorque.Length() = ", testLatTorque.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("testLongTorque.Length() = ", testLongTorque.Length(), "");
    */

    DirectX::SimpleMath::Vector3 windVaningUpdateForce = DirectX::SimpleMath::Vector3::Zero;
    //windVaningUpdateForce.x = 5.0f;
    //m_debugData->DebugPushUILineDecimalNumber("windVaningUpdateForce = ", windVaningUpdateForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 10.0f, 0.0f, DirectX::Colors::Red);

    windVaningUpdateForce = (lateralTorque.axis * lateralTorque.magnitude) + (longitudeTorque.axis * longitudeTorque.magnitude);
    windVaningUpdateForce = (testLatTorque)+(testLongTorque);

    //windVaningUpdateForce = DirectX::SimpleMath::Vector3::Transform(windVaningUpdateForce, updateMat);
    //m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 10.0f, 0.0f, DirectX::Colors::Yellow);
    //windVaningUpdateForce = DirectX::SimpleMath::Vector3::Zero;

    //m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 15.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->DebugPushUILineDecimalNumber("windVaningUpdateForce.Length() = ", windVaningUpdateForce.Length(), "");

    return windVaningUpdateForce;
}

void Vehicle::DebugInputVelocityZero()
{
    //m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    const float speed = m_heli.q.velocity.Length();
    DirectX::SimpleMath::Vector3 antiVelocity = m_heli.q.velocity * -1.0f;
    DirectX::SimpleMath::Vector3 brakeNorm = antiVelocity;
    brakeNorm.Normalize();
    const float brakeVal = 0.02f;

    DirectX::SimpleMath::Vector3 brakeForce = antiVelocity * brakeVal;
    //m_heli.q.velocity += brakeForce;
    m_heli.controlInput.isBrakePressed = true;

    //m_heli.q.angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    //m_heli.q.angularMomentum = DirectX::SimpleMath::Vector3::Zero;
}

void Vehicle::DebugToggle()
{
    if (m_debugToggle == false)
    {
        m_debugToggle = true;
    }
    else
    {
        m_debugToggle = false;
    }
}

void Vehicle::DebugToggle2()
{
    if (m_debugToggle2 == false)
    {
        m_debugToggle2 = true;
    }
    else
    {
        m_debugToggle2 = false;
    }
}

void Vehicle::DebugToggle3()
{
    if (m_debugToggle3 == false)
    {
        m_debugToggle3 = true;
    }
    else
    {
        m_debugToggle3 = false;
    }
}

void Vehicle::DebugToggle4()
{
    if (m_debugToggle4 == false)
    {
        m_debugToggle4 = true;
    }
    else
    {
        m_debugToggle4 = false;
    }
}

void Vehicle::DebugToggle5()
{
    if (m_debugToggle5 == false)
    {
        m_debugToggle5 = true;
    }
    else
    {
        m_debugToggle5 = false;
    }
}

void Vehicle::DebugToggle6()
{
    if (m_debugToggle6 == false)
    {
        m_debugToggle6 = true;
    }
    else
    {
        m_debugToggle6 = false;
    }
}

void Vehicle::DebugToggle7()
{
    if (m_debugToggle7 == false)
    {
        m_debugToggle7 = true;
    }
    else
    {
        m_debugToggle7 = false;
    }
}

void Vehicle::DebugToggle8()
{
    if (m_debugToggle8 == false)
    {
        m_debugToggle8 = true;
    }
    else
    {
        m_debugToggle8 = false;
    }
}

void Vehicle::DebugToggle9()
{
    if (m_debugToggle9 == false)
    {
        m_debugToggle9 = true;
    }
    else
    {
        m_debugToggle9 = false;
    }
}

void Vehicle::DebugToggle0()
{
    if (m_debugToggle0 == false)
    {
        m_debugToggle0 = true;
    }
    else
    {
        m_debugToggle0 = false;
    }
}

void Vehicle::DrawVehicleProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_fireControl->DrawFireControlObjects(aView, aProj);
}

void Vehicle::DrawVehicleProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    m_fireControl->DrawFireControlObjects2(aView, aProj, aEffect, aInputLayout);
}

void Vehicle::FireWeapon()
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
        DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDirWorld();
        DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
        //velocity = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 up = m_modelController->GetWeaponUpWorld();
        //DirectX::SimpleMath::Vector3 weaponUp = m_modelController->Get
        m_fireControl->FireSelectedAmmo(pos, launchDir, velocity, up);
        DirectX::SimpleMath::Vector3 launchDirLocal = m_modelController->GetWeaponDirLocal();
        Utility::ImpulseForce recoil = m_fireControl->GetRecoilImpulseForce(-launchDir);

        //launchDir = DirectX::SimpleMath::Vector3::Transform(launchDir, m_heli.alignmentInverse);

        DirectX::SimpleMath::Vector3 weaponTorqueArmLocal = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        weaponTorqueArmLocal = DirectX::SimpleMath::Vector3::Transform(weaponTorqueArmLocal, m_heli.alignmentInverse);
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - m_heli.centerOfMass;
        //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        DirectX::SimpleMath::Vector3 torqueForceNorm = -launchDirLocal;
        weaponForce.Normalize();
        //float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
        //weaponForce *= weaponForceMag;
        //weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        recoil.torqueArm = weaponTorqueArmLocal;
        recoil.torqueForceNorm = torqueForceNorm;
        //m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
        //m_testImpulseForce = recoil;
    }
}

void Vehicle::FireWeapon(std::shared_ptr<DirectX::SoundEffectInstance> aFireFx)
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
        DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDirWorld();
        DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
        //velocity = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 up = m_modelController->GetWeaponUpWorld();
        //DirectX::SimpleMath::Vector3 weaponUp = m_modelController->Get
        m_fireControl->FireSelectedAmmo(pos, launchDir, velocity, up);
        DirectX::SimpleMath::Vector3 launchDirLocal = m_modelController->GetWeaponDirLocal();
        Utility::ImpulseForce recoil = m_fireControl->GetRecoilImpulseForce(-launchDir);

        //launchDir = DirectX::SimpleMath::Vector3::Transform(launchDir, m_heli.alignmentInverse);

        DirectX::SimpleMath::Vector3 weaponTorqueArmLocal = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        weaponTorqueArmLocal = DirectX::SimpleMath::Vector3::Transform(weaponTorqueArmLocal, m_heli.alignmentInverse);
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - m_heli.centerOfMass;
        //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        DirectX::SimpleMath::Vector3 torqueForceNorm = -launchDirLocal;
        weaponForce.Normalize();
        //float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
        //weaponForce *= weaponForceMag;
        //weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        recoil.torqueArm = weaponTorqueArmLocal;
        recoil.torqueForceNorm = torqueForceNorm;
        //m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
        //m_testImpulseForce = recoil;
    }
}

void Vehicle::FireWeapon(std::shared_ptr<Utility::SoundFx> aFireFx)
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
        DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDirWorld();
        DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
        DirectX::SimpleMath::Vector3 up = m_modelController->GetWeaponUpWorld();
        //m_fireControl->FireSelectedAmmo(pos, launchDir, velocity, up);

        //aFireFx->emitter->SetPosition(pos);
        //aFireFx->emitter->SetVelocity(velocity);
        //aFireFx->emitter->SetOmnidirectional();
        //aFireFx->emitter->SetOrientation(launchDir, up);
        
        aFireFx->pos = pos;
        aFireFx->up = up;
        aFireFx->isDestroyTrue = false;
        aFireFx->forward = launchDir;

        
        aFireFx->fx->Play(true);

        //m_fireControl->FireSelectedWithAudio(pos, launchDir, velocity, up, aFireFx);
        m_fireControl->FireSelectedWithAudio(pos, launchDir, velocity, up, aFireFx, false);

        // weapon recoil
        DirectX::SimpleMath::Vector3 launchDirLocal = m_modelController->GetWeaponDirLocal();
        Utility::ImpulseForce recoil = m_fireControl->GetRecoilImpulseForce(-launchDir);

        DirectX::SimpleMath::Vector3 weaponTorqueArmLocal = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        weaponTorqueArmLocal = DirectX::SimpleMath::Vector3::Transform(weaponTorqueArmLocal, m_heli.alignmentInverse);
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - m_heli.centerOfMass;
        
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        DirectX::SimpleMath::Vector3 torqueForceNorm = -launchDirLocal;
        weaponForce.Normalize();
 
        recoil.torqueArm = weaponTorqueArmLocal;
        recoil.torqueForceNorm = torqueForceNorm;
        //m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
        //m_testImpulseForce = recoil;
    }
}

void Vehicle::FireWeapon(std::shared_ptr<Utility::SoundFx> aFireFx, std::shared_ptr<Utility::SoundFx> aRocketFx)
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        DirectX::SimpleMath::Vector3 pos;
        DirectX::SimpleMath::Vector3 posWorld;
        DirectX::SimpleMath::Vector3 launchDir;
        DirectX::SimpleMath::Vector3 launchDirWorld;
        DirectX::SimpleMath::Vector3 velocity;
        DirectX::SimpleMath::Vector3 velocityWorld;
        DirectX::SimpleMath::Vector3 up;
        DirectX::SimpleMath::Vector3 upWorld;
        DirectX::SimpleMath::Vector3 recoilPosLocal;
        DirectX::SimpleMath::Vector3 recoilPosWorld;
        if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_RIGHT)
        { 
            posWorld = m_modelController->GetMissileTubePosRight();
            pos = m_modelController->GetLocalizedTubeRightPos();
            launchDir = m_modelController->GetMissileTubeTurretLocalRightDir();
            launchDirWorld = m_modelController->GetMissileTubeDirRight();
            velocity = m_heli.q.velocity;
            up = m_modelController->GetMissileTubeTurretLocalRightUp();
            recoilPosLocal = m_modelController->GetLocalizedTubeRightPos();
        }
        else if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_LEFT)
        {
            posWorld = m_modelController->GetMissileTubePosLeft();
            pos = m_modelController->GetLocalizedTubeLeftPos();
            launchDir = m_modelController->GetMissileTubeTurretLocalLeftDir();
            launchDirWorld = m_modelController->GetMissileTubeDirLeft();
            velocity = m_heli.q.velocity;
            up = m_modelController->GetMissileTubeTurretLocalLeftUp();
            recoilPosLocal = m_modelController->GetLocalizedTubeLeftPos();
        }
        else
        {
            int testBreak = 0; 
            testBreak++;
        }

        aFireFx->emitter->SetPosition(posWorld);
        aFireFx->emitter->SetVelocity(velocity);
        aFireFx->emitter->SetOmnidirectional();
        aFireFx->emitter->SetOrientation(launchDirWorld, up);
        aFireFx->pos = posWorld;
        aFireFx->up = up;
        aFireFx->isDestroyTrue = false;
        aFireFx->forward = launchDirWorld;
        aFireFx->fx->Play(false);

        aRocketFx->emitter->SetPosition(posWorld);
        aRocketFx->emitter->SetVelocity(velocity);
        aRocketFx->emitter->SetOmnidirectional();
        aRocketFx->emitter->SetOrientation(launchDirWorld, up);
        aRocketFx->pos = posWorld;
        aRocketFx->up = up;
        aRocketFx->isDestroyTrue = false;
        aRocketFx->forward = launchDirWorld;
        aRocketFx->fx->Play(true);

        if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_RIGHT)
        {
            m_fireControl->FireSelectedWithAudio(posWorld, launchDirWorld, velocity, up, aRocketFx, false);
        }
        else if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_LEFT)
        {
            m_fireControl->FireSelectedWithAudio(posWorld, launchDirWorld, velocity, up, aRocketFx, true);
        }

        Utility::ImpulseForce recoil = m_fireControl->GetRecoilImpulseForce(-launchDir);

        DirectX::SimpleMath::Vector3 weaponForceNorm = launchDir;
        weaponForceNorm.Normalize();
        recoil.torqueForceNorm = weaponForceNorm;
        recoil.directionNorm = -launchDir;
        if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_RIGHT)
        {
            recoil.torqueArm = m_modelController->GetLocalizedTubeRightPos();
        }
        else if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_LEFT)
        {
            recoil.torqueArm = m_modelController->GetLocalizedTubeLeftPos();
        }

        m_heli.impulseForceVec.push_back(recoil);
    }
}

void Vehicle::TestDrawFireDirection()
{
    DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
    DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDirWorld();
    DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 up = m_modelController->GetWeaponUpWorld();
    //velocity = DirectX::SimpleMath::Vector3::Zero;
    m_debugData->PushDebugLine(pos, launchDir, 10.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->DebugPushUILineDecimalNumber("velocity.x ", velocity.x, "");
    m_debugData->DebugPushUILineDecimalNumber("velocity.y ", velocity.y, "");
    m_debugData->DebugPushUILineDecimalNumber("velocity.z ", velocity.z, "");

    m_fireControl->FireSelectedAmmo(pos, launchDir, velocity, up);
    DirectX::SimpleMath::Vector3 launchDirLocal = m_modelController->GetWeaponDirLocal();
    Utility::ImpulseForce recoil = m_fireControl->GetRecoilImpulseForce(-launchDir);

    //launchDir = DirectX::SimpleMath::Vector3::Transform(launchDir, m_heli.alignmentInverse);

    DirectX::SimpleMath::Vector3 weaponTorqueArmLocal = m_heli.localWeaponPos - m_heli.localCenterOfMass;
    weaponTorqueArmLocal = DirectX::SimpleMath::Vector3::Transform(weaponTorqueArmLocal, m_heli.alignmentInverse);
    DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - m_heli.centerOfMass;
    //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
    DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
    weaponForce.Normalize();
    DirectX::SimpleMath::Vector3 torqueForceNorm = -launchDirLocal;
    weaponForce.Normalize();
    //float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
    //weaponForce *= weaponForceMag;
    //weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
    recoil.torqueArm = weaponTorqueArmLocal;
    recoil.torqueForceNorm = torqueForceNorm;
    //m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
    //m_testImpulseForce = recoil;
}

DirectX::SimpleMath::Vector3 Vehicle::GetAntiGravGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass)
{
    DirectX::SimpleMath::Vector3 gravForce = aGravity;
    const float lowerCurveBound = m_heli.hoverRangeLower;
    const float midCurveBound = m_heli.hoverRangeMid;
    const float upperCurveBound = m_heli.hoverRangeUpper;
    if (aAltitude < lowerCurveBound)
    {
        gravForce *= -1.0f;
    }
    else if (aAltitude > upperCurveBound)
    {
        gravForce = aGravity;
    }
    else
    {
        const float currentCurvePos = (aAltitude / upperCurveBound);
        gravForce = gravForce * currentCurvePos;
    }

    return gravForce * aMass;
}

DirectX::SimpleMath::Vector3 Vehicle::GetAntiMassGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass)
{
    const DirectX::SimpleMath::Vector3 gravForce = aGravity;
    const float lowerCurveBound = m_heli.hoverRangeLower;
    const float midCurveBound = m_heli.hoverRangeMid;
    const float upperCurveBound = m_heli.hoverRangeUpper;
    float mass = aMass;
    if (aAltitude < lowerCurveBound)
    {
        const float currentCurvePos = (aAltitude / upperCurveBound);
        mass = mass * currentCurvePos;
    }
    else if (aAltitude > upperCurveBound)
    {
        mass = aMass;
    }
    else
    {
        const float currentCurvePos = (aAltitude / upperCurveBound);
        mass = mass * currentCurvePos;
    }
    return gravForce * mass;
}

DirectX::SimpleMath::Vector3 Vehicle::GetDamperForce(const float aAltitude, const float aMass)
{
    const float lowerCurveBound = m_heli.hoverRangeLower;
    const float midCurveBound = m_heli.hoverNeutralBoyantAlt;
    const float upperCurveBound = m_heli.hoverRangeUpper;
    const float damperConstant = 9.1f;
    DirectX::SimpleMath::Vector3 currentVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 damperForce = DirectX::SimpleMath::Vector3::Zero;
    if (aAltitude > midCurveBound && aAltitude < upperCurveBound)
    {
        if (currentVelocity.y < 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }
    else if (aAltitude < midCurveBound && aAltitude > lowerCurveBound)
    {
        if (currentVelocity.y > 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }
    return damperForce * aMass;
}

DirectX::SimpleMath::Vector3 Vehicle::GetFollowPos() const
{
    DirectX::SimpleMath::Vector3 followPos = -m_heli.forward;
    followPos.y = 0.0f;
    followPos.Normalize();
    return followPos;
}

DirectX::SimpleMath::Vector3 Vehicle::GetHoverGravForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass)
{
    DirectX::SimpleMath::Vector3 hoverGravForceUpdate;
    const float forcePercentage = aAltitude / aGroundInteractionRange;
    if (aAltitude > aGroundInteractionRange)
    {
        hoverGravForceUpdate = aGravity * aMass;
    }
    else if (aAltitude < 0.0f)
    {
        hoverGravForceUpdate = (aGravity * forcePercentage) * (aMass) * 0.5f;
    }
    else
    {
        hoverGravForceUpdate = (aGravity * forcePercentage) * (aMass);
    }

    return hoverGravForceUpdate;
}

DirectX::SimpleMath::Vector3 Vehicle::GetHoverLift(const DirectX::SimpleMath::Vector3 aLiftForce, const float aAltitude)
{
    DirectX::SimpleMath::Vector3 liftForce = aLiftForce;

    const float lowerCurveBound = m_heli.hoverRangeLower;
    const float midCurveBound = m_heli.hoverRangeMid;
    const float upperCurveBound = m_heli.hoverRangeUpper;

    if (aAltitude < lowerCurveBound)
    {
        liftForce *= 2.0f;
    }
    else if (aAltitude > upperCurveBound)
    {
        liftForce = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        const float currentCurvePos = (aAltitude / upperCurveBound);
        liftForce *= currentCurvePos;
    }

    return liftForce;
}

DirectX::SimpleMath::Vector3 Vehicle::GetJetThrust(const DirectX::SimpleMath::Vector3 aForward, const float aInput, const float aThrustMax)
{
    DirectX::SimpleMath::Vector3 thrustUpdate = aForward * (aInput * aThrustMax);
    return thrustUpdate;
}

DirectX::SimpleMath::Vector4 Vehicle::GetRearGlowColor()
{
    DirectX::SimpleMath::Vector4 updateColor = DirectX::SimpleMath::Vector4::Zero;

    return updateColor;
}

DirectX::SimpleMath::Vector3 Vehicle::GetSlopeForce(const DirectX::SimpleMath::Vector3 aTerrainNorm, const float aAltitude, const float aGroundInteractionRange)
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    if (aAltitude > aGroundInteractionRange || aAltitude < 0.0f)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        const float forcePercentage = 1.0f - (aAltitude / aGroundInteractionRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = aTerrainNorm * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * m_heli.mass;
    }
    slopeForceUpdate.y = 0.0f;
    //m_debugData->PushDebugLine(m_heli.q.position, slopeForceUpdate, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->DebugPushUILineDecimalNumber("slopeForceUpdate.Length() = ", slopeForceUpdate.Length(), "");
    return slopeForceUpdate;
}

float Vehicle::GetThrottleTank()
{
    auto throttle = 0.0f;

   // float GetGlowValCenter() const { return m_playerModel.glowCenterVal; };

    throttle += abs(m_modelController->GetGlowValCenter()) + abs(m_modelController->GetGlowValLeft()) + abs(m_modelController->GetGlowValRight());
    if (throttle > 1.0f)
    {
        throttle = 1.0f;
    }
    return throttle;
    //return m_throttleVolume;
}

DirectX::SimpleMath::Vector3 Vehicle::GetThrusterLiftMagnitude(const DirectX::SimpleMath::Vector3 aLiftForce, const DirectX::SimpleMath::Vector3 aPos)
{
    float altitudeAtPos = aPos.y - m_environment->GetTerrainHeightAtPos(aPos);

    //m_debugData->DebugPushUILineDecimalNumber("aPos.y = ", aPos.y, "");
    DirectX::SimpleMath::Vector3 liftForce = aLiftForce;
    const float offset = 5.0f;
    const float lowerCurveBound = m_heli.hoverRangeLower;
    const float midCurveBound = m_heli.hoverRangeMid;
    const float upperCurveBound = m_heli.hoverRangeUpper + offset;

    if (altitudeAtPos < lowerCurveBound)
    {
        liftForce *= 2.0f;
        //m_debugData->DebugPushUILineDecimalNumber("liftMod < = ", 2.0f, "");
    }
    else if (altitudeAtPos > upperCurveBound)
    {
        //liftForce = DirectX::SimpleMath::Vector3::Zero;
        liftForce = liftForce;
    }
    else
    {
        const float altAdjusted = altitudeAtPos - lowerCurveBound;
        const float upperCurveAdjusted = upperCurveBound - lowerCurveBound;
        //const float currentCurvePos = (altitudeAtPos / upperCurveBound);
        const float currentCurvePos = (altAdjusted / upperCurveAdjusted);
        const float inverseCurvePos = 1.0f - currentCurvePos;
        const float liftMod = 1.0f + inverseCurvePos;
        liftForce *= liftMod;
    }

    return liftForce;
    //return aLiftForce;
}

void Vehicle::InitializeFlightControls(ControlInput& aInput)
{
    aInput.collectiveInput = 0.0f;
    aInput.cyclicInputPitch = 0.0f;
    aInput.cyclicInputPitchIsPressed = false;
    aInput.cyclicInputRoll = 0.0f;
    aInput.cyclicInputRollIsPressed = false;
    aInput.cyclicStick = DirectX::SimpleMath::Vector3::UnitY;
    aInput.jetInput = 0.0f;
    aInput.jetInputIsPressed = false;
    aInput.throttleInput = 0.0f;
    aInput.yawPedalInput = 0.0f;
    aInput.yawPedalIsPressed = false;

    aInput.weaponPitch = 0.0f;
    aInput.turretYaw = 0.0f;
}

void Vehicle::InitializeInertiaTensor(HeliData& aHeliData)
{
    float xExtent = m_inertiaModelX;
    float yExtent = m_inertiaModelY;
    float zExtent = m_inertiaModelZ;
    float mass = m_testMass;
    m_heli.inertiaMatrixTest = DirectX::SimpleMath::Matrix::Identity;
    // cuboid
    m_heli.inertiaMatrixTest._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrixTest._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrixTest._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    m_heli.localTurretInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    xExtent = m_heli.turretTensorDimensions.x;
    yExtent = m_heli.turretTensorDimensions.y;
    zExtent = m_heli.turretTensorDimensions.z;
    mass = m_heli.turretTensorMass;
    m_heli.localTurretInertiaMatrix._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_heli.localTurretInertiaMatrix._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_heli.localTurretInertiaMatrix._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    m_heli.localTurretInverseInertiaMatrix = m_heli.localTurretInertiaMatrix;
    m_heli.localTurretInverseInertiaMatrix = m_heli.localTurretInverseInertiaMatrix.Invert();

    //sphere
    /*
    float radius = 4.0f;
    m_heli.inertiaMatrixTest._11 = (2.0f / 3.0f) * (mass) * (radius * radius);
    m_heli.inertiaMatrixTest._22 = (2.0f / 3.0f) * (mass) * (radius * radius);
    m_heli.inertiaMatrixTest._33 = (2.0f / 3.0f) * (mass) * (radius * radius);
    */
    /*
    m_heli.inertiaMatrixTest._11 = m_testMOI;
    m_heli.inertiaMatrixTest._22 = m_testMOI;
    m_heli.inertiaMatrixTest._33 = m_testMOI;
    */
    DirectX::SimpleMath::Matrix ballastInteria = DirectX::SimpleMath::Matrix::Identity;
    float ballastRadius = 1.0f;
    float ballastMass = m_heli.ballastMass;
    ballastInteria._11 = (2.0f / 5.0f) * (mass) * (ballastRadius * ballastRadius);
    ballastInteria._22 = (2.0f / 5.0f) * (mass) * (ballastRadius * ballastRadius);
    ballastInteria._33 = (2.0f / 5.0f) * (mass) * (ballastRadius * ballastRadius);
    DirectX::SimpleMath::Vector3 ballastTranslation = m_heli.ballastInertiaTranslation;
    //ballastInteria *= DirectX::SimpleMath::Matrix::CreateTranslation(ballastTranslation);

    m_heli.localBallastInertiaMatrix = ballastInteria;
    m_heli.localInverseBallastInertiaMatrix = m_heli.localBallastInertiaMatrix;
    m_heli.localInverseBallastInertiaMatrix = m_heli.localInverseBallastInertiaMatrix.Invert();
    m_heli.ballastInertiaTranslation = ballastTranslation;
    //m_heli.inertiaMatrixTest += ballastInteria;
    m_heli.ballastInertiaMatrix = m_heli.localBallastInertiaMatrix;
    m_heli.ballastInertiaMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(ballastTranslation);
    m_heli.inverseBallastInertiaMatrix = m_heli.ballastInertiaMatrix;
    m_heli.inverseBallastInertiaMatrix = m_heli.inverseBallastInertiaMatrix.Invert();

    //m_testInertiaTensorLocal = m_heli.inertiaMatrixTest;
    //m_testInverseInertiaTensorLocal = m_testInertiaTensorLocal;
    //m_testInverseInertiaTensorLocal = m_testInverseInertiaTensorLocal.Invert();

    m_heli.inverseInertiaMatrixTest = m_heli.inertiaMatrixTest;
    m_heli.inverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest.Invert();

    m_heli.localInertiaMatrixTest = m_heli.inertiaMatrixTest;
    m_heli.localInverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest;


    m_heli.inertiaTensor = m_heli.inertiaMatrixTest;
    m_heli.inertiaTensorInverse = m_heli.inverseInertiaMatrixTest;

}

void Vehicle::InitializeRotorBlades(HeliData& aHeliData)
{
    // set values for main rotor blade
    const int bladeCountMainRotor = 2;
    const float angleBetweetnBladesMainRotor = (Utility::GetPi() * 2.0f) / bladeCountMainRotor;
    const float radiusMainRotor = 4.7f;
    const float heightMainRotor = 0.15f;
    const float lengthMainRotor = 4.6f;
    const float widthMainRotor = 0.80f;
    const float initialRPMMainRotor = 0.0f;
    const float maxPitchAngleMainRotor = Utility::ToRadians(20.0f);
    const float maxInputPitchAngleMainRotor = Utility::ToRadians(15.0f);
    const float minInputPitchAngleMainRotor = Utility::ToRadians(0.0f);
    const float minPitchAngleMainRotor = Utility::ToRadians(0.0f);
    const float neutralAngleMainRotor = Utility::ToRadians(0.0f);
    const float initialPitchAngleMainRotor = neutralAngleMainRotor;
    const float rotorRotationMainRotor = Utility::ToRadians(180.0f);

    aHeliData.mainRotor.angleBetweenBlades = angleBetweetnBladesMainRotor;
    aHeliData.mainRotor.bladeCount = bladeCountMainRotor;
    aHeliData.mainRotor.height = heightMainRotor;
    aHeliData.mainRotor.inputPitchAngleMax = maxInputPitchAngleMainRotor;
    aHeliData.mainRotor.inputPitchAngleMin = minInputPitchAngleMainRotor;
    aHeliData.mainRotor.length = lengthMainRotor;
    aHeliData.mainRotor.neutralAngle = neutralAngleMainRotor;
    aHeliData.mainRotor.pitchAngleMax = maxPitchAngleMainRotor;
    aHeliData.mainRotor.pitchAngleMin = minPitchAngleMainRotor;
    aHeliData.mainRotor.radius = radiusMainRotor;
    aHeliData.mainRotor.rpm = initialRPMMainRotor;
    aHeliData.mainRotor.rotorRotation = rotorRotationMainRotor;
    aHeliData.mainRotor.width = widthMainRotor;
    aHeliData.mainRotor.bladeType = Rotor::BladeType::BLADETYPE_MAINROTOR;

    aHeliData.mainRotor.bladeVec.clear();
    aHeliData.mainRotor.bladeVec.resize(bladeCountMainRotor);
    for (unsigned int i = 0; i < aHeliData.mainRotor.bladeVec.size(); ++i)
    {
        aHeliData.mainRotor.bladeVec[i].pitchAngle = initialPitchAngleMainRotor;
        aHeliData.mainRotor.bladeVec[i].cyclicAngle = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftForcePerSecond = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftMag = 0.0f;
        aHeliData.mainRotor.bladeVec[i].liftNorm = DirectX::SimpleMath::Vector3::Zero;
        aHeliData.mainRotor.bladeVec[i].rotationAngle = static_cast<float>(i) * angleBetweetnBladesMainRotor;
    }

    // set values for tail rotor blade
    const int bladeCountTailRotor = 2;
    const float angleBetweetnBladesTailRotor = (Utility::GetPi() * 2.0f) / bladeCountTailRotor;
    const float radiusTailRotor = 1.425f;
    const float heightTailRotor = 0.1f;
    const float lengthTailRotor = 1.25f;
    const float widthTailRotor = 0.300000012f;

    const float initialRPMTailRotor = 0.0f;
    const float maxPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float maxInputPitchAngleTailRotor = Utility::ToRadians(45.0f);
    const float minInputPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float minPitchAngleTailRotor = Utility::ToRadians(0.0f);
    const float neutralAngleTailRotor = (maxPitchAngleTailRotor + minInputPitchAngleTailRotor) / 2.0f;
    const float initialPitchAngleTailRotor = neutralAngleTailRotor;
    const float rotorRotationTailBlade = 0.0f;

    aHeliData.tailRotor.angleBetweenBlades = angleBetweetnBladesTailRotor;
    aHeliData.tailRotor.bladeCount = bladeCountTailRotor;
    aHeliData.tailRotor.height = heightTailRotor;
    aHeliData.tailRotor.inputPitchAngleMax = maxInputPitchAngleTailRotor;
    aHeliData.tailRotor.inputPitchAngleMin = minInputPitchAngleTailRotor;
    aHeliData.tailRotor.length = lengthTailRotor;
    aHeliData.tailRotor.neutralAngle = neutralAngleTailRotor;
    aHeliData.tailRotor.pitchAngleMax = maxPitchAngleTailRotor;
    aHeliData.tailRotor.pitchAngleMin = minPitchAngleTailRotor;
    aHeliData.tailRotor.radius = radiusTailRotor;
    aHeliData.tailRotor.rpm = initialRPMTailRotor;
    aHeliData.tailRotor.rotorRotation = rotorRotationTailBlade;
    aHeliData.tailRotor.width = widthTailRotor;
    aHeliData.tailRotor.bladeType = Rotor::BladeType::BLADETYPE_TAILROTOR;

    aHeliData.tailRotor.bladeVec.clear();
    aHeliData.tailRotor.bladeVec.resize(bladeCountMainRotor);
    for (unsigned int i = 0; i < aHeliData.tailRotor.bladeVec.size(); ++i)
    {
        aHeliData.tailRotor.bladeVec[i].pitchAngle = initialPitchAngleTailRotor;
        aHeliData.tailRotor.bladeVec[i].cyclicAngle = 0.0f;
        aHeliData.tailRotor.bladeVec[i].liftForcePerSecond = 0.0f;
        aHeliData.tailRotor.bladeVec[i].liftMag = 0.0f;
        aHeliData.tailRotor.bladeVec[i].liftNorm = DirectX::SimpleMath::Vector3::Zero;
        aHeliData.tailRotor.bladeVec[i].rotationAngle = static_cast<float>(i) * angleBetweetnBladesTailRotor;
    }
}

void Vehicle::InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNPCController, std::shared_ptr<Vehicle> aVehicle)
{
    InitializeFlightControls(m_heli.controlInput);

    m_heli.impulseForceVec.clear();

    // helicopter data
    m_heli.mainRotorRPM = 0.0f;
    m_heli.mainRotorMaxRPM = 1000.0f;
    m_heli.numEqns = 6;

    m_heli.area = 14.67;
    m_heli.dimensions = DirectX::SimpleMath::Vector3(8.0f, 4.0f, 4.0f);
    m_heli.dimensions = DirectX::SimpleMath::Vector3(4.7f, 2.0f, 2.0f);
    m_heli.airDensity = m_environment->GetAirDensity();
    m_heli.dragCoefficient = 2.5f;

    m_heli.airResistance = 0.0f;
    m_heli.totalResistance = m_heli.airResistance;
    m_heli.gravity = m_environment->GetGravityVec();
    m_heli.time = 0.0;

    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_heli.forward, m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::Identity;
    m_heli.alignmentInverse = m_heli.alignment;
    m_heli.alignmentInverse = m_heli.alignmentInverse.Invert();

    m_heli.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    m_heli.alignmentQuat.Normalize();
    m_heli.alignmentInverseQuat = m_heli.alignmentQuat;
    m_heli.alignmentInverseQuat.Inverse(m_heli.alignmentInverseQuat);
    m_heli.alignmentInverseQuat.Normalize();

    m_heli.cameraOrientation = m_heli.alignment;
    m_heli.cameraOrientationPrevious = m_heli.cameraOrientation;

    m_heli.q.position = m_startPos;
    m_heli.boundingBox.Center = m_heli.q.position;
    m_heli.boundingBox.Extents = m_heli.dimensions;
    m_heli.boundingBox.Orientation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);

    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    m_heli.speed = 0.0;

    m_heli.isVehicleAirborne = false;
    m_heli.isVehicleLanding = false;
    m_heli.isVelocityBackwards = false;

    m_heli.terrainHightAtPos = 0.0;
    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.groundPlane.x = 0.0f;
    m_heli.groundPlane.y = -1.0f;
    m_heli.groundPlane.z = 0.0f;
    m_heli.groundPlane.w = 0.5f;

    m_heli.localWeaponPos = DirectX::SimpleMath::Vector3(0.0f, 3.0f, 0.0f);
    m_heli.weaponPos = m_heli.localWeaponPos;
    m_heli.localWeaponDirection = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    m_heli.weaponDirection = m_heli.localWeaponDirection;
    DirectX::SimpleMath::Vector3 muzzleTrans = DirectX::SimpleMath::Vector3(0.3, 1.32, 0.0);
    m_heli.weaponPos = DirectX::SimpleMath::Vector3::Zero;
    m_heli.weaponPos = DirectX::SimpleMath::Vector3::Transform(m_heli.weaponPos, DirectX::SimpleMath::Matrix::CreateTranslation(muzzleTrans));
    m_heli.localWeaponPos = m_heli.weaponPos;
    m_heli.muzzlePos = m_heli.weaponPos;
    m_heli.localMuzzlePos = m_heli.muzzlePos;
    m_heli.localizedMuzzlePos = m_heli.localMuzzlePos;

    // set up rotor blades
    InitializeRotorBlades(m_heli);

    // Intialize key physics hardpoints based on model 
    m_heli.localCenterOfMass = DirectX::SimpleMath::Vector3::Zero;
    //m_heli.localCenterOfMass.y -= 0.5f;
    m_heli.centerOfMass = m_heli.localCenterOfMass;

    m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3(0.0f, -2.174999997f, 0.00000000f);

    m_heli.mainRotorPos = m_heli.localMainRotorPos;

    m_heli.localGravityTorqueArmPos = m_heli.localCenterOfMass;
    m_heli.localGravityTorqueArmPos.y += 1.0f;
    m_heli.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;

    m_heli.localBuoyancyTorqueArmPos = m_heli.localCenterOfMass;
    m_heli.localBuoyancyTorqueArmPos.y -= 1.0f;
    m_heli.buoyancyTorqueArmPos = m_heli.localBuoyancyTorqueArmPos;

    m_heli.localTopPos = m_heli.localCenterOfMass;
    m_heli.localTopPos.y -= 0.2f;
    m_heli.topPos = m_heli.localTopPos;

    m_heli.localTopTestPos = m_heli.localCenterOfMass;
    m_heli.localTopTestPos.y += 1.0f;
    m_heli.topTestPos = m_heli.localTopTestPos;

    m_heli.localBaseTestPos = m_heli.localCenterOfMass;
    m_heli.localBaseTestPos.y -= 1.0f;
    m_heli.baseTestPos = m_heli.localBaseTestPos;

    m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3(-9.25000000, 0.00000000, 0.00000000);
    m_heli.tailRotorPos = m_heli.localTailRotorPos;

    m_heli.localPhysicsPointFront = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointFront.x += 2.2f;
    m_heli.localPhysicsPointFront.x += 1.0;
    m_heli.physicsPointFront = m_heli.localPhysicsPointFront;

    m_heli.localPhysicsPointLeft = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointLeft.z -= 1.5f;
    m_heli.localPhysicsPointLeft.z -= 1.0f;
    m_heli.physicsPointLeft = m_heli.localPhysicsPointLeft;

    m_heli.localPhysicsPointRear = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointRear.x -= 2.2f;
    m_heli.localPhysicsPointRear.x -= 1.0f;
    m_heli.physicsPointRear = m_heli.localPhysicsPointRear;

    m_heli.localPhysicsPointRight = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointRight.z += 1.5f;
    m_heli.localPhysicsPointRight.z += 1.0f;
    m_heli.physicsPointRight = m_heli.localPhysicsPointRight;

    InitializeInertiaTensor(m_heli);

    m_heli.localLandingGearPos = DirectX::SimpleMath::Vector3::Zero;
    m_heli.localLandingGearPos.y -= 1.0f;
    m_heli.landingGearPos = m_heli.localLandingGearPos;

    m_testTerrainNormTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    m_testTerrainNormTorque.magnitude = 0.0f;

    m_fireControl = std::make_shared<FireControl>();
    m_fireControl->InitializeFireControl(aContext, m_heli.localWeaponPos, m_heli.localWeaponDirection, m_environment, aVehicle);
    m_fireControl->SetNPCController(aNPCController);
    aNPCController->SetFireControl(m_fireControl);

    m_testDrivetrainTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    m_testDrivetrainTorque.magnitude = 0.0f;

    m_testDrivetrainTorqueLocal.axis = DirectX::SimpleMath::Vector3::UnitY;
    m_testDrivetrainTorqueLocal.magnitude = 0.0f;

    m_testGravForce.axis = DirectX::SimpleMath::Vector3::UnitY;
    m_testGravForce.magnitude = 0.0f;

    m_testImpulseForce.isActive = false;
    m_heli.testCollisionImpulseForce.isActive = false;
}

void Vehicle::InputCollective(const float aCollectiveInput)
{
    const float updatedCollective = (aCollectiveInput * m_heli.controlInput.collectiveInputRate) + m_heli.controlInput.collectiveInput;
    if (updatedCollective > m_heli.controlInput.collectiveInputMax)
    {
        m_heli.controlInput.collectiveInput = m_heli.controlInput.collectiveInputMax;
    }
    else if (updatedCollective < m_heli.controlInput.collectiveInputMin)
    {
        m_heli.controlInput.collectiveInput = m_heli.controlInput.collectiveInputMin;
    }
    else
    {
        m_heli.controlInput.collectiveInput = updatedCollective;
    }
}

void Vehicle::InputCyclicPitch(const float aPitchInput)
{
    m_heli.controlInput.cyclicInputPitchIsPressed = true;
    const float updatedPitch = (aPitchInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputPitch;
    if (updatedPitch > m_heli.controlInput.cyclicInputMax)
    {
        m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedPitch < m_heli.controlInput.cyclicInputMin)
    {
        m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        m_heli.controlInput.cyclicInputPitch = updatedPitch;
    }
}

void Vehicle::InputCyclicPitchNew(const float aPitchInput)
{
    m_heli.controlInput.cyclicInputPitchIsPressed = true;
    float updateVal = 0.0f;
    const float updatedPitch = (aPitchInput * m_heli.controlInput.cyclicInputRateRaw) + m_heli.controlInput.cyclicInputPitchRaw;
    if (updatedPitch > m_heli.controlInput.cyclicInputRawMax)
    {
        updateVal = m_heli.controlInput.cyclicInputRawMax;
        //m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedPitch < m_heli.controlInput.cyclicInputRawMin)
    {
        updateVal = m_heli.controlInput.cyclicInputRawMin;
    }
    else
    {
        updateVal = updatedPitch;
    }
    m_heli.controlInput.cyclicInputPitchRaw = updateVal;
}

void Vehicle::InputCyclicRoll(const float aRollInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    float updateVal = 0.0f;
    float updatedRoll = (aRollInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    if (updatedRoll > m_heli.controlInput.cyclicInputMax)
    {
        updateVal = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputMin)
    {
        updateVal = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        updateVal = updatedRoll;
    }
    m_heli.controlInput.cyclicInputRoll = updateVal;
}

void Vehicle::InputCyclicRollNew(const float aRollInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    float updateVal = 0.0f;
    //float updatedRoll = (aRollInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    float updatedRoll = (aRollInput * m_heli.controlInput.cyclicInputRateRaw) + m_heli.controlInput.cyclicInputRollRaw;
    if (updatedRoll > m_heli.controlInput.cyclicInputRawMax)
    {
        updateVal = m_heli.controlInput.cyclicInputRawMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputRawMin)
    {
        updateVal = m_heli.controlInput.cyclicInputRawMin;
    }
    else
    {
        updateVal = updatedRoll;
    }
    m_heli.controlInput.cyclicInputRollRaw = updateVal;
}

void Vehicle::InputCyclicRoll2(const float aRollInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    const float updatedRoll = (aRollInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    if (updatedRoll > m_heli.controlInput.cyclicInputMax)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputMin)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        m_heli.controlInput.cyclicInputRoll = updatedRoll;
    }
}

void Vehicle::InputCyclicRoll3(const float aRollInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;

    float rollInputMod = 1.0f;
    if (m_heli.controlInput.cyclicInputRoll > 0.0f && aRollInput < 0.0f)
    {
        //m_heli.controlInput.cyclicInputRoll = 0.0f;
        float testMod = abs(m_heli.controlInput.cyclicInputRoll / m_heli.controlInput.cyclicInputMax);
        rollInputMod += testMod * 2.0f;
    }
    else if (m_heli.controlInput.cyclicInputRoll < 0.0f && aRollInput > 0.0f)
    {
        //m_heli.controlInput.cyclicInputRoll = 0.0f;
        float testMod = abs(m_heli.controlInput.cyclicInputRoll / m_heli.controlInput.cyclicInputMax);
        rollInputMod += testMod * 2.0f;
    }
    m_debugData->DebugPushUILineDecimalNumber("rollInputMod = ", rollInputMod, "");
    float updatedRoll = ((aRollInput * rollInputMod) * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    if (updatedRoll > m_heli.controlInput.cyclicInputMax)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputMin)
    {
        m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        m_heli.controlInput.cyclicInputRoll = updatedRoll;
    }
}

void Vehicle::InputGamePadForward(const float aForwardInput)
{
    m_heli.controlInput.cyclicInputPitchIsPressed = true;
    const float updatedForward = aForwardInput * m_heli.controlInput.cyclicInputMax;

    m_heli.controlInput.cyclicInputPitch = aForwardInput;
    m_heli.controlInput.cyclicInputPitchRaw = aForwardInput;
}

void Vehicle::InputGamePadStrafe(const float aStrafeInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    //const float updatedRoll = aStrafeInput * m_heli.controlInput.cyclicInputMax;
    //m_heli.controlInput.cyclicInputRoll = updatedRoll;
    float updateVal = 0.0f;
    const float updatedRoll = (aStrafeInput * m_heli.controlInput.cyclicInputRate) + m_heli.controlInput.cyclicInputRoll;
    if (updatedRoll > m_heli.controlInput.cyclicInputMax)
    {
        updateVal = m_heli.controlInput.cyclicInputMax;
    }
    else if (updatedRoll < m_heli.controlInput.cyclicInputMin)
    {
        updateVal = m_heli.controlInput.cyclicInputMin;
    }
    else
    {
        updateVal = updatedRoll;
    }
    updateVal = aStrafeInput;
    m_heli.controlInput.cyclicInputRoll = updateVal;
    m_heli.controlInput.cyclicInputRollRaw = updateVal;
}

void Vehicle::InputGamePadStrafe2(const float aStrafeInput)
{
    m_heli.controlInput.cyclicInputRollIsPressed = true;
    const float updatedRoll = aStrafeInput * m_heli.controlInput.cyclicInputMax;
    m_heli.controlInput.cyclicInputRoll = updatedRoll;
}

void Vehicle::InputGamePadTurn(const float aTurnInput)
{
    m_heli.controlInput.yawPedalIsPressed = true;
    const float updatedYaw = aTurnInput * m_heli.controlInput.yawPedalInputMax;

    m_heli.controlInput.yawPedalInput = updatedYaw;
}

void Vehicle::InputDecay(const double aTimeDelta)
{
    const float timeDelta = static_cast<float>(aTimeDelta);

    if (m_heli.controlInput.cyclicInputPitchIsPressed == false)
    {
        // Cyclic Pitch Decay
        if (m_heli.controlInput.cyclicInputPitch > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitch - (m_heli.controlInput.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitch -= m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputPitch < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitch + (m_heli.controlInput.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputPitch = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitch += m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputPitch = 0.0f;
        }
    }
    // Cyclic Roll Decay
    if (m_heli.controlInput.cyclicInputRollIsPressed == false)
    {
        if (m_heli.controlInput.cyclicInputRoll > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRoll - (m_heli.controlInput.cyclicDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRoll -= m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputRoll < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRoll + (m_heli.controlInput.cyclicDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputRoll = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRoll += m_heli.controlInput.cyclicDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputRoll = 0.0f;
        }
    }
    // Jet Decay
    if (m_heli.controlInput.jetInputIsPressed == false)
    {
        if (m_heli.controlInput.jetInput > 0.0f)
        {
            if (m_heli.controlInput.jetInput - (m_heli.controlInput.jetInputDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.jetInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.jetInput -= m_heli.controlInput.jetInputDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.jetInput < 0.0f)
        {
            if (m_heli.controlInput.jetInput + (m_heli.controlInput.jetInputDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.jetInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.jetInput += m_heli.controlInput.jetInputDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.jetInput = 0.0f;
        }
    }
    // Yaw Pedal Decay
    if (m_heli.controlInput.yawPedalIsPressed == false)
    {
        if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > -m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput += m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput -= m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.yawPedalInput = 0.0f;
        }
    }

    m_heli.controlInput.cyclicInputPitchIsPressed = false;
    m_heli.controlInput.cyclicInputRollIsPressed = false;
    m_heli.controlInput.jetInputIsPressed = false;
    m_heli.controlInput.yawPedalIsPressed = false;
}

void Vehicle::InputDecayNew(const double aTimeDelta)
{
    const float timeDelta = static_cast<float>(aTimeDelta);

    if (m_heli.controlInput.cyclicInputPitchIsPressed == false)
    {
        // Cyclic Pitch Decay
        if (m_heli.controlInput.cyclicInputPitchRaw > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitchRaw - (m_heli.controlInput.cyclicDecayRateRaw * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputPitchRaw = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitchRaw -= m_heli.controlInput.cyclicDecayRateRaw * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputPitchRaw < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputPitchRaw + (m_heli.controlInput.cyclicDecayRateRaw * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputPitchRaw = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputPitchRaw += m_heli.controlInput.cyclicDecayRateRaw * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputPitchRaw = 0.0f;
        }
    }
    // Cyclic Roll Decay
    if (m_heli.controlInput.cyclicInputRollIsPressed == false)
    {
        if (m_heli.controlInput.cyclicInputRollRaw > 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRollRaw - (m_heli.controlInput.cyclicDecayRateRaw * timeDelta) < 0.0f)
            {
                m_heli.controlInput.cyclicInputRollRaw = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRollRaw -= m_heli.controlInput.cyclicDecayRateRaw * timeDelta;
            }
        }
        else if (m_heli.controlInput.cyclicInputRollRaw < 0.0f)
        {
            if (m_heli.controlInput.cyclicInputRollRaw + (m_heli.controlInput.cyclicDecayRateRaw * timeDelta) > 0.0f)
            {
                m_heli.controlInput.cyclicInputRollRaw = 0.0f;
            }
            else
            {
                m_heli.controlInput.cyclicInputRollRaw += m_heli.controlInput.cyclicDecayRateRaw * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.cyclicInputRollRaw = 0.0f;
        }
    }

    /*
    // Jet Decay
    if (m_heli.controlInput.jetInputIsPressed == false)
    {
        if (m_heli.controlInput.jetInput > 0.0f)
        {
            if (m_heli.controlInput.jetInput - (m_heli.controlInput.jetInputDecayRate * timeDelta) < 0.0f)
            {
                m_heli.controlInput.jetInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.jetInput -= m_heli.controlInput.jetInputDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.jetInput < 0.0f)
        {
            if (m_heli.controlInput.jetInput + (m_heli.controlInput.jetInputDecayRate * timeDelta) > 0.0f)
            {
                m_heli.controlInput.jetInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.jetInput += m_heli.controlInput.jetInputDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.jetInput = 0.0f;
        }
    }
    // Yaw Pedal Decay
    if (m_heli.controlInput.yawPedalIsPressed == false)
    {
        if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > -m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput += m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else if (m_heli.controlInput.yawPedalInput - (m_heli.controlInput.yawPedalDecayRate * timeDelta) > 0.0f)
        {
            if (m_heli.controlInput.yawPedalInput + (m_heli.controlInput.yawPedalDecayRate * timeDelta) < m_heli.controlInput.inputDeadZone)
            {
                m_heli.controlInput.yawPedalInput = 0.0f;
            }
            else
            {
                m_heli.controlInput.yawPedalInput -= m_heli.controlInput.yawPedalDecayRate * timeDelta;
            }
        }
        else
        {
            m_heli.controlInput.yawPedalInput = 0.0f;
        }
    }

    m_heli.controlInput.cyclicInputPitchIsPressed = false;
    m_heli.controlInput.cyclicInputRollIsPressed = false;
    m_heli.controlInput.jetInputIsPressed = false;
    m_heli.controlInput.yawPedalIsPressed = false;
    */
}

void Vehicle::InputJet(const float aJetInput)
{
    m_heli.controlInput.jetInputIsPressed = true;
    const float updatedJet = (aJetInput * m_heli.controlInput.jetInputRate) + m_heli.controlInput.jetInput;
    if (updatedJet > m_heli.controlInput.jetInputMax)
    {
        m_heli.controlInput.jetInput = m_heli.controlInput.jetInputMax;
    }
    else if (updatedJet < m_heli.controlInput.jetInputMin)
    {
        m_heli.controlInput.jetInput = m_heli.controlInput.jetInputMin;
    }
    else
    {
        m_heli.controlInput.jetInput = updatedJet;
    }
}

void Vehicle::InputTurretYaw(const float aTurretYawInput)
{
    const float updatedTurretYaw = (aTurretYawInput * m_heli.controlInput.turretYawInputRate) + m_heli.controlInput.turretYaw;
    if (updatedTurretYaw > m_heli.controlInput.turretYawMax)
    {
        m_heli.controlInput.turretYaw = m_heli.controlInput.turretYawMax;
    }
    else if (updatedTurretYaw < m_heli.controlInput.turretYawMin)
    {
        m_heli.controlInput.turretYaw = m_heli.controlInput.turretYawMin;
    }
    else if (updatedTurretYaw < m_heli.controlInput.inputDeadZone && updatedTurretYaw > -m_heli.controlInput.inputDeadZone)
    {
        m_heli.controlInput.turretYaw = 0.0f;
    }
    else
    {
        m_heli.controlInput.turretYaw = updatedTurretYaw;
    }
}

void Vehicle::InputThrottle(const float aThrottleInput)
{
    const float updatedThrottle = (aThrottleInput * m_heli.controlInput.throttleInputRate) + m_heli.controlInput.throttleInput;
    if (updatedThrottle > m_heli.controlInput.throttleInputMax)
    {
        m_heli.controlInput.throttleInput = m_heli.controlInput.throttleInputMax;
    }
    else if (updatedThrottle < m_heli.controlInput.throttleInputMin)
    {
        m_heli.controlInput.throttleInput = m_heli.controlInput.throttleInputMin;
    }
    else
    {
        m_heli.controlInput.throttleInput = updatedThrottle;
    }
}

void Vehicle::InputYawPedal(const float aYawInput)
{
    m_heli.controlInput.yawPedalIsPressed = true;
    const float updatedYaw = (aYawInput * m_heli.controlInput.yawPedalInputRate) + m_heli.controlInput.yawPedalInput;
    if (updatedYaw > m_heli.controlInput.yawPedalInputMax)
    {
        m_heli.controlInput.yawPedalInput = m_heli.controlInput.yawPedalInputMax;
    }
    else if (updatedYaw < m_heli.controlInput.yawPedalInputMin)
    {
        m_heli.controlInput.yawPedalInput = m_heli.controlInput.yawPedalInputMin;
    }
    else if (updatedYaw < m_heli.controlInput.inputDeadZone && updatedYaw > -m_heli.controlInput.inputDeadZone)
    {
        m_heli.controlInput.yawPedalInput = 0.0f;
    }
    else
    {
        m_heli.controlInput.yawPedalInput = updatedYaw;
    }
}

void Vehicle::InputWeaponPitch(const float aPitchInput)
{
    const float updatedPitch = (aPitchInput * m_heli.controlInput.weaponPitchInputRate) + m_heli.controlInput.weaponPitch;
    if (updatedPitch > m_heli.controlInput.weaponPitchMax)
    {
        m_heli.controlInput.weaponPitch = m_heli.controlInput.weaponPitchMax;
    }
    else if (updatedPitch < m_heli.controlInput.weaponPitchMin)
    {
        m_heli.controlInput.weaponPitch = m_heli.controlInput.weaponPitchMin;
    }
    else if (updatedPitch < m_heli.controlInput.inputDeadZone && updatedPitch > -m_heli.controlInput.inputDeadZone)
    {
        m_heli.controlInput.weaponPitch = 0.0f;
    }
    else
    {
        m_heli.controlInput.weaponPitch = updatedPitch;
    }
}

void Vehicle::InputWeaponPitchToZero(const float aTimeStep)
{
    if (m_heli.controlInput.weaponPitch > 0.0f)
    {
        float updatePitch = m_heli.controlInput.weaponPitch - (m_heli.controlInput.weaponPitchInputRate * aTimeStep);
        if (updatePitch < 0.0f)
        {
            updatePitch = 0.0f;
        }
        m_heli.controlInput.weaponPitch = updatePitch;
    }
    else if (m_heli.controlInput.weaponPitch < 0.0f)
    {
        float updatePitch = m_heli.controlInput.weaponPitch + (m_heli.controlInput.weaponPitchInputRate * aTimeStep);
        if (updatePitch > 0.0f)
        {
            updatePitch = 0.0f;
        }
        m_heli.controlInput.weaponPitch = updatePitch;
    }
}

void Vehicle::Jump()
{
    DirectX::SimpleMath::Vector3 jumpVec(50.0f, 0.0f, 0.0f);
    jumpVec = DirectX::SimpleMath::Vector3::Transform(jumpVec, DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f)));
    const float jumpHeight = 20.0f;
    m_heli.q.position.y += jumpHeight;
}

void Vehicle::LandVehicle()
{
    // for now just zero out the velocity and allign vehicle with terrain so the vehicle always lands 'safe'
    m_heli.q.velocity.y *= 0.1f;
    //m_heli.up = m_heli.terrainNormal;
    m_heli.isVehicleAirborne = false;
}

void Vehicle::PassFireControl(std::shared_ptr<FireControl>& aFireControl)
{
    aFireControl = m_fireControl;
}

void Vehicle::ResetVehicle()
{
    m_heli.q.position = DirectX::SimpleMath::Vector3::Zero;
    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = DirectX::SimpleMath::Vector3::UnitZ;
    m_heli.speed = 0.0;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
}

//  This method loads the right-hand sides for the vehicle ODEs
void Vehicle::RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    Motion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;

    DirectX::SimpleMath::Vector3 velocityUpdate = m_heli.vehicleLinearForcesSum;
    velocityUpdate += newQ.velocity;
    DirectX::SimpleMath::Vector3 airResistance = CalculateDragLinear(newQ.velocity);
    //DirectX::SimpleMath::Vector3 airResistance = CalculateDragLinear(aHeli->q.velocity);

    velocityUpdate += airResistance;

    //  Assign right-hand side values.
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
    DirectX::SimpleMath::Vector3 momentumUpdate = newQ.angularMomentum;
    momentumUpdate *= m_angularDampConst;

    //DirectX::SimpleMath::Vector3 testInput = m_testTorqueLocal;
    DirectX::SimpleMath::Vector3 testInput = aHeli->vehicleAngularForcesSum;
    DirectX::SimpleMath::Vector3 torqueForce = testInput;
    torqueForce += (m_heli.angularDrag);
    testInput += momentumUpdate;

    //Airplane.vAngularVelocity += Airplane.mInertiaInverse * (Airplane.vMoments - (Airplane.vAngularVelocity ^ (Airplane.mInertia * Airplane.vAngularVelocity))) * dt;
    DirectX::SimpleMath::Vector3 zero = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 one = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 AirplanevAngularVelocity = newQ.angularVelocity;
    DirectX::SimpleMath::Vector3 testAngVel = DirectX::SimpleMath::Vector3::Zero;
    one = AirplanevAngularVelocity;
    one = DirectX::SimpleMath::Vector3::Transform(one, aHeli->inertiaMatrixTest);
    zero = torqueForce - (AirplanevAngularVelocity.Cross(one));
    testAngVel += DirectX::SimpleMath::Vector3::Transform(zero, aHeli->inverseInertiaMatrixTest);
    DirectX::SimpleMath::Vector3 momentsInput = momentumUpdate;
    DirectX::SimpleMath::Vector3 vAngularVelocity = testInput;
    vAngularVelocity = torqueForce;

    DirectX::SimpleMath::Vector3 toCrossVelocity = vAngularVelocity;
    toCrossVelocity = DirectX::SimpleMath::Vector3::Transform(toCrossVelocity, aHeli->inertiaMatrixTest);

    DirectX::SimpleMath::Vector3 toInverseInertiaVelocity = vAngularVelocity;
    DirectX::SimpleMath::Vector3 testCross = vAngularVelocity;
    testCross = testCross.Cross(toCrossVelocity);

    toInverseInertiaVelocity = (momentsInput - (vAngularVelocity.Cross(toCrossVelocity)));

    toInverseInertiaVelocity = DirectX::SimpleMath::Vector3::Transform(toInverseInertiaVelocity, aHeli->inverseInertiaMatrixTest);
    vAngularVelocity = toInverseInertiaVelocity;

    DirectX::SimpleMath::Vector3 updatedAngularVelocity = torqueForce;
    updatedAngularVelocity = testAngVel;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   
    momentumUpdate = updatedAngularVelocity;
    momentumUpdate = DirectX::SimpleMath::Vector3::Transform(momentumUpdate, aHeli->inertiaMatrixTest);
    DirectX::SimpleMath::Vector3 momentumUpdate2 = updatedAngularVelocity;
    momentumUpdate2 = DirectX::SimpleMath::Vector3::Transform(momentumUpdate2, aHeli->inverseInertiaMatrixTest);
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * updatedAngularVelocity;
    //aDQ->angularMomentum = static_cast<float>(aTimeDelta) * torqueForce;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * momentumUpdate;
    //aDQ->angularMomentum = static_cast<float>(aTimeDelta) * (DirectX::SimpleMath::Vector3::UnitX * 100.0f);
    */
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //DirectX::SimpleMath::Vector3 torqueAccum = m_testTorqueLocal;
    DirectX::SimpleMath::Vector3 torqueAccum = aHeli->vehicleAngularForcesSum;
    //torqueAccum = DirectX::SimpleMath::Vector3::UnitX * m_testTorque;
    torqueAccum += newQ.angularVelocity;
    //torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, aHeli->inverseInertiaMatrixTest);
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, aHeli->inertiaTensorInverse);

    //torqueAccum += newQ.angularVelocity;
    torqueAccum += (m_heli.angularDrag);
    //torqueAccum *= 0.8f;

    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
    DirectX::SimpleMath::Vector3 torqueForceorld = torqueForce;
    torqueForceorld = DirectX::SimpleMath::Vector3::Transform(torqueForceorld, m_heli.alignment);
    DirectX::SimpleMath::Vector3 momentumUpdateWorld = momentumUpdate;
    momentumUpdateWorld = DirectX::SimpleMath::Vector3::Transform(momentumUpdateWorld, m_heli.alignment);
    DirectX::SimpleMath::Vector3 momentumUpdateWorld2 = momentumUpdate2;
    momentumUpdateWorld2 = DirectX::SimpleMath::Vector3::Transform(momentumUpdateWorld2, m_heli.alignment);
    */
    //m_debugData->DebugPushUILineDecimalNumber("torqueForce    ", torqueForce.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("momentumUpdate ", momentumUpdate.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("momentumUpdate2", momentumUpdate2.Length(), "");

    //m_debugData->PushDebugLine(m_heli.q.position, torqueForceorld, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, momentumUpdateWorld, 15.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(m_heli.q.position, momentumUpdateWorld2, 20.0f, 0.0f, DirectX::Colors::Yellow);
}

void Vehicle::RightHandSide2(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    Motion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularMomentum = aQ->angularMomentum + static_cast<float>(aQScale) * aDeltaQ->angularMomentum;

    DirectX::SimpleMath::Vector3 velocityUpdate = m_heli.vehicleLinearForcesSum;
    DirectX::SimpleMath::Vector3 airResistance = CalculateDragLinear(newQ.velocity);

    velocityUpdate += airResistance;

    //  Assign right-hand side values.
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DirectX::SimpleMath::Vector3 momentumUpdate = newQ.angularMomentum;
    momentumUpdate *= m_angularDampConst;

    //DirectX::SimpleMath::Vector3 testInput = m_testTorqueLocal;
    DirectX::SimpleMath::Vector3 testInput = aHeli->vehicleAngularForcesSum;
    DirectX::SimpleMath::Vector3 torqueForce = testInput;
    //torqueForce += (m_heli.angularDrag / aTimeDelta);
    torqueForce += (m_heli.angularDrag);
    //torqueForce -= m_heli.q.angularVelocityUpdateTest * 0.1f;
    testInput += momentumUpdate;
    DirectX::SimpleMath::Vector3 testInputScaled = testInput * aTimeDelta;

    //Airplane.vAngularVelocity += Airplane.mInertiaInverse * (Airplane.vMoments - (Airplane.vAngularVelocity ^ (Airplane.mInertia * Airplane.vAngularVelocity))) * dt;
    //Airplane.vAngularVelocity += Airplane.mInertiaInverse * (Airplane.vMoments - (Airplane.vAngularVelocity ^ (Airplane.mInertia * Airplane.vAngularVelocity))) * dt;
    DirectX::SimpleMath::Vector3 zero = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 one = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 AirplanevAngularVelocity = newQ.angularVelocity;
    DirectX::SimpleMath::Vector3 testAngVel = DirectX::SimpleMath::Vector3::Zero;
    one = AirplanevAngularVelocity;
    //one = DirectX::SimpleMath::Vector3::Transform(one, m_testInertiaTensorLocal);
    one = DirectX::SimpleMath::Vector3::Transform(one, aHeli->inertiaMatrixTest);
    zero = torqueForce - (AirplanevAngularVelocity.Cross(one));
    //testAngVel += DirectX::SimpleMath::Vector3::Transform(zero, m_testInverseInertiaTensorLocal);
    testAngVel += DirectX::SimpleMath::Vector3::Transform(zero, aHeli->inverseInertiaMatrixTest);
    DirectX::SimpleMath::Vector3 momentsInput = momentumUpdate;
    DirectX::SimpleMath::Vector3 vAngularVelocity = testInput;
    vAngularVelocity = torqueForce;

    DirectX::SimpleMath::Vector3 toCrossVelocity = vAngularVelocity;
    //toCrossVelocity = DirectX::SimpleMath::Vector3::Transform(toCrossVelocity, m_testInertiaTensorLocal);
    toCrossVelocity = DirectX::SimpleMath::Vector3::Transform(toCrossVelocity, aHeli->inertiaMatrixTest);

    DirectX::SimpleMath::Vector3 toInverseInertiaVelocity = vAngularVelocity;
    DirectX::SimpleMath::Vector3 testCross = vAngularVelocity;
    testCross = testCross.Cross(toCrossVelocity);

    toInverseInertiaVelocity = (momentsInput - (vAngularVelocity.Cross(toCrossVelocity)));

    //toInverseInertiaVelocity = DirectX::SimpleMath::Vector3::Transform(toInverseInertiaVelocity, m_testInverseInertiaTensorLocal);
    toInverseInertiaVelocity = DirectX::SimpleMath::Vector3::Transform(toInverseInertiaVelocity, aHeli->inverseInertiaMatrixTest);
    vAngularVelocity = toInverseInertiaVelocity;

    DirectX::SimpleMath::Vector3 updatedAngularVelocity = torqueForce;
    updatedAngularVelocity = testAngVel;
    //testInput += vAngularVelocity;
    ///////
    float testFloat = powf(0.8f, aTimeDelta);

    momentumUpdate = testInput;
    //momentumUpdate *= 0.1f;
    momentumUpdate *= powf(0.9f, aTimeDelta);
    //momentumUpdate = momentumUpdate - (m_heli.q.angularVelocityUpdateTest * 1.1f);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    momentumUpdate = updatedAngularVelocity;
    momentumUpdate = DirectX::SimpleMath::Vector3::Transform(momentumUpdate, aHeli->inertiaMatrixTest);
    DirectX::SimpleMath::Vector3 momentumUpdate2 = updatedAngularVelocity;
    momentumUpdate2 = DirectX::SimpleMath::Vector3::Transform(momentumUpdate2, aHeli->inverseInertiaMatrixTest);
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * updatedAngularVelocity;
    //aDQ->angularMomentum = static_cast<float>(aTimeDelta) * torqueForce;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * momentumUpdate;
    //aDQ->angularMomentum = static_cast<float>(aTimeDelta) * (DirectX::SimpleMath::Vector3::UnitX * 100.0f);

    DirectX::SimpleMath::Vector3 torqueForceorld = torqueForce;
    torqueForceorld = DirectX::SimpleMath::Vector3::Transform(torqueForceorld, m_heli.alignment);
    DirectX::SimpleMath::Vector3 momentumUpdateWorld = momentumUpdate;
    momentumUpdateWorld = DirectX::SimpleMath::Vector3::Transform(momentumUpdateWorld, m_heli.alignment);
    DirectX::SimpleMath::Vector3 momentumUpdateWorld2 = momentumUpdate2;
    momentumUpdateWorld2 = DirectX::SimpleMath::Vector3::Transform(momentumUpdateWorld2, m_heli.alignment);

    m_debugData->DebugPushUILineDecimalNumber("torqueForce    ", torqueForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("momentumUpdate ", momentumUpdate.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("momentumUpdate2", momentumUpdate2.Length(), "");

    m_debugData->PushDebugLine(m_heli.q.position, torqueForceorld, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, momentumUpdateWorld, 15.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, momentumUpdateWorld2, 20.0f, 0.0f, DirectX::Colors::Yellow);
}

//  This method solves for the vehicle motion using a 4th-order Runge-Kutta solver
void Vehicle::RungeKutta4(struct HeliData* aHeli, double aTimeDelta)
{
    float prevAngularMomentum = m_heli.q.angularMomentum.Length();
    float prevAngularVelocity = m_heli.q.angularVelocity.Length();

    //  Define a convenience variables
    const float numEqns = static_cast<float>(aHeli->numEqns);
    //  Retrieve the current values of the dependent and independent variables.
    Motion q = aHeli->q;
    Motion dq1;
    Motion dq2;
    Motion dq3;
    Motion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aHeli, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aHeli, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aHeli, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aHeli, &q, &dq3, aTimeDelta, 1.0, &dq4);

    aHeli->time = aHeli->time + aTimeDelta;
    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    DirectX::SimpleMath::Vector3 angularVelocityUpdateTest = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdateTest = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
    
    q.velocity += velocityUpdate;
    q.position += posUpdate; 
    q.angularVelocity += angularVelocityUpdateTest;
    q.angularMomentum += angularMomentumUpdateTest;
    
    aHeli->q.velocity = q.velocity;
    aHeli->q.position = q.position;

    aHeli->q.angularVelocity = q.angularVelocity;
    aHeli->q.angularMomentum = q.angularMomentum;
}

void Vehicle::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
    m_fireControl->SetDebugData(aDebugPtr);
}

void Vehicle::SetEnvironment(Environment* aEnviron)
{
    m_environment = aEnviron;
}

void Vehicle::SetTestCollisionVelocityUpdate(DirectX::SimpleMath::Vector3 aCollisionVelocityUpdate)
{
    m_heli.testCollisionVelocityUpdate = aCollisionVelocityUpdate;
}

void Vehicle::SetTestCollisionImpulseForce(Utility::ImpulseForce aCollisionImpulseForce)
{
    m_heli.testCollisionImpulseForce = aCollisionImpulseForce;
}

void Vehicle::SetTestPostImpactVelocity(DirectX::SimpleMath::Vector3 aPostImpactVelocity)
{
    m_heli.testPostImpactVelocity = aPostImpactVelocity;
}

void Vehicle::SetModelController(std::shared_ptr<ModelController> aModelController)
{
    m_modelController = aModelController;
}

void Vehicle::StabilizeWeaponPitch(struct HeliData& aVehicle, const float aTimeStep)
{
    DirectX::SimpleMath::Quaternion weaponQuat = aVehicle.alignmentQuat;
    DirectX::SimpleMath::Quaternion yawQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(aVehicle.up, aVehicle.controlInput.turretYaw);
    weaponQuat *= yawQuat;
    weaponQuat.Normalize();

    DirectX::SimpleMath::Vector3 eulerVec = weaponQuat.ToEuler();

    float stabilizedPitch = aVehicle.controlInput.weaponPitch - eulerVec.z;

    if (stabilizedPitch > aVehicle.controlInput.weaponPitchMax)
    {
        stabilizedPitch = aVehicle.controlInput.weaponPitchMax;
    }
    else if (stabilizedPitch < aVehicle.controlInput.weaponPitchMin)
    {
        stabilizedPitch = aVehicle.controlInput.weaponPitchMin;
    }
  
    aVehicle.stabilizedWeaponPitch = stabilizedPitch;
}

void Vehicle::ToggleFireControlLaser()
{
    m_fireControl->ToggleTargetingLaser();
}

void Vehicle::UpdateAlignment(const float aTimeDelta)
{
    DirectX::SimpleMath::Quaternion prevAlignQuat = m_heli.alignmentQuat;
    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = m_heli.q.angularVelocity.x;
    updateAlignQuat.y = m_heli.q.angularVelocity.y;
    updateAlignQuat.z = m_heli.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    //DirectX::SimpleMath::Quaternion updateAlignOutputQuat = m_heli.q.angularQuat;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = m_heli.alignmentQuat;
    updateAlignOutputQuat.Normalize();
    //m_heli.q.angularQuatUpdateTest.Normalize();
    //updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * m_heli.q.angularQuat);
    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * m_heli.alignmentQuat);
    updateAlignOutputQuat.Normalize();
    //m_heli.q.angularQuat = updateAlignOutputQuat;
    m_heli.alignmentQuat = updateAlignOutputQuat;

    //m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularQuat);
    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.alignmentQuat);
    m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
    m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.alignmentInverse = m_heli.alignment;
    m_heli.alignmentInverse = m_heli.alignmentInverse.Invert();
    m_heli.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    m_heli.alignmentQuat.Normalize();
    m_heli.alignmentInverseQuat = m_heli.alignmentQuat;
    m_heli.alignmentInverseQuat.Inverse(m_heli.alignmentInverseQuat);
    m_heli.alignmentInverseQuat.Normalize();

    m_heli.cameraOrientation = DirectX::XMMatrixLookAtRH(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.cameraOrientation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);

    m_angQuatStep = m_heli.alignmentQuat / prevAlignQuat;
    m_angQuatStep.Normalize();
}

void Vehicle::UpdateAlignmentCamera()
{
    float yaw = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitX, m_heli.forward);
    float pitch = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, m_heli.up);
    float roll = Utility::GetAngleBetweenVectors(-DirectX::SimpleMath::Vector3::UnitZ, m_heli.right);
    roll = 0.0f;
    pitch = asin(m_heli.forward.y);
    yaw = atan2(-m_heli.forward.z, m_heli.forward.x);

    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, alignMat);

    testLine = DirectX::SimpleMath::Vector3::UnitX;
    testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_heli.cameraOrientation);

    m_heli.cameraOrientation = alignMat;
}

void Vehicle::UpdateBladeLiftForce(const float aTimeStep)
{
    float angleOfAttack = m_heli.mainRotor.bladeVec[0].pitchAngle;
    float rho = m_heli.airDensity;
    float Cl = CalculateLiftCoefficient(angleOfAttack);
    float surface = m_heli.mainRotor.length * m_heli.mainRotor.width;
    surface *= 1.0f;
    float y = m_heli.mainRotor.width;
    const float omega = (m_heli.mainRotor.rpm * 0.10472f) * aTimeStep; // 0.10472 is conversion of RPM to rads per second
    const float omega2 = (m_heli.mainRotor.rpm * 0.10472f);
    float lift = .5f * rho * omega * omega * Cl * y;

    m_heli.mainRotor.bladeVec[0].liftMag = lift;
    m_heli.mainRotor.bladeVec[0].liftForcePerSecond = .5f * rho * omega2 * omega2 * Cl * surface;
    Cl = CalculateLiftCoefficient(m_heli.mainRotor.bladeVec[1].pitchAngle);
    lift = .5f * rho * omega * omega * Cl * y;
    m_heli.mainRotor.bladeVec[1].liftMag = lift;
    m_heli.mainRotor.bladeVec[1].liftForcePerSecond = .5f * rho * omega2 * omega2 * Cl * surface;

    lift = .5f * rho * omega2 * omega2 * Cl * y;

    m_heli.mainRotor.bladeVec[0].liftForcePerSecond = UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[0].liftForcePerSecond);
    m_heli.mainRotor.bladeVec[1].liftForcePerSecond = UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[1].liftForcePerSecond);
    UpdateGroundEffectForce(m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond);
    // L = .5 * rho * omega^2 * Cl * y integral(r1 -> r2)  r^2 * dr    
}

DirectX::SimpleMath::Vector3 Vehicle::UpdateBodyTorqueLocalNew(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = m_heli.localTailRotorPos - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 tailForce = (-DirectX::SimpleMath::Vector3::UnitZ * (m_heli.controlInput.yawPedalInput) * m_heli.yawForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.localCenterOfMass - m_heli.localGravityTorqueArmPos;
    //DirectX::SimpleMath::Vector3 gravForce = m_heli.up * -m_heli.gravity.y * m_heli.mass;
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.centerOfMass - m_heli.gravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.testCenterOfMass - m_heli.gravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 gravForce = m_heli.gravity * m_heli.mass;
    //Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, m_heli.gravity * m_gravTorqueModTest);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravForce);
    Utility::Torque weaponTorque;
    weaponTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    weaponTorque.magnitude = 0.0f;
    if (m_testImpulseForce.isActive == true)
    {
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
        weaponForce *= weaponForceMag;
        weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        //weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        if (weaponTorque.magnitude > 1000.0f)
        {
            //weaponTorque.magnitude *= 0.1f; 
        }
        int testBreak = 0;
        testBreak++;
    }

    DirectX::SimpleMath::Vector3 tailVec = tailTorque.axis * tailTorque.magnitude;
    DirectX::SimpleMath::Vector3 gravVec = gravTorque.axis * gravTorque.magnitude;
    gravVec = DirectX::SimpleMath::Vector3::Transform(gravVec, m_heli.alignmentInverseQuat);
    DirectX::SimpleMath::Vector3 driveVec = m_testDrivetrainTorqueLocal.axis * m_testDrivetrainTorqueLocal.magnitude;

    DirectX::SimpleMath::Vector3 weaponVec = weaponTorque.axis * weaponTorque.magnitude;
    //DirectX::SimpleMath::Vector3 terrainVec = m_testDrivetrainTorqueLocal.axis;
    DirectX::SimpleMath::Vector3 terrainVec = m_testTerrainNormTorque.axis;
    terrainVec.Normalize();
    terrainVec *= m_testTerrainNormTorque.magnitude;
    //m_debugData->DebugPushUILineDecimalNumber("m_testTerrainNormTorque.magnitude ", m_testTerrainNormTorque.magnitude, "");

    DirectX::SimpleMath::Vector3 pendVec = aPendTorque.axis * aPendTorque.magnitude;

    DirectX::SimpleMath::Vector3 windVanVec = m_heli.windVaningTorueForce;
    DirectX::SimpleMath::Vector3 torqueVec = driveVec + tailVec;
    torqueVec = tailVec + driveVec + weaponVec + terrainVec;
    //torqueVec = tailVec + weaponVec + terrainVec;
    //torqueVec = tailVec + driveVec + weaponVec + gravVec;
    //torqueVec = tailVec + weaponVec + gravVec;

    DirectX::SimpleMath::Vector3 gravVecWorld = gravVec;
    gravVecWorld = DirectX::SimpleMath::Vector3::Transform(gravVecWorld, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, gravVecWorld, 10.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 terrainVecWorld = terrainVec;
    terrainVecWorld = DirectX::SimpleMath::Vector3::Transform(terrainVecWorld, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, terrainVecWorld, 12.0f, 0.0f, DirectX::Colors::Teal);

    DirectX::SimpleMath::Vector3 driveVecWorld = driveVec;
    driveVecWorld = DirectX::SimpleMath::Vector3::Transform(driveVecWorld, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, driveVecWorld, 12.0f, 0.0f, DirectX::Colors::Lime);

    aAccelVec = torqueVec;

    return torqueVec;
}


DirectX::SimpleMath::Vector3 Vehicle::UpdateBodyTorqueLocal(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep)
{
    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = m_heli.localTailRotorPos - m_heli.localCenterOfMass;

    DirectX::SimpleMath::Vector3 tailForce = (-DirectX::SimpleMath::Vector3::UnitZ * (m_heli.controlInput.yawPedalInput) * m_heli.yawForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);

    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.localCenterOfMass - m_heli.localGravityTorqueArmPos;

    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, m_heli.gravity * m_gravTorqueModTest);

    Utility::Torque weaponTorque;
    weaponTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    weaponTorque.magnitude = 0.0f;
    if (m_testImpulseForce.isActive == true)
    {
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
        weaponForce *= weaponForceMag;
        weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        //weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        if (weaponTorque.magnitude > 1000.0f)
        {
            //weaponTorque.magnitude *= 0.1f; 
        }
        int testBreak = 0;
        testBreak++;
    }

    DirectX::SimpleMath::Vector3 tailVec = tailTorque.axis * tailTorque.magnitude;
    DirectX::SimpleMath::Vector3 gravVec = gravTorque.axis * gravTorque.magnitude;
    DirectX::SimpleMath::Vector3 driveVec = m_testDrivetrainTorqueLocal.axis * m_testDrivetrainTorqueLocal.magnitude;

    DirectX::SimpleMath::Vector3 weaponVec = weaponTorque.axis * weaponTorque.magnitude;
    //DirectX::SimpleMath::Vector3 terrainVec = m_testDrivetrainTorqueLocal.axis;
    DirectX::SimpleMath::Vector3 terrainVec = m_testTerrainNormTorque.axis;
    terrainVec.Normalize();
    terrainVec *= m_testTerrainNormTorque.magnitude;
    m_debugData->DebugPushUILineDecimalNumber("m_testTerrainNormTorque.magnitude ", m_testTerrainNormTorque.magnitude, "");

    DirectX::SimpleMath::Vector3 pendVec = aPendTorque.axis * aPendTorque.magnitude;

    DirectX::SimpleMath::Vector3 windVanVec = m_heli.windVaningTorueForce;
    DirectX::SimpleMath::Vector3 torqueVec = driveVec + tailVec;

    gravVec = CalculateStabilityTorqueLocal(m_heli, aTimeStep);

    DirectX::SimpleMath::Vector3 selfRightingForce = CalculateSelfRightingForce(m_heli);

    DirectX::SimpleMath::Vector3 eulerRot = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion testAlignQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    testAlignQuat.Normalize();
    eulerRot = m_heli.alignmentQuat.ToEuler();
    //eulerRot = testAlignQuat.ToEuler();
    /*
    m_debugData->DebugPushUILineDecimalNumber("eulerRot.x ", eulerRot.x, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRot.y ", eulerRot.y, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRot.z ", eulerRot.z, "");

    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularVelocity.x ", m_heli.q.angularVelocity.x, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularVelocity.y ", m_heli.q.angularVelocity.y, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularVelocity.z ", m_heli.q.angularVelocity.z, "");

    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularMomentum.x ", m_heli.q.angularMomentum.x, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularMomentum.y ", m_heli.q.angularMomentum.y, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.angularMomentum.z ", m_heli.q.angularMomentum.z, "");
    */

    //DirectX::SimpleMath::Quaternion testLocalQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRot.y, eulerRot.z, eulerRot.x);
    DirectX::SimpleMath::Quaternion testLocalQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRot.x, eulerRot.z);
    //testLocalQuat = testAlignQuat;
    DirectX::SimpleMath::Vector3 testUp = DirectX::SimpleMath::Vector3::UnitY;
    testUp = DirectX::SimpleMath::Vector3::Transform(testUp, testLocalQuat);
    //m_debugData->PushDebugLine(m_heli.q.position, testUp, 15.0f, 0.0f, DirectX::Colors::Yellow);

    Utility::Torque testTorque;
    testTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    testTorque.magnitude = 0.0f;
    DirectX::SimpleMath::Vector3 testVel = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 pendulumForce = UpdatePendulumMotion2(testTorque, testVel, aTimeStep);
    pendulumForce *= 0.07f;
    //pendulumForce = DirectX::SimpleMath::Vector3::Transform(pendulumForce, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, pendulumForce, 15.0f, 0.0f, DirectX::Colors::Orange);

    DirectX::SimpleMath::Vector3 terrainVecNorm = terrainVec;
    terrainVecNorm.Normalize();
    //DirectX::SimpleMath::Vector3 angVelNorm = m_heli.q.angularVelocity;
    DirectX::SimpleMath::Vector3 angVelNorm = m_heli.q.angularMomentum;
    angVelNorm.Normalize();
    float angDot = -terrainVecNorm.Dot(angVelNorm);
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float inverseDot = 1.0f - upDot;
    m_debugData->DebugPushUILineDecimalNumber("angDot  = ", angDot, "");
    m_debugData->DebugPushUILineDecimalNumber("upDot  = ", upDot, "");
    m_debugData->DebugPushUILineDecimalNumber("inverseDot  = ", inverseDot, "");
 
    //m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 angMomNorm = m_heli.q.angularMomentum;
    angMomNorm.Normalize();
    //angMomNorm = DirectX::SimpleMath::Vector3::Transform(angMomNorm, m_heli.alignment);
    DirectX::SimpleMath::Vector3 angVelNorm2 = m_heli.q.angularVelocity;
    angVelNorm2.Normalize();
    //angVelNorm2 = DirectX::SimpleMath::Vector3::Transform(angVelNorm2, m_heli.alignment);

    DirectX::SimpleMath::Vector3 terrainVecNorm2 = terrainVec;
    terrainVecNorm2.Normalize();

    float terrainDot = terrainVecNorm2.Dot(angMomNorm);
    m_debugData->DebugPushUILineDecimalNumber("terrainDot   = ", terrainDot, "");
    float terrainDot2 = terrainVecNorm2.Dot(-angMomNorm);
    m_debugData->DebugPushUILineDecimalNumber("terrainDot2  = ", terrainDot2, "");

    float inverseDot2 = terrainDot;
    float t = 0.1f;
    if (inverseDot2 < 0.0f)
    {
        inverseDot2 = -1.0f - inverseDot2;
        t = -t + inverseDot2;
    }
    else
    {
        inverseDot2 = 1.0f - inverseDot2;
        t = t + inverseDot2;
    }
    //inverseDot2 = abs(inverseDot2);
    m_debugData->DebugPushUILineDecimalNumber("inverseDot2  = ", inverseDot2, "");
    m_debugData->DebugPushUILineDecimalNumber("t            = ", t, "");
    //float t = 0.2f;
    //t = 0.1 + inverseDot2;
    DirectX::SimpleMath::Vector3 smartTerrainVec = DirectX::SimpleMath::Vector3::Lerp(terrainVecNorm2, -angMomNorm, t);


    smartTerrainVec *= terrainVec.Length();
    terrainVec = smartTerrainVec;
    if (angMomNorm.Length() > 0.5f)
    {
        DirectX::SimpleMath::Quaternion smartQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(-angMomNorm, m_testAngularRotationPerSecond);
        //terrainVec = DirectX::SimpleMath::Vector3::Transform(terrainVec, smartQuat);
    }
    smartTerrainVec = DirectX::SimpleMath::Vector3::Transform(smartTerrainVec, m_heli.alignment);
    angMomNorm = DirectX::SimpleMath::Vector3::Transform(angMomNorm, m_heli.alignment);
    angVelNorm2 = DirectX::SimpleMath::Vector3::Transform(angVelNorm2, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, angMomNorm, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, angVelNorm2, 13.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(m_heli.q.position, smartTerrainVec, 13.0f, 0.0f, DirectX::Colors::Yellow);

    if (angDot < 0.0f)
    {
        //terrainVec *= inverseDot * 5.0f;
    }

    DirectX::SimpleMath::Vector3 testStabilityVec = TestStabilityForce3(terrainVec, aTimeStep);

    torqueVec = tailVec + gravVec + driveVec + weaponVec;
    torqueVec = tailVec + gravVec + driveVec + weaponVec + terrainVec;
    torqueVec = tailVec + driveVec + weaponVec + terrainVec;
    //torqueVec = tailVec + driveVec + weaponVec + pendulumForce;
    //torqueVec = tailVec + driveVec + weaponVec + gravVec;
    //torqueVec = tailVec + driveVec + weaponVec + selfRightingForce;
    //torqueVec = tailVec + driveVec;
    //torqueVec = driveVec + gravVec;
    //torqueVec = driveVec;
    //torqueVec = tailVec + driveVec + testStabilityVec;
    //torqueVec = tailVec + driveVec;
    torqueVec = tailVec + driveVec + weaponVec + gravVec;
    DirectX::SimpleMath::Vector3 gravVecWorld = gravVec;
    gravVecWorld = DirectX::SimpleMath::Vector3::Transform(gravVecWorld, m_heli.alignment); 
    //m_debugData->PushDebugLine(m_heli.q.position, gravVecWorld, 10.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 terrainVecWorld = terrainVec;
    terrainVecWorld = DirectX::SimpleMath::Vector3::Transform(terrainVecWorld, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, terrainVecWorld, 12.0f, 0.0f, DirectX::Colors::Teal);

    DirectX::SimpleMath::Vector3 driveVecWorld = driveVec;
    driveVecWorld = DirectX::SimpleMath::Vector3::Transform(driveVecWorld, m_heli.alignment);
    //m_debugData->PushDebugLine(m_heli.q.position, driveVecWorld, 12.0f, 0.0f, DirectX::Colors::Lime);

    aAccelVec = torqueVec;
    m_debugData->DebugPushUILineDecimalNumber("driveVec ", driveVec.Length(), "");
    return torqueVec;
}

DirectX::SimpleMath::Vector3 Vehicle::TestStabilityForce(const float aTimeStep)
{
    m_debugData->DebugClearUI();
    float maxForce = m_stabilityTorqueForceMax;
    maxForce = 800.0f;
    DirectX::SimpleMath::Vector3 eulerRotVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion testAlignQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    testAlignQuat.Normalize();
    eulerRotVec = m_heli.alignmentQuat.ToEuler();
    //eulerRot = testAlignQuat.ToEuler();
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.x ", eulerRotVec.x, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.y ", eulerRotVec.y, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.z ", eulerRotVec.z, "");
    //DirectX::SimpleMath::Quaternion testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, eulerRotVec.z, eulerRotVec.x);
    //DirectX::SimpleMath::Quaternion testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, eulerRotVec.x, eulerRotVec.z);
    DirectX::SimpleMath::Quaternion testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRotVec.z, eulerRotVec.x);
    testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRotVec.x, eulerRotVec.z);
    testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRotVec.x, eulerRotVec.z);
    testLocalQuatForward = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, eulerRotVec.z);

    DirectX::SimpleMath::Vector3 testForward = DirectX::SimpleMath::Vector3::UnitX;
    testForward = DirectX::SimpleMath::Vector3::Transform(testForward, testLocalQuatForward);
    m_debugData->PushDebugLine(m_heli.q.position, testForward, 20.0f, 0.0f, DirectX::Colors::Fuchsia);

    DirectX::SimpleMath::Vector3 stabilityForceForward = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityTorqueForward = DirectX::SimpleMath::Vector3::Zero;;
    DirectX::SimpleMath::Vector3 localizedGravityForward = DirectX::SimpleMath::Vector3::UnitX * (-m_heli.gravity.y * maxForce);
    localizedGravityForward = DirectX::SimpleMath::Vector3::Transform(localizedGravityForward, m_heli.alignmentInverse);

    Utility::AddForceAtPoint(localizedGravityForward, (DirectX::SimpleMath::Vector3::UnitX * m_gravStabilityArmMod), m_heli.localCenterOfMass, stabilityForceForward, stabilityTorqueForward);
   
    //DirectX::SimpleMath::Quaternion testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, eulerRotVec.z, eulerRotVec.x);
    //DirectX::SimpleMath::Quaternion testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, eulerRotVec.x, eulerRotVec.z);
    DirectX::SimpleMath::Quaternion testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRotVec.x, eulerRotVec.z);
    //DirectX::SimpleMath::Quaternion testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, 0.0f, eulerRotVec.x);
    testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, 0.0f, eulerRotVec.x);
    testLocalQuatRight = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, eulerRotVec.x, 0.0f);

    DirectX::SimpleMath::Vector3 testRight = DirectX::SimpleMath::Vector3::UnitZ;
    testRight = DirectX::SimpleMath::Vector3::Transform(testRight, testLocalQuatRight);
    m_debugData->PushDebugLine(m_heli.q.position, testRight, 20.0f, 0.0f, DirectX::Colors::Red);
    DirectX::SimpleMath::Vector3 stabilityForceRight = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 stabilityTorqueRight = DirectX::SimpleMath::Vector3::Zero;;
    DirectX::SimpleMath::Vector3 localizedGravityRight = DirectX::SimpleMath::Vector3::UnitZ * (-m_heli.gravity.y * maxForce);
    localizedGravityRight = DirectX::SimpleMath::Vector3::Transform(localizedGravityRight, m_heli.alignmentInverse);
    Utility::AddForceAtPoint(localizedGravityRight, (DirectX::SimpleMath::Vector3::UnitZ * m_gravStabilityArmMod), m_heli.localCenterOfMass, stabilityForceRight, stabilityTorqueRight);

    m_debugData->DebugPushUILineDecimalNumber("stabilityTorqueForward.lenght() ", stabilityTorqueForward.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("stabilityTorqueRight.lenght() ", stabilityTorqueRight.Length(), "");
    DirectX::SimpleMath::Vector3 stabilityUpdate = stabilityTorqueForward;
    stabilityUpdate += stabilityTorqueRight;

    DirectX::SimpleMath::Vector3 worldStabilityTorque = stabilityTorqueForward;
    worldStabilityTorque = DirectX::SimpleMath::Vector3::Transform(worldStabilityTorque, m_heli.alignment);

    m_debugData->PushDebugLine(m_heli.q.position, worldStabilityTorque, 20.0f, 0.0f, DirectX::Colors::White);
    m_debugData->DebugPushUILineDecimalNumber("stabilityUpdate.lenght() ", stabilityUpdate.Length(), "");

    //stabilityUpdate = DirectX::SimpleMath::Vector3::Zero;
    m_testVal += aTimeStep;
    return stabilityUpdate;
}

DirectX::SimpleMath::Vector3 Vehicle::TestStabilityForce2(const DirectX::SimpleMath::Vector3 aVec, const float aTimeStep)
{
    m_debugData->DebugClearUI();
    float maxForce = m_stabilityTorqueForceMax;
    maxForce = 800.0f;
    DirectX::SimpleMath::Vector3 eulerRotVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion testAlignQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    testAlignQuat.Normalize();
    eulerRotVec = m_heli.alignmentQuat.ToEuler();
    //eulerRot = testAlignQuat.ToEuler();
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.x ", eulerRotVec.x, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.y ", eulerRotVec.y, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerRotVec.z ", eulerRotVec.z, "");


    DirectX::SimpleMath::Vector3 eulerAccVec = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Quaternion testAccQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.q.angularMomentum);
    //testAccQuat.Normalize();
    eulerAccVec = m_heli.alignmentQuat.ToEuler();
    //eulerRot = testAlignQuat.ToEuler();
    
    /*
    m_debugData->DebugPushUILineDecimalNumber("eulerAccVec.x ", eulerAccVec.x, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerAccVec.y ", eulerAccVec.y, "");
    m_debugData->DebugPushUILineDecimalNumber("eulerAccVec.z ", eulerAccVec.z, "");

    m_debugData->DebugPushUILineDecimalNumber("m_heli.alignmentQuatDelta.x ", m_heli.alignmentQuatDelta.x, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.alignmentQuatDelta.y ", m_heli.alignmentQuatDelta.y, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.alignmentQuatDelta.z ", m_heli.alignmentQuatDelta.z, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.alignmentQuatDelta.w ", m_heli.alignmentQuatDelta.w, "");
    */
    DirectX::SimpleMath::Quaternion alignQuatTest = m_heli.alignmentQuat;
    DirectX::SimpleMath::Quaternion angAccQuatTest = m_heli.alignmentQuatDelta;
    DirectX::SimpleMath::Quaternion angAccQuatInverseTest = m_heli.alignmentInverseQuatDelta;
    DirectX::SimpleMath::Quaternion desiredQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(eulerRotVec.y, 0.0f, 0.0f);
    desiredQuat.Normalize();

    DirectX::SimpleMath::Quaternion rotateTowards = angAccQuatInverseTest;

    rotateTowards.RotateTowards(desiredQuat, (m_heli.angularRadsPerSec * aTimeStep) * 0.1f);
    rotateTowards.Normalize();
    DirectX::SimpleMath::Vector3 testForward = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRight = DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 testUp = DirectX::SimpleMath::Vector3::UnitY;



    float t = 0.3f;
    DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Slerp(angAccQuatTest, desiredQuat, t);
    DirectX::SimpleMath::Vector3 stabilityUpdate = DirectX::SimpleMath::Vector3::Zero;
    stabilityUpdate = aVec;
    stabilityUpdate = DirectX::SimpleMath::Vector3::Transform(stabilityUpdate, updateQuat);
    stabilityUpdate = aVec;
    stabilityUpdate = DirectX::SimpleMath::Vector3::Transform(stabilityUpdate, m_heli.alignmentQuat);
    stabilityUpdate = DirectX::SimpleMath::Vector3::Transform(stabilityUpdate, rotateTowards);
    stabilityUpdate = DirectX::SimpleMath::Vector3::Transform(stabilityUpdate, m_heli.alignmentInverseQuat);

    testForward = DirectX::SimpleMath::Vector3::Transform(testForward, desiredQuat);
    testRight = DirectX::SimpleMath::Vector3::Transform(testRight, desiredQuat);
    testUp = DirectX::SimpleMath::Vector3::Transform(testUp, desiredQuat);
    //m_debugData->PushDebugLine(m_heli.q.position, testForward, 15.0f, 0.0f, DirectX::Colors::Fuchsia);
    //m_debugData->PushDebugLine(m_heli.q.position, testRight, 15.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, testUp, 15.0f, 0.0f, DirectX::Colors::Blue);

    float stability = 0.3f;
    float speed = 2.0f;
    DirectX::SimpleMath::Vector3 predictedUp = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 predictedAxis = m_heli.q.angularMomentum;
    DirectX::SimpleMath::Quaternion predictedQuat = DirectX::SimpleMath::Quaternion::Identity;
    if (m_heli.angularRadsPerSec != 0.0f)
    {
        predictedQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(predictedAxis, (m_heli.angularRadsPerSec * (stability / speed)));
    }
    //DirectX::SimpleMath::Quaternion predictedQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(predictedAxis, (m_heli.angularRadsPerSec * (stability / speed)) + 0.000001f);
    predictedUp = m_heli.up;
    predictedUp = DirectX::SimpleMath::Vector3::Transform(predictedUp, predictedQuat);
    m_debugData->DebugPushUILineDecimalNumber("predictedUp.Length() ", predictedUp.Length(), "");

    DirectX::SimpleMath::Vector3 torqueVector = predictedUp.Cross(m_heli.up);
    torqueVector.Normalize();
    m_debugData->PushDebugLine(m_heli.q.position, torqueVector, 10.0f, 0.0f, DirectX::Colors::Teal);
    m_debugData->PushDebugLine(m_heli.q.position, aVec, 15.0f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->PushDebugLine(m_heli.q.position, predictedUp, 15.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.up, 10.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 testVec = torqueVector;
    m_debugData->DebugPushUILineDecimalNumber("testVec.Length()1 ", testVec.Length(), "");
    testVec *= aVec.Length();
    m_debugData->DebugPushUILineDecimalNumber("testVec.Length()2 ", testVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("aVec.Length()     ", aVec.Length(), "");
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, m_heli.alignmentInverseQuat);

    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.localCenterOfMass - m_heli.localGravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 gravForce = predictedUp * m_stabilityTorqueForceMax;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravForce);

    testVec = gravTorque.axis;
    testVec *= gravTorque.magnitude;
    m_debugData->DebugPushUILineDecimalNumber("testVec.Length()3 ", testVec.Length(), "");

    DirectX::SimpleMath::Vector3 anticipateAxis = m_heli.q.angularMomentum;
    anticipateAxis.Normalize();
    DirectX::SimpleMath::Quaternion anticipateQuat = DirectX::SimpleMath::Quaternion::Identity;

    if (anticipateAxis.Length() > 0.5f)
    {
        anticipateQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(anticipateAxis, -m_heli.angularRadsPerSec);
    }
    anticipateQuat.Normalize();
    //testVec = DirectX::SimpleMath::Vector3::Transform(testVec, anticipateQuat);
    //testVec = DirectX::SimpleMath::Vector3::Transform(testVec, m_heli.alignmentInverseQuatDelta);
    m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 aVecWorld = aVec;
    aVecWorld = DirectX::SimpleMath::Vector3::Transform(aVecWorld, m_heli.alignment);
    DirectX::SimpleMath::Vector3 testVecWord = testVec;
    testVecWord = DirectX::SimpleMath::Vector3::Transform(testVecWord, m_heli.alignment);
    DirectX::SimpleMath::Vector3 momVecWord = m_heli.q.angularMomentum;
    momVecWord = DirectX::SimpleMath::Vector3::Transform(momVecWord, m_heli.alignment);

    m_debugData->PushDebugLine(m_heli.q.position, testVecWord, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, aVecWorld, 15.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, momVecWord, 13.0f, 0.0f, DirectX::Colors::Yellow);
    //return stabilityUpdate;
    //return aVec;
    return testVec;
}

DirectX::SimpleMath::Vector3 Vehicle::TestStabilityForce3(const DirectX::SimpleMath::Vector3 aVec, const float aTimeStep)
{
    //m_debugData->DebugClearUI();


    const float maxForce = m_stabilityTorqueForceMax;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.centerOfMass - m_heli.gravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 gravForce = - DirectX::SimpleMath::Vector3::UnitY * maxForce;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravForce);

    DirectX::SimpleMath::Vector3 updateForce = gravTorque.axis;
    updateForce *= gravTorque.magnitude;
    updateForce = DirectX::SimpleMath::Vector3::Transform(updateForce, m_heli.alignmentInverseQuat);
    return updateForce;
}

void Vehicle::UpdateBrakeForce(const float aTimeStep)
{
    if (m_heli.controlInput.isBrakePressed == true)
    {
        m_heli.controlInput.brakeInput += m_heli.controlInput.brakeInputRate * aTimeStep;
        if (m_heli.controlInput.brakeInput > m_heli.controlInput.brakeInputMax)
        {
            m_heli.controlInput.brakeInput = m_heli.controlInput.brakeInputMax;
        }
    }
    else if (m_heli.controlInput.isBrakePressed == false)
    {
        m_heli.controlInput.brakeInput -= m_heli.controlInput.brakeInputRate * aTimeStep;
        if (m_heli.controlInput.brakeInput < m_heli.controlInput.brakeInputMin)
        {
            m_heli.controlInput.brakeInput = m_heli.controlInput.brakeInputMin;
        }
    }
    m_heli.controlInput.isBrakePressed = false;

    if (m_heli.controlInput.brakeInput > m_heli.controlInput.brakeInputMin)
    {
        DirectX::SimpleMath::Vector3 antiVelocity = m_heli.q.velocity * -1.0f;
        DirectX::SimpleMath::Vector3 brakeNorm = antiVelocity;
        brakeNorm.y = 0.0f;
        const float brakableVelocity = brakeNorm.Length();
        brakeNorm.Normalize();
        DirectX::SimpleMath::Vector3 brakeForce = brakeNorm * ((m_heli.controlInput.brakeInput * m_heli.brakeMagMax) * brakableVelocity);
        m_heli.controlInput.brakeForce = brakeForce;
    }
    else
    {
        m_heli.controlInput.brakeForce = DirectX::SimpleMath::Vector3::Zero;
    }
}

void Vehicle::UpdateCollisionImpulseForces(const float aTimeStep)
{
    Utility::UpdateImpulseForceBellCurve(m_heli.testCollisionImpulseForce, aTimeStep);

    if (m_heli.testCollisionImpulseForce.isActive == false)
    {
        m_heli.isVehicleCollisionTrue = false;
    }
}

void Vehicle::UpdateCyclicData()
{
    //m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputPitchRaw;

    m_heli.controlInput.cyclicInputRoll = m_heli.controlInput.cyclicInputRollRaw * m_heli.controlInput.cyclicInputMax;
    m_heli.controlInput.cyclicInputPitch = m_heli.controlInput.cyclicInputPitchRaw * m_heli.controlInput.cyclicInputMax;
    //m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputRoll ", m_heli.controlInput.cyclicInputRoll, "");
}

void Vehicle::UpdateCyclicNorm()
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);

    //cyclicQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    //DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);


    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
        //updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);
    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;


    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormLocal, 10.0f, 0.0f, DirectX::Colors::Blue);

    //m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicNormLocal size = ", m_heli.controlInput.cyclicNormLocal.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicNormWorld size = ", m_heli.controlInput.cyclicNormWorld.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormLocal, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(m_heli.q.position, DirectX::SimpleMath::Vector3::UnitY, 7.0f, 0.0f, DirectX::Colors::White);


    //m_debugData->PushDebugLinePositionIndicator(m_heli.physicsPointRear, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_heli.physicsPointRight, 10.0f, 0.0f, DirectX::Colors::Yellow);
}

void Vehicle::UpdateCyclicStick(ControlInput& aInput)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, aInput.cyclicInputRoll, aInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > aInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = aInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    }

    aInput.cyclicStick = updatedCyclic;
}

float Vehicle::UpdateGroundEffectForce(const float aLiftForce)
{
    const float wingSpan = m_heli.mainRotor.length * 3.0f;
    const float altitude = m_heli.q.position.y - m_heli.terrainHightAtPos - 3.0f;
    const float groundEffectFactor = ((16.0f * altitude / wingSpan) * (16.0f * altitude / wingSpan)) / (1.0f + ((16.0f * altitude / wingSpan) * (16.0f * altitude / wingSpan)));

    float liftMod = 1.0f - groundEffectFactor;
    float groundEffectLift = aLiftForce * liftMod;

    return groundEffectLift;
}

void Vehicle::UpdateInertiaTensor(struct HeliData& aVehicle, const float aTimeStep)
{
    /*
    // init
    m_heli.localTurretInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    float xExtent = m_heli.turretTensorDimensions.x;
    float yExtent = m_heli.turretTensorDimensions.y;
    float zExtent = m_heli.turretTensorDimensions.z;
    float mass = m_heli.turretTensorMass;
    m_heli.localTurretInertiaMatrix._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_heli.localTurretInertiaMatrix._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_heli.localTurretInertiaMatrix._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    //m_heli.localTurretInertiaMatrix.Translation(DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));

    m_heli.localTurretInverseInertiaMatrix = m_heli.localTurretInertiaMatrix;
    m_heli.localTurretInverseInertiaMatrix = m_heli.localTurretInverseInertiaMatrix.Invert();
    // end init
    auto frontLocal = DirectX::SimpleMath::Vector3(xExtent, 0.0f, 0.0f);
    auto upLocal = DirectX::SimpleMath::Vector3(0.0f, yExtent, 0.0f);
    auto rightLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, zExtent);
    auto originLocal = m_modelController->GetTurretTransLocal();
    frontLocal += m_modelController->GetTurretTransLocal();
    rightLocal += m_modelController->GetTurretTransLocal();
    upLocal += m_modelController->GetTurretTransLocal();

    auto frontWorld = frontLocal;
    frontWorld = DirectX::SimpleMath::Vector3::Transform(frontWorld, aVehicle.alignment);
    frontWorld += aVehicle.q.position;
    auto rightWorld = rightLocal;
    rightWorld = DirectX::SimpleMath::Vector3::Transform(rightWorld, aVehicle.alignment);
    rightWorld += aVehicle.q.position;
    auto upWorld = upLocal;
    upWorld = DirectX::SimpleMath::Vector3::Transform(upWorld, aVehicle.alignment);
    upWorld += aVehicle.q.position;

    auto originWorld = originLocal;
    originWorld = DirectX::SimpleMath::Vector3::Transform(originWorld, aVehicle.alignment);
    originWorld += aVehicle.q.position;

    const float turretRotation = aVehicle.controlInput.turretYaw;

    auto turretMat = DirectX::SimpleMath::Matrix::Identity;
    turretMat = aVehicle.localTurretInertiaMatrix;
    turretMat *= DirectX::SimpleMath::Matrix::CreateRotationY(turretRotation);
    //turretMat = turretMat.Invert();

    auto updateMat = DirectX::SimpleMath::Matrix::Identity;
    //updateMat = turretMat * aVehicle.inverseInertiaMatrixTest;
    updateMat = turretMat * aVehicle.inertiaMatrixTest;
    updateMat = updateMat.Invert();

    aVehicle.inertiaTensorInverse = aVehicle.inverseInertiaMatrixTest;
    aVehicle.inertiaTensorInverse = updateMat;

    m_debugData->ToggleDebugOnOverRide();
    //m_debugData->DebugClearUI();

    m_debugData->PushDebugLinePositionIndicator(frontWorld, 3.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLinePositionIndicator(rightWorld, 3.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(upWorld, 3.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->PushDebugLinePositionIndicator(originWorld, 3.0f, 0.0f, DirectX::Colors::White);
    //m_debugData->PushDebugLinePositionIndicator(aVehicle.q.position, 3.0f, 0.0f, DirectX::Colors::White);
    m_debugData->ToggleDebugOff();
    */

    aVehicle.inertiaTensorInverse = aVehicle.inverseInertiaMatrixTest;
}

void Vehicle::UpdateInertiaTensorOld(struct HeliData& aVehicle, const float aTimeStep)
{
    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicle.right, aVehicle.up);

    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputPitch", m_heli.controlInput.cyclicInputPitch, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputRoll", m_heli.controlInput.cyclicInputRoll, "");
    const float pitchMax = Utility::ToRadians(30.0f);
    const float rollMax = Utility::ToRadians(30.0f);
    float pitchVal = aVehicle.controlInput.cyclicInputPitch * pitchMax;
    float rollVal = aVehicle.controlInput.cyclicInputRoll * rollMax;
    DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, rollVal, pitchVal);
    //rotQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitZ, rollVal);
    //DirectX::SimpleMath::Vector3 ballastTranslation = aVehicle.ballastInertiaTranslation;
    DirectX::SimpleMath::Vector3 ballastTranslation = aVehicle.ballastInertiaTranslationBase;
    //ballastTranslation = DirectX::SimpleMath::Vector3::Transform(ballastTranslation, aVehicle.alignmentQuat);
    ballastTranslation = DirectX::SimpleMath::Vector3::Transform(ballastTranslation, rotQuat);

    //ballastTranslation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, aVehicle.controlInput.cyclicInputRoll);
    DirectX::SimpleMath::Matrix ballastMat = aVehicle.localBallastInertiaMatrix;
    ballastMat *= DirectX::SimpleMath::Matrix::CreateTranslation(ballastTranslation);

    aVehicle.testLocalCenterOfMass = aVehicle.ballastInertiaTranslationBase;
    aVehicle.testLocalCenterOfMass.y *= 0.5f;
    aVehicle.testLocalCenterOfMass = DirectX::SimpleMath::Vector3::Transform(aVehicle.testLocalCenterOfMass, rotQuat);
    aVehicle.testCenterOfMass = aVehicle.testLocalCenterOfMass;
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(aVehicle.q.position, -aVehicle.right, aVehicle.up);
    aVehicle.testCenterOfMass = DirectX::SimpleMath::Vector3::Transform(aVehicle.testCenterOfMass, updateMat);

    DirectX::SimpleMath::Matrix updateTensor = aVehicle.localInertiaMatrixTest;
    updateTensor += ballastMat;
    //updateTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix updateInverseTensor = updateTensor;
    updateInverseTensor = updateInverseTensor.Invert();
    //aVehicle.inertiaMatrixTest = updateTensor;
    //aVehicle.inverseInertiaMatrixTest = updateInverseTensor;

    //m_debugData->PushDebugLinePositionIndicator(ballastTranslation + aVehicle.q.position, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.q.angularMomentum, 10.0f, 0.0f, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.q.angularVelocity, 10.0f, 0.0f, DirectX::Colors::Blue);

}

void Vehicle::UpdateModelColorVals(const float aTimeStep)
{
    const float centerVal = abs(m_heli.controlInput.cyclicInputPitch / m_heli.controlInput.cyclicInputMax);

    float leftYawVal = m_heli.controlInput.yawPedalInput / m_heli.controlInput.yawPedalInputMax;
    if (leftYawVal <= 0.0f)
    {
        leftYawVal = 0.0f;
    }
    float leftVal = m_heli.controlInput.cyclicInputRoll / m_heli.controlInput.cyclicInputMin;
    if (leftVal <= 0.0f)
    {
        leftVal = 0.0f;
    }
    leftVal += leftYawVal;

    float rightYawVal = m_heli.controlInput.yawPedalInput / m_heli.controlInput.yawPedalInputMin;
    if (rightYawVal <= 0.0f)
    {
        rightYawVal = 0.0f;
    }
    float rightVal = m_heli.controlInput.cyclicInputRoll / m_heli.controlInput.cyclicInputMax;
    if (rightVal <= 0.0f)
    {
        rightVal = 0.0f;
    }
    rightVal += rightYawVal;

    m_modelController->SetGlowVals(centerVal, leftVal, rightVal, m_heli.q.position, m_heli.forward, aTimeStep);

    auto volume = centerVal + leftVal + rightVal;
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }

    m_throttleVolume = volume;
}

void Vehicle::UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep)
{
    //float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 testRod = m_heli.localMainRotorPos - m_heli.localCenterOfMass;
    testRod = m_heli.mainRotorPos - m_heli.centerOfMass;
    testRod = m_heli.gravityTorqueArmPos - m_heli.centerOfMass;
    float rodLength = testRod.Length();


    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    cyclicQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    //DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);


    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        //updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    }



    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    rodUp = DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    //DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    DirectX::SimpleMath::Vector3 rod = rodUp + updatedCyclic;
    //float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);
    float thetaAngle = Utility::GetAngleBetweenVectors(m_heli.up, rod);


    m_debugData->PushDebugLine(m_heli.q.position, rod, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, updatedCyclic, 10.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.up, 5.0f, 0.0f, DirectX::Colors::White);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = rod;
    //torqueUpdate.axis = torqueUpdate.axis.Cross(DirectX::SimpleMath::Vector3::UnitY);
    torqueUpdate.axis = torqueUpdate.axis.Cross(-m_heli.up);
    torqueUpdate.axis.Normalize();
    //torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude = -(m_heli.gravity.y) * rodLength * sin(thetaAngle);
    torqueUpdate.magnitude = -(m_heli.gravity.y * 1.0f) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude *= 0.2f;
    //torqueUpdate.magnitude = -((abs(m_heli.controlInput.cyclicInputPitch) + abs(m_heli.controlInput.cyclicInputRoll)) * 20.0f) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation

    aTorque = torqueUpdate;

    m_debugData->DebugPushUILineDecimalNumber("torqueUpdate.magnitude =", torqueUpdate.magnitude, "");

    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    accelerationDirection.Normalize();
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);
    accelerationMag *= 10000.0f;
    //accelerationMag *= m_heli.mass;
    //m_debugData->DebugPushUILineDecimalNumber("accelerationMag =", accelerationMag, "");
    //m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f && 1 == 1)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        accelerationDirection *= accelerationMag;
        m_debugData->DebugPushUILineDecimalNumber("Pendulum on =", 0.0f, "");
        m_debugData->DebugPushUILineDecimalNumber("accelerationDirection.Length() = ", accelerationDirection.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("accelerationMag = ", accelerationMag, "");
        aVelocity += accelerationDirection;
    }

    //accelerationDirection *= accelerationMag * aTimeStep;
    accelerationDirection *= accelerationMag;
    /*
    m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("Pendulum on =", 0.0f, "");
    m_debugData->DebugPushUILineDecimalNumber("accelerationDirection.Length() = ", accelerationDirection.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("accelerationMag = ", accelerationMag, "");

    m_debugData->DebugPushTestLine(m_heli.q.position, aVelocity, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("aVelocity.Length() = ", aVelocity.Length(), "");
    */
    aVelocity += accelerationDirection;
}

DirectX::SimpleMath::Vector3 Vehicle::UpdatePendulumMotion2(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep)
{
    //float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 testRod = m_heli.localMainRotorPos - m_heli.localCenterOfMass;
    testRod = m_heli.mainRotorPos - m_heli.centerOfMass;
    testRod = m_heli.gravityTorqueArmPos - m_heli.centerOfMass;
    float rodLength = testRod.Length();


    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    cyclicQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    //DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);


    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        //updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    }



    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    rodUp = DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    //DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    DirectX::SimpleMath::Vector3 rod = rodUp + updatedCyclic;
    //float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);
    float thetaAngle = Utility::GetAngleBetweenVectors(m_heli.up, rod);


    //m_debugData->PushDebugLine(m_heli.q.position, rod, 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, updatedCyclic, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.up, 5.0f, 0.0f, DirectX::Colors::White);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = rod;
    //torqueUpdate.axis = torqueUpdate.axis.Cross(DirectX::SimpleMath::Vector3::UnitY);
    torqueUpdate.axis = torqueUpdate.axis.Cross(-m_heli.up);
    torqueUpdate.axis.Normalize();

    thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);
    rod = m_heli.up;
    torqueUpdate.axis = rod;
    torqueUpdate.axis = torqueUpdate.axis.Cross(DirectX::SimpleMath::Vector3::UnitY);
    torqueUpdate.axis.Normalize();

    //torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude = -(m_heli.gravity.y) * rodLength * sin(thetaAngle);
    torqueUpdate.magnitude = -(m_heli.gravity.y * 1.0f) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude *= 0.2f;
    //torqueUpdate.magnitude = -((abs(m_heli.controlInput.cyclicInputPitch) + abs(m_heli.controlInput.cyclicInputRoll)) * 20.0f) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation

    torqueUpdate.magnitude = ((-m_heli.gravity.y * m_heli.mass)) * sin(thetaAngle);

    aTorque = torqueUpdate;

    //m_debugData->DebugPushTestLine(m_heli.q.position, torqueUpdate.axis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("PendTorqueUpdate.magnitude =", torqueUpdate.magnitude, "");

    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    accelerationDirection.Normalize();
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);
    accelerationMag *= 10000.0f;
    //accelerationMag *= m_heli.mass;
    //m_debugData->DebugPushUILineDecimalNumber("accelerationMag =", accelerationMag, "");
    //m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f && 1 == 1)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        accelerationDirection *= accelerationMag;
        m_debugData->DebugPushUILineDecimalNumber("Pendulum on =", 0.0f, "");
        m_debugData->DebugPushUILineDecimalNumber("accelerationDirection.Length() = ", accelerationDirection.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("accelerationMag = ", accelerationMag, "");
        aVelocity += accelerationDirection;
    }

    //accelerationDirection *= accelerationMag * aTimeStep;
    accelerationDirection *= accelerationMag;
    /*
    m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("Pendulum on =", 0.0f, "");
    m_debugData->DebugPushUILineDecimalNumber("accelerationDirection.Length() = ", accelerationDirection.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("accelerationMag = ", accelerationMag, "");

    m_debugData->DebugPushTestLine(m_heli.q.position, aVelocity, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("aVelocity.Length() = ", aVelocity.Length(), "");
    */
    aVelocity += accelerationDirection;

    DirectX::SimpleMath::Vector3 pendulumForce = DirectX::SimpleMath::Vector3::Zero;
    pendulumForce = torqueUpdate.axis;
    pendulumForce.Normalize();
    pendulumForce *= torqueUpdate.magnitude;
    return pendulumForce;
}

void Vehicle::UpdatePhysicsPoints(struct HeliData& aVehicle, const float aTimeStep)
{
    //UpdateInertiaTensor(aVehicle, aTimeStep);
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(aVehicle.q.position, -aVehicle.right, aVehicle.up);

    aVehicle.centerOfMass = m_heli.localCenterOfMass;
    aVehicle.centerOfMass = DirectX::SimpleMath::Vector3::Transform(aVehicle.localCenterOfMass, updateMat);
    aVehicle.mainRotorPos = m_heli.localMainRotorPos;
    aVehicle.mainRotorPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localMainRotorPos, updateMat);

    aVehicle.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;
    aVehicle.gravityTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.gravityTorqueArmPos, updateMat);

    aVehicle.buoyancyTorqueArmPos = m_heli.localBuoyancyTorqueArmPos;
    aVehicle.buoyancyTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.buoyancyTorqueArmPos, updateMat);

    aVehicle.topPos = m_heli.localTopPos;
    aVehicle.topPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.topPos, updateMat);

    aVehicle.topTestPos = m_heli.localTopTestPos;
    aVehicle.topTestPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.topTestPos, updateMat);

    aVehicle.baseTestPos = m_heli.localBaseTestPos;
    aVehicle.baseTestPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.baseTestPos, updateMat);

    aVehicle.topPos = m_heli.localTopPos;
    aVehicle.topPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.topPos, updateMat);

    aVehicle.tailRotorPos = m_heli.localTailRotorPos;
    aVehicle.tailRotorPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localTailRotorPos, updateMat);

    aVehicle.landingGearPos = aVehicle.localLandingGearPos;
    aVehicle.landingGearPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localLandingGearPos, updateMat);

    aVehicle.physicsPointFront = m_heli.localPhysicsPointFront;
    aVehicle.physicsPointFront = DirectX::SimpleMath::Vector3::Transform(aVehicle.physicsPointFront, updateMat);

    aVehicle.physicsPointLeft = m_heli.localPhysicsPointLeft;
    aVehicle.physicsPointLeft = DirectX::SimpleMath::Vector3::Transform(aVehicle.physicsPointLeft, updateMat);

    aVehicle.physicsPointRear = m_heli.localPhysicsPointRear;
    aVehicle.physicsPointRear = DirectX::SimpleMath::Vector3::Transform(aVehicle.physicsPointRear, updateMat);

    aVehicle.physicsPointRight = m_heli.localPhysicsPointRight;
    aVehicle.physicsPointRight = DirectX::SimpleMath::Vector3::Transform(aVehicle.physicsPointRight, updateMat);

    // update with new model
    DirectX::SimpleMath::Matrix yawMat = DirectX::SimpleMath::Matrix::CreateRotationY(aVehicle.controlInput.turretYaw);
    DirectX::SimpleMath::Matrix pitchMat = DirectX::SimpleMath::Matrix::CreateRotationZ(aVehicle.controlInput.weaponPitch);
    DirectX::SimpleMath::Matrix tempMat = yawMat * pitchMat;

    aVehicle.weaponPos = aVehicle.localWeaponPos;
    aVehicle.weaponPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localWeaponPos, updateMat);

    DirectX::SimpleMath::Matrix weaponMuzzleMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicle.right, aVehicle.up);
    aVehicle.weaponDirection = aVehicle.localWeaponDirection;
    aVehicle.weaponDirection = DirectX::SimpleMath::Vector3::Transform(aVehicle.weaponDirection, tempMat);
    aVehicle.weaponDirection = DirectX::SimpleMath::Vector3::Transform(aVehicle.weaponDirection, weaponMuzzleMat);
    aVehicle.weaponDirection = m_modelController->GetWeaponDirWorld();

    //aVehicle.weaponPos = m_modelController->GetWeaponPos();
    aVehicle.weaponPos = m_modelController->GetMuzzlePos();
    aVehicle.muzzlePos = m_modelController->GetMuzzlePos();
    aVehicle.localizedMuzzlePos = m_modelController->GetLocalizedMuzzlePos();

    // bounding box
    updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicle.right, aVehicle.up);
    DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(updateMat);
    m_heli.boundingBox.Orientation = updateQuat;
    m_heli.boundingBox.Center = m_heli.q.position;

    // missle hardpoints
    aVehicle.worldMissileTubeLeftPos = m_modelController->GetMissileTubePosLeft();
    aVehicle.worldMissileTubeRightPos = m_modelController->GetMissileTubePosRight();
    aVehicle.worldMissileTubeLeftDir = m_modelController->GetMissileTubeDirLeft();
    aVehicle.worldMissileTubeRightDir = m_modelController->GetMissileTubeDirRight();

    aVehicle.worldMissileTubeLeftUp = m_modelController->GetMissileTubeLeftUp();
    aVehicle.worldMissileTubeRightUp = m_modelController->GetMissileTubeRightUp();
}

void Vehicle::UpdateResistance()
{
    /*
    Rair = (1/2) rho[mass ensity of air] V^2 Sp  Cd
        Sp = projected frontal area of vehicle normal to the direction V
        Cd = drag coeffient == 0.4?ish
        */
    float velocity = m_heli.q.velocity.Length();
    float drag = .5f * m_heli.dragCoefficient * m_heli.airDensity * m_heli.area * (velocity * velocity);

    m_heli.airResistance = drag;
}

void Vehicle::UpdateRotorData(HeliData& aHeliData, const double aTimer)
{
    UpdateRotorSpin(aHeliData, aTimer);
    UpdateRotorPitch(aHeliData);
}

void Vehicle::UpdateRotorPitch(HeliData& aHeliData)
{
    float mainRotorPitch = aHeliData.controlInput.collectiveInput * aHeliData.mainRotor.pitchAngleMax;
    for (unsigned int i = 0; i < aHeliData.mainRotor.bladeVec.size(); ++i)
    {
        aHeliData.mainRotor.bladeVec[i].pitchAngle = mainRotorPitch;
        DirectX::SimpleMath::Vector3 rotorDir = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX,
            DirectX::SimpleMath::Matrix::CreateRotationY(aHeliData.mainRotor.rotorRotation + (static_cast<float>(i) * aHeliData.mainRotor.angleBetweenBlades)));

        const float pitchAng = Utility::GetAngleBetweenVectors(aHeliData.controlInput.cyclicStick, rotorDir) - Utility::ToRadians(90.0f);
        if (mainRotorPitch + pitchAng > 0.0f)
        {
            aHeliData.mainRotor.bladeVec[i].pitchAngle = mainRotorPitch + pitchAng;
        }
        else
        {
            aHeliData.mainRotor.bladeVec[i].pitchAngle = 0.0f;
        }
    }
    float tailRotorPitch = aHeliData.tailRotor.neutralAngle + (aHeliData.controlInput.yawPedalInput * (aHeliData.tailRotor.pitchAngleMax * .5f));
    for (unsigned int i = 0; i < aHeliData.tailRotor.bladeVec.size(); ++i)
    {
        aHeliData.tailRotor.bladeVec[i].pitchAngle = tailRotorPitch;
    }
}

void Vehicle::UpdateRotorSpin(HeliData& aHeliData, const double aTimer)
{
    const float rpmMax = aHeliData.mainRotorMaxRPM;
    const float rpmMin = 0.0f;
    const float prevRPM = aHeliData.mainRotor.rpm;
    const float rpmThrottleSet = aHeliData.controlInput.throttleInput * rpmMax;
    float rpmUpdate = prevRPM;
    const float currentTorqueCurvePos = (prevRPM / rpmMax) + 0.001f; // Small value added so the value can push past 0 rpm value for prevRPM
    float rpmDelta;
    if (currentTorqueCurvePos < 0.333f)
    {
        const float revDeltaRate = 200.4f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else if (currentTorqueCurvePos < 0.666f)
    {
        const float revDeltaRate = 52.75f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    else
    {
        const float revDeltaRate = 1.85f;
        rpmDelta = revDeltaRate * currentTorqueCurvePos;
    }
    // accelerate
    if (rpmThrottleSet > prevRPM)
    {
        if (prevRPM + rpmDelta > rpmThrottleSet)
        {
            rpmUpdate = rpmThrottleSet;
        }
        else
        {
            rpmUpdate = prevRPM + rpmDelta;
        }
    }
    // deccelerate
    if (rpmThrottleSet < prevRPM)
    {
        if (prevRPM - rpmDelta < rpmThrottleSet)
        {
            rpmUpdate = rpmThrottleSet;
        }
        else
        {
            rpmUpdate = prevRPM - rpmDelta;
        }
    }
    // bracket rpm between min or max rate
    if (rpmUpdate > rpmMax)
    {
        rpmUpdate = rpmMax;
    }
    else if (rpmUpdate < rpmMin)
    {
        rpmUpdate = rpmMin;
    }

    aHeliData.mainRotor.rpm = rpmUpdate;

    const float mainRotorSpinUpdate = (aHeliData.mainRotor.rpm * 0.10472f) * static_cast<float>(aTimer); // 0.10472 is conversion of RPM to rads per second
    aHeliData.mainRotor.rotorRotation += mainRotorSpinUpdate;
    aHeliData.mainRotor.rotorRotation = Utility::WrapAngle(aHeliData.mainRotor.rotorRotation);

    const float tailRotarGearing = 0.75f;
    const float tailRotorSpinUpdate = mainRotorSpinUpdate * tailRotarGearing;
    aHeliData.tailRotor.rotorRotation += tailRotorSpinUpdate;
    aHeliData.tailRotor.rotorRotation = Utility::WrapAngle(aHeliData.tailRotor.rotorRotation);
}

DirectX::SimpleMath::Vector3 Vehicle::UpdateRotorForceRunge()
{
    float pitch = m_heli.controlInput.cyclicInputPitch;
    float roll = m_heli.controlInput.cyclicInputRoll;

    DirectX::SimpleMath::Vector3 swashplate = DirectX::SimpleMath::Vector3::UnitY;
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
    swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    float combinedAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, swashplate);

    if (combinedAngle > m_heli.controlInput.cyclicInputMax)
    {
        float ratio = m_heli.controlInput.cyclicInputMax / combinedAngle;
        pitch = m_heli.controlInput.cyclicInputPitch * ratio;
        roll = m_heli.controlInput.cyclicInputRoll * ratio;

        swashplate = DirectX::SimpleMath::Vector3::UnitY;
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationZ(pitch));
        swashplate = DirectX::SimpleMath::Vector3::Transform(swashplate, DirectX::SimpleMath::Matrix::CreateRotationX(roll));
    }

    DirectX::SimpleMath::Matrix pitchRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.right, pitch);
    DirectX::SimpleMath::Matrix rollRot = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.forward, roll);

    DirectX::SimpleMath::Vector3 updateForce = DirectX::SimpleMath::Vector3::TransformNormal(m_heli.up, pitchRot * rollRot);
    updateForce.Normalize();
    return updateForce;
}

void Vehicle::UpdateTerrainData()
{

    bool isVehicleInPlayUpdate = m_environment->GetVehicleUpdateData(m_heli.q.position, m_heli.terrainNormal,
        m_heli.terrainHightAtPos, m_heli.groundPlane);
    if (isVehicleInPlayUpdate == false)
    {
        // to do: add error handling if out of play
    }

    //m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    m_heli.altitude = m_heli.landingGearPos.y - m_heli.terrainHightAtPos;
    //m_debugData->DebugPushUILineDecimalNumber("altitude = ", m_heli.altitude, "");
    //m_heli.terrainNormal = m_environment->GetTerrainNormal(m_heli.q.position);
}

void Vehicle::UpdateTerrainNorm()
{
    m_heli.terrainNormal = m_environment->GetTerrainNormal(m_heli.q.position);
}

void Vehicle::UpdateTerrainNormTorqueNew()
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    Utility::Torque prevTorque = m_testTerrainNormTorque;
    Utility::Torque updateTorque;
    float forceMod = 1.0f;
    const float mass = m_testMass;
    float groundNormalForceRange = m_heli.groundNormalForceRange;

    if (m_heli.altitude > groundNormalForceRange)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
        const float forcePercentage = 1.0f;
        const DirectX::SimpleMath::Vector3 terrrainForce = DirectX::SimpleMath::Vector3::UnitY * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
        updateTorque.axis = DirectX::SimpleMath::Vector3::Zero;
        updateTorque.magnitude = 0.0f;
    }
    else
    {
        //const float forcePercentage = 1.0f - (m_heli.altitude / m_heli.groundNormalForceRange);
        const float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
    }

    float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
    if (forcePercentage < 0.0f)
    {
        forcePercentage = 0.0f;
    }
    //m_debugData->DebugPushUILineDecimalNumber("forcePercentage ", forcePercentage, "");
    const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
    //slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
    slopeForceUpdate = (terrrainForce * forcePercentage) * m_terrainTorqueForceMax;
    //m_debugData->DebugPushUILineDecimalNumber("slopeForceUpdate ", slopeForceUpdate.Length(), "");
    DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
    //DirectX::SimpleMath::Vector3 torqueArm = m_heli.testCenterOfMass - m_heli.topTestPos;
    updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
    //updateTorque = Utility::GetTorqueForce(torqueArm, -slopeForceUpdate);
    updateTorque.magnitude *= forceMod;

    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);

    //updateTorque.magnitude *= 1.0f;
    //updateTorque.magnitude *= 1.0f - abs(upDot);
    updateTorque.axis.Normalize();
    updateTorque.axis = DirectX::SimpleMath::Vector3::Transform(updateTorque.axis, m_heli.alignmentInverse);
    m_testTerrainNormTorque.axis = updateTorque.axis;
    m_testTerrainNormTorque.magnitude = updateTorque.magnitude;
}

void Vehicle::UpdateTerrainNormTorque()
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    Utility::Torque prevTorque = m_testTerrainNormTorque;
    Utility::Torque updateTorque;
    float forceMod = 1.0f;
    const float mass = m_testMass;
    float groundNormalForceRange = m_heli.groundNormalForceRange;

    if (m_heli.altitude > groundNormalForceRange)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
        const float forcePercentage = 1.0f;
        const DirectX::SimpleMath::Vector3 terrrainForce = DirectX::SimpleMath::Vector3::UnitY * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
        updateTorque.axis = DirectX::SimpleMath::Vector3::Zero;
        updateTorque.magnitude = 0.0f;
    }
    else
    {
        //const float forcePercentage = 1.0f - (m_heli.altitude / m_heli.groundNormalForceRange);
        const float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
    }

    float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
    m_debugData->DebugPushUILineDecimalNumber("forcePercentage ", forcePercentage, "");
    const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
    //slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
    slopeForceUpdate = (terrrainForce * forcePercentage) * m_terrainTorqueForceMax;
    
    DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
    updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
    updateTorque.magnitude *= forceMod;

    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);

    updateTorque.magnitude *= 1.0f;
    updateTorque.magnitude *= 1.0f - abs(upDot);
    updateTorque.axis.Normalize();
    updateTorque.axis = DirectX::SimpleMath::Vector3::Transform(updateTorque.axis, m_heli.alignmentInverse);
    m_testTerrainNormTorque.axis = updateTorque.axis;
    m_testTerrainNormTorque.magnitude = updateTorque.magnitude;
}

void Vehicle::UpdateTerrainNormTorque2()
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    Utility::Torque prevTorque = m_testTerrainNormTorque;
    Utility::Torque updateTorque;
    //float forceMod = 0.00001f;
    float forceMod = 0.01f;
    const float mass = m_testMass;
    //m_debugData->PushDebugLinePositionIndicator(m_heli.topTestPos, 10.0f, 0.0f, DirectX::Colors::Red);
    float groundNormalForceRange = m_heli.groundNormalForceRange;
    //m_debugData->DebugPushUILineDecimalNumber("groundNormalForceRange = " , groundNormalForceRange, "");
    //if (m_heli.altitude > m_heli.groundNormalForceRange)
    if (m_heli.altitude > groundNormalForceRange)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
        const float forcePercentage = 1.0f;
        const DirectX::SimpleMath::Vector3 terrrainForce = DirectX::SimpleMath::Vector3::UnitY * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
        updateTorque.axis = DirectX::SimpleMath::Vector3::Zero;
        updateTorque.magnitude = 0.0f;
    }
    else
    {
        //const float forcePercentage = 1.0f - (m_heli.altitude / m_heli.groundNormalForceRange);
        const float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= forceMod;
    }

    DirectX::SimpleMath::Vector3 terrainNorm = DirectX::SimpleMath::Vector3::UnitY;
    float testVal = m_testTimer2;
    if (testVal > DirectX::XM_PI)
    {
        //m_testTimer = -DirectX::XM_PI * 0.5f;
    }

    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationX(testVal);
    //terrainNorm = DirectX::SimpleMath::Vector3::Transform(terrainNorm, rotMat);


    float forcePercentage = 1.0f - (m_heli.altitude / groundNormalForceRange);
    forcePercentage = 0.5f;
    const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
    //const DirectX::SimpleMath::Vector3 terrrainForce = terrainNorm * (-m_heli.gravity.y);
    slopeForceUpdate = (terrrainForce * forcePercentage) * mass;
    DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
    updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
    updateTorque.magnitude *= forceMod;

    updateTorque.magnitude *= 1.5f;
    updateTorque.axis.Normalize();
    m_testTerrainNormTorque.axis = updateTorque.axis;
    m_testTerrainNormTorque.magnitude = updateTorque.magnitude;
}

void Vehicle::UpdateTestDrivetrainTorqueLocalNew(const float aTimer)
{
    const float torqueMax = m_driveTorqueForceMax;

    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;

    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    m_testDrivetrainTorqueLocal = torqueSumLocal;
}

void Vehicle::UpdateTestDrivetrainTorqueLocal(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

    
    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float lateralDot2 = m_heli.right.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float longitudinalDot2 = -m_heli.forward.Dot(DirectX::SimpleMath::Vector3::UnitY);
    /*
    m_debugData->DebugPushUILineDecimalNumber("lateralDot  ", lateralDot, "");
    m_debugData->DebugPushUILineDecimalNumber("lateralDot2 ", lateralDot2, "");
    m_debugData->DebugPushUILineDecimalNumber("longitudinalDot  ", longitudinalDot, "");
    m_debugData->DebugPushUILineDecimalNumber("longitudinalDot2 ", longitudinalDot2, "");
    m_debugData->DebugPushUILineDecimalNumber("upDot ", upDot, "");
    */
    const float torqueMax = m_driveTorqueForceMax;

    
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;
    
    
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicNormLocal.z * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicNormLocal.x * torqueMax) * m_heli.up;

    lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    longitudinalForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up;
    lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    longitudinalForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;

    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);

    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    //DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);
    //lateralTorqueLocal.magnitude *= 1.0 - abs(lateralDot);
    //longitudinalTorqueLocal.magnitude *= 1.0 - abs(longitudinalDot);

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    Utility::Torque torqueSumWorld;
    torqueSumWorld.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSumWorld.axis.Normalize();
    torqueSumWorld.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;
    torqueSumWorld.axis = DirectX::SimpleMath::Vector3::Transform(torqueSumWorld.axis, m_heli.alignmentInverse);

    //m_debugData->DebugPushUILineDecimalNumber("torqueSumLocal.magnitude ", torqueSumLocal.magnitude, "");
    //m_debugData->DebugPushUILineDecimalNumber("torqueSumWorld.magnitude ", torqueSumWorld.magnitude, "");

    m_testDrivetrainTorqueLocal = torqueSumLocal;
    //m_testDrivetrainTorqueLocal = torqueSumWorld;
}

void Vehicle::UpdateTestDrivetrainTorqueLocal2(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);

    const float torqueMax = m_driveTorqueForceMax;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;

    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicNormLocal.z * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicNormLocal.x * torqueMax) * m_heli.up;

    lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    longitudinalForce = (m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up;

    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);

    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    m_testDrivetrainTorqueLocal = torqueSumLocal;
}

void Vehicle::UpdateVehicle(const double aTimeDelta)
{
    m_testTimer += aTimeDelta;
    m_testTimer2 += aTimeDelta;
    m_testTimer3 += aTimeDelta;
    DirectX::SimpleMath::Quaternion preQuat = m_heli.alignmentQuat;
    StabilizeWeaponPitch(m_heli, static_cast<float>(aTimeDelta));
    UpdatePhysicsPoints(m_heli, static_cast<float>(aTimeDelta));
    //UpdateCyclicNorm();

    UpdateCyclicData();

    Utility::Torque prevGravTorque = m_testGravForce;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.mainRotorPos - m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 gravityForce = m_heli.gravity * (static_cast<float>(0.01));
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    m_testGravForce = gravTorque;

    DirectX::SimpleMath::Vector3 prevVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 prevPos = m_heli.q.position;

    UpdateTerrainData();

    //UpdateTerrainNorm();
    //m_heli.buoyancyForce = CalculateBuoyancyForce(m_heli);
    UpdateBrakeForce(static_cast<float>(aTimeDelta));
    
    if (m_testImpulseForce.isActive == true)
    {
        //Utility::UpdateImpulseForceBellCurve(m_testImpulseForce, static_cast<float>(aTimeDelta));
        Utility::ImpulseForce preForce = m_testImpulseForce;
        Utility::UpdateImpulseForceBellCurve3(m_testImpulseForce, static_cast<float>(aTimeDelta));
        Utility::ImpulseForce postForce = m_testImpulseForce;

        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.localWeaponPos - m_heli.localCenterOfMass;
        //DirectX::SimpleMath::Vector3 weaponForce = -m_heli.localWeaponDirection;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        float weaponForceMag = m_testImpulseForce.currentMagnitude * 0.05f;
        weaponForce *= weaponForceMag;
        Utility::Torque weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        //weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        weaponTorque = Utility::GetTorqueForce(m_testImpulseForce.torqueArm, m_testImpulseForce.torqueForceNorm * m_testImpulseForce.currentTorqueMagnitude);
        int testBreak = 0;
        testBreak++;
        if (weaponTorque.magnitude > 5000.0f)
        {
            testBreak++;
        }
        if (weaponTorque.axis.Length() > 1.1f || weaponTorque.axis.Length() < -0.9f)
        {
            testBreak++;
        }
        testBreak++;
    }
    if (m_testImpulseForce.isActive == true)
    {
        //UpdateCollisionImpulseForces(static_cast<float>(aTimeDelta));
    }
   
    UpdateImpulseForces(m_heli, aTimeDelta);

    UpdateInertiaTensor(m_heli, static_cast<float>(aTimeDelta));

    UpdateVehicleForces(static_cast<float>(aTimeDelta));
    RungeKutta4(&m_heli, aTimeDelta);

    if (m_heli.forward.Dot(m_heli.q.velocity) < 0.0)
    {
        m_heli.isVelocityBackwards = true;
    }
    else
    {
        m_heli.isVelocityBackwards = false;
    }

    DirectX::SimpleMath::Vector3 speed = m_heli.q.velocity;
    //speed.y = 0.0f;
    m_heli.speed = speed.Length();
    //m_debugData->DebugPushUILineDecimalNumber("Speed = ", m_heli.speed, "");
    //m_debugData->DebugPushUILineDecimalNumber("MPH = ", m_heli.speed * 2.237f, "");
    //m_debugData->DebugPushUILineDecimalNumber("Altitude = ", m_heli.altitude, "");
    InputDecayNew(aTimeDelta);
    InputDecay(aTimeDelta);

    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    m_heli.testAccelVec = (m_heli.q.velocity - prevVelocity) / static_cast<float>(aTimeDelta);

    UpdateModelColorVals(static_cast<float>(aTimeDelta));

    UpdateResistance();

    UpdateAlignment(static_cast<float>(aTimeDelta));
    UpdateAlignmentCamera();

    //TestDrawFireDirection();
    /*
    DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
    DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDirWorld();
    DirectX::SimpleMath::Vector3 launchUp = m_modelController->GetWeaponUpWorld();
    m_debugData->PushDebugLine(pos, launchDir, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(pos, launchUp, 10.0f, 0.0f, DirectX::Colors::Red);
    */

    //m_heli.isVehicleCollisionTrue = false;

    
    DirectX::SimpleMath::Quaternion postQuat = m_heli.alignmentQuat;
    DirectX::SimpleMath::Quaternion deltaQuat = (postQuat - preQuat) * (1.0f / aTimeDelta);
    deltaQuat.Normalize();
    m_heli.alignmentQuatDelta = deltaQuat;
    deltaQuat.Inverse(deltaQuat);
    deltaQuat.Normalize();
    m_heli.alignmentInverseQuatDelta = deltaQuat;

    float angRad = DirectX::SimpleMath::Quaternion::Angle(preQuat, postQuat);
    float angDegrees = Utility::ToDegrees(angRad);
    float angDegreesPerSecond = angDegrees / aTimeDelta;
    float angRadsPerSecond = angRad / aTimeDelta;
    m_heli.angularRadsPerSec = angRadsPerSecond;
    m_testAngularRotationPerSecond = angRadsPerSecond;


    //m_debugData->ToggleDebugOnOverRide();
    //pos = m_modelController->GetLocalizedTubeRightPos();
    //m_debugData->PushDebugLinePositionIndicator(m_modelController->GetLocalizedTubeRightPos(), 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLinePositionIndicator(m_modelController->GetLocalizedTubeLeftPos(), 10.0f, 0.0f, DirectX::Colors::Lime);

    //m_debugData->PushDebugLine(m_heli.q.position, m_modelController->GetLocalizedTubeRightPos(), 10.0f, 0.0f, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, m_modelController->GetLocalizedTubeLeftPos(), 10.0f, 0.0f, DirectX::Colors::Lime);

    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetLocalizedTubeLeftPos(), 15.0f, 0.0f, DirectX::Colors::Lavender);
    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetLocalizedTubeRightPos(), 14.0f, 0.0f, DirectX::Colors::Orange);
    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosLeft(), m_modelController->GetMissileTubeDirLeft(), 12.0f, 0.0f, DirectX::Colors::Lime);
    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetMissileTubeDirRight(), 10.0f, 0.0f, DirectX::Colors::Red);

    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosLeft(), m_modelController->GetMissileTubeDirLeft(), 12.0f, 0.0f, DirectX::Colors::Lime);
    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetMissileTubeDirRight(), 10.0f, 0.0f, DirectX::Colors::Red);

    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosLeft(), m_modelController->GetMissileTubeLeftUp(), 10.0f, 0.0f, DirectX::Colors::Lime);
    //m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetMissileTubeRightUp(), 10.0f, 0.0f, DirectX::Colors::Red);




    //m_debugData->ToggleDebugOnOverRide();
    m_debugData->PushDebugLine(m_modelController->GetMissileTubePosLeft(), m_modelController->GetMissileTubeTurretLocalLeftDir(), 10.0f, 0.0f, DirectX::Colors::Lime);
    m_debugData->PushDebugLine(m_modelController->GetMissileTubePosRight(), m_modelController->GetMissileTubeTurretLocalRightDir(), 10.0f, 0.0f, DirectX::Colors::Yellow);
    
    auto front = DirectX::SimpleMath::Vector3(m_inertiaModelX, 0.0f, 0.0f);
    front = DirectX::SimpleMath::Vector3::Transform(front, m_heli.alignment);
    front += m_heli.q.position;

    auto back = DirectX::SimpleMath::Vector3(-m_inertiaModelX, 0.0f, 0.0f);
    back = DirectX::SimpleMath::Vector3::Transform(back, m_heli.alignment);
    back += m_heli.q.position;

    auto right = DirectX::SimpleMath::Vector3(0.0f, 0.0f, m_inertiaModelZ);
    right = DirectX::SimpleMath::Vector3::Transform(right, m_heli.alignment);
    right += m_heli.q.position;

    auto left = DirectX::SimpleMath::Vector3(0.0f, 0.0f, - m_inertiaModelZ);
    left = DirectX::SimpleMath::Vector3::Transform(left, m_heli.alignment);
    left += m_heli.q.position;

    auto top = DirectX::SimpleMath::Vector3(0.0f, m_inertiaModelY, 0.0f);
    top = DirectX::SimpleMath::Vector3::Transform(top, m_heli.alignment);
    top += m_heli.q.position;

    auto bottom = DirectX::SimpleMath::Vector3(0.0f, - m_inertiaModelY, 0.0f);
    bottom = DirectX::SimpleMath::Vector3::Transform(bottom, m_heli.alignment);
    bottom += m_heli.q.position;

    m_debugData->PushDebugLinePositionIndicator(front, 3.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(right, 3.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(top, 3.0f, 0.0f, DirectX::Colors::White);

    m_debugData->PushDebugLinePositionIndicator(back, 3.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(left, 3.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(bottom, 3.0f, 0.0f, DirectX::Colors::White);

    m_debugData->ToggleDebugOff();
}

void Vehicle::UpdateVehicleFireControl(const double aTimeDelta)
{
    m_fireControl->UpdateFireControl(aTimeDelta);
}

void Vehicle::UpdateVehicleForces(const float aTimeStep)
{
    UpdateCollisionImpulseForces(static_cast<float>(aTimeStep));
    m_heli.angularDrag = CalculateDragAngularLocal(m_heli.q.angularVelocity, aTimeStep);
    //m_debugData->DebugPushUILineDecimalNumber("m_heli.angularDrag ", m_heli.angularDrag.Length(), "");
    //m_heli.angularDrag = CalculateDragAngularLocal(m_heli.q.angularMomentum);
    //UpdateTestDrivetrainTorqueLocal(aTimeStep);
    UpdateTestDrivetrainTorqueLocalNew(aTimeStep);
    //UpdateTerrainNormTorque();
    UpdateTerrainNormTorqueNew();

    // linear
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    if (m_testImpulseForce.isActive == true)
    {
        velocityUpdate += m_testImpulseForce.directionNorm * m_testImpulseForce.currentMagnitude;
    }

    DirectX::SimpleMath::Vector3 calcHoverDriveForce = CalculateHoverDriveForce(m_heli);
    DirectX::SimpleMath::Vector3 damperForce = GetDamperForce(GetAltitude(), m_heli.mass);
    DirectX::SimpleMath::Vector3 gravForce = GetAntiGravGravityForce(GetAltitude(), m_heli.gravity, m_heli.mass);
    gravForce = m_heli.gravity * m_heli.mass;
    DirectX::SimpleMath::Vector3 jetThrust = GetJetThrust(m_heli.forward, m_heli.controlInput.jetInput, m_heli.jetThrustMax);
    DirectX::SimpleMath::Vector3 rotorForce = m_heli.hoverFloat;
    rotorForce = DirectX::SimpleMath::Vector3::Transform(rotorForce, m_heli.alignment);
    rotorForce = rotorForce * m_heli.mass;
    rotorForce = GetHoverLift(rotorForce, GetAltitude());
    DirectX::SimpleMath::Vector3 slopeForce = GetSlopeForce(m_heli.terrainNormal, GetAltitude(), m_heli.groundNormalForceRange);
    m_heli.buoyancyForce = CalculateBuoyancyForce(m_heli);

    velocityUpdate += calcHoverDriveForce;
    //velocityUpdate += damperForce;
    velocityUpdate += gravForce;
    velocityUpdate += jetThrust;
    //velocityUpdate += rotorForce;
    velocityUpdate += m_heli.buoyancyForce;
    velocityUpdate += slopeForce;
    velocityUpdate += m_heli.controlInput.brakeForce;

    //m_heli.vehicleLinearForcesSum = velocityUpdate;

    DirectX::SimpleMath::Vector3 preVelocityUpdate = velocityUpdate;

    // angular

    m_heli.selfRightingForce = CalculateSelfRightingForce(m_heli);

    m_heli.windVaningTorueForce = CalculateWindVaningTorqueForce(m_heli);

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    //DirectX::SimpleMath::Vector3 tempVel = DirectX::SimpleMath::Vector3::Zero;
    //UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeStep));
    //UpdatePendulumMotion(pendTorque, tempVel, static_cast<float>(aTimeStep));
    DirectX::SimpleMath::Vector3 postVelocityUpdate = velocityUpdate;

    DirectX::SimpleMath::Vector3 accelVecUpdate = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 localAngularVec = UpdateBodyTorqueLocalNew(accelVecUpdate, pendTorque, static_cast<float>(aTimeStep));

    if (m_heli.altitude < 0.0f)
    {
        CalculateGroundImpactForce(velocityUpdate, localAngularVec);
    }

    m_heli.vehicleLinearForcesSum = velocityUpdate;
    m_heli.vehicleAngularForcesSum = localAngularVec;

    if (m_heli.impulseForceVec.size() > 0)
    {
        m_heli.vehicleLinearForcesSum += m_heli.impulseForceSum;
        m_heli.vehicleAngularForcesSum += m_heli.impulseTorqueSum.axis * m_heli.impulseTorqueSum.magnitude;
    }
}

void Vehicle::CalculateGroundImpactForce(DirectX::SimpleMath::Vector3& aForce, DirectX::SimpleMath::Vector3& aTorque)
{
    DirectX::SimpleMath::Vector3 groundNormal = m_heli.terrainNormal;
    DirectX::SimpleMath::Vector3 bounceDirection = groundNormal;
    float bounceMag = m_heli.q.velocity.Length();
    DirectX::SimpleMath::Vector3 bounceForce = bounceDirection * bounceMag * m_heli.mass;
    //aForce += bounceForce;

    DirectX::SimpleMath::Vector3 preVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 terrainNorm = m_heli.terrainNormal;
    const float b = 0.8f;
    DirectX::SimpleMath::Vector3 postVelocity = b * (-2.0f * (preVelocity.Dot(terrainNorm)) * terrainNorm + preVelocity);
    m_heli.q.velocity = postVelocity;

    DirectX::SimpleMath::Vector3 velocityNorm = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 upGroundCross = m_heli.up.Cross(terrainNorm);
    upGroundCross.Normalize();
    DirectX::SimpleMath::Vector3 velocityGroundCross = velocityNorm.Cross(terrainNorm);
    velocityGroundCross.Normalize();
    DirectX::SimpleMath::Vector3 testTorque = upGroundCross * (m_heli.q.velocity.Length() * m_heli.mass);
    testTorque = DirectX::SimpleMath::Vector3::Transform(testTorque, m_heli.alignmentInverse);
    aTorque += testTorque;

    int testBreak = 0;
    testBreak++;
}


void Vehicle::UpdateImpulseForces(struct HeliData& aVehicle, const float aTimeDelta)
{
    aVehicle.impulseForceSum = DirectX::SimpleMath::Vector3::Zero;
    aVehicle.impulseTorqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicle.impulseTorqueSum.magnitude = 0.0f;

    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque torqueSum;
    torqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    torqueSum.magnitude = 0.0f;
    for (int i = 0; i < aVehicle.impulseForceVec.size(); ++i)
    {
        //Utility::ImpulseForce testImpulse2 = aVehicle.impulseForceVec[i];
        Utility::UpdateImpulseForceBellCurve(aVehicle.impulseForceVec[i], static_cast<float>(aTimeDelta));
        //Utility::ImpulseForce testImpulse = aVehicle.impulseForceVec[i];

        if (aVehicle.impulseForceVec[i].isActive == true)
        {
            forceSum += (aVehicle.impulseForceVec[i].currentMagnitude * aVehicle.impulseForceVec[i].directionNorm) * 1.0f;

            Utility::Torque torqueImpulse = Utility::GetTorqueForce(aVehicle.impulseForceVec[i].torqueArm, (aVehicle.impulseForceVec[i].currentTorqueMagnitude * aVehicle.impulseForceVec[i].torqueForceNorm));

            torqueSum.axis += torqueImpulse.axis * torqueImpulse.magnitude;
            torqueSum.magnitude += torqueImpulse.magnitude;

            forceSum = DirectX::SimpleMath::Vector3::Transform(forceSum, aVehicle.alignment);

            auto tubePosWorld = m_modelController->GetLocalizedTubeLeftPos();
            if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_LEFT)
            {
                tubePosWorld = m_modelController->GetLocalizedTubeRightPos();
            }
            else if (m_fireControl->GetNextTubeToFire() == MissileTubeSelected::MISSILETUBESELECTED_RIGHT)
            {
                tubePosWorld = m_modelController->GetLocalizedTubeLeftPos();
            }
            tubePosWorld = DirectX::SimpleMath::Vector3::Transform(tubePosWorld, aVehicle.alignment);
            tubePosWorld += aVehicle.q.position;
        }
    }

    torqueSum.axis.Normalize();

    aVehicle.impulseForceSum = forceSum;
    aVehicle.impulseTorqueSum = torqueSum;

    aVehicle.impulseForceVec.erase(
        std::remove_if(
            aVehicle.impulseForceVec.begin(),
            aVehicle.impulseForceVec.end(),
            [](Utility::ImpulseForce const& p) {return p.isActive == false; }
        ),
        aVehicle.impulseForceVec.end()
    );

    m_debugData->ToggleDebugOff();
}