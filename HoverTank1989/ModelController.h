#pragma once
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

    std::shared_ptr<DirectX::Model> bodyModel;
    DirectX::SimpleMath::Matrix bodyLocalMatrix;
    DirectX::SimpleMath::Matrix bodyWorldMatrix;

    std::shared_ptr<DirectX::Model> turretModel;
    DirectX::SimpleMath::Matrix turretLocalMatrix;
    DirectX::SimpleMath::Matrix turretOffSetMatrix;
    DirectX::SimpleMath::Matrix turretWorldMatrix;

    DirectX::SimpleMath::Vector3 weaponPosLocal;
    DirectX::SimpleMath::Vector3 weaponPosWorld;
    DirectX::SimpleMath::Vector3 weaponDirLocal;
    DirectX::SimpleMath::Vector3 weaponDirWorld;

    DirectX::SimpleMath::Vector3 localizedMuzzlePos;
    DirectX::SimpleMath::Vector3 muzzlePosLocal;
    DirectX::SimpleMath::Vector3 muzzlePosWorld;
    DirectX::SimpleMath::Matrix  muzzleLocalMatrix;
    DirectX::SimpleMath::Matrix  muzzleWorldMatrix;
    DirectX::SimpleMath::Matrix  muzzleTransMatrix;

};


class ModelController
{
public:
    void InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);
    void DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);

    DirectX::SimpleMath::Matrix GetMuzzleDirMat() const { return m_playerModel.muzzleWorldMatrix; };
    DirectX::SimpleMath::Vector3 GetMuzzlePos() const { return m_playerModel.muzzlePosWorld; };
    DirectX::SimpleMath::Vector3 GetLocalizedMuzzlePos() const { return m_playerModel.localizedMuzzlePos; };
    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_playerModel.weaponPosWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponDir() const { return m_playerModel.weaponDirWorld; };

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation);

private:
    void InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);
    void DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);
    void UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation);

    TankModel m_playerModel;

    std::shared_ptr<DebugData> m_debugData;

    std::unique_ptr<DirectX::Model> m_testModel;
    std::shared_ptr<DirectX::Model> m_testModel2;
    std::unique_ptr<DirectX::Model> m_testModel3;
};

