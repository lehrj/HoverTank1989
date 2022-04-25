#include "pch.h"
#include "VehiclePlayer.h"


void VehiclePlayer::InitializePlayer(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr)
{
    InitializeVehicle(aContext, aHeading, aPosition, aEnvironment, aDebugPtr, m_player.vehicle);
}