#pragma once
#include "DebugData.h"
#include "Environment.h"

#include "NpcAI.h"
#include "Utility.h"


class NPCController;

struct NpcControlInput
{
    // input control data
    float       angleToDestination;
    const float inputDeadZone = 0.0018;  // small deadzone to ignore nominal control input

    DirectX::SimpleMath::Vector3 omniDirection;
    float                        omniThrust
        ;
    bool        stearingIsPressed;
    float       steeringInput;
    const float steeringDecayRate = 0.2f;
    const float steeringInputMax = 6.0f;
    const float steeringInputMin = -6.0f;
    const float steeringInputRate = 3.0f;
    //const float steeringForceMod = 100.0f;
    const float steeringForceMod = 9000.0f;
    DirectX::SimpleMath::Vector3 steeringVec;

    float       throttleInput;
    const float throttleInputDecayRate = 0.6f;
    const float throttleInputMin = -1.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 0.1f;
    float baseThrottleInput = 0.0f;
};

struct HoverData
{
    const float hoverRangeLower = 5.0f;
    const float hoverRangeMid = 8.5f;
    const float hoverRangeUpper = 12.0f;
    const float groundNormalForceRange = hoverRangeUpper;
    const float hoverNeutralBoyantAlt = hoverRangeMid;

    float forwardThrust;
    const float forwardThrustMax = 50000.0f;
    const float reverseThrustMax = forwardThrustMax * 1.0f;
    float omniThrust;
    const float omniThrustMax = 65000.0f;
    float omniThrustDirection = 0.0f;
    const float omniThrustDirectionChangeRate = 1.5f;
    DirectX::SimpleMath::Vector3 omniThrustVec;

    DirectX::SimpleMath::Vector3    hoverLiftMax;
    DirectX::SimpleMath::Vector3    hoverLiftNeutralWithGrav;
    DirectX::SimpleMath::Vector3    hoverLiftCurrent;

    float turnRateCurrent;
    float turnRateMax;
};

struct MotionNPC
{
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
    Utility::Torque              bodyTorqueForce;
    DirectX::SimpleMath::Vector3 angularMomentum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 angularVelocity = DirectX::SimpleMath::Vector3::Zero;
};

enum class NPCType
{
    NPCTYPE_NPC00,
    NPCTYPE_NPC01,
    NPCTYPE_NPC02,
};

struct NPCModel
{
    DirectX::SimpleMath::Vector4 color1;
    DirectX::SimpleMath::Vector4 color2;
    DirectX::SimpleMath::Vector4 color3;
    DirectX::SimpleMath::Vector4 color4;

    std::unique_ptr<DirectX::GeometricPrimitive>    afterBurnShape;
    DirectX::SimpleMath::Matrix localAfterBurnLeftMatrix;
    DirectX::SimpleMath::Matrix worldAfterBurnLeftMatrix;
    DirectX::SimpleMath::Matrix localAfterBurnRightMatrix;
    DirectX::SimpleMath::Matrix worldAfterBurnRightMatrix;
    float afterBurnLengthLeft = 0.0f;
    float afterBurnLengthLeftPrev = 0.0f;
    float afterBurnLengthLeftPrev2 = 0.0f;
    float afterBurnLengthLeftPrev3 = 0.0f;
    float afterBurnLengthRight = 0.0f;
    float afterBurnLengthRightPrev = 0.0f;
    float afterBurnLengthRightPrev2 = 0.0f;
    float afterBurnLengthRightPrev3 = 0.0f;

    float afterBurnRadius;
    float afterBurnHeight;
    float afterBurnLeftScaledHeight;
    float afterBurnRightScaledHeight;

    DirectX::SimpleMath::Vector3 jetDirectionBase = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 jetDirectionLeft = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 jetDirectionRight = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Matrix jetMatLeft = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix jetMatRight = DirectX::SimpleMath::Matrix::Identity;

    float maxDelta = 0.0f;

    DirectX::SimpleMath::Matrix worldAfterBurnLeftMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix worldAfterBurnRightMatrix2 = DirectX::SimpleMath::Matrix::Identity;

