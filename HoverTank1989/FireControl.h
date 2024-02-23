#pragma once
#include "Environment.h"
#include "NPCController.h"
#include "Utility.h"
#include "Vehicle.h"

class Vehicle;

enum class AmmoType
{
    AMMOTYPE_CANNON,
    AMMOTYPE_EXPLOSIVE,
    AMMOTYPE_MIRV,
    AMMOTYPE_MACHINEGUN,
    AMMOTYPE_SHOTGUN,
    AMMOTYPE_GUIDEDMISSILE,
};

struct LauncherData
{
    DirectX::SimpleMath::Vector3 launchDirectionNorm;
    DirectX::SimpleMath::Vector3 launcherPosition;
    float reloadCoolDown;
    float coolDownTimer;
};

struct ProjectileMotion
{
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
    //DirectX::SimpleMath::Vector3 angularMomentum = DirectX::SimpleMath::Vector3::Zero;
    //DirectX::SimpleMath::Vector3 angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularMomentum;
    DirectX::SimpleMath::Vector3 angularVelocity;
};

struct ProjectileModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    projectileShape;
    DirectX::SimpleMath::Matrix projectileMatrix;
    DirectX::SimpleMath::Matrix localProjectileMatrix;
};

struct AmmoData
{
    AmmoType ammoType;
    float   baseDamage;
    float   cooldown;
    float   dragCoefficient;
    float   impactDurration;
    float   impactModifier;
    float   launchVelocity;
    float   length;
    float   mass;
    float   frontSurfaceArea;
    float   radius;
    int tickDownCounter;  // value to tick down as collisions occure, to be used for penetration and ricochet 

    bool isGuided = false;
};

enum class FlightState
{
    FLIGHTSTATE_ATTACK,
    FLIGHTSTATE_CLIMBOUT,
    FLIGHTSTATE_CRUISE,
    FLIGHTSTATE_DEBUG,
    FLIGHTSTATE_EXPLODING,
    FLIGHTSTATE_LAUNCH,
};

struct Seeker
{
    DirectX::SimpleMath::Vector3 targetDest;
    DirectX::SimpleMath::Vector3 targetPos;
    DirectX::SimpleMath::Vector3 targetVel;
    float targetDistance;
    float targetDistanceDelta;
    float closureRate;

    DirectX::SimpleMath::Matrix laserAlignment;
};

struct Controller
{
    DirectX::SimpleMath::Vector3 prevVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion prevQuat = DirectX::SimpleMath::Quaternion::Identity;
    float prevControlVal = 0.0f;

    DirectX::SimpleMath::Vector3 deltaVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion deltaQuat = DirectX::SimpleMath::Quaternion::Identity;
    float deltaControlVal = 0.0f;

    DirectX::SimpleMath::Vector3 currentVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion currentQuat = DirectX::SimpleMath::Quaternion::Identity;
    float currentControlVal = 0.0f;

    DirectX::SimpleMath::Vector3 vec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion quat = DirectX::SimpleMath::Quaternion::Identity;
    float controlVal = 0.0f;

    DirectX::SimpleMath::Vector3 prevVecConed = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion prevQuatConed = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 prevVecRaw = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion prevQuatRaw = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 currentVecConed = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion currentQuatConed = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 currentVecRaw = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion currentQuatRaw = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 deltaVecRaw = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion deltaQuatRaw = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 deltaVecConed = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion deltaQuatConed = DirectX::SimpleMath::Quaternion::Identity;
};

enum class MissileType
{
    TYPE_BASE,
    TYPE_BLUE,
    TYPE_ORANGE,
    TYPE_CANARDCONTROL,
    TYPE_TAILCONTRTROL,
    TYPE_THRUSTVECCONTROL,
};

enum class AeroType
{
    AERO_CLASSIC,
    AERO_HIGHAOA,
    AERO_STABILITYMAX,
};

enum class ControlInputType
{
    INPUT_CANARD,
    INPUT_TAILFIN,
    INPUT_THRUSTVEC,
};

enum class FinType
{
    CANARD_PITCH,
    CANARD_YAW,
    MAIN_PITCH,
    MAIN_YAW,
    TAIL_PITCH,
    TAIL_YAW,
};

struct FinDataDynamic
{
    DirectX::SimpleMath::Matrix aeroTensor = DirectX::SimpleMath::Matrix::Identity;
    float controlInput = 0.0f;
    DirectX::SimpleMath::Vector3 liftForce = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 dragForce = DirectX::SimpleMath::Vector3::Zero;
    float finAngle = Utility::ToRadians(0.0f);
    DirectX::SimpleMath::Vector3 finDir = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 chordLine = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 resultantForce = DirectX::SimpleMath::Vector3::Zero;
};

struct DynamicFinPackage
{
    FinDataDynamic canardPitch;
    FinDataDynamic canardYaw;

    FinDataDynamic mainPitch;
    FinDataDynamic mainYaw;

    FinDataDynamic tailPitch;
    FinDataDynamic tailYaw;
};

struct FinDataStatic
{
    FinType finType;

    DirectX::SimpleMath::Vector3 axis;
    DirectX::SimpleMath::Vector3 finNormal;
    DirectX::SimpleMath::Vector3 posLocal;

    float chord;
    float span;
    float thickness;

    float wingArea;
    float frontArea;

    float dragCoeffBase;
    float dragCoeefMod;

    bool isFinAngFixed;

    DirectX::SimpleMath::Matrix baseTensor;
    DirectX::SimpleMath::Matrix maxTensor;
    DirectX::SimpleMath::Matrix minTensor;
};

