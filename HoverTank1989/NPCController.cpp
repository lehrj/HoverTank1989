#include "pch.h"
#include "NPCController.h"


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
    NPCVehicle* testNPC = new NPCVehicle;
    testNPC->InitializeNPCVehicle2(aContext, aHeading, aPosition, m_environment);
    m_npcVec.push_back(testNPC);
}

void NPCController::CheckProjectileCollisions(Utility::CollisionData& aProjectile)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetCollisionData().Intersects(aProjectile.collisionSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aProjectile.collisionSphere) == true)
        {
            m_npcVec[i]->SetCollisionVal(true);
            Utility::ImpactForce projectileForce;
            //projectileForce.impactMass = aProjectile.mass;
            projectileForce.impactMass = 10000000.0f;
            projectileForce.impactVelocity = aProjectile.velocity;
            m_npcVec[i]->CalculateImpactForce(projectileForce);
        }
    }
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


void NPCController::SetNPCEnvironment(Environment const* aEnvironment)
{
    m_environment = aEnvironment;
}

void NPCController::UpdateNPCController(const double aTimeDelta)
{
    UpdateNPCs(aTimeDelta);
}

void NPCController::UpdateNPCs(const double aTimeDelta)
{
    m_testTimer += static_cast<float>(aTimeDelta);

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (i == 7)
        {
            m_npcVec[i]->UpdateTestForce(DirectX::SimpleMath::Vector3::Zero, cos(m_testTimer * 0.5f));
        }
        m_npcVec[i]->UpdateNPC(aTimeDelta);
    }

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
                    //projectileForce.impactMass = 10000000.0f;
                    projectileForce.impactVelocity = testV2.q.velocity;
                    m_npcVec[i]->CalculateImpactForce(projectileForce);
                }
            }
        }
    }
}