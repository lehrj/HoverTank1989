#include "pch.h"
#include "NPCVehicle.h"


void NPCVehicle::InitializeNPCData(VehicleData& aVehicleData, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, const NPCType aNPCType, Environment const* aEnvironment)
{
    aVehicleData.environment = aEnvironment;
    aVehicleData.q.position = aPosition;
    aVehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleData.dragCoefficient = 0.5f;
    aVehicleData.frontalArea = 4.5f;
    aVehicleData.hitPoints = 100.0f;
    aVehicleData.mass = 1000.0f;
    aVehicleData.npcType = NPCType::NPCTYPE_NPC00;
    aVehicleData.terrainHightAtPos = 0.0f;
    aVehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;

    aVehicleData.forward = aHeading;
    aVehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleData.right = aVehicleData.forward.Cross(aVehicleData.up);
    aVehicleData.alignment = DirectX::SimpleMath::Matrix::Identity;
}

void NPCVehicle::InitializeNPCModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCModel& aModel)
{
    const DirectX::SimpleMath::Vector3 vehicleSize(3.0f, 2.0f, 1.5f);
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), vehicleSize);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldModelMatrix = aModel.localModelMatrix;
}

void NPCVehicle::InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, VehicleData& aNPCVehicle, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment)
{
    InitializeNPCData(aNPCVehicle, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    InitializeNPCModel(aContext, aNPCVehicle.npcModel);
}