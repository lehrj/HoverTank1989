#pragma once
#include "pch.h"
#include "Utility.h"
#include "Environment.h"
#include "FireControl.h"
#include "ModelController.h"

class FireControl;

struct ControlInput
{
    // input control data
    const float inputDeadZone = 0.001f;  // small deadzone to ignore nominal control input

    float       brakeInput = 0.0f;
    const float breakInputMax = 1.0f;
    const float breakInputMin = 0.0f;
    const float breakInputRate = 1.9f;
    bool        brakeIsPressed = false;
    DirectX::SimpleMath::Vector3 brakeForce = DirectX::SimpleMath::Vector3::Zero;

    float       collectiveInput;
    const float collectiveInputMax = 1.0f;
    const float collectiveInputMin = 0.0f;
    const float collectiveInputRate = 0.5f;

    DirectX::SimpleMath::Vector3 cyclicStick;
    const float cyclicDecayRate = 0.3f;
    float       cyclicInputPitch;
    bool        cyclicInputPitchIsPressed;
    float       cyclicInputRoll;
    bool        cyclicInputRollIsPressed;
    const float cyclicInputMax = Utility::ToRadians(20.0f);
    const float cyclicInputMin = -Utility::ToRadians(20.0f);
    const float cyclicInputRate = 0.1f;

    DirectX::SimpleMath::Vector3 cyclicNormLocal = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 cyclicNormWorld = DirectX::SimpleMath::Vector3::UnitY;

    float       jetInput;
    const float jetInputDecayRate = 0.6f;
    bool        jetInputIsPressed;
    const float jetInputMax = 1.0f;
    const float jetInputMin = 0.0f;
    const float jetInputRate = 0.4f;

    float       throttleInput;
    const float throttleInputMin = 0.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 0.4f;

    bool        yawPedalIsPressed;
    float       yawPedalInput;
    const float yawPedalDecayRate = 10.2f;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    const float yawPedalInputRate = 10.15f;

    float weaponPitch;
    const float weaponPitchInputRate = 0.7f;
    const float weaponPitchMax = Utility::ToRadians(20.0f);
    const float weaponPitchMin = Utility::ToRadians(-20.0f);

    float turretYaw;
    const float turretYawInputRate = 0.7f;
    const float turretYawMax = Utility::ToRadians(110.0f);
    const float turretYawMin = Utility::ToRadians(-110.0f);
};

struct Motion
{
    DirectX::SimpleMath::Vector3 airResistance;
    DirectX::SimpleMath::Vector3 bodyVelocity;
    DirectX::SimpleMath::Vector3 engineForce;

    DirectX::SimpleMath::Vector3 mainRotorForceNormal;
    float                        mainRotorForceMagnitude;
    DirectX::SimpleMath::Vector3 position;

    float                        tailRotorForceMagnitude;
    DirectX::SimpleMath::Vector3 tailRotorForceNormal;

    DirectX::SimpleMath::Vector3 totalVelocity;
    DirectX::SimpleMath::Vector3 velocity;

    Utility::Torque              bodyTorqueForce;
    float                        bodyTorqueMagnitude;
    DirectX::SimpleMath::Vector3 bodyTorqueVec;

    Utility::Torque              pendulumTorqueForceTest;

    DirectX::SimpleMath::Vector3 parabolicMomentum;

    DirectX::SimpleMath::Vector3 angPosVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularVelocityVec = DirectX::SimpleMath::Vector3::Zero;
};

struct Rotor
{
    int bladeCount;
    float angleBetweenBlades;
    float height;
    float inputPitchAngleMax;
    float inputPitchAngleMin;
    float length;
    float neutralAngle;
    float pitchAngleMax;
    float pitchAngleMin;
    float radius;
    float rpm;
    float rotorRotation;
    float width;

    struct RotorBlade
    {
        float pitchAngle;
        float cyclicAngle;
        float liftForcePerSecond;
        float liftMag;
        DirectX::SimpleMath::Vector3 liftNorm;
        float rotationAngle;
    };
    std::vector<RotorBlade> bladeVec;

    enum class BladeType
    {
        BLADETYPE_MAINROTOR,
        BLADETYPE_TAILROTOR,
    };
    BladeType bladeType;
};

