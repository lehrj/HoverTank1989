#pragma once
#include "Environment.h"
#include "NPCController.h"
#include "Utility.h"
#include "Vehicle.h"

class Vehicle;


struct AudioFxData
{
    enum class FxType
    {
        FXTYPE_MISSILE_LAUNCH,
        FXTYPE_MISSILE_ROCKET_BURN,
    };
    FxType type;
    std::vector<DirectX::SimpleMath::Vector3> position;
    DirectX::SimpleMath::Vector3 up;
    bool isLoopTrue;
    bool isDeadTrue;
};

enum class AmmoType
{
    AMMOTYPE_CANNON,
    AMMOTYPE_EXPLOSIVE,
    AMMOTYPE_MIRV,
    AMMOTYPE_MACHINEGUN,
    AMMOTYPE_SHOTGUN,
    AMMOTYPE_GUIDEDMISSILE,
};

struct ContrailData
{
    std::vector<DirectX::SimpleMath::Vector3> posVec;
    DirectX::SimpleMath::Vector3 posLocal;
};

struct ContrailPackage
{
    ContrailData top;
    ContrailData port;
    ContrailData starboard;
    ContrailData bottom;
    unsigned int iterator;
    bool isContrailOn = false;
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
    FLIGHTSTATE_OVERSHOOT,
    FLIGHTSTATE_POSE,
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
    float prevFinAngle = Utility::ToRadians(0.0f);
    DirectX::SimpleMath::Vector3 finDir = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 chordLine = DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Vector3 prevFinDir = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 prevChordLine = DirectX::SimpleMath::Vector3::UnitX;

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
    std::string name = "name";
    DirectX::SimpleMath::Vector3 axis;
    DirectX::SimpleMath::Vector3 finNormal;
    DirectX::SimpleMath::Vector3 posLocal;

    float chord;
    float semiSpan; // half span value for fin span
    float span;
    float wingSpanFull; // full wing span including body
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

    DirectX::SimpleMath::Vector3 thrustVec = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion thrustQuat = DirectX::SimpleMath::Quaternion::Identity;
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

    bool isShotFromLeftTubeTrue = true;

    float timeStepDelta = 0.0f;
    unsigned int uniqueId = 0;
    int targetID = 0;

    float estimatedMaxVelocity = 375.0f;
    float estimatedDrag = 0.0f;
    float estimatedCrossArea = 0.0f;

    float altitude = 0.0f;
    
    bool isContrailOn = false;
    int contrailLength = 0;
    bool contrailToggle = false;

    bool isFinsDeployStarted = false;
    bool isFinsDeployEnd = false;
    bool isFinDeployAudioTriggered = false;

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

    DirectX::SimpleMath::Vector3 selfVelLocal = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 selfVelLocalNorm = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion selfLocalVelQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion selfLocalVelQuatInverse = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 selfUpLocal = DirectX::SimpleMath::Vector3::UnitY;

    DirectX::SimpleMath::Vector3 targetDestLocal = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetPosLocal = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetVelLocal = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 targetDestination = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetPosition = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetPositionWorld = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetForward = DirectX::SimpleMath::Vector3::UnitX;
    float targetDistance = 0.0f;
    float targetDistanceDelta = 0.0f;
    float closureRate = 0.0f;

    DirectX::SimpleMath::Vector3 steeringDirNormLocal = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion steeringQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 headingLocalVecTest = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Quaternion headingLocalQuatTest = DirectX::SimpleMath::Quaternion::Identity;

    bool isFacingDestTrue = true;
    bool isVelocityForward = true;

    DirectX::SimpleMath::Quaternion angularStepQuat = DirectX::SimpleMath::Quaternion::Identity;
    //DirectX::SimpleMath::Matrix targetLaserAlignment = DirectX::SimpleMath::Matrix::Identity;

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

    float testTimer = 0.0f;
    float testTimerFirstGuessA = 0.0f;
    float testTimerFirstGuessB = 0.0f;
    bool testTimerTriggerIsTrueA = false;
    bool testTimerTriggerIsTrueB = false;

    float testTimerDeltaA = 0.0f;
    float testTimerDeltaB = 0.0f;

    float afterBurnFlickerRotation = 0.0f;

    DirectX::SimpleMath::Matrix seekerHousingMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix seekerLensMat = DirectX::SimpleMath::Matrix::Identity;

