#pragma once
#include "VehicleBase.h"

class VehicleNPC : public VehicleBase
{
public:
    void InitializeNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr);
    void UpdateNPC(const double aTimeDelta);
    void Update(const double aTimeDelta);
protected:
    struct NPC
    {
        //VehicleData vehicleData;
        Vehicle vehicle;
        //Environment const* environment;
        std::unique_ptr<NpcAI> vAI;
    };

private:
    //std::unique_ptr<NpcAI> m_npcAI;
    NPC m_npc;

};

