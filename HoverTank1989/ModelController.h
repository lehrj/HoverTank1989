#pragma once
#include "DeviceResources.h"

class ModelController
{
public:
    //void InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCController* aNPCController);
    //void InitializeModelController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext);
    
    
    //ID3D11Device
    void InitializeModelController(Microsoft::WRL::ComPtr<ID3D11Device> aDevice, std::unique_ptr<DirectX::IEffectFactory> aFxFactory);
    //void InitializeModelController(std::unique_ptr<DX::DeviceResources> aDevice, std::unique_ptr<DirectX::IEffectFactory>& aFxFactory);
    //std::unique_ptr<DX::DeviceResources>

    void InitModel(std::unique_ptr<DirectX::Model> aModel);
    void InitModel2(std::shared_ptr<DirectX::Model> aModel);
    void InitModel3(std::shared_ptr<DirectX::Model> aModel);
    void InitModelData(DirectX::Model aModel);

    void DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aWorld, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection);






private:

    std::unique_ptr<DirectX::Model> m_testModel;
    std::shared_ptr<DirectX::Model> m_testModel2;
    std::unique_ptr<DirectX::Model> m_testModel3;
    DirectX::Model m_modelData;

};

