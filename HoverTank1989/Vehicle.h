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
    const float brakeInputMax = 10.0f;
    const float brakeInputMin = 0.0f;
    const float brakeInputRate = 1.9f;
    bool        isBrakePressed = false;
    DirectX::SimpleMath::Vector3 brakeForce = DirectX::SimpleMath::Vector3::Zero;

    float       collectiveInput;
    const float collectiveInputMax = 1.0f;
    const float collectiveInputMin = 0.0f;
    const float collectiveInputRate = 0.5f;

    DirectX::SimpleMath::Vector3 cyclicStick;
    const float cyclicDecayRate = 0.9f;
    float       cyclicInputPitch;
    bool        cyclicInputPitchIsPressed;
    float       cyclicInputRoll;
    bool        cyclicInputRollIsPressed;
    //const float cyclicInputMax = Utility::ToRadians(20.0f);
    //const float cyclicInputMin = -Utility::ToRadians(20.0f);
    const float cyclicInputMax = 1.0f;
    const float cyclicInputMin = -1.0f;
    const float cyclicInputRate = 0.7f;

    float cyclicInputRollRaw = 0.0f;
    float cyclicInputPitchRaw = 0.0f;
    const float cyclicInputRawMax = 1.0f;
    const float cyclicInputRawMin = -1.0f;
    DirectX::SimpleMath::Vector3 cyclicNormRaw = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 cyclicNormUpdated = DirectX::SimpleMath::Vector3::UnitY;
    //const float cyclicDecayRateRaw = 10.3f;
    //const float cyclicInputRateRaw = 10.1f;
    const float cyclicDecayRateRaw = 0.6f;
    const float cyclicInputRateRaw = 0.5f;

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
    //const float yawPedalDecayRate = 1.2f;
    const float yawPedalDecayRate = 100.0f;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    //const float yawPedalInputRate = 1.15f;
    //const float yawPedalInputRate = 0.5f;
    const float yawPedalInputRate = 1.0f;

    float weaponPitch;
    //const float weaponPitchInputRate = 0.7f;
    //const float weaponPitchInputRate = 0.2f;
    //const float weaponPitchMax = Utility::ToRadians(25.0f);
    //const float weaponPitchMin = Utility::ToRadians(-10.0f);
    const float weaponPitchInputRate = 0.4f;
    const float weaponPitchMax = Utility::ToRadians(110.0f);
    const float weaponPitchMin = Utility::ToRadians(-100.0f);

    float turretYaw;
    //const float turretYawInputRate = 0.7f;
    //const float turretYawInputRate = 0.3f;
    //const float turretYawMax = Utility::ToRadians(110.0f);
    //const float turretYawMin = Utility::ToRadians(-110.0f);
    const float turretYawInputRate = 0.4f;
    const float turretYawMax = Utility::ToRadians(360.0f);
    const float turretYawMin = Utility::ToRadians(-360.0f);
};

