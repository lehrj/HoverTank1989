#include "pch.h"
#include "Vehicle.h"


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

    /*
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.x = ", buoyancyForce.x, "");
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.y = ", buoyancyForce.y, "");
    m_debugData->DebugPushUILineDecimalNumber("buoyancyForce.z = ", buoyancyForce.z, "");
    */

    return buoyancyForce;
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

DirectX::SimpleMath::Vector3 Vehicle::CalculateDragAngular(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    const float length = 8.0f;
    const float width = 4.0f;
    const float height = 2.0f;
    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));

    float yawRadius = (length * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;
   
    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    float angVelocityF = aAngVelocity.Length();
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    float angFrontSurfaceArea = m_heli.area;
    float radius = 4.0f;
    angFrontSurfaceArea = airSurfaceArea;
    radius = radiusSum;

    DirectX::SimpleMath::Vector3 angularDrag = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));

    m_debugData->DebugPushUILineDecimalNumber("angularDrag.L = ", angularDrag.Length(), "");
    return angularDrag;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateDragAngular2(const DirectX::SimpleMath::Vector3 aAngVelocity)
{
    DirectX::SimpleMath::Vector3 angVelocityNorm = aAngVelocity;
    angVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, m_heli.alignmentInverse);

    DirectX::SimpleMath::Vector3 longCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 latCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitZ);
    DirectX::SimpleMath::Vector3 horizCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);

    /*
    m_debugData->DebugPushUILineDecimalNumber("longCross    = ", longCross.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("latCross     = ", latCross.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("horizCross   = ", horizCross.Length(), "");

    longCross = DirectX::SimpleMath::Vector3::Transform(longCross, m_heli.alignment);
    latCross = DirectX::SimpleMath::Vector3::Transform(latCross, m_heli.alignment);
    horizCross = DirectX::SimpleMath::Vector3::Transform(horizCross, m_heli.alignment);

    m_debugData->PushDebugLine(m_heli.q.position, longCross, 10.0f, 0.0, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, latCross, 10.0f, 1.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, horizCross, 10.0f, 0.0, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(m_heli.q.position, aAngVelocity, 10.0f, 0.0, DirectX::Colors::White);
    */

    DirectX::SimpleMath::Vector3 testVec = DirectX::SimpleMath::Vector3(1.0f, 0.2f, 0.0f);
    testVec.Normalize();

    DirectX::SimpleMath::Vector3 testCross = testVec.Cross(DirectX::SimpleMath::Vector3::UnitY);

    //////////////////////////////////////////////
    const float length = 8.0f;
    const float width = 4.0f;
    const float height = 2.0f;
    float frontSurfaceArea = width * height;
    float sideSurfaceArea = length * height;
    float topSurfaceArea = length * width;

    float frontDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float topDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);

    float xDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float yDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float zDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);

    float yawDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float pitchDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float rollDot = localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    //m_debugData->DebugPushUILineDecimalNumber("yawDot         = ", yawDot, "");
    //m_debugData->DebugPushUILineDecimalNumber("pitchDot         = ", pitchDot, "");
    //m_debugData->DebugPushUILineDecimalNumber("rollDot         = ", rollDot, "");

    /*
    float yawSurface = (length * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    float rollSurface = (width * abs(rollDot)) + (height * abs(rollDot));
    float yawRadius = (width * abs(yawDot));
    float pitchRadius = (length * abs(pitchDot));
    float rollRadius = (height * abs(rollDot));
    */

    float yawSurface = (width * abs(yawDot)) + (height * abs(yawDot));
    float pitchSurface = (length * abs(pitchDot)) + (width * abs(pitchDot));
    float rollSurface = (length * abs(rollDot)) + (height * abs(rollDot));

    float yawRadius = (height * abs(yawDot)) * 0.5f;
    float pitchRadius = (length * abs(pitchDot)) * 0.5f;
    float rollRadius = (width * abs(rollDot)) * 0.5f;

    m_debugData->DebugPushUILineDecimalNumber("yawSurface         = ", yawSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("pitchSurface       = ", pitchSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("rollSurface        = ", rollSurface, "");

    m_debugData->DebugPushUILineDecimalNumber("yawRadius         = ", yawRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("pitchRadius       = ", pitchRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("rollRadius        = ", rollRadius, "");

    float frontSurface = abs(frontSurfaceArea * frontDot);
    float sideSurface = abs(sideSurfaceArea * sideDot);
    float topSurface = abs(topSurfaceArea * topDot);

    float frontRadius = abs(length * frontDot);
    float sideRadius = abs(width * sideDot);
    float topRadius = abs(height * topDot);

    float xSurface = abs(frontSurfaceArea * zDot);
    float ySurface = abs(sideSurfaceArea * yDot);
    float zSurface = abs(topSurfaceArea * xDot);

    float xRadius = abs(length * (localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY)));
    float yRadius = abs(width * (localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX)));
    float zRadius = abs(height * (localAngVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ)));

    //float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);
    //float airSurfaceArea = frontSurface + sideSurface + topSurface;
    //float radiusSum = frontRadius + sideRadius + topRadius;

    float airSurfaceArea = yawSurface + pitchSurface + rollSurface;
    float radiusSum = yawRadius + pitchRadius + rollRadius;

    m_debugData->DebugPushUILineDecimalNumber("airSurfaceArea    = ", airSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("radiusSum         = ", radiusSum, "");

    float axisSurfaceArea = xSurface + ySurface + zSurface;
    float axisRadiusSum = xRadius + yRadius + zRadius;
    /*
    m_debugData->DebugPushUILineDecimalNumber("axisSurfaceArea    = ", axisSurfaceArea, "");
    m_debugData->DebugPushUILineDecimalNumber("axisRadiusSum      = ", axisRadiusSum, "");

    m_debugData->DebugPushUILineDecimalNumber("xSurface     = ", xSurface, "");
    m_debugData->DebugPushUILineDecimalNumber("ySurface     = ", ySurface, "");
    m_debugData->DebugPushUILineDecimalNumber("zSurface     = ", zSurface, "");

    m_debugData->DebugPushUILineDecimalNumber("xRadius      = ", xRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("yRadius      = ", yRadius, "");
    m_debugData->DebugPushUILineDecimalNumber("zRadius      = ", zRadius, "");

    m_debugData->DebugPushUILineDecimalNumber("xDot         = ", xDot, "");
    m_debugData->DebugPushUILineDecimalNumber("yDot         = ", yDot, "");
    m_debugData->DebugPushUILineDecimalNumber("zDot         = ", zDot, "");
    */



    //////////////////////////////////////////////////////////////////////////////////
    /*
    horizCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
    horizCross.Normalize();
    if (horizCross.Length() < 0.9999)
    {
        DirectX::SimpleMath::Vector3 testVec(0.001f, 0.999f, -0.0001f);
        testVec.Normalize();
        horizCross = localAngVelocityNorm.Cross(testVec);
        horizCross.Normalize();
        m_debugData->DebugPushUILineDecimalNumber("localAngVelocityNorm = ", localAngVelocityNorm.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("horizCross   = ", horizCross.Length(), "");
        m_debugData->DebugPushUILineDecimalNumber("testVec      = ", testVec.Length(), "");

        m_debugData->PushDebugLine(m_heli.q.position, localAngVelocityNorm, 10.0f, 0.0, DirectX::Colors::Red);
        m_debugData->PushDebugLine(m_heli.q.position, horizCross, 7.0f, 1.0, DirectX::Colors::Blue);
        m_debugData->PushDebugLine(m_heli.q.position, testVec, 5.0f, 0.0, DirectX::Colors::Yellow);

        int testBreak = 0;
    }
    */

    /*

    m_debugData->DebugPushUILineDecimalNumber("localAngVelocityNorm.x", localAngVelocityNorm.x, "");
    m_debugData->DebugPushUILineDecimalNumber("localAngVelocityNorm.y", localAngVelocityNorm.y, "");
    m_debugData->DebugPushUILineDecimalNumber("localAngVelocityNorm.z", localAngVelocityNorm.z, "");

    m_debugData->DebugPushUILineDecimalNumber("horizCross.x", horizCross.x, "");
    m_debugData->DebugPushUILineDecimalNumber("horizCross.y", horizCross.y, "");
    m_debugData->DebugPushUILineDecimalNumber("horizCross.z", horizCross.z, "");

    */

    /*
    m_debugData->PushDebugLine(m_heli.q.position, longCross, 10.0f, 0.0, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, latCross, 10.0f, 1.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, horizCross, 10.0f, 0.0, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(m_heli.q.position, aAngVelocity, 10.0f, 0.0, DirectX::Colors::White);
    */


    DirectX::SimpleMath::Vector3 testRotPath = horizCross * 10.0f;
    if (testRotPath.x > m_inertiaModelX)
    {
        testRotPath.x = m_inertiaModelX;
    }
    else if (testRotPath.x < -m_inertiaModelX)
    {
        testRotPath.x = -m_inertiaModelX;
    }

    if (testRotPath.y > m_inertiaModelY)
    {
        testRotPath.y = m_inertiaModelY;
    }
    else if (testRotPath.y < -m_inertiaModelY)
    {
        testRotPath.y = -m_inertiaModelY;
    }

    if (testRotPath.z > m_inertiaModelZ)
    {
        testRotPath.z = m_inertiaModelZ;
    }
    else if (testRotPath.z < -m_inertiaModelZ)
    {
        testRotPath.z = -m_inertiaModelZ;
    }

    float angVelocityF = aAngVelocity.Length();
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    float angFrontSurfaceArea = m_heli.area;
    float radius = 4.0f;

    radius = testRotPath.Length();


    radius = radiusSum;
    angFrontSurfaceArea = airSurfaceArea;
    //angFrontSurfaceArea = m_heli.area;
    //radius = 4.0f;
    DirectX::SimpleMath::Vector3 angDampeningVecTest2 = angVelocityNorm * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocityF * angVelocityF) * angFrontSurfaceArea * angAirDensity)));
    //DirectX::SimpleMath::Vector3 angDampeningVecTest2 = angVelNormVec * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocity * angVelocity) * angFrontSurfaceArea * angAirDensity)));
    /*
    m_debugData->DebugPushUILineDecimalNumber("testRotPath.x", testRotPath.x, "");
    m_debugData->DebugPushUILineDecimalNumber("testRotPath.y", testRotPath.y, "");
    m_debugData->DebugPushUILineDecimalNumber("testRotPath.z", testRotPath.z, "");
    */
    //m_debugData->DebugPushUILineDecimalNumber("radius       = ", radius, "");
    //m_debugData->DebugPushUILineDecimalNumber("angVelocityF = ", angVelocityF, "");
    m_debugData->DebugPushUILineDecimalNumber("angDampeningVecTest2.L = ", angDampeningVecTest2.Length(), "");
    return angDampeningVecTest2;
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
    float dragResistance      = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;
}

DirectX::SimpleMath::Vector3 Vehicle::CalculateDragLinear2(const DirectX::SimpleMath::Vector3 aVelocity, const DirectX::SimpleMath::Vector3 aNewQVelocity)
{
    DirectX::SimpleMath::Vector3 velocityNorm = aNewQVelocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 localVelocityNorm = aVelocity;
    localVelocityNorm.Normalize();
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, m_heli.alignmentInverse);

    float frontSurfaceArea = m_inertiaModelY * m_inertiaModelZ;
    float sideSurfaceArea = m_inertiaModelX * m_inertiaModelY;
    float topSurfaceArea = m_inertiaModelX * m_inertiaModelZ;

    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    float airSurfaceArea = (frontSurfaceArea * frontDot) + (sideSurfaceArea * sideDot) + (topSurfaceArea * topDot);

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = m_heli.dragCoefficient;
    //float velocity = aVelocity.Length();
    float velocity = aNewQVelocity.Length();
    float dragResistance = 0.5f * airDensity * airSurfaceArea * dragCoefficient * velocity * velocity;

    //velocityNorm = m_heli.q.velocity;
    //velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-dragResistance);

    DirectX::SimpleMath::Vector3 drag = airResistance;
    return drag;
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

void Vehicle::DrawVehicleProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_fireControl->DrawFireControlObjects(aView, aProj);
}

