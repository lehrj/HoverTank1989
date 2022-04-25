#pragma once
#include "VehicleBase.h"

class VehiclePlayer : public VehicleBase
{
public:
    void InitializePlayer(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr);
protected:
    struct Player
    {
        //VehicleData vehicleData;
        Vehicle vehicle;
        //Environment const* environment;
        //std::unique_ptr<NpcAI> vAI;
    };
private:
    Player m_player;
};