    float laserPulseScale = 1.0f;
    float laserPulseTimer = 0.0f;
    bool isLaserFlickerTrue = true;
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
    bool isDeleteTestingDebug = false;

    float time;
    int liveTimeTick;
    float liveTimeCountDown = 0.0f;
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

    int rhsCounter = 0;
};

struct MissileModel
{
    // consts & colors
    // contrails
    //const float contrailColorMax = 0.9f; 
    //const float contrailColorMin = 0.35f;
    const float contrailColorMax = 0.98f;
    const float contrailColorMin = 0.45f;
    const unsigned int contrailDrawCountMax = 30;

    //const float afterBurnPlumeLength = 1.925f;
    const float afterBurnPlumeLength = 2.5f;

    DirectX::SimpleMath::Vector3 afterBurnPlumeBasePos;
    DirectX::SimpleMath::Vector3 afterBurnPlumeThrottleModPos;
    float afterBurnBaseSize;
    float afterBurnConeBaseLength;

    // afterburn flicker
    //const float afterBurnFlickerRotationRate = Utility::ToRadians(700.1f);
    const float afterBurnFlickerRotationRate = Utility::ToRadians(1700.1f);
    const float afterBurnFlickerRateScale = 100.1f;
    const float plumeflickerScaleLength = 0.5f;
    const float plumeflickerScaleWidth = 0.7f;

    const DirectX::SimpleMath::Vector4 bodyColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 plumeColor = DirectX::SimpleMath::Vector4(1.000000000f, 0.270588249f, 0.000000000f, 1.40000000f);
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


    std::unique_ptr<DirectX::GeometricPrimitive>    seekerHousingShape;
    DirectX::SimpleMath::Matrix localSeekerHousingMatrix;
    DirectX::SimpleMath::Matrix localSeekerHousingTranslation;
    DirectX::SimpleMath::Matrix seekerHousingTranslation;

    std::unique_ptr<DirectX::GeometricPrimitive>    seekerLensShape;
    DirectX::SimpleMath::Matrix localSeekerLensMatrix;
    DirectX::SimpleMath::Matrix localSeekerLensTranslation;
    DirectX::SimpleMath::Matrix seekerLensTranslation;

    std::unique_ptr<DirectX::GeometricPrimitive>    seekerRingShape;
    DirectX::SimpleMath::Matrix localSeekerRingMatrix;
    DirectX::SimpleMath::Matrix localSeekerRingTranslation;
    DirectX::SimpleMath::Matrix seekerRingTranslation;

    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeShapeInvert;
    DirectX::SimpleMath::Matrix localPlumeMatrix;
    DirectX::SimpleMath::Matrix worldPlumeMatrix;
    DirectX::SimpleMath::Matrix plumeTranslation;
    DirectX::SimpleMath::Matrix plumeTranslationTest;
    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeBaseShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    rocketPlumeBaseShapeInvert;
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
    ContrailPackage contrails;
    
    std::shared_ptr<Utility::SoundFx> audioFx;
};

struct MissileStruct
{
    AmmoData ammoData;
    MissileModel modelData;
};

enum class MissileTubeSelected
{
    MISSILETUBESELECTED_BOTH,
    MISSILETUBESELECTED_LEFT,
    MISSILETUBESELECTED_RIGHT,
    MISSILETUBESELECTED_DEBUG,
};

struct MissileConsts
{
    const float angularDragMod = 0.8f;

    const float detonationRange = 10.0f;
    const float detonationDrawDelay = 0.25f;
    const DirectX::SimpleMath::Vector3 dimensions = DirectX::SimpleMath::Vector3(1.1f, 0.127f, 0.127f);

    const float explosionRadiusInitial = dimensions.z * 2.0f;

    /*
    const float finDeployDelay = 0.2f;
    const float rocketFireDelay = 1.0f;
    const float finDeployTime = 0.4f;
    const float rocketFireFullTime = 0.5f;
    */    

    /*
    const float finDeployDelay = 0.1f;
    const float rocketFireDelay = 0.05f;

    //const float finDeployTime = 0.15f;
    const float finDeployTime = 1.15f;
    //const float rocketFireFullTime = 0.01f;
    const float rocketFireFullTime = 1.01f;
    */

    /* latest
    const float finDeployDelay = 0.2f;
    const float rocketFireDelay = 0.6f;
    const float finDeployTime = 0.4f;
    const float rocketFireFullTime = 0.3f;
    const float rocketOverBoostTime = 0.3f;
    const float rocketOverBoostMax = 2.0f;
    */