void Vehicle::DrawVehicleProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    m_fireControl->DrawFireControlObjects2(aView, aProj, aEffect, aInputLayout);
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

    // helicopter data
    m_heli.mainRotorRPM = 0.0f;
    m_heli.mainRotorMaxRPM = 1000.0f;
    m_heli.numEqns = 6;
    m_heli.mass = 1700.0f;
 
    m_heli.massTest = 2000.0f;
    m_heli.area = 14.67;
    m_heli.dimensions = DirectX::SimpleMath::Vector3(8.0f, 4.0f, 4.0f);
    m_heli.dimensions = DirectX::SimpleMath::Vector3(4.7f, 2.0f, 2.0f);
    m_heli.airDensity = m_environment->GetAirDensity();
    m_heli.dragCoefficient = 1.05f;

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
    //m_heli.alignment = m_heli.alignmentInverse;

    //m_heli.alignmentInverse = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, -m_heli.forward, -m_heli.up);

    m_heli.cameraOrientation = m_heli.alignment;
    m_heli.cameraOrientationPrevious = m_heli.cameraOrientation;

    m_heli.q.airResistance = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueMagnitude = 0.0f;
    m_heli.q.bodyTorqueVec = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.magnitude = 0.0f;

    m_heli.q.pendulumTorqueForceTest.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.pendulumTorqueForceTest.magnitude = 0.0f;

    m_heli.q.bodyVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.engineForce = DirectX::SimpleMath::Vector3::Zero;

    m_heli.q.mainRotorForceNormal = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.q.mainRotorForceMagnitude = 15.0f;
    m_heli.q.position = DirectX::SimpleMath::Vector3(0.0f, 8.8f, 0.0f);
    //m_heli.q.position = DirectX::SimpleMath::Vector3(0.0f, 20.8f, 0.0f);

    m_heli.boundingBox.Center = m_heli.q.position;
    m_heli.boundingBox.Extents = m_heli.dimensions;
    m_heli.boundingBox.Orientation = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);


    m_heli.q.tailRotorForceNormal = -m_heli.right;
    m_heli.q.tailRotorForceMagnitude = 0.0f;

    m_heli.q.totalVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.parabolicMomentum = DirectX::SimpleMath::Vector3::Zero;

    m_heli.speed = 0.0;

    m_heli.isVehicleAirborne = false;
    m_heli.isVehicleLanding = false;
    m_heli.isVelocityBackwards = false;

    m_heli.terrainHightAtPos = 0.0;
    m_heli.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;


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
    //m_heli.localCenterOfMass = DirectX::SimpleMath::Vector3(0.0, 0.5f, 0.0f);
    m_heli.centerOfMass = m_heli.localCenterOfMass;

    m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3(0.0f, -2.174999997f, 0.00000000f);
    //m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3(0.0f, 2.174999997f, 0.00000000f);
    //m_heli.localMainRotorPos = m_heli.localCenterOfMass;
    //m_heli.localMainRotorPos.y += -2.2f;
    m_heli.mainRotorPos = m_heli.localMainRotorPos;

    m_heli.localGravityTorqueArmPos = m_heli.localCenterOfMass;
    m_heli.localGravityTorqueArmPos.y += 1.0f;
    m_heli.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;

    m_heli.localTopPos = m_heli.localCenterOfMass;
    //m_heli.localTopPos.y -= 2.0f;
    m_heli.localTopPos.y -= 0.2f;
    m_heli.topPos = m_heli.localTopPos;

    m_heli.localTopTestPos = m_heli.localCenterOfMass;
    m_heli.localTopTestPos.y += 1.0f;
    m_heli.topTestPos = m_heli.localTopTestPos;

    m_heli.localBaseTestPos = m_heli.localCenterOfMass;
    m_heli.localBaseTestPos.y -= 1.0f;
    m_heli.baseTestPos = m_heli.localBaseTestPos;


    m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3(-9.25000000, 0.00000000, 0.00000000);
    //m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3(-4.25000000, 0.00000000, 0.00000000);
    //m_heli.localTailRotorPos = m_heli.localCenterOfMass;
    //m_heli.localTailRotorPos.x -= 2.2f;
    m_heli.tailRotorPos = m_heli.localTailRotorPos;

    m_heli.localPhysicsPointFront = m_heli.localCenterOfMass;
    m_heli.localPhysicsPointFront.x += 2.2f;
    m_heli.physicsPointFront = m_heli.localPhysicsPointFront;

    // 1.5
    m_heli.localPhysicsPointLeft = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointLeft.z -= 2.2f;
    m_heli.localPhysicsPointLeft.z -= 1.5f;
    m_heli.physicsPointLeft = m_heli.localPhysicsPointLeft;

    m_heli.localPhysicsPointRear = m_heli.localCenterOfMass;
    m_heli.localPhysicsPointRear.x -= 2.2f;
    m_heli.physicsPointRear = m_heli.localPhysicsPointRear;

    m_heli.localPhysicsPointRight = m_heli.localCenterOfMass;
    //m_heli.localPhysicsPointRight.z += 2.2f;
    m_heli.localPhysicsPointRight.z += 1.5f;
    m_heli.physicsPointRight = m_heli.localPhysicsPointRight;


    m_heli.inertiaMatrixTest = DirectX::SimpleMath::Matrix::Identity;
    float Ixx = 0.0f;
    float Iyy = 0.0f;
    float Izz = 0.0f;
    float Ixy = 0.0f;
    float Ixz = 0.0f;
    float Iyz = 0.0f;

    float xExtent = 2.2f * 2.0f;
    float yExtent = 0.5f * 2.0f;
    float zExtent = 1.5f * 2.0f;

    xExtent = m_inertiaModelX;
    yExtent = m_inertiaModelY;
    zExtent = m_inertiaModelZ;

    float mass = m_testMass;
    // cuboid
    m_heli.inertiaMatrixTest._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrixTest._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    m_heli.inertiaMatrixTest._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    //m_heli.inertiaMatrixTest.Translation(DirectX::SimpleMath::Vector3(0.0, 0.0, 50.0f));

    float radius = 4.0f;
    //sphere
    
    //m_heli.inertiaMatrixTest._11 = (2.0f / 3.0f) * (mass) * (radius * radius);
    //m_heli.inertiaMatrixTest._22 = (2.0f / 3.0f) * (mass) * (radius * radius);
    //m_heli.inertiaMatrixTest._33 = (2.0f / 3.0f) * (mass) * (radius * radius);
    

    //DirectX::SimpleMath::Vector3 testTrans = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 testTrans = DirectX::SimpleMath::Vector3(xExtent, yExtent, zExtent);
    DirectX::SimpleMath::Matrix testTransTensor = m_heli.localInertiaMatrixTest;
    DirectX::SimpleMath::Matrix testOutPutTensor = DirectX::SimpleMath::Matrix::Identity;
    Utility::TransformInertiaTensor(testTrans, testTransTensor, testOutPutTensor);

    testTransTensor = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    m_heli.inverseInertiaMatrixTest = m_heli.inertiaMatrixTest;
    //m_heli.inertiaMatrixTest *= testTransTensor;
    m_heli.inverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest.Invert();

    m_heli.localInertiaMatrixTest = m_heli.inertiaMatrixTest;
    m_heli.localInverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest;

    m_heli.toUseTensor = m_heli.localInertiaMatrixTest;
    m_heli.toUseTensor = m_heli.inverseInertiaMatrixTest;
    testTransTensor = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(90.0f));
    //testTransTensor = DirectX::SimpleMath::Matrix::Identity;
    //Utility::TransformInertiaTensor(testTrans, testTransTensor, m_heli.toUseTensor);
    //m_heli.toUseTensor = m_heli.toUseTensor.Invert();
    //m_heli.toUseTensor = m_heli.inverseInertiaMatrixTest;
    

    mass = 1.0f;
    const float cannonRadius = 1.0f;
    const float cannonHeight = 1.0f;
    m_heli.localCannonTensor._11 = ((1.0f / 12.0f) * (mass) * (cannonHeight * cannonHeight)) + ((1.0f / 4.0f) * mass * (radius * radius));
    m_heli.localCannonTensor._22 = ((1.0f / 12.0f) * (mass) * (cannonHeight * cannonHeight)) + ((1.0f / 4.0f) * mass * (radius * radius));
    m_heli.localCannonTensor._33 = (1.0f / 2.0f) * (mass) * (radius * radius);

    //m_heli.localCannonTensor.Translation(m_heli.cannonPos);

    m_heli.cannonTensor = m_heli.localCannonTensor;
    m_heli.inverseCannonTensor = m_heli.localCannonTensor;
    m_heli.inverseCannonTensor = m_heli.inverseCannonTensor.Invert();


    DirectX::SimpleMath::Matrix aTestMat = DirectX::SimpleMath::Matrix::Identity;
    mass = 1.0f / mass;
    aTestMat._11 = (2.0f / 3.0f) * (mass) * (radius * radius);
    aTestMat._22 = (2.0f / 3.0f) * (mass) * (radius * radius);
    aTestMat._33 = (2.0f / 3.0f) * (mass) * (radius * radius);

    DirectX::SimpleMath::Matrix aTestMatInvert = aTestMat;
    aTestMatInvert.Invert();

    aTestMat = DirectX::SimpleMath::Matrix::Identity;
    aTestMat._11 = 1.0f;
    aTestMat._22 = 2.0f;
    aTestMat._33 = 3.0f;

    aTestMatInvert = aTestMat;
    aTestMatInvert.Invert();
    
    aTestMatInvert = aTestMatInvert.Invert();
    
    DirectX::SimpleMath::Vector3 posShift(0.0f, 0.0f, 0.0f);
    posShift = -m_heli.localCenterOfMass;

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

void Vehicle::InputCyclicRoll(const float aRollInput)
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

void Vehicle::InputGamePadForward(const float aForwardInput)
{
    m_heli.controlInput.cyclicInputPitchIsPressed = true;
    const float updatedForward = aForwardInput * m_heli.controlInput.cyclicInputMax;

    m_heli.controlInput.cyclicInputPitch = updatedForward;
}

void Vehicle::InputGamePadStrafe(const float aStrafeInput)
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

void Vehicle::Jump()
{
    DirectX::SimpleMath::Vector3 jumpVec(50.0f, 0.0f, 0.0f);
    jumpVec = DirectX::SimpleMath::Vector3::Transform(jumpVec, DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(45.0f)));
    const float jumpHeight = 10.0f;
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
    newQ.angularVelocityVec = aQ->angularVelocityVec + static_cast<float>(aQScale) * aDeltaQ->angularVelocityVec;
    newQ.angPosVec = aQ->angPosVec + static_cast<float>(aQScale) * aDeltaQ->angPosVec;

    DirectX::SimpleMath::Vector3 velocityUpdate = m_heli.vehicleLinearForcesSum;
    DirectX::SimpleMath::Vector3 airResistance = CalculateDragLinear(newQ.velocity);
    velocityUpdate += airResistance;

    //  Assign right-hand side values.
    aDQ->airResistance = airResistance;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->totalVelocity = velocityUpdate;
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    
    // angular updates
    DirectX::SimpleMath::Vector3 angVelocityNorm = newQ.angularVelocityVec;
    float angVelocity = angVelocityNorm.Length();
    angVelocityNorm.Normalize();

    DirectX::SimpleMath::Vector3 accelVecUpdate = m_heli.vehicleAngularForcesSum;
    DirectX::SimpleMath::Vector3 angDampTest = CalculateDragAngular(newQ.angularVelocityVec);
    accelVecUpdate += angDampTest;

    aDQ->angularVelocityVec = static_cast<float>(aTimeDelta) * (accelVecUpdate);
    aDQ->angPosVec = static_cast<float>(aTimeDelta) * newQ.angularVelocityVec;
    //aDQ->bodyTorqueForce = angAccelVecTensorUpdate;
}

