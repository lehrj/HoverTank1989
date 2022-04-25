#include "pch.h"
#include "VehicleNPC.h"


void VehicleNPC::InitializeNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr)
{
    InitializeVehicle(aContext, aHeading, aPosition, aEnvironment, aDebugPtr, m_npc.vehicle);
    //VehicleBase::InitializeVehicle(aContext, aHeading, aPosition, aEnvironment, aDebugPtr);

}

void VehicleNPC::Update(const double aTimeDelta)
{
    VehicleBase::Update(aTimeDelta);
    int number = 1;
    number++;
}

void VehicleNPC::UpdateNPC(const double aTimeDelta)
{
    int number = 2;
    number++;
}