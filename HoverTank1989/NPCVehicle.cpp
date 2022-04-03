#include "pch.h"
#include "NPCVehicle.h"

/*
NPCVehicle::NPCVehicle()
{

}
*/

void NPCVehicle::DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    //m_vehicleData.npcModel.modelShape->Draw(m_vehicleData.npcModel.worldModelMatrix, aView, aProj, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_vehicleStruct00.npcModel.modelShape->Draw(m_vehicleStruct00.npcModel.worldModelMatrix, aView, aProj, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

/*
VehicleData NPCVehicle::GetNewNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment)
{
    VehicleData newVehicle;
    InitializeNPCVehicle(aContext, newVehicle, aHeading, aPosition, aEnvironment);
    return newVehicle;
}
*/

/*
void NPCVehicle::InitializeNPCData(VehicleData& aVehicleData, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, const NPCType aNPCType, Environment const* aEnvironment)
{
    //aVehicleData.environment = aEnvironment;
    aVehicleData.q.position = aPosition;
    aVehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleData.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleData.bodyTorqueForce.magnitude = 0.0f;

    aVehicleData.dragCoefficient = 0.5f;
    aVehicleData.frontalArea = 4.5f;
    aVehicleData.hitPoints = 100.0f;
    aVehicleData.mass = 1000.0f;
    aVehicleData.npcType = NPCType::NPCTYPE_NPC00;
    aVehicleData.terrainHightAtPos = 0.0f;
    aVehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleData.time = 0.0f;
    aVehicleData.forward = aHeading;
    aVehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleData.right = aVehicleData.forward.Cross(aVehicleData.up);
    aVehicleData.alignment = DirectX::SimpleMath::Matrix::Identity;
}
*/

void NPCVehicle::InitializeNPCStruct(VehicleStruct& aVehicleStruct,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition,
    const NPCType aNPCType, Environment const* aEnvironment)
{
    aVehicleStruct.environment = aEnvironment;
    aVehicleStruct.vehicleData.q.position = aPosition;
    aVehicleStruct.vehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleStruct.vehicleData.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.bodyTorqueForce.magnitude = 0.0f;

    aVehicleStruct.vehicleData.dragCoefficient = 0.5f;
    aVehicleStruct.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 9.5f);
    // to do update when transisioned from sphere to box
    aVehicleStruct.vehicleData.collisionBox.Radius = aVehicleStruct.vehicleData.dimensions.x;
    aVehicleStruct.vehicleData.collisionBox.Center = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.isCollisionTrue = false;

    aVehicleStruct.vehicleData.frontalArea = aVehicleStruct.vehicleData.dimensions.z * aVehicleStruct.vehicleData.dimensions.y;
    aVehicleStruct.vehicleData.hitPoints = 100.0f;
    aVehicleStruct.vehicleData.mass = 1000.0f;
    aVehicleStruct.vehicleData.npcType = NPCType::NPCTYPE_NPC00;
    aVehicleStruct.vehicleData.terrainHightAtPos = 0.0f;
    aVehicleStruct.vehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.time = 0.0f;
    aVehicleStruct.vehicleData.forward = aHeading;
    aVehicleStruct.vehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.right = aVehicleStruct.vehicleData.forward.Cross(aVehicleStruct.vehicleData.up);
    aVehicleStruct.vehicleData.alignment = DirectX::SimpleMath::Matrix::Identity;
}

void NPCVehicle::InitializeNPCModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCModel& aModel)
{
    const DirectX::SimpleMath::Vector3 vehicleSize(14.0f, 7.0f, 9.5f);
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), vehicleSize);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldModelMatrix = aModel.localModelMatrix;
}

void NPCVehicle::InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    NPCModel& aModel, const DirectX::SimpleMath::Vector3 aDimensions)
{
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), aDimensions);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldModelMatrix = aModel.localModelMatrix;
}

/*
void NPCVehicle::InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
    VehicleData& aNPCVehicle, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment)
{
    int testBrea = 0;
    testBrea++;
    //InitializeNPCData(aNPCVehicle, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    //InitializeNPCModel(aContext, aNPCVehicle.npcModel);
}
*/

void NPCVehicle::InitializeNPCVehicle2(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment)
{
    //InitializeNPCData(m_vehicleData, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    //InitializeNPCModel(aContext, m_vehicleData.npcModel);

    InitializeNPCStruct(m_vehicleStruct00, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    InitializeNPCModelStruct(aContext, m_vehicleStruct00.npcModel, m_vehicleStruct00.vehicleData.dimensions);
}

void NPCVehicle::RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ)
{
    //  Compute the intermediate values of the 
//  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    velocityUpdate = aVehicle->forward * cos(m_vehicleStruct00.vehicleData.time) * 10.0f;

    //velocityUpdate.z = cos(m_vehicleData.time) * 10.0f;
    //velocityUpdate.x = cos(m_vehicleData.time) * 5.0f;
    DirectX::SimpleMath::Vector3 gravForce = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    //velocityUpdate += gravForce;


    float mass = 0.5f;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void NPCVehicle::RungeKutta4(struct VehicleData* aVehicle, double aTimeDelta)
{
    //  Define a convenience variables

    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    MotionNPC q = aVehicle->q;
    MotionNPC dq1;
    MotionNPC dq2;
    MotionNPC dq3;
    MotionNPC dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aVehicle, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aVehicle, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aVehicle, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aVehicle, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aVehicle->time = aVehicle->time + aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aVehicle->q.velocity = q.velocity;
    aVehicle->q.position = q.position;
}

void NPCVehicle::UpdateAlignment()
{

}

void NPCVehicle::UpdateNPC(const double aTimeDelta)
{
    //m_vehicleData.timer += aTimeDelta;
    /*
    m_vehicleData.terrainHightAtPos = m_vehicleData.environment->GetTerrainHeightAtPos(m_vehicleData.q.position);
    m_vehicleData.terrainNormal = m_vehicleData.environment->GetTerrainNormal(m_vehicleData.q.position);
    RungeKutta4(&m_vehicleData, aTimeDelta);
    */

    m_vehicleStruct00.vehicleData.terrainHightAtPos = m_vehicleStruct00.environment->GetTerrainHeightAtPos(m_vehicleStruct00.vehicleData.q.position);    
    m_vehicleStruct00.vehicleData.terrainNormal = m_vehicleStruct00.environment->GetTerrainNormal(m_vehicleStruct00.vehicleData.q.position);
    RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);

    UpdateNPCModel(aTimeDelta);
    m_vehicleStruct00.vehicleData.collisionBox.Center = m_vehicleStruct00.vehicleData.q.position;
}

void NPCVehicle::UpdateNPCModel(const double aTimeDelta)
{
    /*
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleData.q.position, -m_vehicleData.right, m_vehicleData.up);
    m_vehicleData.npcModel.worldModelMatrix = m_vehicleData.npcModel.localModelMatrix;
    m_vehicleData.npcModel.worldModelMatrix *= updateMat;
    */

    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.npcModel.worldModelMatrix = m_vehicleStruct00.npcModel.localModelMatrix;
    m_vehicleStruct00.npcModel.worldModelMatrix *= updateMat;
}