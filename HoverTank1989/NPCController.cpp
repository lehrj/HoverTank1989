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

void NPCController::ActivateNPCs()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsActivated() == false)
        {
            m_npcVec[i]->ActivateNPC();
        }
    }
}

void NPCController::AddNPC(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController)
{
    NPCVehicle* newNPC = new NPCVehicle;
    newNPC->SetDebugData(m_debugData);
    newNPC->InitializeNPCVehicle(aContext, aHeading, aPosition, m_environment, aNpcController, m_player, GetUniqueID());
    newNPC->SetNpcType(aNPCType);
    m_npcVec.push_back(newNPC);
}

void NPCController::AirDropNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController, const DirectX::SimpleMath::Vector3 aDropPosition,
    const DirectX::SimpleMath::Vector3 aOrientation, const float aAltitude, const unsigned int aColumnCount, const unsigned int aRowCount, const float aColumnSpacing, const float aRowSpacing)
{
    DirectX::SimpleMath::Vector3 pos = aDropPosition;
    DirectX::SimpleMath::Vector3 heading = aOrientation;

    for (unsigned int i = 0; i < aColumnCount; ++i)
    {
        for (unsigned int j = 0; j < aRowCount; ++j)
        {
            this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
            pos.x += aColumnSpacing;
        }
        pos.x = aDropPosition.x;
        pos.z += aRowSpacing;
    }
}

bool NPCController::CheckExplosionCollisions(DirectX::BoundingSphere aBoundingSphere)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (DirectX::SimpleMath::Vector3::Distance(aBoundingSphere.Center, m_npcVec[i]->GetPos()) < (m_npcVec[i]->GetCollisionDetectionRange() + aBoundingSphere.Radius) && m_npcVec[i]->GetIsExploding() == false && m_npcVec[i]->GetIsDead() == false)
        {
            if (m_npcVec[i]->GetIsExploding() == false)
            {
                if (m_npcVec[i]->GetCollisionData().Intersects(aBoundingSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aBoundingSphere) == true)
                {
                    m_fireControl->PushVehicleExplosion(m_npcVec[i]->GetPos(), m_npcVec[i]->GetID());
                    m_npcVec[i]->SetExplosionTrue();

                    const float maxExplosionRange = m_fireControl->GetMaxExplosionImpactRadius();
                    const float distanceToExplosion = DirectX::SimpleMath::Vector3::Distance(aBoundingSphere.Center, m_npcVec[i]->GetPos());
                    float distanceMod = 1.0f - (distanceToExplosion / maxExplosionRange);
                    const float maxExplosionForce = m_fireControl->GetMaxExplosionForce();
                    const float explosionForce = maxExplosionForce * distanceMod;
                    const float impulseTimeTotal = 1.0f;
                    DirectX::SimpleMath::Vector3 impactNorm = m_npcVec[i]->GetPos() - aBoundingSphere.Center;
                    impactNorm.Normalize();

                    Utility::ImpulseForce explosionImpulseForce;
                    explosionImpulseForce.currentMagnitude = 0.0f;
                    explosionImpulseForce.currentTime = 0.0f;
                    explosionImpulseForce.directionNorm = impactNorm;
                    explosionImpulseForce.isActive = true;
                    explosionImpulseForce.maxMagnitude = explosionForce;
                    explosionImpulseForce.torqueArm = impactNorm;

                    DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::CreateWorld(m_npcVec[i]->GetPos(), -m_npcVec[i]->GetRight(), m_npcVec[i]->GetUp());
                    DirectX::SimpleMath::Matrix localizationMatrix = worldMatrix;
                    localizationMatrix = localizationMatrix.Invert();
                    DirectX::SimpleMath::Vector3 localizedExplosionCenter = aBoundingSphere.Center;
                    localizedExplosionCenter = DirectX::SimpleMath::Vector3::Transform(localizedExplosionCenter, localizationMatrix);

                    DirectX::BoundingOrientedBox localizedBox = m_npcVec[i]->GetCollisionData();
             
                    DirectX::SimpleMath::Vector3 localImpactPos = DirectX::SimpleMath::Vector3::Zero;
                    // X
                    if (localizedExplosionCenter.x > localizedBox.Extents.x)
                    {
                        localImpactPos.x = localizedBox.Extents.x;
                    }
                    else if (localizedExplosionCenter.x < -localizedBox.Extents.x)
                    {
                        localImpactPos.x = -localizedBox.Extents.x;
                    }
                    else
                    {
                        localImpactPos.x = localizedExplosionCenter.x;
                    }
                    // Y
                    if (localizedExplosionCenter.y > localizedBox.Extents.y)
                    {
                        localImpactPos.y = localizedBox.Extents.y;
                    }
                    else if (localizedExplosionCenter.y < -localizedBox.Extents.y)
                    {
                        localImpactPos.y = -localizedBox.Extents.y;
                    }
                    else
                    {
                        localImpactPos.y = localizedExplosionCenter.y;
                    }
                    // Z
                    if (localizedExplosionCenter.z > localizedBox.Extents.z)
                    {
                        localImpactPos.z = localizedBox.Extents.z;
                    }
                    else if (localizedExplosionCenter.z < -localizedBox.Extents.z)
                    {
                        localImpactPos.z = -localizedBox.Extents.z;
                    }
                    else
                    {
                        localImpactPos.z = localizedExplosionCenter.z;
                    }

                    DirectX::SimpleMath::Vector3 impactPos = localImpactPos;
                    impactPos = DirectX::SimpleMath::Vector3::Transform(impactPos, worldMatrix);

                    impactNorm = impactPos - aBoundingSphere.Center;
                    impactNorm.Normalize();
                    explosionImpulseForce.directionNorm = impactNorm;

                    explosionImpulseForce.torqueArm = impactPos - m_npcVec[i]->GetPos();
                    explosionImpulseForce.torqueArm *= 5.0f;
                    explosionImpulseForce.totalTime = impulseTimeTotal;

                    m_npcVec[i]->PushImpulseForce(explosionImpulseForce);

                    isCollisionTrue = true;
                }
            }
        }
    }
    return isCollisionTrue;
}

