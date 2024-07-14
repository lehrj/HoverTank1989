#pragma once
#include "Environment.h"
#include "DeviceResources.h"
#include "DebugData.h"
#include "Utility.h"
#include <memory>


struct TankModel
{
    std::shared_ptr<DirectX::Model> barrelModel;
    DirectX::SimpleMath::Matrix barrelLocalMatrix;
    DirectX::SimpleMath::Matrix barrelWorldMatrix;
    DirectX::SimpleMath::Matrix barrelTransMatrix;
    DirectX::SimpleMath::Matrix barrelShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    std::shared_ptr<DirectX::Model> bodyModel;
    DirectX::SimpleMath::Matrix bodyLocalMatrix;
    DirectX::SimpleMath::Matrix bodyWorldMatrix;
    DirectX::SimpleMath::Matrix bodyShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Matrix bodyShadowLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix bodyShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;

    std::shared_ptr<DirectX::Model> turretModel;
    DirectX::SimpleMath::Matrix turretLocalMatrix;
    DirectX::SimpleMath::Matrix turretOffSetMatrix;
    DirectX::SimpleMath::Matrix turretWorldMatrix;
    DirectX::SimpleMath::Matrix turretShadowMatrix = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3 weaponPosLocal;
    DirectX::SimpleMath::Vector3 weaponPosWorld;
    DirectX::SimpleMath::Vector3 weaponDirLocal;
    DirectX::SimpleMath::Vector3 weaponDirWorld;
    DirectX::SimpleMath::Vector3 weaponUpLocal;
    DirectX::SimpleMath::Vector3 weaponUpWorld;

    DirectX::SimpleMath::Vector3 localizedMuzzlePos;
    DirectX::SimpleMath::Vector3 muzzlePosLocal;
    DirectX::SimpleMath::Vector3 muzzlePosWorld;
    DirectX::SimpleMath::Matrix  muzzleLocalMatrix;
    DirectX::SimpleMath::Matrix  muzzleWorldMatrix;
    DirectX::SimpleMath::Matrix  muzzleTransMatrix;

    // missile tubes
    DirectX::SimpleMath::Vector3 localMissileTubeLeftPos = DirectX::SimpleMath::Vector3(-0.9f, -0.3f, -0.9f);
    DirectX::SimpleMath::Vector3 localMissileTubeRightPos = DirectX::SimpleMath::Vector3(0.9f, -0.3f, -0.9f);

    DirectX::SimpleMath::Vector3 localizedTubeLeftPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 localizedTubeRightPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3 worldMissileTubeLeftPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightPos = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 turretLocalMissileTubeLeftDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 turretLocalMissileTubeRightDir = DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Vector3 localMissileTubeLeftDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 localMissileTubeRightDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 worldMissileTubeLeftDir = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightDir = DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Vector3 localMissileTubeLeftUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 localMissileTubeRightUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 turretLocalMissileTubeLeftUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 turretLocalMissileTubeRightUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 worldMissileTubeLeftUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 worldMissileTubeRightUp = DirectX::SimpleMath::Vector3::UnitY;

    DirectX::SimpleMath::Matrix  targetingMatrix = DirectX::SimpleMath::Matrix::Identity;

    // engine glows
    std::unique_ptr<DirectX::GeometricPrimitive>    frontGlowCenterShape;
    DirectX::SimpleMath::Matrix localFrontGlowCenterMatrix;
    DirectX::SimpleMath::Matrix worldFrontGlowCenterMatrix;
    DirectX::SimpleMath::Vector4 frontGlowCenterColor;
    float glowFrontVal = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearGlowCenterShape;
    DirectX::SimpleMath::Matrix localRearGlowCenterMatrix;
    DirectX::SimpleMath::Matrix worldRearGlowCenterMatrix;
    DirectX::SimpleMath::Vector4 rearGlowCenterColor;
    float glowCenterVal = 0.0f;

    std::unique_ptr<DirectX::GeometricPrimitive>    rearGlowSideShape;
    DirectX::SimpleMath::Matrix localRearGlowLeftMatrix;
    DirectX::SimpleMath::Matrix worldRearGlowLeftMatrix;
    DirectX::SimpleMath::Vector4 rearGlowLeftColor;
    float glowLeftVal = 0.0f;

    DirectX::SimpleMath::Matrix localRearGlowRightMatrix;
    DirectX::SimpleMath::Matrix worldRearGlowRightMatrix;
    DirectX::SimpleMath::Vector4 rearGlowRightColor;
    float glowRightVal = 0.0f;

    const float grayVal = 0.2f;
    const DirectX::SimpleMath::Vector4 glowColorMin = DirectX::SimpleMath::Vector4(grayVal, grayVal, grayVal, 0.7f);
    const DirectX::SimpleMath::Vector4 glowColorMax = DirectX::SimpleMath::Vector4(1.0f, 0.1f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector3 glowLightDirectionBase = DirectX::SimpleMath::Vector3::UnitX;
};


class ModelController
{
public:
    void InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    void DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    DirectX::SimpleMath::Matrix GetMuzzleDirMat() const { return m_playerModel.muzzleWorldMatrix; };
    DirectX::SimpleMath::Vector3 GetMuzzlePos() const { return m_playerModel.muzzlePosWorld; };
    DirectX::SimpleMath::Vector3 GetLocalizedMuzzlePos() const { return m_playerModel.localizedMuzzlePos; };   

