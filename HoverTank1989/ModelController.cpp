#include "pch.h"
#include "ModelController.h"


void ModelController::DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection)
{
    //m_testModel2->Draw(deviceContext, states, aWorld, aView, aProjection);

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