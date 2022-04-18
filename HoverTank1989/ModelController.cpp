#include "pch.h"
#include "ModelController.h"


void ModelController::DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection)
{
    //m_testModel2->Draw(deviceContext, states, aWorld, aView, aProjection);
    DrawTank(m_playerModel, deviceContext, states, aWorld, aView, aProjection);
}

void ModelController::DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection)
{
    //aModel.barrelModel->Draw(deviceContext, states, aWorld, aView, aProjection);
    //aModel.bodyModel->Draw(deviceContext, states, aWorld, aView, aProjection);
    //aModel.turretModel->Draw(deviceContext, states, aWorld, aView, aProjection);

    aModel.barrelModel->Draw(deviceContext, states, aModel.barrelWorldMatrix, aView, aProjection);
    //aModel.bodyModel->Draw(deviceContext, states, aModel.bodyWorldMatrix, aView, aProjection);
    aModel.turretModel->Draw(deviceContext, states, aModel.turretWorldMatrix, aView, aProjection);
}

void ModelController::InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret)
{
    aModel.barrelModel = aBarrel;
    aModel.bodyModel = aBody;
    aModel.turretModel = aTurret;
  
    aModel.bodyLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.bodyLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;

    aModel.turretLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    DirectX::SimpleMath::Vector3 turretTrans = DirectX::SimpleMath::Vector3(1.9f, 1.6f, 0.0f);
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(turretTrans);
    aModel.turretWorldMatrix = aModel.turretLocalMatrix;

    aModel.barrelLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aModel.barrelLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    /*
    DirectX::SimpleMath::Vector3 barrelTrans = turretTrans;
    barrelTrans.y += -0.2f;
    barrelTrans.x += 1.85f;
    */
    DirectX::SimpleMath::Vector3 barrelTrans = DirectX::SimpleMath::Vector3(1.85f, -0.0f, 0.0f);
    aModel.barrelLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(barrelTrans);
    aModel.barrelWorldMatrix = aModel.barrelLocalMatrix;
}

void ModelController::UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
{
    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);
    DirectX::SimpleMath::Matrix updateMat = aAlignment;
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;
    aModel.bodyWorldMatrix *= updateMat;

    DirectX::SimpleMath::Matrix turretMat = DirectX::SimpleMath::Matrix::CreateRotationY(aTurretRotation);
    aModel.turretWorldMatrix = turretMat;
    aModel.turretWorldMatrix *= aModel.turretLocalMatrix;
    aModel.turretWorldMatrix *= updateMat;

    DirectX::SimpleMath::Matrix barrelMat = DirectX::SimpleMath::Matrix::CreateRotationX(aBarrelPitch);
    aModel.barrelWorldMatrix = barrelMat;
    aModel.barrelWorldMatrix *= aModel.turretLocalMatrix;

    aModel.barrelWorldMatrix *= turretMat;
    aModel.barrelWorldMatrix = aModel.barrelLocalMatrix;
    aModel.barrelWorldMatrix *= barrelMat;
    aModel.barrelWorldMatrix *= turretMat;
    aModel.barrelWorldMatrix *= updateMat;

    DirectX::SimpleMath::Vector3 testPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Matrix testMat = aModel.barrelWorldMatrix;
    //testMat.Translation(testPos);
    DirectX::SimpleMath::Vector3 testPos2 = testPos;
    DirectX::SimpleMath::Vector3 testPos3 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion testQuat;

    testMat.Decompose(testPos, testQuat, testPos3);
    //m_debugData->DebugPushTestLinePositionIndicator(testPos3, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    //DirectX::SimpleMath::Matrix testMat2 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(aTurretRotation, aBarrelPitch, 0.0f);
    DirectX::SimpleMath::Matrix testMat2 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(aTurretRotation, 0.0, aBarrelPitch);
    DirectX::SimpleMath::Vector3 testAim = DirectX::SimpleMath::Vector3::UnitX;
    testAim = DirectX::SimpleMath::Vector3::Transform(testAim, testMat2);
    m_debugData->DebugPushTestLine(testPos3, testAim, 6.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
  
}

void ModelController::UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
{
    UpdateModel(m_playerModel, aAlignment, aPos, aBarrelPitch, aTurretRotation);
}

void ModelController::InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret)
{
    InitializeModel(m_playerModel, aBarrel, aBody, aTurret);
}

//void ModelController::InitializeModelController(std::unique_ptr<DX::DeviceResources> aDevice, std::unique_ptr<DirectX::IEffectFactory>& aFxFactory)
void ModelController::InitializeModelController(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, std::unique_ptr<DirectX::IEffectFactory> aFxFactory)
{
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;
    //std::unique_ptr<DirectX::Device
    //std::unique_ptr<DX::DeviceResources>    m_deviceResources;
    //auto device = m_deviceResources->GetD3DDevice();
    
    //auto device = aDevice->GetD3DDevice();


    //auto m_fxFactory = std::make_unique<DirectX::EffectFactory>(device);
    //m_testModel = DirectX::Model::CreateFromCMO(aDevice, L"HoverTankBody02.cmo", aFxFactory);


}

void ModelController::InitModelData(DirectX::Model aModel)
{
    m_modelData = aModel;
    m_testModel = std::make_unique<DirectX::Model>(m_modelData);
}

void ModelController::InitModel2(std::shared_ptr<DirectX::Model> aModel)
{
    //m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_testModel2 = std::make_shared<DirectX::Model>();
    m_testModel2 = aModel;
}

void ModelController::InitModel3(std::shared_ptr<DirectX::Model> aModel)
{
    //m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_testModel3 = std::make_unique<DirectX::Model>();
    //m_testModel3 = aModel;
}

void ModelController::InitModel(std::unique_ptr<DirectX::Model> aModel)
{
    m_testModel = std::make_unique<DirectX::Model>();
    //m_testModel = aModel;
    //m_testModel = aModel;
}

void ModelController::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}