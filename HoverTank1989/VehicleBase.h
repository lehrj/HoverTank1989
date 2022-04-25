#pragma once
#include "pch.h"
#include "Utility.h"
#include "Environment.h"
#include "FireControl.h"
#include "ModelController.h"


class VehicleBase
{
public:
    void Update(const double aTimeDelta);
    
protected:
    struct Motion1
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 velocity;
        Utility::Torque              bodyTorqueForce;
    };

    struct hPoint
    {
        struct hPointType
        {
            int hType;
        };
        enum class hPointEnum
        {
            NPCTYPE_NPC00,
            NPCTYPE_NPC01,
            NPCTYPE_NPC02,
        };

        DirectX::SimpleMath::Matrix localMat;
        DirectX::SimpleMath::Matrix worldMat;
        hPointType pointType;
        hPointEnum pointEnum;
    };

    struct VehicleData
    {
        float                       altitude;
        DirectX::SimpleMath::Vector3 dimensions;

        float                       dragCoefficient;
        float                       frontalArea;
        float                       hitPoints;
        float                       mass;
        Motion1                     q;
        float                       terrainHightAtPos;
        DirectX::SimpleMath::Vector3 terrainNormal;
        float                       time;
        DirectX::SimpleMath::Matrix alignment;
        DirectX::SimpleMath::Vector3 forward;
        DirectX::SimpleMath::Vector3 right;
        DirectX::SimpleMath::Vector3 up;

        VehicleHardPoints           hardPoints;

        DirectX::SimpleMath::Vector3 updateVelocityForce;
        Utility::Torque              updateTorqueForce;
        //NPCType                     npcType;
        //DirectX::BoundingBox        collisionBox;
        //bool                        isCollisionTrue;

        //Utility::ImpactForce        impactForce;
        //Utility::Torque             impactTorque;

        //HoverData                    hoverData;
        //NpcControlInput              controlInput;
    };



    struct Vehicle
    {
        VehicleData vehicleData;
        Environment const* environment;
    };

public:
    Motion1 GetMotion();

    void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr, Vehicle& aVehicle);

protected:
    void InitializeVehicleData(const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Vehicle& aVehicle);

private:

    void RightHandSide(struct Vehicle* aVehicle, Motion1* aQ, Motion1* aDeltaQ, double aTimeDelta, float aQScale, Motion1* aDQ);
    void RightHandSideTest(struct VehicleData* aVehicle, Motion1* aQ, Motion1* aDeltaQ, double aTimeDelta, float aQScale, Motion1* aDQ);
    void RungeKutta4(struct Vehicle* aVehicle, double aTimeDelta);
    void RungeKutta4Test(struct VehicleData* aVehicle, double aTimeDelta);
    Environment const* m_environment;
    std::shared_ptr<DebugData> m_debugData;
    Vehicle m_vehicle;

};

