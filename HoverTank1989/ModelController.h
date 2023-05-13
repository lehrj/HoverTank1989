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
    //void InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);
    void InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    void DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    DirectX::SimpleMath::Matrix GetMuzzleDirMat() const { return m_playerModel.muzzleWorldMatrix; };
    DirectX::SimpleMath::Vector3 GetMuzzlePos() const { return m_playerModel.muzzlePosWorld; };
    DirectX::SimpleMath::Vector3 GetLocalizedMuzzlePos() const { return m_playerModel.localizedMuzzlePos; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_playerModel.weaponPosWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponDirLocal() const { return m_playerModel.weaponDirLocal; };
    DirectX::SimpleMath::Vector3 GetWeaponDirWorld() const { return m_playerModel.weaponDirWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponUpLocal() const { return m_playerModel.weaponUpLocal; };
    DirectX::SimpleMath::Vector3 GetWeaponUpWorld() const { return m_playerModel.weaponUpWorld; };

    void SetEnvironment(Environment const* m_environment);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetGlowColors(const DirectX::SimpleMath::Vector4 aColorCenter, const DirectX::SimpleMath::Vector4 aColorLeft, const DirectX::SimpleMath::Vector4 aColorRight, const DirectX::SimpleMath::Vector3 aLightDir);
    void SetGlowVals(const float aCenterVal, const float aLeftVal, const float aRightVal, const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aDir, const float aTimeStep);
    void UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane);
    //void UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane, const DirectX::SimpleMath::Vector4 aGlowColor);

private:
    
    //void InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);
    void InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    void DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawTank2(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);
    void UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane);

    const DirectX::SimpleMath::Vector3 m_centerOfMassOffset = DirectX::SimpleMath::Vector3(0.0f, -0.5f, 0.0f);

    TankModel m_playerModel;
    //std::shared_ptr<Vehicle> m_player;
    std::shared_ptr<DebugData> m_debugData;
    Environment const* m_environment;

    //std::unique_ptr<DirectX::Model> m_testModel;
    //std::shared_ptr<DirectX::Model> m_testModel2;
    //std::unique_ptr<DirectX::Model> m_testModel3;

    //DirectX::SimpleMath::Vector3 m_glowLightDirection = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 m_testPos = DirectX::SimpleMath::Vector3::Zero;
};

