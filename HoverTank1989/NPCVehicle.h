#pragma once
#include "DebugData.h"
#include "Environment.h"
#include "NpcAI.h"
#include "Utility.h"

class NPCController;


struct NpcControlInput
{
    // input control data
    const float inputDeadZone = 0.7;  // small deadzone to ignore nominal control input

    float       throttleInput;
    const float throttleInputDecayRate = 0.6f;
    const float throttleInputMin = 0.0f;
    const float throttleInputMax = 1.0f;
    const float throttleInputRate = 0.4f;

    bool        stearingIsPressed;
    float       steeringInput;
    const float steeringDecayRate = 0.2f;
    const float steeringInputMax = 19.0f;
    const float steeringInputMin = -19.0f;
    const float steeringInputRate = 9.95f;
    DirectX::SimpleMath::Vector3 steeringVec;

    float       angleToDestination;   
};

struct HoverData
{
    const float groundNormalForceRange = 5.0f;
    const float hoverNeutralBoyantAlt = 0.52f;
    const float hoverRangeLower = 0.5f;
    const float hoverRangeMid = 1.0f;
    const float hoverRangeUpper = 3.0f;
    const float forwardThrustMax = 500.0f;
    float forwardThrust;
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
};

struct VehicleHardPoints
{
    DirectX::SimpleMath::Vector3 centerOfMassPos;
    DirectX::SimpleMath::Vector3 localCenterOfMassPos;
    DirectX::SimpleMath::Vector3 weaponDirection;
    DirectX::SimpleMath::Vector3 localWeaponDirection;
    DirectX::SimpleMath::Vector3 weaponPos;
    DirectX::SimpleMath::Vector3 localWeaponPos;

    DirectX::SimpleMath::Vector3 testArmPos;
    DirectX::SimpleMath::Vector3 localTestArmPos;

    DirectX::SimpleMath::Vector3 basePos;
    DirectX::SimpleMath::Vector3 localBasePos;

};

struct VehicleData
{   
    unsigned int                id;
    float                       altitude;
    DirectX::SimpleMath::Vector3 dimensions;
    
    float                       dragCoefficient;
    float                       frontalArea;
    float                       hitPoints;
    float                       mass;
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

    void CalculateSelfRightingTorque();

    void InitializeNPCVehicle2(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, 
        std::shared_ptr<NPCController> aNpcController, Vehicle const* aPlayer, const unsigned int aID);

    float GetHeight() const { return m_vehicleStruct00.vehicleData.q.position.y; };
    int GetID() const { return m_vehicleStruct00.vehicleData.id; };
    DirectX::SimpleMath::Vector3 GetForward() const { return m_vehicleStruct00.vehicleData.forward; };
    DirectX::SimpleMath::Vector3 GetRight() const { return m_vehicleStruct00.vehicleData.right; };
    DirectX::SimpleMath::Vector3 GetUp() const { return m_vehicleStruct00.vehicleData.up; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_vehicleStruct00.vehicleData.q.position; };

    float GetMaxTurnRate() const { return m_vehicleStruct00.vehicleData.hoverData.turnRateMax; };
    
    static VehicleData GetNewNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);

    void DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    void UpdateNPC(const double aTimeDelta);
    
    VehicleData GetVehicleData() {return m_vehicleStruct00.vehicleData; };
    const DirectX::BoundingBox& GetCollisionData() { return m_vehicleStruct00.vehicleData.collisionBox; };

    void PushImpactForce(Utility::ImpactForce aImpact) { m_vehicleStruct00.vehicleData.impactForceVec.push_back(aImpact); };
    void PushImpactTorque(Utility::Torque aTorque) { m_vehicleStruct00.vehicleData.impactTorqueVec.push_back(aTorque); };
    
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetCollisionVal(const bool aIsCollisionTrue);
    
    void UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos);

    void UpdateTestForce(const DirectX::SimpleMath::Vector3 aForce, const float aVal);

private:
    bool CheckVehiclePenetration(DirectX::SimpleMath::Vector3 aPos);

    DirectX::SimpleMath::Vector3 GetAntiGravGravityForce(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetDamperForce(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetForwardThrust(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetImpactForceSum(const VehicleData& aVehicleData);
    Utility::Torque GetImpactTorqueSum(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetHoverLift(const VehicleData& aVehicleData);
    DirectX::SimpleMath::Vector3 GetOmniDirectionalThrust(const VehicleData& aVehicleData);
    
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
    void UpdateForceTorqueVecs();
    void UpdateHardPoints();
    void UpdateNPCModel(const double aTimeDelta);
    

    Environment const* m_environment;
    
    std::shared_ptr<DebugData> m_debugData;

    std::unique_ptr<NpcAI> m_npcAI;
    std::shared_ptr<NPCController> m_npcController;

    //VehicleData m_vehicleData;
    VehicleStruct m_vehicleStruct00;
    //Vehicle const* m_player;
};