    const float burnFlickerFrequencyMod = 39.04f;
    const float burnFlickerLengthMod = 7.1f;

    float burnFlickerLegth = 0.0f;

    float afterBurnLeftFlicker;
    float afterBurnLeftFlicker2 = 0.0f;
    float afterBurnRightFlicker;
    float afterBurnRightFlicker2 = 0.0f;
    float afterBurnFlickerRate;

    std::unique_ptr<DirectX::GeometricPrimitive>    baseBurnShape;
    DirectX::SimpleMath::Matrix localBaseBurnMatrix;
    DirectX::SimpleMath::Matrix worldBaseBurnMatrix1;
    DirectX::SimpleMath::Matrix worldBaseBurnMatrix2;
    float baseBurnLength;
    float baseBurnLengthPrev1 = 0.0f;
    float baseBurnFlicker1;
    float baseBurnFlicker2;
    float baseBurnRadius;
    float baseBurnHeight;
    float baseBurnScaledHeight;

    float mainThrustLengthMod = 1.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    jetHousingShape;
    DirectX::SimpleMath::Matrix jetHousingTranslationLeftMatrix;
    DirectX::SimpleMath::Matrix localJetHousingLeftMatrix;
    DirectX::SimpleMath::Matrix worldJetHousingLeftMatrix;
    DirectX::SimpleMath::Matrix jetHousingTranslationRightMatrix;
    DirectX::SimpleMath::Matrix localJetHousingRightMatrix;
    DirectX::SimpleMath::Matrix worldJetHousingRightMatrix;

    DirectX::SimpleMath::Matrix localJetHousingShellLeftMatrix;
    DirectX::SimpleMath::Matrix worldJetHousingShellLeftMatrix;
    DirectX::SimpleMath::Matrix localJetHousingShellRightMatrix;
    DirectX::SimpleMath::Matrix worldJetHousingShellRightMatrix;

    float jetRotationLeft = 0.0f;
    float jetRotationLeftPrev = 0.0f;
    float jetRotationLeftPrev2 = 0.0f;
    float jetRotationLeftPrev3 = 0.0f;
    float jetRotationRight = 0.0f;
    float jetRotationRightPrev = 0.0f;
    float jetRotationRightPrev2 = 0.0f;
    float jetRotationRightPrev3 = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    baseJetHousingShape;
    DirectX::SimpleMath::Matrix localBaseJetHousingMatrix;
    DirectX::SimpleMath::Matrix worldBaseJetHousingMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    baseJetShadowShape;
    DirectX::SimpleMath::Matrix localBaseJetShadowMatrix;
    DirectX::SimpleMath::Matrix worldBaseJetShadowMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    jetMountShape;
    DirectX::SimpleMath::Matrix localJetMountMatrix;
    DirectX::SimpleMath::Matrix worldJetMountMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    jetIntakeCoverShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    jetIntakeCoverShape2;
    DirectX::SimpleMath::Matrix localJetIntakeCoverLeftTranslationMatrix;
    DirectX::SimpleMath::Matrix localJetIntakeCoverLeftMatrix;
    DirectX::SimpleMath::Matrix worldJetIntakeCoverLeftMatrix;
    DirectX::SimpleMath::Matrix localJetIntakeCoverLeftTranslationMatrix2;
    DirectX::SimpleMath::Matrix localJetIntakeCoverLeftMatrix2;
    DirectX::SimpleMath::Matrix worldJetIntakeCoverLeftMatrix2;

    DirectX::SimpleMath::Matrix localJetIntakeCoverRightMatrix;
    DirectX::SimpleMath::Matrix worldJetIntakeCoverRightMatrix;

    DirectX::SimpleMath::Matrix localJetIntakeCoverRightMatrix2;
    DirectX::SimpleMath::Matrix worldJetIntakeCoverRightMatrix2;

