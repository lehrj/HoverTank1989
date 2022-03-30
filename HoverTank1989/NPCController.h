#pragma once
//#include "NPCController.h"
#include "NPCVehicle.h"


class NPCController
{
public:
    NPCController();
    ~NPCController();
    void DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment);
    void UpdateNPCController(const double aTimeDelta);
    void DrawNPCs();
    
    void SetNPCEnvironment(Environment const* aEnvironment);


    void AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, 
        const DirectX::SimpleMath::Vector3 aPosition);
    void UpdateNPCs(const double aTimeDelta);

private:
    Environment const* m_environment;

    std::vector<VehicleData*> m_npcVec2;
    std::vector<NPCVehicle*> m_npcVec;
};