bool NPCController::CheckProjectileCollisions(Utility::CollisionData& aProjectile, unsigned int& aVehicleHitId)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (DirectX::SimpleMath::Vector3::Distance(aProjectile.collisionSphere.Center, m_npcVec[i]->GetPos()) < m_npcVec[i]->GetCollisionDetectionRange())
        {
            if (m_npcVec[i]->GetCollisionData().Intersects(aProjectile.collisionSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aProjectile.collisionSphere) == true)
            {
                m_npcVec[i]->SetCollisionVal(true);
                Utility::ImpactForce projectileForce;
                projectileForce.impactModifier = aProjectile.collisionModifier;
                projectileForce.impactMass = aProjectile.mass;
                projectileForce.impactVelocity = aProjectile.velocity;
                m_npcVec[i]->CalculateImpulseForceFromProjectile(projectileForce, aProjectile.collisionSphere.Center);
                isCollisionTrue = true;
                aVehicleHitId = m_npcVec[i]->GetID();
            }
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
        if (m_npcVec[i]->GetIsDead() == false)
        {
            m_npcVec[i]->DrawNPC(aView, aProj);
        }
    }
}

void NPCController::DrawNPCs2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsDead() == false && m_npcVec[i]->GetIsActivated() == true)
        {
            m_npcVec[i]->DrawNPC3(aView, aProj, aEffect, aInputLayout);
        }
    }
}

DirectX::SimpleMath::Vector3 NPCController::GetNpcAccelVecTest(const unsigned int aId)
{
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            return m_npcVec[i]->GetTestAccelVec();
        }
    }
    // to do : add error handling for id out of scope
    return DirectX::SimpleMath::Vector3::Zero;
}

