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
    newNPC->SetNpcType(aNPCType);
    newNPC->InitializeNPCVehicle(aContext, aHeading, aPosition, m_environment, aNpcController, m_player, GetUniqueID());
    newNPC->InitializeTextureMaps(m_textureDataBlank.textureMapType, m_textureDataBlank.textureMap, m_textureDataBlank.normalMap, m_textureDataBlank.specularMap);
    newNPC->InitializeTextureMaps(m_textureDataFlame.textureMapType, m_textureDataFlame.textureMap, m_textureDataFlame.normalMap, m_textureDataFlame.specularMap);
    newNPC->InitializeTextureMaps(m_textureDataTest1.textureMapType, m_textureDataTest1.textureMap, m_textureDataTest1.normalMap, m_textureDataTest1.specularMap);
    newNPC->InitializeTextureMaps(m_textureDataTest2.textureMapType, m_textureDataTest2.textureMap, m_textureDataTest2.normalMap, m_textureDataTest2.specularMap);
    
    if (aNPCType == NPCType::NPCTYPE_SPAWNED || aNPCType == NPCType::NPCTYPE_SPAWNEDALT)
    {
        newNPC->DebugToggleAI();
    }
    
    m_npcVec.push_back(newNPC);
}

void NPCController::AddNpcWithAudio(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCType aNPCType, const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, std::shared_ptr<NPCController> aNpcController, std::shared_ptr<Utility::SoundFx> aAudioFx)
{
    NPCVehicle* newNPC = new NPCVehicle;
    newNPC->SetDebugData(m_debugData);
    newNPC->SetNpcType(aNPCType);
    newNPC->InitializeNPCVehicleWithAudio(aContext, aHeading, aPosition, m_environment, aNpcController, m_player, aAudioFx, GetUniqueID());
    newNPC->InitializeTextureMaps(m_textureDataTest2.textureMapType, m_textureDataTest2.textureMap, m_textureDataTest2.normalMap, m_textureDataTest2.specularMap);

    if (aNPCType == NPCType::NPCTYPE_SPAWNED || aNPCType == NPCType::NPCTYPE_SPAWNEDALT)
    {
        //newNPC->DebugToggleAI();
    }

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

bool NPCController::CheckExplosionCollisions(DirectX::BoundingSphere aBoundingSphere, const double aTimeDelta, const float aRemainingDuration)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (DirectX::SimpleMath::Vector3::Distance(aBoundingSphere.Center, m_npcVec[i]->GetPos()) < (m_npcVec[i]->GetCollisionDetectionRange() + aBoundingSphere.Radius) && m_npcVec[i]->GetIsExploding() == false && m_npcVec[i]->GetIsDead() == false)
        {
            //if (m_npcVec[i]->GetIsExploding() == false)
            if (m_npcVec[i]->GetIsExploding() == false || 1 == 1)
            {
                if (m_npcVec[i]->GetCollisionData().Intersects(aBoundingSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aBoundingSphere) == true)
                {
                    m_fireControl->PushVehicleExplosion(m_npcVec[i]->GetPos(), m_npcVec[i]->GetID());
                    m_npcVec[i]->SetExplosionTrue();

                    const float maxExplosionRange = m_fireControl->GetMaxExplosionImpactRadius();
                    const float distanceToExplosion = DirectX::SimpleMath::Vector3::Distance(aBoundingSphere.Center, m_npcVec[i]->GetPos());
                    float distanceMod = 1.0f - (distanceToExplosion / maxExplosionRange);
                    if (distanceMod > 1.0f || distanceMod < 0.0f)
                    {
                        if (distanceMod > 1.0f)
                        {
                            distanceMod = 1.0f;
                            int testBreak = 0;
                            testBreak++;
                        }
                        if (distanceMod < 0.0f)
                        {
                            distanceMod = 0.0f;
                            int testBreak = 0;
                            testBreak++;
                        }
 
                    }
                    const float maxExplosionForce = m_fireControl->GetMaxExplosionForce();
                    float explosionForce = maxExplosionForce * distanceMod;
                    if (explosionForce > maxExplosionForce)
                    {
                        int testBreak = 0;
                        testBreak++;
                    }
                    //explosionForce = maxExplosionForce;
                    //const float impulseTimeTotal = 1.0f;
                    const float impulseTimeTotal = aRemainingDuration;
                    //const float impulseTimeTotal = static_cast<float>(aTimeDelta);
                    DirectX::SimpleMath::Vector3 impactNorm = m_npcVec[i]->GetPos() - aBoundingSphere.Center;
                    impactNorm.Normalize();
                    if (impactNorm.Length() < 0.9f)
                    {
                        int testBreak = 0;
                        testBreak++;
                    }
                    else
                    {
                        int testBreak = 0;
                        testBreak++;
                    }

                    Utility::ImpulseForce explosionImpulseForce;
                    explosionImpulseForce.currentMagnitude = 0.0f;
                    explosionImpulseForce.currentTime = 0.0f;
                    explosionImpulseForce.directionNorm = impactNorm;
                    explosionImpulseForce.isActive = true;
                    explosionImpulseForce.maxMagnitude = explosionForce;
                    explosionImpulseForce.torqueArm = impactNorm;

                    explosionImpulseForce.forceSum = explosionForce;
                    explosionImpulseForce.forceRemaining = explosionImpulseForce.forceSum;

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
                    //impactNorm = DirectX::SimpleMath::Vector3::Zero;
                    if (impactNorm.Length() < 0.9f)
                    {
                        int testBreak = 0;
                        testBreak++;
                    }
                    //explosionImpulseForce.directionNorm = impactNorm;

                    explosionImpulseForce.torqueArm = impactPos - m_npcVec[i]->GetPos();
                    explosionImpulseForce.torqueArm *= m_fireControl->GetExplosiveTorqueArmMod();
                    explosionImpulseForce.totalTime = impulseTimeTotal;
                    //explosionImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_FRONTLOADCURVE;
                    explosionImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_LAGCURVE;
                    explosionImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
                    explosionImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_FLAT;
                    //m_npcVec[i]->PushImpulseForce(explosionImpulseForce);

                    //////////
    
                    DirectX::SimpleMath::Vector3 impactPosToCoM = impactPos - m_npcVec[i]->GetCenterOfMass();
                    DirectX::SimpleMath::Vector3 testForceNorm = impactPos - aBoundingSphere.Center;
                    testForceNorm += impactPosToCoM;
                    DirectX::SimpleMath::Vector3 testForceNorm3 = impactPos - aBoundingSphere.Center;
                    testForceNorm3 -= impactPosToCoM;
                    DirectX::SimpleMath::Vector3 testForceNorm4 = impactPos - aBoundingSphere.Center;
                    testForceNorm4.Normalize();
                    testForceNorm4 -= impactPosToCoM;
                    testForceNorm4.Normalize();
                    DirectX::SimpleMath::Vector3 testForceNorm2 = impactPos - aBoundingSphere.Center;
                    testForceNorm2.Normalize();
                    impactPosToCoM.Normalize();
                    testForceNorm2 += impactPosToCoM;
                    testForceNorm2.Normalize();
                    testForceNorm.Normalize();
                    DirectX::SimpleMath::Vector3 forcePoint = m_npcVec[i]->GetPos() + explosionImpulseForce.torqueArm;
                    explosionImpulseForce.torqueForceNorm = testForceNorm;
                    explosionImpulseForce.torqueForceNorm.Normalize();

                    //explosionImpulseForce.directionNorm = testForceNorm;

                    //explosionImpulseForce.torqueArm.Normalize();
                    m_npcVec[i]->PushImpulseForce(explosionImpulseForce);

                    /*
                    m_debugData->PushDebugLineScaled(m_npcVec[i]->GetPos(), explosionImpulseForce.torqueArm, 1.0f, 1.0f, 0.0f, DirectX::Colors::DarkSeaGreen);
                   // m_debugData->PushDebugLineScaled(forcePoint, explosionImpulseForce.torqueArm, 1.0f, 1.0f, 0.0f, DirectX::Colors::DarkSeaGreen);
                    m_debugData->PushDebugLine(forcePoint, explosionImpulseForce.directionNorm, 20.0f, 0.0f, DirectX::Colors::Blue);
                    m_debugData->PushDebugLine(forcePoint, impactNorm, 20.0f, 0.0f, DirectX::Colors::Coral);
                    m_debugData->PushDebugLine(forcePoint, testForceNorm, 25.0f, 0.0f, DirectX::Colors::Teal);
                    m_debugData->PushDebugLine(forcePoint, testForceNorm2, 20.0f, 0.0f, DirectX::Colors::Yellow);
                    m_debugData->PushDebugLine(forcePoint, testForceNorm3, 20.0f, 0.0f, DirectX::Colors::Orange);
                    m_debugData->PushDebugLine(forcePoint, testForceNorm4, 20.0f, 0.0f, DirectX::Colors::HotPink);
                    m_debugData->DebugPushUILineDecimalNumber("explosionImpulseForce.directionNorm.x = ", explosionImpulseForce.directionNorm.x, "");
                    m_debugData->DebugPushUILineDecimalNumber("explosionImpulseForce.directionNorm.y = ", explosionImpulseForce.directionNorm.y, "");
                    m_debugData->DebugPushUILineDecimalNumber("explosionImpulseForce.directionNorm.z = ", explosionImpulseForce.directionNorm.z, "");
                    */

                    /*
                    m_debugData->PushDebugLine(impactPos, explosionImpulseForce.torqueArm, 20.0f, 0.0f, DirectX::Colors::White);
                    m_debugData->PushDebugLinePositionIndicator(impactPos, 10.0f, 0.0f, DirectX::Colors::Blue);
                    m_debugData->DebugPushUILineDecimalNumber("aBoundingSphere.Radius", aBoundingSphere.Radius, "");
                    m_debugData->PushDebugLine(impactPos, explosionImpulseForce.directionNorm, 20.0f, 0.0f, DirectX::Colors::Teal);
                    */
                    /*
                    DirectX::SimpleMath::Vector3 sphereCenter = aBoundingSphere.Center;
                    DirectX::SimpleMath::Vector3 sphereTop = aBoundingSphere.Center;
                    sphereTop.y += (aBoundingSphere.Radius * 1.0f);
                    m_debugData->PushDebugLinePositionIndicator(sphereCenter, 10.0f, 0.0f, DirectX::Colors::Green);
                    m_debugData->PushDebugLinePositionIndicator(sphereTop, 10.0f, 0.0f, DirectX::Colors::Teal);
                    */

                    Utility::ImpulseForce testForce = explosionImpulseForce;
                    Utility::UpdateImpulseForceBellCurve(testForce, static_cast<float>(aTimeDelta));
                    float testMag1 = testForce.currentMagnitude;
                    Utility::UpdateImpulseForceBellCurve(testForce, static_cast<float>(aTimeDelta));
                    float testMag2 = testForce.currentMagnitude;
                    Utility::UpdateImpulseForceBellCurve(testForce, static_cast<float>(aTimeDelta));
                    float testMag3 = testForce.currentMagnitude;


                    isCollisionTrue = true;
                }
            }
        }
    }
    return isCollisionTrue;
}

