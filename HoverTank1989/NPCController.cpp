#include "pch.h"
#include "NPCController.h"
#include "Vehicle.h"

NPCController::NPCController()
{
    m_npcVec.clear();
}

NPCController::~NPCController()
{
    for (auto p : m_npcVec)
    {
        delete p;
    }
    m_npcVec.clear();
}

void NPCController::AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController)
{
    NPCVehicle* newNPC = new NPCVehicle;
    newNPC->SetDebugData(m_debugData);  
    newNPC->InitializeNPCVehicle(aContext, aHeading, aPosition, m_environment, aNpcController, m_player, GetUniqueID());
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
            //m_npcVec[i]->CalculateImpactForce(projectileForce, aProjectile.collisionSphere.Center);
            //m_npcVec[i]->CalculateImpactForceFromProjectile(projectileForce, aProjectile.collisionSphere.Center);
            m_npcVec[i]->CalculateImpulseForceFromProjectile(projectileForce, aProjectile.collisionSphere.Center);
            isCollisionTrue = true;
        }
    }
    return isCollisionTrue;
}

void NPCController::DebugToggleAI()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->DebugToggleAI();
    }
}

void NPCController::DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->DrawNPC(aView, aProj);
    }
}

unsigned int NPCController::GetUniqueID()
{
    const unsigned int idVal = m_nextUniqueID;
    m_nextUniqueID++;
    return idVal;
}

std::vector<DirectX::SimpleMath::Vector3> NPCController::GetVecOfNpcPos(const int aSelfID)
{
    std::vector<DirectX::SimpleMath::Vector3> posVec;
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() != aSelfID)
        {
            posVec.push_back(m_npcVec[i]->GetPos());
        }
    }
    return posVec;
}

void NPCController::InitializeNPCController(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, Environment const* aEnvironment)
{
    m_nextUniqueID = 0;
    m_npcVec.clear();
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
    CheckNpcCollisions();
    // collisions, 
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        for (unsigned int j = 0; j < m_npcVec.size(); ++j)
        {
            VehicleData testV1 = m_npcVec[i]->GetVehicleData();
            if (i != j)
            {
                VehicleData testV2 = m_npcVec[j]->GetVehicleData();
                const float distance = (testV1.q.position - testV2.q.position).Length();
                const float maxCollisionRange = testV1.maxCollisionDetectionRange + testV2.maxCollisionDetectionRange;
                // only check collisions in potential range and prevent collision checks with vehicles knocked out of play
                /*
                if (distance < maxCollisionRange && distance > -1.0f)
                {
                    if (testV1.collisionBox.Intersects(testV2.collisionBox) == true || testV1.collisionBox.Contains(testV2.collisionBox) == true)
                    {
                        m_npcVec[i]->SetCollisionVal(true);
                        Utility::ImpactForce projectileForce;
                        projectileForce.impactMass = testV2.mass;
                        projectileForce.impactVelocity = testV2.q.velocity;
                        //m_npcVec[i]->CalculateImpactForce2(projectileForce, testV1.collisionBox.Center);
                        m_npcVec[i]->CalculateImpactForce4(m_npcVec[j]->GetVehicleData());
                    }
                }
                */
                // avoidance check
                DirectX::BoundingBox avoidanceBox = m_npcVec[i]->GetAvoidanceBox();
                DirectX::BoundingBox avoidanceBox2 = m_npcVec[j]->GetAvoidanceBox();

                bool testBool1 = avoidanceBox.Intersects(testV2.collisionBox);
                bool testBool2 = avoidanceBox.Contains(testV2.collisionBox);
                if (avoidanceBox.Intersects(testV2.collisionBox) == true || avoidanceBox.Contains(testV2.collisionBox) == true)
                {

                    bool testBool7 = avoidanceBox.Intersects(testV2.collisionBox);
                    bool testBool8 = avoidanceBox.Contains(testV2.collisionBox);
                    m_npcVec[i]->PushAvoidanceTarget(testV2.q.position, m_npcVec[j]);

                    bool testBool9 = avoidanceBox.Intersects(testV2.collisionBox);
                    bool testBool10 = avoidanceBox.Contains(testV2.collisionBox);


                    NPCVehicle const* testTarget = m_npcVec[j];
                    DirectX::BoundingBox collisionBox = m_npcVec[i]->GetCollisionData();
                    DirectX::BoundingBox avoidanceBox2 = testTarget->GetAvoidanceBox();
                    //bool testBool3 = avoidanceBox.Intersects(testV2.collisionBox);
                    //bool testBool4 = avoidanceBox.Contains(testV2.collisionBox);
                    //if (avoidanceBox.Intersects(collisionBox) == false && avoidanceBox.Contains(collisionBox) == false)
                    if (avoidanceBox2.Intersects(testV2.collisionBox) == false && avoidanceBox2.Contains(testV2.collisionBox) == false)
                    {
                        bool testBool3 = avoidanceBox2.Intersects(testV2.collisionBox);
                        bool testBool4 = avoidanceBox2.Contains(testV2.collisionBox);
                        bool testBool5 = avoidanceBox2.Intersects(testV2.collisionBox);
                        bool testBool6 = avoidanceBox2.Contains(testV2.collisionBox);
                        if (avoidanceBox.Intersects(collisionBox) == true && avoidanceBox.Contains(collisionBox) == false)
                        {
                        }
                        int selfIndex = m_npcVec[i]->GetID();
                        int targetIndex = testTarget->GetID();
                        int testBreak = 0;
                        //m_npcVec[i]->PushAvoidanceTarget(testTarget->GetPos(), testTarget);


                        testBreak++;
                    }
                }
                else if (avoidanceBox.Intersects(avoidanceBox2) == true || avoidanceBox.Contains(avoidanceBox2) == true)
                {
                    //m_npcVec[i]->PushAvoidanceTarget(testV2.q.position, m_npcVec[j]);
                }
            }
        }
    }

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetAvoidanceIsTrue() == true)
        {
            NPCVehicle const* testTarget = m_npcVec[i]->GetAvoidanceNPCTarget();
            //DirectX::BoundingBox collisionBox = m_npcVec[i]->GetCollisionData();
            //DirectX::BoundingBox avoidanceBox = testTarget->GetAvoidanceBox();
            int testInt2 = testTarget->GetID();
            DirectX::BoundingBox collisionBox = testTarget->GetCollisionData();
            DirectX::BoundingBox avoidanceBox = m_npcVec[i]->GetAvoidanceBox();
            if (avoidanceBox.Intersects(collisionBox) == false && avoidanceBox.Contains(collisionBox) == false)
            {
                if (avoidanceBox.Intersects(collisionBox) == true && avoidanceBox.Contains(collisionBox) == false)
                {
                }
                int selfIndex = m_npcVec[i]->GetID();
                int targetIndex = testTarget->GetID();
                int testBreak = 0;
                //m_npcVec[i]->PushAvoidanceTarget(testTarget->GetPos(), testTarget);

                
                testBreak++;
            }
        }
    }
}