    //const float finDeployDelay = 0.15f;
    const float finDeployDelay = 0.35f;
    const float finDeployTime = 0.5f;

    const float rocketFireDelay = 0.9f;
    //const float rocketFireFullTime = 0.3f;
    const float rocketFireFullTime = 0.6f;
    const float rocketOverBoostTime = 0.3f;
    const float rocketOverBoostMax = 2.0f;
  
    const float launchVelocity = 19.0f;

    const float contrailVelocityMin = 19.0f;
    const float contrailVelocityMax = 27.0f;

    //const float laserDepoyDelay = 3.9f;
    //const float laserDepoyDelay = finDeployTime + finDeployDelay;
    const float laserDepoyDelay = rocketFireDelay + rocketFireFullTime + 2.0f;

    const float laserDiameterModMax = 20.0f;
    const float laserDiameterModMin = 5.0f;
    const float laserModRangeMin = 200.0f;
    const float laserModRangeMax = 500.0f;
    const DirectX::SimpleMath::Vector3 laserPosOffSet = DirectX::SimpleMath::Vector3(0.0f, 2.0f, 0.0f);

    /*
    const float finDeployDelay = 5.2f;
    const float rocketFireDelay = 5.6f;
    const float finDeployTime = 0.4f;
    const float rocketFireFullTime = 0.3f;
    const float rocketOverBoostTime = 0.3f;
    const float rocketOverBoostMax = 2.0f;
    */

    const float wingArea = 0.3f;

    const float dragCoefficientDebug = 0.5f;
    const float dragAreaDebug        = 0.03f;

    const float dragCoefficient = 0.3f;
    const float dragCoefficientAng = 0.3f;
    const float postExplosionMass = 10.0f;
    const float postExplosionDragCoefficient = 0.5f;
    const float explosionDragCoefficientBase = 0.1f;
    const float explosionDragCoefficientAddMax = 0.1f;