void Vehicle::RightHandSide2(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    Motion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
    newQ.angularVelocityVec = aQ->angularVelocityVec + static_cast<float>(aQScale) * aDeltaQ->angularVelocityVec;
    newQ.angPosVec = aQ->angPosVec + static_cast<float>(aQScale) * aDeltaQ->angPosVec;

    DirectX::SimpleMath::Vector3 rotorForce = aQ->mainRotorForceNormal * aQ->mainRotorForceMagnitude * m_heli.controlInput.collectiveInput;
    rotorForce = UpdateRotorForceRunge();
    rotorForce *= (m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond);

    // testing new controls
    rotorForce = m_heli.hoverFloat;
    rotorForce = DirectX::SimpleMath::Vector3::Transform(rotorForce, m_heli.alignment);
    rotorForce = rotorForce * m_heli.mass;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = aHeli->dragCoefficient;
    float frontSurfaceArea = aHeli->area;
    float velocity = newQ.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = m_heli.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 velocityNorm2 = newQ.velocity;
    velocityNorm2.Normalize();
    DirectX::SimpleMath::Vector3 airResistance2 = velocityNorm2 * (-frontDragResistance);




    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    if (m_testImpulseForce.isActive == true)
    {
        velocityUpdate += m_testImpulseForce.directionNorm * m_testImpulseForce.currentMagnitude;
    }

    velocityUpdate += CalculateHoverDriveForce(m_heli);
    //velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 damperForce = GetDamperForce(GetAltitude(), aHeli->mass);
    //velocityUpdate += damperForce;
    DirectX::SimpleMath::Vector3 gravForce = GetAntiGravGravityForce(GetAltitude(), aHeli->gravity, aHeli->mass);
    gravForce = aHeli->gravity * aHeli->mass;
    velocityUpdate += gravForce;

    velocityUpdate += GetJetThrust(aHeli->forward, aHeli->controlInput.jetInput, aHeli->jetThrustMax);
    rotorForce = GetHoverLift(rotorForce, GetAltitude());
    velocityUpdate += rotorForce;

    velocityUpdate += m_heli.buoyancyForce;

    velocityUpdate += GetSlopeForce(aHeli->terrainNormal, GetAltitude(), aHeli->groundNormalForceRange);
    velocityUpdate += airResistance;
    velocityUpdate += aHeli->controlInput.brakeForce;

    velocityUpdate = m_heli.vehicleLinearForcesSum;



    airResistance = CalculateDragLinear(newQ.velocity);
    velocityUpdate += airResistance;


    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    DirectX::SimpleMath::Vector3 tempVel = DirectX::SimpleMath::Vector3::Zero;
    //UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));
    //UpdatePendulumMotion(pendTorque, tempVel, static_cast<float>(aTimeDelta));

    DirectX::SimpleMath::Vector3 accelVecUpdate = DirectX::SimpleMath::Vector3::Zero;
    //Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueRunge(accelVecUpdate, pendTorque, static_cast<float>(aTimeDelta));
    accelVecUpdate = m_heli.vehicleAngularForcesSum;
    Utility::Torque bodyTorqueUpdate = m_bodyTorqueTest;
    bodyTorqueUpdate.axis = DirectX::SimpleMath::Vector3::UnitY;
    bodyTorqueUpdate.magnitude = 0.0f;
    /*
    if (bodyTorqueUpdate.axis.Length() < 0.9f)
    {
        bodyTorqueUpdate.axis = DirectX::SimpleMath::Vector3::UnitY;
        bodyTorqueUpdate.magnitude = 0.0f;
        m_debugData->DebugPushUILineDecimalNumber("body torque error ", 0.0f, "");
    }
    */

    //  Assign right-hand side values.
    aDQ->airResistance = airResistance;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->totalVelocity = velocityUpdate;
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    aDQ->bodyTorqueForce = bodyTorqueUpdate;


    DirectX::SimpleMath::Vector3 angVelocityNorm = newQ.angularVelocityVec;
    float angVelocity = angVelocityNorm.Length();
    angVelocityNorm.Normalize();

    /////////////////////////////////////////////////////////

    DirectX::SimpleMath::Matrix inverseAlignment2 = m_heli.alignment;
    inverseAlignment2 = inverseAlignment2.Invert();

    DirectX::SimpleMath::Vector3 localAngVelocityNorm = angVelocityNorm;
    localAngVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localAngVelocityNorm, inverseAlignment2);

    DirectX::SimpleMath::Vector3 localVelocityNorm = velocityNorm;
    localVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localVelocityNorm, inverseAlignment2);
    /*
    DirectX::SimpleMath::Vector3 longCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 latCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitZ);
    DirectX::SimpleMath::Vector3 horizCross = localAngVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
    */
    DirectX::SimpleMath::Vector3 longCross = localVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 latCross = localVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitZ);
    DirectX::SimpleMath::Vector3 horizCross = localVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);

    /*
    DirectX::SimpleMath::Vector3 longCross = localAngVelocityNorm.Cross(m_heli.forward);
    DirectX::SimpleMath::Vector3 latCross = localAngVelocityNorm.Cross(m_heli.right);
    DirectX::SimpleMath::Vector3 horizCross = localAngVelocityNorm.Cross(m_heli.up);
    */
    //m_debugData->PushDebugLine(m_heli.q.position, longCross, 10.0f, 0.0, DirectX::Colors::Red);
    //m_debugData->PushDebugLine(m_heli.q.position, latCross, 10.0f, 0.0, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(m_heli.q.position, horizCross, 10.0f, 0.0, DirectX::Colors::Yellow);
    //m_debugData->PushDebugLine(m_heli.q.position, newQ.angularVelocityVec, 10.0f, 0.0, DirectX::Colors::White);

    DirectX::SimpleMath::Vector3 testRotPath = horizCross * 10.0f;
    if (testRotPath.x > m_inertiaModelX)
    {
        testRotPath.x = m_inertiaModelX;
    }
    else if (testRotPath.x < -m_inertiaModelX)
    {
        testRotPath.x = -m_inertiaModelX;
    }

    if (testRotPath.y > m_inertiaModelY)
    {
        testRotPath.y = m_inertiaModelY;
    }
    else if (testRotPath.y < -m_inertiaModelY)
    {
        testRotPath.y = -m_inertiaModelY;
    }

    if (testRotPath.z > m_inertiaModelZ)
    {
        testRotPath.z = m_inertiaModelZ;
    }
    else if (testRotPath.z < -m_inertiaModelZ)
    {
        testRotPath.z = -m_inertiaModelZ;
    }
    //m_debugData->DebugPushUILineDecimalNumber("testRotPath  = ", testRotPath.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, testRotPath, 10.0f, 0.0, DirectX::Colors::Orange);

    float frontSurfaceArea2 = m_inertiaModelY * m_inertiaModelZ;
    float sideSurfaceArea = m_inertiaModelX * m_inertiaModelY;
    float topSurfaceArea = m_inertiaModelX * m_inertiaModelZ;
    /*
    float frontDot = horizCross.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = horizCross.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    //float topDot = horizCross.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //float topDot = latCross.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float topDot = longCross.Dot(DirectX::SimpleMath::Vector3::UnitY);
    */
    float frontDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitX);
    float sideDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitZ);
    //float topDot = horizCross.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //float topDot = latCross.Dot(DirectX::SimpleMath::Vector3::UnitY);
    float topDot = localVelocityNorm.Dot(DirectX::SimpleMath::Vector3::UnitY);

    //m_debugData->DebugPushUILineDecimalNumber("frontDot  = ", frontDot, "");
    //m_debugData->DebugPushUILineDecimalNumber("sideDot   = ", sideDot, "");
    //m_debugData->DebugPushUILineDecimalNumber("topDot    = ", topDot, "");
    /////////////////////////////////////////////////////////

    //float angDragCoefficient = 1.3f;
    float angDragCoefficient = 0.8f;
    float angAirDensity = m_environment->GetAirDensity();
    float angFrontSurfaceArea = aHeli->area;

    float angFrontDragResistance = 0.5f * angAirDensity * angFrontSurfaceArea * angDragCoefficient * angVelocity * angVelocity;
    float radius = 4.0f;
    radius = testRotPath.Length();
    float testDragVal = angAirDensity * angFrontSurfaceArea * angDragCoefficient;
    float testDragVal2 = (angDragCoefficient * (radius * radius * radius)) * (angFrontSurfaceArea * angAirDensity);

    const float cyclicInputSum = abs(m_heli.controlInput.cyclicInputPitch) + abs(m_heli.controlInput.cyclicInputRoll);
    const float cyclicInputMax = m_heli.controlInput.cyclicInputMax * 2.0f;
    const float inputRatio = cyclicInputSum / cyclicInputMax;
    const float inverseRatio = 1.0f - inputRatio;
    //testDragVal *= 1.0f + inverseRatio;
    //m_debugData->DebugPushUILineDecimalNumber("testDragVal     = ", testDragVal, "");
    //m_debugData->DebugPushUILineDecimalNumber("testDragVal2    = ", testDragVal2, "");
    angFrontDragResistance = 0.5f * testDragVal * angVelocity * angVelocity;
    DirectX::SimpleMath::Vector3 angVelNormVec = m_heli.q.angularVelocityVec;
    angVelNormVec.Normalize();
    DirectX::SimpleMath::Vector3 angDampeningVec = angVelNormVec * (-angFrontDragResistance);

    DirectX::SimpleMath::Vector3 angDampeningVecTest1 = angVelNormVec * (-(0.5f * angAirDensity * angFrontSurfaceArea * angDragCoefficient * angVelocity * angVelocity));
    DirectX::SimpleMath::Vector3 angDampeningVecTest2 = angVelNormVec * (-((0.5f) * (angDragCoefficient * (radius * radius * radius)) * ((angVelocity * angVelocity) * angFrontSurfaceArea * angAirDensity)));
    //m_debugData->DebugPushUILineDecimalNumber("angDampeningVecTest1  = ", angDampeningVecTest1.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("angDampeningVecTest2  = ", angDampeningVecTest2.Length(), "");
    DirectX::SimpleMath::Vector3 angAccelVecTensorUpdate = accelVecUpdate;
    //m_debugData->DebugPushUILineDecimalNumber("angAccelVecTensorUpdate  = ", angAccelVecTensorUpdate.Length(), "");
    DirectX::SimpleMath::Matrix inverseAlignment = m_heli.alignment;
    inverseAlignment = inverseAlignment.Invert();

    DirectX::SimpleMath::Vector3 testAngDampingVec = angDampeningVec;
    /*
    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, inverseAlignment);
    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, m_heli.toUseTensor);
    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, m_heli.alignment);
    */

    testAngDampingVec = DirectX::SimpleMath::Vector3::Transform(testAngDampingVec, inverseAlignment);
    testAngDampingVec = DirectX::SimpleMath::Vector3::Transform(testAngDampingVec, m_heli.toUseTensor);
    testAngDampingVec = DirectX::SimpleMath::Vector3::Transform(testAngDampingVec, m_heli.alignment);

    /*
    DirectX::SimpleMath::Vector3 preAngVec = angAccelVecTensorUpdate;
    m_debugData->PushDebugLine(m_heli.q.position, preAngVec, 10.0f, 0.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLine(m_heli.q.position, angDampeningVec, 10.0f, 0.0, DirectX::Colors::Red);
    m_debugData->DebugPushUILineDecimalNumber("preAngVec.Length()       = ", preAngVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("angDampeningVec.Length() = ", angDampeningVec.Length(), "");
    */

    //angAccelVecTensorUpdate += angDampeningVec;
    //angAccelVecTensorUpdate += testAngDampingVec;
    //angAccelVecTensorUpdate += angDampeningVecTest2;

    //DirectX::SimpleMath::Vector3 angDampTest = CalculateDragAngular(m_heli.q.angularVelocityVec);
    DirectX::SimpleMath::Vector3 angDampTest = CalculateDragAngular(newQ.angularVelocityVec);

    angAccelVecTensorUpdate += angDampTest;

    aDQ->angularVelocityVec = static_cast<float>(aTimeDelta) * (angAccelVecTensorUpdate);
    aDQ->angPosVec = static_cast<float>(aTimeDelta) * newQ.angularVelocityVec;
}

