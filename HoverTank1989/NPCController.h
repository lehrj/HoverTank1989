#pragma once
#include "NPCVehicle.h"
#include "FireControl.h"

class Vehicle;
class FireControl;

class NPCController
{
public:
    NPCController();
    ~NPCController();
    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController);

    bool CheckExplosionCollisions(DirectX::BoundingSphere aBoundingSphere);

    bool CheckProjectileCollisions(Utility::CollisionData& aProjectile, unsigned int& aVehicleHitId);
    void DebugToggleAI();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawNPCs2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);

    DirectX::SimpleMath::Vector3 GetNpcAccelVecTest(const unsigned int aId);
    DirectX::SimpleMath::Matrix GetNpcAlignment(const unsigned int aId);
    int GetNpcCount() { return static_cast<int>(m_npcVec.size()); };
    DirectX::SimpleMath::Vector3 GetNpcPos(const unsigned int aId);
    std::vector<NPCVehicle*>& GetVec() { return m_npcVec; };
    std::vector<DirectX::SimpleMath::Vector3> GetVecOfNpcPos(const unsigned int aSelfID);

    void LoadNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController);

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetFireControl(std::shared_ptr<FireControl> aFireControlPtr);
    void SetNPCEnvironment(Environment const* aEnvironment);
    void SetPlayer(Vehicle const* aVehicle);
    void SetPlayer(std::shared_ptr<Vehicle> aVehicle);
    void SetVehicleDeath(const unsigned int aVehicleId);
    void TestPositionChange();
    void UnlockJumpAbility();
    void UpdateNPCs(const double aTimeDelta);
    void UpdateNPCController(const double aTimeDelta);

private:
    void CheckNpcAvoidance();
    void CheckNpcCollisions();
    unsigned int GetUniqueID();

    Environment const* m_environment;
    std::shared_ptr<Vehicle> m_player;
    std::shared_ptr<DebugData> m_debugData;
    std::shared_ptr<FireControl> m_fireControl;
    std::vector<NPCVehicle*> m_npcVec;

    unsigned int m_nextUniqueID = 0;
    float m_testTimer = 0.0f;    
};

