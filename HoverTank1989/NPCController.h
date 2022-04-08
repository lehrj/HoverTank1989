#pragma once
//#include "pch.h"
#include "NPCVehicle.h"
//#include "FireControl.h"

class NPCController
{
public:
    NPCController();
    ~NPCController();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);
    void UpdateNPCController(const double aTimeDelta);
    void DrawNPCs();
    
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetNPCEnvironment(Environment const* aEnvironment);


    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, 
        const DirectX::SimpleMath::Vector3 aPosition);


    bool CheckProjectileCollisions(Utility::CollisionData& aProjectile);

    void UpdateNPCs(const double aTimeDelta);

    std::vector<NPCVehicle*>& GetVec() { return m_npcVec; };

private:
    Environment const* m_environment;
    std::shared_ptr<DebugData> m_debugData;

    std::vector<VehicleData*> m_npcVec2;
    std::vector<NPCVehicle*> m_npcVec;

    float m_testTimer = 0.0f;
};