struct Motion
{
    DirectX::SimpleMath::Vector3 angularMomentum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Quaternion angularQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
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
    DirectX::SimpleMath::Plane groundPlane;
    DirectX::SimpleMath::Vector3 buoyancyForce = DirectX::SimpleMath::Vector3::Zero;
    const DirectX::SimpleMath::Vector3 hoverFloat = DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f);
    DirectX::SimpleMath::Vector3 hoverDriveNorm = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    float hoverDriveMag = 0.0f;
    const float hoverDriveMagMax = 20000.0f;
    const float brakeMagMax = 3000.0f;
    //const float yawForce = 300.0f;
    const float yawForce = 1900.0f;

    const float hoverRangeLower = 2.0f;
    const float hoverRangeMid = 4.0f;
    const float hoverRangeUpper = 8.0f;
    const float groundNormalForceRange = hoverRangeUpper + 5.0f;
    const float hoverNeutralBoyantAlt = hoverRangeMid;

    const float jetThrustMax = 5000.0f;
    // rotor data
    float       mainRotorRPM;
    const float mainRotorRPMmin = 0.0f;
    const float mainRotorRPMmax = 500.0f;

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

    DirectX::SimpleMath::Vector3 buoyancyTorqueArmPos;
    DirectX::SimpleMath::Vector3 localBuoyancyTorqueArmPos;

    DirectX::SimpleMath::Vector3 tailRotorPos;
    DirectX::SimpleMath::Vector3 localTailRotorPos;
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;

    DirectX::SimpleMath::Vector3 testCenterOfMass = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testLocalCenterOfMass = DirectX::SimpleMath::Vector3::Zero;

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

    DirectX::SimpleMath::Vector3 localMissileTubeLeftPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f);
    DirectX::SimpleMath::Vector3 localMissileTubeRightPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 worldMissileTubeLeftPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightPos = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 localMissileTubeLeftDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 localMissileTubeRightDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 worldMissileTubeLeftDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightDir = DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Vector3 worldMissileTubeLeftUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightUp = DirectX::SimpleMath::Vector3::UnitY;
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
    const float   mass = 1700.0f;
    //float   massTest = 2000.0f;

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
    DirectX::SimpleMath::Quaternion alignmentQuat;
    DirectX::SimpleMath::Quaternion alignmentInverseQuat;

    DirectX::SimpleMath::Quaternion alignmentQuatDelta = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion alignmentInverseQuatDelta = DirectX::SimpleMath::Quaternion::Identity;
    float angularRadsPerSec = 0.0f;

    DirectX::SimpleMath::Matrix cameraOrientation;
    DirectX::SimpleMath::Matrix cameraOrientationPrevious;
    float   terrainHightAtPos;

    float   testAccel = 0.0;
    DirectX::SimpleMath::Vector3 testAccelVec = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Matrix inertiaMatrixTest;
    DirectX::SimpleMath::Matrix inverseInertiaMatrixTest;
    DirectX::SimpleMath::Matrix localInertiaMatrixTest;
    DirectX::SimpleMath::Matrix localInverseInertiaMatrixTest;

    DirectX::SimpleMath::Matrix ballastInertiaMatrix;
    DirectX::SimpleMath::Matrix inverseBallastInertiaMatrix;
    DirectX::SimpleMath::Matrix localBallastInertiaMatrix;
    DirectX::SimpleMath::Matrix localInverseBallastInertiaMatrix;
    const DirectX::SimpleMath::Vector3 ballastInertiaTranslationBase = DirectX::SimpleMath::Vector3(0.0f, -5.0f, 0.0f);
    DirectX::SimpleMath::Vector3 ballastInertiaTranslation = DirectX::SimpleMath::Vector3(0.0f, -5.0f, 0.0f);
    float ballastMass = 1.0f;

    DirectX::SimpleMath::Vector3 vehicleLinearForcesSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 vehicleAngularForcesSum = DirectX::SimpleMath::Vector3::Zero;

    DirectX::BoundingOrientedBox boundingBox;
    DirectX::SimpleMath::Vector3 testPostImpactVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testCollisionVelocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    Utility::ImpulseForce testCollisionImpulseForce;
    bool isVehicleCollisionTrue = false;

    DirectX::SimpleMath::Vector3 windVaningTorueForce = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 selfRightingForce = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 angularDrag = DirectX::SimpleMath::Vector3::Zero;

    float stabilizedWeaponPitch = 0.0f;

    ControlInput  controlInput;
    Rotor         mainRotor;
    Rotor         tailRotor;

    std::vector<Utility::ImpulseForce> impulseForceVec;
    DirectX::SimpleMath::Vector3 impulseForceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque              impulseTorqueSum;
};

class Vehicle
{
public:
    ~Vehicle();
    void CycleFireControlAmmo();
    void DebugToggle();
    void DebugToggle2();
    void DebugToggle3();
    void DebugToggle4();
    void DebugToggle5();
    void DebugToggle6();
    void DebugToggle7();
    void DebugToggle8();
    void DebugToggle9();
    void DebugToggle0();

