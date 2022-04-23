#pragma once
#include "pch.h"
#include "Utility.h"
#include "Environment.h"
#include "FireControl.h"
#include "ModelController.h"


class VehicleBase
{
public:

protected:
    struct Motion
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 velocity;
        Utility::Torque              bodyTorqueForce;
    };

    struct VehicleData
    {
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
        //NPCType                     npcType;
        //DirectX::BoundingBox        collisionBox;
        //bool                        isCollisionTrue;

        //Utility::ImpactForce        impactForce;
        //Utility::Torque             impactTorque;

        //HoverData                    hoverData;
        //NpcControlInput              controlInput;
    };


private:



};