    DirectX::SimpleMath::Vector3 GetLocalizedTubeLeftDir() const { return m_playerModel.localMissileTubeLeftDir; };
    DirectX::SimpleMath::Vector3 GetLocalizedTubeRightDir() const { return m_playerModel.localMissileTubeRightDir; };

    DirectX::SimpleMath::Vector3 GetLocalizedTubeLeftPos() const { return m_playerModel.localizedTubeLeftPos; };
    DirectX::SimpleMath::Vector3 GetLocalizedTubeRightPos() const { return m_playerModel.localizedTubeRightPos; };
 
    DirectX::SimpleMath::Matrix GetLocalizedTubeMatLeft() const { return m_missileTubeMatLeft; };
    DirectX::SimpleMath::Matrix GetLocalizedTubeMatRight() const { return m_missileTubeMatRight; };

    DirectX::SimpleMath::Vector3 GetMissileTubeDirLeft() const { return m_playerModel.worldMissileTubeLeftDir; };
    DirectX::SimpleMath::Vector3 GetMissileTubeDirRight() const { return m_playerModel.worldMissileTubeRightDir; };

    DirectX::SimpleMath::Vector3 GetMissileTubePosLeft() const { return m_playerModel.worldMissileTubeLeftPos; };
    DirectX::SimpleMath::Vector3 GetMissileTubePosRight() const { return m_playerModel.worldMissileTubeRightPos; };

    DirectX::SimpleMath::Vector3 GetMissileTubeLeftUp() const { return m_playerModel.worldMissileTubeLeftUp; };
    DirectX::SimpleMath::Vector3 GetMissileTubeRightUp() const { return m_playerModel.worldMissileTubeRightUp; };

    DirectX::SimpleMath::Vector3 GetMissileTubeTurretLocalLeftUp() const { return m_playerModel.turretLocalMissileTubeLeftUp; };
    DirectX::SimpleMath::Vector3 GetMissileTubeTurretLocalRightUp() const { return m_playerModel.turretLocalMissileTubeRightUp; };

    DirectX::SimpleMath::Vector3 GetMissileTubeTurretLocalLeftDir() const { return m_playerModel.turretLocalMissileTubeLeftDir; };
    DirectX::SimpleMath::Vector3 GetMissileTubeTurretLocalRightDir() const { return m_playerModel.turretLocalMissileTubeRightDir; };

    DirectX::SimpleMath::Matrix GetTargetingMatrix() const { return m_playerModel.targetingMatrix; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_playerModel.weaponPosWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponPosLocal() const { return m_playerModel.weaponPosLocal; };

    DirectX::SimpleMath::Vector3 GetWeaponDirLocal() const { return m_playerModel.weaponDirLocal; };
    DirectX::SimpleMath::Vector3 GetWeaponDirWorld() const { return m_playerModel.weaponDirWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponUpLocal() const { return m_playerModel.weaponUpLocal; };
    DirectX::SimpleMath::Vector3 GetWeaponUpWorld() const { return m_playerModel.weaponUpWorld; };

    void SetEnvironment(Environment const* m_environment);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetGlowColors(const DirectX::SimpleMath::Vector4 aColorCenter, const DirectX::SimpleMath::Vector4 aColorLeft, const DirectX::SimpleMath::Vector4 aColorRight, const DirectX::SimpleMath::Vector3 aLightDir);
    void SetGlowVals(const float aCenterVal, const float aLeftVal, const float aRightVal, const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aDir, const float aTimeStep);
    void UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane);

private:
    
    void DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    void UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane);

    const DirectX::SimpleMath::Vector3 m_centerOfMassOffset = DirectX::SimpleMath::Vector3(0.0f, -0.5f, 0.0f);

    TankModel m_playerModel;
    std::shared_ptr<DebugData> m_debugData;
    Environment const* m_environment;

    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;

    const float m_missileTubeVerticalRot = Utility::ToRadians(45.0f);
    const float m_missileTubeHorizontalRot = Utility::ToRadians(10.0f);

    const DirectX::SimpleMath::Matrix m_missileTubeMatLeft  = DirectX::SimpleMath::Matrix::CreateRotationZ(m_missileTubeVerticalRot) * DirectX::SimpleMath::Matrix::CreateRotationY(m_missileTubeHorizontalRot);
    const DirectX::SimpleMath::Matrix m_missileTubeMatRight = DirectX::SimpleMath::Matrix::CreateRotationZ(m_missileTubeVerticalRot) * DirectX::SimpleMath::Matrix::CreateRotationY(-m_missileTubeHorizontalRot);

};