struct HeliData
{
    float altitude = 0.0f;
    DirectX::SimpleMath::Vector3 buoyancyForce = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 hoverFloat = DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f);
    DirectX::SimpleMath::Vector3 hoverDriveNorm = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    float hoverDriveMag = 0.0f;
    const float hoverDriveMagMax = 100000.0f;
    const float brakeMagMax = 3000.0f;

    /*
    const float groundNormalForceRange = 5.0f;
    const float hoverNeutralBoyantAlt = 0.52f;
    const float hoverRangeLower = 0.5f;
    const float hoverRangeMid = 1.0f;
    const float hoverRangeUpper = 3.0f;
    */
    /*
    const float hoverRangeLower = 5.0f;
    const float hoverRangeMid = 8.5f;
    const float hoverRangeUpper = 12.0f;
    const float groundNormalForceRange = hoverRangeUpper;
    const float hoverNeutralBoyantAlt = hoverRangeMid;
    */
    /*
    const float hoverRangeLower = 1.0f;
    const float hoverRangeMid = 3.0f;
    const float hoverRangeUpper = 5.0f;
    */
    const float hoverRangeLower = 2.0f;
    const float hoverRangeMid = 4.0f;
    const float hoverRangeUpper = 8.0f;
    const float groundNormalForceRange = hoverRangeUpper + 5.0f;
    const float hoverNeutralBoyantAlt = hoverRangeMid;

    const float jetThrustMax = 5000.0f;
    // rotor data
    const float mainRotorForceMagMax = 15.0f;
    const float mainRotorForceMagMin = 0.0f;
    DirectX::SimpleMath::Vector3 mainRotorPos;
    DirectX::SimpleMath::Vector3 localMainRotorPos;

    DirectX::SimpleMath::Vector3 topPos;
    DirectX::SimpleMath::Vector3 localTopPos;
    DirectX::SimpleMath::Vector3 topTestPos;
    DirectX::SimpleMath::Vector3 localTopTestPos;
    DirectX::SimpleMath::Vector3 baseTestPos;
    DirectX::SimpleMath::Vector3 localBaseTestPos;

    DirectX::SimpleMath::Vector3 gravityTorqueArmPos;
    DirectX::SimpleMath::Vector3 localGravityTorqueArmPos;

    float       mainRotorRPM;
    const float mainRotorRPMmin = 0.0f;
    const float mainRotorRPMmax = 500.0f;
    DirectX::SimpleMath::Vector3 tailRotorPos;
    DirectX::SimpleMath::Vector3 localTailRotorPos;
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;

    DirectX::SimpleMath::Vector3 physicsPointFront;
    DirectX::SimpleMath::Vector3 localPhysicsPointFront;
    DirectX::SimpleMath::Vector3 physicsPointLeft;
    DirectX::SimpleMath::Vector3 localPhysicsPointLeft;
    DirectX::SimpleMath::Vector3 physicsPointRear;
    DirectX::SimpleMath::Vector3 localPhysicsPointRear;
    DirectX::SimpleMath::Vector3 physicsPointRight;
    DirectX::SimpleMath::Vector3 localPhysicsPointRight;

    DirectX::SimpleMath::Vector3 landingGearPos;
    DirectX::SimpleMath::Vector3 localLandingGearPos;

    DirectX::SimpleMath::Vector3 muzzlePos;
    DirectX::SimpleMath::Vector3 localMuzzlePos;
    DirectX::SimpleMath::Vector3 localizedMuzzlePos;
    DirectX::SimpleMath::Vector3 weaponPos;
    DirectX::SimpleMath::Vector3 localWeaponPos;
    DirectX::SimpleMath::Vector3 weaponDirection;
    DirectX::SimpleMath::Vector3 localWeaponDirection;

    DirectX::SimpleMath::Vector3 weaponTorqueArmPos;
    DirectX::SimpleMath::Vector3 localWeaponTorqueArmPos;

    //
    int     numEqns;
    double  time;
    float   area;
    float   airResistance;
    float   airDensity;

    float   dragCoefficient;
    DirectX::SimpleMath::Vector3 dimensions;
    DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.0f, -9.80665f, 0.0f);
    float   mainRotorMaxRPM;
    float   mass;
    float   massTest = 2000.0f;
    Motion  q;
    float   totalResistance;
    //
    float   speed;                   // speed vehicle is traveling
    bool    isVehicleAirborne;
    bool    isVehicleLanding;
    bool    isVelocityBackwards;
    DirectX::SimpleMath::Vector3 terrainNormal;
    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 up;
    DirectX::SimpleMath::Vector3 right;
    DirectX::SimpleMath::Matrix alignment;
    DirectX::SimpleMath::Matrix alignmentInverse;
    DirectX::SimpleMath::Matrix cameraOrientation;
    DirectX::SimpleMath::Matrix cameraOrientationPrevious;
    float   terrainHightAtPos;

    float   testAccel = 0.0;
    DirectX::SimpleMath::Vector3 testAccelVec = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Matrix inertiaMatrixTest;
    DirectX::SimpleMath::Matrix inverseInertiaMatrixTest;
    DirectX::SimpleMath::Matrix localInertiaMatrixTest;
    DirectX::SimpleMath::Matrix localInverseInertiaMatrixTest;

    DirectX::SimpleMath::Matrix cannonTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localCannonTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix inverseCannonTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix toUseTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix testTensor = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 cannonPos = DirectX::SimpleMath::Vector3(3.0f, 1.0f, 0.0f);

    DirectX::SimpleMath::Vector3 vehicleLinearForcesSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 vehicleAngularForcesSum = DirectX::SimpleMath::Vector3::Zero;

    DirectX::BoundingOrientedBox boundingBox;
    DirectX::SimpleMath::Vector3 testPostImpactVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testCollisionVelocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    Utility::ImpulseForce testCollisionImpulseForce;
    bool isVehicleCollisionTrue = false;

    DirectX::SimpleMath::Vector3 windVaningTorueForce = DirectX::SimpleMath::Vector3::Zero;

    ControlInput  controlInput;
    Rotor         mainRotor;
    Rotor         tailRotor;
};