    std::unique_ptr<DirectX::GeometricPrimitive>    modelShape;
    DirectX::SimpleMath::Matrix localModelMatrix;
    DirectX::SimpleMath::Matrix worldModelMatrix;
    DirectX::SimpleMath::Matrix localInteriorShadowRightMatrix;
    DirectX::SimpleMath::Matrix worldInteriorShadowRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearBodyShape;
    DirectX::SimpleMath::Matrix localRearBodyMatrix;
    DirectX::SimpleMath::Matrix worldRearBodyMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearDeckShape;
    DirectX::SimpleMath::Matrix localRearDeckMatrix;
    DirectX::SimpleMath::Matrix worldRearDeckMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    forwardShape;
    DirectX::SimpleMath::Matrix localForwardMatrix;
    DirectX::SimpleMath::Matrix worldForwardMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    eyeShape;
    DirectX::SimpleMath::Matrix localEyeLeftMatrix;
    DirectX::SimpleMath::Matrix worldEyeLeftMatrix;
    DirectX::SimpleMath::Matrix localEyeRightMatrix;
    DirectX::SimpleMath::Matrix worldEyeRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    grillShape;
    DirectX::SimpleMath::Matrix localGrillLeftMatrix;
    DirectX::SimpleMath::Matrix worldGrillLeftMatrix;
    DirectX::SimpleMath::Matrix localGrillRightMatrix;
    DirectX::SimpleMath::Matrix worldGrillRightMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    frontAirDamShape;
    DirectX::SimpleMath::Matrix localFrontAirDamMatrix;
    DirectX::SimpleMath::Matrix worldFrontAirDamMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    noseConeShape;
    DirectX::SimpleMath::Matrix localNoseConeMatrix;
    DirectX::SimpleMath::Matrix worldNoseConeMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    omniBaseShape;
    DirectX::SimpleMath::Matrix localOmniBaseMatrix;
    DirectX::SimpleMath::Matrix worldOmniBaseMatrix;
    float omniDialRadius;

    std::unique_ptr<DirectX::GeometricPrimitive>    omniDialShape;
    DirectX::SimpleMath::Matrix localOmniDialMatrix;
    DirectX::SimpleMath::Matrix worldOmniDialMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearShape;
    DirectX::SimpleMath::Matrix localRearMatrix;
    DirectX::SimpleMath::Matrix worldRearMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingArmShape;
    DirectX::SimpleMath::Matrix localWingArmTranslationMatrix;
    DirectX::SimpleMath::Matrix localWingArmMatrix;
    DirectX::SimpleMath::Matrix worldWingArmMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingShape;
    DirectX::SimpleMath::Matrix localWingTranslationMatrix;
    DirectX::SimpleMath::Matrix localWingMatrix;
    DirectX::SimpleMath::Matrix worldWingMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    wingFinShape;
    DirectX::SimpleMath::Matrix localWingFinLeftTranslationMatrix;
    DirectX::SimpleMath::Matrix localWingFinLeftMatrix;
    DirectX::SimpleMath::Matrix worldWingFinLeftMatrix;
    DirectX::SimpleMath::Matrix localWingFinRightTranslationMatrix;
    DirectX::SimpleMath::Matrix localWingFinRightMatrix;
    DirectX::SimpleMath::Matrix worldWingFinRightMatrix;

    float wingRotPrev1 = 0.0f;
    float wingRotPrev2 = 0.0f;
    float wingRotPrev3 = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    skirtShape;
    DirectX::SimpleMath::Matrix localSkirtMatrix;
    DirectX::SimpleMath::Matrix worldSkirtMatrix;
    float skirtRotation = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    steeringShape;
    DirectX::SimpleMath::Matrix localSteeringMatrix;
    DirectX::SimpleMath::Matrix translationSteeringMatrix;
    DirectX::SimpleMath::Matrix worldSteeringMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    throttleShape;
    DirectX::SimpleMath::Matrix localThrottleMatrix;
    DirectX::SimpleMath::Matrix worldThrottleMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    ventShape;
    DirectX::SimpleMath::Matrix localVentMatrix1;
    DirectX::SimpleMath::Matrix worldVentMatrix1;
    DirectX::SimpleMath::Matrix localVentMatrix2;
    DirectX::SimpleMath::Matrix worldVentMatrix2;
    DirectX::SimpleMath::Matrix localVentMatrix3;
    DirectX::SimpleMath::Matrix worldVentMatrix3;
    DirectX::SimpleMath::Matrix localVentMatrix4;
    DirectX::SimpleMath::Matrix worldVentMatrix4;
    DirectX::SimpleMath::Matrix localVentMatrix5;
    DirectX::SimpleMath::Matrix worldVentMatrix5;
    DirectX::SimpleMath::Matrix localVentMatrix6;
    DirectX::SimpleMath::Matrix worldVentMatrix6;
    DirectX::SimpleMath::Matrix localVentMatrix7;
    DirectX::SimpleMath::Matrix worldVentMatrix7;
    DirectX::SimpleMath::Matrix localVentMatrix8;
    DirectX::SimpleMath::Matrix worldVentMatrix8;
    DirectX::SimpleMath::Matrix localVentMatrix9;
    DirectX::SimpleMath::Matrix worldVentMatrix9;