bool NPCController::CheckIsNpcLoadQueueEmptyTrue()
{
    if (m_loadQueue.size() > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
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
                            float e = Utility::GetVehicleImpactReboundCoefficient(); // rebound coefficent 

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
                                        else if (distanceToCenter < testV1ClosestSecondCornerDistance)
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

void NPCController::CheckPlayerCollisions()
{

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        const VehicleData& testV1 = m_npcVec[i]->GetVehicleData();

        //const float distance = (m_npcVec[i]->GetPos() - m_npcVec[j]->GetPos()).Length();
        const float distance = (m_npcVec[i]->GetPos() - m_player->GetPos()).Length();
        const float maxCollisionRange = m_npcVec[i]->GetCollisionDetectionRange() * 2.0f;
        // only check collisions in potential range and prevent collision checks with vehicles knocked out of play
        if (distance < maxCollisionRange && distance > -1.0f)
        {
            //const VehicleData& testV2 = m_npcVec[j]->GetVehicleData();
            const DirectX::BoundingOrientedBox playerBox = m_player->GetBoundingBox();

            //if (testV1.collisionBox.Contains(testV2.collisionBox) == true || testV1.collisionBox.Intersects(testV2.collisionBox) == true)
            if (testV1.collisionBox.Contains(playerBox) == true || testV1.collisionBox.Intersects(playerBox) == true)
            {
                const DirectX::SimpleMath::Vector3 playerPosition = m_player->GetPos();
                const DirectX::SimpleMath::Vector3 playerVelocity = m_player->GetVelocity();
                const float playerMass = m_player->GetMass();
                DirectX::SimpleMath::Vector3 p1localizedTo2 = testV1.q.position - playerPosition;
                DirectX::SimpleMath::Vector3 p1localizedTo2Norm = p1localizedTo2;
                p1localizedTo2Norm.Normalize();
                DirectX::SimpleMath::Vector3 p2localizedTo1 = playerPosition - testV1.q.position;
                DirectX::SimpleMath::Vector3 p2localizedTo1Norm = p2localizedTo1;
                p2localizedTo1Norm.Normalize();

                float mass1 = testV1.mass; // aVehicleHit.mass;
                float mass2 = playerMass; // m_vehicleStruct00.vehicleData.mass;
                float e = Utility::GetVehicleImpactReboundCoefficient(); // rebound coefficent 

                float tmp = 1.0f / (mass1 + mass2);
                DirectX::SimpleMath::Vector3 vx1 = testV1.q.velocity; // aVehicleHit.q.velocity;
                DirectX::SimpleMath::Vector3 vx2 = playerVelocity; // m_vehicleStruct00.vehicleData.q.velocity;

                DirectX::SimpleMath::Vector3 aVelocityvx1 = testV1.q.velocity; // aVehicleHit.q.velocity;
                DirectX::SimpleMath::Vector3 aVelocityvx2 = playerVelocity; // m_vehicleStruct00.vehicleData.q.velocity;

                DirectX::SimpleMath::Vector3 aNormVelocity1 = aVelocityvx1;
                aNormVelocity1.Normalize();
                DirectX::SimpleMath::Vector3 aNormVelocity2 = aVelocityvx2;
                aNormVelocity2.Normalize();

                DirectX::SimpleMath::Vector3 aPositionvx1 = testV1.q.position; // aVehicleHit.q.velocity;
                DirectX::SimpleMath::Vector3 aPositionvx2 = playerPosition; // m_vehicleStruct00.vehicleData.q.velocity;

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
                //m_npcVec[j]->m_prevImpact = newVx2;
                m_player->SetTestPostImpactVelocity(newVx2);

                float x1a = p1localizedTo2Norm.Dot(testVelocityNorm1);

                if (x1a < 0.0f)
                {
                    DirectX::XMFLOAT3 testV1Corners[8];
                    DirectX::XMFLOAT3* pCorners1;
                    pCorners1 = testV1Corners;
                    testV1.collisionBox.GetCorners(pCorners1);

                    DirectX::XMFLOAT3 testV2Corners[8];
                    DirectX::XMFLOAT3* pCorners2 = testV2Corners;
                    //testV2.collisionBox.GetCorners(pCorners2);
                    playerBox.GetCorners(pCorners2);
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
                        //float distanceToCenter = (cornerPos - testV2.collisionBox.Center).Length();
                        float distanceToCenter = (cornerPos - playerBox.Center).Length();

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

                    m_npcVec[i]->TestCollisionVelocityUpdate(newVx1);
                    //m_npcVec[j]->TestCollisionVelocityUpdate(newVx2);
                    m_player->SetTestCollisionVelocityUpdate(newVx2);

                    //m_npcVec[i]->CalculateImpulseForce(m_npcVec[j]->GetVehicleData(), newVx1, newVx2, impactPoint);
                    //m_npcVec[j]->CalculateImpulseForce(m_npcVec[i]->GetVehicleData(), newVx2, newVx1, impactPoint);
                    m_npcVec[i]->CalculateImpulseForceFromPlayer(m_player->GetMass(), m_player->GetVelocity(), m_player->GetCenterOfMassPos(), newVx1, newVx2, impactPoint);

                    DirectX::SimpleMath::Vector3 aForceVec1 = newVx2;
                    ////////m_lastImpactPos = aImpactPos;

                    DirectX::SimpleMath::Vector3 testVecUsed = aForceVec1;
                    Utility::ImpulseForce impulseToVec;
                    impulseToVec.currentMagnitude = 0.0f;
                    impulseToVec.currentTime = 0.0f;
                    //impulseToVec.directionNorm = aVehicleHit.q.velocity;
                    impulseToVec.directionNorm = testV1.q.velocity;
                    impulseToVec.directionNorm = testVecUsed;
                    impulseToVec.directionNorm.Normalize();
                    impulseToVec.torqueForceNorm = impulseToVec.directionNorm;
                    impulseToVec.isActive = true;
                    impulseToVec.maxMagnitude = (0.5f * testV1.mass * testV1.q.velocity * testV1.q.velocity).Length();
                    //impulseToVec.torqueArm = aVehicleHit.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
                    impulseToVec.torqueArm = testV1.hardPoints.centerOfMassPos - m_player->GetCenterOfMassPos();
                    //impulseToVec.torqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - aImpactPos;
                    impulseToVec.torqueArm = m_player->GetCenterOfMassPos() - impactPoint;
                    //impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
                    impulseToVec.torqueArm = impactPoint - m_player->GetCenterOfMassPos();
                    impulseToVec.torqueArm *= 1.0f;
                    //float impactVelocity = (aVehicleHit.q.velocity - m_vehicleStruct00.vehicleData.q.velocity).Length();
                    float impactVelocity = (testV1.q.velocity - m_player->GetVelocity()).Length();
                    float impactTime = 1.0f / (impactVelocity + 0.00000000001f);
                    impulseToVec.totalTime = impactTime;
                    impulseToVec.totalTime = 0.1f;

                    impulseToVec.maxMagnitude = aForceVec1.Length() * testV1.mass;

                    //PushImpulseForce(impulseToVec);

                    m_player->SetTestCollisionImpulseForce(impulseToVec);

                    m_player->SetTestVehicleCollisionTrue();
                }
            }
        }
    }
}

bool NPCController::CheckProjectileCollisions(Utility::CollisionData& aProjectile, unsigned int& aVehicleHitId, const bool aIsExplosive)
{
    bool isCollisionTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (DirectX::SimpleMath::Vector3::Distance(aProjectile.collisionSphere.Center, m_npcVec[i]->GetPos()) < m_npcVec[i]->GetCollisionDetectionRange())
        {
            if (m_npcVec[i]->GetCollisionData().Intersects(aProjectile.collisionSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aProjectile.collisionSphere) == true)
            {
                m_npcVec[i]->SetCollisionVal(true);
                if (aIsExplosive == true)
                {
                    m_npcVec[i]->SetExplosionTrue();
                    m_fireControl->PushVehicleExplosion(m_npcVec[i]->GetPos(), m_npcVec[i]->GetID());
                }

                /*
                Utility::ImpactForce projectileForce;
                projectileForce.impactModifier = aProjectile.collisionMagnitudeMod;
                projectileForce.impactMass = aProjectile.mass;
                projectileForce.impactVelocity = aProjectile.velocity;
                m_npcVec[i]->CalculateImpulseForceFromProjectile(projectileForce, aProjectile.collisionSphere.Center);
                */

                Utility::ImpulseForce impulseToVec;
                impulseToVec.currentMagnitude = 0.0f;
                impulseToVec.currentTime = 0.0f;
                impulseToVec.directionNorm = aProjectile.velocity;
                impulseToVec.directionNorm.Normalize();
                impulseToVec.isActive = true;
                float impactVelocity = aProjectile.velocity.Length();
                impulseToVec.maxMagnitude = (0.5f * aProjectile.mass * aProjectile.velocity * aProjectile.velocity).Length();
                impulseToVec.maxMagnitude *= aProjectile.collisionMagnitudeMod;
                impulseToVec.torqueArm = aProjectile.collisionSphere.Center - m_npcVec[i]->GetCenterOfMass();
                impulseToVec.torqueArm *= m_fireControl->GetExplosiveTorqueArmMod();
                impulseToVec.torqueForceNorm = impulseToVec.directionNorm;
                const float velocityF = aProjectile.velocity.Length();
                if (velocityF != 0.0f)
                {
                    impulseToVec.totalTime = (aProjectile.collisionDurationMod) / aProjectile.velocity.Length();
                }
                else
                {
                    const float tol = 0.01f;
                    impulseToVec.totalTime = aProjectile.collisionDurationMod / tol;
                }
                impulseToVec.totalTime = 0.1f;
                impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_FLAT;
                //impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_FRONTLOADCURVE;
                //impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_LAGCURVE;
                impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;

                impulseToVec.forceSum = (aProjectile.mass * impactVelocity);
                impulseToVec.forceRemaining = impulseToVec.forceSum;
                float forceByDistance = (1.0f / (2.0f * 1.0f)) * aProjectile.mass * (impactVelocity * impactVelocity);
                float collisionTime = 1.0f / impactVelocity;
                float forceByTime = (aProjectile.mass * impactVelocity) / impulseToVec.totalTime;
                float forceByTime2 = (aProjectile.mass * impactVelocity) / collisionTime;
                //impulseToVec.maxMagnitude = forceByTime2;
                //impulseToVec.totalTime = collisionTime;
                m_npcVec[i]->PushImpulseForce(impulseToVec);

                float testDuration1 = (aProjectile.collisionSphere.Radius * 1.0f) / aProjectile.velocity.Length();
                float testDuration2 = (aProjectile.collisionSphere.Radius * 2.0f) / aProjectile.velocity.Length();
                float testDuration3 = (aProjectile.collisionSphere.Radius * 3.0f) / aProjectile.velocity.Length();
                float testDuration4 = (aProjectile.collisionSphere.Radius * 4.0f) / aProjectile.velocity.Length();

                isCollisionTrue = true;
                aVehicleHitId = m_npcVec[i]->GetID();
            }
        }
    }
    return isCollisionTrue;
}