//  This method solves for the vehicle motion using a 4th-order Runge-Kutta solver
void Vehicle::RungeKutta4(struct HeliData* aHeli, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(aHeli->numEqns);
    //  Retrieve the current values of the dependent and independent variables.
    Motion q = aHeli->q;
    Motion dq1;
    Motion dq2;
    Motion dq3;
    Motion dq4;

    Motion q2 = aHeli->q;
    Motion dq12;
    Motion dq22;
    Motion dq32;
    Motion dq42;
    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aHeli, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aHeli, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aHeli, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aHeli, &q, &dq3, aTimeDelta, 1.0, &dq4);

    RightHandSide2(aHeli, &q2, &q2, aTimeDelta, 0.0, &dq12);
    RightHandSide2(aHeli, &q2, &dq12, aTimeDelta, 0.5, &dq22);
    RightHandSide2(aHeli, &q2, &dq22, aTimeDelta, 0.5, &dq32);
    RightHandSide2(aHeli, &q2, &dq32, aTimeDelta, 1.0, &dq42);

    aHeli->time = aHeli->time + aTimeDelta;
    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 bodyVelocityyUpdate = (dq1.bodyVelocity + 2.0 * dq2.bodyVelocity + 2.0 * dq3.bodyVelocity + dq4.bodyVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 engineVelocityUpdate = (dq1.engineForce + 2.0 * dq2.engineForce + 2.0 * dq3.engineForce + dq4.engineForce) / numEqns;
    DirectX::SimpleMath::Vector3 airResistnaceVelocityUpdate = (dq1.airResistance + 2.0 * dq2.airResistance + 2.0 * dq3.airResistance + dq4.airResistance) / numEqns;
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;




    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;

    DirectX::SimpleMath::Vector3 angularPosVecUpdate = (dq1.angPosVec + 2.0 * dq2.angPosVec + 2.0 * dq3.angPosVec + dq4.angPosVec) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityVecUpdate = (dq1.angularVelocityVec + 2.0 * dq2.angularVelocityVec + 2.0 * dq3.angularVelocityVec + dq4.angularVelocityVec) / numEqns;


    DirectX::SimpleMath::Vector3 posUpdate2 = (dq12.position + 2.0 * dq22.position + 2.0 * dq32.position + dq42.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate2 = (dq12.velocity + 2.0 * dq22.velocity + 2.0 * dq32.velocity + dq42.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularPosVecUpdate2 = (dq12.angPosVec + 2.0 * dq22.angPosVec + 2.0 * dq32.angPosVec + dq42.angPosVec) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityVecUpdate2 = (dq12.angularVelocityVec + 2.0 * dq22.angularVelocityVec + 2.0 * dq32.angularVelocityVec + dq42.angularVelocityVec) / numEqns;

    if (posUpdate != posUpdate2)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (velocityUpdate != velocityUpdate2)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (angularPosVecUpdate != angularPosVecUpdate2)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (angularVelocityVecUpdate != angularVelocityVecUpdate2)
    {
        int testBreak = 0;
        testBreak++;
    }



    q.velocity += velocityUpdate;
    q.engineForce += engineVelocityUpdate;
    q.airResistance += airResistnaceVelocityUpdate;
    q.totalVelocity += velocityUpdate;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;
    q.position += posUpdate;
    q.angPosVec += angularPosVecUpdate;
    q.angularVelocityVec += angularVelocityVecUpdate;

    aHeli->q.velocity = q.velocity;
    aHeli->q.position = q.position;
    aHeli->q.engineForce = q.engineForce;
    aHeli->q.bodyTorqueForce = q.bodyTorqueForce;
    aHeli->q.angularVelocityVec = q.angularVelocityVec;
    aHeli->q.angPosVec = q.angPosVec;
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

void Vehicle::SetModelController(std::shared_ptr<ModelController> aModelController)
{
    m_modelController = aModelController;
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

void Vehicle::UpdateAlignmentTorque()
{
    m_heli.cameraOrientationPrevious = m_heli.cameraOrientation;

    if (m_heli.isVehicleAirborne == false && 1 == 0)
    {
        DirectX::SimpleMath::Vector3 newUp = m_heli.terrainNormal;
        DirectX::SimpleMath::Vector3 oldUp = m_heli.up;
        DirectX::SimpleMath::Vector3 updateUp = DirectX::SimpleMath::Vector3::SmoothStep(oldUp, newUp, 0.2);
        updateUp.Normalize();
        m_heli.up = updateUp;
        m_heli.right = m_heli.forward.Cross(m_heli.up);
        m_heli.right.Normalize();
        m_heli.forward = m_heli.up.Cross(m_heli.right);
        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }
    else if (1 == 0)
    {
        DirectX::SimpleMath::Matrix preAlignment = m_heli.alignment;
        DirectX::SimpleMath::Quaternion preAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
        DirectX::SimpleMath::Matrix torqueMat;
        const float tol = 0.0001f;
        if (abs(m_heli.q.bodyTorqueForce.magnitude) < tol || m_heli.q.bodyTorqueForce.axis.Length() < tol)
        {
            //torqueMat = DirectX::SimpleMath::Matrix::Identity;
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, 0.0f);
        }
        else
        {
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.q.bodyTorqueForce.axis, m_heli.q.bodyTorqueForce.magnitude);
        }

        const float t = 0.5f;
        DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(torqueMat);
        DirectX::SimpleMath::Matrix postAlignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, rotQuat);

        DirectX::SimpleMath::Quaternion postAlignmentQuat = preAlignmentQuat * rotQuat;
        postAlignmentQuat.Normalize();
        DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Slerp(preAlignmentQuat, postAlignmentQuat, t);
        //DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Slerp(preAlignmentQuat, rotQuat, t);
        m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, rotQuat);
        m_heli.alignment = DirectX::SimpleMath::Matrix::Lerp(preAlignment, postAlignment, t);
        //m_heli.alignment = postAlignment;

        //m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(updateQuat);
        //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, updateQuat);

        DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
        testMat *= torqueMat;
        //m_heli.alignment *= testMat;
        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }
    else
    {
        //DirectX::SimpleMath::Matrix testRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularVelocity);
        //DirectX::SimpleMath::Matrix testRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularPositionQuat);

        if (m_debugToggle2 == true)
        {
            //m_heli.alignment *= m_heli.q.angularVelocityMat;
            /////m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocityQuat);
        }
        else
        {
            DirectX::SimpleMath::Vector3 testAngVec = m_heli.q.angularVelocityVec;

            if (testAngVec != DirectX::SimpleMath::Vector3::Zero)
            {
                DirectX::SimpleMath::Quaternion testQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_heli.q.angularVelocityVec, m_heli.q.angularVelocityVec.Length());
                /////m_heli.alignment *= m_heli.q.angularVelocityMat;
                //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocityQuat);
                m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, testQuat);
            }
        }

        //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocityQuat);
        //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularPositionQuat);
        
        //m_heli.alignment *= m_heli.q.angularVelocityMat;
        //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocityMat);

        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
        //m_heli.alignmentInverse = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, m_heli.right, -m_heli.up);
        m_heli.alignmentInverse = m_heli.alignment.Invert();
        DirectX::SimpleMath::Matrix aTestAlign = m_heli.alignment;
        DirectX::SimpleMath::Matrix aTestAlignInvert = aTestAlign;
        aTestAlignInvert = aTestAlignInvert.Invert();
        DirectX::SimpleMath::Matrix aTestAlignInvert2 = m_heli.alignmentInverse;
    }

    m_heli.inertiaMatrixTest = m_heli.localInertiaMatrixTest;
    m_heli.inertiaMatrixTest *= m_heli.alignment;
    m_heli.inverseInertiaMatrixTest = m_heli.localInverseInertiaMatrixTest;
    m_heli.inverseInertiaMatrixTest *= m_heli.alignment;

    m_heli.inertiaMatrixTest = m_heli.localInertiaMatrixTest;
    m_heli.inertiaMatrixTest *= m_heli.alignment;
    m_heli.inverseInertiaMatrixTest = m_heli.inertiaMatrixTest;
    m_heli.inverseInertiaMatrixTest.Invert();
    m_heli.inverseInertiaMatrixTest = m_heli.localInverseInertiaMatrixTest;

    //m_heli.inverseInertiaMatrixTest = DirectX::SimpleMath::Matrix::Identity;
    m_heli.inverseInertiaMatrixTest = m_heli.localInertiaMatrixTest;
    //m_heli.inverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest.Invert();
    //m_heli.inverseInertiaMatrixTest = m_heli.localInverseInertiaMatrixTest;
    m_heli.inverseInertiaMatrixTest = m_heli.inverseInertiaMatrixTest.Invert();
    m_heli.inverseInertiaMatrixTest = DirectX::SimpleMath::Matrix::Transform(m_heli.inverseInertiaMatrixTest, DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix( m_heli.alignment));
    
    //m_heli.inverseInertiaMatrixTest.Invert();
    


    m_heli.cameraOrientation = DirectX::XMMatrixLookAtRH(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    m_heli.cameraOrientation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
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

Utility::Torque Vehicle::UpdateBodyTorqueRunge(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep)
{
    const float timeStepMod = aTimeStep;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.localCenterOfMass;
    centerMassPos = DirectX::SimpleMath::Vector3::Transform(centerMassPos, m_heli.alignment);
    centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 rotorPos = m_heli.localMainRotorPos;
    rotorPos = DirectX::SimpleMath::Vector3::Transform(rotorPos, m_heli.alignment);
    rotorPos = m_heli.mainRotorPos;
    DirectX::SimpleMath::Vector3 tailPos = m_heli.localTailRotorPos;
    tailPos = DirectX::SimpleMath::Vector3::Transform(tailPos, m_heli.alignment);
    tailPos = m_heli.tailRotorPos;

    DirectX::SimpleMath::Vector3 mainRotorTorqueArm = rotorPos - centerMassPos;
    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = tailPos - centerMassPos;
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = centerMassPos - m_heli.topPos;
    //gravityTorqueArm = m_heli.gravityTorqueArmPos - centerMassPos;
    gravityTorqueArm = centerMassPos - m_heli.gravityTorqueArmPos;

    const float cyclicInputSum = abs(m_heli.controlInput.cyclicInputPitch) + abs(m_heli.controlInput.cyclicInputRoll);
    const float cyclicInputMax = m_heli.controlInput.cyclicInputMax * 2.0f;
    const float inputRatio = cyclicInputSum / cyclicInputMax;
    const float inverseRatio = 1.0f - inputRatio;
    const float torqueArmLengthMod = 5.0f;
    //gravityTorqueArm *= (inverseRatio * torqueArmLengthMod);

    //m_debugData->PushDebugLine(m_heli.q.position, gravityTorqueArm, 10.0f, 0.0f, DirectX::Colors::Red);

    /*
    m_debugData->PushDebugLinePositionIndicator(rotorPos, 10.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(m_heli.topPos, 10.0f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLinePositionIndicator(centerMassPos, 10.0f, 0.0f, DirectX::Colors::Yellow);
    */

    //const float modVal = 0.01f;
    const float modVal = aTimeStep;
    DirectX::SimpleMath::Vector3 mainRotorForce = m_heli.q.mainRotorForceNormal * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = UpdateRotorForceRunge();
    mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;

    //const float windVaning = CalculateWindVaningVal(m_heli);
    //DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + windVaning) * modVal;
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * modVal * m_testForceMod2;
    //DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * m_heli.yawForce;
 

    DirectX::SimpleMath::Vector3 gravityForce = (m_heli.gravity) * modVal;
    gravityForce = m_heli.gravity;
    gravityForce.Normalize();
    gravityForce = gravityForce * (4.8 * modVal);
    gravityForce = m_heli.gravity * (modVal) * m_testMass;
    //gravityForce = m_heli.gravity * (modVal) * (0.01f) * m_testMass;
    //gravityForce = m_heli.gravity * (modVal);
    gravityForce = m_heli.gravity * (0.0001f) * m_testMass;
    gravityForce = m_heli.gravity * (0.01f) * m_testMass;
    gravityForce = m_heli.gravity * 5.0f;
    
    //gravityForce = m_heli.gravity * (0.01f) * m_testForceMod1;
    
    //m_debugData->PushDebugLine(m_heli.q.position, gravityForce, 10.0f, 0.0f, DirectX::Colors::Blue);

    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    //m_debugData->PushDebugLine(m_heli.q.position, gravTorque.axis, 10.0f, 0.0f, DirectX::Colors::Yellow);

    DirectX::SimpleMath::Vector3 gravityTorqueArm2 = rotorPos - centerMassPos;
    gravityTorqueArm2 = -DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 gravityForce2 = m_heli.gravity * (modVal);
    gravityForce2 = m_heli.up * 0.3f;
    Utility::Torque gravTorque2 = Utility::GetTorqueForce(gravityTorqueArm2, gravityForce2);

    if (tailTorque.axis.Length() < 0.5f)
    {
        //tailTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
        //tailTorque.magnitude = 0.0f;
    }

    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testTerrainNormTorque.axis * m_testTerrainNormTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (aPendTorque.axis * aPendTorque.magnitude);

    torqueAxis = (tailTorque.axis * tailTorque.magnitude) + (aPendTorque.axis * aPendTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);

    torqueAxis = (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);

    torqueAxis.Normalize();

    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testTerrainNormTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testDrivetrainTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + aPendTorque.magnitude;

    //torqueMag = tailTorque.magnitude + aPendTorque.magnitude + m_testDrivetrainTorque.magnitude;

    torqueMag = tailTorque.magnitude + gravTorque.magnitude + m_testDrivetrainTorque.magnitude;

    Utility::Torque updatedTorque2;
    updatedTorque2.axis = (tailRotorTorqueArm.Cross(tailForce)) + (gravityTorqueArm.Cross(gravityForce)) + (m_testLatTorqueArm.Cross(m_testLatTorqueForce)) + (m_testLongTorqueArm.Cross(m_testLongTorqueForce));
    updatedTorque2.magnitude = (tailRotorTorqueArm.Cross(tailForce)).Length() + (gravityTorqueArm.Cross(gravityForce)).Length() + (m_testLatTorqueArm.Cross(m_testLatTorqueForce)).Length() + (m_testLongTorqueArm.Cross(m_testLongTorqueForce)).Length();
    updatedTorque2.axis.Normalize();

    Utility::Torque weaponTorque2;
    weaponTorque2.axis = DirectX::SimpleMath::Vector3::UnitY;
    weaponTorque2.magnitude = 0.0f;
    if (m_testImpulseForce.isActive == true)
    {
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - centerMassPos;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        float weaponForceMag = 0.2f * m_testForceMod2;
        //weaponForceMag = m_testImpulseForce.currentMagnitude * 0.000001f;
        weaponForceMag = m_testImpulseForce.currentMagnitude * 0.005f;
        weaponForce *= weaponForceMag;
        Utility::Torque weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);
        weaponTorque2 = weaponTorque;
        torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude)
            + (weaponTorque.axis * weaponTorque.magnitude);
        torqueAxis.Normalize();
        torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + weaponTorque.magnitude;
    }
    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.magnitude = torqueMag;

    if (updatedTorque.axis.Length() < 0.9f)
    {
        updatedTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
        updatedTorque.magnitude = 0.0f;
    }
    if (updatedTorque2.axis.Length() < 0.9f)
    {
        updatedTorque2.axis = DirectX::SimpleMath::Vector3::UnitY;
        updatedTorque2.magnitude = 0.0f;
    }

    float tailLength = tailTorque.axis.Length();
    float gravLength = gravTorque.axis.Length();
    float driveLength = m_testDrivetrainTorque.axis.Length();

    DirectX::SimpleMath::Vector3 tailVec = tailTorque.axis * tailTorque.magnitude;
    DirectX::SimpleMath::Vector3 gravVec = gravTorque.axis * gravTorque.magnitude;
    DirectX::SimpleMath::Vector3 driveVec = m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude;
    //driveVec *= modVal;
    DirectX::SimpleMath::Vector3 weaponVec = weaponTorque2.axis * weaponTorque2.magnitude;

    //const float gravVal = 2.45f;
    const float gravVal = 9.8f;
    gravityForce = (DirectX::SimpleMath::Vector3::UnitY * gravVal) * modVal * m_testMass;
    DirectX::SimpleMath::Vector3 gravVectTest = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 terrainVec = m_testTerrainNormTorque.axis;
    terrainVec.Normalize();
    terrainVec *= m_testTerrainNormTorque.magnitude;



    //DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;
    // static DirectX::SimpleMath::Vector3 ProcessTorqueForce(const DirectX::SimpleMath::Vector3 aArmPos, const DirectX::SimpleMath::Vector3 aFulcrumPos, const DirectX::SimpleMath::Vector3 aForce)
    //gravVectTest += Utility::ProcessTorqueForce(rotorPos, centerMassPos, gravityForce);
    centerMassPos = m_heli.topPos;
    gravVectTest += Utility::ProcessTorqueForce(m_heli.physicsPointRear, centerMassPos, gravityForce);
    gravVectTest += Utility::ProcessTorqueForce(m_heli.physicsPointLeft, centerMassPos, gravityForce);
    gravVectTest += Utility::ProcessTorqueForce(m_heli.physicsPointRight, centerMassPos, gravityForce);
    gravVectTest += Utility::ProcessTorqueForce(m_heli.physicsPointFront, centerMassPos, gravityForce);

    //m_debugData->DebugPushUILineDecimalNumber("gravVec.Length()      = ", gravVec.Length(), "");
    //m_debugData->DebugPushUILineDecimalNumber("gravVectTest.Length() = ", gravVectTest.Length(), "");



    /*
    DirectX::SimpleMath::Matrix inverseAlignment = m_heli.alignment;
    inverseAlignment = inverseAlignment.Invert();
    
    DirectX::SimpleMath::Vector3 tailVecLine = tailVec;
    tailVecLine = DirectX::SimpleMath::Vector3::Transform(tailVecLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, tailVecLine, 10.0f, 0.0f, DirectX::Colors::Yellow);

    DirectX::SimpleMath::Vector3 gravVecLine = gravVec;
    gravVecLine = DirectX::SimpleMath::Vector3::Transform(gravVecLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, gravVecLine, 10.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 driveVecLine = driveVec;
    driveVecLine = DirectX::SimpleMath::Vector3::Transform(driveVecLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, driveVecLine, 10.0f, 1.0f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 weaponVecLine = weaponVec;
    weaponVecLine = DirectX::SimpleMath::Vector3::Transform(weaponVecLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, weaponVecLine, 10.0f, 0.0f, DirectX::Colors::White);
    */

    //DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec;
    //DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec + terrainVec;
    //DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + weaponVec + terrainVec;
    // //DirectX::SimpleMath::Vector3 torqueVec = tailVec + weaponVec + hoverForceVec;
    //DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec;

    DirectX::SimpleMath::Vector3 windVanVec = m_heli.windVaningTorueForce;
    DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec + terrainVec + windVanVec;
    
    /*
    DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec + terrainVec + pendulumForce;
    DirectX::SimpleMath::Vector3 hoverForceVec = m_hoverTorqueForceSum;
    DirectX::SimpleMath::Vector3 pendulumForce = aPendTorque.axis * aPendTorque.magnitude * 100.0f;
    DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVec + driveVec + weaponVec + terrainVec + pendulumForce;
    m_debugData->PushDebugLine(m_heli.q.position, pendulumForce, 10.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, gravVec, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->DebugPushUILineDecimalNumber("pendulumForce.Length() = ", pendulumForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("terrainVec.Length() = ", terrainVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("gravVec.Length() = ", gravVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("driveVec.Length() = ", driveVec.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("gravVec.Length() - driveVec.Length() = ", gravVec.Length() - driveVec.Length(), "");
    */

    //DirectX::SimpleMath::Vector3 torqueVec = hoverForceVec;
    //DirectX::SimpleMath::Vector3 torqueVec = tailVec + gravVectTest + driveVec + weaponVec;
    float torqueVecLength = torqueVec.Length();
    DirectX::SimpleMath::Vector3 torqueVecNorm = torqueVec;
    torqueVecNorm.Normalize();

    Utility::Torque testTorque;
    testTorque.axis = torqueVecNorm;
    testTorque.axis.Normalize();
    testTorque.magnitude = torqueVecLength;
    
    aAccelVec = torqueVec;

    updatedTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
    updatedTorque.magnitude = 0.0f;
    return updatedTorque;
}

