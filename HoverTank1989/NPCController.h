#pragma once
#include "NPCVehicle.h"

class Vehicle;

class NPCController
{
public:
    NPCController();
    ~NPCController();
    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController);

    bool CheckProjectileCollisions(Utility::CollisionData& aProjectile);
    void DebugToggleAI();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);

    DirectX::SimpleMath::Vector3 GetNpcAccelVecTest(const int aId) { return m_npcVec[aId]->GetTestAccelVec(); };
    DirectX::SimpleMath::Matrix GetNpcAlignment(const int aId) { return m_npcVec[aId]->GetAlignment(); };
    int GetNpcCount() { return static_cast<int>(m_npcVec.size()); };
    DirectX::SimpleMath::Vector3 GetNpcPos(const int aId) { return m_npcVec[aId]->GetPos(); };
    std::vector<NPCVehicle*>& GetVec() { return m_npcVec; };
    std::vector<DirectX::SimpleMath::Vector3> GetVecOfNpcPos(const int aSelfID);

    void LoadNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController);

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCEnvironment(Environment const* aEnvironment);
    void SetPlayer(Vehicle const* aVehicle);
    void TestPositionChange();
    void UnlockJumpAbility();
    void UpdateNPCs(const double aTimeDelta);
    void UpdateNPCController(const DirectX::SimpleMath::Vector3 aPlayerPos, const DirectX::SimpleMath::Vector3 aPlayerVelocity, const DirectX::SimpleMath::Matrix aPlayerAlignment, const double aTimeDelta);

private:
    void CheckNpcCollisions();
    unsigned int GetUniqueID();

    Environment const* m_environment;
    Vehicle const* m_player;
    std::shared_ptr<DebugData> m_debugData;

    std::vector<NPCVehicle*> m_npcVec;

    unsigned int m_nextUniqueID = 0;
    float m_testTimer = 0.0f;    
};