bool NPCController::CheckProjectileCollisionsMissile(Utility::CollisionData& aProjectile, unsigned int& aVehicleHitId, const bool aIsExplosive, const int aTargetId, const float aDetonationRange, bool& aIsProximityDetonationTrue)
{
    bool isCollisionTrue = false;
    bool isProximityTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aTargetId)
        {
            if (DirectX::SimpleMath::Vector3::Distance(aProjectile.collisionSphere.Center, m_npcVec[i]->GetPos()) < aDetonationRange)
            {
                isCollisionTrue = true;
                isProximityTrue = true;
                m_npcVec[i]->SetIsMissileDetonationTrue();
            }
        }
        else if (DirectX::SimpleMath::Vector3::Distance(aProjectile.collisionSphere.Center, m_npcVec[i]->GetPos()) < m_npcVec[i]->GetCollisionDetectionRange())
        {
            if (m_npcVec[i]->GetCollisionData().Intersects(aProjectile.collisionSphere) == true || m_npcVec[i]->GetCollisionData().Contains(aProjectile.collisionSphere) == true)
            {
                m_npcVec[i]->SetCollisionVal(true);
                if (aIsExplosive == true)
                {
                    m_npcVec[i]->SetExplosionTrue();
                    m_fireControl->PushVehicleExplosion(m_npcVec[i]->GetPos(), m_npcVec[i]->GetID());
                }

                /*
                Utility::ImpactForce projectileForce;
                projectileForce.impactModifier = aProjectile.collisionMagnitudeMod;
                projectileForce.impactMass = aProjectile.mass;
                projectileForce.impactVelocity = aProjectile.velocity;
                m_npcVec[i]->CalculateImpulseForceFromProjectile(projectileForce, aProjectile.collisionSphere.Center);
                */

                Utility::ImpulseForce impulseToVec;
                impulseToVec.currentMagnitude = 0.0f;
                impulseToVec.currentTime = 0.0f;
                impulseToVec.directionNorm = aProjectile.velocity;
                impulseToVec.directionNorm.Normalize();
                impulseToVec.isActive = true;
                float impactVelocity = aProjectile.velocity.Length();
                impulseToVec.maxMagnitude = (0.5f * aProjectile.mass * aProjectile.velocity * aProjectile.velocity).Length();
                impulseToVec.maxMagnitude *= aProjectile.collisionMagnitudeMod;
                impulseToVec.torqueArm = aProjectile.collisionSphere.Center - m_npcVec[i]->GetCenterOfMass();
                impulseToVec.torqueArm *= m_fireControl->GetExplosiveTorqueArmMod();
                impulseToVec.torqueForceNorm = impulseToVec.directionNorm;
                const float velocityF = aProjectile.velocity.Length();
                if (velocityF != 0.0f)
                {
                    impulseToVec.totalTime = (aProjectile.collisionDurationMod) / aProjectile.velocity.Length();
                }
                else
                {
                    const float tol = 0.01f;
                    impulseToVec.totalTime = aProjectile.collisionDurationMod / tol;
                }
                impulseToVec.totalTime = 0.1f;
                impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_FLAT;
                //impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_FRONTLOADCURVE;
                //impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_LAGCURVE;
                impulseToVec.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;

                impulseToVec.forceSum = (aProjectile.mass * impactVelocity);
                impulseToVec.forceRemaining = impulseToVec.forceSum;
                float forceByDistance = (1.0f / (2.0f * 1.0f)) * aProjectile.mass * (impactVelocity * impactVelocity);
                float collisionTime = 1.0f / impactVelocity;
                float forceByTime = (aProjectile.mass * impactVelocity) / impulseToVec.totalTime;
                float forceByTime2 = (aProjectile.mass * impactVelocity) / collisionTime;
                //impulseToVec.maxMagnitude = forceByTime2;
                //impulseToVec.totalTime = collisionTime;
                m_npcVec[i]->PushImpulseForce(impulseToVec);

                float testDuration1 = (aProjectile.collisionSphere.Radius * 1.0f) / aProjectile.velocity.Length();
                float testDuration2 = (aProjectile.collisionSphere.Radius * 2.0f) / aProjectile.velocity.Length();
                float testDuration3 = (aProjectile.collisionSphere.Radius * 3.0f) / aProjectile.velocity.Length();
                float testDuration4 = (aProjectile.collisionSphere.Radius * 4.0f) / aProjectile.velocity.Length();

                isCollisionTrue = true;
                aVehicleHitId = m_npcVec[i]->GetID();
            }
        }
    }
    aIsProximityDetonationTrue = isCollisionTrue;
    return isCollisionTrue;
}

