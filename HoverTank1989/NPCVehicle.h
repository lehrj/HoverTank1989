#pragma once
#include "Environment.h"


struct MotionNPC
{
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
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
    DirectX::SimpleMath::Vector3 centerOfMass;
    DirectX::SimpleMath::Vector3 localCenterOfMass;
    DirectX::SimpleMath::Vector3 weaponDirection;
    DirectX::SimpleMath::Vector3 localWeaponDirection;
    DirectX::SimpleMath::Vector3 weaponPos;
    DirectX::SimpleMath::Vector3 localWeaponPos;
};

struct VehicleData
{
    Utility::Torque             bodyTorqueForce;
    //Environment                 const* environment;
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
    DirectX::BoundingSphere     collisionBox;
    bool                        isCollisionTrue;
    //NPCModel                    npcModel;
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
    void UpdateNPCModel(const double aTimeDelta);
    

    //VehicleData m_vehicleData;
    VehicleStruct m_vehicleStruct00;
};

