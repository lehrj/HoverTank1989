#pragma once
#include "pch.h"
#include "Utility.h"
#include "Environment.h"
#include "FireControl.h"
#include "ModelController.h"

struct ControlInput
{
    // input control data
    const float inputDeadZone = 0.001;  // small deadzone to ignore nominal control input

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
    const float yawPedalDecayRate = 0.2f;
    const float yawPedalInputMax = 1.0f;
    const float yawPedalInputMin = -1.0f;
    const float yawPedalInputRate = 0.15f;

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
    // test data for new controls
    const DirectX::SimpleMath::Vector3 hoverFloat = DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f);
    DirectX::SimpleMath::Vector3 hoverDriveNorm = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    float hoverDriveMag = 0.0f;
    const float hoverDriveMagMax = 100000.0f;

    const float groundNormalForceRange = 5.0f;
    const float hoverNeutralBoyantAlt = 0.52f;
    const float hoverRangeLower = 0.5f;
    const float hoverRangeMid = 1.0f;
    const float hoverRangeUpper = 3.0f;
    const float jetThrustMax = 5000.0f;
    // rotor data
    const float mainRotorForceMagMax = 15.0f;
    const float mainRotorForceMagMin = 0.0f;
    DirectX::SimpleMath::Vector3 mainRotorPos;
    DirectX::SimpleMath::Vector3 localMainRotorPos;
    float       mainRotorRPM;
    const float mainRotorRPMmin = 0.0f;
    const float mainRotorRPMmax = 500.0f;
    DirectX::SimpleMath::Vector3 tailRotorPos;
    DirectX::SimpleMath::Vector3 localTailRotorPos;
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;
    DirectX::SimpleMath::Vector3 landingGearPos;
    DirectX::SimpleMath::Vector3 localLandingGearPos;

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
    const DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.0f, -9.80665f, 0.0f);
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
    DirectX::SimpleMath::Matrix cameraOrientation;
    DirectX::SimpleMath::Matrix cameraOrientationPrevious;
    float   terrainHightAtPos;

    float   testAccel = 0.0;
    DirectX::SimpleMath::Vector3 testAccelVec = DirectX::SimpleMath::Vector3::Zero;

    ControlInput  controlInput;
    Rotor         mainRotor;
    Rotor         tailRotor;
};

class Vehicle
{
public: 
    ~Vehicle();
    void DebugToggle();

    void DebugInputVelocityZero();
    void DrawVehicleProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    float GetAccel() const { return m_heli.testAccel; };
    DirectX::SimpleMath::Vector3 GetAccelVec() const { return m_heli.testAccelVec; };
    float GetAirSpeed() { return m_heli.q.velocity.Length(); };
    float GetAltitude() const { return m_heli.q.position.y - m_heli.terrainHightAtPos - 2.85f; };
    float GetCollective() const { return m_heli.controlInput.collectiveInput; };
    DirectX::SimpleMath::Vector3 GetFollowPos() const;
    DirectX::SimpleMath::Vector3 GetForward() const { return m_heli.forward; };
    float GetGroundSpeed() { return m_heli.speed; };
    DirectX::SimpleMath::Vector3 GetHoverGravForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetAntiMassGravityForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);
    DirectX::SimpleMath::Vector3 GetDamperForce(const float aAltitude, const float aGroundInteractionRange, const DirectX::SimpleMath::Vector3 aGravity, const float aMass);

    DirectX::SimpleMath::Vector3 GetHoverLift(const DirectX::SimpleMath::Vector3 aLiftForce, const float aAltitude);
    DirectX::SimpleMath::Vector3 GetJetThrust(const DirectX::SimpleMath::Vector3 aForward, const float aInput, const float aThrustMax);
    DirectX::SimpleMath::Vector3 GetPos() const { return m_heli.q.position; };
    float GetRPM() const { return m_heli.mainRotor.rpm; };
    DirectX::SimpleMath::Vector3 GetSlopeForce(const DirectX::SimpleMath::Vector3 aTerrainNorm, const float aAltitude, const float aGroundInteractionRange);
    float GetThrottle() { return m_heli.controlInput.throttleInput; };
    double GetTime() { return m_heli.time; };
    DirectX::SimpleMath::Vector3 GetVehicleUp() const { return m_heli.up; };
    DirectX::SimpleMath::Matrix GetVehicleOrientation() const { return m_heli.cameraOrientation; };
    DirectX::SimpleMath::Matrix GetAlignment() const { return m_heli.alignment; };

    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_heli.q.velocity; };
  
    float GetTurretYaw() const { return m_heli.controlInput.turretYaw; };
    float GetWeaponPitch() const { return m_heli.controlInput.weaponPitch; };
    DirectX::SimpleMath::Vector3 GetWeaponDirection() const { return m_heli.weaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalDirection() const { return m_heli.localWeaponDirection; };
    DirectX::SimpleMath::Vector3 GetWeaponLocalPos() const { return m_heli.localWeaponPos; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_heli.weaponPos; };
    

    //void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCController* aNPCController);
    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNPCController);

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
    
    void Jump();

    void ResetVehicle();

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetEnvironment(Environment* aEnviron);
    void SetModelController(std::shared_ptr<ModelController> aModelController);

    void UpdateVehicle(const double aTimeDelta);

    void TestFire();
    void TestFire2();
private:

    DirectX::SimpleMath::Vector3 CalcHoverDriveForce(const struct HeliData& aHeli);

    float CalculateLiftCoefficient(const float aAngle);

    void InitializeFlightControls(ControlInput& aInput);

    void InitializeRotorBlades(HeliData& aHeliData);

    void LandVehicle();

    void RightHandSide(struct HeliData* aHeli, Motion* aQ, Motion* aDeltaQ, double aTimeDelta, float aQScale, Motion* aDQ);
    void RungeKutta4(struct HeliData* aHeli, double aTimeDelta);
    
    void UpdateAlignmentTorque();

    void UpdateBladeLiftForce(const float aTimeStep);

    Utility::Torque UpdateBodyTorqueRunge(Utility::Torque aPendulumTorque, const float aTimeStep);
    void UpdateAlignmentCamera();
    void UpdateCyclicStick(ControlInput& aInput);
    float UpdateGroundEffectForce(const float aLiftForce);

    void UpdatePendulumMotion(Utility::Torque& aTorque, DirectX::SimpleMath::Vector3& aVelocity, const float aTimeStep);
    void UpdatePhysicsPoints(struct HeliData& aHeli);
    void UpdateResistance();
    DirectX::SimpleMath::Vector3 UpdateRotorForceRunge();

    void UpdateRotorData(HeliData& aHeliData, const double aTimer);
    void UpdateRotorPitch(HeliData& aHeliData, const double aTimer);
    void UpdateRotorSpin(HeliData& aHeliData, const double aTimer);

    void UpdateTerrainNorm();

    float WindVaningVal(const HeliData& aHeliData, const float aTimeStep);
    
    Environment const*              m_environment;
    HeliData                        m_heli;

    FireControl*                    m_fireControl;
    std::shared_ptr<DebugData>      m_debugData;

    std::shared_ptr<ModelController>      m_modelController;

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
    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testPos3 = DirectX::SimpleMath::Vector3::Zero;

    bool m_isFiredTest = false;
    DirectX::SimpleMath::Vector3 m_fireForceTest = DirectX::SimpleMath::Vector3::Zero;

    Utility::ImpulseForce m_testImpulseForce;
};