int NPCController::CheckTargetingLaser(DirectX::SimpleMath::Ray aRay,float& aDistance)
{
    bool isTargetHit = false;
    float distanceToTarget = 10000.0f;
    int targetID = -1;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        float distance = 0.0f;
        DirectX::SimpleMath::Vector3 localizedPos = aRay.position - m_npcVec[i]->GetPos();
        localizedPos = DirectX::SimpleMath::Vector3::Transform(localizedPos, m_npcVec[i]->GetAlignmentInverseQuat());
        DirectX::SimpleMath::Vector3 localizedRayDirection = DirectX::SimpleMath::Vector3::Transform(aRay.direction, m_npcVec[i]->GetAlignmentInverseQuat());
        DirectX::SimpleMath::Ray localizedRay = DirectX::SimpleMath::Ray(localizedPos, localizedRayDirection);
        DirectX::BoundingOrientedBox testOrientedBox = m_npcVec[i]->GetCollisionData();
        DirectX::BoundingBox testBox;
        testBox.Center = testOrientedBox.Center;
        testBox.Center = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 collisionDimensions = DirectX::SimpleMath::Vector3(16.0f, 8.0f, 10.0f);
        testBox.Extents = collisionDimensions * 0.5f;

        //if (aRay.Intersects(testBox, distance) == true)
        if (localizedRay.Intersects(testBox, distance) == true)
        {
            if (isTargetHit == false)
            {
                distanceToTarget = distance;
                targetID = m_npcVec[i]->GetID();
            }
            else
            {
                if (distance < distanceToTarget)
                {
                    targetID = m_npcVec[i]->GetID();
                    distanceToTarget = distance;
                }
            }
        }
    }

    //m_debugData->PushDebugLine(aRay.position, aRay.direction, distanceToTarget, 0.0f, DirectX::Colors::Red);
    aDistance = distanceToTarget;
    return targetID;
}