struct FinLibrary
{
    FinDataStatic canardPitch;
    FinDataStatic canardYaw;

    FinDataStatic mainPitch;
    FinDataStatic mainYaw;

    FinDataStatic tailPitch;
    FinDataStatic tailYaw;
};

struct NavData
{
    DirectX::SimpleMath::Vector3 vecToTargLocal = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion quatToTarg = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 targPosLocalized = DirectX::SimpleMath::Vector3::UnitX;
};

struct ControlData
{
    DirectX::SimpleMath::Vector3 tailFinVec = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion tailFinQuat = DirectX::SimpleMath::Quaternion::Identity;
    float finPitch = Utility::ToRadians(0.0f);
    float finYaw = Utility::ToRadians(0.0f);

    DirectX::SimpleMath::Vector3 thrustVecNorm = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion thrustVecQuat = DirectX::SimpleMath::Quaternion::Identity;
    float thrustPitch = Utility::ToRadians(0.0f);
    float thrustYaw = Utility::ToRadians(0.0f);
};

struct GuidanceSystem
{
    //Seeker seeker;
    Controller controller;
    FlightState flightStateCurrent = FlightState::FLIGHTSTATE_LAUNCH;
    MissileType type = MissileType::TYPE_BASE;
    AeroType testFinType = AeroType::AERO_CLASSIC;
    NavData nav;
    ControlData conDat;
    DynamicFinPackage finPak;

    float timeStepDelta = 0.0f;
    unsigned int uniqueId = 0;
    int targetID = 0;

    float altitude = 0.0f;
    float detonationRadius = 5.0f;
    
    bool isFinsDeployStarted = false;
    bool isFinsDeployEnd = false;
    bool isRocketFired = false;
    bool isTargetLocked = false;
    bool isRocketPlumeFlickerTrue = false;
    bool isSelfDestructTrue = false;
    bool isExplodingTrue = false;
    bool isTargetingLaserOn = false;
    float postExplosionDrawCountDown = 2.0f;

    float finDeployPercent = 0.0f;
    float finAngle1 = 0.0f;
    float finAngle2 = 0.0f;
    float finAngle3 = 0.0f;
    float finAngle4 = 0.0f;

    float canardPitchAng = 0.0f;
    float canardYawAng = 0.0f;
    float tailPitchAng = 0.0f;
    float tailYawAng = 0.0f;

    float throttlePercentage = 0.0f;
    float forwardThrust = 0.0f;

    DirectX::SimpleMath::Vector3 thrustPosWorldAligned = DirectX::SimpleMath::Vector3(-1.0f, 0.0, 0.0f);

    DirectX::SimpleMath::Vector3 liftForce = DirectX::SimpleMath::Vector3::Zero;
    float liftForceFloat = 0.0f;
    float airFoilDragMod = 0.0f;
    float liftCoefficient = 0.0f;
    float angleOfAttack = 0.0f;
    float dragSurfaceArea = 0.0f;
    float dragCoefficientFull = 0.0f;

    float plumeRotation = 0.0f;

    DirectX::SimpleMath::Vector3 gimbaleVec = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion gimbaleQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 airDragTorqueLocal = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 airDragForceLocalTest = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 airDragTorqueLocalTest = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 linearForceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 linearDragSum = DirectX::SimpleMath::Vector3::Zero;

    // heading, steering, targeting, & seeker data, needs to be streamlined
    DirectX::SimpleMath::Vector3 heading = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 headingLocal = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 targetDestination = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetPosition = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetForward = DirectX::SimpleMath::Vector3::UnitX;
    float targetDistance = 0.0f;
    float targetDistanceDelta = 0.0f;
    float closureRate = 0.0f;

    DirectX::SimpleMath::Vector3 steeringDirNormLocal = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion steeringQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Matrix targetLaserAlignment = DirectX::SimpleMath::Matrix::Identity;

    bool isFacingDestTrue = true;
    bool isVelocityForward = true;

    DirectX::SimpleMath::Quaternion angularStepQuat = DirectX::SimpleMath::Quaternion::Identity;

    float climbOutTimer = 0.0f;

    DirectX::SimpleMath::Vector3 centerOfPressureLocalPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 centerOfThrustLocalPos = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Matrix afterBurnPlumeSphereMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix afterBurnPlumeConeMat = DirectX::SimpleMath::Matrix::Identity;
    bool flickerBool = false;

    float thrustAngle = 0.0f;

    DirectX::SimpleMath::Vector3 losCurrent = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 losDelta = DirectX::SimpleMath::Vector3::Zero;
    float losRate = 0.0f;

    float closingVelocityToTarget = 0.0f;
    DirectX::SimpleMath::Vector3 latax = DirectX::SimpleMath::Vector3::Zero;
    float navigationTime = 0.0f;
    DirectX::SimpleMath::Vector3 pnAdjusted = DirectX::SimpleMath::Vector3::Zero;

    //DirectX::SimpleMath::Vector3 losNormTest = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 losNormTest = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 losDeltaTest = DirectX::SimpleMath::Vector3::Zero;

    float losAngTest = 0.0f;
    float losAngDeltaTest = 0.0f;

    DirectX::SimpleMath::Vector3 targVelNormToLos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targAccelNormalToLOS = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 localVel = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 uiTargPosRaw = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 uiTargPosMod = DirectX::SimpleMath::Vector3::Zero;

    bool isHardBurnModeTrue = false;
    DirectX::SimpleMath::Vector3 hardBurnVec = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion hardBurnQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 navVecTest = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion navQuatTest = DirectX::SimpleMath::Quaternion::Identity;