    std::unique_ptr<DirectX::GeometricPrimitive>    avoidanceShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    collisionShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    avoidanceRadiusShape;

    std::unique_ptr<DirectX::GeometricPrimitive>    shadowBaseShape;
    DirectX::SimpleMath::Matrix localShadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowBaseMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowSkirtMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowJetLeftMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowJetRightMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowJetInteriorLeftMat = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix shadowJetInteriorRightMat = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3 shadowJetTranslationLeft = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Matrix shadowJetTranslationLeftMat = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive>    jetMountShadowShape;
    DirectX::SimpleMath::Matrix localJetMountShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix jetMountShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix worldJetMountShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix localSkirtShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix skirtShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix worldSkirtShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix localBodyMainShadowMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix bodyMainShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix worldBodyMainShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    std::unique_ptr<DirectX::GeometricPrimitive>    customShape;
    DirectX::SimpleMath::Matrix localCustomMatrix;
    DirectX::SimpleMath::Matrix worldCustomMatrix;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureBlank;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularBlank;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapBlank;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureFlame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularFlame;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapFlame;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureTest1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularTest1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapTest1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureTest2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularTest2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapTest2;

    bool isLeftPlumeFlickerTrue = false;
    bool isMainPlumeFlickerTrue = false;
    bool isRightPlumeFlickerTrue = false;
    const float plumeScale = 0.5f;
};

struct VehicleHardPoints
{
    DirectX::SimpleMath::Vector3 centerOfMassPos;
    DirectX::SimpleMath::Vector3 localCenterOfMassPos;
    DirectX::SimpleMath::Vector3 weaponDirection;
    DirectX::SimpleMath::Vector3 localWeaponDirection;
    DirectX::SimpleMath::Vector3 weaponPos;
    DirectX::SimpleMath::Vector3 localWeaponPos;

    DirectX::SimpleMath::Vector3 verticalStabilizerPos;
    DirectX::SimpleMath::Vector3 localVerticalStabilizerPos;

    DirectX::SimpleMath::Vector3 steeringTorqueArmPos;
    DirectX::SimpleMath::Vector3 localSteeringTorqueArmPos;

    DirectX::SimpleMath::Vector3 basePos;
    DirectX::SimpleMath::Vector3 localBasePos;

    DirectX::SimpleMath::Vector3 jetArmCenterAxis;
    DirectX::SimpleMath::Vector3 localJetArmCenterAxis;

    DirectX::SimpleMath::Vector3 leftJetAxis;
    DirectX::SimpleMath::Vector3 localLeftJetAxis;

    DirectX::SimpleMath::Vector3 rightJetAxis;
    DirectX::SimpleMath::Vector3 localRightJetAxis;
};

struct JumpData
{
    bool isImpulseBurnActive = false;
    bool isLaunchImpulseBurnActive = false;
    bool isLandImpulseBurnActive = false;
    bool isJumpActive = false;
    bool isJumpOnCoolDown = false;
    bool isJumpReady = false;
    bool isJumpUnlocked = false;