void NPCController::CheckTargetingLaser(DirectX::SimpleMath::Ray aRay, float& aDistance, int& aTargetId, bool& aIsTargetLockTrue)
{
    bool isTargetHit = false;
    float distanceToTarget = 10000.0f;
    int targetID = -1;
    int targetIndex = 0;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        float distance = 0.0f;
        DirectX::SimpleMath::Vector3 localizedPos = aRay.position - m_npcVec[i]->GetPos();
        localizedPos = DirectX::SimpleMath::Vector3::Transform(localizedPos, m_npcVec[i]->GetAlignmentInverseQuat());
        DirectX::SimpleMath::Vector3 localizedRayDirection = DirectX::SimpleMath::Vector3::Transform(aRay.direction, m_npcVec[i]->GetAlignmentInverseQuat());
        DirectX::SimpleMath::Ray localizedRay = DirectX::SimpleMath::Ray(localizedPos, localizedRayDirection);
        DirectX::BoundingOrientedBox testOrientedBox = m_npcVec[i]->GetCollisionData();
        DirectX::BoundingBox testBox;
        testBox.Center = testOrientedBox.Center;
        testBox.Center = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 collisionDimensions = DirectX::SimpleMath::Vector3(16.0f, 8.0f, 10.0f);
        testBox.Extents = collisionDimensions * 0.5f;

        if (localizedRay.Intersects(testBox, distance) == true)
        {
            if (isTargetHit == false)
            {
                distanceToTarget = distance;
                targetID = m_npcVec[i]->GetID();
                isTargetHit = true;
                targetIndex = i;
            }
            else
            {
                if (distance < distanceToTarget)
                {
                    targetID = m_npcVec[i]->GetID();
                    distanceToTarget = distance;
                    targetIndex = i;
                }
            }
        }
    }

    /////////////////////////////////////////////
    // force target for testing

    /*
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {   
        float distance = (m_npcVec[i]->GetPos() - aRay.position).Length();
        distanceToTarget = distance;
        targetID = m_npcVec[i]->GetID();
        isTargetHit = true;
        targetIndex = i;
    }
    */

    /////////////////////////////////////////////
    if (isTargetHit == true)
    {
        //m_npcVec[targetIndex]->SetIsTargetedTrue();
        m_npcVec[targetIndex]->SetIsPlayerTargetedTrue();
    }

    aDistance = distanceToTarget;
    aTargetId = targetID;
    aIsTargetLockTrue = isTargetHit;
}

