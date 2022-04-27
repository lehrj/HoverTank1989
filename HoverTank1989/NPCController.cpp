#include "pch.h"
#include "NPCController.h"
#include "Vehicle.h"

NPCController::NPCController()
{
    m_npcVec.clear();
    m_npcVec2.clear();
}

NPCController::~NPCController()
{
    for (auto p : m_npcVec)
    {
        delete p;
    }
    m_npcVec.clear();

    for (auto p : m_npcVec2)
    {
        delete p;
    }
    m_npcVec2.clear();
}

void NPCController::AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition)
{
    NPCVehicle* newNPC = new NPCVehicle;
    newNPC->SetDebugData(m_debugData);
    newNPC->InitializeNPCVehicle2(aContext, aHeading, aPosition, m_environment, m_player);
    
    m_npcVec.push_back(newNPC);
}

bool NPCController::CheckProjectileCollisions(Utility::CollisionData& aProjectile)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetCollisionData().Intersects(aProjectile.collisionSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aProjectile.collisionSphere) == true)
        {
            m_npcVec[i]->SetCollisionVal(true);
            Utility::ImpactForce projectileForce;
            projectileForce.impactMass = aProjectile.mass;
            projectileForce.impactVelocity = aProjectile.velocity;
            m_npcVec[i]->CalculateImpactForce(projectileForce, aProjectile.collisionSphere.Center);
        }
    }
    return isCollisionTrue;
}

void NPCController::DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->DrawNPC(aView, aProj);
    }
}

void NPCController::InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment)
{
    m_environment = aEnvironment;
    //AddNPC(NPCType::NPCTYPE_NPC00, const DirectX::SimpleMath::Vector3(10.0f, 2.0f, 0.0f));
}

void NPCController::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void NPCController::SetNPCEnvironment(Environment const* aEnvironment)
{
    m_environment = aEnvironment;
}

void NPCController::SetPlayer(Vehicle const* aVehicle)
{
    m_player = aVehicle;
}

void NPCController::UpdateNPCController(const DirectX::SimpleMath::Vector3 aPlayerPos, const double aTimeDelta)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->UpdatePlayerPos(aPlayerPos);
    }
    UpdateNPCs(aTimeDelta);
}

void NPCController::UpdateNPCs(const double aTimeDelta)
{
    m_testTimer += static_cast<float>(aTimeDelta);
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->UpdateNPC(aTimeDelta);
    }

    // collisions, 
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        for (unsigned int j = 0; j < m_npcVec.size(); ++j)
        {
            VehicleData testV1 = m_npcVec[i]->GetVehicleData();
            if (i != j)
            {
                VehicleData testV2 = m_npcVec[j]->GetVehicleData();

                if (testV1.collisionBox.Intersects(testV2.collisionBox) == true || testV1.collisionBox.Contains(testV2.collisionBox) == true)
                {
                    m_npcVec[i]->SetCollisionVal(true);

                    Utility::ImpactForce projectileForce;
                    projectileForce.impactMass = testV2.mass;
                    projectileForce.impactVelocity = testV2.q.velocity;
                    m_npcVec[i]->CalculateImpactForce(projectileForce, testV1.collisionBox.Center);
                }
            }
        }
    }
}