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

    return driveForce;
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
    m_heli.airDensity = m_environment->GetAirDensity();
    m_heli.dragCoefficient = 1.05f;

    m_heli.airResistance = 0.0f;
    m_heli.totalResistance = m_heli.airResistance;
    m_heli.time = 0.0;

    m_heli.forward = DirectX::SimpleMath::Vector3::UnitX;
    m_heli.up = DirectX::SimpleMath::Vector3::UnitY;
    m_heli.right = m_heli.forward.Cross(m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_heli.forward, m_heli.up);
    m_heli.alignment = DirectX::SimpleMath::Matrix::Identity;
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
    m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3(0.0f, -2.174999997f, 0.00000000f);
    //m_heli.localMainRotorPos = DirectX::SimpleMath::Vector3(0.0f, 2.174999997f, 0.00000000f);

    m_heli.mainRotorPos = m_heli.localMainRotorPos;
    m_heli.localCenterOfMass = DirectX::SimpleMath::Vector3::Zero;
    //m_heli.localCenterOfMass = DirectX::SimpleMath::Vector3(0.0, 0.5f, 0.0f);
    m_heli.centerOfMass = m_heli.localCenterOfMass;
    m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3(-9.25000000, 0.00000000, 0.00000000);
    //m_heli.localTailRotorPos = DirectX::SimpleMath::Vector3(-4.25000000, 0.00000000, 0.00000000);
    m_heli.tailRotorPos = m_heli.localTailRotorPos;

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
    newQ.angularVelocity = aQ->angularVelocity + static_cast<float>(aQScale) * aDeltaQ->angularVelocity;
    newQ.angularPosition = aQ->angularPosition + static_cast<float>(aQScale) * aDeltaQ->angularPosition;

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

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    if (m_testImpulseForce.isActive == true)
    {
        velocityUpdate += m_testImpulseForce.directionNorm * m_testImpulseForce.currentMagnitude;
    }

    velocityUpdate += CalculateHoverDriveForce(m_heli);

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

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    DirectX::SimpleMath::Vector3 tempVel = DirectX::SimpleMath::Vector3::Zero;
    UpdatePendulumMotion(pendTorque, velocityUpdate, static_cast<float>(aTimeDelta));
    //UpdatePendulumMotion(pendTorque, tempVel, static_cast<float>(aTimeDelta));

    Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueRunge(pendTorque, static_cast<float>(aTimeDelta));

    //  Assign right-hand side values.
    aDQ->airResistance = airResistance;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / m_heli.mass);
    aDQ->totalVelocity = velocityUpdate;
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
    aDQ->bodyTorqueForce = bodyTorqueUpdate;

    DirectX::SimpleMath::Quaternion angularAcceleration = DirectX::SimpleMath::Quaternion::Identity;
    if (bodyTorqueUpdate.axis.Length() > 0.4f)
    {
        angularAcceleration = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(bodyTorqueUpdate.axis, bodyTorqueUpdate.magnitude * 1.0f);

        float angAirDensity = m_environment->GetAirDensity();
        //float angDragCoefficient = aHeli->dragCoefficient;
        float angDragCoefficient = 0.3f;
        float angFrontSurfaceArea = aHeli->area;
        float angVelocity = newQ.angularVelocity.Length();
        float angFrontDragResistance = 0.5f * angAirDensity * angFrontSurfaceArea * angDragCoefficient * angVelocity * angVelocity;
        DirectX::SimpleMath::Quaternion angVelocityNorm = m_heli.q.angularVelocity;
        angVelocityNorm.Normalize();
        DirectX::SimpleMath::Quaternion angAirResistance = angVelocityNorm * (-angFrontDragResistance);

        angularAcceleration += angAirResistance;
    }
    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * (angularAcceleration);
    aDQ->angularPosition = static_cast<float>(aTimeDelta) * newQ.angularVelocity;
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
    DirectX::SimpleMath::Vector3 bodyVelocityyUpdate = (dq1.bodyVelocity + 2.0 * dq2.bodyVelocity + 2.0 * dq3.bodyVelocity + dq4.bodyVelocity) / numEqns;
    DirectX::SimpleMath::Vector3 engineVelocityUpdate = (dq1.engineForce + 2.0 * dq2.engineForce + 2.0 * dq3.engineForce + dq4.engineForce) / numEqns;
    DirectX::SimpleMath::Vector3 airResistnaceVelocityUpdate = (dq1.airResistance + 2.0 * dq2.airResistance + 2.0 * dq3.airResistance + dq4.airResistance) / numEqns;
    DirectX::SimpleMath::Vector3 totalVelocityUpdate = (dq1.totalVelocity + 2.0 * dq2.totalVelocity + 2.0 * dq3.totalVelocity + dq4.totalVelocity) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;



    DirectX::SimpleMath::Quaternion angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) * (1.0f / numEqns);
    DirectX::SimpleMath::Quaternion angularPositionUpdate = (dq1.angularPosition + 2.0 * dq2.angularPosition + 2.0 * dq3.angularPosition + dq4.angularPosition) * (1.0f / numEqns);


    if (q.position.y + posUpdate.y <= aHeli->terrainHightAtPos - aHeli->localLandingGearPos.y)
    {
        if (q.velocity.y + velocityUpdate.y <= aHeli->gravity.y * aTimeDelta)
        {
            /*
            aHeli->q.position.y = aHeli->terrainHightAtPos + - aHeli->localLandingGearPos.y;
            q.position.y = aHeli->terrainHightAtPos + -aHeli->localLandingGearPos.y - posUpdate.y;
            bodyTorqueUpdate.magnitude = 0.0f;
            q.bodyTorqueForce.magnitude = 0.0f;
            */
        }
    }

    q.velocity += velocityUpdate;
    q.engineForce += engineVelocityUpdate;
    q.airResistance += airResistnaceVelocityUpdate;
    //q.gravityForce += gravityVelocityUpdate;
    q.totalVelocity += velocityUpdate;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;
    q.position += posUpdate;

    q.angularPosition += angularPositionUpdate;
    q.angularVelocity += angularVelocityUpdate;



    aHeli->q.velocity = q.velocity;
    aHeli->q.position = q.position;
    aHeli->q.engineForce = q.engineForce;
    aHeli->q.bodyTorqueForce = q.bodyTorqueForce;
    aHeli->q.angularPosition = q.angularPosition;
    aHeli->q.angularVelocity = q.angularVelocity;

    DirectX::SimpleMath::Vector3 angVelVec = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 angPosVec = DirectX::SimpleMath::Vector3::UnitY;

    angVelVec = DirectX::SimpleMath::Vector3::Transform(angVelVec, aHeli->q.angularVelocity);
    angPosVec = DirectX::SimpleMath::Vector3::Transform(angPosVec, aHeli->q.angularPosition);

    //m_debugData->DebugPushTestLine(m_heli.q.position, angVelVec, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_heli.q.position, angPosVec, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    if (aHeli->q.position.y < aHeli->terrainHightAtPos)
    {
        //aHeli->q.position.y = aHeli->terrainHightAtPos + 1.9f;
    }
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
        DirectX::SimpleMath::Matrix testRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularVelocity);
        m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocity);

        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }

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

    return slopeForceUpdate;
}