void NPCController::DebugToggleAI()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->DebugToggleAI();
    }
}

void NPCController::DrawNPCs(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsDead() == false && m_npcVec[i]->GetIsActivated() == true && m_npcVec[i]->GetIsInCameraFrustrum() == true)
        {
            m_npcVec[i]->DrawNPC(aView, aProj, aEffect, aInputLayout);
        }
    }
}

bool NPCController::GetFreeTargetsData(std::vector<unsigned int>& aTargetList)
{
    bool isListUpdatedTrue = false;

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsMissileTargetingMe() == false)
        {
            aTargetList.push_back(m_npcVec[i]->GetID());
            isListUpdatedTrue = true;
        }
    }

    return isListUpdatedTrue;
}

bool NPCController::GetIsDebugPauseToggleTrue()
{
    bool isDebugPauseTrue = false;
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsDebugPauseToggleTrue() == true)
        {
            isDebugPauseTrue = true;
        }
    }
    return isDebugPauseTrue;
}

bool NPCController::GetIsNpcAliveTrue(int aId)
{
    bool isAliveTrue = false;
    unsigned int index = 0;

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            if (m_npcVec[i]->GetIsDead() == false && m_npcVec[i]->GetIsExploding() == false)
            {
                isAliveTrue = true;
                index = i;
            }
        }
    }

    if (isAliveTrue == false)
    {
        bool isDead = m_npcVec[index]->GetIsDead();
        bool isExploding = m_npcVec[index]->GetIsExploding();
        int testBreak = 0;
        testBreak++;
    }
    return isAliveTrue;
}

bool NPCController::GetIsNpcUntargetedTrue(int aId)
{
    bool isUntargetedTrue = true;
    unsigned int index = 0;

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            if (m_npcVec[i]->GetIsMissileTargetingMe() == true && m_npcVec[i]->GetIsExploding() == false && m_npcVec[i]->GetIsDead() == false && m_npcVec[i]->GetIsMissileDetonationTrue() == false)
            {
                isUntargetedTrue = false;
                index = i;
            }
        }
    }

    if (isUntargetedTrue == false)
    {
        bool isDead = m_npcVec[index]->GetIsDead();
        bool isExploding = m_npcVec[index]->GetIsExploding();
        bool isTargeted = m_npcVec[index]->GetIsMissileTargetingMe();
        int testBreak = 0;
        testBreak++;
    }
    return isUntargetedTrue;
}

DirectX::SimpleMath::Vector3 NPCController::GetNextSpawnerLoc()
{
    if (m_isAltSpawnLocTrue == true)
    {
        return m_spawnerPos;
        //return m_spawnerPosAlt;
    }
    else
    {
        return m_spawnerPosAlt;
        //return m_spawnerPos;
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
    if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_BLANK)
    {
        m_textureDataBlank.textureMapType = aTextureMapType;
        m_textureDataBlank.normalMap = aNormalMap;
        m_textureDataBlank.specularMap = aSpecularMap;
        m_textureDataBlank.textureMap = aTexture;
    }
    if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_FLAME)
    {
        m_textureDataFlame.textureMapType = aTextureMapType;
        m_textureDataFlame.normalMap = aNormalMap;
        m_textureDataFlame.specularMap = aSpecularMap;
        m_textureDataFlame.textureMap = aTexture;
    }
    if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_TEST1)
    {
        m_textureDataTest1.textureMapType = aTextureMapType;
        m_textureDataTest1.normalMap = aNormalMap;
        m_textureDataTest1.specularMap = aSpecularMap;
        m_textureDataTest1.textureMap = aTexture;
    }
    if (aTextureMapType == NpcTextureMapType::TEXTUREMAPTYPE_TEST2)
    {
        m_textureDataTest2.textureMapType = aTextureMapType;
        m_textureDataTest2.normalMap = aNormalMap;
        m_textureDataTest2.specularMap = aSpecularMap;
        m_textureDataTest2.textureMap = aTexture;
    }

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->InitializeTextureMaps(aTextureMapType, aTexture, aNormalMap, aSpecularMap);
    }
}

