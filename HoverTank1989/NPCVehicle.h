#pragma once
#include "Environment.h"
#include "Utility.h"
#include "DebugData.h"


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
};

struct VehicleData
{
    
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
    Utility::Torque             impactTorque;
    DirectX::SimpleMath::Vector3 testForce;
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
    //NPCVehicle();

    void CalculateImpactForce(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos);
    void CalculateSelfRightingTorque();

    static void InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
        VehicleData& aNPCVehicle, const DirectX::SimpleMath::Vector3 aHeading, 
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);
    void InitializeNPCVehicle2(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);
    static VehicleData GetNewNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment);

    void DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);

    void UpdateNPC(const double aTimeDelta);
    
    VehicleData GetVehicleData() {return m_vehicleStruct00.vehicleData; };
    const DirectX::BoundingBox& GetCollisionData() { return m_vehicleStruct00.vehicleData.collisionBox; };

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetCollisionVal(const bool aIsCollisionTrue);
    
    void UpdateTestForce(const DirectX::SimpleMath::Vector3 aForce, const float aVal);

private:
    static void InitializeNPCData(VehicleData& aVehicleData, 
        const DirectX::SimpleMath::Vector3 aHeading, 
        const DirectX::SimpleMath::Vector3 aPosition, 
        const NPCType aNPCType, Environment const* aEnvironment);
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
    void UpdateNPCModel(const double aTimeDelta);
    void UpdateHardPoints();

    //Environment const* m_environment;
    std::shared_ptr<DebugData> m_debugData;

    //VehicleData m_vehicleData;
    VehicleStruct m_vehicleStruct00;
};

