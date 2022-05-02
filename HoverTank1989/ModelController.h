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
};


class ModelController
{
public:

    void InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);

    //void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCController* aNPCController);
    //void InitializeModelController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    
    
    //ID3D11Device
    //void InitializeModelController(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, std::unique_ptr<DirectX::IEffectFactory> aFxFactory);
    //void InitializeModelController(std::unique_ptr<DX::DeviceResources> aDevice, std::unique_ptr<DirectX::IEffectFactory>& aFxFactory);
    //std::unique_ptr<DX::DeviceResources>

    //void InitModel(std::unique_ptr<DirectX::Model> aModel);
    //void InitModel2(std::shared_ptr<DirectX::Model> aModel);
    //void InitModel3(std::shared_ptr<DirectX::Model> aModel);
    //void InitModelData(DirectX::Model aModel);

    void DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);

    DirectX::SimpleMath::Vector3 GetWeaponPos() const { return m_playerModel.weaponPosWorld; };
    DirectX::SimpleMath::Vector3 GetWeaponDir() const { return m_playerModel.weaponDirWorld; };

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation);


private:

    void InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret);
    void DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);
    void UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation);


    TankModel m_playerModel;

    std::shared_ptr<DebugData> m_debugData;

    std::unique_ptr<DirectX::Model> m_testModel;
    std::shared_ptr<DirectX::Model> m_testModel2;
    std::unique_ptr<DirectX::Model> m_testModel3;
    //DirectX::Model m_modelData;


};