void NPCController::LoadNPCs(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController)
{
    //DirectX::SimpleMath::Vector3 heading = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 heading = m_startHeading;
    
    const float low = 0.1f;
    const float high = 5.0f;
    //const float zPosOffSet = 12.0f;
    //const float zPosOffSet = 25.0f;
    const float zPosOffSet = -75.0f;
    float baseHeight = 10.0f;
    //const int rows = 6;
    //const int columns = 4;
    const int rows = 1;
    //const int columns = 12;
    const int columns = 1;
    //const float xOrgVal = -500.0f;
    //const float xOrgVal = -200.0f;
    //const float zOrgVal = (static_cast<float>(columns * 0.5f) * -zPosOffSet) + (zPosOffSet * 0.5f);
    //const float xOrgVal = -270.0f;
    const float xOrgVal = 570.0f;
    const float zOrgVal = -375.0f;
    //DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(xOrgVal, 11.0f, -40.0f);
    //DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(xOrgVal, 11.0f, zOrgVal);
    DirectX::SimpleMath::Vector3 pos = m_startPos;
    
    for (int i = 0; i < columns; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            float yOffSet = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
            pos.y = baseHeight + yOffSet;
            this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
            pos.x += 55.0f;
        }
        //baseHeight = 120.0f;
        pos.x = xOrgVal;
        pos.z += zPosOffSet;
    }
    

    pos = m_startPos;
    //this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);

    const float xLine = 85.0f;
    const float yLine = 10.0f;
    const float zSpacing = 90.0f;
    // target 1
    pos.x = xLine;
    pos.y = yLine;
    pos.z = -300.0f;
    //this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
}

void NPCController::LoadNPCsTestFireRange(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController)
{
    const float xOrgVal = 70.0f;
    const float yOrgVal = 11.0f;
    const float zOrgVal = -240.0f;
    DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(xOrgVal, yOrgVal, zOrgVal);
    DirectX::SimpleMath::Vector3 heading = -DirectX::SimpleMath::Vector3::UnitX;
    const float low = 0.1f;
    const float high = 5.0f;
    const float zPosSpacing = 62.0f;
    float baseHeight = 10.0f;
    const int rows = 1;
    const int columns = 4;
    for (int i = 0; i < columns; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            float yOffSet = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
            pos.y = baseHeight + yOffSet;
            this->AddNPC(aContext, NPCType::NPCTYPE_NPC00, heading, pos, aNpcController);
            pos.x += 25.0f;
        }
   
        pos.x = xOrgVal;
        pos.z += zPosSpacing;
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

void NPCController::LoadToQueue(const DirectX::SimpleMath::Vector3 aLoadPosition, const DirectX::SimpleMath::Vector3 aHeading, const unsigned int aColumnCount, const unsigned int aRowCount,
    const DirectX::SimpleMath::Vector2 aSpacing, const DirectX::SimpleMath::Quaternion aQuat)
{
    DirectX::SimpleMath::Vector3 heading = aHeading;
    DirectX::SimpleMath::Vector3 columnOffset = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, aSpacing.x), aQuat);
    DirectX::SimpleMath::Vector3 rowOffset = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(aSpacing.y, 0.0f, 0.0f), aQuat);
    DirectX::SimpleMath::Vector3 pos = aLoadPosition;
    pos -= columnOffset * (static_cast<float>(aColumnCount - 1) * 0.5f);
    pos -= rowOffset * (static_cast<float>(aRowCount - 1) * 0.5f);
    const DirectX::SimpleMath::Vector3 originPos = pos;
    LoadQueue loadData;
    loadData.deployType = NPCType::NPCTYPE_NPC00;
    for (unsigned int i = 0; i < aRowCount; ++i)
    {
        for (unsigned int j = 0; j < aColumnCount; ++j)
        {
            loadData.deployOrientation = heading;
            loadData.deployPosition = pos;
            m_loadQueue.push_back(loadData);
            pos += columnOffset;
        }
        pos = originPos;
        pos += (rowOffset * static_cast<float>(i + 1));
    }
}

void NPCController::LoadToQueueAxisAligned(const DirectX::SimpleMath::Vector3 aLoadPosition, const DirectX::SimpleMath::Vector3 aOrientation, 
    const unsigned int aColumnCount, const unsigned int aRowCount, const float aColumnSpacing, const float aRowSpacing)
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

void NPCController::ResetNpcDebugPauseToggle()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->ResetDebugPauseToggle();
    }
}

void NPCController::SetAllNpcsToDead()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetIsInCameraFrustrum() == false)
        {
            m_npcVec[i]->SetDeadTrue();
        }
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
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aVehicleId)
        {
            m_npcVec[i]->SetDeadTrue();
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

void NPCController::ToggleDebugBool()
{
    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        m_npcVec[i]->ToggleDebugBool();
    }
}

void NPCController::ToggleSpawner()
{
    if (m_isAltSpawnLocTrue == true)
    {
        m_isAltSpawnLocTrue = false;
    }
    else
    {
        m_isAltSpawnLocTrue = true;
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
        const unsigned int i = static_cast<unsigned int>(m_loadQueue.size() - 1);
        this->AddNPC(aContext, m_loadQueue[i].deployType, m_loadQueue[i].deployOrientation, m_loadQueue[i].deployPosition, aNpcController);
        m_loadQueue.pop_back();
    }
}