DirectX::SimpleMath::Matrix NPCController::GetNpcAlignment(const unsigned int aId)
{
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            return m_npcVec[i]->GetAlignment();
        }
    }
    // to do : add error handling for id out of scope
    return DirectX::SimpleMath::Matrix::Identity;
}

DirectX::SimpleMath::Vector3 NPCController::GetNpcPos(const unsigned int aId)
{
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            return m_npcVec[i]->GetPos();
        }
    }
    // to do : add error handling for id out of scope
    return DirectX::SimpleMath::Vector3::Zero;
}

unsigned int NPCController::GetUniqueID()
{
    const unsigned int idVal = m_nextUniqueID;
    m_nextUniqueID++;
    return idVal;
}

std::vector<DirectX::SimpleMath::Vector3> NPCController::GetVecOfNpcPos(const unsigned int aSelfID)
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
    m_loadQueue.clear();
}

void NPCController::InitializeTextureMaps(NpcTextureMapType aTextureMapType, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->InitializeTextureMaps(aTextureMapType, aTexture, aNormalMap, aSpecularMap);
    }
}

void NPCController::LoadNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController)
{
    const float xOrgVal = 70.0f;
    DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(xOrgVal, 11.0, -40.0f);
    DirectX::SimpleMath::Vector3 heading = -DirectX::SimpleMath::Vector3::UnitX;
    const float low = 0.1f;
    const float high = 5.0f;
    const float zPosOffSet = 12.0f;
    float baseHeight = 10.0f;
    const int rows = 1;
    const int columns = 8;
    for (int i = 0; i < columns; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            float yOffSet = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
            pos.y = baseHeight + yOffSet;
            this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
            pos.x += 25.0f;
        }
        baseHeight = 120.0f;
        pos.x = xOrgVal;
        pos.z += zPosOffSet;
    }

    const float xLine = 85.0f;
    const float yLine = 10.0f;
    const float zSpacing = 90.0f;
    // target 1
    pos.x = xLine;
    pos.y = yLine;
    pos.z = -300.0f;
    //this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
}

void NPCController::LoadToQueue(const DirectX::SimpleMath::Vector3 aLoadPosition, const DirectX::SimpleMath::Vector3 aOrientation, const float aAltitude, const unsigned int aColumnCount, const unsigned int aRowCount, const float aColumnSpacing, const float aRowSpacing)
{
    DirectX::SimpleMath::Vector3 pos = aLoadPosition;
    DirectX::SimpleMath::Vector3 heading = aOrientation;

    LoadQueue loadData;
    loadData.deployType = NPCType::NPCTYPE_NPC00;
    for (unsigned int i = 0; i < aColumnCount; ++i)
    {
        for (unsigned int j = 0; j < aRowCount; ++j)
        {
            loadData.deployOrientation = aOrientation;
            loadData.deployPosition = pos;
            m_loadQueue.push_back(loadData);

            pos.x += aColumnSpacing;
        }
        pos.x = aLoadPosition.x;
        pos.z += aRowSpacing;
    }
}

void NPCController::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void NPCController::SetFireControl(std::shared_ptr<FireControl> aFireControlPtr)
{
    m_fireControl = aFireControlPtr;
}

void NPCController::SetNPCEnvironment(Environment const* aEnvironment)
{
    m_environment = aEnvironment;
}

void NPCController::SetPlayer(std::shared_ptr<Vehicle> aVehicle)
{
    m_player = aVehicle;
}

void NPCController::SetVehicleDeath(const unsigned int aVehicleId)
{
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aVehicleId)
        {
            m_npcVec[i]->SetDeadTrue();
        }
    }
}

void NPCController::UnlockJumpAbility()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->UnlockJump();
    }
}

void NPCController::UpdateLoadQueue(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController, const double aTimeDelta)
{
    if (m_loadQueue.size() > 0)
    {
        const unsigned int i = m_loadQueue.size() - 1;
        m_loadQueue[i].deployOrientation;
        this->AddNPC(aContext, m_loadQueue[i].deployType, m_loadQueue[i].deployOrientation, m_loadQueue[i].deployPosition, aNpcController);
        m_loadQueue.pop_back();
    }
}

