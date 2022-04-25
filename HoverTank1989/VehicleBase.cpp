#include "pch.h"
#include "VehicleBase.h"



void VehicleBase::Update(const double aTimeDelta)
{
    RungeKutta4Test(&m_vehicle.vehicleData, aTimeDelta);

    int number = 0;
    number++;
}

void VehicleBase::InitializeVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, std::shared_ptr<DebugData> aDebugPtr, Vehicle& aVehicle)
{
    m_vehicle.environment = aEnvironment;
    //m_environment = aEnvironment;
    m_debugData = aDebugPtr;
    InitializeVehicleData(aHeading, aPosition, aVehicle);
}

void VehicleBase::InitializeVehicleData(const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Vehicle& aVehicle)
{
    aVehicle.vehicleData.q.position = aPosition;
    aVehicle.vehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicle.vehicleData.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicle.vehicleData.q.bodyTorqueForce.magnitude = 0.0f;

    aVehicle.vehicleData.dragCoefficient = 0.5f;
    aVehicle.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 9.5f);
    aVehicle.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 10.0f);


    aVehicle.vehicleData.frontalArea = aVehicle.vehicleData.dimensions.z * aVehicle.vehicleData.dimensions.y;
    aVehicle.vehicleData.hitPoints = 100.0f;
    aVehicle.vehicleData.mass = 1000.0f;

    aVehicle.vehicleData.terrainHightAtPos = 0.0f;
    aVehicle.vehicleData.altitude = 0.0f;
    aVehicle.vehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    aVehicle.vehicleData.time = 0.0f;
    aVehicle.vehicleData.forward = aHeading;
    aVehicle.vehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicle.vehicleData.right = aVehicle.vehicleData.forward.Cross(aVehicle.vehicleData.up);
    aVehicle.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicle.vehicleData.right, aVehicle.vehicleData.up);

    aVehicle.vehicleData.updateVelocityForce = DirectX::SimpleMath::Vector3::Zero;
    aVehicle.vehicleData.updateTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicle.vehicleData.updateTorqueForce.magnitude = 0.0f;
}


void VehicleBase::RightHandSide(struct Vehicle* aVehicle, Motion1* aQ, Motion1* aDeltaQ, double aTimeDelta, float aQScale, Motion1* aDQ)
{

}

void VehicleBase::RungeKutta4(struct Vehicle* aVehicle, double aTimeDelta)
{
    //  Define a convenience variables

    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    //MotionNPC q = aVehicle->q;
    Motion1 q = aVehicle->vehicleData.q;
    Motion1 dq1;
    Motion1 dq2;
    Motion1 dq3;
    Motion1 dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aVehicle, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aVehicle, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aVehicle, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aVehicle, &q, &dq3, aTimeDelta, 1.0, &dq4);
    //aVehicle->time = aVehicle->time + aTimeDelta;
    aVehicle->vehicleData.time += aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    //aVehicle->q.velocity = q.velocity;
    aVehicle->vehicleData.q.velocity = q.velocity;
    //aVehicle->q.position = q.position;
    aVehicle->vehicleData.q.position = q.position;
    //aVehicle->q.bodyTorqueForce = q.bodyTorqueForce;
    aVehicle->vehicleData.q.bodyTorqueForce = q.bodyTorqueForce;
}

void VehicleBase::RungeKutta4Test(struct VehicleData* aVehicle, double aTimeDelta)
{
    //  Define a convenience variables

    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    Motion1 q = aVehicle->q;
    Motion1 dq1;
    Motion1 dq2;
    Motion1 dq3;
    Motion1 dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSideTest(aVehicle, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSideTest(aVehicle, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSideTest(aVehicle, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSideTest(aVehicle, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aVehicle->time = aVehicle->time + aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aVehicle->q.velocity = q.velocity;
    aVehicle->q.position = q.position;
    aVehicle->q.bodyTorqueForce = q.bodyTorqueForce;
}


void VehicleBase::RightHandSideTest(struct VehicleData* aVehicle, Motion1* aQ, Motion1* aDeltaQ, double aTimeDelta, float aQScale, Motion1* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    //  Compute the total drag force
    const float v = newQ.velocity.Length();
    //const float airDensity = 1.225f; // ToDo rework to pull data from environment
    const float airDensity = m_vehicle.environment->GetAirDensity();
    const float dragCoefficient = aVehicle->dragCoefficient;
    const float frontSurfaceArea = aVehicle->frontalArea;
    const float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;

    float mass = aVehicle->mass;
    DirectX::SimpleMath::Vector3 velocityNorm = aVehicle->q.velocity;
    velocityNorm.Normalize();
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += aVehicle->updateVelocityForce;
    velocityUpdate += airResistance;
    Utility::Torque bodyTorqueUpdate = aVehicle->updateTorqueForce;

    aDQ->bodyTorqueForce = bodyTorqueUpdate;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}