void Vehicle::UpdateBrakeForce(const float aTimeStep)
{
    if (m_heli.controlInput.brakeIsPressed == true)
    {
        m_heli.controlInput.brakeInput += m_heli.controlInput.breakInputRate * aTimeStep;
        if (m_heli.controlInput.brakeInput > m_heli.controlInput.breakInputMax)
        {
            m_heli.controlInput.brakeInput = m_heli.controlInput.breakInputMax;
        }
    }
    else if (m_heli.controlInput.brakeIsPressed == false)
    {
        m_heli.controlInput.brakeInput -= m_heli.controlInput.breakInputRate * aTimeStep;
        if (m_heli.controlInput.brakeInput < m_heli.controlInput.breakInputMin)
        {
            m_heli.controlInput.brakeInput = m_heli.controlInput.breakInputMin;
        }
    }
    m_heli.controlInput.brakeIsPressed = false;

    if (m_heli.controlInput.brakeInput > m_heli.controlInput.breakInputMin)
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

    m_debugData->DebugPushUILineDecimalNumber("windVaning = ", windVaning, "");
    const float dragVal = airDensity * surfaceArea * dragCoefficient;
    m_debugData->DebugPushUILineDecimalNumber("dragVal = ", dragVal, "");

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

    m_debugData->DebugPushUILineDecimalNumber("isVelocityForward = ", static_cast<float>(isVelocityForward), "");
    m_debugData->DebugPushUILineDecimalNumber("isVelocityLeft = ", static_cast<float>(isVelocityLeft), "");



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

    m_debugData->DebugPushUILineDecimalNumber("latDotMod = ", latDotMod, "");
    m_debugData->DebugPushUILineDecimalNumber("longDotMod = ", longDotMod, "");


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
    m_debugData->PushDebugLine(m_heli.q.position, localizedAirVelocityLateral, 7.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, localizedAirVelocityLongitudinal, 7.0f, 0.0f, DirectX::Colors::Orchid);
    m_debugData->DebugPushUILineDecimalNumber("localizedAirVelocityLateral.Length() = ", localizedAirVelocityLateral.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("localizedAirVelocityLongitudinal.Length() = ", localizedAirVelocityLongitudinal.Length(), "");

    m_debugData->PushDebugLine(m_heli.q.position, testLatTorque, 10.0f, 0.0f, DirectX::Colors::Aqua);
    m_debugData->PushDebugLine(m_heli.q.position, testLongTorque, 10.0f, 0.0f, DirectX::Colors::Orange);
    m_debugData->DebugPushUILineDecimalNumber("testLatTorque.Length() = ", testLatTorque.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("testLongTorque.Length() = ", testLongTorque.Length(), "");
   
    DirectX::SimpleMath::Vector3 windVaningUpdateForce = DirectX::SimpleMath::Vector3::Zero;
    //windVaningUpdateForce.x = 5.0f;
    //m_debugData->DebugPushUILineDecimalNumber("windVaningUpdateForce = ", windVaningUpdateForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 10.0f, 0.0f, DirectX::Colors::Red);

    windVaningUpdateForce = (lateralTorque.axis * lateralTorque.magnitude) + (longitudeTorque.axis * longitudeTorque.magnitude);
    windVaningUpdateForce = (testLatTorque) + (testLongTorque);
    
    //windVaningUpdateForce = DirectX::SimpleMath::Vector3::Transform(windVaningUpdateForce, updateMat);
    //m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 10.0f, 0.0f, DirectX::Colors::Yellow);
    //windVaningUpdateForce = DirectX::SimpleMath::Vector3::Zero;

    m_debugData->PushDebugLine(m_heli.q.position, windVaningUpdateForce, 15.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->DebugPushUILineDecimalNumber("windVaningUpdateForce.Length() = ", windVaningUpdateForce.Length(), "");
    return windVaningUpdateForce;
}


void Vehicle::UpdateCollisionImpulseForces(const float aTimeStep)
{
    Utility::UpdateImpulseForceBellCurve(m_heli.testCollisionImpulseForce, aTimeStep);

    if (m_heli.testCollisionImpulseForce.isActive == false)
    {
        m_heli.isVehicleCollisionTrue = false;
    }
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

    
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormWorld, 10.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.cyclicNormLocal, 10.0f, 0.0f, DirectX::Colors::Blue);

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

void Vehicle::UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep)
{
    float rodLength = (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput);
    DirectX::SimpleMath::Vector3 testRod = m_heli.localMainRotorPos - m_heli.localCenterOfMass;
    testRod = m_heli.mainRotorPos - m_heli.centerOfMass;
    testRod = m_heli.gravityTorqueArmPos - m_heli.centerOfMass;
    rodLength = testRod.Length();
    

    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    cyclicQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    //DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);
    

    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    }


    
    DirectX::SimpleMath::Vector3 rodUp = m_heli.up;
    rodUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 rodRotor = m_heli.q.mainRotorForceNormal;
    //DirectX::SimpleMath::Vector3 rod = rodUp + rodRotor;
    DirectX::SimpleMath::Vector3 rod = rodUp + updatedCyclic;
    //float thetaAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, rod);
    float thetaAngle = Utility::GetAngleBetweenVectors(m_heli.up, rod);

    // body roll torque due to pendulum
    Utility::Torque torqueUpdate;
    torqueUpdate.axis = -rod;
    torqueUpdate.axis = rod;
    //torqueUpdate.axis = torqueUpdate.axis.Cross(DirectX::SimpleMath::Vector3::UnitY);
    torqueUpdate.axis = torqueUpdate.axis.Cross(-m_heli.up);
    torqueUpdate.axis.Normalize();
    torqueUpdate.magnitude = -(m_heli.gravity.y * aTimeStep) * rodLength * sin(thetaAngle);
    torqueUpdate.magnitude *= 0.2f;
    //torqueUpdate.magnitude = -((abs(m_heli.controlInput.cyclicInputPitch) + abs(m_heli.controlInput.cyclicInputRoll)) * 20.0f) * rodLength * sin(thetaAngle);
    //torqueUpdate.magnitude *= aTimeStep; // reducing further to eliminate rotational jitters, needs further investigation
    m_heli.q.pendulumTorqueForceTest = torqueUpdate;
    aTorque = torqueUpdate;

    //m_debugData->DebugPushTestLine(m_heli.q.position, torqueUpdate.axis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushUILineDecimalNumber("torqueUpdate.magnitude =", torqueUpdate.magnitude, "");

    // acceleration due to pendulum motion
    DirectX::SimpleMath::Vector3 accelerationDirection = torqueUpdate.axis;
    accelerationDirection = accelerationDirection.Cross(-rod);
    accelerationDirection.Normalize();
    float accelerationMag = -((m_heli.gravity.y) / rodLength) * sin(thetaAngle);
    accelerationMag *= 10.0f;
    //m_debugData->DebugPushUILineDecimalNumber("accelerationMag =", accelerationMag, "");
    //m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    
    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        accelerationDirection *= accelerationMag;
        //m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        //m_debugData->DebugPushUILineDecimalNumber("Pendulum on =", 0.0f, "");
        aVelocity += accelerationDirection;
    }
}

void Vehicle::UpdatePhysicsPoints(struct HeliData& aVehicle)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(aVehicle.q.position, -aVehicle.right, aVehicle.up);

    aVehicle.centerOfMass = m_heli.localCenterOfMass;
    aVehicle.centerOfMass = DirectX::SimpleMath::Vector3::Transform(aVehicle.localCenterOfMass, updateMat);
    aVehicle.mainRotorPos = m_heli.localMainRotorPos;
    aVehicle.mainRotorPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localMainRotorPos, updateMat);

    aVehicle.gravityTorqueArmPos = m_heli.localGravityTorqueArmPos;
    aVehicle.gravityTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.gravityTorqueArmPos, updateMat);

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

    aVehicle.weaponDirection = m_modelController->GetWeaponDir();
    //aVehicle.weaponPos = m_modelController->GetWeaponPos();
    aVehicle.weaponPos = m_modelController->GetMuzzlePos();
    aVehicle.muzzlePos = m_modelController->GetMuzzlePos();
    aVehicle.localizedMuzzlePos = m_modelController->GetLocalizedMuzzlePos();

    // bounding box
    updateMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicle.right, aVehicle.up);
    DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(updateMat);
    m_heli.boundingBox.Orientation = updateQuat;
    //m_heli.boundingBox.Center = m_heli.q.position + (m_heli.forward * 0.75f);
    m_heli.boundingBox.Center = m_heli.q.position;
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

void Vehicle::UpdateTensor()
{
    DirectX::SimpleMath::Matrix tensorUpdate = DirectX::SimpleMath::Matrix::Identity;
    //DirectX::SimpleMath::Matrix tensorUpdate = m_heli.inverseCannonTensor;
    //DirectX::SimpleMath::Matrix tensorUpdate = m_heli.cannonTensor;
    //tensorUpdate *= DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.controlInput.turretYaw);

    DirectX::SimpleMath::Matrix cannonTensorUpdate = m_heli.localCannonTensor;
    DirectX::SimpleMath::Matrix turretRotationMat = DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.controlInput.turretYaw);
    DirectX::SimpleMath::Quaternion turretRotationQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, m_heli.controlInput.turretYaw);
    //cannonTensorUpdate *= DirectX::SimpleMath::Matrix::CreateRotationY(m_heli.controlInput.turretYaw);
    cannonTensorUpdate = DirectX::SimpleMath::Matrix::Transform(cannonTensorUpdate, turretRotationQuat);
    //m_heli.testTensor = cannonTensorUpdate;
    cannonTensorUpdate = cannonTensorUpdate.Invert();

    DirectX::SimpleMath::Vector3 testPos = DirectX::SimpleMath::Vector3::Zero;
    testPos = DirectX::SimpleMath::Vector3::Transform(testPos, tensorUpdate);
    testPos = DirectX::SimpleMath::Vector3::Transform(testPos, m_heli.alignment);
    testPos += m_heli.q.position;
    m_debugData->PushDebugLinePositionIndicator(testPos, 10.0f, 0.0f, DirectX::Colors::Red);
    //tensorUpdate *= m_heli.localInverseInertiaMatrixTest;
    tensorUpdate = m_heli.localInertiaMatrixTest;
    //tensorUpdate += m_heli.localCannonTensor;
    //tensorUpdate += cannonTensorUpdate;
    tensorUpdate = tensorUpdate.Invert();

    tensorUpdate += cannonTensorUpdate;

    tensorUpdate = m_heli.localInverseInertiaMatrixTest;
    //m_heli.toUseTensor = tensorUpdate;
    //m_heli.toUseTensor = DirectX::SimpleMath::Matrix::Identity;

    float mass = 500.0f;
    const float radius = 1.0f;
    const float height = 40.0f;
    DirectX::SimpleMath::Matrix testTensor = DirectX::SimpleMath::Matrix::Identity;
    testTensor._11 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * mass * (radius * radius));
    testTensor._22 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * mass * (radius * radius));
    testTensor._33 = (1.0f / 2.0f) * (mass) * (radius * radius);

    float xExtent = m_inertiaModelX;
    float yExtent = m_inertiaModelY;
    float zExtent = m_inertiaModelZ;

    mass = m_testMass;
    // cuboid
    testTensor._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    testTensor._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    testTensor._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    testTensor = m_heli.localCannonTensor;
    testTensor.Translation(m_heli.cannonPos);

    DirectX::SimpleMath::Quaternion testRotationQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, m_heli.controlInput.turretYaw);
    
    testTensor = DirectX::SimpleMath::Matrix::Transform(testTensor, testRotationQuat);
    testTensor *= m_heli.localInertiaMatrixTest;
    m_heli.testTensor = testTensor;
    DirectX::SimpleMath::Matrix inverseTestTensor = testTensor;
    inverseTestTensor = inverseTestTensor.Invert();

    inverseTestTensor = DirectX::SimpleMath::Matrix::Transform(inverseTestTensor, testRotationQuat);

    //m_heli.toUseTensor = inverseTestTensor;

    DirectX::SimpleMath::Vector3 testTrans = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Matrix testTransTensor = m_heli.localInertiaMatrixTest;
    DirectX::SimpleMath::Matrix testOutPutTensor = DirectX::SimpleMath::Matrix::Identity;
    Utility::TransformInertiaTensor(testTrans, testTransTensor, testOutPutTensor);
}

void Vehicle::UpdateTerrainNorm()
{
    m_heli.terrainNormal = m_environment->GetTerrainNormal(m_heli.q.position);
}

void Vehicle::UpdateTerrainNormTorque()
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
    updateTorque.magnitude *= 1.5f;
    //m_debugData->DebugPushUILineDecimalNumber("m_testTerrainNormTorque.magnitude = ", m_testTerrainNormTorque.magnitude, "");
    m_testTerrainNormTorque.axis = updateTorque.axis;
    m_testTerrainNormTorque.magnitude = updateTorque.magnitude;
}