    const DirectX::SimpleMath::Vector3 thrustPosLocal = DirectX::SimpleMath::Vector3(-0.5f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 thrustPosLocalOffset = DirectX::SimpleMath::Vector3(0.01f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureBasePosLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureFullFinDeployOffset = DirectX::SimpleMath::Vector3(-0.3f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureAngleOfAttackOffSetMax = DirectX::SimpleMath::Vector3(-0.5f, 0.0, 0.0f);
    const DirectX::SimpleMath::Vector3 centerOfPressureFullDeployLocal = centerOfPressureBasePosLocal + centerOfPressureFullFinDeployOffset;
    const DirectX::SimpleMath::Vector3 centerOfMassLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0, 0.0f);

    const float steerAngMax = Utility::ToRadians(25.0f);
    const float steeringAngPerSecDeltaMax = Utility::ToRadians(50.0f);

    const float tailFinAngMax = Utility::ToRadians(15.0f);
    //const float tailFinAngPerSecDeltaMax = Utility::ToRadians(90.0f);
    const float tailFinAngPerSecDeltaMax = Utility::ToRadians(65.0f);

    const float thrustVecAudioPitchModMax = 0.9f;
    const float thrustVecAngMax = Utility::ToRadians(15.0f);
    const float thrustVecDeadZoneAng = Utility::ToRadians(10.0f);
    //const float thrustVecAngPerSecDeltaMax = Utility::ToRadians(40.0f);
    const float thrustVecAngPerSecDeltaMax = Utility::ToRadians(15.0f);

    const float mass = 12.0f;
    const float rocketBoostForceMax = mass * 11.0f; // Use desired thrust to weight ratio for boost value
    const float velMaxEst = (rocketBoostForceMax / mass) * 30.0f;

    // flight modeling
    const float climbOutAngle = Utility::ToRadians(75.0f);
    const float climbOutDuration = 2.5f;
    const float climbOutAltMin = 100.0f;

    //const float cruiseAltMin = 100.0f;
    const float cruiseAltMin = 200.0f;
    const float maxAlt = 200.0f;
    //const float terminalRange = 100.0f;
    //const float terminalRange = 25.0f;
    const float terminalRange = 25.0f;

    const float navigationGain = 3.0f;

    const float hardBurnRadPerSec = Utility::ToRadians(25.0f);

    const DirectX::SimpleMath::Vector3 canardPosLocal = DirectX::SimpleMath::Vector3(0.4f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 tailPosLocal = DirectX::SimpleMath::Vector3(-dimensions.x * 0.45f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 mainWingPosLocal = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    const AeroType testFinType = AeroType::AERO_CLASSIC;

    const float canardChord = ((dimensions.x * 0.3f) * 0.2f);
    const float canardSpan = dimensions.x * 0.3f;
    const float canardThickness = ((dimensions.x * 0.3f) * 0.2f) * 0.3f;
    
    /*
    const float mainChord = 0.06f;
    const float mainSpan = 0.3f;
    const float mainThickness = 0.015f;
    */
    //const float mainChord = 0.132f;
    const float mainChord = 0.17f;
    const float mainSpan = 0.333f;
    const float mainThickness = 0.014f;

    const float tailChord = ((dimensions.x * 0.3f) * 0.2f) * 2.0f;
    //const float tailSpan = dimensions.x * 0.3f * 2.0f;
    const float tailSpan = dimensions.x * 0.3f * 2.0f;
    //const float tailThickness = ((dimensions.x * 0.3f) * 0.2f) * 0.3f;
    const float tailThickness = tailChord * 0.11f;

    //const float cooldown = 3.0f;
    const float cooldown = 1.0f;

    const int selectFirePattern = 2;

    const bool useAdvancedMoiTensorTrue = false;
    const bool isMissileFreezeTrue = false;
    const bool isMissleTargetingLaserTrue = true;
    const bool isUseDebugRG4True = false;
    const bool isUseConstFinClTrue = false;
    const bool isManualControlTrue = false;
    const bool isThrustVecOn = true;
    const bool isDynamicFinOn = true;
    const bool isFinForceOn = true;
    const bool isBodyAeroOn = false;
    const bool isContrailsOn = true;
    const bool isGravityOn = true;

    // recoil force consts
    const float recoilImpulseTimeMax = 0.3f;
    const float recoilTorqueMod = 1.0f;
    const float recoilMagnitudeMax = 7500.0f;
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

    std::shared_ptr<Utility::SoundFx> soundFx;
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

enum class MuzzleFlashType
{
    MUZZLEFLASHTYPE_MAINGUN,
    MUZZLEFLASHTYPE_LEFT,
    MUZZLEFLASHTYPE_RIGHT,
};

struct MuzzleFlash
{
    MuzzleFlashType flashType = MuzzleFlashType::MUZZLEFLASHTYPE_MAINGUN;

    DirectX::SimpleMath::Vector4 color1 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    DirectX::SimpleMath::Vector4 startColor = DirectX::SimpleMath::Vector4(0.7f, 0.0f, 0.0f, 1.3f);
    DirectX::SimpleMath::Vector4 endColor = DirectX::SimpleMath::Vector4(1.f, 0.4f, 0.f, 0.7f);
    DirectX::SimpleMath::Vector4 currentColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    DirectX::SimpleMath::Matrix localOrientation = DirectX::SimpleMath::Matrix::Identity;
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

    bool isDeleteTrue = false;
};

struct MuzzleFlashModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape2;
    DirectX::SimpleMath::Matrix worldMuzzleFlashConeMatrix;
    DirectX::SimpleMath::Matrix localMuzzleConeMatrix;
    DirectX::SimpleMath::Matrix worldTestMatrix;
    DirectX::SimpleMath::Matrix localTestMatrix;
    const float baseConeHeight = 1.0f;
    const float baseConeDiameter = 0.3f;
};

struct LaserModel
{
    // colors
    float colorVal = 0.0f;
    const float colorValDelta = 5.0f;
    DirectX::SimpleMath::Vector4 laserColorToUse = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 laserColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 laserColorLockTrue = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector4 testColor = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f);
    float distance = 0.0f;
    float flickerRot = 0.0f;
    const float flickerRate = 0.7f;

    bool isFlickerTrue = false;

    std::unique_ptr<DirectX::GeometricPrimitive>    laserShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    laserShape2;
    std::unique_ptr<DirectX::GeometricPrimitive>    laserShapeMissile;

    DirectX::SimpleMath::Matrix localBodyMatrix;
    DirectX::SimpleMath::Matrix worldBodyMatrix;
    DirectX::SimpleMath::Matrix translationMatrix;

    const float missileLaserLensDiameter = 0.0304800030f;
};

struct TargetControl
{
    bool isTargetOnDeckTrue = false;
    std::vector<unsigned int> targetList;

    unsigned int targetId = 0;
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
    void FireMissileWithAudio(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp, const float aTimeOffSet, std::shared_ptr<Utility::SoundFx> aFireFx, bool aDebugToggle);

    void FireSelectedWithAudio(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp, std::shared_ptr<Utility::SoundFx> aFireFx, bool aDebugToggle);

    void FireProjectileCannon(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMachineGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);

    void FireSelectedAmmo(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);    
    void FireDefaultProjectile(const AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    AmmoType GetCurrentAmmoType() const { return m_currentAmmoType; };
    bool GetIsMissileActiveTrue() const;
    bool GetIsMissileExplodingTrue() const;

    void GetCameraMissileData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget, bool isFoundTrue) const;
    void GetCameraTrackAllData(DirectX::SimpleMath::Quaternion& aAlignment, DirectX::SimpleMath::Vector3& aPos, DirectX::SimpleMath::Vector3& aTarget, DirectX::SimpleMath::Vector3& aUp) const;
    Utility::ImpulseForce GetRecoilImpulseForce(DirectX::SimpleMath::Vector3 aDirectionNorm);
    
    int GetTargetControlID() const { return m_targetControl.targetId; };
    int GetTargetCurrentID() const { return m_currentTargetID; };
    

    void GetUIData(DirectX::SimpleMath::Vector3& aPosRaw, DirectX::SimpleMath::Vector3& aPosMod);

    bool GetIsAutoFireOn() { return m_isAutoFireOn; };
    bool GetIsAutoFireTargetReadyTrue() { return m_targetControl.isTargetOnDeckTrue; };
    bool GetIsAutoFireTargetValidTrue();

    bool GetIsCameraTrackedMissileExplodingTrue() const;

    bool GetIsFireCooldownTrue() { return m_isCoolDownActive; };
    
    bool GetIsTargetingLaserOn() { return m_isTargetingLaserOn; };
    bool GetIsTargetingLaserHitTrue() { return m_isTargetingLaserLockTrue; };

    bool GetIsMissileFireAvailable() const;
    bool GetIsMissileDualFireTrue() const {return m_isTubeDualFireTrue;};

    void PushVehicleExplosion(const DirectX::SimpleMath::Vector3 aPos, const int aVehicleId);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCController(std::shared_ptr<NPCController> aNPCController);
    void SetMissileCamID();

    void ToggleAutoFire();
    void ToggleTargetingLaser();
    void TriggerMirvDeploy();
    void ToggleDebug1();
    void ToggleDebug2();
    void ToggleDebug3();

    void LaserMissileToggleOn();
    void LaserMissileToggleOff();
    void LaserMissileSizeModOn();
    void LaserMissileSizeModOff();

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
    float CalculateFinLiftCoefUpdate(const float aAngleOfAttack);
    float CalculateFinLiftCoefDebug(const float aAngleOfAttack);
    float CalculateFinLiftCoefFlat(const float aAngleOfAttack);
    float CalculateFinLiftCoefTest(const float aAngleOfAttack);
    float CalculateFinLiftWholeBody(const float aAngleOfAttack);
    float CalculateFinLiftWholeBodySymmetric(const float aAngleOfAttack);
    float CalculateFinLiftCoefFlatOld(const float aAngleOfAttack);
    float CalculateFinLiftCoefFlatOld2(const float aAngleOfAttack);

    void CalculateGimbaledThrust(MissileData& aMissile, const float aTimeDelta);

    float CalculateLiftInducedDrag(const float aLiftCoeff, const float aLiftForce, const float aVelocity, const float aWingSpan, const float aChord, const float aWingArea);

    DirectX::SimpleMath::Vector3 CalculateWindVaningTorqueForce(const MissileData& aMissile);

    void CastRayLaser();
    void ControllerUpdate(MissileData& aMissile, const float aTimeDelta);
    void CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aVelocity, ExplosionType aExplosionType, const int aVehicleId);
    void CheckCollisions();
    //void CheckCollisionsMissile();
    void CheckCollisionsMissile(const double aTimeDelta);

    void CycleMissileTubeSelected();

    void DebugMissileFunc(MissileData* aMissile);
    void DebugGraphCurveData(MissileData& aMissile, const float aTimeDelta);
    void DebugSetMissileToPlayerPos(MissileData& aMissile);

    void DeleteMissileFromVec(const unsigned int aIndex);
    void DeleteMissileFromVecWithSound(const unsigned int aIndex);
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void DeployMirv(ProjectileData& aProjectile);

    void DrawContrails(MissileData& aMissile);

    void DrawExplosions(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawExplosions2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawLaser(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawLaserOld(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawMissiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    
    void DrawMissilesWithLighting(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawMissilesWithLightingOld(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void DrawMuzzleFlash(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawMuzzleFlash2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawMuzzleFlashVec(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    float GetAeroTensorControlInput(const float aAng, const float aMinAng, const float aMaxAng);
    DirectX::SimpleMath::Matrix GetAeroTensor(FinDataDynamic& aFin, FinDataStatic& aLib);
    Utility::ForceAccum GetAeroForceAccum(const MissileData& aMissile, const float aTimeDelta, FinDataDynamic& aFin, FinDataStatic& aLib);

    unsigned int GetUniqueMissileID();
    
    void HardBurnModeActivator(MissileData& aMissile, const float aTimeDelta);
    void HardBurnModeTest(MissileData& aMissile, const float aTimeDelta);

    void InitializeAmmoCannon(AmmoStruct& aAmmo);
    void InitializeAmmoExplosive(AmmoStruct& aAmmo);
    void InitializeAmmoMachineGun(AmmoStruct& aAmmo);
    void InitializeAmmoMirv(AmmoStruct& aAmmo);
    void InitializeAmmoShotgun(AmmoStruct& aAmmo);
    void InitializeAmmoMissile(MissileStruct& aAmmo);

    void InitializeContrails(MissileData& aMissile);

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

    void InitializeTargetControl();

    void InputNavData(MissileData& aMissile, DirectX::SimpleMath::Quaternion aQuatToTarg, const DirectX::SimpleMath::Vector3 aTargPosLocalized, const DirectX::SimpleMath::Vector3 aVecToTargLocal);

    void IRSeekerTest(MissileData& aMissile, const float aTimeDelta);

    float ManualInputUpdate(const float aCurrentVal, const float aInput);
    float ManualInputDecay(const float aCurrentVal, const float aTimeStep);
    void ManualInputDecayVector(const float aTimeStep);

    void ManualMissileControl(MissileData& aMissile, const float aTimeDelta);
    void PrintFinData(FinDataStatic& aFinStat, FinDataDynamic& aFinDyn, MissileData& aMissile);
    void PrintFlightStateData(MissileData& aMissile);
    void PrintMissileData(MissileData& aMissile, const float aTimeDelta);

    void ResetContrailData(MissileData& aMissile);

    void ResetMissileForceAccumulators(MissileData& aMissile);

    Utility::ForceAccum RHSBodyAero(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    Utility::ForceAccum RHSFinAccumSum(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    Utility::ForceAccum RHSFinCalc(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    Utility::ForceAccum RHSAeroForceAccumulator(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    DirectX::SimpleMath::Vector3 RHSFinForce(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    
    Utility::ForceAccum RHSLiftForce(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    Utility::ForceAccum RHSLiftForceRebuild(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    Utility::ForceAccum RHSLiftForceSymmetric(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);
    DirectX::SimpleMath::Vector3  RHSDragForceSymmetric(MissileData* aMissile, const DirectX::SimpleMath::Vector3 aVelocity);

    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RightHandSideMissile(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);
    void RungeKutta4Missile(struct MissileData* aProjectile, double aTimeDelta);

    DirectX::SimpleMath::Vector3 Seek(const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargPosLocal);
    DirectX::SimpleMath::Vector3 Pursuit(MissileData& aMissile, const DirectX::SimpleMath::Vector3 aVelLocal, const DirectX::SimpleMath::Vector3 aTargLocalPos, const DirectX::SimpleMath::Vector3 aTargLocalVel);

    void UpdateAngularStability(MissileData& aMissile, const float aTimeDelta);

    void UpdateContrails(MissileData& aMissile);

    void UpdateControlData(MissileData& aMissile, const float aTimeDelta);

    void UpdateDynamicExplosive(struct ExplosionData& aExplosion, const double aTimeDelta);

    void UpdateEstimatedMaxVelocity(MissileData& aMissile);

    void UpdateExplosionVec(double aTimeDelta);

    void UpdateFlightStateData(MissileData& aMissile, const double aTimeDelta);
  
    void UpdateFlightState(MissileData& aMissile, const double aTimeDelta);
    void UpdateFlightDataDependantVars(MissileData& aMissile, const double aTimeDelta);
    void UpdateFlightDataIndependentVars(MissileData& aMissile, const double aTimeDelta);

    void UpdateFlightDataTarget(MissileData& aMissile, const double aTimeDelta);

    void UpdateMirv(ProjectileData& aProjectile, const double aTimeDelta);

    void UpdateLOSData(MissileData& aMissile, const float aTimeDelta);
    void UpdateLOSData2(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileAlignment(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileDragLinear(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileForcesLift(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileCoefficients(MissileData& aMissile, const float aTimeDelta);
   
    // Missile accumulators
    void AccumulateMissileForces(MissileData& aMissile, const float aTimeDelta);
    Utility::ForceAccum AeroAccum(MissileData& aMissile);
    Utility::ForceAccum BoosterAccum(MissileData& aMissile);
    void BoosterSteeringUpdate(MissileData& aMissile);
    Utility::ForceAccum DragAccum(MissileData& aMissile, const float aTimeDelta);
    Utility::ForceAccum BodyAeroAccum(MissileData& aMissile);

    // fin funcs
    void UpdateFinData(MissileData& aMissile);
    void UpdateFinAngles(MissileData& aMissile);
    void UpdateFinForces(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const MissileData& aMissile);
    void UpdateFinForcesOld(const FinDataStatic& aStaticDat, FinDataDynamic& aFinDyn, const MissileData& aMissile);

    Utility::ForceAccum FinForceAccum(const FinDataStatic& aFinLib, const FinDataDynamic& aFinDyn, const MissileData& aMissile);
    Utility::ForceAccum FinAccumSum(const MissileData& aMissile);
    Utility::ForceAccum FinAccumSumTest(const MissileData& aMissile);

    void UpdateLaserColor(const float aTimeDelta);
    void UpdateLaserData(const float aTimeDelta);
    void UpdateMissileForces(MissileData& aMissile, const float aTimeDelta);

    //void UpdateMissileModelData(MissileData& aMissile);
    void UpdateMissileModelData(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileAudioData(MissileData& aMissile, const float aTimeDelta);
    void UpdateMissileVec(double aTimeDelta);
    void UpdateMuzzleFlash(MuzzleFlash& aMuzzleFlash, const double aTimeDelta);
    void UpdateMuzzleFlashVec(const double aTimeDelta);
    void UpdateMuzzleFlashOld(MuzzleFlash& aMuzzleFlash, const double aTimeDelta);
    void UpdateNavData(MissileData& aMissile, const float aTimeDelta);
    void UpdateProjectileVec(double aTimeDelta);
    void UpdateProjectileData(ProjectileData& aProjectile, const float aTimeDelta);

    void UpdateSteeringDirNorm(MissileData& aMissile, const float aTimeDelta);
    void UpdateSteeringDirNormOld(MissileData& aMissile, const float aTimeDelta);
    void UpdateSteeringDirNormOld2(MissileData& aMissile, const float aTimeDelta);
    void UpdateThrustVector(MissileData& aMissile);

    void UpdateTargetControl(const float aTimeDelta);

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
    
    TargetControl m_targetControl;

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
    MuzzleFlashModel m_muzzleFlashModel;
    std::vector<MuzzleFlash> m_muzzleFlashVec;

    const float m_gravityMod = 1.0f;
    const float m_explosiveTorqueArmMod = 1.0f;
    const float m_maxExplosiveForce = 500000.0f;

    bool m_isTestBoolTrue = false;

    bool m_isTargetingLaserOn = false;
    int m_currentTargetID = -1;
    bool m_isTargetingLaserLockTrue = false;
    bool m_isLaserHitLastFrameTrue = false;
    bool m_isLaserLockToneOn = false;
    bool m_isLaserFlickerTrue = false;
    const float m_laserLightingRate = 17.0f;
    float m_laserLightingRotation = 0.0f;
    float m_laserLightingPulseScale = 1.0f;
    float m_laserLightingPulseTimer = 0.0f;
    const float m_laserLightingPulseConst = 0.657f;
    const float m_laserMissileSize = 0.2f;
    bool m_isLaserMissileTrue = true;
    bool m_isLaserSizeModTrue = true;

    DirectX::SimpleMath::Matrix m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix m_missileInverseInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;
    //const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(4.0f, 1.0f, 1.0f);
    //const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(1.1f, 0.127f, 0.127f);  // clean up needed and merge to MissileConst data struct
    const DirectX::SimpleMath::Vector3 m_missileDimensions = DirectX::SimpleMath::Vector3(1.1f, 0.13f, 0.13f);  // clean up needed and merge to MissileConst data struct

    DirectX::SimpleMath::Vector3 m_testVec = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 1.0f);
    float m_testAng = 0.0f;
    int m_cameraTrackedMissileID = -1;

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

    //bool m_isRocketOnDebug = true;
    bool m_isDebugMissilePosToTankTrue = false;

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

    const bool m_isDebugAngularStabilityOn = true;
    const bool m_isHardBurnModeTestOn = true;
    bool m_isUseProNavOn = false;

    MissileType m_currentMissileType = MissileType::TYPE_TAILCONTRTROL;

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

    DirectX::SimpleMath::Vector3 m_testLiftVec = DirectX::SimpleMath::Vector3::Zero;
 
    bool m_isTestLiftInducedDragOn = false;

    // grouping up guidance functions for testing and debuging
    void GuidanceBasic(MissileData& aMissile, const float aTimeDelta);
    void GuidanceBasicGravityOld(MissileData& aMissile, const float aTimeDelta);
    void GuidanceBasicGravity(MissileData& aMissile, const float aTimeDelta);
    void GuidanceClimbOut(MissileData& aMissile, const float aTimeDelta);
    void GuidanceClimbOutOld(MissileData& aMissile, const float aTimeDelta);
    void GuidanceManual(MissileData& aMissile, const float aTimeDelta);
    void GuidanceManualVector(MissileData& aMissile, const float aTimeDelta);
    void GuidanceTest(MissileData& aMissile, const float aTimeDelta);
    void GuidanceTestOld(MissileData& aMissile, const float aTimeDelta);

    void GuidanceOvershoot(MissileData& aMissile, const float aTimeDelta);
    void GuidancePrototype(MissileData& aMissile, const float aTimeDelta);
    void GuidancePrototypeOld(MissileData& aMissile, const float aTimeDelta);
    void GuidanceVelocitySteeringTest(MissileData& aMissile, const float aTimeDelta);

    DirectX::SimpleMath::Vector3 GuidanceCalcInercept(MissileData& aMissile, const float aTimeDelta);

    void ProNavTest(MissileData& aMissile, const float aTimeDelta);
    void ProNav(MissileData& aMissile, const float aTimeDelta);
    void ProNav2(MissileData& aMissile, const float aTimeDelta);
    void ProNav3(MissileData& aMissile, const float aTimeDelta);
    void ProNav4(MissileData& aMissile, const float aTimeDelta);
    void ProNav5(MissileData& aMissile, const float aTimeDelta);
    void ProNavOpenSteer(MissileData& aMissile, const float aTimeDelta);

    void UpdateMissileGuidance(MissileData& aMissile, const float aTimeDelta);
    void CruiseGuidance(MissileData& aMissile, const float aTimeDelta);
    // end guidance functions

    // audio and explosions
    std::vector<std::shared_ptr<Utility::SoundFx>> m_fxExplosionVec;

    bool m_isTubeRippleFireTrue = true;
    bool m_isTubeDualFireTrue = true;
    MissileTubeSelected m_tubeFireSelected = MissileTubeSelected::MISSILETUBESELECTED_RIGHT;
    bool m_isDualFireCoolDownOverRideTrue = false;

    bool m_isAutoFireOn = false;

    std::vector<int> m_fireList;

    DirectX::SimpleMath::Vector3 m_currentCameraPos = DirectX::SimpleMath::Vector3::Zero;

public:

    unsigned int GetCreateAudioCount() const { return m_fxExplosionVec.size(); };
    std::shared_ptr <Utility::SoundFx> GetFxToCreate(unsigned int aIndex) { return m_fxExplosionVec[aIndex]; };
    void ClearCreateAudioVec() { m_fxExplosionVec.clear(); };

    float GetExplosiveTorqueArmMod() const { return m_explosiveTorqueArmMod; };
    bool GetIsCoolDownActive() const { return m_isCoolDownActive; };
    float GetMaxExplosionForce() const { return m_explosionStruct.maxExplosionForce; };
    float GetMaxExplosionImpactRadius() const { return m_explosionStruct.maxExplosionImpactRadius; };

    MissileTubeSelected GetNextTubeToFire() const { return m_tubeFireSelected; };

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

    void SetCurrentCameraPos(DirectX::SimpleMath::Vector3 aPos){m_currentCameraPos = aPos;};
};