void NPCController::CheckNpcCollisions()
{
    std::vector<std::pair<int, int>> collisionsRecorded;
    
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        for (unsigned int j = 0; j < m_npcVec.size(); ++j)
        {
            VehicleData testV1 = m_npcVec[i]->GetVehicleData();
            if (i != j)
            {
                VehicleData testV2 = m_npcVec[j]->GetVehicleData();
                const float distance = (testV1.q.position - testV2.q.position).Length();
                const float maxCollisionRange = testV1.maxCollisionDetectionRange + testV2.maxCollisionDetectionRange;
                // only check collisions in potential range and prevent collision checks with vehicles knocked out of play
                if (distance < maxCollisionRange && distance > -1.0f)
                {
                    if (testV1.collisionBox.Intersects(testV2.collisionBox) == true || testV1.collisionBox.Contains(testV2.collisionBox) == true)
                    {
                        bool hasCollisionBeenRecordedYet = false;
                        for (int k = 0; k < collisionsRecorded.size(); ++k)
                        {
                            if (((collisionsRecorded[k].first == testV1.id) && (collisionsRecorded[k].second == testV2.id)) ||
                                ((collisionsRecorded[k].first == testV2.id) && (collisionsRecorded[k].second == testV1.id)))
                            {
                                hasCollisionBeenRecordedYet = true;
                            }
                        }

                        collisionsRecorded.push_back(std::pair<int, int>(testV1.id, testV2.id));

                        if (hasCollisionBeenRecordedYet == false)
                        {
                            float mass1 = testV1.mass; // aVehicleHit.mass;
                            float mass2 = testV2.mass; // m_vehicleStruct00.vehicleData.mass;
                            float e = 0.4f;

                            float tmp = 1.0f / (mass1 + mass2);
                            DirectX::SimpleMath::Vector3 vx1 = testV1.q.velocity; // aVehicleHit.q.velocity;
                            DirectX::SimpleMath::Vector3 vx2 = testV2.q.velocity; // m_vehicleStruct00.vehicleData.q.velocity;
                            DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp + (1.0 + e) * mass2 * vx2 * tmp;
                            DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp + (mass2 - e * mass1) * vx2 * tmp;

                            float newVX1Length = newVx1.Length();
                            float newVX2Length = newVx2.Length();

                            //m_npcVec[i]->TestCollisionVelocityUpdate(newVx1);
                            //m_npcVec[j]->TestCollisionVelocityUpdate(newVx2);

                            m_npcVec[i]->m_prevImpact = newVx1;
                            m_npcVec[j]->m_prevImpact = newVx2;

                            //m_npcVec[i]->CalculateImpactForce4(m_npcVec[j]->GetVehicleData());
                            //m_npcVec[j]->CalculateImpactForce4(m_npcVec[i]->GetVehicleData());
                            m_npcVec[i]->CalculateImpulseForce(m_npcVec[j]->GetVehicleData());
                            m_npcVec[j]->CalculateImpulseForce(m_npcVec[i]->GetVehicleData());


                            DirectX::SimpleMath::Vector3 testVec = newVx1.Cross(newVx2);
                            float testDot2 = vx1.Dot(vx2);
                            vx1.Normalize();
                            vx2.Normalize();
                            float testDot = vx1.Dot(vx2);
                            if (testDot < 0.5f)
                            {
                                m_npcVec[i]->TestCollisionVelocityUpdate(newVx1);
                                m_npcVec[j]->TestCollisionVelocityUpdate(newVx2);
                            }
                            else
                            {
                                int testBreak = 0;
                                testBreak++;
                            }
                            //m_npcVec[i]->TestCollisionVelocityUpdate(newVx1);
                            //m_npcVec[j]->TestCollisionVelocityUpdate(newVx2);
                        }

                        //m_npcVec[i]->SetCollisionVal(true);
                        //m_npcVec[i]->CalculateImpactForce4(m_npcVec[j]->GetVehicleData());
                    }
                }
            }
        }
    }
}


void NPCController::TestPositionChange()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->TestPositionChange();
    }
}