void Vehicle::UpdateTestDrivetrainTorque(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);
    //m_debugData->DebugPushUILineDecimalNumber("cyclicAngleUpdate                   = ", cyclicAngleUpdate, "");

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);
    //m_debugData->PushDebugLinePositionIndicator(localRear, 1.f, 0.0, DirectX::Colors::White);
    //m_debugData->PushDebugLinePositionIndicator(DirectX::SimpleMath::Vector3::Zero, 1.f, 0.0, DirectX::Colors::Red);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("lateralDot = ", lateralDot, "");
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("longitudinalDot = ", longitudinalDot, "");
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("upDot      = ", upDot, "");

    const float torqueMax = m_testForceMod1;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;
    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);


    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);



    if (m_debugToggle == true || 1 == 1)
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
    //momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(forceSumTest));
    momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(longitudinalForce));
    float momentSumTestLength = (torqueArmLateral.Cross(lateralForce)).Length() + (torqueArmLongitudinal.Cross(longitudinalForce)).Length();

    DirectX::SimpleMath::Vector3 latLongForceSum = lateralForce + longitudinalForce;

    m_testLatTorqueArm = torqueArmLateral;
    m_testLatTorqueForce = lateralForce;
    m_testLongTorqueArm = torqueArmLongitudinal;
    m_testLongTorqueForce = longitudinalForce;


    Utility::Torque torqueSumTest = Utility::GetTorqueForce(momentSumTest, forceSumTest);


    Utility::Torque torqueSum;
    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    Utility::Torque testTorque2;
    testTorque2.axis = momentSumTest;
    testTorque2.magnitude = momentSumTestLength;
    testTorque2.axis.Normalize();


    if (m_debugToggle == false)
    {
        m_testDrivetrainTorque = torqueSum;
    }
    else
    {
        m_testDrivetrainTorque = torqueSum;
        //m_testDrivetrainTorque = testTorque2;
    }
    m_testDrivetrainTorqueLocal = torqueSumLocal;

    ////////////////////////////////////////////////
    // DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    // DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;

    //const float torqueMax = m_testForceMod1;
    centerMassPos = m_heli.centerOfMass;

    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forcePart = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Vector3 liftDirection = m_heli.up;
    //DirectX::SimpleMath::Vector3 liftDirection = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 liftDirection = DirectX::SimpleMath::Vector3::UnitY + m_heli.up;
    liftDirection.Normalize();
    /*
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * -m_heli.up, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up, m_heli.physicsPointRear);
    */
    /*
    const float constTorque = torqueMax * 0.2f;
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRear);
    */
    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputRoll = ", m_heli.controlInput.cyclicInputRoll, "");
    //centerMassPos = m_heli.topTestPos;
    //centerMassPos = m_heli.baseTestPos;
    const float constTorque = torqueMax * (m_heli.controlInput.cyclicInputMax * 0.1f);
    //const float constTorque = 0.0f;;
    float rollRight = 0.0f;
    float rollLeft = 0.0f;
    if (m_heli.controlInput.cyclicInputRoll > 0.0f)
    {
        rollLeft = constTorque;
        if (m_heli.physicsPointLeft.y > m_heli.physicsPointRear.y)
        {
            rollLeft = 0.0f;
        }
        rollRight = (m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputRoll < 0.0f)
    {
        rollLeft = (-m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
        rollRight = constTorque;
        if (m_heli.physicsPointLeft.y < m_heli.physicsPointRear.y)
        {
            rollRight = 0.0f;
        }
    }
    else
    {
        rollLeft = constTorque;
        rollRight = constTorque;
        if (m_heli.physicsPointLeft.y > m_heli.physicsPointRear.y)
        {
            //rollLeft = 0.0f;
        }
        if (m_heli.physicsPointLeft.y < m_heli.physicsPointRear.y)
        {
            //rollRight = 0.0f;
        }
    }

    m_debugData->DebugPushUILineDecimalNumber("xxxrollLeft  = ", rollLeft, "");
    m_debugData->DebugPushUILineDecimalNumber("xxxrollRight = ", rollRight, "");

    float rollBack = 0.0f;
    float rollForward = 0.0f;
    
    if (m_heli.controlInput.cyclicInputPitch > 0.0f)
    {
        rollForward = constTorque;
        rollBack = (m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputPitch < 0.0f)
    {
        rollForward = (-m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
        rollBack = constTorque;
    }
    else
    {
        rollBack = constTorque;
        rollForward = constTorque;
    }


    
    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputPitch = ", m_heli.controlInput.cyclicInputPitch, "");
    DirectX::SimpleMath::Vector3 leftForce = rollLeft * liftDirection;
    DirectX::SimpleMath::Vector3 rightForce = rollRight * liftDirection;
    DirectX::SimpleMath::Vector3 backForce = rollBack * liftDirection;
    DirectX::SimpleMath::Vector3 forwardForce = rollForward * liftDirection;
    DirectX::SimpleMath::Vector3 frontForce = forwardForce * liftDirection;
    DirectX::SimpleMath::Vector3 rearForce = backForce * liftDirection;

    leftForce = GetThrusterLiftMagnitude(leftForce, m_heli.physicsPointLeft);
    rightForce = GetThrusterLiftMagnitude(rightForce, m_heli.physicsPointRight);
    frontForce = GetThrusterLiftMagnitude(frontForce, m_heli.physicsPointFront);
    rearForce = GetThrusterLiftMagnitude(rearForce, m_heli.physicsPointRear);

    
    m_debugData->DebugPushUILineDecimalNumber("leftForce.Length() = ", leftForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("rightForce.Length() = ", rightForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("frontForce.Length() = ", frontForce.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("rearForce.Length() = ", rearForce.Length(), "");
    

    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, leftForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, rightForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, frontForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, rearForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);

    /*
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);
    Utility::AddForce(forceSum, forcePart);
    */

    DirectX::SimpleMath::Vector3 leftTorqueArm = m_heli.physicsPointLeft - centerMassPos;
    DirectX::SimpleMath::Vector3 rightTorqueArm = m_heli.physicsPointRight - centerMassPos;

    m_debugData->PushDebugLine(m_heli.centerOfMass, leftTorqueArm, 10.0f, 0.0f, DirectX::Colors::Yellow);
    m_debugData->PushDebugLine(m_heli.centerOfMass, rightTorqueArm, 10.0f, 0.0f, DirectX::Colors::Orange);

    /* dumb dumb dumb arg!!!!!!
    DirectX::SimpleMath::Vector3 forcePartLeft = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    DirectX::SimpleMath::Vector3 forcePartRight = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    DirectX::SimpleMath::Vector3 forcePartFront = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    DirectX::SimpleMath::Vector3 forcePartBack = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);
    */

    //DirectX::SimpleMath::Vector3 forcePartLeft = Utility::ProcessTorqueForce(leftTorqueArm, centerMassPos, leftForce);
    //DirectX::SimpleMath::Vector3 forcePartRight = Utility::ProcessTorqueForce(rightTorqueArm, centerMassPos, rightForce);
    DirectX::SimpleMath::Vector3 forcePartLeft = Utility::ProcessTorqueForce(m_heli.physicsPointLeft, centerMassPos, leftForce);
    DirectX::SimpleMath::Vector3 forcePartRight = Utility::ProcessTorqueForce(m_heli.physicsPointRight, centerMassPos, rightForce);
    DirectX::SimpleMath::Vector3 forcePartFront = Utility::ProcessTorqueForce(m_heli.physicsPointFront, centerMassPos, frontForce);
    DirectX::SimpleMath::Vector3 forcePartBack = Utility::ProcessTorqueForce(m_heli.physicsPointRear, centerMassPos, rearForce);
    
    m_debugData->DebugPushUILineDecimalNumber("forcePartLeft.Length() = ", forcePartLeft.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("forcePartRight.Length() = ", forcePartRight.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("forcePartFront.Length() = ", forcePartFront.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("forcePartBack.Length() = ", forcePartBack.Length(), "");
    /*
    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    */

    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);

    forceSum = DirectX::SimpleMath::Vector3::Zero;
    forceSum = forcePartLeft + forcePartRight + forcePartFront + forcePartBack;
    //forceSum = forcePartLeft + forcePartRight;

    m_debugData->DebugPushUILineDecimalNumber("forceSum.Length() = ", forceSum.Length(), "");
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forceSum, 2.0f, 1.0f, 0.0f, DirectX::Colors::Yellow);
    /*
    m_debugData->DebugClearUI();
    m_debugData->PushDebugLinePositionIndicator(m_heli.centerOfMass, 5.0f, 0.0, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(m_heli.topTestPos, 6.0f, 0.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLinePositionIndicator(m_heli.baseTestPos, 4.0f, 0.0, DirectX::Colors::Red);
    */
    m_hoverTorqueForceSum = forceSum;
}

void Vehicle::UpdateTestDrivetrainTorque2(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);
    m_debugData->DebugPushUILineDecimalNumber("cyclicAngleUpdate                   = ", cyclicAngleUpdate, "");

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);
    //m_debugData->PushDebugLinePositionIndicator(localRear, 1.f, 0.0, DirectX::Colors::White);
    //m_debugData->PushDebugLinePositionIndicator(DirectX::SimpleMath::Vector3::Zero, 1.f, 0.0, DirectX::Colors::Red);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    m_debugData->DebugPushUILineDecimalNumber("lateralDot = ", lateralDot, "");
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    m_debugData->DebugPushUILineDecimalNumber("longitudinalDot = ", longitudinalDot, "");
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    m_debugData->DebugPushUILineDecimalNumber("upDot      = ", upDot, "");

    const float torqueMax = m_testForceMod1;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;
    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);


    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);


    m_debugData->DebugPushUILineDecimalNumber("m_debugToggle    = ", static_cast<float>(m_debugToggle), "");
    if (m_debugToggle == true && 1 == 0)
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
    //momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(forceSumTest));
    momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(longitudinalForce));
    float momentSumTestLength = (torqueArmLateral.Cross(lateralForce)).Length() + (torqueArmLongitudinal.Cross(longitudinalForce)).Length();

    DirectX::SimpleMath::Vector3 latLongForceSum = lateralForce + longitudinalForce;

    m_testLatTorqueArm = torqueArmLateral;
    m_testLatTorqueForce = lateralForce;
    m_testLongTorqueArm = torqueArmLongitudinal;
    m_testLongTorqueForce = longitudinalForce;


    Utility::Torque torqueSumTest = Utility::GetTorqueForce(momentSumTest, forceSumTest);


    Utility::Torque torqueSum;
    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    Utility::Torque testTorque2;
    testTorque2.axis = momentSumTest;
    testTorque2.magnitude = momentSumTestLength;
    testTorque2.axis.Normalize();


    if (m_debugToggle == false)
    {
        m_testDrivetrainTorque = torqueSum;
    }
    else
    {
        m_testDrivetrainTorque = torqueSum;
        //m_testDrivetrainTorque = testTorque2;
    }
    m_testDrivetrainTorqueLocal = torqueSumLocal;
}

void Vehicle::UpdateTestDrivetrainTorque3(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);
    //m_debugData->DebugPushUILineDecimalNumber("cyclicAngleUpdate                   = ", cyclicAngleUpdate, "");

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);
    //m_debugData->PushDebugLinePositionIndicator(localRear, 1.f, 0.0, DirectX::Colors::White);
    //m_debugData->PushDebugLinePositionIndicator(DirectX::SimpleMath::Vector3::Zero, 1.f, 0.0, DirectX::Colors::Red);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("lateralDot = ", lateralDot, "");
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("longitudinalDot = ", longitudinalDot, "");
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("upDot      = ", upDot, "");

    const float torqueMax = m_testForceMod1;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;
    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);


    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);



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
    //momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(forceSumTest));
    momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(longitudinalForce));
    float momentSumTestLength = (torqueArmLateral.Cross(lateralForce)).Length() + (torqueArmLongitudinal.Cross(longitudinalForce)).Length();

    DirectX::SimpleMath::Vector3 latLongForceSum = lateralForce + longitudinalForce;

    m_testLatTorqueArm = torqueArmLateral;
    m_testLatTorqueForce = lateralForce;
    m_testLongTorqueArm = torqueArmLongitudinal;
    m_testLongTorqueForce = longitudinalForce;


    Utility::Torque torqueSumTest = Utility::GetTorqueForce(momentSumTest, forceSumTest);


    Utility::Torque torqueSum;
    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    Utility::Torque testTorque2;
    testTorque2.axis = momentSumTest;
    testTorque2.magnitude = momentSumTestLength;
    testTorque2.axis.Normalize();


    if (m_debugToggle == false)
    {
        m_testDrivetrainTorque = torqueSum;
    }
    else
    {
        m_testDrivetrainTorque = torqueSum;
        //m_testDrivetrainTorque = testTorque2;
    }
    m_testDrivetrainTorqueLocal = torqueSumLocal;


    ////////////////////////////////////////////////
    // DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    // DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;

    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forcePart = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 liftDirection = m_heli.up;
    //DirectX::SimpleMath::Vector3 liftDirection = DirectX::SimpleMath::Vector3::UnitY;
    /*
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * -m_heli.up, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up, m_heli.physicsPointRear);
    */
    /*
    const float constTorque = torqueMax * 0.2f;
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRear);
    */

    //centerMassPos = m_heli.topTestPos;
    //centerMassPos = m_heli.baseTestPos;
    //const float constTorque = torqueMax * (m_heli.controlInput.cyclicInputMax * 0.01f);
    const float constTorque = 0.0f;;
    float rollRight = 0.0f;
    float rollLeft = 0.0f;
    if (m_heli.controlInput.cyclicInputRoll > 0.0f)
    {
        rollLeft = constTorque;
        rollRight = (m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputRoll < 0.0f)
    {
        rollLeft = (-m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
        rollRight = constTorque;
    }
    else
    {
        rollLeft = constTorque;
        rollRight = constTorque;
    }
    float rollBack = 0.0f;
    float rollForward = 0.0f;

    if (m_heli.controlInput.cyclicInputPitch > 0.0f)
    {
        rollForward = constTorque;
        rollBack = (m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputPitch < 0.0f)
    {
        rollForward = (-m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
        rollBack = constTorque;
    }
    else
    {
        rollBack = constTorque;
        rollForward = constTorque;
    }

    DirectX::SimpleMath::Vector3 leftForce = rollLeft * liftDirection;
    DirectX::SimpleMath::Vector3 rightForce = rollRight * liftDirection;
    DirectX::SimpleMath::Vector3 backForce = rollBack * liftDirection;
    DirectX::SimpleMath::Vector3 forwardForce = rollForward * liftDirection;
    DirectX::SimpleMath::Vector3 frontForce = forwardForce * liftDirection;
    DirectX::SimpleMath::Vector3 rearForce = backForce * liftDirection;

    leftForce = GetThrusterLiftMagnitude(leftForce, m_heli.physicsPointLeft);
    rightForce = GetThrusterLiftMagnitude(rightForce, m_heli.physicsPointRight);
    frontForce = GetThrusterLiftMagnitude(frontForce, m_heli.physicsPointFront);
    rearForce = GetThrusterLiftMagnitude(rearForce, m_heli.physicsPointRear);

    /*
    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, leftForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, rightForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, frontForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, rearForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    */

    /*
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);
    Utility::AddForce(forceSum, forcePart);
    */

    DirectX::SimpleMath::Vector3 forcePartLeft = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    DirectX::SimpleMath::Vector3 forcePartRight = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    DirectX::SimpleMath::Vector3 forcePartFront = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    DirectX::SimpleMath::Vector3 forcePartBack = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);

    /*
    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    */

    /*
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    */

    forceSum = DirectX::SimpleMath::Vector3::Zero;
    forceSum = forcePartLeft + forcePartRight + forcePartFront + forcePartBack;
    //forceSum = forcePartLeft;// +forcePartRight;

    //m_debugData->DebugPushUILineDecimalNumber("forceSum.Length() = ", forceSum.Length(), "");
    //m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forceSum, 2.0f, 1.0f, 0.0f, DirectX::Colors::Yellow);

    /*
    m_debugData->DebugClearUI();
    m_debugData->PushDebugLinePositionIndicator(m_heli.centerOfMass, 5.0f, 0.0, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(m_heli.topTestPos, 6.0f, 0.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLinePositionIndicator(m_heli.baseTestPos, 4.0f, 0.0, DirectX::Colors::Red);
    */

    //m_hoverTorqueForceSum = forceSum;
}

void Vehicle::UpdateTestDrivetrainTorque4(const float aTimer)
{
    DirectX::SimpleMath::Quaternion cyclicQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, m_heli.controlInput.cyclicInputRoll, m_heli.controlInput.cyclicInputPitch);
    DirectX::SimpleMath::Vector3 updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);
    float cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    if (cyclicAngle > m_heli.controlInput.cyclicInputMax)
    {
        const float angleProportionToEdgeOfCone = m_heli.controlInput.cyclicInputMax / cyclicAngle;
        cyclicQuat = DirectX::SimpleMath::Quaternion::Slerp(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f), cyclicQuat, angleProportionToEdgeOfCone);
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, cyclicQuat);

    }
    m_heli.controlInput.cyclicNormLocal = updatedCyclic;
    updatedCyclic = DirectX::SimpleMath::Vector3::Transform(updatedCyclic, m_heli.alignment);
    m_heli.controlInput.cyclicNormWorld = updatedCyclic;

    float cyclicAngleUpdate = Utility::GetAngleBetweenVectors(m_heli.up, updatedCyclic);
    //m_debugData->DebugPushUILineDecimalNumber("cyclicAngleUpdate                   = ", cyclicAngleUpdate, "");

    DirectX::SimpleMath::Vector3 localRight = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    DirectX::SimpleMath::Vector3 localRear = DirectX::SimpleMath::Vector3::TransformNormal(-DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);
    //m_debugData->PushDebugLinePositionIndicator(localRear, 1.f, 0.0, DirectX::Colors::White);
    //m_debugData->PushDebugLinePositionIndicator(DirectX::SimpleMath::Vector3::Zero, 1.f, 0.0, DirectX::Colors::Red);

    float lateralDot = localRight.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("lateralDot = ", lateralDot, "");
    float longitudinalDot = localRear.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("longitudinalDot = ", longitudinalDot, "");
    float upDot = m_heli.up.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("upDot      = ", upDot, "");

    const float torqueMax = m_testForceMod1;
    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLateral = m_heli.physicsPointRight - centerMassPos;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinal = m_heli.physicsPointRear - centerMassPos;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    //DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    //DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicNormLocal.z * torqueMax) * m_heli.up;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicNormLocal.x * torqueMax) * m_heli.up;

    Utility::Torque lateralTorque = Utility::GetTorqueForce(torqueArmLateral, lateralForce);
    Utility::Torque longitudinalTorque = Utility::GetTorqueForce(torqueArmLongitudinal, longitudinalForce);


    DirectX::SimpleMath::Vector3 torqueArmLateralLocal = m_heli.localPhysicsPointRight - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 torqueArmLongitudinalLocal = m_heli.localPhysicsPointRear - m_heli.localCenterOfMass;
    DirectX::SimpleMath::Vector3 lateralForceLocal = (m_heli.controlInput.cyclicInputRoll * torqueMax) * DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 longitudinalForceLocal = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -DirectX::SimpleMath::Vector3::UnitY;
    Utility::Torque lateralTorqueLocal = Utility::GetTorqueForce(torqueArmLateralLocal, lateralForceLocal);
    Utility::Torque longitudinalTorqueLocal = Utility::GetTorqueForce(torqueArmLongitudinalLocal, longitudinalForceLocal);



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
    //momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(forceSumTest));
    momentSumTest = (torqueArmLateral.Cross(lateralForce)) + (torqueArmLongitudinal.Cross(longitudinalForce));
    float momentSumTestLength = (torqueArmLateral.Cross(lateralForce)).Length() + (torqueArmLongitudinal.Cross(longitudinalForce)).Length();

    DirectX::SimpleMath::Vector3 latLongForceSum = lateralForce + longitudinalForce;

    m_testLatTorqueArm = torqueArmLateral;
    m_testLatTorqueForce = lateralForce;
    m_testLongTorqueArm = torqueArmLongitudinal;
    m_testLongTorqueForce = longitudinalForce;


    Utility::Torque torqueSumTest = Utility::GetTorqueForce(momentSumTest, forceSumTest);


    Utility::Torque torqueSum;
    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    Utility::Torque torqueSumLocal;
    torqueSumLocal.axis = (lateralTorqueLocal.axis * lateralTorqueLocal.magnitude) + (longitudinalTorqueLocal.axis * longitudinalTorqueLocal.magnitude);
    torqueSumLocal.axis.Normalize();
    torqueSumLocal.magnitude = lateralTorqueLocal.magnitude + longitudinalTorqueLocal.magnitude;

    Utility::Torque testTorque2;
    testTorque2.axis = momentSumTest;
    testTorque2.magnitude = momentSumTestLength;
    testTorque2.axis.Normalize();


    if (m_debugToggle == false)
    {
        m_testDrivetrainTorque = torqueSum;
    }
    else
    {
        m_testDrivetrainTorque = torqueSum;
        //m_testDrivetrainTorque = testTorque2;
    }
    m_testDrivetrainTorqueLocal = torqueSumLocal;


    ////////////////////////////////////////////////
    // DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up;
    // DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up;

    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forcePart = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 liftDirection = m_heli.up;
    //DirectX::SimpleMath::Vector3 liftDirection = DirectX::SimpleMath::Vector3::UnitY;
    /*
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * -m_heli.up, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.up, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * -m_heli.up, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude((m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.up, m_heli.physicsPointRear);
    */
    /*
    const float constTorque = torqueMax * 0.2f;
    DirectX::SimpleMath::Vector3 leftForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointLeft);
    DirectX::SimpleMath::Vector3 rightForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRight);
    DirectX::SimpleMath::Vector3 frontForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointFront);
    DirectX::SimpleMath::Vector3 rearForce = GetThrusterLiftMagnitude(((m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque) * liftDirection, m_heli.physicsPointRear);
    */

    //centerMassPos = m_heli.topTestPos;
    //centerMassPos = m_heli.baseTestPos;
    //const float constTorque = torqueMax * (m_heli.controlInput.cyclicInputMax * 0.01f);
    const float constTorque = 0.0f;;
    float rollRight = 0.0f;
    float rollLeft = 0.0f;
    if (m_heli.controlInput.cyclicInputRoll > 0.0f)
    {
        rollLeft = constTorque;
        rollRight = (m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputRoll < 0.0f)
    {
        rollLeft = (-m_heli.controlInput.cyclicInputRoll * torqueMax) + constTorque;
        rollRight = constTorque;
    }
    else
    {
        rollLeft = constTorque;
        rollRight = constTorque;
    }
    float rollBack = 0.0f;
    float rollForward = 0.0f;

    if (m_heli.controlInput.cyclicInputPitch > 0.0f)
    {
        rollForward = constTorque;
        rollBack = (m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
    }
    else if (m_heli.controlInput.cyclicInputPitch < 0.0f)
    {
        rollForward = (-m_heli.controlInput.cyclicInputPitch * torqueMax) + constTorque;
        rollBack = constTorque;
    }
    else
    {
        rollBack = constTorque;
        rollForward = constTorque;
    }

    DirectX::SimpleMath::Vector3 leftForce = rollLeft * liftDirection;
    DirectX::SimpleMath::Vector3 rightForce = rollRight * liftDirection;
    DirectX::SimpleMath::Vector3 backForce = rollBack * liftDirection;
    DirectX::SimpleMath::Vector3 forwardForce = rollForward * liftDirection;
    DirectX::SimpleMath::Vector3 frontForce = forwardForce * liftDirection;
    DirectX::SimpleMath::Vector3 rearForce = backForce * liftDirection;

    leftForce = GetThrusterLiftMagnitude(leftForce, m_heli.physicsPointLeft);
    rightForce = GetThrusterLiftMagnitude(rightForce, m_heli.physicsPointRight);
    frontForce = GetThrusterLiftMagnitude(frontForce, m_heli.physicsPointFront);
    rearForce = GetThrusterLiftMagnitude(rearForce, m_heli.physicsPointRear);

    /*
    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, leftForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, rightForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, frontForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, rearForce, 1.0f, 0.4f, 0.0f, DirectX::Colors::Red);
    */

    /*
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    Utility::AddForce(forceSum, forcePart);
    forcePart = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);
    Utility::AddForce(forceSum, forcePart);
    */

    DirectX::SimpleMath::Vector3 forcePartLeft = Utility::ProcessTorqueForce(m_heli.physicsPointLeft - centerMassPos, centerMassPos, leftForce);
    DirectX::SimpleMath::Vector3 forcePartRight = Utility::ProcessTorqueForce(m_heli.physicsPointRight - centerMassPos, centerMassPos, rightForce);
    DirectX::SimpleMath::Vector3 forcePartFront = Utility::ProcessTorqueForce(m_heli.physicsPointFront - centerMassPos, centerMassPos, frontForce);
    DirectX::SimpleMath::Vector3 forcePartBack = Utility::ProcessTorqueForce(m_heli.physicsPointRear - centerMassPos, centerMassPos, rearForce);

    /*
    m_debugData->PushDebugLineScaled(m_heli.physicsPointLeft, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRight, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointFront, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.physicsPointRear, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    */

    /*
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartLeft, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartRight, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartFront, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forcePartBack, 1.0f, 0.4f, 0.0f, DirectX::Colors::Blue);
    */

    forceSum = DirectX::SimpleMath::Vector3::Zero;
    forceSum = forcePartLeft + forcePartRight + forcePartFront + forcePartBack;
    //forceSum = forcePartLeft;// +forcePartRight;

    //m_debugData->DebugPushUILineDecimalNumber("forceSum.Length() = ", forceSum.Length(), "");
    //m_debugData->PushDebugLineScaled(m_heli.centerOfMass, forceSum, 2.0f, 1.0f, 0.0f, DirectX::Colors::Yellow);

    /*
    m_debugData->DebugClearUI();
    m_debugData->PushDebugLinePositionIndicator(m_heli.centerOfMass, 5.0f, 0.0, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(m_heli.topTestPos, 6.0f, 0.0, DirectX::Colors::Blue);
    m_debugData->PushDebugLinePositionIndicator(m_heli.baseTestPos, 4.0f, 0.0, DirectX::Colors::Red);
    */

    //m_hoverTorqueForceSum = forceSum;
}

void Vehicle::UpdateVehicle(const double aTimeDelta)
{
    m_sumOfForceTest = DirectX::SimpleMath::Vector3::Zero;
    m_sumOfMomentsTest = DirectX::SimpleMath::Vector3::Zero;
    m_bodyTorqueTest.axis = DirectX::SimpleMath::Vector3::UnitY;
    m_bodyTorqueTest.magnitude = 0.0f;

    //UpdateTensor();
    UpdatePhysicsPoints(m_heli);
    //UpdateCyclicNorm();
    UpdateTestDrivetrainTorque4(static_cast<float>(aTimeDelta));
    
    //m_hoverTorqueForceSum = CalculateHoverTorqueForce(m_heli, static_cast<float>(aTimeDelta));


    Utility::Torque prevGravTorque = m_testGravForce;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_heli.mainRotorPos - m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 gravityForce = m_heli.gravity * (static_cast<float>(0.01));
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    //m_testGravForce = gravTorque;
    //m_testGravForce.axis = (prevGravTorque.axis * prevGravTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    //m_testGravForce.magnitude = (prevGravTorque.magnitude + gravTorque.magnitude) * 0.5f;
    //m_testGravForce.axis.Normalize();
    m_testGravForce = gravTorque;


    m_heli.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_heli.q.bodyTorqueForce.magnitude = 0.0f;

    DirectX::SimpleMath::Vector3 prevVelocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 prevPos = m_heli.q.position;

    m_heli.isVehicleLanding = false;
    m_heli.terrainHightAtPos = m_environment->GetTerrainHeightAtPos(m_heli.q.position);
    m_heli.altitude = m_heli.landingGearPos.y - m_heli.terrainHightAtPos;
    m_debugData->DebugPushUILineDecimalNumber("altitude = ", m_heli.altitude, "");
    if (m_heli.landingGearPos.y - m_heli.terrainHightAtPos > 0.1)
    {
        m_heli.isVehicleAirborne = true;
        m_testTimer += static_cast<float>(aTimeDelta);
    }
    else
    {
        if (m_heli.isVehicleAirborne == true)
        {
            //LandVehicle();
            m_heli.isVehicleLanding = true;
            m_testTimer2 = m_testTimer;
            //m_testTimer = 0.0f;
        }
        m_heli.isVehicleAirborne = false;
    }
    
    UpdateTerrainNorm();
    //m_heli.buoyancyForce = CalculateBuoyancyForce(m_heli);
    UpdateBrakeForce(static_cast<float>(aTimeDelta));
    UpdateTerrainNormTorque();
    Utility::UpdateImpulseForceBellCurve(m_testImpulseForce, static_cast<float>(aTimeDelta));
    UpdateCollisionImpulseForces(static_cast<float>(aTimeDelta));

    UpdateVehicleForces(static_cast<float>(aTimeDelta));
    if (m_isRungeOn == true)
    {
        RungeKutta4(&m_heli, aTimeDelta);
    }

    //m_debugData->DebugClearUI();
   

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
    m_debugData->DebugPushUILineDecimalNumber("Speed = ", m_heli.speed, "");
    //m_debugData->DebugPushUILineDecimalNumber("MPH = ", m_heli.speed * 2.237f, "");
    InputDecay(aTimeDelta);

    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    m_heli.testAccelVec = (m_heli.q.velocity - prevVelocity) / static_cast<float>(aTimeDelta);

    /*
    //m_debugData->DebugClearUI();
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.forward, 10.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.up, 10.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.right, 10.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLine(m_heli.q.position, -m_heli.right, 10.0f, 0.0f, DirectX::Colors::White);
    */

    /*
    m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 xDimension = m_heli.q.position;
    xDimension.x += m_heli.dimensions.x * 0.5f;
    DirectX::SimpleMath::Vector3 yDimension = m_heli.q.position;
    yDimension.y += m_heli.dimensions.y * 0.5f;
    DirectX::SimpleMath::Vector3 zDimension = m_heli.q.position;
    zDimension.z += m_heli.dimensions.z * 0.5f;
    DirectX::SimpleMath::Vector3 halfDimensions = m_heli.dimensions * 0.5f;
    DirectX::SimpleMath::Vector3 xDimensionNeg = m_heli.q.position;
    xDimensionNeg.x -= m_heli.dimensions.x * 0.5f;
    DirectX::SimpleMath::Vector3 yDimensionNeg = m_heli.q.position;
    yDimensionNeg.y -= m_heli.dimensions.y * 0.5f;
    DirectX::SimpleMath::Vector3 zDimensionNeg = m_heli.q.position;
    zDimensionNeg.z -= m_heli.dimensions.z * 0.5f;
    m_debugData->PushDebugLinePositionIndicator(xDimension, 5.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(yDimension, 5.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(zDimension, 5.0f, 0.0f, DirectX::Colors::White);

    m_debugData->PushDebugLinePositionIndicator(xDimensionNeg, 5.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(yDimensionNeg, 5.0f, 0.0f, DirectX::Colors::White);
    m_debugData->PushDebugLinePositionIndicator(zDimensionNeg, 5.0f, 0.0f, DirectX::Colors::White);

    m_debugData->PushDebugLinePositionIndicator(m_heli.q.position + m_heli.dimensions, 5.0f, 0.0f, DirectX::Colors::White);
    */

    UpdateResistance();
    UpdateAlignmentTorque();
    UpdateAlignmentCamera();

    //m_heli.isVehicleCollisionTrue = false;
    m_isFiredTest = false;
}

void Vehicle::UpdateVehicleFireControl(const double aTimeDelta)
{
    m_fireControl->UpdateFireControl(aTimeDelta);
}

void Vehicle::UpdateVehicleForces(const float aTimeStep)
{
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
    velocityUpdate += rotorForce;
    velocityUpdate += m_heli.buoyancyForce;
    velocityUpdate += slopeForce;
    velocityUpdate += m_heli.controlInput.brakeForce;

    m_heli.vehicleLinearForcesSum = velocityUpdate;

    /*
    m_debugData->DebugPushUILineDecimalNumber("calcHoverDriveForce = ", calcHoverDriveForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, calcHoverDriveForce, 10.0f, 0.0f, DirectX::Colors::White);

    m_debugData->DebugPushUILineDecimalNumber("damperForce         = ", damperForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, damperForce, 10.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->DebugPushUILineDecimalNumber("gravForce           = ", gravForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, gravForce, 10.0f, 0.0f, DirectX::Colors::Lime);

    m_debugData->DebugPushUILineDecimalNumber("jetThrust           = ", jetThrust.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, jetThrust, 10.0f, 0.0f, DirectX::Colors::Orange);

    m_debugData->DebugPushUILineDecimalNumber("rotorForce          = ", rotorForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, rotorForce, 10.0f, 0.0f, DirectX::Colors::Yellow);

    m_debugData->DebugPushUILineDecimalNumber("m_heli.buoyancyForce = ", m_heli.buoyancyForce.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, m_heli.buoyancyForce, 10.0f, 0.0f, DirectX::Colors::Blue);

    m_debugData->DebugPushUILineDecimalNumber("slopeForce            = ", slopeForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, slopeForce, 10.0f, 0.0f, DirectX::Colors::Salmon);

    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.brakeForce = ", m_heli.controlInput.brakeForce.Length(), "");
    //m_debugData->PushDebugLine(m_heli.q.position, m_heli.controlInput.brakeForce, 10.0f, 0.0f, DirectX::Colors::Gray);
    */

    DirectX::SimpleMath::Vector3 preVelocityUpdate = velocityUpdate;

    // angular

    m_heli.windVaningTorueForce = CalculateWindVaningTorqueForce(m_heli);

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    DirectX::SimpleMath::Vector3 tempVel = DirectX::SimpleMath::Vector3::Zero;
    UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeStep));
    //UpdatePendulumMotion(pendTorque, tempVel, static_cast<float>(aTimeDelta));
    DirectX::SimpleMath::Vector3 postVelocityUpdate = velocityUpdate;
    
    /*
    m_debugData->DebugPushUILineDecimalNumber("preVelocityUpdate.Length()", preVelocityUpdate.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("postVelocityUpdate.Length()", postVelocityUpdate.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("Delta   = ", postVelocityUpdate.Length() - preVelocityUpdate.Length(), "");
    m_debugData->PushDebugLine(m_heli.q.position, preVelocityUpdate, 10.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLine(m_heli.q.position, postVelocityUpdate, 10.0f, 0.0f, DirectX::Colors::Blue);
    */

    DirectX::SimpleMath::Vector3 accelVecUpdate = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueRunge(accelVecUpdate, pendTorque, static_cast<float>(aTimeStep));

    DirectX::SimpleMath::Vector3 angAccelVecTensorUpdate = accelVecUpdate;
    DirectX::SimpleMath::Matrix inverseAlignment = m_heli.alignment;
    inverseAlignment = inverseAlignment.Invert();


    /*
    DirectX::SimpleMath::Vector3 calcHoverDriveForceLine = calcHoverDriveForce;
    calcHoverDriveForceLine = DirectX::SimpleMath::Vector3::Transform(calcHoverDriveForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, calcHoverDriveForceLine, 10.0f, 0.0f, DirectX::Colors::Red);

    DirectX::SimpleMath::Vector3 damperForceLine = damperForce;
    damperForceLine = DirectX::SimpleMath::Vector3::Transform(damperForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, damperForceLine, 10.0f, 0.0f, DirectX::Colors::Green);

    DirectX::SimpleMath::Vector3 gravForceLine = gravForce;
    gravForceLine = DirectX::SimpleMath::Vector3::Transform(gravForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, gravForceLine, 10.0f, 0.0f, DirectX::Colors::Blue);

    DirectX::SimpleMath::Vector3 jetThrustLine = jetThrust;
    jetThrustLine = DirectX::SimpleMath::Vector3::Transform(jetThrustLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, jetThrustLine, 10.0f, 0.0f, DirectX::Colors::Yellow);

    DirectX::SimpleMath::Vector3 rotorForceLine = rotorForce;
    rotorForceLine = DirectX::SimpleMath::Vector3::Transform(rotorForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, rotorForceLine, 10.0f, 0.0f, DirectX::Colors::Orange);

    DirectX::SimpleMath::Vector3 buoyancyForceLine = m_heli.buoyancyForce;
    buoyancyForceLine = DirectX::SimpleMath::Vector3::Transform(buoyancyForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, buoyancyForceLine, 10.0f, 0.0f, DirectX::Colors::Purple);

    DirectX::SimpleMath::Vector3 slopeForceLine = slopeForce;
    slopeForceLine = DirectX::SimpleMath::Vector3::Transform(slopeForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, slopeForceLine, 10.0f, 0.0f, DirectX::Colors::Gray);

    DirectX::SimpleMath::Vector3 brakeForceLine = m_heli.controlInput.brakeForce;
    brakeForceLine = DirectX::SimpleMath::Vector3::Transform(brakeForceLine, inverseAlignment);
    m_debugData->PushDebugLine(m_heli.q.position, brakeForceLine, 10.0f, 0.0f, DirectX::Colors::White);


    DirectX::SimpleMath::Vector3 accelTorqueLine = accelVecUpdate;
    accelTorqueLine = DirectX::SimpleMath::Vector3::Transform(accelTorqueLine, inverseAlignment);
    //m_debugData->PushDebugLine(m_heli.q.position, accelTorqueLine, 10.0f, 0.0f, DirectX::Colors::Red);

    
    DirectX::SimpleMath::Vector3 driveTrainTorque = m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude;
    //driveTrainTorque = DirectX::SimpleMath::Vector3::Transform(driveTrainTorque, inverseAlignment);
    //m_debugData->PushDebugLine(m_heli.q.position, driveTrainTorque, 10.0f, 0.0f, DirectX::Colors::Blue);
    */
  

    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, inverseAlignment);
    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, m_heli.toUseTensor);
    angAccelVecTensorUpdate = DirectX::SimpleMath::Vector3::Transform(angAccelVecTensorUpdate, m_heli.alignment);
    accelVecUpdate = angAccelVecTensorUpdate;
    //m_debugData->DebugPushUILineDecimalNumber("accelVecUpdate = ", accelVecUpdate.Length(), "");

    m_debugData->DebugClearUI();
    m_heli.vehicleAngularForcesSum = accelVecUpdate;
}