class Vehicle
{
public:
    ~Vehicle();
    void CycleFireControlAmmo();
    void DebugToggle();
    void DebugToggle2();
    void DebugToggle3();

    void DebugInputVelocityZero();
    void DrawVehicleProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawVehicleProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    float GetAccel() const { return m_heli.testAccel; };
    DirectX::SimpleMath::Vector3 GetAccelVec() const { return m_heli.testAccelVec; };
    float GetAirSpeed() { return m_heli.q.velocity.Length(); };
    //float GetAltitude() const { return m_heli.q.position.y - m_heli.terrainHightAtPos - 2.85f; };
    float GetAltitude() const { return m_heli.q.position.y - m_heli.terrainHightAtPos; };

    DirectX::BoundingOrientedBox GetBoundingBox() const { return m_heli.boundingBox; };
    DirectX::SimpleMath::Vector3 GetCenterOfMassPos() const { return m_heli.centerOfMass; };
    float GetCollective() const { return m_heli.controlInput.collectiveInput; };
    DirectX::SimpleMath::Vector3 GetFollowPos() const;
    DirectX::SimpleMath::Vector3 GetForward() const { return m_heli.forward; };
    float GetGroundSpeed() { return m_heli.speed; };
    DirectX::SimpleMath::Vector3 GetHoverGravForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetAntiMassGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetDamperForce(const float aAltitude, const float aMass);