Utility::Torque Vehicle::UpdateBodyTorqueRunge(Utility::Torque aPendTorque, const float aTimeStep)
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
    DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;

    const float modVal = 0.01f;
    DirectX::SimpleMath::Vector3 mainRotorForce = m_heli.q.mainRotorForceNormal * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = m_heli.controlInput.cyclicStick * (m_heli.q.mainRotorForceMagnitude * m_heli.controlInput.collectiveInput) * modVal;
    mainRotorForce = UpdateRotorForceRunge();
    mainRotorForce *= ((m_heli.mainRotor.bladeVec[0].liftForcePerSecond + m_heli.mainRotor.bladeVec[1].liftForcePerSecond) / m_heli.mass) * modVal;

    //const float windVaning = CalculateWindVaningVal(m_heli);
    //DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput + windVaning) * modVal;
    DirectX::SimpleMath::Vector3 tailForce = -m_heli.right * (m_heli.controlInput.yawPedalInput) * modVal;

    DirectX::SimpleMath::Vector3 gravityForce = (m_heli.gravity) * modVal;
    gravityForce = m_heli.gravity;
    gravityForce.Normalize();
    gravityForce = gravityForce * (4.8 * modVal);


    if (m_debugToggle == true)
    {
        m_debugToggle = false;
    }
    Utility::Torque rotorTorque = Utility::GetTorqueForce(mainRotorTorqueArm, mainRotorForce);
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);

    DirectX::SimpleMath::Vector3 torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testTerrainNormTorque.axis * m_testTerrainNormTorque.magnitude);
    //torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testTerrainNormTorque.axis * m_testTerrainNormTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);
    //torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testTerrainNormTorque.axis * m_testTerrainNormTorque.magnitude) + (aPendTorque.axis * aPendTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);
    torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (aPendTorque.axis * aPendTorque.magnitude);
    
    torqueAxis = (tailTorque.axis * tailTorque.magnitude) + (aPendTorque.axis * aPendTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);

    //torqueAxis = (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (m_testDrivetrainTorque.axis * m_testDrivetrainTorque.magnitude);

    torqueAxis.Normalize();
    float torqueMag = rotorTorque.magnitude + tailTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testTerrainNormTorque.magnitude;
    //torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testTerrainNormTorque.magnitude + m_testDrivetrainTorque.magnitude;
    //torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testTerrainNormTorque.magnitude + aPendTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + m_testDrivetrainTorque.magnitude;
    torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + aPendTorque.magnitude;

    torqueMag = tailTorque.magnitude + aPendTorque.magnitude + m_testDrivetrainTorque.magnitude;

    //torqueMag = tailTorque.magnitude + gravTorque.magnitude + m_testDrivetrainTorque.magnitude;

    if (m_testImpulseForce.isActive == true)
    {
        DirectX::SimpleMath::Vector3 weaponTorqueArm = m_heli.weaponPos - centerMassPos;
        DirectX::SimpleMath::Vector3 weaponForce = -m_heli.weaponDirection;
        weaponForce.Normalize();
        float weaponForceMag = 0.2f;
        weaponForceMag = m_testImpulseForce.currentMagnitude * 0.000001f;
        weaponForce *= weaponForceMag;
        Utility::Torque weaponTorque = Utility::GetTorqueForce(weaponTorqueArm, weaponForce);

        torqueAxis = (rotorTorque.axis * rotorTorque.magnitude) + (tailTorque.axis * tailTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude)
            + (weaponTorque.axis * weaponTorque.magnitude);
        torqueAxis.Normalize();
        torqueMag = rotorTorque.magnitude + tailTorque.magnitude + gravTorque.magnitude + weaponTorque.magnitude;
    }
    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.magnitude = torqueMag;

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
    m_debugData->DebugPushUILineDecimalNumber("accelerationMag =", accelerationMag, "");
    //m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    // only update with viable data and if not aligned with gravity direction
    if (accelerationMag > 0.00001f && accelerationMag < 20.0f)
    {
        accelerationDirection *= accelerationMag * aTimeStep;
        accelerationDirection *= accelerationMag;
        m_debugData->DebugPushTestLine(m_heli.q.position, accelerationDirection, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
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
    aVehicle.tailRotorPos = m_heli.localTailRotorPos;
    aVehicle.tailRotorPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localTailRotorPos, updateMat);

    aVehicle.landingGearPos = aVehicle.localLandingGearPos;
    aVehicle.landingGearPos = DirectX::SimpleMath::Vector3::Transform(aVehicle.localLandingGearPos, updateMat);

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

void Vehicle::UpdateTerrainNorm()
{
    m_heli.terrainNormal = m_environment->GetTerrainNormal(m_heli.q.position);
}

void Vehicle::UpdateTerrainNormTorque()
{
    DirectX::SimpleMath::Vector3 slopeForceUpdate;
    Utility::Torque prevTorque = m_testTerrainNormTorque;
    Utility::Torque updateTorque;
    if (m_heli.altitude > m_heli.groundNormalForceRange)
    {
        slopeForceUpdate = DirectX::SimpleMath::Vector3::Zero;
        const float forcePercentage = 1.0f;
        const DirectX::SimpleMath::Vector3 terrrainForce = DirectX::SimpleMath::Vector3::UnitY * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * m_heli.mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= 0.00001f;
        updateTorque.axis = DirectX::SimpleMath::Vector3::Zero;
        updateTorque.magnitude = 0.0f;
    }
    else
    {
        const float forcePercentage = 1.0f - (m_heli.altitude / m_heli.groundNormalForceRange);
        const DirectX::SimpleMath::Vector3 terrrainForce = m_heli.terrainNormal * (-m_heli.gravity.y);
        slopeForceUpdate = (terrrainForce * forcePercentage) * m_heli.mass;
        DirectX::SimpleMath::Vector3 torqueArm = m_heli.centerOfMass - m_heli.mainRotorPos;
        updateTorque = Utility::GetTorqueForce(torqueArm, slopeForceUpdate);
        updateTorque.magnitude *= 0.00001f;
    }
    m_testTerrainNormTorque.axis = updateTorque.axis;
    m_testTerrainNormTorque.magnitude = updateTorque.magnitude;
}

void Vehicle::UpdateTestDrivetrainTorque(const float aTimer)
{
    //m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputPitch = ", m_heli.controlInput.cyclicInputPitch, "");
    //m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputRoll = ", m_heli.controlInput.cyclicInputRoll, "");

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
        updatedCyclic = DirectX::SimpleMath::Vector3::Transform(m_heli.up, cyclicQuat);
    }
   
    //m_debugData->DebugPushTestLine(m_heli.q.position, updatedCyclic, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));



    //const float torqueMax = 25.0f * aTimer;
    const float torqueMax = 0.3f;
    Utility::Torque lateralTorque;
    lateralTorque.axis = DirectX::SimpleMath::Vector3::UnitX;
    lateralTorque.axis = m_heli.forward;
    lateralTorque.magnitude = -m_heli.controlInput.cyclicInputRoll * torqueMax;
    Utility::Torque longitudinalTorque;
    longitudinalTorque.axis = DirectX::SimpleMath::Vector3::UnitZ;
    longitudinalTorque.axis = m_heli.right;
    longitudinalTorque.magnitude = -m_heli.controlInput.cyclicInputPitch * torqueMax;

    Utility::Torque torqueSum;

    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    //torqueSum.axis = DirectX::SimpleMath::Vector3::TransformNormal(torqueSum.axis, m_heli.alignment);
    m_testDrivetrainTorque = torqueSum;

    //m_debugData->DebugPushUILineDecimalNumber("m_testDrivetrainTorque.magnitude = ", m_testDrivetrainTorque.magnitude, "");
    //m_debugData->DebugPushUILineDecimalNumber("torqueMax = ", torqueMax, "");

    //m_debugData->DebugPushTestLine(m_heli.q.position, m_testDrivetrainTorque.axis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    DirectX::SimpleMath::Vector3 centerMassPos = m_heli.centerOfMass;
    DirectX::SimpleMath::Vector3 rotorPos = m_heli.mainRotorPos;
    DirectX::SimpleMath::Vector3 torqueArm = rotorPos - centerMassPos;
    //DirectX::SimpleMath::Vector3 torqueArm = centerMassPos - rotorPos;
    DirectX::SimpleMath::Vector3 lateralForce = (m_heli.controlInput.cyclicInputRoll * torqueMax) * m_heli.right;
    DirectX::SimpleMath::Vector3 longitudinalForce = (-m_heli.controlInput.cyclicInputPitch * torqueMax) * m_heli.forward;
    lateralTorque = Utility::GetTorqueForce(torqueArm, lateralForce);
    longitudinalTorque = Utility::GetTorqueForce(torqueArm, longitudinalForce);

    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;
    

    //torqueSum.magnitude *= 0.3f;
    float testDot = updatedCyclic.Dot(DirectX::SimpleMath::Vector3::UnitY);
    //m_debugData->DebugPushUILineDecimalNumber("testDot = -> ", testDot, "");
    float bodyAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, m_heli.up);
    cyclicAngle = Utility::GetAngleBetweenVectors(DirectX::SimpleMath::Vector3::UnitY, updatedCyclic);
    float maxBodyRoll = Utility::ToRadians(75.0f);
    if (bodyAngle > maxBodyRoll)
    {
        //torqueSum.magnitude = 0.0f;
        //torqueSum.magnitude *= testDot;
    }
    else
    {
        float ratio = bodyAngle / maxBodyRoll;
        float invRatio = 1.0f - ratio;
        //torqueSum.magnitude *= invRatio;
        //torqueSum.magnitude *= testDot;
    }

    float testMod = 0.0f;
    if (cyclicAngle > bodyAngle)
    {
        torqueSum.magnitude *= testDot;
        testMod = testDot;
    }
    else
    {
        float ratio = bodyAngle / maxBodyRoll;
        float invRatio = 1.0f - ratio;
        torqueSum.magnitude *= invRatio;

        //torqueSum.magnitude *= testDot;
        //testMod = invRatio;
    }
    //m_debugData->DebugPushUILineDecimalNumber("testMod = -> ", testMod, "");
    //m_debugData->DebugPushUILineDecimalNumber("torqueSum.magnitude = -> ", torqueSum.magnitude, "");
    //torqueSum.axis = DirectX::SimpleMath::Vector3::TransformNormal(torqueSum.axis, m_heli.alignment);
    m_testDrivetrainTorque = torqueSum;
}