void NPCController::UpdateNPCController(const double aTimeDelta)
{
    UpdateNPCs(aTimeDelta);
}

void NPCController::UpdateNPCs(const double aTimeDelta)
{
    m_testTimer += static_cast<float>(aTimeDelta);

    ActivateNPCs();

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->UpdateNPC(aTimeDelta);
    }

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsReadyToDelete() == true)
        {
            std::vector<NPCVehicle*>::iterator it;
            it = m_npcVec.begin() + i;
            m_npcVec.erase(it);
        }
    }

    CheckNpcCollisions();
    CheckNpcAvoidance();
}

void NPCController::CheckNpcAvoidance()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        DirectX::BoundingOrientedBox avoidanceBox = m_npcVec[i]->GetAvoidanceBox();
        for (unsigned int j = 0; j < m_npcVec.size(); ++j)
        {
            if (i != j)
            {
                const float distance = (m_npcVec[i]->GetPos() - m_npcVec[j]->GetPos()).Length();
                if (distance < (m_npcVec[i]->GetAvoidanceRadius() + m_npcVec[j]->GetAvoidanceRadius()))
                {
                    DirectX::BoundingOrientedBox avoidanceBox2 = m_npcVec[j]->GetCollisionData();
                    if (avoidanceBox.Contains(avoidanceBox2) == true || avoidanceBox.Intersects(avoidanceBox2) == true)
                    {
                        m_npcVec[i]->PushAvoidanceTarget(m_npcVec[j]->GetPos(), m_npcVec[j]);
                    }
                }
            }
        }
    }
}