    float jumpActiveTimer = 0.0f;
    const float jumpActiveTimeTotal = 1.0f;

    float jumpCoolDownTimer = 0.0f;
    const float jumpCoolDownTotal = 8.0f;
    const float jumpVelocity = 25000.0f;
    const float impulseBurnTimeTotal = 1.0f;
    float impulseBurnTimer = 0.0f;

    Utility::ImpulseForce impulseBurnForce;

    float landingStartAltitude = 20.0f;
};

struct VehicleData
{
    unsigned int                id;
    float                       altitude;
    DirectX::SimpleMath::Vector3 dimensions;
    float                       maxCollisionDetectionRange;
    float                       dragCoefficient;
    float                       frontalArea;
    float                       hitPoints;
    const float                 mass = 700.0f;
    float                       topSpeedCalculated;
    MotionNPC                   q;
    float                       terrainHightAtPos;
    DirectX::SimpleMath::Vector3 terrainNormal;
    float                       time;
    DirectX::SimpleMath::Matrix alignment;
    DirectX::SimpleMath::Matrix alignmentInverse;
    DirectX::SimpleMath::Quaternion alignmentQuat;
    DirectX::SimpleMath::Quaternion alignmentInverseQuat;

    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 right;
    DirectX::SimpleMath::Vector3 up;

    DirectX::SimpleMath::Plane  groundPlane;

    VehicleHardPoints           hardPoints;
    NPCType                     npcType;
    DirectX::BoundingOrientedBox        collisionBox;
    bool                        isCollisionTrue;

    bool                        isActivated = false;
    bool                        isExploding = false;
    bool                        isDead = false;
    bool                        isReadyToDelete = false;
    bool                        isVehicleInCameraFrustum = true;
    float                       deleteCountDownTimer = 1.0f;
    Utility::ImpactForce        impactForce;
    std::vector<Utility::ImpactForce> impactForceVec;
    DirectX::SimpleMath::Vector3 impactForceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque              impactTorque;
    std::vector<Utility::Torque> impactTorqueVec;
    std::vector<Utility::ImpulseForce> impulseForceVec;

    DirectX::SimpleMath::Vector3    collisionImpulseForceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque              collisionImpulseTorqueSum;

    DirectX::SimpleMath::Vector3 testProjectileImpulse;
    Utility::Torque              testProjectileTorque;

    HoverData                    hoverData;
    NpcControlInput              controlInput;
    DirectX::SimpleMath::Vector3 playerPos;
    DirectX::SimpleMath::Vector3 playerVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Matrix playerAlignment = DirectX::SimpleMath::Matrix::Identity;
    JumpData                     jumpData;

    DirectX::SimpleMath::Matrix localInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localInverseInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;

    const float angularDragCoefficient = 0.8f;
    //const float tensorMass = 70.0f;
    const float tensorMass = 1.0f;
    const DirectX::SimpleMath::Vector3 tensorDimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 10.0f);

    DirectX::SimpleMath::Vector3 vehicleLinearForcesSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 vehicleAngularForcesSum = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 angularDrag = DirectX::SimpleMath::Vector3::Zero;
};

struct VehicleStruct
{
    VehicleData                 vehicleData;
    Environment                 const* environment;
    NPCModel                    npcModel;
};

enum class NpcTextureMapType
{
    TEXTUREMAPTYPE_BLANK,
    TEXTUREMAPTYPE_FLAME,
    TEXTUREMAPTYPE_TEST1,
    TEXTUREMAPTYPE_TEST2
};