    float lataxNeeded = 0.0f;
    DirectX::SimpleMath::Vector3 lataxAxis = DirectX::SimpleMath::Vector3::UnitY;

};

struct AmmoStruct
{
    AmmoData ammoData;
    ProjectileModel ammoModel;
};

struct ProjectileData
{
    AmmoData ammoData;
    ProjectileMotion q;
    Utility::CollisionData  collisionData;
    bool isCollisionTrue;
    bool isDeleteTrue;
    float time;
    int liveTimeTick;
    bool isMidAirDeployAvailable = false;
    bool isFuseTriggered = false;
    float fuseTimer = 0.0f;
    GuidanceSystem guidance;

    DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion inverseAlignmentQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 angularForceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularDragSum = DirectX::SimpleMath::Vector3::Zero;
};

struct MissileModel
{
    // consts & colors
    // colors
    const DirectX::SimpleMath::Vector4 bodyColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 plumeColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 testColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 finColor1 = DirectX::SimpleMath::Vector4(0.8f, 0.8f, 0.8f, 1.0f);
    const DirectX::SimpleMath::Vector4 finColor1Alt = DirectX::SimpleMath::Vector4(1.f, 0.647058845f, 0.f, 1.f);
    const DirectX::SimpleMath::Vector4 finColor2 = DirectX::SimpleMath::Vector4(1.f, 0.647058845f, 0.f, 1.f);
    const DirectX::SimpleMath::Vector4 finColor2Alt = DirectX::SimpleMath::Vector4(0.6f, 0.6f, 0.6f, 1.0f);
    const DirectX::SimpleMath::Vector4 axelColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    const DirectX::SimpleMath::Vector4 voidBlackColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    const float tailFinDeployAngleMax = Utility::ToRadians(120.0f);
    const float wingFinDeployAngleMax = Utility::ToRadians(90.0f);

    std::unique_ptr<DirectX::GeometricPrimitive>    mainBodyShape;
    DirectX::SimpleMath::Matrix localBodyMatrix;
    DirectX::SimpleMath::Matrix worldBodyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    noseConeShape;
    DirectX::SimpleMath::Matrix localNoseConeMatrix;
    DirectX::SimpleMath::Matrix worldNoseConeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeShape;
    DirectX::SimpleMath::Matrix localPlumeMatrix;
    DirectX::SimpleMath::Matrix worldPlumeMatrix;
    DirectX::SimpleMath::Matrix plumeTranslation;
    DirectX::SimpleMath::Matrix plumeTranslationTest;
    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeBaseShape;
    DirectX::SimpleMath::Matrix localPlumeBaseMatrix;
    DirectX::SimpleMath::Matrix worldPlumeBaseMatrix;
    DirectX::SimpleMath::Matrix plumeBaseTranslation;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailFinShape;
    DirectX::SimpleMath::Matrix localTailFinMat;
    DirectX::SimpleMath::Matrix tailFinTransMat;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingFinShape;
    DirectX::SimpleMath::Matrix localWingFinMatrix;
    DirectX::SimpleMath::Matrix wingTranslation;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustRodShape;
    DirectX::SimpleMath::Vector3 thustRodLocalPos;


    std::unique_ptr<DirectX::GeometricPrimitive>    tailEndCapShape;
    DirectX::SimpleMath::Matrix localTailEndCapMatrix;
    DirectX::SimpleMath::Matrix worldTailEndCapMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    tailBlackVoidShape;
    DirectX::SimpleMath::Matrix localBlackVoidMatrix;
    DirectX::SimpleMath::Matrix worldBlackVoidMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustVectorMainShape;
    DirectX::SimpleMath::Matrix localThrustVectorMainMatrix;
    DirectX::SimpleMath::Matrix localThrustVectorMainMatrix1;
    DirectX::SimpleMath::Matrix thrustVectorTrans1;
    DirectX::SimpleMath::Matrix localThrustVectorMainMatrix2;
    DirectX::SimpleMath::Matrix thrustVectorTrans2;
    DirectX::SimpleMath::Matrix localThrustVectorMainMatrix3;
    DirectX::SimpleMath::Matrix thrustVectorTrans3;
    DirectX::SimpleMath::Matrix localThrustVectorMainMatrix4;
    DirectX::SimpleMath::Matrix thrustVectorTrans4;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustConnectingRodShape;
    DirectX::SimpleMath::Matrix localThrustConnectingRodMatrix2;
    DirectX::SimpleMath::Matrix thrustConnectingRodTrans2;

    DirectX::SimpleMath::Matrix localThrustConnectingRodMatrix3;
    DirectX::SimpleMath::Matrix thrustConnectingRodTrans3;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustVectorHousingShape;
    DirectX::SimpleMath::Matrix localThrustVectorHousingMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustVectorHousingEndCapShape;
    DirectX::SimpleMath::Matrix localThrustVectorHousingEndCapMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    thrustAxelShape;
    DirectX::SimpleMath::Matrix localThrustAxelMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    finAxelShape;
    DirectX::SimpleMath::Vector3 finAxelTranslation;

    DirectX::SimpleMath::Vector3 afterBurnPlumeBasePos;
    DirectX::SimpleMath::Vector3 afterBurnPlumeThrottleModPos;
    float afterBurnBaseSize;
    float afterBurnConeBaseLength;

    DirectX::SimpleMath::Matrix testWingMat;
    DirectX::SimpleMath::Matrix testWingTrans;
    std::unique_ptr<DirectX::GeometricPrimitive> testWingShape;

