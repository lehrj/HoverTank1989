#pragma once
#include "VehicleBase.h"


class VehicleHover : public VehicleBase
{
public:
    void InitializeVehicleHover(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
        const DirectX::SimpleMath::Vector3 aHeading,
        const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr);
    void Update(const double aTimeDelta);
protected:
    struct HoverModel
    {
        std::unique_ptr<DirectX::GeometricPrimitive>    modelShape;
        DirectX::SimpleMath::Matrix localModelMatrix;
        DirectX::SimpleMath::Matrix worldModelMatrix;
    };
    struct HoverVehicle
    {
        HoverModel model;
        Vehicle vehicle;
    };
private:
    HoverVehicle m_hoverVehicle;
};