void Vehicle::UpdateTestDrivetrainTorque2(const float aTimer)
{
    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputPitch = ", m_heli.controlInput.cyclicInputPitch, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.controlInput.cyclicInputRoll = ", m_heli.controlInput.cyclicInputRoll, "");

    //const float torqueMax = 25.0f * aTimer;
    const float torqueMax = 0.3f;
    Utility::Torque lateralTorque;
    lateralTorque.axis = DirectX::SimpleMath::Vector3::UnitX;
    lateralTorque.axis = m_heli.forward;
    lateralTorque.magnitude = -m_heli.controlInput.cyclicInputRoll * torqueMax;
    Utility::Torque longitudinalTorque;
    longitudinalTorque.axis = DirectX::SimpleMath::Vector3::UnitZ;
    longitudinalTorque.axis = m_heli.right;
    longitudinalTorque.magnitude = -m_heli.controlInput.cyclicInputPitch * torqueMax;

    Utility::Torque torqueSum;

    torqueSum.axis = (lateralTorque.axis * lateralTorque.magnitude) + (longitudinalTorque.axis * longitudinalTorque.magnitude);
    torqueSum.axis.Normalize();
    torqueSum.magnitude = lateralTorque.magnitude + longitudinalTorque.magnitude;

    //torqueSum.axis = DirectX::SimpleMath::Vector3::TransformNormal(torqueSum.axis, m_heli.alignment);
    m_testDrivetrainTorque = torqueSum;

    m_debugData->DebugPushUILineDecimalNumber("m_testDrivetrainTorque.magnitude = ", m_testDrivetrainTorque.magnitude, "");
    m_debugData->DebugPushUILineDecimalNumber("torqueMax = ", torqueMax, "");

    m_debugData->DebugPushTestLine(m_heli.q.position, m_testDrivetrainTorque.axis, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Vehicle::UpdateVehicle(const double aTimeDelta)
{
    UpdatePhysicsPoints(m_heli);
    UpdateTestDrivetrainTorque(static_cast<float>(aTimeDelta));

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
    m_heli.buoyancyForce = CalculateBuoyancyForce(m_heli);
    UpdateBrakeForce(static_cast<float>(aTimeDelta));
    UpdateTerrainNormTorque();
    Utility::UpdateImpulseForceBellCurve(m_testImpulseForce, static_cast<float>(aTimeDelta));
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
    speed.y = 0.0f;
    m_heli.speed = speed.Length();
    m_debugData->DebugPushUILineDecimalNumber("Speed = ", m_heli.speed, "");
    m_debugData->DebugPushUILineDecimalNumber("MPH = ", m_heli.speed * 2.237f, "");
    InputDecay(aTimeDelta);

    m_heli.testAccel = (m_heli.q.velocity.Length() - prevVelocity.Length()) / static_cast<float>(aTimeDelta);
    m_heli.testAccelVec = (m_heli.q.velocity - prevVelocity) / static_cast<float>(aTimeDelta);

    UpdateResistance();

    /*
    m_debugData->DebugClearUI();
    m_debugData->DebugPushTestLine(m_heli.q.position, m_heli.forward, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.q.position, m_heli.up, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.q.position, m_heli.right, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.q.position, -m_heli.right, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.q.position, -m_heli.forward, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_debugData->DebugPushTestLine(m_heli.centerOfMass, m_heli.forward, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.centerOfMass, m_heli.up, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_heli.centerOfMass, m_heli.right, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.position.x = ", m_heli.q.position.x, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.position.y = ", m_heli.q.position.y, "");
    m_debugData->DebugPushUILineDecimalNumber("m_heli.q.position.z = ", m_heli.q.position.z, "");
    m_debugData->DebugClearUI();
    m_debugData->DebugPushTestLinePositionIndicator(m_heli.tailRotorPos, 8.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLinePositionIndicator(m_heli.mainRotorPos, 18.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    */


    /*
    m_debugData->DebugPushTestLinePositionIndicator(DirectX::SimpleMath::Vector3::Zero, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLinePositionIndicator(m_heli.q.position, 8.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_rotorTimerTest += static_cast<float>(aTimeDelta);

    m_testTimer += static_cast<float>(aTimeDelta);
    m_debugData->DebugPushUILineDecimalNumber("m_testTimer = ", m_testTimer, "");
    DirectX::SimpleMath::Vector3 yawPitchRoll(0.0f, 0.0f, 0.0f);
    //yawPitchRoll.x = Utility::ToRadians(m_testTimer);
    yawPitchRoll.x = m_testTimer;
    //m_heli.q.angularVelocity = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yawPitchRoll);

    DirectX::SimpleMath::Matrix testRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_heli.q.angularVelocity);
    //m_heli.alignment = DirectX::SimpleMath::Matrix::Transform(m_heli.alignment, m_heli.q.angularVelocity);

    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawPitchRoll);

    m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
    m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
    m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

    m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    */

    UpdateAlignmentTorque();
    UpdateAlignmentCamera();

    m_isFiredTest = false;
}

void Vehicle::UpdateVehicleFireControl(const double aTimeDelta)
{
    m_fireControl->UpdateFireControl(aTimeDelta);
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