void Vehicle::DebugInputVelocityZero()
{
    //m_heli.q.velocity = DirectX::SimpleMath::Vector3::Zero;
    const float speed = m_heli.q.velocity.Length();
    DirectX::SimpleMath::Vector3 antiVelocity = m_heli.q.velocity * -1.0f;
    DirectX::SimpleMath::Vector3 brakeNorm = antiVelocity;
    brakeNorm.Normalize();
    const float brakeVal = 0.02f;

    DirectX::SimpleMath::Vector3 brakeForce = antiVelocity  * brakeVal;
    //m_heli.q.velocity += brakeForce;
    m_heli.controlInput.brakeIsPressed = true;
}

void Vehicle::FireWeapon()
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        //DirectX::SimpleMath::Vector3 pos = m_heli.weaponPos;
        //DirectX::SimpleMath::Vector3 launchDir = m_heli.weaponDirection;

        DirectX::SimpleMath::Vector3 pos = m_modelController->GetMuzzlePos();
        DirectX::SimpleMath::Vector3 launchDir = m_modelController->GetWeaponDir();
        DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;

        m_fireControl->FireSelectedAmmo(pos, launchDir, velocity);
        m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
    }
}

void Vehicle::TestFireCannon()
{
    if (m_fireControl->GetIsCoolDownActive() == false)
    {
        DirectX::SimpleMath::Vector3 pos = m_heli.weaponPos;
        DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
        DirectX::SimpleMath::Vector3 launchDir = m_heli.weaponDirection;
        m_fireControl->FireProjectileCannon(pos, launchDir, velocity);
        
        m_isFiredTest = true;
        m_fireForceTest = -launchDir;
        m_fireForceTest.Normalize();
        const float fireForceMag = 1000000.0f;
        m_fireForceTest *= fireForceMag;
        /*
        m_testImpulseForce.currentTime = 0.0f;
        m_testImpulseForce.totalTime = 0.1f;
        m_testImpulseForce.currentMagnitude = 0.0f;
        m_testImpulseForce.maxMagnitude = 50000.0f;
        m_testImpulseForce.directionNorm = -launchDir;
        m_testImpulseForce.directionNorm.Normalize();
        m_testImpulseForce.isActive = true;     
        */
        m_testImpulseForce = m_fireControl->GetRecoilImpulseForce(-launchDir);
    }
}

