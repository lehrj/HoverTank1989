#pragma once
#include "NPCVehicle.h"


class Vehicle;

class NPCController
{
public:
    NPCController();
    ~NPCController();
    void DebugToggleAI();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);
    void UpdateNPCController(const DirectX::SimpleMath::Vector3 aPlayerPos, const double aTimeDelta);
    void DrawNPCs();
    
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetNPCEnvironment(Environment const* aEnvironment);
    void SetPlayer(Vehicle const* aVehicle);

    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, 
        const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController);


    bool CheckProjectileCollisions(Utility::CollisionData& aProjectile);

    void UpdateNPCs(const double aTimeDelta);

    std::vector<NPCVehicle*>& GetVec() { return m_npcVec; };

private:
    unsigned int GetUniqueID();

    void CheckNpcCollisions();

    Environment const* m_environment;
    Vehicle const* m_player;
    std::shared_ptr<DebugData> m_debugData;

    std::vector<NPCVehicle*> m_npcVec;

    unsigned int m_nextUniqueID = 0;
    float m_testTimer = 0.0f;
};