class NPCVehicle
{
public:
    NPCVehicle();
    bool ActivateJump();
    void ActivateNPC();
    void CalculateImpactForce(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateImpactForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateImpulseForce(const VehicleData& aVehicleHit, DirectX::SimpleMath::Vector3 aForceVec1, DirectX::SimpleMath::Vector3 aForceVec2, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateImpulseForceFromPlayer(const float aPlayerMass, const DirectX::SimpleMath::Vector3 aPlayerVelocity, const DirectX::SimpleMath::Vector3 aPlayerCenterOfMass, DirectX::SimpleMath::Vector3 aForceVec1, DirectX::SimpleMath::Vector3 aForceVec2, const DirectX::SimpleMath::Vector3 aImpactPos);

    void CalculateImpulseForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateSelfRightingTorque();

    void CheckIfInCameraFrustum(const DirectX::BoundingFrustum& aFrustum);

    void DebugToggleAI();
    void DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawNPC2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawNPC3(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawNPC4(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment,
        std::shared_ptr<NPCController> aNpcController, std::shared_ptr<Vehicle> aPlayer, const unsigned int aID);
    void InitializeTextureMaps(NpcTextureMapType aTextureMapType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap);

    DirectX::SimpleMath::Matrix GetAlignment() const { return m_vehicleStruct00.vehicleData.alignment; };
    DirectX::BoundingOrientedBox GetAvoidanceBox() const { return m_npcAI->GetAiAvoidanceBox(); };
    bool GetAvoidanceIsTrue() const { return m_npcAI->GetIsAvoidanceTrue(); };
    int GetAvoidanceTargetIndex() const { return m_avoidanceTargetIndex; };
    float GetAvoidanceRadius() const { return m_npcAI->GetAvoidanceRadius(); };
    NPCVehicle const* GetAvoidanceNPCTarget() { return m_npcAI->GetAvoidanceTarget(); };

    const DirectX::SimpleMath::Vector3 GetCenterOfMass() const { return m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos; };

    const DirectX::BoundingOrientedBox& GetCollisionData() const { return m_vehicleStruct00.vehicleData.collisionBox; };
    float GetCollisionDetectionRange() const { return m_vehicleStruct00.vehicleData.maxCollisionDetectionRange; };
    float GetDelta() { return m_vehicleStruct00.npcModel.maxDelta; };
    DirectX::SimpleMath::Vector3 GetDimensions() const { return m_vehicleStruct00.vehicleData.dimensions; };
    DirectX::SimpleMath::Vector3 GetForward() const { return m_vehicleStruct00.vehicleData.forward; };
    float GetHeight() const { return m_vehicleStruct00.vehicleData.q.position.y; };
    unsigned int GetID() const { return m_vehicleStruct00.vehicleData.id; };

    bool GetIsActivated() const { return m_vehicleStruct00.vehicleData.isActivated; };
    bool GetIsExploding() const { return m_vehicleStruct00.vehicleData.isExploding; };
    bool GetIsDead() const { return m_vehicleStruct00.vehicleData.isDead; };
    bool GetIsDebugPauseToggleTrue() const { return m_isDebugPauseToggleTrue; };
    bool GetIsJumpActive() const { return m_vehicleStruct00.vehicleData.jumpData.isJumpActive; };
    bool GetIsJumpOnCoolDown() const { return m_vehicleStruct00.vehicleData.jumpData.isJumpOnCoolDown; };
    bool GetIsJumpReady() const { return m_vehicleStruct00.vehicleData.jumpData.isJumpReady; };
    bool GetIsInCameraFrustrum() const { return m_vehicleStruct00.vehicleData.isVehicleInCameraFrustum; };
    bool GetIsReadyToDelete() const { return m_vehicleStruct00.vehicleData.isReadyToDelete; };
    float GetMaxSteeringAngle() const { return m_vehicleStruct00.vehicleData.controlInput.steeringInputMax; };
    float GetMaxTurnRate() const { return m_vehicleStruct00.vehicleData.hoverData.turnRateMax; };

    static VehicleData GetNewNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);

    int GetNodesHit() const { return m_npcAI->GetNodesReachCount(); };

    DirectX::SimpleMath::Vector3 GetPos() const { return m_vehicleStruct00.vehicleData.q.position; };
    DirectX::SimpleMath::Vector3 GetRight() const { return m_vehicleStruct00.vehicleData.right; };
    DirectX::SimpleMath::Vector3 GetTestAccelVec() const { return m_testAccelVec; };
    float GetTopSpeedCalculated() const { return m_vehicleStruct00.vehicleData.topSpeedCalculated; };
    DirectX::SimpleMath::Vector3 GetUp() const { return m_vehicleStruct00.vehicleData.up; };
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_vehicleStruct00.vehicleData.q.velocity; };
    VehicleData GetVehicleData() { return m_vehicleStruct00.vehicleData; };

    float GetTestTimer() const { return static_cast<float>(m_testTimer); };
    void PushAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle) { m_npcAI->PushAiAvoidanceTarget(aAvoidancePos, aVehicle); };
    void PushImpactForce(Utility::ImpactForce aImpact) { m_vehicleStruct00.vehicleData.impactForceVec.push_back(aImpact); };
    //void PushImpactTorque(Utility::Torque aTorque) { m_vehicleStruct00.vehicleData.impactTorqueVec.push_back(aTorque); };
    //void PushImpulseForce(Utility::ImpulseForce aImpulse) { m_vehicleStruct00.vehicleData.impulseForceVec.push_back(aImpulse); };
    void PushImpactTorque(Utility::Torque aTorque);
    void PushImpulseForce(Utility::ImpulseForce aImpulse);

