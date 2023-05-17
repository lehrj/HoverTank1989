#pragma once
#include "NPCVehicle.h"
#include "FireControl.h"

class Vehicle;
class FireControl;

struct LoadQueue
{
    DirectX::SimpleMath::Vector3 deployPosition;
    DirectX::SimpleMath::Vector3 deployOrientation;
    NPCType deployType;
};

struct TexturePack
{
    NpcTextureMapType textureMapType;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMap;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularMap;
};

class NPCController
{
public:
    NPCController();
    ~NPCController();

    void ActivateNPCs();
    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController);

    void AirDropNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController, const DirectX::SimpleMath::Vector3 aDropPosition,
        const DirectX::SimpleMath::Vector3 aOrientation, const float aAltitude, const unsigned int aColumnCount, const unsigned int aRowCount, const float aColumnSpacing, const float aRowSpacing);
    
    bool CheckExplosionCollisions(DirectX::BoundingSphere aBoundingSphere, const double aTimeDelta, const float aRemainingDuration);
    bool CheckProjectileCollisions(Utility::CollisionData& aProjectile, unsigned int& aVehicleHitId, const bool aIsExplosive);
    void DebugToggleAI();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawNPCs2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, 
        Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);
    void InitializeTextureMaps(NpcTextureMapType aTextureMapType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, 
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap);

    bool GetIsDebugPauseToggleTrue();

    DirectX::SimpleMath::Vector3 GetNpcAccelVecTest(const unsigned int aId);
    DirectX::SimpleMath::Matrix GetNpcAlignment(const unsigned int aId);
    int GetNpcCount() { return static_cast<int>(m_npcVec.size()); };
    DirectX::SimpleMath::Vector3 GetNpcPos(const unsigned int aId);
    std::vector<NPCVehicle*>& GetVec() { return m_npcVec; };
    std::vector<DirectX::SimpleMath::Vector3> GetVecOfNpcPos(const unsigned int aSelfID);

    void LoadNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController);
    void LoadNPCsTestFireRange(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController);
    void LoadToQueue(const DirectX::SimpleMath::Vector3 aLoadPosition, const DirectX::SimpleMath::Vector3 aHeading, const unsigned int aColumnCount, const unsigned int aRowCount, 
        const DirectX::SimpleMath::Vector2 aSpacing, const DirectX::SimpleMath::Quaternion aQuat);
    void LoadToQueueAxisAligned(const DirectX::SimpleMath::Vector3 aLoadPosition, const DirectX::SimpleMath::Vector3 aOrientation, const unsigned int aColumnCount, const unsigned int aRowCount, 
        const float aColumnSpacing, const float aRowSpacing);

    void ResetNpcDebugPauseToggle();
    void SetAllNpcsToDead();
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetFireControl(std::shared_ptr<FireControl> aFireControlPtr);
    void SetNPCEnvironment(Environment const* aEnvironment);
    void SetPlayer(std::shared_ptr<Vehicle> aVehicle);
    void SetVehicleDeath(const unsigned int aVehicleId);

    void TestPositionChange();
    void ToggleDebugBool();
    void UnlockJumpAbility();
    void UpdateLoadQueue(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController, const double aTimeDelta);
    void UpdateNPCs(const DirectX::BoundingFrustum& aFrustum, const double aTimeDelta);
    void UpdateNPCController(const DirectX::BoundingFrustum& aFrustum, const double aTimeDelta);

private:
    void CheckNpcAvoidance();
    void CheckNpcCollisions();
    void CheckPlayerCollisions();
    unsigned int GetUniqueID();

    Environment const* m_environment;
    std::shared_ptr<Vehicle> m_player;
    std::shared_ptr<DebugData> m_debugData;
    std::shared_ptr<FireControl> m_fireControl;
    std::vector<NPCVehicle*> m_npcVec;

    std::vector<LoadQueue> m_loadQueue;

    unsigned int m_nextUniqueID = 0;

    TexturePack m_textureDataBlank;
    TexturePack m_textureDataFlame;
    TexturePack m_textureDataTest1;
    TexturePack m_textureDataTest2;

    float m_testTimer = 0.0f;

    const bool m_isNpcAiOn = false;
};