    DirectX::SimpleMath::Vector3 GetHoverLift(const DirectX::SimpleMath::Vector3 aLiftForce, const float aAltitude);
    DirectX::SimpleMath::Vector3 GetJetThrust(const DirectX::SimpleMath::Vector3 aForward, const float aInput, const float aThrustMax);
    float GetMass() const { return m_heli.mass; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetRPM() const { return m_heli.mainRotor.rpm; };
    DirectX::SimpleMath::Vector3 GetSlopeForce(const DirectX::SimpleMath::Vector3 aTerrainNorm, const float aAltitude, const float aGroundInteractionRange);
    float GetThrottle() { return m_heli.controlInput.throttleInput; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleRight() const { return m_heli.right; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };
    DirectX::SimpleMath::Matrix GetAlignment() const { return m_heli.alignment; };

    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };

    DirectX::SimpleMath::Matrix GetMuzzleDirMat() const { return m_modelController->GetMuzzleDirMat(); };
    DirectX::SimpleMath::Vector3 GetMuzzlePos() const { return m_heli.muzzlePos; };
    DirectX::SimpleMath::Vector3 GetLocalizedMuzzlePos() const { return m_heli.localizedMuzzlePos; };
    float GetTurretYaw() const { return m_heli.controlInput.turretYaw; };

    DirectX::SimpleMath::Matrix GetTensorTest() const { return m_heli.localInertiaMatrixTest; };
    DirectX::SimpleMath::Matrix GetTensorTest2() const { return m_heli.testTensor; };
    float GetWeaponPitch() const { return m_heli.controlInput.weaponPitch; };
    DirectX::SimpleMath::Vector3 GetWeaponDirection() const { return m_heli.weaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalDirection() const { return m_heli.localWeaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalPos() const { return m_heli.localWeaponPos; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_heli.weaponPos; };

    bool GetIsDebugToggled() const { return m_debugToggle; };
    bool GetIsDebugToggled2() const { return m_debugToggle2; };
    bool GetIsDebugToggled3() const { return m_debugToggle3; };

    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNPCController, std::shared_ptr<Vehicle> aVehicle);

    // helicopter functions
    void InputCollective(const float aCollectiveInput);
    void InputCyclicPitch(const float aPitchInput);
    void InputCyclicRoll(const float aRollInput);
    void InputDecay(const double aTimeDelta);
    void InputJet(const float aJetInput);
    void InputThrottle(const float aThrottleInput);
    void InputTurretYaw(const float aTurretYawInput);
    void InputYawPedal(const float aYawInput);
    void InputWeaponPitch(const float aPitchInput);

    void InputGamePadForward(const float aForwardInput);
    void InputGamePadStrafe(const float aStrafeInput);
    void InputGamePadTurn(const float aTurnInput);

    void Jump();

    void PassFireControl(std::shared_ptr<FireControl>& aFireControl);

    void ResetVehicle();

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetEnvironment(Environment* aEnviron);
    void SetModelController(std::shared_ptr<ModelController> aModelController);

    void UpdateVehicle(const double aTimeDelta);
    void UpdateVehicleFireControl(const double aTimeDelta);
    void TestFireCannon();
    void TestFireExplosive();
    void TestFireMirv();
    void TestFireShotgun();
    void FireWeapon();

    void SetTestPostImppactVelocity(DirectX::SimpleMath::Vector3 aPostImpactVelocity);
    void SetTestCollisionVelocityUpdate(DirectX::SimpleMath::Vector3 aCollisionVelocityUpdate);
    void SetTestCollisionImpulseForce(Utility::ImpulseForce aCollisionImpulseForce);
    void SetTestVehicleCollisionTrue() { m_heli.isVehicleCollisionTrue = true; };

private:
    DirectX::SimpleMath::Vector3 CalculateBuoyancyForce(const HeliData& aVehicleData);
    DirectX::SimpleMath::Vector3 CalculateHoverDriveForce(const struct HeliData& aHeli);
    DirectX::SimpleMath::Vector3 CalculateHoverTorqueForce(const struct HeliData& aHeli, const float aTimeStep);
    float CalculateLiftCoefficient(const float aAngle);
    DirectX::SimpleMath::Vector3 CalculateDragAngular(const DirectX::SimpleMath::Vector3 aAngVelocity, DirectX::SimpleMath::Vector3 aTestVec);
    DirectX::SimpleMath::Vector3 CalculateDragLinear(const DirectX::SimpleMath::Vector3 aVelocity, const DirectX::SimpleMath::Vector3 aNewQVelocity, const float aSurfaceArea);
    DirectX::SimpleMath::Vector3 CalculateDragLinear2(const DirectX::SimpleMath::Vector3 aVelocity, const DirectX::SimpleMath::Vector3 aNewQVelocity);
    float CalculateWindVaningVal(const HeliData& aHeliData);
    DirectX::SimpleMath::Vector3 CalculateWindVaningTorqueForce(const HeliData& aHeliData);

    DirectX::SimpleMath::Vector3 CalculateImpactLinearForceSum(const float aTimeDelta);
    DirectX::SimpleMath::Vector3 CalculateImpactTorqueSum(const float aTimeDelta);
    DirectX::SimpleMath::Vector3 GetThrusterLiftMagnitude(const DirectX::SimpleMath::Vector3 aLiftForce, const DirectX::SimpleMath::Vector3 aPos);

    void InitializeFlightControls(ControlInput& aInput);
    void InitializeRotorBlades(HeliData& aHeliData);

    void LandVehicle();

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);

    void UpdateAlignmentTorque();
    void UpdateAlignmentCamera();
    void UpdateBladeLiftForce(const float aTimeStep);

    Utility::Torque UpdateBodyTorqueRunge(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep);

    void UpdateBrakeForce(const float aTimeStep);
    void UpdateCollisionImpulseForces(const float aTimeStep);
    void UpdateCyclicStick(ControlInput& aInput);
    void UpdateCyclicNorm();
    float UpdateGroundEffectForce(const float aLiftForce);
    void UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    void UpdatePhysicsPoints(struct HeliData& aVehicle);
    void UpdateResistance();
    DirectX::SimpleMath::Vector3 UpdateRotorForceRunge();
    void UpdateRotorData(HeliData& aHeliData, const double aTimer);
    void UpdateRotorPitch(HeliData& aHeliData);
    void UpdateRotorSpin(HeliData& aHeliData, const double aTimer);
    void UpdateTerrainNorm();
    void UpdateTerrainNormTorque();
    void UpdateTestDrivetrainTorque(const float aTimer);
    void UpdateTestDrivetrainTorque2(const float aTimer);
    void UpdateTestDrivetrainTorque3(const float aTimer);
    void UpdateTestDrivetrainTorque4(const float aTimer);

    void UpdateVehicleForces(const float aTimeStep);

    void UpdateTensor();

    std::shared_ptr<DebugData>      m_debugData;
    Environment const*              m_environment;
    std::shared_ptr<FireControl>    m_fireControl;
    std::shared_ptr<ModelController> m_modelController;

    HeliData                        m_heli;

    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_forward = DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector4    m_defaultRight = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector4    m_right = DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0);