    void SetAlignment(const DirectX::SimpleMath::Matrix aMat) { m_vehicleStruct00.vehicleData.alignment = aMat; };
    void SetPos(const DirectX::SimpleMath::Vector3 aPos) { m_vehicleStruct00.vehicleData.q.position = aPos; };
    void SetVelocity(const DirectX::SimpleMath::Vector3 aVelocity) { m_vehicleStruct00.vehicleData.q.velocity = aVelocity; };
    bool GetDebugVal() { return m_isDebugToggleTrue; };

    void SetCollisionVal(const bool aIsCollisionTrue);
    void SetExplosionTrue() { m_vehicleStruct00.vehicleData.isExploding = true; };
    void SetDeadTrue() { m_vehicleStruct00.vehicleData.isDead = true; };
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNpcType(NPCType aNPCType);
    void TestCollisionVelocityUpdate(const DirectX::SimpleMath::Vector3 aVelocity) { m_vehicleStruct00.vehicleData.q.velocity = aVelocity; };
    void TestPositionChange() {
        ActivateJump();
    };

    void UnlockJump() {
        m_vehicleStruct00.vehicleData.jumpData.isJumpUnlocked = true;
        m_vehicleStruct00.vehicleData.jumpData.isJumpReady = true;
    };

    void UpdateNPC(const double aTimeDelta);
    void UpdatePlayerAlignment(const DirectX::SimpleMath::Matrix aAlignment);
    void UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos);
    void UpdatePlayerVelocity(const DirectX::SimpleMath::Vector3 aPlayerVelocity);

