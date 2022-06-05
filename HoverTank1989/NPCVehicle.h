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
    const float steeringInputMax = 3.0f;
    const float steeringInputMin = -3.0f;
    const float steeringInputRate = 5.0f;
    DirectX::SimpleMath::Vector3 steeringVec;

    float       throttleInput;
    const float throttleInputDecayRate = 0.6f;
    const float throttleInputMin = -1.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 0.4f;
};

struct HoverData
{
    /*
    const float groundNormalForceRange = 5.0f;
    const float hoverNeutralBoyantAlt = 0.52f;
    const float hoverRangeLower = 0.5f;
    const float hoverRangeMid = 1.0f;
    const float hoverRangeUpper = 3.0f;
    */
    /*
    const float groundNormalForceRange = 5.0f;
    const float hoverNeutralBoyantAlt = 0.52f;
    const float hoverRangeLower = 1.5f;
    const float hoverRangeMid = 3.0f;
    const float hoverRangeUpper = 5.0f;
    */
    const float groundNormalForceRange = 0.2f;
    const float hoverNeutralBoyantAlt = 0.1f;
    const float hoverRangeLower = 0.0f;
    const float hoverRangeMid = 0.1f;
    const float hoverRangeUpper = 0.2f;

    float forwardThrust;
    const float forwardThrustMax = 100000.0f;
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
};

enum class NPCType
{
    NPCTYPE_NPC00,
    NPCTYPE_NPC01,
    NPCTYPE_NPC02,
};

struct NPCModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    modelShape;   
    DirectX::SimpleMath::Matrix localModelMatrix;
    DirectX::SimpleMath::Matrix worldModelMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    forwardShape;
    DirectX::SimpleMath::Matrix localForwardMatrix;
    DirectX::SimpleMath::Matrix worldForwardMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearShape;
    DirectX::SimpleMath::Matrix localRearMatrix;
    DirectX::SimpleMath::Matrix worldRearMatrix;

    std::unique_ptr<DirectX::GeometricPrimitive>    avoidanceShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    collisionShape;
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
    float                       mass;
    float                       topSpeedCalculated;
    MotionNPC                   q;
    float                       terrainHightAtPos;
    DirectX::SimpleMath::Vector3 terrainNormal;
    float                       time;
    DirectX::SimpleMath::Matrix alignment;
    DirectX::SimpleMath::Vector3 forward;
    DirectX::SimpleMath::Vector3 right;
    DirectX::SimpleMath::Vector3 up;

    VehicleHardPoints           hardPoints;
    NPCType                     npcType;
    DirectX::BoundingBox        collisionBox;
    bool                        isCollisionTrue;

    Utility::ImpactForce        impactForce;
    std::vector<Utility::ImpactForce> impactForceVec;
    DirectX::SimpleMath::Vector3 impactForceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque             impactTorque;
    std::vector<Utility::Torque> impactTorqueVec;
    DirectX::SimpleMath::Vector3 testForce;
    HoverData                    hoverData;
    NpcControlInput              controlInput;
    DirectX::SimpleMath::Vector3 playerPos;
};

struct VehicleStruct
{
    VehicleData                 vehicleData;
    Environment                 const* environment;
    NPCModel                    npcModel;
    
};

class NPCVehicle
{
public:
    NPCVehicle();

    void CalculateImpactForce(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateImpactForce2(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateImpactForce3(const VehicleData& aVehicleHit);
    void CalculateImpactForce4(const VehicleData& aVehicleHit);
    void CalculateImpactForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);

    void CalculateSelfRightingTorque();

    void DebugToggleAI();
    void DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    void InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, 
        std::shared_ptr<NPCController> aNpcController, Vehicle const* aPlayer, const unsigned int aID);

    DirectX::SimpleMath::Matrix GetAlignment() const { return m_vehicleStruct00.vehicleData.alignment; };
    DirectX::BoundingBox GetAvoidanceBox() const { return m_npcAI->GetAiAvoidanceBox(); };
    const DirectX::BoundingBox& GetCollisionData() { return m_vehicleStruct00.vehicleData.collisionBox; };
    DirectX::SimpleMath::Vector3 GetDimensions() const { return m_vehicleStruct00.vehicleData.dimensions; };
    DirectX::SimpleMath::Vector3 GetForward() const { return m_vehicleStruct00.vehicleData.forward; };
    float GetHeight() const { return m_vehicleStruct00.vehicleData.q.position.y; };
    int GetID() const { return m_vehicleStruct00.vehicleData.id; }; 
    float GetMaxSteeringAngle() const { return m_vehicleStruct00.vehicleData.controlInput.steeringInputMax; };
    float GetMaxTurnRate() const { return m_vehicleStruct00.vehicleData.hoverData.turnRateMax; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_vehicleStruct00.vehicleData.q.position; };
    DirectX::SimpleMath::Vector3 GetRight() const { return m_vehicleStruct00.vehicleData.right; };
    float GetTopSpeedCalculated() const { return m_vehicleStruct00.vehicleData.topSpeedCalculated; };
    DirectX::SimpleMath::Vector3 GetUp() const { return m_vehicleStruct00.vehicleData.up; };
    DirectX::SimpleMath::Vector3 GetVelocity() const { return m_vehicleStruct00.vehicleData.q.velocity; };
    VehicleData GetVehicleData() { return m_vehicleStruct00.vehicleData; };

    static VehicleData GetNewNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);
      
    void UpdateNPC(const double aTimeDelta);
    void PushAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos) { m_npcAI->PushAiAvoidanceTarget(aAvoidancePos); };
    void PushImpactForce(Utility::ImpactForce aImpact) { m_vehicleStruct00.vehicleData.impactForceVec.push_back(aImpact); };
    void PushImpactTorque(Utility::Torque aTorque) { m_vehicleStruct00.vehicleData.impactTorqueVec.push_back(aTorque); };
    
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetCollisionVal(const bool aIsCollisionTrue);
    
    void UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos);

    void UpdateTestForce(const DirectX::SimpleMath::Vector3 aForce, const float aVal);

    void TestCollisionVelocityUpdate(const DirectX::SimpleMath::Vector3 aVelocity) { m_vehicleStruct00.vehicleData.q.velocity = aVelocity; };

private:
    void CalculateTopSpeed();
    bool CheckVehiclePenetration(DirectX::SimpleMath::Vector3 aPos);

    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const VehicleData& aVehicleData);
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
    static void InitializeNPCModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
        NPCModel& aModel);
    static void InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCModel& aModel, const DirectX::SimpleMath::Vector3 aDimensions);

    void RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ);
    void RungeKutta4(struct VehicleData* aVehicle, double aTimeDelta);

    void UpdateAlignment();
    Utility::Torque UpdateBodyTorqueRunge(Utility::Torque aPendulumTorque, const float aTimeStep);
    void UpdateControlInput();
    void UpdateControlInputFromAi();
    void UpdateForceTorqueVecs();
    void UpdateHardPoints();
    void UpdateNPCModel(const double aTimeDelta);   

    Environment const* m_environment;
    
    std::shared_ptr<DebugData> m_debugData;

    std::unique_ptr<NpcAI> m_npcAI;
    std::shared_ptr<NPCController> m_npcController;

    VehicleStruct m_vehicleStruct00;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_context;
};