    float                           m_moveBackForward = 0.0;
    float                           m_moveLeftRight = 0.0;
    float                           m_moveUpDown = 0.0;

    float m_rotorTimerTest = 0.0f;
    float m_rotorTimerTest2 = 0.0f;
    float m_testFPS = 0.0f;
    float m_testTimer = 0.0f;
    float m_testTimer2 = 0.0f;

    bool m_debugToggle = false;
    bool m_debugToggle2 = false;
    bool m_debugToggle3 = false;

    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos3 = DirectX::SimpleMath::Vector3::Zero;

    bool m_isFiredTest = false;
    DirectX::SimpleMath::Vector3 m_fireForceTest = DirectX::SimpleMath::Vector3::Zero;

    Utility::ImpulseForce m_testImpulseForce;
    Utility::Torque m_testTerrainNormTorque;

    Utility::Torque m_testDrivetrainTorque;
    Utility::Torque m_testDrivetrainTorqueLocal;

    Utility::Torque m_testGravForce;

    DirectX::SimpleMath::Vector3 m_sumOfForceTest = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_sumOfMomentsTest = DirectX::SimpleMath::Vector3::Zero;

    Utility::Torque m_bodyTorqueTest;

    bool m_isRungeOn = true;

    DirectX::SimpleMath::Vector3 m_testLatTorqueArm = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testLatTorqueForce = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 m_testLongTorqueArm = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testLongTorqueForce = DirectX::SimpleMath::Vector3::Zero;

    float m_testVal = 0.0f;
    float m_testVal2 = 0.0f;
    DirectX::SimpleMath::Vector3 m_testVec = DirectX::SimpleMath::Vector3::Zero;

    const float m_testMass = 1500.0f;
    const float m_testForceMod1 = 300.1f;
    const float m_testForceMod2 = 300.0f;
    
    
    const float m_inertiaModelX = 4.4f;
    const float m_inertiaModelY = 1.0f;
    const float m_inertiaModelZ = 3.0f;
    
    /*
    const float m_inertiaModelX = 8.0f;
    const float m_inertiaModelY = 2.0f;
    const float m_inertiaModelZ = 6.0f;
    */

    DirectX::SimpleMath::Vector3 m_hoverTorqueForceSum = DirectX::SimpleMath::Vector3::Zero;
};