void Vehicle::TestFireExplosive()
{
    DirectX::SimpleMath::Vector3 pos = m_heli.weaponPos;
    DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 launchDir = m_heli.weaponDirection;
    m_fireControl->FireProjectileExplosive(pos, launchDir, velocity);
}

void Vehicle::TestFireMirv()
{
    DirectX::SimpleMath::Vector3 pos = m_heli.weaponPos;
    DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 launchDir = m_heli.weaponDirection;
    m_fireControl->FireProjectileMirv(pos, launchDir, velocity);
}

void Vehicle::TestFireShotgun()
{
    DirectX::SimpleMath::Vector3 pos = m_heli.weaponPos;
    DirectX::SimpleMath::Vector3 velocity = m_heli.q.velocity;
    DirectX::SimpleMath::Vector3 launchDir = m_heli.weaponDirection;
    m_fireControl->FireProjectileShotGun(pos, launchDir, m_heli.right, velocity);
}


void Vehicle::SetTestPostImppactVelocity(DirectX::SimpleMath::Vector3 aPostImpactVelocity)
{
    m_heli.testPostImpactVelocity = aPostImpactVelocity;
}

void Vehicle::SetTestCollisionVelocityUpdate(DirectX::SimpleMath::Vector3 aCollisionVelocityUpdate)
{
    m_heli.testCollisionVelocityUpdate = aCollisionVelocityUpdate;
}

void Vehicle::SetTestCollisionImpulseForce(Utility::ImpulseForce aCollisionImpulseForce)
{
    m_heli.testCollisionImpulseForce = aCollisionImpulseForce;
}