    void DebugInputVelocityZero();
    void DrawVehicleProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawVehicleProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void FireWeapon();
    void FireWeapon(std::shared_ptr<DirectX::SoundEffectInstance> aFireFx);
    //void FireWeapon(std::shared_ptr<SoundFx> aFireFx);
    void FireWeapon(std::shared_ptr<Utility::SoundFx> aFireFx);
    void FireWeapon(std::shared_ptr<Utility::SoundFx> aFireFx, std::shared_ptr<Utility::SoundFx> aRocketFx);
    void FireWeapon(std::shared_ptr<Utility::SoundFx> aExplosionFx, std::shared_ptr<Utility::SoundFx> aFireFx, std::shared_ptr<Utility::SoundFx> aRocketFx);
    
    float GetAccel() const { return m_heli.testAccel; };
    DirectX::SimpleMath::Vector3 GetAccelVec() const { return m_heli.testAccelVec; };
    
    DirectX::SimpleMath::Vector3 GetAngularVelocity() const { return m_heli.q.angularVelocity; };
    DirectX::SimpleMath::Quaternion GetAngularQuatStep() const { return m_angQuatStep; };

    float GetAirSpeed() { return m_heli.q.velocity.Length(); };
    DirectX::SimpleMath::Matrix GetAlignment() const { return m_heli.alignment; };
    DirectX::SimpleMath::Matrix GetAlignmentInverse() const { return m_heli.alignmentInverse; };
    float GetAltitude() const { return m_heli.q.position.y - m_heli.terrainHightAtPos; };
    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetAntiMassGravityForce(const float aAltitude, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::BoundingOrientedBox GetBoundingBox() const { return m_heli.boundingBox; };
    DirectX::SimpleMath::Vector3 GetCenterOfMassPos() const { return m_heli.centerOfMass; };
    float GetCollective() const { return m_heli.controlInput.collectiveInput; };
    DirectX::SimpleMath::Vector3 GetDamperForce(const float aAltitude, const float aMass);
    DirectX::SimpleMath::Vector3 GetFollowPos() const;
    DirectX::SimpleMath::Vector3 GetForward() const { return m_heli.forward; };
    DirectX::SimpleMath::Plane GetGroundPlane() const { return m_heli.groundPlane; };
    float GetGroundSpeed() { return m_heli.speed; };
    DirectX::SimpleMath::Vector3 GetHoverGravForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetHoverLift(const DirectX::SimpleMath::Vector3 aLiftForce, const float aAltitude);
    bool GetIsDebugToggled() const { return m_debugToggle; };
    bool GetIsDebugToggled2() const { return m_debugToggle2; };
    bool GetIsDebugToggled3() const { return m_debugToggle3; };
    DirectX::SimpleMath::Vector3 GetJetThrust(const DirectX::SimpleMath::Vector3 aForward, const float aInput, const float aThrustMax);
    float GetMass() const { return m_heli.mass; };

    DirectX::SimpleMath::Vector3 GetMissleTubeDirLeft() const { return m_heli.worldMissileTubeLeftDir; };
    DirectX::SimpleMath::Vector3 GetMissleTubeDirRight() const { return m_heli.worldMissileTubeRightDir; };
    DirectX::SimpleMath::Vector3 GetMissleTubePosLeft() const { return m_heli.worldMissileTubeLeftPos; };
    DirectX::SimpleMath::Vector3 GetMissleTubePosRight() const { return m_heli.worldMissileTubeRightPos; };

    DirectX::SimpleMath::Vector3 GetMissleTubePosLeftLocalized() const { return m_modelController->GetLocalizedTubeLeftPos(); };
    DirectX::SimpleMath::Vector3 GetMissleTubePosRightLocalized() const { return m_modelController->GetLocalizedTubeRightPos(); };

    DirectX::SimpleMath::Matrix GetMissileTubeMatLeft() const { return m_modelController->GetLocalizedTubeMatLeft(); };
    DirectX::SimpleMath::Matrix GetMissileTubeMatRight() const { return m_modelController->GetLocalizedTubeMatRight(); };

    DirectX::SimpleMath::Vector3 GetMissleTubeUpLeft() const { return m_heli.worldMissileTubeLeftUp; };
    DirectX::SimpleMath::Vector3 GetMissleTubeUpRight() const { return m_heli.worldMissileTubeRightUp; };

    DirectX::SimpleMath::Matrix GetMuzzleDirMat() const { return m_modelController->GetMuzzleDirMat(); };
    DirectX::SimpleMath::Vector3 GetMuzzlePos() const { return m_heli.muzzlePos; };
    DirectX::SimpleMath::Vector3 GetLocalizedMuzzlePos() const { return m_heli.localizedMuzzlePos; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetRPM() const { return m_heli.mainRotor.rpm; };
    DirectX::SimpleMath::Vector4 GetRearGlowColor();
    DirectX::SimpleMath::Vector3 GetSlopeForce(const DirectX::SimpleMath::Vector3 aTerrainNorm, const float aAltitude, const float aGroundInteractionRange);
    float GetThrottle() { return m_heli.controlInput.throttleInput; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleRight() const { return m_heli.right; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
    DirectX::SimpleMath::Matrix GetTargetingMatrix() const { return m_modelController->GetTargetingMatrix(); };
    DirectX::SimpleMath::Matrix GetTensorTest() const { return m_heli.localInertiaMatrixTest; };
    float GetTurretYaw() const { return m_heli.controlInput.turretYaw; };
    //float GetWeaponPitch() const { return m_heli.controlInput.weaponPitch; };
    float GetWeaponPitch() const { return m_heli.stabilizedWeaponPitch; };
    DirectX::SimpleMath::Vector3 GetWeaponDirection() const { return m_heli.weaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalDirection() const { return m_heli.localWeaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalPos() const { return m_heli.localWeaponPos; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_heli.weaponPos; };

    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNPCController, std::shared_ptr<Vehicle> aVehicle);

    // helicopter functions
    void InputCollective(const float aCollectiveInput);
    void InputCyclicPitch(const float aPitchInput);
    void InputCyclicPitchNew(const float aPitchInput);
    void InputCyclicRoll(const float aRollInput);
    void InputCyclicRollNew(const float aRollInput);
    void InputCyclicRoll2(const float aRollInput);
    void InputCyclicRoll3(const float aRollInput);
    void InputDecay(const double aTimeDelta);
    void InputDecayNew(const double aTimeDelta);
    void InputJet(const float aJetInput);
    void InputThrottle(const float aThrottleInput);
    void InputTurretYaw(const float aTurretYawInput);
    void InputYawPedal(const float aYawInput);
    void InputWeaponPitch(const float aPitchInput);

    void InputGamePadForward(const float aForwardInput);
    void InputGamePadStrafe(const float aStrafeInput);
    void InputGamePadStrafe2(const float aStrafeInput);
    void InputGamePadTurn(const float aTurnInput);

    void InputWeaponPitchToZero(const float aTimeStep);

    void Jump();

    void PassFireControl(std::shared_ptr<FireControl>& aFireControl);

    void ResetVehicle();

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetEnvironment(Environment* aEnviron);
    void SetModelController(std::shared_ptr<ModelController> aModelController);
    void SetTestPostImpactVelocity(DirectX::SimpleMath::Vector3 aPostImpactVelocity);
    void SetTestCollisionVelocityUpdate(DirectX::SimpleMath::Vector3 aCollisionVelocityUpdate);
    void SetTestCollisionImpulseForce(Utility::ImpulseForce aCollisionImpulseForce);
    void SetTestVehicleCollisionTrue() { m_heli.isVehicleCollisionTrue = true; };
    void ToggleFireControlLaser();
    void UpdateVehicle(const double aTimeDelta);
    void UpdateVehicleFireControl(const double aTimeDelta);

private:
    DirectX::SimpleMath::Vector3 CalculateBuoyancyForce(const HeliData& aVehicleData);
    DirectX::SimpleMath::Vector3 CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity, const float aTimeStep);
    DirectX::SimpleMath::Vector3 CalculateDragLinear(const DirectX::SimpleMath::Vector3 aVelocity);
    void CalculateGroundImpactForce(DirectX::SimpleMath::Vector3& aForce, DirectX::SimpleMath::Vector3& aTorque);
    DirectX::SimpleMath::Vector3 CalculateHoverDriveForce(const struct HeliData& aHeli);
    DirectX::SimpleMath::Vector3 CalculateHoverTorqueForce(const struct HeliData& aHeli, const float aTimeStep);
    DirectX::SimpleMath::Vector3 CalculateImpactLinearForceSum(const float aTimeDelta);
    DirectX::SimpleMath::Vector3 CalculateImpactTorqueSum(const float aTimeDelta);
    float CalculateLiftCoefficient(const float aAngle);
    DirectX::SimpleMath::Vector3 CalculateSelfRightingForce(const HeliData& aHeliData);
    DirectX::SimpleMath::Vector3 CalculateStabilityTorqueLocal(const HeliData& aHeliData, const float aTimeStep);
    float CalculateWindVaningVal(const HeliData& aHeliData);
    DirectX::SimpleMath::Vector3 CalculateWindVaningTorqueForce(const HeliData& aHeliData);

    DirectX::SimpleMath::Vector3 GetThrusterLiftMagnitude(const DirectX::SimpleMath::Vector3 aLiftForce, const DirectX::SimpleMath::Vector3 aPos);

    void InitializeFlightControls(ControlInput& aInput);
    void InitializeRotorBlades(HeliData& aHeliData);

    void LandVehicle();

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RightHandSide2(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);

    void StabilizeWeaponPitch(struct HeliData& aVehicle, const float aTimeStep);
    DirectX::SimpleMath::Vector3 TestStabilityForce(const float aTimeStep);
    DirectX::SimpleMath::Vector3 TestStabilityForce2(const DirectX::SimpleMath::Vector3 aVec, const float aTimeStep);
    DirectX::SimpleMath::Vector3 TestStabilityForce3(const DirectX::SimpleMath::Vector3 aVec, const float aTimeStep);

    void UpdateAlignment(const float aTimeDelta);
    void UpdateAlignmentCamera();
    void UpdateBladeLiftForce(const float aTimeStep);
    DirectX::SimpleMath::Vector3 UpdateBodyTorqueLocalNew(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep);
    DirectX::SimpleMath::Vector3 UpdateBodyTorqueLocal(DirectX::SimpleMath::Vector3& aAccelVec, Utility::Torque aPendTorque, const float aTimeStep);
    void UpdateBrakeForce(const float aTimeStep);
    void UpdateCollisionImpulseForces(const float aTimeStep);
    void UpdateCyclicData();
    void UpdateCyclicStick(ControlInput& aInput);
    void UpdateCyclicNorm(); 
    float UpdateGroundEffectForce(const float aLiftForce);
    void UpdateImpulseForces(struct HeliData& aVehicle, const float aTimeDelta);
    void UpdateInertiaTensor(struct HeliData& aVehicle, const float aTimeStep);
    void UpdateModelColorVals(const float aTimeStep);
    void UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    DirectX::SimpleMath::Vector3 UpdatePendulumMotion2(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    void UpdatePhysicsPoints(struct HeliData& aVehicle, const float aTimeStep);
    void UpdateResistance();
    DirectX::SimpleMath::Vector3 UpdateRotorForceRunge();
    void UpdateRotorData(HeliData& aHeliData, const double aTimer);
    void UpdateRotorPitch(HeliData& aHeliData);
    void UpdateRotorSpin(HeliData& aHeliData, const double aTimer);
    void UpdateTerrainData();
    void UpdateTerrainNorm();
    void UpdateTerrainNormTorqueNew();
    void UpdateTerrainNormTorque();
    void UpdateTerrainNormTorque2();
    void UpdateTestDrivetrainTorqueLocalNew(const float aTimer);
    void UpdateTestDrivetrainTorqueLocal(const float aTimer);
    void UpdateTestDrivetrainTorqueLocal2(const float aTimer);
    void UpdateVehicleForces(const float aTimeStep);

    void TestDrawFireDirection();

    std::shared_ptr<DebugData>      m_debugData;
    Environment const* m_environment;
    std::shared_ptr<FireControl>    m_fireControl;
    std::shared_ptr<ModelController> m_modelController;

    HeliData                        m_heli;

    float m_testTimer = 0.0f;
    float m_testTimer2 = 0.0f;
    float m_testTimer3 = 0.0f;

    bool m_debugToggle = false;
    bool m_debugToggle2 = false;
    bool m_debugToggle3 = false;
    bool m_debugToggle4 = false;
    bool m_debugToggle5 = false;
    bool m_debugToggle6 = false;
    bool m_debugToggle7 = false;
    bool m_debugToggle8 = false;
    bool m_debugToggle9 = false;
    bool m_debugToggle0 = false;

    DirectX::SimpleMath::Vector3 m_hoverTorqueForceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::ImpulseForce m_testImpulseForce;
    Utility::Torque m_testTerrainNormTorque;

    Utility::Torque m_testDrivetrainTorque;
    Utility::Torque m_testDrivetrainTorqueLocal;

    Utility::Torque m_testGravForce;

    const float m_inertiaModelX = 4.4f;
    const float m_inertiaModelY = 1.0f;
    const float m_inertiaModelZ = 3.0f;

    /*
    const float m_testMass = 800.1f;
    const float m_driveTorqueForceMax = 300.0f;
    const float m_stabilityTorqueForceMax = 33000.0f;
    const float m_stabilityTorqueForceMax2 = 200.0f;
    const float m_terrainTorqueForceMax = 1800.0f;
    const float m_gravStabilityArmMod = 3.0f;
    */

    const float m_testMass = 1700.0f;
    //const float m_driveTorqueForceMax = 1300.0f;
    //const float m_driveTorqueForceMax = 2300.0f;
    const float m_driveTorqueForceMax = 2300.0f;
    //const float m_driveTorqueForceMax = 7300.0f;
    //const float m_stabilityTorqueForceMax = 33000.0f;
    const float m_stabilityTorqueForceMax = 800.0f;
    const float m_stabilityTorqueForceMax2 = 1200.0f;
    //const float m_terrainTorqueForceMax = 3800.0f;
    const float m_terrainTorqueForceMax = 1400.0f;
    //const float m_terrainTorqueForceMax = 2700.0f;
    const float m_gravStabilityArmMod = 1.0f;

    const float m_gravTorqueModTest = 15.0f;

    const float m_angularDampConst = 0.9f;
    const float m_testAngDampConst = 0.1f;

    const float m_angDragCoefficient = 0.3f;
    const float m_airDensityDragMod = 1.0f;
    const float m_angularDragMod = 0.8f;

    //DirectX::SimpleMath::Vector3 m_testTorqueLocal = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Vector3 m_testTorqueWorld = DirectX::SimpleMath::Vector3::Zero;

    //DirectX::SimpleMath::Matrix m_testInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    //DirectX::SimpleMath::Matrix m_testInverseInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;

    float m_testAngularRotationPerSecond = 0.0f;

    float m_testVal = 0.0f;

    const float m_testMOI = 1000.0f;
    const float m_testTorque = 1000.0f;

    DirectX::SimpleMath::Quaternion m_angQuatStep = DirectX::SimpleMath::Quaternion::Identity;

    //const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(-400.0f, 9.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(400.0f, 9.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(-400.0f, 8.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(-500.0f, 8.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_startPos = DirectX::SimpleMath::Vector3(0.0f, 8.0f, 0.0f);
};