void NPCController::CheckNpcCollisions()
{
    std::vector<std::pair<unsigned int, unsigned int>> collisionsRecorded;
    
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        const VehicleData& testV1 = m_npcVec[i]->GetVehicleData();
        for (unsigned int j = 0; j < m_npcVec.size(); ++j)
        {
            if (i != j)
            {
                const float distance = (m_npcVec[i]->GetPos() - m_npcVec[j]->GetPos()).Length();
                const float maxCollisionRange = m_npcVec[i]->GetCollisionDetectionRange() + m_npcVec[j]->GetCollisionDetectionRange();
                // only check collisions in potential range and prevent collision checks with vehicles knocked out of play
                if (distance < maxCollisionRange && distance > -1.0f)
                {
                    const VehicleData& testV2 = m_npcVec[j]->GetVehicleData();
                    if (testV1.collisionBox.Contains(testV2.collisionBox) == true || testV1.collisionBox.Intersects(testV2.collisionBox) == true)
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

                        if (hasCollisionBeenRecordedYet == false)
                        {
                            collisionsRecorded.push_back(std::pair<int, int>(testV1.id, testV2.id));

                            DirectX::SimpleMath::Vector3 p1localizedTo2 = testV1.q.position - testV2.q.position;
                            DirectX::SimpleMath::Vector3 p1localizedTo2Norm = p1localizedTo2;
                            p1localizedTo2Norm.Normalize();
                            DirectX::SimpleMath::Vector3 p2localizedTo1 = testV2.q.position - testV1.q.position;
                            DirectX::SimpleMath::Vector3 p2localizedTo1Norm = p2localizedTo1;
                            p2localizedTo1Norm.Normalize();

                            float mass1 = testV1.mass; // aVehicleHit.mass;
                            float mass2 = testV2.mass; // m_vehicleStruct00.vehicleData.mass;
                            float e = 0.4f; // rebound coefficent 

                            float tmp = 1.0f / (mass1 + mass2);
                            DirectX::SimpleMath::Vector3 vx1 = testV1.q.velocity; // aVehicleHit.q.velocity;
                            DirectX::SimpleMath::Vector3 vx2 = testV2.q.velocity; // m_vehicleStruct00.vehicleData.q.velocity;

                            DirectX::SimpleMath::Vector3 aVelocityvx1 = testV1.q.velocity; // aVehicleHit.q.velocity;
                            DirectX::SimpleMath::Vector3 aVelocityvx2 = testV2.q.velocity; // m_vehicleStruct00.vehicleData.q.velocity;

                            DirectX::SimpleMath::Vector3 aNormVelocity1 = aVelocityvx1;
                            aNormVelocity1.Normalize();
                            DirectX::SimpleMath::Vector3 aNormVelocity2 = aVelocityvx2;
                            aNormVelocity2.Normalize();

                            DirectX::SimpleMath::Vector3 aPositionvx1 = testV1.q.position; // aVehicleHit.q.velocity;
                            DirectX::SimpleMath::Vector3 aPositionvx2 = testV2.q.position; // m_vehicleStruct00.vehicleData.q.velocity;

                            DirectX::SimpleMath::Vector3 testVelocityPre1 = vx1 - vx2;
                            DirectX::SimpleMath::Vector3 testVelocityPre2 = vx2 - vx1;

                            DirectX::SimpleMath::Vector3 testVelocityNorm1 = testVelocityPre1;
                            testVelocityNorm1.Normalize();

                            DirectX::SimpleMath::Vector3 testVelocityNorm2 = testVelocityPre2;
                            testVelocityNorm2.Normalize();

                            DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp + (1.0f + e) * mass2 * vx2 * tmp;
                            DirectX::SimpleMath::Vector3 newVx2 = (1.0f + e) * mass1 * vx1 * tmp + (mass2 - e * mass1) * vx2 * tmp;

                            DirectX::SimpleMath::Vector3 testVelocityPost1 = newVx1 - newVx2;
                            DirectX::SimpleMath::Vector3 testVelocityPost2 = newVx2 - newVx1;

                            m_npcVec[i]->m_prevImpact = newVx1;
                            m_npcVec[j]->m_prevImpact = newVx2;

                            float x1a = p1localizedTo2Norm.Dot(testVelocityNorm1);
   
                            if (x1a < 0.0f)
                            {
                                DirectX::XMFLOAT3 testV1Corners[8];
                                DirectX::XMFLOAT3* pCorners1;
                                pCorners1 = testV1Corners;
                                testV1.collisionBox.GetCorners(pCorners1);
                                
                                DirectX::XMFLOAT3 testV2Corners[8];         
                                DirectX::XMFLOAT3* pCorners2 = testV2Corners;
                                testV2.collisionBox.GetCorners(pCorners2);
                                pCorners2 = nullptr;
 
                                DirectX::SimpleMath::Vector3 testV1ClosestCorner = DirectX::SimpleMath::Vector3::Zero;
                                float testV1ClosestCornerDistance = 0.0f;
                                bool isTestV1ClosestCornerFound = false;
                                DirectX::SimpleMath::Vector3 testV1SecondClosestCorner = DirectX::SimpleMath::Vector3::Zero;
                                float testV1ClosestSecondCornerDistance = 0.0f;
                                bool isTestV1SecondClosestCornerFound = false;
                                for (int l = 0; l < 8; ++l)
                                {
                                    DirectX::SimpleMath::Vector3 cornerPos = testV1Corners[l];
                                    float distanceToCenter = (cornerPos - testV2.collisionBox.Center).Length();

                                    if (isTestV1ClosestCornerFound == false)
                                    {
                                        testV1ClosestCorner = cornerPos;
                                        testV1ClosestCornerDistance = distanceToCenter;
                                        isTestV1ClosestCornerFound = true;
                                    }
                                    else if (isTestV1SecondClosestCornerFound == false)
                                    {
                                        if (distanceToCenter < testV1ClosestCornerDistance)
                                        {
                                            testV1SecondClosestCorner = testV1ClosestCorner;
                                            testV1ClosestSecondCornerDistance = testV1ClosestCornerDistance;
                                            testV1ClosestCorner = cornerPos;
                                            testV1ClosestCornerDistance = distanceToCenter;
                                            isTestV1SecondClosestCornerFound = true;
                                        }
                                        else
                                        {
                                            testV1SecondClosestCorner = cornerPos;
                                            testV1ClosestSecondCornerDistance = distanceToCenter;
                                            isTestV1SecondClosestCornerFound = true;
                                        }
                                    }
                                    else
                                    {
                                        if (distanceToCenter < testV1ClosestCornerDistance)
                                        {
                                            testV1SecondClosestCorner = testV1ClosestCorner;
                                            testV1ClosestSecondCornerDistance = testV1ClosestCornerDistance;
                                            testV1ClosestCorner = cornerPos;
                                            testV1ClosestCornerDistance = distanceToCenter;

                                        }
                                        else if(distanceToCenter < testV1ClosestSecondCornerDistance)
                                        {
                                            testV1SecondClosestCorner = cornerPos;
                                            testV1ClosestSecondCornerDistance = distanceToCenter;
                                        }
                                    }
                                }
                                DirectX::SimpleMath::Vector3 testV1ImpactPos = (testV1ClosestCorner + testV1SecondClosestCorner) * 0.5f;
                                isTestV1ClosestCornerFound = false;
                                isTestV1SecondClosestCornerFound = false;
                                for (int l = 0; l < 8; ++l)
                                {
                                    DirectX::SimpleMath::Vector3 cornerPos = testV2Corners[l];
                                    float distanceToCenter = (cornerPos - testV1.collisionBox.Center).Length();
                                    if (isTestV1ClosestCornerFound == false)
                                    {
                                        testV1ClosestCorner = cornerPos;
                                        testV1ClosestCornerDistance = distanceToCenter;
                                        isTestV1ClosestCornerFound = true;
                                    }
                                    else if (isTestV1SecondClosestCornerFound == false)
                                    {
                                        if (distanceToCenter < testV1ClosestCornerDistance)
                                        {
                                            testV1SecondClosestCorner = testV1ClosestCorner;
                                            testV1ClosestSecondCornerDistance = testV1ClosestCornerDistance;
                                            testV1ClosestCorner = cornerPos;
                                            testV1ClosestCornerDistance = distanceToCenter;
                                            isTestV1SecondClosestCornerFound = true;
                                        }
                                        else
                                        {
                                            testV1SecondClosestCorner = cornerPos;
                                            testV1ClosestSecondCornerDistance = distanceToCenter;
                                            isTestV1SecondClosestCornerFound = true;
                                        }
                                    }
                                    else
                                    {
                                        if (distanceToCenter < testV1ClosestCornerDistance)
                                        {
                                            testV1SecondClosestCorner = testV1ClosestCorner;
                                            testV1ClosestSecondCornerDistance = testV1ClosestCornerDistance;
                                            testV1ClosestCorner = cornerPos;
                                            testV1ClosestCornerDistance = distanceToCenter;
                                        }
                                        else if (distanceToCenter < testV1ClosestSecondCornerDistance)
                                        {
                                            testV1SecondClosestCorner = cornerPos;
                                            testV1ClosestSecondCornerDistance = distanceToCenter;
                                        }
                                    }
                                }
                                DirectX::SimpleMath::Vector3 testV2ImpactPos = (testV1ClosestCorner + testV1SecondClosestCorner) * 0.5f;
                                DirectX::SimpleMath::Vector3 impactPoint = (testV1ImpactPos + testV2ImpactPos) * 0.5f;

                                pCorners1 = nullptr;
                                pCorners2 = nullptr;
                                delete pCorners1;
                                delete pCorners2;

                                m_npcVec[i]->CalculateImpulseForce(m_npcVec[j]->GetVehicleData(), newVx1, newVx2, impactPoint);
                                m_npcVec[j]->CalculateImpulseForce(m_npcVec[i]->GetVehicleData(), newVx2, newVx1, impactPoint);
                                m_npcVec[i]->TestCollisionVelocityUpdate(newVx1);
                                m_npcVec[j]->TestCollisionVelocityUpdate(newVx2);
                            }
                        }
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