void NPCController::UpdateLoadQueueWithAudio(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, std::shared_ptr<NPCController> aNpcController, std::shared_ptr<Utility::SoundFx> aAudioFx)
{
    if (m_loadQueue.size() > 0)
    {
        const unsigned int i = static_cast<unsigned int>(m_loadQueue.size() - 1);
        this->AddNpcWithAudio(aContext, m_loadQueue[i].deployType, m_loadQueue[i].deployOrientation, m_loadQueue[i].deployPosition, aNpcController, aAudioFx);
        m_loadQueue.pop_back();
    }
}

void NPCController::UpdateMissleGuidance(const int aId, DirectX::SimpleMath::Vector3& aPosition, DirectX::SimpleMath::Vector3& aVelocity, DirectX::SimpleMath::Vector3& aForward)
{
    int idIndex = -1;
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            if (idIndex == -1)
            {
                idIndex = i;
            }
            else
            {
                int testBreak = 0;
                ++testBreak;
            }
        }
    }
    if (idIndex != -1)
    {
        aPosition = m_npcVec[idIndex]->GetPos();
        aVelocity = m_npcVec[idIndex]->GetVelocity();
    }
    else
    {
        int testBreak = 0;
        ++testBreak;
    }
}

bool NPCController::UpdateMissleGuidanceBool(const int aId, DirectX::SimpleMath::Vector3& aPosition, DirectX::SimpleMath::Vector3& aVelocity, DirectX::SimpleMath::Vector3& aForward)
{
    int idIndex = -1;
    bool isIdFound = false;
    for (int i = 0; i < m_npcVec.size(); ++i)
    {
        if (m_npcVec[i]->GetID() == aId)
        {
            if (idIndex == -1)
            {
                idIndex = i;
            }
            else
            {
                int testBreak = 0;
                ++testBreak;
            }
        }
    }
    if (idIndex != -1)
    {
        isIdFound = true;
        aPosition = m_npcVec[idIndex]->GetPos();
        aVelocity = m_npcVec[idIndex]->GetVelocity();
        m_npcVec[idIndex]->SetIsMissileTargetedTrue();
    }
    else
    {
        isIdFound = false;
        int testBreak = 0;
        ++testBreak;
    }
    return isIdFound;
}

void NPCController::UpdateNPCController(const DirectX::BoundingFrustum& aFrustum, const double aTimeDelta)
{
    UpdateSpawner(aTimeDelta);
    UpdateNPCs(aFrustum, aTimeDelta);

    /*
    loadData.deployPosition = m_spawnerPosAlt;
    loadData.isAltTrue = true;
    m_isAltSpawnLocTrue = false;
}
    else
    {
    loadData.deployType = NPCType::NPCTYPE_SPAWNED;
    loadData.deployOrientation = m_spawnerHeading;
    loadData.deployPosition = m_spawnerPos;

    */
}

void NPCController::UpdateNPCs(const DirectX::BoundingFrustum& aFrustum, const double aTimeDelta)
{
    m_testTimer += static_cast<float>(aTimeDelta);

    ActivateNPCs();

    for (unsigned int i = 0; i < m_npcVec.size(); ++i)
    {
        
        if (m_npcVec[i]->GetID() == 0 && m_npcVec[i]->GetDebugVal() == false)
        {
            /*
            DirectX::SimpleMath::Vector3 playerPos = m_player->GetPos();
            const DirectX::SimpleMath::Matrix playerAlignment = m_player->GetAlignment();
            const DirectX::SimpleMath::Vector3 playerVelocity = m_player->GetVelocity();
            m_npcVec[i]->SetPos(playerPos);
            m_npcVec[i]->SetAlignment(playerAlignment);
            m_npcVec[i]->SetVelocity(playerVelocity);
            */
        }
        
        DirectX::SimpleMath::Vector3 playerPos = m_player->GetPos();
        const DirectX::SimpleMath::Matrix playerAlignment = m_player->GetAlignment();
        const DirectX::SimpleMath::Vector3 playerVelocity = m_player->GetVelocity();
        m_npcVec[i]->UpdatePlayerPos(playerPos);
        m_npcVec[i]->UpdatePlayerAlignment(playerAlignment);
        m_npcVec[i]->UpdatePlayerVelocity(playerVelocity);

        m_npcVec[i]->CheckIfInCameraFrustum(aFrustum);
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
    CheckPlayerCollisions();
    if (m_isNpcAiOn == true)
    {
        CheckNpcAvoidance();
    }
}

void NPCController::UpdateSpawner(const double aTimeDelta)
{
    m_spawnCount = m_npcVec.size();

    if (m_spawnCount < m_spawnCountMax)
    {
        m_spawnerCooldown -= static_cast<float>(aTimeDelta);

        if (m_spawnerCooldown < 0.0f)
        {
            m_isSpawnerReady = true;
            m_spawnerCooldown = m_spawnerCooldownTime;
        }

        if (m_isSpawnerReady == true)
        {
            SpawnToQueue();
            m_isSpawnerReady = false;
            m_spawnerCooldown = m_spawnerCooldownTime;
        }
    }
}

void NPCController::SpawnToQueue()
{
    LoadQueue loadData;
    
    if (m_isAltSpawnLocTrue == true)
    {
        loadData.deployType = NPCType::NPCTYPE_SPAWNEDALT;
        loadData.deployOrientation = m_spawnerHeadingAlt;
        loadData.deployPosition = m_spawnerPosAlt;
        loadData.isAltTrue = true;
        //m_isAltSpawnLocTrue = false;
    }
    else
    {
        loadData.deployType = NPCType::NPCTYPE_SPAWNED;
        loadData.deployOrientation = m_spawnerHeading;
        loadData.deployPosition = m_spawnerPos;
        loadData.isAltTrue = false;
        //m_isAltSpawnLocTrue = true;
    }

    m_loadQueue.push_back(loadData);
}