private:
    void ActivateJumpLanding();
    DirectX::SimpleMath::Vector3 CalculateDragAngular(const DirectX::SimpleMath::Vector3 aAngVelocity);
    DirectX::SimpleMath::Vector3 CalculateDragAngular2(const DirectX::SimpleMath::Vector3 aAngVelocity);
    DirectX::SimpleMath::Vector3 CalculateDragAngular3(const DirectX::SimpleMath::Vector3 aAngVelocity);
    DirectX::SimpleMath::Vector3 CalculateDragAngularLocal(const DirectX::SimpleMath::Vector3 aAngVelocity);
    void CalculateTopSpeed();
    bool CheckVehiclePenetration(DirectX::SimpleMath::Vector3 aPos);

    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetBuoyancyForce(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetDamperForce(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetForwardThrust(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetImpactForceSum(const VehicleData& aVehicleData);
    Utility::Torque GetImpactTorqueSum(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetHoverLift(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetOmniDirectionalThrust(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetRepulsionForce(const VehicleData& aRepulsorVehicleData);

    static void InitializeNPCStruct(VehicleStruct& aVehicleStruct,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition,
        const NPCType aNPCType, Environment const* aEnvironment);

    static void InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCModel& aModel, VehicleHardPoints& aHardPoints, const DirectX::SimpleMath::Vector3 aDimensions);

    void RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ);
    void RightHandSide2(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ);
    void RungeKutta4(struct VehicleData* aVehicle, double aTimeDelta);

    void TerrainImpactHandling();

    void UpdateAlignmentNew(const float aTimeDelta);
    void UpdateAlignment();
    void UpdateAlignment2();
    void UpdateAngularDrag(const float aTimeDelta);

    Utility::Torque UpdateBodyTorqueRunge(DirectX::SimpleMath::Vector3& aAngVec, const float aTimeStep);
    DirectX::SimpleMath::Vector3 UpdateBodyTorqueRungeLocal(const float aTimeStep);
    DirectX::SimpleMath::Vector3 UpdateBodyTorqueRungeLocalNew(const float aTimeStep);
    Utility::Torque UpdateBodyTorqueRungeOld(DirectX::SimpleMath::Vector3& aAngVec, const float aTimeStep);
    void UpdateControlInput();
    void UpdateControlInputFromAi();
    void UpdateForceTorqueVecs();
    void UpdateHardPoints();
    void UpdateImpulseForces(const float aTimeDelta);
    void UpdateJumpData(JumpData& aJumpData, const float aTimeDelta);
    void UpdateNPCModel(const double aTimeDelta);
    void UpdateVehicleForces(const float aTimeStep);

    Environment const* m_environment;

    std::shared_ptr<DebugData> m_debugData;

    std::unique_ptr<NpcAI> m_npcAI;
    std::shared_ptr<NPCController> m_npcController;

    VehicleStruct m_vehicleStruct00;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_context;

    double m_testTimer = 0.0f;
    bool m_isGoToggleTrue = false;

    float m_testMaxVelocity = 0.0f;

    int m_avoidanceTargetIndex = -1;

    float m_launchAltitude = 0.0f;
    float m_altitudeDelta = 0.0f;
    float m_prevMaxAltitudeDelta = 0.0f;
    float m_launchVelocityY = 0.0f;
    float m_launchForce = 0.0f;
    float m_forceMod1 = 0.0f;
    float m_forceMod2 = 0.0f;
    float m_forceMod3 = 0.0f;
    float m_testRatio1 = 0.0f;
    float m_testRatio2 = 0.0f;
    float m_testRatio3 = 0.0f;
    float m_launchForceRatio = 0.0f;

    float m_prevYvelocityStep = 0.0f;
    float m_prevYvelocityStepHold = 0.0f;
    float m_prevYvelocityStepHoldTest = 0.0f;
    float m_prevYvelocityQ = 0.0f;
    float m_prevYvelocityQHold = 0.0f;
    float m_prevYvelocityQHoldTest = 0.0f;

    float m_forceUsed = 0.0f;

    DirectX::SimpleMath::Vector3 m_buoyancyTestForce = DirectX::SimpleMath::Vector3::Zero;
    float m_testHoverFlutter = 0.0f;

    float m_testMaxAlt = 0.0f;
    float m_testValForce = 0.0f;

    DirectX::SimpleMath::Vector3 m_lastImpactPos = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 m_testAccelVec = DirectX::SimpleMath::Vector3::Zero;

    float m_testLightRotation = 0.0f;
    float m_testShapeRotation = 0.0f;

    bool m_isDebugPauseToggleTrue = false;
    bool m_isDebugToggleTrue = false;
    DirectX::SimpleMath::Vector3 m_testTorqueVec = DirectX::SimpleMath::Vector3::Zero;

    //const DirectX::SimpleMath::Vector3 m_torqueDebugTest = DirectX::SimpleMath::Vector3(0.0f, 118.0f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_torqueDebugTest = DirectX::SimpleMath::Vector3(0.0f, 50.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_torqueDebugTest = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3 m_angDampTest1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_angDampTest2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_angDampTest3 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_angDampTest4 = DirectX::SimpleMath::Vector3::Zero;

    const float m_angularDampConst = 0.9f;
    const float m_angDragCoefficient = 0.4f;

public:
    void ResetDebugPauseToggle() { m_isDebugPauseToggleTrue = false; };
    void ToggleDebugBool();
    DirectX::SimpleMath::Vector3 m_prevImpact = DirectX::SimpleMath::Vector3::Zero;

};