    // canards
    DirectX::SimpleMath::Matrix localCanardFinMat;
    DirectX::SimpleMath::Matrix canardFinTransMat;
    DirectX::SimpleMath::Vector3 canardAxelTranslation;
};

struct MissileData
{
    ProjectileData projectileData;
    GuidanceSystem guidance;
};

struct MissileStruct
{
    AmmoData ammoData;
    MissileModel modelData;
};

struct MissileConsts
{
    const float angularDragMod = 0.8f;

    const float detonationRange = 10.0f;

    const float finDeployDelay = 0.2f;
    const float rocketFireDelay = 1.0f;

    const float finDeployTime = 0.4f;
    const float rocketFireFullTime = 0.5f;
    const float wingArea = 0.3f;
    const float testFinArea = 0.3f;

    const float dragCoefficientDebug = 0.3f;
    const float dragAreaDebug        = 0.03f;

    const float dragCoefficient = 0.3f;
    const float dragCoefficientAng = 0.3f;
    const float postExplosionMass = 10.0f;
    const float postExplosionDragCoefficient = 0.5f;
    const float explosionDragCoefficientBase = 0.1f;
    const float explosionDragCoefficientAddMax = 0.1f;

    const float laserDepoyDelay = 3.9f;

    const DirectX::SimpleMath::Vector3 thrustPosLocal = DirectX::SimpleMath::Vector3(-0.5f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 thrustPosLocalOffset = DirectX::SimpleMath::Vector3(0.01f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureBasePosLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureFullFinDeployOffset = DirectX::SimpleMath::Vector3(-0.3f, 0.0, 0.0f);

    const DirectX::SimpleMath::Vector3 centerOfMassLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0, 0.0f);

    const float plumeRotationRate = 0.432f;

    const float headingRadiansPerSecondMax = 1.12f;
    const float steerAngMax = Utility::ToRadians(15.0f);
    const float steeringAngPerSecDeltaMax = Utility::ToRadians(80.0f);

    const float tailFinAngMax = Utility::ToRadians(25.0f);
    const float tailFinAngPerSecDeltaMax = Utility::ToRadians(110.0f);

    const float thrustVecAngMax = Utility::ToRadians(15.0f);
    const float thrustVecDeadZoneAng = Utility::ToRadians(1.0f);
    const float thrustVecAngPerSecDeltaMax = Utility::ToRadians(110.0f);

    const float rocketBoostForceMax = 125.0f;
    const float mass = 11.0f;
    //const float velMaxEst = (rocketBoostForceMax / mass) * 10.0f;
    const float velMaxEst = 10.0f;

    // flight modeling
    const float climbOutAltMin = 20.0f;
    const float cruiseAltMin = 100.0f;
    const float maxAlt = 200.0f;
    const float terminalRange = 100.0f;

    const float climbOutDuration = 0.5f;

    const float launchVelocity = 0.0f;

    const float testVal = 1.0f;
    const float testVal2 = testVal * 2.0f;

    const float navigationGain = 3.0f;

    const float hardBurnRadPerSec = Utility::ToRadians(25.0f);

    const DirectX::SimpleMath::Vector3 dimensions = DirectX::SimpleMath::Vector3(1.1f, 0.127f, 0.127f);

    const DirectX::SimpleMath::Vector3 testFinPosLocal = DirectX::SimpleMath::Vector3(-0.4f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 canardPosLocal = DirectX::SimpleMath::Vector3(0.4f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailPosLocal = DirectX::SimpleMath::Vector3(-dimensions.x * 0.45f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 mainWingPosLocal = DirectX::SimpleMath::Vector3(-0.2f, 0.0f, 0.0f);
    const AeroType testFinType = AeroType::AERO_CLASSIC;

    const float canardChord = ((dimensions.x * 0.3f) * 0.2f);
    const float canardSpan = dimensions.x * 0.3f;
    const float canardThickness = ((dimensions.x * 0.3f) * 0.2f) * 0.3f;

    const float mainChord = 0.06f;
    const float mainSpan = 0.3f;
    const float mainThickness = 0.015f;

    const float tailChord = ((dimensions.x * 0.3f) * 0.2f) * 2.0f;
    const float tailSpan = dimensions.x * 0.3f * 2.0f;
    const float tailThickness = ((dimensions.x * 0.3f) * 0.2f) * 0.3f;

    const bool useAdvancedMoiTensorTrue = false;
    const bool isMissileFreezeTrue = false;
    const bool isMissleTargetingLaserTrue = true;
    const bool isUseDebugRG4True = false;
    const bool isUseConstFinClTrue = false;
    const bool isManualControlTrue = true;
    const bool isThrustVecOn = true;
    const bool isDynamicFinOn = false;
    const bool isFinForceOn = false;
    const bool isBodyAeroOn = true;
    const bool isDebugSetPosToPlayerTrue = false;
};

enum class ExplosionType
{
    EXPLOSIONTYPE_DYNAMIC,
    EXPLOSIONTYPE_NONVEHICLE,
    EXPLOSIONTYPE_VEHICLESURFACE,
    EXPLOSIONTYPE_VEHICLEINTERNAL,
};

struct ExplosionData
{
    DirectX::BoundingSphere collisionSphere;
    DirectX::SimpleMath::Vector4 explosionCurrentColor;
    DirectX::SimpleMath::Vector4 explosionEndColor;
    DirectX::SimpleMath::Vector4 explosionStartColor;
    DirectX::SimpleMath::Vector4 color0;
    DirectX::SimpleMath::Vector4 color1;
    DirectX::SimpleMath::Vector4 color2;
    DirectX::SimpleMath::Vector4 color3;
    DirectX::SimpleMath::Vector4 color4;
    DirectX::SimpleMath::Vector4 color5;
    DirectX::SimpleMath::Vector4 color6;
    DirectX::SimpleMath::Vector4 color7;
    DirectX::SimpleMath::Vector4 color8;
    DirectX::SimpleMath::Matrix explosionMatrix0;
    DirectX::SimpleMath::Matrix explosionMatrix1;
    DirectX::SimpleMath::Matrix explosionMatrix2;
    DirectX::SimpleMath::Matrix explosionMatrix3;
    DirectX::SimpleMath::Matrix explosionMatrix4;
    DirectX::SimpleMath::Matrix explosionMatrix5;
    DirectX::SimpleMath::Matrix explosionMatrix6;
    DirectX::SimpleMath::Matrix explosionMatrix7;
    DirectX::SimpleMath::Matrix explosionMatrix8;
    DirectX::SimpleMath::Matrix localExplosionMatrix;
    DirectX::SimpleMath::Matrix rotationVariationMatrix;
    double currentDuration;
    float currentRadius;
    float initialRadius;
    bool isLifeTimeExpired;
    DirectX::SimpleMath::Vector3 localizedSurfaceExplosionPos = DirectX::SimpleMath::Vector3::Zero;
    int vehicleExplosionID = -1;
    float maxRadius;
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity = DirectX::SimpleMath::Vector3::Zero;
    ExplosionType explosionType;

    double totalDuration;

    DirectX::SimpleMath::Vector3 lightDir0 = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 lightDir1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 lightDir2 = DirectX::SimpleMath::Vector3::UnitZ;
};

struct ExplosionStruct
{
    ExplosionData explosionRefData;
    std::unique_ptr<DirectX::GeometricPrimitive> explosionShape;
    std::unique_ptr<DirectX::GeometricPrimitive> explosionShape1;
    std::vector<ExplosionData> explosionVec;
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, int>> explosionToPushVec;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureExplosion;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapExplosion;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularExplosion;

    const float internalVehicleExplosionRadiusMod = 1.5f;
    float maxExplosionForce;
    float maxExplosionImpactRadius;
};

struct MuzzleFlash
{
    DirectX::SimpleMath::Vector4 color1 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    DirectX::SimpleMath::Vector4 startColor = DirectX::SimpleMath::Vector4(0.7f, 0.0f, 0.0f, 1.3f);
    DirectX::SimpleMath::Vector4 endColor = DirectX::SimpleMath::Vector4(1.f, 0.4f, 0.f, 0.7f);
    DirectX::SimpleMath::Vector4 currentColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape2;
    DirectX::SimpleMath::Matrix worldMuzzleFlashConeMatrix;
    DirectX::SimpleMath::Matrix localMuzzleConeMatrix;
    DirectX::SimpleMath::Matrix worldTestMatrix;
    DirectX::SimpleMath::Matrix localTestMatrix;
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::UnitX;
    float flashDuration = 0.15f;
    float flashTimer = 0.0f;
    bool isFlashActive = false;
    float sizeMod = 0.0f;
    float growthRate = 20.0f;

    bool isFlickerTrue = false;
    float flickerScale = 0.5f;
    float baseConeHeight = 1.0f;
    float baseConeDiameter = 0.3f;
};

struct LaserModel
{
    // colors
    const DirectX::SimpleMath::Vector4 laserColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 laserColorLockTrue = DirectX::SimpleMath::Vector4(0.0f, 1.f, 0.f, 1.0f);
    const DirectX::SimpleMath::Vector4 testColor = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f);
    float distance = 0.0f;
    float flickerRot = 0.0f;
    const float flickerRate = 0.7f;

    bool isFlickerTrue = false;

    std::unique_ptr<DirectX::GeometricPrimitive>    laserShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    laserShape2;
    DirectX::SimpleMath::Matrix localBodyMatrix;
    DirectX::SimpleMath::Matrix worldBodyMatrix;
    DirectX::SimpleMath::Matrix translationMatrix;
};

class FireControl
{
public:
    void CycleLoadedAmmo();
    void DetonateAllMissiles();
    void DrawFireControlObjects(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawFireControlObjects2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aLaunchPos,
        const DirectX::SimpleMath::Vector3 aLaunchDirection,
        Environment const* aEnvironment, std::shared_ptr<Vehicle> aVehicle);

    void InitializeTextureMapsExplosion(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap);

    void FireMissile(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp, const float aTimeOffSet);

    void FireProjectileCannon(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMachineGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);

    void FireSelectedAmmo(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireDefaultProjectile(const AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    AmmoType GetCurrentAmmoType() const { return m_currentAmmoType; };
    bool GetIsMissileActiveTrue() const;
    void GetCameraMissileData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget) const;
    void GetCameraTrackAllData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget, DirectX::SimpleMath::Vector3& aUp) const;
    Utility::ImpulseForce GetRecoilImpulseForce(DirectX::SimpleMath::Vector3 aDirectionNorm);
    
    void GetUIData(DirectX::SimpleMath::Vector3& aPosRaw, DirectX::SimpleMath::Vector3& aPosMod);

    void PushVehicleExplosion(const DirectX::SimpleMath::Vector3 aPos, const int aVehicleId);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCController(std::shared_ptr<NPCController> aNPCController);

    void ToggleTargetingLaser();
    void TriggerMirvDeploy();
    void ToggleDebug1();
    void ToggleDebug2();
    void ToggleDebug3();

    void UpdateFireControl(double aTimeDelta);

    void ZeroMissileVelocities();

private:
    void ActivateMuzzleFlash(AmmoType aAmmoType);
    void AltitudeController(MissileData& aMissile, const float aTimeDelta);
    DirectX::SimpleMath::Vector3 CalculateBoostForceVec(MissileData& aMissile);
    void CalculateDragAngularSumLocal(MissileData& aMissile, const float aTimeDelta);
    DirectX::SimpleMath::Vector3 CalculateDragAngularSumLocal(MissileData& aMissile);
    DirectX::SimpleMath::Vector3 CalculateDragAngularSumLocalTest(MissileData& aMissile, const float aTimeDelta);
    void CalculateAirDragTorque(MissileData& aMissile, const float aTimeDelta);

    DirectX::SimpleMath::Vector3 CalculateDragLinearForRunge(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);

    DirectX::SimpleMath::Vector3 CalcDragLinearCurrent(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    DirectX::SimpleMath::Vector3 CalcDragLinearCurrentOld(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);

    DirectX::SimpleMath::Vector3 CalculateDragLinearForAccumulator(MissileData& aMissile);
    DirectX::SimpleMath::Vector3 CalculateDragLinearForAccumulatorOld(MissileData& aMissile);
    DirectX::SimpleMath::Vector3 CalculateDragLinearSum(MissileData& aMissile, const float aTimeDelta);

    float CalculateFinDragArea(const DirectX::SimpleMath::Vector3 aVelocityNormLocal, const DirectX::SimpleMath::Vector3 aFinDir, const FinDataStatic& aFinDat);

    float CalculateFinLiftCoef(const float aAngleOfAttack);
    float CalculateFinLiftCoefDebug(const float aAngleOfAttack);
    float CalculateFinLiftCoefFlat(const float aAngleOfAttack);
    float CalculateFinLiftCoefTest(const float aAngleOfAttack);


    void CalculateGimbaledThrust(MissileData& aMissile, const float aTimeDelta);
    DirectX::SimpleMath::Vector3 CalculateWindVaningTorqueForce(const MissileData& aMissile);

    void CastRayLaser();
    void ControllerUpdate(MissileData& aMissile, const float aTimeDelta);
    void CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aVelocity, ExplosionType aExplosionType, const int aVehicleId);
    void CheckCollisions();
    void CheckCollisionsMissile();
    
    void DebugMissileFunc(MissileData* aMissile);
    void DebugGraphCurveData(MissileData& aMissile, const float aTimeDelta);
    void DebugSetMissileToPlayerPos(MissileData& aMissile);

    void DeleteMissileFromVec(const unsigned int aIndex);
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void DeployMirv(ProjectileData& aProjectile);

    void DrawExplosions(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawExplosions2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawMissiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawMuzzleFlash(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawMuzzleFlash2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawLaser(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    float GetAeroTensorControlInput(const float aAng, const float aMinAng, const float aMaxAng);
    DirectX::SimpleMath::Matrix GetAeroTensor(FinDataDynamic& aFin, FinDataStatic& aLib);
    Utility::ForceAccum GetAeroForceAccum(const MissileData& aMissile, const float aTimeDelta, FinDataDynamic& aFin, FinDataStatic& aLib);


    unsigned int GetUniqueMissileID();
    
    void GuidanceBasic(MissileData& aMissile, const float aTimeDelta);
    void GuidanceManual(MissileData& aMissile, const float aTimeDelta);
    void GuidanceManualVector(MissileData& aMissile, const float aTimeDelta);
    void GuidanceTest(MissileData& aMissile, const float aTimeDelta);
    void GuidanceVelocitySteeringTest(MissileData& aMissile, const float aTimeDelta);

    void HardBurnModeActivator(MissileData& aMissile, const float aTimeDelta);
    void HardBurnModeTest(MissileData& aMissile, const float aTimeDelta);

    void InitializeAmmoCannon(AmmoStruct& aAmmo);
    void InitializeAmmoExplosive(AmmoStruct& aAmmo);
    void InitializeAmmoMachineGun(AmmoStruct& aAmmo);
    void InitializeAmmoMirv(AmmoStruct& aAmmo);
    void InitializeAmmoShotgun(AmmoStruct& aAmmo);
    void InitializeAmmoMissile(MissileStruct& aAmmo);

    void InitializeExplosionData(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, ExplosionData& aExplosionData);
    void InitializeFinLibrary(FinLibrary& aFinLib);



    void InitializeFinAeroTensor(FinDataStatic& aFinData, const DirectX::SimpleMath::Quaternion aAlignment, const float aAngleMinMax);

    void InitializeMissileInertiaTensor();

    void InitializeMuzzleFlashModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MuzzleFlash& aMuzzleFlash);
    void InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelMachineGun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelMirv(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelMissile(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MissileStruct& aAmmo);
    void InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection);
    void InitializeLaserModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, LaserModel& aLazerModel);

    void InputNavData(MissileData& aMissile, DirectX::SimpleMath::Quaternion aQuatToTarg, const DirectX::SimpleMath::Vector3 aTargPosLocalized, const DirectX::SimpleMath::Vector3 aVecToTargLocal);


    void IRSeekerTest(MissileData& aMissile, const float aTimeDelta);

    float ManualInputUpdate(const float aCurrentVal, const float aInput);
    float ManualInputDecay(const float aCurrentVal, const float aTimeStep);
    void ManualInputDecayVector(const float aTimeStep);

    void ManualMissileControl(MissileData& aMissile, const float aTimeDelta);
    void PrintFinData(FinDataStatic& aFinStat, FinDataDynamic& aFinDyn, MissileData& aMissile);
    void PrintFlightStateData(MissileData& aMissile);
    void PrintMissileData(MissileData& aMissile, const float aTimeDelta);
    void ProNavTest(MissileData& aMissile, const float aTimeDelta);
    void ProNav(MissileData& aMissile, const float aTimeDelta);
    void ProNav2(MissileData& aMissile, const float aTimeDelta);
    void ProNav3(MissileData& aMissile, const float aTimeDelta);
    void ProNav4(MissileData& aMissile, const float aTimeDelta);
    void ProNav5(MissileData& aMissile, const float aTimeDelta);
    void ProNavOpenSteer(MissileData& aMissile, const float aTimeDelta);
    void ResetMissileForceAccumulators(MissileData& aMissile);

    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RightHandSideMissile(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RightHandSideMissileDebugTest(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);
    void RungeKutta4Missile(struct MissileData* aProjectile, double aTimeDelta);

    DirectX::SimpleMath::Vector3 Seek(const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargPosLocal);
    DirectX::SimpleMath::Vector3 Pursuit(MissileData& aMissile, const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargLocalPos, const DirectX::SimpleMath::Vector3 aTargLocalVel);

    void UpdateAngularStability(MissileData& aMissile, const float aTimeDelta);

    void UpdateControlData(MissileData& aMissile, const float aTimeDelta);

    void UpdateDynamicExplosive(struct ExplosionData& aExplosion, const double aTimeDelta);
    void UpdateExplosionVec(double aTimeDelta);

    void UpdateFlightStateData(MissileData& aMissile, const double aTimeDelta);

    void UpdateMirv(ProjectileData& aProjectile, const double aTimeDelta);

    void UpdateLOSData(MissileData& aMissile, const float aTimeDelta);
    void UpdateLOSData2(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileAlignment(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileDragLinear(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileForcesLift(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileGuidance(MissileData& aMissile, const float aTimeDelta);
    void CruiseGuidance(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileCoefficients(MissileData& aMissile, const float aTimeDelta);

    
    // Missile accumulators
    void AccumulateMissileForces(MissileData& aMissile, const float aTimeDelta);
    Utility::ForceAccum AeroAccum(MissileData& aMissile);
    Utility::ForceAccum BoosterAccum(MissileData& aMissile);
    Utility::ForceAccum BoosterAccumStatic(MissileData& aMissile);
    Utility::ForceAccum BoosterAccumVectored(MissileData& aMissile);
    void BoosterSteeringUpdate(MissileData& aMissile);
    Utility::ForceAccum DragAccum(MissileData& aMissile, const float aTimeDelta);
    Utility::ForceAccum BodyAeroAccum(MissileData& aMissile);

    // fin funcs
    void UpdateFinData(MissileData& aMissile);
    void UpdateFinAngles(MissileData& aMissile);
    void UpdateFinForces(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const MissileData& aMissile);
    //void UpdateFinForcesTemp(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Quaternion aAlignQuat, const MissileData& aMissile);
    //void UpdateFinForcesOld(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Quaternion aAlignQuat, const MissileData& aMissile);
    
    Utility::ForceAccum FinForceAccum(const FinDataStatic& aFinLib, const FinDataDynamic& aFinDyn, const MissileData& aMissile);
    Utility::ForceAccum FinAccumSum(const MissileData& aMissile);
    Utility::ForceAccum FinAccumSumTest(const MissileData& aMissile);

    void UpdateMissileForces(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileModelData(MissileData& aMissile);
    void UpdateMissileVec(double aTimeDelta);
    void UpdateMuzzleFlash(MuzzleFlash& aMuzzleFlash, const double aTimeDelta);
    void UpdateNavData(MissileData& aMissile, const float aTimeDelta);
    void UpdateProjectileVec(double aTimeDelta);
    void UpdateProjectileData(ProjectileData& aProjectile, const float aTimeDelta);

    void UpdateSteeringDirNorm(MissileData& aMissile, const float aTimeDelta);
    void UpdateSteeringDirNormOld(MissileData& aMissile, const float aTimeDelta);
    void UpdateSteeringDirNormOld2(MissileData& aMissile, const float aTimeDelta);

    Environment const* m_environment;
    std::shared_ptr<DebugData> m_debugData;
    std::shared_ptr<NPCController> m_npcController;
    std::shared_ptr<Vehicle> m_playerVehicle;

    LauncherData m_launcherData;

    AmmoStruct m_ammoCannon;
    AmmoStruct m_ammoExplosive;
    AmmoStruct m_ammoMachineGun;
    AmmoStruct m_ammoMirv;
    AmmoStruct m_ammoShotgun;

    ExplosionStruct m_explosionStruct;

    FinLibrary      m_finLib;

    MissileStruct m_ammoMissile;
    MissileConsts m_missileConsts;

    LaserModel m_playerLaser;

    const float m_projectileLifeTimeMax = 10.0f;
    const float m_missileLifeTimeMax = 151.0f;

    std::vector<ProjectileData> m_projectileVec;
    std::vector<ProjectileData> m_newProjectilePushVec;
    std::vector<MissileData> m_missileVec;

    float m_testTimer = 0.0f;
    float m_testTimer2 = 0.0f;

    float m_coolDownTimer = 0.0f;
    bool m_isCoolDownActive = false;

    const float m_mirvFuseDelayTime = 0.7f;

    AmmoType m_currentAmmoType;
    MuzzleFlash m_muzzleFlash;

    const float m_gravityMod = 1.0f;
    const float m_explosiveTorqueArmMod = 1.0f;
    const float m_maxExplosiveForce = 500000.0f;

    bool m_isTestBoolTrue = false;

    bool m_isTargetingLaserOn = true;
    int m_currentTargetID = -1;
    bool m_isTargetingLaserLockTrue = false;

    DirectX::SimpleMath::Matrix m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix m_missileInverseInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    //const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(4.0f, 1.0f, 1.0f);
    //const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(1.1f, 0.127f, 0.127f);  // clean up needed and merge to MissileConst data struct
    const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(1.1f, 0.13f, 0.13f);  // clean up needed and merge to MissileConst data struct

    DirectX::SimpleMath::Vector3 m_testVec = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 1.0f);
    float m_testAng = 0.0f;
    int m_cameraTrackedMissileID = -1;
    bool m_isLaserFlickerTrue = false;

    unsigned int m_nextUniqueMissileID = 0;
    unsigned int m_camTrackMissileId = 0;

    float m_testRotation = 0.0f;

    const DirectX::SimpleMath::Vector3 m_testTargetPos = DirectX::SimpleMath::Vector3(570.0f, 10.0f, 0.0f);

    float m_testRotYaw = 0.0f;
    float m_testRotPitch = 0.0f;
    float m_testRotRoll = 0.0f;

    bool m_isDebugToggleTrue = false;

    bool m_isDebugToggleTrue1 = false;
    bool m_isDebugToggleTrue2 = false;
    bool m_isDebugToggleTrue3 = false;

    bool m_isRocketOnDebug = true;

    const bool m_isDebugToggleTrueTestConst = false;
    const bool m_isDebugToggleTrueTestConst1 = false;
    const bool m_isDebugToggleTrueTestConst2 = false;
    const bool m_isDebugToggleTrueTestConst3 = false;

    DirectX::SimpleMath::Vector3 m_debugVec1 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugVec2 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugVec3 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugHeadingVec1 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugHeadingVec2 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugHeadingVec3 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_debugHeadingVec4 = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Quaternion m_debugHeadingQuat1 = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_debugHeadingQuat2 = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_debugHeadingQuat3 = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_debugHeadingQuat4 = DirectX::SimpleMath::Quaternion::Identity;

    float m_debugDistanceToTarget = 0.0f;
    float m_debugDistanceToTarget2 = 0.0f;
    float m_debugDistanceToTarget3 = 0.0f;

    const int m_selectMissileFire = 0;

    const bool m_isDebugAngularStabilityOn = true;
    const bool m_isHardBurnModeTestOn = true;
    bool m_isUseProNavOn = false;

    MissileType m_currantMissileType = MissileType::TYPE_TAILCONTRTROL;

    float m_debugVal1 = Utility::ToRadians(0.0f);
    const float m_debugValMax = Utility::ToRadians(180.0f);
    const float m_debugValMin = Utility::ToRadians(-180.0f);
    const float m_debugValDeltaRate = Utility::ToRadians(30.0f);

    ControlInputType m_currentControlType = ControlInputType::INPUT_TAILFIN;

    float m_manualCanardPitch = Utility::ToRadians(0.0f);
    float m_manualCanardYaw = Utility::ToRadians(0.0f);
    float m_manualThrustVecPitch = Utility::ToRadians(0.0f);
    float m_manualThrustVecYaw = Utility::ToRadians(0.0f);
    float m_manualTailPitch = Utility::ToRadians(0.0f);
    float m_manualTailYaw = Utility::ToRadians(0.0f);
    const float m_manualDecayRateMod = 0.5f;
    const bool m_isManualInputDecayTrue = true;

    DirectX::SimpleMath::Vector2 m_manualControlInput = DirectX::SimpleMath::Vector2::Zero;
    const float m_manualInputRate = 1.1f;
    const float m_manualDecayRate = 0.9f;
    bool m_isManualUpdatePitchTrue = false;
    bool m_isManualUpdateYawTrue = false;

    float m_debugThrustAngMax = 0.0f;
    float m_debugThrustAngMin = 0.0f;

    float m_aeroDebugVal1 = 0.0f;
    float m_aeroDebugVal2 = 0.0f;
    const float m_aeroDebugDelta1 = 0.1f;
    const float m_aeroDebugDelta2 = 0.4f;

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32, bool, bool>> m_debugDrawVec;
    void DebugPushDrawData(const DirectX::SimpleMath::Vector3 aPosLocal, const DirectX::SimpleMath::Vector3 aVecLocal, const DirectX::XMVECTORF32 aColor, const bool aIsDrawScaledTrue, const bool aIsRealignTrue);
    void DebugDrawUpdate(MissileData& aMissile);

    bool m_isInRunge = false;

public:
    float GetExplosiveTorqueArmMod() const { return m_explosiveTorqueArmMod; };
    bool GetIsCoolDownActive() const { return m_isCoolDownActive; };
    float GetMaxExplosionForce() const { return m_explosionStruct.maxExplosionForce; };
    float GetMaxExplosionImpactRadius() const { return m_explosionStruct.maxExplosionImpactRadius; };

    void CamMissileSelectNext();

    const float m_testDebugAng = Utility::ToRadians(15.0f);
    void DebugIntputValUpdate(const float aInput);
    void DebugIntputValUpdateStatic(const bool aIsTrue);
    void DebugInputZero();

    void CycleControlInputType();

    void ManualControlInputPitch(const float aInput);
    void ManualControlInputYaw(const float aInput);

    void ManualControlInput(FinType aFinType, const float aInput);
    void ManualInputReset(FinType aFinType, const bool aIsResetAllTrue);
};

