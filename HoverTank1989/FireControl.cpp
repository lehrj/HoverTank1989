#include "pch.h"
#include "FireControl.h"

/*
void FireControl::ActivateMuzzleFlash()
{
    m_muzzleFlash.flashTimer = 0.0f;
    m_muzzleFlash.isFlashActive = true;
    m_muzzleFlash.sizeMod = 0.0f;
}
*/

void FireControl::ActivateMuzzleFlash(AmmoType aAmmoType)
{
    if (aAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.4f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.25f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.45f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else if (aAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.3f;
        m_muzzleFlash.growthRate = 20.0f;
    }
    else // default to cannon muzzle flash aAmmoType == AmmoType::AMMOTYPE_CANNON
    {
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.isFlashActive = true;
        m_muzzleFlash.sizeMod = 0.0f;
        m_muzzleFlash.flashDuration = 0.35f;
        m_muzzleFlash.growthRate = 20.0f;
    }
}

void FireControl::CastRayLaser()
{
    DirectX::SimpleMath::Ray laserRay = DirectX::SimpleMath::Ray(m_playerVehicle->GetMuzzlePos(), m_playerVehicle->GetWeaponDirection());
    bool isTargetHit = false;
    float distanceToTarget = 0.0f;
    int targetID = m_npcController->CheckTargetingLaser(laserRay, distanceToTarget);
    m_playerLaser.distance = distanceToTarget;
    //m_debugData->DebugPushUILineWholeNumber("m_playerLaser.distance = ", m_playerLaser.distance, "");
    m_debugData->DebugPushUILineWholeNumber("targetID = ", targetID, "");
    m_currentTargetID = targetID;
}

void FireControl::CheckCollisions()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        unsigned int vehicleHitId = 0;
        bool isHitTrue;
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE || m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId, true);
        }
        else
        {
            isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId, false);
        }

        //bool isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId);
        if (isHitTrue == true)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE || m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
            {
                CreateExplosion(m_projectileVec[i].q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE, static_cast<unsigned int>(vehicleHitId));
            }
            m_projectileVec[i].isCollisionTrue = true;
            m_projectileVec[i].liveTimeTick--;
            if (m_projectileVec[i].liveTimeTick <= 0)
            {
                m_projectileVec[i].isDeleteTrue = true;
            }
        }
        else if (m_projectileVec[i].time > m_projectileLifeTimeMax)
        {
            m_projectileVec[i].isDeleteTrue = true;
        }
        else if (m_environment->GetIsPosInPlay(m_projectileVec[i].q.position) == false)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
            {
                CreateExplosion(m_projectileVec[i].q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_NONVEHICLE, -1);
            }
            m_projectileVec[i].isDeleteTrue = true;
        }
    }
}

void FireControl::CheckCollisionsMissile()
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        if (m_missileVec[i].guidance.isExplodingTrue == false)
        {
            unsigned int vehicleHitId = -1;
            bool isHitTrue;
            bool isProximityDetonationTrue = false;
            isHitTrue = m_npcController->CheckProjectileCollisionsMissile(m_missileVec[i].projectileData.collisionData, vehicleHitId, true, m_missileVec[i].guidance.targetID, m_missileConsts.detonationRange, isProximityDetonationTrue);

            if (isProximityDetonationTrue == true)
            {
                CreateExplosion(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.q.velocity, ExplosionType::EXPLOSIONTYPE_DYNAMIC, -1);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (vehicleHitId != -1 && isHitTrue == true)
            {
                CreateExplosion(m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE, vehicleHitId);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (isHitTrue == true)
            {
                /*
                m_missileVec[i].projectileData.isCollisionTrue = true;
                m_missileVec[i].projectileData.liveTimeTick--;
                if (m_missileVec[i].projectileData.liveTimeTick <= 0)
                {
                    m_missileVec[i].projectileData.isDeleteTrue = true;
                }
                */
                CreateExplosion(m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_NONVEHICLE, -1);
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_missileVec[i].projectileData.time > m_missileLifeTimeMax)
            {
                m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_environment->GetIsPosInPlay(m_missileVec[i].projectileData.q.position) == false)
            {
                //m_missileVec[i].projectileData.isDeleteTrue = true;
            }
            else if (m_missileVec[i].guidance.isSelfDestructTrue == true)
            {
                CreateExplosion(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.q.velocity, ExplosionType::EXPLOSIONTYPE_DYNAMIC, -1);
                //m_missileVec[i].projectileData.isDeleteTrue = true;

                m_missileVec[i].guidance.isExplodingTrue = true;
                m_missileVec[i].guidance.isRocketFired = false;
                m_missileVec[i].guidance.forwardThrust = 0.0f;
                m_missileVec[i].guidance.throttlePercentage = 0.0f;
            }
        }
    }
}

void FireControl::CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aVelocity, ExplosionType aExplosionType, const int aVehicleId)
{
    ExplosionData createdExplosion;
    createdExplosion.position = aPos;
    createdExplosion.explosionStartColor = m_explosionStruct.explosionRefData.explosionStartColor;
    createdExplosion.explosionEndColor = m_explosionStruct.explosionRefData.explosionEndColor;
    createdExplosion.explosionCurrentColor = createdExplosion.explosionStartColor;
    createdExplosion.color0 = createdExplosion.explosionStartColor;
    createdExplosion.color1 = createdExplosion.explosionStartColor;
    createdExplosion.color2 = createdExplosion.explosionStartColor;
    createdExplosion.color3 = createdExplosion.explosionStartColor;
    createdExplosion.color4 = createdExplosion.explosionStartColor;
    createdExplosion.color5 = createdExplosion.explosionStartColor;
    createdExplosion.color6 = createdExplosion.explosionStartColor;
    createdExplosion.color7 = createdExplosion.explosionStartColor;
    createdExplosion.color8 = createdExplosion.explosionStartColor;

    createdExplosion.currentDuration = 0.0f;
    createdExplosion.currentRadius = m_explosionStruct.explosionRefData.currentRadius;

    createdExplosion.initialRadius = m_explosionStruct.explosionRefData.initialRadius;
    createdExplosion.collisionSphere = m_explosionStruct.explosionRefData.collisionSphere;
    createdExplosion.collisionSphere.Center = aPos;
    createdExplosion.collisionSphere.Radius = createdExplosion.initialRadius;
    createdExplosion.localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(aPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
    createdExplosion.explosionMatrix0 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix1 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix2 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix3 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix4 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix5 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix6 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix7 = createdExplosion.localExplosionMatrix;
    createdExplosion.explosionMatrix8 = createdExplosion.localExplosionMatrix;

    createdExplosion.maxRadius = m_explosionStruct.explosionRefData.maxRadius;

    createdExplosion.totalDuration = m_explosionStruct.explosionRefData.totalDuration;
    createdExplosion.isLifeTimeExpired = false;

    const float randomRotationVariation = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (Utility::GetPi())));
    createdExplosion.rotationVariationMatrix = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(randomRotationVariation, randomRotationVariation, randomRotationVariation);

    if (aExplosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL;
        createdExplosion.vehicleExplosionID = aVehicleId;
    }
    else if (aExplosionType == ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE;
        createdExplosion.vehicleExplosionID = aVehicleId;
        DirectX::SimpleMath::Matrix vehicleAlignment = m_npcController->GetNpcAlignment(aVehicleId);
        vehicleAlignment = vehicleAlignment.Invert();
        DirectX::SimpleMath::Vector3 posOnVehicleSurface = aPos - m_npcController->GetNpcPos(aVehicleId);
        posOnVehicleSurface = DirectX::SimpleMath::Vector3::Transform(posOnVehicleSurface, vehicleAlignment);
        createdExplosion.localizedSurfaceExplosionPos = posOnVehicleSurface;
    }
    else if (aExplosionType == ExplosionType::EXPLOSIONTYPE_DYNAMIC)
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_DYNAMIC;
        createdExplosion.velocity = aVelocity;
        createdExplosion.vehicleExplosionID = -1;
    }
    else
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_NONVEHICLE;
        createdExplosion.vehicleExplosionID = -1;
    }
    m_explosionStruct.explosionVec.push_back(createdExplosion);
}

void FireControl::CycleLoadedAmmo()
{
    if (m_currentAmmoType == AmmoType::AMMOTYPE_CANNON)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_MACHINEGUN;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_SHOTGUN;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_EXPLOSIVE;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_MIRV;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_GUIDEDMISSILE)
    {
        m_currentAmmoType = AmmoType::AMMOTYPE_CANNON;
    }
}

void FireControl::DeleteMissileFromVec(const unsigned int aIndex)
{
    if (aIndex > m_missileVec.size())
    {
        // add error handling here
    }
    else
    {
        std::vector<MissileData>::iterator it;
        it = m_missileVec.begin() + aIndex;
        m_missileVec.erase(it);
    }
}

void FireControl::DeleteProjectileFromVec(const unsigned int aIndex)
{
    if (aIndex > m_projectileVec.size())
    {
        // add error handling here
    }
    else
    {
        std::vector<ProjectileData>::iterator it;
        it = m_projectileVec.begin() + aIndex;
        m_projectileVec.erase(it);
    }
}

void FireControl::DeployMirv(ProjectileData& aProjectile)
{
    AmmoData firedAmmo = m_ammoExplosive.ammoData;

    ProjectileData firedProjectile;
    firedProjectile.ammoData = firedAmmo;
    firedProjectile.q.position = aProjectile.q.position;
    firedProjectile.q.velocity = aProjectile.q.velocity;
    firedProjectile.forward = aProjectile.forward;
    firedProjectile.right = aProjectile.right;
    firedProjectile.up = aProjectile.up;
    firedProjectile.alignmentQuat = aProjectile.alignmentQuat;
    firedProjectile.inverseAlignmentQuat = aProjectile.inverseAlignmentQuat;

    firedProjectile.isCollisionTrue = false;
    firedProjectile.isDeleteTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
    firedProjectile.time = 0.0f;

    // collision data
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

    DirectX::SimpleMath::Vector3 velocityNorm = firedProjectile.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 right = velocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
    right = aProjectile.right;
    DirectX::SimpleMath::Vector3 up = velocityNorm.Cross(-right);
    right = velocityNorm.Cross(up);

    DirectX::SimpleMath::Vector3 forward = aProjectile.forward;
    right = aProjectile.right;
    up = aProjectile.up;

    const int rowCount = 8;
    const int columnCount = 5;

    float rowSpacingAngle = Utility::ToRadians(2.0f);
    float columnSpacingAngle = Utility::ToRadians(12.0f);
    float yawOrg = columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f);
    float pitchOrg = rowSpacingAngle * (static_cast<float>(rowCount) * 0.5f) - (rowSpacingAngle * 0.5f);
    float yawVal = yawOrg;
    float pitchVal = pitchOrg;

    DirectX::SimpleMath::Vector3 updateVelocity = firedProjectile.q.velocity;

    // calculate angle at deploy
    DirectX::SimpleMath::Vector3 flatForward = right.Cross(-DirectX::SimpleMath::Vector3::UnitY);
    const float flatDelta = Utility::GetAngleBetweenVectors(forward, flatForward);

    const float deployDownAngle = Utility::ToRadians(-30.0f) - flatDelta;
    float deployDownMod = Utility::ToRadians(-15.0f);
    DirectX::SimpleMath::Quaternion deployDownQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle);
    //updateVelocity = DirectX::SimpleMath::Vector3::Transform(updateVelocity, DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle));
    updateVelocity = DirectX::SimpleMath::Vector3::Transform(updateVelocity, deployDownQuat);
    const float velocityBoostMod = 2.0f;
    updateVelocity *= velocityBoostMod;

    const float angleOffsetMin = Utility::ToRadians(-0.5f);
    const float angleOffSetMax = Utility::ToRadians(0.5f);
    float yawMod = 0.0f;
    float rowMod = 0.0f;
    float yawTestMod = Utility::ToRadians(2.0f);
    std::vector<float> testYawVec;
    testYawVec.clear();
    std::vector<float> testPitchVec;
    testPitchVec.clear();
    for (int i = 0; i < rowCount; ++i)
    {
        if (static_cast<float>(i) < static_cast<float>(rowCount * 0.5f))
        {
            columnSpacingAngle += yawTestMod;
            yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));
        }
        else
        {
            columnSpacingAngle -= yawTestMod;
            yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));
        }
        for (int j = 0; j < columnCount; ++j)
        {
            if (static_cast<float>(i) <= static_cast<float>(rowCount * 0.5f))
            {
                if (static_cast<float>(j) <= static_cast<float>(columnCount * 0.5f))
                {
                    //yawVal -= yawTestMod;
                }
                else
                {
                    //yawVal += yawTestMod;
                }
            }
            else
            {
                if (static_cast<float>(j) <= static_cast<float>(columnCount * 0.5f))
                {
                    //yawVal += yawTestMod;
                }
                else
                {
                    //yawVal -= yawTestMod;
                }
            }

            ProjectileData mirv = firedProjectile;
            DirectX::SimpleMath::Matrix rotMatYaw = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.up, yawVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            DirectX::SimpleMath::Matrix rotMatPitch = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.right, pitchVal + angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            //DirectX::SimpleMath::Matrix rotMatYaw = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.up, yawVal);// +angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));
            //DirectX::SimpleMath::Matrix rotMatPitch = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(mirv.right, pitchVal);// +angleOffsetMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (angleOffSetMax - angleOffsetMin))));

            DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::Identity;
            rotMat *= rotMatYaw;
            //rotMat *= rotMatPitch;
            if (j == 0)
            {
                testPitchVec.push_back(Utility::ToDegrees(pitchVal));
                testYawVec.push_back(Utility::ToDegrees(yawVal));
            }

            DirectX::SimpleMath::Quaternion updateQuat = DirectX::SimpleMath::Quaternion::Identity;
            //updateQuat *= deployDownQuat;
            updateQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rotMat);
            updateQuat *= deployDownQuat;
            //mirv.alignmentQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rotMat);

            mirv.alignmentQuat *= updateQuat;
            mirv.up = DirectX::SimpleMath::Vector3::Transform(mirv.up, updateQuat);
            mirv.right = DirectX::SimpleMath::Vector3::Transform(mirv.right, updateQuat);
            mirv.forward = DirectX::SimpleMath::Vector3::Transform(mirv.forward, updateQuat);

            mirv.q.velocity = mirv.forward * updateVelocity.Length();
            if (j == 0)
            {
                //m_newProjectilePushVec.push_back(mirv);
            }

            m_newProjectilePushVec.push_back(mirv);
            yawVal -= columnSpacingAngle;

        }
        //yawVal = yawOrg;
        columnSpacingAngle += Utility::ToRadians(0.0f);// *static_cast<float>(i + 1);
        yawVal = (columnSpacingAngle * (static_cast<float>(columnCount) * 0.5f) - (columnSpacingAngle * 0.5f));// -((static_cast<float>(i) * Utility::ToRadians(5.0f)) * 0.5f);
        pitchVal -= rowSpacingAngle;
        deployDownQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(firedProjectile.right, deployDownAngle + (deployDownMod * static_cast<float>(i + 1)));
    }

    aProjectile.isDeleteTrue = true;
}

void FireControl::DetonateAllMissiles()
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        m_missileVec[i].guidance.isSelfDestructTrue = true;
    }
}

void FireControl::DrawExplosions(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix0, aView, aProj, m_explosionStruct.explosionVec[i].explosionCurrentColor);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix1, aView, aProj, m_explosionStruct.explosionVec[i].color1);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix2, aView, aProj, m_explosionStruct.explosionVec[i].color2);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix3, aView, aProj, m_explosionStruct.explosionVec[i].color3);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix4, aView, aProj, m_explosionStruct.explosionVec[i].color4);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix5, aView, aProj, m_explosionStruct.explosionVec[i].color5);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix6, aView, aProj, m_explosionStruct.explosionVec[i].color6);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix7, aView, aProj, m_explosionStruct.explosionVec[i].color7);
        m_explosionStruct.explosionShape->Draw(m_explosionStruct.explosionVec[i].explosionMatrix8, aView, aProj, m_explosionStruct.explosionVec[i].color8);
    }
}

void FireControl::DrawExplosions2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        aEffect->EnableDefaultLighting();

        const float high = Utility::GetPi();
        const float low = -Utility::GetPi();
        float yaw = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        float pitch = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
        float roll = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

        const float t = 0.9999f;
        DirectX::SimpleMath::Vector3 lightDir0 = m_explosionStruct.explosionVec[i].lightDir0;
        DirectX::SimpleMath::Matrix lightRot = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
        lightDir0 = DirectX::SimpleMath::Vector3::Transform(lightDir0, lightRot);
        lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir0, lightDir0, t);
        m_explosionStruct.explosionVec[i].lightDir0 = lightDir0;

        DirectX::SimpleMath::Vector3 lightDir1 = m_explosionStruct.explosionVec[i].lightDir1;
        lightDir0 = DirectX::SimpleMath::Vector3::Transform(lightDir1, lightRot);
        lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir1, lightDir1, t);
        m_explosionStruct.explosionVec[i].lightDir1 = lightDir1;

        DirectX::SimpleMath::Vector3 lightDir2 = m_explosionStruct.explosionVec[i].lightDir2;
        lightDir2 = DirectX::SimpleMath::Vector3::Transform(lightDir2, lightRot);
        lightDir2 = DirectX::SimpleMath::Vector3::SmoothStep(m_explosionStruct.explosionVec[i].lightDir2, lightDir2, t);
        m_explosionStruct.explosionVec[i].lightDir2 = lightDir2;

        aEffect->SetLightDirection(0, m_explosionStruct.explosionVec[i].lightDir0);
        aEffect->SetLightDirection(1, m_explosionStruct.explosionVec[i].lightDir1);
        aEffect->SetLightDirection(2, m_explosionStruct.explosionVec[i].lightDir2);

        DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
        DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
        DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

        float durationMod = m_explosionStruct.explosionVec[i].currentDuration / m_explosionStruct.explosionVec[i].totalDuration;
        durationMod = 1.0f;
        DirectX::SimpleMath::Vector3 camPos = m_environment->GetCameraPos();
        DirectX::SimpleMath::Vector3 expPos = m_explosionStruct.explosionVec[i].position;
        DirectX::SimpleMath::Vector3 dirNormToExplosion = expPos - camPos;
        dirNormToExplosion.Normalize();
        float distanceToCenter = (expPos - camPos).Length();
        float distanceToFogStart = distanceToCenter - (m_explosionStruct.explosionVec[i].collisionSphere.Radius * durationMod);
        float distanceToFogEnd = distanceToCenter + (m_explosionStruct.explosionVec[i].collisionSphere.Radius * durationMod);
        //m_debugData->DebugPushUILineDecimalNumber("distanceToFogStart = ", distanceToFogStart, "");
        //m_debugData->DebugPushUILineDecimalNumber("distanceToFogEnd = ", distanceToFogEnd, "");

        if (m_isTestBoolTrue == true)
        {
            m_isTestBoolTrue = false;
        }
        else
        {
            m_isTestBoolTrue = true;
        }

        if (m_isTestBoolTrue == true)
        {
            aEffect->SetFogEnabled(true);
        }
        else
        {
            aEffect->SetFogEnabled(false);
        }
        aEffect->SetFogColor(DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 0.5f));
        aEffect->SetFogColor(DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        //aEffect->SetFogColor(m_explosionStruct.explosionVec[i].color1);
        aEffect->SetFogStart(distanceToFogStart - 5.0f);
        aEffect->SetFogEnd(distanceToFogEnd + 5.0f);

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix0);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color0);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix1);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color1);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix2);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color2);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix3);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color3);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix4);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color4);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix5);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color5);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix6);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color6);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix7);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color7);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetWorld(m_explosionStruct.explosionVec[i].explosionMatrix8);
        aEffect->SetColorAndAlpha(m_explosionStruct.explosionVec[i].color8);
        m_explosionStruct.explosionShape->Draw(aEffect.get(), aInputLayout.Get());
        //m_explosionStruct.explosionShape1->Draw(aEffect.get(), aInputLayout.Get());

        aEffect->SetFogEnabled(false);
    }
}

void FireControl::DrawFireControlObjects(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DrawExplosions(aView, aProj);
    if (m_muzzleFlash.isFlashActive == true)
    {
        DrawMuzzleFlash(aView, aProj);
    }
    DrawProjectiles(aView, aProj);
}

void FireControl::DrawFireControlObjects2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    DrawExplosions2(aView, aProj, aEffect, aInputLayout);

    if (m_muzzleFlash.isFlashActive == true)
    {
        DrawMuzzleFlash2(aView, aProj, aEffect, aInputLayout);
    }
    DrawProjectiles(aView, aProj);
    DrawMissiles(aView, aProj, aEffect, aInputLayout);
    DrawLaser(aView, aProj, aEffect, aInputLayout);
}

void FireControl::DrawMuzzleFlash(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    m_muzzleFlash.muzzleFlashConeShape2->Draw(m_muzzleFlash.worldTestMatrix, aView, aProj, m_muzzleFlash.currentColor);
    m_muzzleFlash.muzzleFlashConeShape->Draw(m_muzzleFlash.worldMuzzleFlashConeMatrix, aView, aProj, m_muzzleFlash.currentColor);
}

void FireControl::DrawMuzzleFlash2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    aEffect->EnableDefaultLighting();

    DirectX::SimpleMath::Vector3 weaponDir = -m_playerVehicle->GetWeaponDirection();
    DirectX::SimpleMath::Vector3 lightDir0 = weaponDir;
    DirectX::SimpleMath::Vector3 lightDir1 = weaponDir;
    DirectX::SimpleMath::Vector3 lightDir2 = weaponDir;

    const float flashDurationRatio = m_muzzleFlash.flashTimer / m_muzzleFlash.flashDuration;
    float coneSideAngle = atan((m_muzzleFlash.baseConeHeight / (m_muzzleFlash.baseConeDiameter * 0.5f)));
    const float coneSideAngleDegrees = Utility::ToDegrees(coneSideAngle);
    float lightAngle = flashDurationRatio * (Utility::GetPi() * -0.5f) + coneSideAngle;
    const float lightAngleDegrees = Utility::ToDegrees(lightAngle);

    const float low = 0.0f;
    const float high = DirectX::XM_2PI;
    float lightRotation = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
  
    if (m_muzzleFlash.isFlickerTrue == true)
    {
        Utility::GetDispersedLightDirectionsRotation(weaponDir, lightAngle, lightRotation, lightDir0, lightDir1, lightDir2);
    }
    else
    {
        Utility::GetDispersedLightDirectionsRotation(-weaponDir, lightAngle, lightRotation, lightDir0, lightDir1, lightDir2);
    }

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);

    if (m_muzzleFlash.isFlickerTrue == true)
    {
        aEffect->SetLightDirection(0, lightDir0);
        aEffect->SetLightDirection(1, lightDir1);
        aEffect->SetLightDirection(2, lightDir2);
    }
    else
    {
        aEffect->SetLightDirection(0, -lightDir0);
        aEffect->SetLightDirection(1, -lightDir1);
        aEffect->SetLightDirection(2, -lightDir2);
    }

    aEffect->SetBiasedVertexNormals(true);

    aEffect->SetWorld(m_muzzleFlash.worldTestMatrix);
    aEffect->SetColorAndAlpha(m_muzzleFlash.currentColor);
    m_muzzleFlash.muzzleFlashConeShape2->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetBiasedVertexNormals(false);
}

void FireControl::UpdateMuzzleFlash(MuzzleFlash& aMuzzleFlash, const double aTimeDelta)
{
    aMuzzleFlash.flashTimer += static_cast<float>(aTimeDelta);
    if (aMuzzleFlash.flashTimer >= aMuzzleFlash.flashDuration)
    {
        aMuzzleFlash.isFlashActive = false;
        m_muzzleFlash.flashTimer = 0.0f;
        m_muzzleFlash.sizeMod = 0.0f;
    }
    const float durationPercentage = aMuzzleFlash.flashTimer / aMuzzleFlash.flashDuration;
    const float inverseDurationPercentage = 1.0f - durationPercentage;
    if (durationPercentage <= 0.5f)
    {
        aMuzzleFlash.sizeMod += aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);
    }
    else
    {
        aMuzzleFlash.sizeMod -= aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);
    }
    //aMuzzleFlash.sizeMod += aMuzzleFlash.growthRate * static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector4 colorTest = aMuzzleFlash.startColor - aMuzzleFlash.endColor;
    aMuzzleFlash.currentColor = aMuzzleFlash.startColor - (colorTest * durationPercentage);
    //aMuzzleFlash.currentColor.w = 1.0f;
    //aMuzzleFlash.currentColor.x = 1.0f;

    DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());
    const float maxModSize = aMuzzleFlash.flashDuration * aMuzzleFlash.growthRate;

    float scale = aMuzzleFlash.sizeMod;
    if (aMuzzleFlash.isFlickerTrue == true)
    {
        scale *= aMuzzleFlash.flickerScale;
        aMuzzleFlash.isFlickerTrue = false;
    }
    else
    {
        aMuzzleFlash.isFlickerTrue = true;
    }

    const float testRot = Utility::WrapAngle(m_testTimer * 345.2345f);
    //const float scaleTransOffset = (scale * 1.0f) * 0.5f;
    const float scaleTransOffset = ((maxModSize * durationPercentage) * 1.0f) * 0.5f;
    DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));
    DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(-scale, scale, -scale));
    DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);
    aMuzzleFlash.worldTestMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.1f * -scale, 0.0f));
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.2f, 0.0f));
    aMuzzleFlash.worldTestMatrix *= scaleMat;
    aMuzzleFlash.worldTestMatrix *= scaleTransOffsetMat;
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetMuzzlePos());
    aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
    aMuzzleFlash.worldTestMatrix *= updateMat;

    aMuzzleFlash.worldMuzzleFlashConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= scaleMat;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= scaleTransOffsetMat;
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, (-0.5f * scale), 0.0f));
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

    //aMuzzleFlash.worldTestMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetMuzzlePos());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
    aMuzzleFlash.worldMuzzleFlashConeMatrix *= updateMat;
}

void FireControl::InitializeMuzzleFlashModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MuzzleFlash& aMuzzleFlash)
{
    aMuzzleFlash.muzzleFlashConeShape2 = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), aMuzzleFlash.baseConeDiameter, aMuzzleFlash.baseConeHeight);
    aMuzzleFlash.muzzleFlashConeShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), aMuzzleFlash.baseConeDiameter, 16Ui64);

    aMuzzleFlash.localMuzzleConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.worldMuzzleFlashConeMatrix = aMuzzleFlash.localMuzzleConeMatrix;

    aMuzzleFlash.worldTestMatrix = DirectX::SimpleMath::Matrix::Identity;
    aMuzzleFlash.localTestMatrix = DirectX::SimpleMath::Matrix::Identity;
}

void FireControl::DrawProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);

    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();

    DirectX::SimpleMath::Plane groundPlane;
    groundPlane.x = 0.0f;
    groundPlane.y = -1.0f;
    groundPlane.z = 0.0f;
    groundPlane.w = 0.5f;

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * 0.1f;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    planeTrans = planeTrans.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans);
    groundPlane.Normalize();
    DirectX::SimpleMath::Matrix shadowMat = DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    const float maxShadowRange = m_environment->GetMaxShadowCastRange();
    float shadowScale;
    float inverseShadowScale;

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat;
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            projMat = m_ammoExplosive.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            projMat = m_ammoMachineGun.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            projMat = m_ammoMirv.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            projMat = m_ammoShotgun.ammoModel.localProjectileMatrix;
        }
        else
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }

        const float altitude = m_projectileVec[i].q.position.y;
        if (altitude > maxShadowRange)
        {
            shadowScale = 0.0f;
            inverseShadowScale = 1.0f;
        }
        else
        {
            inverseShadowScale = altitude / maxShadowRange;
            shadowScale = 1.0f - inverseShadowScale;

            const float tol = 0.00001f;
            if (inverseShadowScale < tol)
            {
                inverseShadowScale = tol;
            }
        }
        DirectX::SimpleMath::Matrix shadowScaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(shadowScale, shadowScale, shadowScale));
        DirectX::SimpleMath::Matrix shadowDrawMat = projMat;

        DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_projectileVec[i].alignmentQuat);
        projMat *= alignMat;

        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);

        shadowDrawMat = shadowDrawMat;
        shadowDrawMat *= alignMat;
        shadowDrawMat *= inverseShadowScale;
        shadowDrawMat *= shadowScaleMat;
        shadowDrawMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);;
        shadowDrawMat *= shadowMat;

        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoCannon.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            m_ammoExplosive.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoExplosive.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            m_ammoMachineGun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoMachineGun.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            m_ammoMirv.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoMirv.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            m_ammoShotgun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoShotgun.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
        else
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
            m_ammoCannon.ammoModel.projectileShape->Draw(shadowDrawMat, aView, aProj, projectileColor);
        }
    }
}

void FireControl::DrawProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat;
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            projMat = m_ammoExplosive.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            projMat = m_ammoMachineGun.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            projMat = m_ammoMirv.ammoModel.localProjectileMatrix;
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            projMat = m_ammoShotgun.ammoModel.localProjectileMatrix;
        }
        else
        {
            projMat = m_ammoCannon.ammoModel.localProjectileMatrix;
        }

        DirectX::SimpleMath::Vector3 forward = m_projectileVec[i].q.velocity;
        if (forward.Length() < 0.00001f)
        {
            forward = DirectX::SimpleMath::Vector3::UnitX;
        }
        forward.Normalize();
        DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitY.Cross(forward);
        DirectX::SimpleMath::Vector3 up = right.Cross(forward);
        DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -right, up);
        projMat *= alignMat;

        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);

        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            m_ammoExplosive.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            m_ammoMachineGun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            m_ammoMirv.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        else if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            m_ammoShotgun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        else
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
    }
}

void FireControl::FireMissile(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoGuidedMissile.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        MissileData firedMissile;
        firedMissile.projectileData.ammoData = firedAmmo;
        firedMissile.projectileData.q.position = aLaunchPos;
        firedMissile.projectileData.q.velocity = (firedMissile.projectileData.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedMissile.projectileData.isCollisionTrue = false;
        firedMissile.projectileData.isDeleteTrue = false;
        firedMissile.projectileData.liveTimeTick = firedAmmo.tickDownCounter;
        firedMissile.projectileData.forward = aLaunchDirectionForward;
        firedMissile.projectileData.forward.Normalize();
        firedMissile.projectileData.up = aUp;
        firedMissile.projectileData.up.Normalize();
        firedMissile.projectileData.right = -firedMissile.projectileData.up.Cross(firedMissile.projectileData.forward);
        firedMissile.projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedMissile.projectileData.right, firedMissile.projectileData.up));
        firedMissile.projectileData.alignmentQuat.Normalize();
        firedMissile.projectileData.inverseAlignmentQuat = firedMissile.projectileData.alignmentQuat;
        firedMissile.projectileData.inverseAlignmentQuat.Inverse(firedMissile.projectileData.inverseAlignmentQuat);
        firedMissile.projectileData.inverseAlignmentQuat.Normalize();

        // collision data
        firedMissile.projectileData.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedMissile.projectileData.collisionData.collisionMagnitudeMod = firedMissile.projectileData.ammoData.impactModifier;
        firedMissile.projectileData.collisionData.collisionSphere.Center = firedMissile.projectileData.q.position;
        firedMissile.projectileData.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedMissile.projectileData.collisionData.velocity = firedMissile.projectileData.q.velocity;
        firedMissile.projectileData.collisionData.mass = firedAmmo.mass;
        firedMissile.projectileData.collisionData.isCollisionTrue = firedMissile.projectileData.isCollisionTrue;
        firedMissile.projectileData.time = 0.0f;

        firedMissile.guidance.heading = aLaunchDirectionForward;
        firedMissile.guidance.targetID = m_currentTargetID;
        if (m_currentTargetID != -1)
        {
            firedMissile.guidance.isTargetLocked = true;
        }
        else
        {
            firedMissile.guidance.isTargetLocked = false;
        }
        m_npcController->UpdateMissleGuidance(m_currentTargetID, firedMissile.guidance.targetPosition, firedMissile.guidance.targetVelocity);
        firedMissile.guidance.targetDistance = (aLaunchPos - firedMissile.guidance.targetPosition).Length();

        m_missileVec.push_back(firedMissile);
    }
}

void FireControl::FireMissile2(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    AmmoData firedAmmo = m_ammoGuidedMissile.ammoData;
    //AmmoData firedAmmo = m_ammoMissile.ammoData;

    m_isCoolDownActive = true;
    m_coolDownTimer = firedAmmo.cooldown;

    MissileData firedMissile;
    firedMissile.projectileData.ammoData = firedAmmo;
    firedMissile.projectileData.q.position = aLaunchPos;
    firedMissile.projectileData.q.velocity = (firedMissile.projectileData.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedMissile.projectileData.isCollisionTrue = false;
    firedMissile.projectileData.isDeleteTrue = false;
    firedMissile.projectileData.liveTimeTick = firedAmmo.tickDownCounter;
    firedMissile.projectileData.forward = aLaunchDirectionForward;
    firedMissile.projectileData.forward.Normalize();
    firedMissile.projectileData.up = aUp;
    firedMissile.projectileData.up.Normalize();
    firedMissile.projectileData.right = -firedMissile.projectileData.up.Cross(firedMissile.projectileData.forward);
    firedMissile.projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedMissile.projectileData.right, firedMissile.projectileData.up));
    firedMissile.projectileData.alignmentQuat.Normalize();
    firedMissile.projectileData.inverseAlignmentQuat = firedMissile.projectileData.alignmentQuat;
    firedMissile.projectileData.inverseAlignmentQuat.Inverse(firedMissile.projectileData.inverseAlignmentQuat);
    firedMissile.projectileData.inverseAlignmentQuat.Normalize();

    // collision data
    firedMissile.projectileData.collisionData.collisionDurationMod = firedAmmo.impactDurration;
    firedMissile.projectileData.collisionData.collisionMagnitudeMod = firedMissile.projectileData.ammoData.impactModifier;
    firedMissile.projectileData.collisionData.collisionSphere.Center = firedMissile.projectileData.q.position;
    firedMissile.projectileData.collisionData.collisionSphere.Radius = firedAmmo.radius;
    firedMissile.projectileData.collisionData.velocity = firedMissile.projectileData.q.velocity;
    firedMissile.projectileData.collisionData.mass = firedAmmo.mass;
    firedMissile.projectileData.collisionData.isCollisionTrue = firedMissile.projectileData.isCollisionTrue;
    firedMissile.projectileData.time = 0.0f;

    firedMissile.guidance.heading = aLaunchDirectionForward;
    firedMissile.guidance.targetID = m_currentTargetID;
    if (m_currentTargetID != -1)
    {
        firedMissile.guidance.isTargetLocked = true;
    }
    else
    {
        firedMissile.guidance.isTargetLocked = false;
    }
    m_npcController->UpdateMissleGuidance(m_currentTargetID, firedMissile.guidance.targetPosition, firedMissile.guidance.targetVelocity);
    firedMissile.guidance.targetDistance = (aLaunchPos - firedMissile.guidance.targetPosition).Length();

    m_missileVec.push_back(firedMissile);
}

void FireControl::FireProjectileCannon(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoCannon.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;
        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireDefaultProjectile(const AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    if (m_isCoolDownActive == false)
    {
        ProjectileData firedProjectile;
        AmmoData firedAmmo;
        if (aAmmoType == AmmoType::AMMOTYPE_CANNON)
        {
            firedAmmo = m_ammoCannon.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_MIRV)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = true;
        }
        else if (aAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            firedAmmo = m_ammoExplosive.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }
        else
        {
            // ToDo - add error handling, ammo type missing
            firedAmmo = m_ammoCannon.ammoData;
            firedProjectile.isMidAirDeployAvailable = false;
        }

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedAmmo.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoExplosive.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoExplosive.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        firedProjectile.isMidAirDeployAvailable = false;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileGuidedMissile(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoGuidedMissile.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;
        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileMachineGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoMachineGun.ammoData;

        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoMirv.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoMirv.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        firedProjectile.up = aUp;
        firedProjectile.up.Normalize();
        firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;

        firedProjectile.isMidAirDeployAvailable = true;
        firedProjectile.isFuseTriggered = false;

        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    if (m_isCoolDownActive == false)
    {
        AmmoData firedAmmo = m_ammoShotgun.ammoData;
        m_isCoolDownActive = true;
        m_coolDownTimer = firedAmmo.cooldown;
        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;

        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
        firedProjectile.time = 0.0f;
        firedProjectile.forward = aLaunchDirectionForward;
        firedProjectile.forward.Normalize();
        //firedProjectile.right = -firedProjectile.up.Cross(firedProjectile.forward);
        firedProjectile.right = aLaunchDirectionRight;
        firedProjectile.up = firedProjectile.right.Cross(firedProjectile.forward);
        firedProjectile.up.Normalize();
        firedProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -firedProjectile.right, firedProjectile.up));
        firedProjectile.alignmentQuat.Normalize();
        firedProjectile.inverseAlignmentQuat = firedProjectile.alignmentQuat;
        firedProjectile.inverseAlignmentQuat.Inverse(firedProjectile.inverseAlignmentQuat);
        firedProjectile.inverseAlignmentQuat.Normalize();

        // collision data
        firedProjectile.collisionData.collisionDurationMod = firedAmmo.impactDurration;
        firedProjectile.collisionData.collisionMagnitudeMod = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.collisionSphere.Center = firedProjectile.q.position;
        firedProjectile.collisionData.collisionSphere.Radius = firedAmmo.radius;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        DirectX::SimpleMath::Vector3 up = aLaunchDirectionForward;
        up = aLaunchDirectionRight.Cross(aLaunchDirectionForward);

        const unsigned int shotCount = 10;
        const float shotRotationOffset = DirectX::XM_2PI / static_cast<float>(shotCount);
        float shotRotation = 0.0f;
        const float chokeAngle = Utility::ToRadians(5.0f);
        DirectX::SimpleMath::Vector3 shotCenterAim = m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward;
        for (unsigned int i = 0; i < shotCount; ++i)
        {
            const float chokeOffset = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));

            DirectX::SimpleMath::Matrix barrelRotationMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionForward, shotRotation);
            DirectX::SimpleMath::Matrix chokeVariationMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, chokeOffset);

            firedProjectile.q.velocity = shotCenterAim;
            firedProjectile.q.velocity = DirectX::SimpleMath::Vector3::Transform(firedProjectile.q.velocity, chokeVariationMat);
            firedProjectile.q.velocity = DirectX::SimpleMath::Vector3::Transform(firedProjectile.q.velocity, barrelRotationMat);
            firedProjectile.q.velocity += aLauncherVelocity;
            m_projectileVec.push_back(firedProjectile);

            shotRotation += shotRotationOffset;
        }
    }
}

void FireControl::FireSelectedAmmo(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp)
{
    if (m_isCoolDownActive == false)
    {
        ActivateMuzzleFlash(m_currentAmmoType);
        if (m_currentAmmoType == AmmoType::AMMOTYPE_CANNON)
        {
            FireProjectileCannon(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            FireProjectileExplosive(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            FireProjectileMachineGun(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
        {
            FireProjectileMirv(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            //DirectX::SimpleMath::Vector3 up = m_playerVehicle->GetVehicleUp();
            DirectX::SimpleMath::Vector3 up = aUp;
            DirectX::SimpleMath::Vector3 right = -up.Cross(aLaunchDirectionForward);
            FireProjectileShotGun(aLaunchPos, aLaunchDirectionForward, right, aLauncherVelocity);
        }
        else if (m_currentAmmoType == AmmoType::AMMOTYPE_GUIDEDMISSILE)
        {
            if (m_currentTargetID != -1)
            {
                //FireMissile(aLaunchPos, aLaunchDirectionForward, aLauncherVelocity, aUp);
                FireMissile2(m_playerVehicle->GetMissleTubePosLeft(), m_playerVehicle->GetMissleTubeDirLeft(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpLeft());
                //FireMissile2(m_playerVehicle->GetMissleTubePosRight(), m_playerVehicle->GetMissleTubeDirRight(), aLauncherVelocity, m_playerVehicle->GetMissleTubeUpRight());

                m_isCoolDownActive = true;
                m_coolDownTimer = m_ammoGuidedMissile.ammoData.cooldown;
            }
        }
        if (m_isTestBoolTrue == true)
        {
            m_isTestBoolTrue = false;
        }
        else
        {
            m_isTestBoolTrue = true;
        }
    }
}

Utility::ImpulseForce FireControl::GetRecoilImpulseForce(DirectX::SimpleMath::Vector3 aDirectionNorm)
{
    Utility::ImpulseForce recoilImpulseForce;

    if (m_currentAmmoType == AmmoType::AMMOTYPE_SHOTGUN)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_EXPLOSIVE)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.2f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MACHINEGUN)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 10000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else if (m_currentAmmoType == AmmoType::AMMOTYPE_MIRV)
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.25f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.1f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }
    else  // Default to cannon, AmmoType::AMMOTYPE_CANNON
    {
        recoilImpulseForce.impulseType = Utility::ImpulseType::IMPULSETYPE_BELLCURVE;
        recoilImpulseForce.currentTime = 0.0f;
        recoilImpulseForce.totalTime = 0.1f;
        recoilImpulseForce.currentMagnitude = 0.0f;
        recoilImpulseForce.currentTorqueMagnitude = 0.0f;
        recoilImpulseForce.torqueForceMod = 0.6f;
        recoilImpulseForce.maxMagnitude = 50000.0f;
        recoilImpulseForce.directionNorm = aDirectionNorm;
        recoilImpulseForce.directionNorm.Normalize();
        recoilImpulseForce.isActive = true;
    }

    return recoilImpulseForce;
}

void FireControl::InitializeAmmoCannon(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_CANNON;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.8f;
    //aAmmo.ammoData.dragCoefficient = 0.82f;
    aAmmo.ammoData.dragCoefficient = 0.042f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 395.0f;
    aAmmo.ammoData.length = 9.0f;
    aAmmo.ammoData.mass = 10.0f;
    aAmmo.ammoData.radius = 0.2f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoExplosive(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_EXPLOSIVE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.3f;
    aAmmo.ammoData.dragCoefficient = 0.2f;
    aAmmo.ammoData.impactDurration = 0.3f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 215.0f;
    aAmmo.ammoData.length = 25.0f;
    aAmmo.ammoData.mass = 55.0f;
    aAmmo.ammoData.radius = 0.22f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoGuidedMissile(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.9f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 25.0f;
    aAmmo.ammoData.length = 5.0f;
    aAmmo.ammoData.mass = m_missileConsts.mass;
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = true;
}

void FireControl::InitializeAmmoMissile(MissileStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.9f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 25.0f;
    aAmmo.ammoData.length = 4.0f;
    aAmmo.ammoData.mass = m_missileConsts.mass;
    aAmmo.ammoData.radius = 0.75f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = true;
}

void FireControl::InitializeAmmoMachineGun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MACHINEGUN;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.09f;
    aAmmo.ammoData.dragCoefficient = 0.25f;
    aAmmo.ammoData.impactDurration = 0.7f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 465.0f;
    aAmmo.ammoData.length = 0.4f;
    aAmmo.ammoData.mass = 1.4f;
    aAmmo.ammoData.radius = 0.14f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoMirv(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MIRV;
    aAmmo.ammoData.baseDamage = 1.0f;
    //aAmmo.ammoData.cooldown = 1.9f;
    aAmmo.ammoData.cooldown = 1.0f;
    //aAmmo.ammoData.dragCoefficient = 0.19f;
    aAmmo.ammoData.dragCoefficient = 0.5f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    //aAmmo.ammoData.launchVelocity = 165.0f;
    //aAmmo.ammoData.launchVelocity = 105.0f;
    aAmmo.ammoData.launchVelocity = 195.0f;
    aAmmo.ammoData.length = 24.5f;
    aAmmo.ammoData.mass = 65.0f;
    aAmmo.ammoData.radius = 0.22f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeAmmoShotgun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_SHOTGUN;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 0.5f;
    aAmmo.ammoData.dragCoefficient = 0.7f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 395.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 1.6f;
    aAmmo.ammoData.radius = 0.1f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.isGuided = false;
}

void FireControl::InitializeExplosionData(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, ExplosionData& aExplosionData)
{
    aExplosionData.explosionType = ExplosionType::EXPLOSIONTYPE_NONVEHICLE;
    aExplosionData.explosionStartColor = DirectX::Colors::DarkRed;
    aExplosionData.explosionEndColor = DirectX::Colors::OrangeRed;
    aExplosionData.explosionCurrentColor = aExplosionData.explosionStartColor;
    aExplosionData.color0 = aExplosionData.explosionStartColor;
    aExplosionData.color1 = aExplosionData.explosionStartColor;
    aExplosionData.color2 = aExplosionData.explosionStartColor;
    aExplosionData.color3 = aExplosionData.explosionStartColor;
    aExplosionData.color4 = aExplosionData.explosionStartColor;
    aExplosionData.color5 = aExplosionData.explosionStartColor;
    aExplosionData.color6 = aExplosionData.explosionStartColor;
    aExplosionData.color7 = aExplosionData.explosionStartColor;
    aExplosionData.color8 = aExplosionData.explosionStartColor;
    aExplosionData.initialRadius = m_ammoExplosive.ammoData.radius;
    aExplosionData.currentRadius = aExplosionData.initialRadius;
    aExplosionData.currentDuration = 0.0f;
    aExplosionData.totalDuration = 3.0f;
    aExplosionData.maxRadius = 15.f;
    aExplosionData.position = DirectX::SimpleMath::Vector3::Zero;
    m_explosionStruct.explosionShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), -1.0f);
    m_explosionStruct.explosionShape1 = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), -0.75f);
    aExplosionData.localExplosionMatrix = DirectX::SimpleMath::Matrix::Identity;
    aExplosionData.explosionMatrix0 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix1 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix2 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix3 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix4 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix5 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix6 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix7 = aExplosionData.localExplosionMatrix;
    aExplosionData.explosionMatrix8 = aExplosionData.localExplosionMatrix;
    aExplosionData.rotationVariationMatrix = DirectX::SimpleMath::Matrix::Identity;
    aExplosionData.collisionSphere.Center = aExplosionData.position;
    aExplosionData.collisionSphere.Radius = aExplosionData.initialRadius;
    aExplosionData.isLifeTimeExpired = false;

    //m_explosionStruct.maxExplosionForce = 10000000.0f;
    //m_explosionStruct.maxExplosionForce = 100.0f;
    //m_explosionStruct.maxExplosionForce = 1000000.0f;
    //m_explosionStruct.maxExplosionForce = 100000.0f;
    m_explosionStruct.maxExplosionForce = m_maxExplosiveForce;
    m_explosionStruct.maxExplosionImpactRadius = aExplosionData.maxRadius * 2.0f;

    m_explosionStruct.explosionVec.clear();
}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection,
    Environment const* aEnvironment, std::shared_ptr<Vehicle> aVehicle)
{
    m_playerVehicle = aVehicle;
    m_explosionStruct.explosionToPushVec.clear();
    m_projectileVec.clear();
    m_newProjectilePushVec.clear();
    m_missileVec.clear();
    m_environment = aEnvironment;
    //m_currentAmmoType = AmmoType::AMMOTYPE_CANNON;
    m_currentAmmoType = AmmoType::AMMOTYPE_MIRV;
    m_currentAmmoType = AmmoType::AMMOTYPE_GUIDEDMISSILE;

    ////////////
    // inertia tensors
    m_missileInertiaTensorLocal = DirectX::SimpleMath::Matrix::Identity;

    const float xExtent = m_missileDimensions.x;
    const float yExtent = m_missileDimensions.y;
    const float zExtent = m_missileDimensions.z;
    const float mass = m_missileMass;

    // cuboid
    //m_missileInertiaTensorLocal._11 = (1.0f / 12.0f) * (mass) * ((yExtent * yExtent) + (zExtent * zExtent));
    //m_missileInertiaTensorLocal._22 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (zExtent * zExtent));
    //m_missileInertiaTensorLocal._33 = (1.0f / 12.0f) * (mass) * ((xExtent * xExtent) + (yExtent * yExtent));

    // cylinder
    const float radius = m_missileDimensions.z;
    const float height = m_missileDimensions.x;
    m_missileInertiaTensorLocal._11 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._22 = ((1.0f / 12.0f) * (mass) * (height * height)) + ((1.0f / 4.0f) * (mass) * (radius * radius));
    m_missileInertiaTensorLocal._33 = (1.0f / 2.0f) * (mass) * (radius * radius);

    m_missileInverseInertiaTensorLocal = m_missileInertiaTensorLocal;
    m_missileInverseInertiaTensorLocal = m_missileInverseInertiaTensorLocal.Invert();
    ////////////

    InitializeAmmoCannon(m_ammoCannon);
    InitializeAmmoExplosive(m_ammoExplosive);
    InitializeAmmoGuidedMissile(m_ammoGuidedMissile);
    InitializeAmmoMachineGun(m_ammoMachineGun);
    InitializeAmmoMirv(m_ammoMirv);
    InitializeAmmoShotgun(m_ammoShotgun);

    InitializeAmmoMissile(m_ammoMissile);
    InitializeProjectileModelMissile(aContext, m_ammoMissile);

    InitializeExplosionData(aContext, m_explosionStruct.explosionRefData);
    InitializeMuzzleFlashModel(aContext, m_muzzleFlash);
    InitializeProjectileModelCannon(aContext, m_ammoCannon);
    InitializeProjectileModelExplosive(aContext, m_ammoExplosive);
    InitializeProjectileModelGuidedMissile(aContext, m_ammoGuidedMissile);
    InitializeProjectileModelMachineGun(aContext, m_ammoMachineGun);
    InitializeProjectileModelMirv(aContext, m_ammoMirv);
    InitializeProjectileModelShotgun(aContext, m_ammoShotgun);
    InitializeLaserModel(aContext, m_playerLaser);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //const float ammoSize = 0.3f;
    //const float ammoLength = 10.0f;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, ammoLength, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelGuidedMissile(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, ammoLength, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMachineGun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoDiameter = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), ammoDiameter);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMirv(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateDodecahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, ammoLength, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMissile(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MissileStruct& aAmmo)
{
    const float ammoDiameter = aAmmo.ammoData.radius * 2.0f;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.modelData.mainBodyShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoDiameter);
    //aAmmo.modelData.mainBodyShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), 4.0f, 1.5f);
    aAmmo.modelData.worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localBodyMatrix = aAmmo.modelData.worldBodyMatrix;

    // nose cone
    aAmmo.modelData.noseConeShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoDiameter);
    aAmmo.modelData.localNoseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 noseConeTranslation = DirectX::SimpleMath::Vector3(ammoLength * 0.5f, 0.0f, 0.0f);
    aAmmo.modelData.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    aAmmo.modelData.worldNoseConeMatrix = aAmmo.modelData.localNoseConeMatrix;

    // rocket plume
    const float plumeLength = aAmmo.ammoData.length * 0.5f;
    aAmmo.modelData.rocketPlumeShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoDiameter, plumeLength);

    //DirectX::SimpleMath::Vector3 plumeTranslation = DirectX::SimpleMath::Vector3(-2.0f - (plumeLength * 0.5f), 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 plumeTranslation = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);
    aAmmo.modelData.plumeTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(plumeTranslation);
    DirectX::SimpleMath::Vector3 rotationCenterTranslation = DirectX::SimpleMath::Vector3((-plumeLength * 0.5f), 0.0f, 0.0f);
    aAmmo.modelData.localPlumeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localPlumeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aAmmo.modelData.localPlumeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rotationCenterTranslation);
    //aAmmo.modelData.localPlumeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(plumeTranslation);
    aAmmo.modelData.worldPlumeMatrix = aAmmo.modelData.localPlumeMatrix;

    // tail fins
    const float tailFinLength = aAmmo.ammoData.length * 0.3f;
    const float tailFinWidth = tailFinLength * 0.2f;
    //const float tailFinLength = 4.0f;
    //const float tailFinWidth = 2.0f;
    //const DirectX::SimpleMath::Vector3 tailFinDimensions = DirectX::SimpleMath::Vector3(tailFinLength, tailFinWidth * 0.3f, tailFinWidth);
    const DirectX::SimpleMath::Vector3 tailFinDimensions = DirectX::SimpleMath::Vector3(tailFinLength, tailFinWidth, tailFinWidth * 0.3f);
    //DirectX::SimpleMath::Vector3 tailFinDimensions = DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f);
    aAmmo.modelData.tailFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), tailFinDimensions);

    DirectX::SimpleMath::Vector3 tailFinTranslation1 = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);
    aAmmo.modelData.tailFinTranslation1 = DirectX::SimpleMath::Matrix::CreateTranslation(tailFinTranslation1);
    //DirectX::SimpleMath::Vector3 rotationCenterTranslation = DirectX::SimpleMath::Vector3((-plumeLength * 0.5f), 0.0f, 0.0f);
    aAmmo.modelData.localTailFinMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.localTailFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    //aAmmo.modelData.localTailFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(rotationCenterTranslation);
    //aAmmo.modelData.worldPlumeMatrix = aAmmo.modelData.localTailFinMatrix1;
    //DirectX::SimpleMath::Vector3 plumeTranslation = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);
    aAmmo.modelData.tailFinTranslation1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-ammoLength * 0.4f, aAmmo.ammoData.radius, 0.0f));

    // tail fin 2
    aAmmo.modelData.localTailFinMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.localTailFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.tailFinTranslation2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-ammoLength * 0.4f, -aAmmo.ammoData.radius, 0.0f));

    // tail fin 3
    aAmmo.modelData.localTailFinMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.localTailFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aAmmo.modelData.localTailFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.tailFinTranslation3 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-ammoLength * 0.4f, 0.0f, aAmmo.ammoData.radius));

    // tail fin 4
    aAmmo.modelData.localTailFinMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localTailFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(tailFinLength * 0.5f, 0.0f, 0.0f));
    aAmmo.modelData.localTailFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localTailFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.tailFinTranslation4 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-ammoLength * 0.4f, 0.0f, -aAmmo.ammoData.radius));

    // wing fins
    const float wingFinLength = aAmmo.ammoData.length * 0.3f;
    const float wingFinWidth = wingFinLength * 0.2f;
    const DirectX::SimpleMath::Vector3 wingFinDimensions = DirectX::SimpleMath::Vector3(wingFinLength, wingFinWidth, wingFinWidth * 0.3f);
    aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingFinDimensions);
    aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateTetrahedron(aContext.Get(), 2.0f);
    //aAmmo.modelData.wingFinShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), 2.0f, 1.0f);

    DirectX::SimpleMath::Matrix wingScale = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(10.0f, 1.0f, 1.0f));
    DirectX::SimpleMath::Matrix wingScale2 = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.3f, 0.05f, 1.0f));
    //DirectX::SimpleMath::Matrix wingScale2 = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    const float posOffset = -0.6f;
    const float wingOffset = (2.0f * 1.1f) * 0.5f;
    const DirectX::SimpleMath::Matrix localWingTrans = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -0.5f, 0.0f));
    DirectX::SimpleMath::Vector3 wingFinTranslation1 = DirectX::SimpleMath::Vector3(-2.0f, 0.0f, 0.0f);
    aAmmo.modelData.wingTranslation1 = DirectX::SimpleMath::Matrix::CreateTranslation(wingFinTranslation1);
    aAmmo.modelData.localWingFinMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix1 *= wingScale2;
    aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(0.0f));
    //aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    //aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-180.0f));
    aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(wingFinLength * 0.5f, 0.0f, 0.0f));
    //aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -0.5f, 0.0f));
    //aAmmo.modelData.localWingFinMatrix1 *= localWingTrans;
    //aAmmo.modelData.localWingFinMatrix1 *= wingScale2;
    //aAmmo.modelData.localWingFinMatrix1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aAmmo.modelData.wingTranslation1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, aAmmo.ammoData.radius, 0.0f));
    const float mainWingLongPos = 0.2f;
    aAmmo.modelData.wingTranslation1 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(mainWingLongPos, aAmmo.ammoData.radius + posOffset, 0.0f));

    // wing fin 2
    aAmmo.modelData.localWingFinMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix2 *= wingScale2;
    //aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    //aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-180.0f));
    aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-180.0f));
    aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(wingFinLength * 0.5f, 0.0f, 0.0f));
    //aAmmo.modelData.localWingFinMatrix2 *= localWingTrans;
    //aAmmo.modelData.localWingFinMatrix2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.modelData.wingTranslation2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -aAmmo.ammoData.radius, 0.0f));
    aAmmo.modelData.wingTranslation2 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(mainWingLongPos, -aAmmo.ammoData.radius - posOffset, 0.0f));

    // wing fin 3
    aAmmo.modelData.localWingFinMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix3 *= wingScale2;
    aAmmo.modelData.localWingFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
    aAmmo.modelData.localWingFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(wingFinLength * 0.5f, 0.0f, 0.0f));
    //aAmmo.modelData.localWingFinMatrix3 *= localWingTrans;
    //aAmmo.modelData.localWingFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    //aAmmo.modelData.localWingFinMatrix3 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.wingTranslation3 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, aAmmo.ammoData.radius));
    aAmmo.modelData.wingTranslation3 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(mainWingLongPos, 0.0f, -aAmmo.ammoData.radius - posOffset));

    // wing fin 4
    aAmmo.modelData.localWingFinMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.localWingFinMatrix4 *= wingScale2;
    aAmmo.modelData.localWingFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    aAmmo.modelData.localWingFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(wingFinLength * 0.5f, 0.0f, 0.0f));
    //aAmmo.modelData.localWingFinMatrix4 *= localWingTrans;
    //aAmmo.modelData.localWingFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    //aAmmo.modelData.localWingFinMatrix4 *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    aAmmo.modelData.wingTranslation4 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -aAmmo.ammoData.radius));
    aAmmo.modelData.wingTranslation4 = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(mainWingLongPos, 0.0f, aAmmo.ammoData.radius + posOffset));

    DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 5.0));
    /*
    aAmmo.modelData.wingTranslation1 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.wingTranslation2 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.wingTranslation3 = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.modelData.wingTranslation4 = DirectX::SimpleMath::Matrix::Identity;

    aAmmo.modelData.wingTranslation1 = testMat;
    aAmmo.modelData.wingTranslation2 = testMat;
    aAmmo.modelData.wingTranslation3 = testMat;
    aAmmo.modelData.wingTranslation4 = testMat;
    */
}

void FireControl::InitializeLaserModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, LaserModel& aLazerModel)
{
    const float length = 1.0f;
    const float diameter = 0.2f;
    aLazerModel.laserShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), length, diameter);
    aLazerModel.laserShape2 = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), length, -diameter);
    aLazerModel.localBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 transMat = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    aLazerModel.translationMatrix = DirectX::SimpleMath::Matrix::Identity;
    aLazerModel.localBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aLazerModel.localBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(transMat);
    aLazerModel.worldBodyMatrix = aLazerModel.localBodyMatrix;
}


void FireControl::DrawMissiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    const DirectX::SimpleMath::Vector4 projectileColor = m_ammoMissile.modelData.bodyColor;
    const DirectX::SimpleMath::Vector4 plumeColor = m_ammoMissile.modelData.plumeColor;
    const DirectX::SimpleMath::Vector4 testColor = m_ammoMissile.modelData.testColor;
    const DirectX::SimpleMath::Vector4 testColor2 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f);
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat = m_ammoMissile.modelData.localBodyMatrix;
        /*
        DirectX::SimpleMath::Vector3 forward = m_missileVec[i].projectileData.q.velocity;
        if (forward.Length() < 0.00001f)
        {
            forward = DirectX::SimpleMath::Vector3::UnitX;
        }
        forward.Normalize();
        DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitY.Cross(forward);
        DirectX::SimpleMath::Vector3 up = right.Cross(forward);
        DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -right, up);
        projMat *= alignMat;
        */
        projMat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);

        m_ammoMissile.modelData.mainBodyShape->Draw(projMat, aView, aProj, projectileColor);

        // nose cone
        DirectX::SimpleMath::Matrix noseConeProjMat = m_ammoMissile.modelData.localNoseConeMatrix;
        noseConeProjMat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        noseConeProjMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.noseConeShape->Draw(noseConeProjMat, aView, aProj, projectileColor);

        //float finDeployAngle = m_missileVec[i].projectileData.time;
        float finDeployAngle = Utility::ToRadians(-90.0f);
        float wingFinDeployAngle = Utility::ToRadians(90.0f);
        if (m_missileVec[i].guidance.isRocketFired == true)
        {
            DirectX::SimpleMath::Quaternion thrustRotQuat = DirectX::SimpleMath::Quaternion::FromToRotation(-m_missileVec[i].projectileData.forward, -m_missileVec[i].guidance.heading);
            thrustRotQuat.Inverse(thrustRotQuat);
            DirectX::SimpleMath::Matrix thrustRotMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(thrustRotQuat);
            projMat = m_ammoMissile.modelData.localPlumeMatrix;

            if (m_missileVec[i].projectileData.time <= m_missileConsts.rocketFireDelay + m_missileConsts.rocketFireFullTime)
            {
                const float rocketPlumeScale = (m_missileVec[i].projectileData.time - m_missileConsts.rocketFireDelay) / (m_missileConsts.rocketFireFullTime);
                projMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(rocketPlumeScale, 1.0f, 1.0f));
            }

            if (m_missileVec[i].guidance.isRocketPlumeFlickerTrue == true)
            {
                const float plumeflickerScaleLength = 0.5f;
                const float plumeflickerScaleWidth = 0.9f;
                projMat *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(plumeflickerScaleLength, plumeflickerScaleWidth, plumeflickerScaleWidth));
            }

            if (m_missileVec[i].guidance.isRocketPlumeFlickerTrue == true)
            {
                m_missileVec[i].guidance.isRocketPlumeFlickerTrue = false;
            }
            else
            {
                m_missileVec[i].guidance.isRocketPlumeFlickerTrue = true;
            }

            //projMat *= alignMat;
            projMat *= thrustRotMat;
            projMat *= m_ammoMissile.modelData.plumeTranslation;
            projMat *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);

            projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
            m_ammoMissile.modelData.rocketPlumeShape->Draw(projMat, aView, aProj, plumeColor);
            /*
            if (m_missileVec[i].projectileData.time <= m_missileConsts.rocketFireDelay + m_missileConsts.finDeployTime)
            {
                float finDeployPercent = (m_missileVec[i].projectileData.time - m_missileConsts.rocketFireDelay) / (m_missileConsts.finDeployTime);
                finDeployAngle += m_ammoMissile.modelData.tailFinDeployAngleMax * finDeployPercent;
                wingFinDeployAngle += m_ammoMissile.modelData.wingFinDeployAngleMax * finDeployPercent;
            }
            else
            {
                finDeployAngle += m_ammoMissile.modelData.tailFinDeployAngleMax;
                wingFinDeployAngle += m_ammoMissile.modelData.wingFinDeployAngleMax;
            }
            */
        }

        // fin steering
        DirectX::SimpleMath::Vector3 localizedHeading = m_missileVec[i].guidance.heading;
        localizedHeading = DirectX::SimpleMath::Vector3::Transform(localizedHeading, m_missileVec[i].projectileData.inverseAlignmentQuat);

        /*
        localizedHeading = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
        localizedHeading.Normalize();

        m_testVec.Normalize();
        localizedHeading = m_testVec;
        localizedHeading = DirectX::SimpleMath::Vector3::Transform(localizedHeading, DirectX::SimpleMath::Matrix::CreateRotationX(0.005f));
        m_testVec = localizedHeading;

        float testVar = cos(m_testTimer);
        localizedHeading = DirectX::SimpleMath::Vector3(1.0f, testVar, 0.0f);
        //localizedHeading = DirectX::SimpleMath::Vector3(1.0f, 0.0f, testVar);
        localizedHeading.Normalize();
        */

        DirectX::SimpleMath::Vector3 vertVec = localizedHeading;
        vertVec.z = 0.0f;
        vertVec.Normalize();
        float vertAng = Utility::GetAngleBetweenVectors(vertVec, DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 vertCross = vertVec.Cross(DirectX::SimpleMath::Vector3::UnitX);

        if (vertCross.z < 0.0f)
        {
            vertAng *= -1.0f;
        }

        DirectX::SimpleMath::Vector3 horzVec = localizedHeading;
        horzVec.y = 0.0f;
        horzVec.Normalize();
        float horzAng = Utility::GetAngleBetweenVectors(horzVec, DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 horzCross = horzVec.Cross(DirectX::SimpleMath::Vector3::UnitX);

        if (horzCross.y < 0.0f)
        {
            horzAng *= -1.0f;
        }

        DirectX::SimpleMath::Vector3 testHeadingWorld = localizedHeading;
        testHeadingWorld = DirectX::SimpleMath::Vector3::Transform(testHeadingWorld, m_missileVec[i].projectileData.alignmentQuat);

        if (m_missileVec[i].guidance.isFinsDeployEnd == false)
        {
            if (m_missileVec[i].projectileData.time >= m_missileConsts.finDeployDelay)
            {
                float finDeployPercent = (m_missileVec[i].projectileData.time - m_missileConsts.finDeployDelay) / (m_missileConsts.finDeployTime);
                finDeployAngle += m_ammoMissile.modelData.tailFinDeployAngleMax * finDeployPercent;
                wingFinDeployAngle += m_ammoMissile.modelData.wingFinDeployAngleMax * finDeployPercent;
            }
        }
        else
        {
            finDeployAngle += m_ammoMissile.modelData.tailFinDeployAngleMax;
            wingFinDeployAngle += m_ammoMissile.modelData.wingFinDeployAngleMax;
        }
        //horzAng = Utility::ToRadians(90.0f);
        //vertAng = Utility::ToRadians(90.0f);
        m_testRot += m_testTimer;
        //m_testTimer = 0.0f;
        //wingFinDeployAngle = m_testRot;
        //wingFinDeployAngle = Utility::ToRadians(-0.0f);
        float tailFinTrim = m_testRot;

        m_missileVec[i].guidance.finAngle1 = (m_missileVec[i].guidance.finAngle1 + horzAng) * 0.5f;
        m_missileVec[i].guidance.finAngle2 = (m_missileVec[i].guidance.finAngle2 - horzAng) * 0.5f;
        m_missileVec[i].guidance.finAngle3 = (m_missileVec[i].guidance.finAngle3 + vertAng) * 0.5f;
        m_missileVec[i].guidance.finAngle4 = (m_missileVec[i].guidance.finAngle4 - vertAng) * 0.5f;

        // tail fins
        DirectX::SimpleMath::Matrix tailFinProjMat1 = DirectX::SimpleMath::Matrix::Identity;
        //tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateRotationX(tailFinTrim);
        //tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateRotationX(horzAng);
        tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_missileVec[i].guidance.finAngle1);
        tailFinProjMat1 *= m_ammoMissile.modelData.localTailFinMatrix1;
        tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(finDeployAngle);
        tailFinProjMat1 *= m_ammoMissile.modelData.tailFinTranslation1;
        tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        tailFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.tailFinShape->Draw(tailFinProjMat1, aView, aProj, projectileColor);

        // tail fin 2
        DirectX::SimpleMath::Matrix tailFinProjMat2 = DirectX::SimpleMath::Matrix::Identity;
        //tailFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateRotationX(-horzAng);
        tailFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_missileVec[i].guidance.finAngle2);
        tailFinProjMat2 *= m_ammoMissile.modelData.localTailFinMatrix2;
        tailFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(-finDeployAngle);
        tailFinProjMat2 *= m_ammoMissile.modelData.tailFinTranslation2;
        tailFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        tailFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.tailFinShape->Draw(tailFinProjMat2, aView, aProj, projectileColor);

        // tail fin 3
        DirectX::SimpleMath::Matrix tailFinProjMat3 = DirectX::SimpleMath::Matrix::Identity;
        //tailFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateRotationX(vertAng);
        tailFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_missileVec[i].guidance.finAngle3);
        tailFinProjMat3 *= m_ammoMissile.modelData.localTailFinMatrix3;
        tailFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateRotationY(-finDeployAngle);
        tailFinProjMat3 *= m_ammoMissile.modelData.tailFinTranslation3;
        tailFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        tailFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.tailFinShape->Draw(tailFinProjMat3, aView, aProj, projectileColor);

        // tail fin 4
        DirectX::SimpleMath::Matrix tailFinProjMat4 = DirectX::SimpleMath::Matrix::Identity;
        //tailFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateRotationX(-vertAng);
        tailFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateRotationX(m_missileVec[i].guidance.finAngle4);
        tailFinProjMat4 *= m_ammoMissile.modelData.localTailFinMatrix4;
        tailFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateRotationY(finDeployAngle);
        tailFinProjMat4 *= m_ammoMissile.modelData.tailFinTranslation4;
        tailFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        tailFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.tailFinShape->Draw(tailFinProjMat4, aView, aProj, projectileColor);


        // wing fins
        DirectX::SimpleMath::Matrix wingFinProjMat1 = DirectX::SimpleMath::Matrix::Identity;
        wingFinProjMat1 *= m_ammoMissile.modelData.localWingFinMatrix1;
        wingFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateRotationZ(wingFinDeployAngle);
        wingFinProjMat1 *= m_ammoMissile.modelData.wingTranslation1;
        wingFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        wingFinProjMat1 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.wingFinShape->Draw(wingFinProjMat1, aView, aProj, projectileColor);

        // wing fin 2
        DirectX::SimpleMath::Matrix wingFinProjMat2 = DirectX::SimpleMath::Matrix::Identity;
        wingFinProjMat2 *= m_ammoMissile.modelData.localWingFinMatrix2;
        wingFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateRotationZ(-wingFinDeployAngle);
        wingFinProjMat2 *= m_ammoMissile.modelData.wingTranslation2;
        wingFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        wingFinProjMat2 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.wingFinShape->Draw(wingFinProjMat2, aView, aProj, projectileColor);

        // wing fin 3
        DirectX::SimpleMath::Matrix wingFinProjMat3 = DirectX::SimpleMath::Matrix::Identity;
        wingFinProjMat3 *= m_ammoMissile.modelData.localWingFinMatrix3;
        wingFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateRotationY(wingFinDeployAngle);
        wingFinProjMat3 *= m_ammoMissile.modelData.wingTranslation3;
        wingFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        wingFinProjMat3 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.wingFinShape->Draw(wingFinProjMat3, aView, aProj, projectileColor);

        // wing fin 4
        DirectX::SimpleMath::Matrix wingFinProjMat4 = DirectX::SimpleMath::Matrix::Identity;
        wingFinProjMat4 *= m_ammoMissile.modelData.localWingFinMatrix4;
        wingFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateRotationY(-wingFinDeployAngle);
        wingFinProjMat4 *= m_ammoMissile.modelData.wingTranslation4;
        wingFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
        wingFinProjMat4 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
        m_ammoMissile.modelData.wingFinShape->Draw(wingFinProjMat4, aView, aProj, projectileColor);
    }
}

void FireControl::InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateIcosahedron(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeTextureMapsExplosion(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap)
{
    m_explosionStruct.textureExplosion = aTexture;
    m_explosionStruct.normalMapExplosion = aNormalMap;
    m_explosionStruct.specularExplosion = aSpecularMap;
}

void FireControl::PushVehicleExplosion(const DirectX::SimpleMath::Vector3 aPos, const int aVehicleId)
{
    std::tuple<DirectX::SimpleMath::Vector3, int> tupleToVec(aPos, aVehicleId);
    m_explosionStruct.explosionToPushVec.push_back(tupleToVec);
}

void FireControl::RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    const float mass = aProjectile->collisionData.mass;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = aProjectile->ammoData.dragCoefficient;
    float frontSurfaceArea = aProjectile->ammoData.frontSurfaceArea;
    float velocity = newQ.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 gravForce = (m_environment->GetGravityVec() * mass) * m_gravityMod;

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += airResistance;
    velocityUpdate += gravForce;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void FireControl::RightHandSideMissile(struct MissileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the 
//  dependent variables.
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    float mass = aProjectile->projectileData.collisionData.mass;

    //  Compute the total drag force.
    float airDensity = m_environment->GetAirDensity();
    //float dragCoefficient = aProjectile->projectileData.ammoData.dragCoefficient;
    float dragCoefficient = aProjectile->projectileData.ammoData.dragCoefficient + aProjectile->guidance.airFoilDragMod;
    if (aProjectile->guidance.isExplodingTrue == true)
    {
        //dragCoefficient = m_missileConsts.postExplosionDragCoefficient;
        //mass = m_missileConsts.postExplosionMass;
    }

    float frontSurfaceArea = aProjectile->projectileData.ammoData.frontSurfaceArea;
    float velocity = newQ.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 gravForce = (m_environment->GetGravityVec() * mass) * m_gravityMod;
    //DirectX::SimpleMath::Vector3 liftForce = aProjectile->projectileData.up * aProjectile->guidance.liftForceFloat;
    DirectX::SimpleMath::Vector3 liftForce = aProjectile->guidance.liftForce;

    DirectX::SimpleMath::Vector3 linearDrag = DirectX::SimpleMath::Vector3::Zero;
    float dragSurfaceArea = aProjectile->guidance.dragSurfaceArea;
    dragCoefficient = aProjectile->guidance.dragCoefficientFull;
    float dragResistanceFull = 0.5f * airDensity * dragSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 dragForce = velocityNorm * (-dragResistanceFull);

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    //velocityUpdate += airResistance;
    velocityUpdate += dragForce;
    velocityUpdate += gravForce;
    velocityUpdate += liftForce;

    if (aProjectile->guidance.isRocketFired == true)
    {
        //DirectX::SimpleMath::Vector3 boostForce = aProjectile->projectileData.forward * m_missileConsts.rocketBoostForceMax;
        DirectX::SimpleMath::Vector3 boostForce = aProjectile->projectileData.forward * aProjectile->guidance.forwardThrust;
        velocityUpdate += boostForce;
    }

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

    // angular

    //DirectX::SimpleMath::Vector3 torqueAccum = aHeli->vehicleAngularForcesSum;
    DirectX::SimpleMath::Vector3 torqueAccum = aProjectile->projectileData.angularForceSum;

    torqueAccum += newQ.angularVelocity;
    //torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, aHeli->inverseInertiaMatrixTest);
    torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, m_missileInverseInertiaTensorLocal);

    //torqueAccum += (m_heli.angularDrag); 
    torqueAccum += (aProjectile->projectileData.angularDragSum);

    aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    aDQ->angularMomentum = static_cast<float>(aTimeDelta) * DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    ProjectileMotion q = aProjectile->q;
    ProjectileMotion dq1;
    ProjectileMotion dq2;
    ProjectileMotion dq3;
    ProjectileMotion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aProjectile->time = aProjectile->time + static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aProjectile->q.velocity = q.velocity;
    aProjectile->q.position = q.position;
    aProjectile->collisionData.collisionSphere.Center = q.position;
    aProjectile->collisionData.velocity = q.velocity;
}

void FireControl::RungeKutta4Missile(struct MissileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    //ProjectileMotion q = aProjectile->q;
    ProjectileMotion q = aProjectile->projectileData.q;
    ProjectileMotion dq1;
    ProjectileMotion dq2;
    ProjectileMotion dq3;
    ProjectileMotion dq4;

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSideMissile(aProjectile, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSideMissile(aProjectile, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSideMissile(aProjectile, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSideMissile(aProjectile, &q, &dq3, aTimeDelta, 1.0, &dq4);

    aProjectile->projectileData.time = aProjectile->projectileData.time + static_cast<float>(aTimeDelta);

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
    DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
    DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    q.angularMomentum += angularMomentumUpdate;
    q.angularVelocity += angularVelocityUpdate;

    aProjectile->projectileData.q.velocity = q.velocity;
    aProjectile->projectileData.q.position = q.position;
    aProjectile->projectileData.collisionData.collisionSphere.Center = q.position;
    aProjectile->projectileData.collisionData.velocity = q.velocity;
    aProjectile->projectileData.q.angularVelocity = q.angularVelocity;
    aProjectile->projectileData.q.angularMomentum = q.angularMomentum;
}

void FireControl::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void FireControl::SetNPCController(std::shared_ptr<NPCController> aNPCController)
{
    m_npcController = aNPCController;
}

void FireControl::ToggleTargetingLaser()
{
    if (m_isTargetingLaserOn == false)
    {
        m_isTargetingLaserOn = true;
    }
    else
    {
        m_isTargetingLaserOn = false;
    }
}

void FireControl::TriggerMirvDeploy()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            if (m_projectileVec[i].isFuseTriggered == false)
            {
                m_projectileVec[i].isFuseTriggered = true;
            }
        }
    }
}

void FireControl::UpdateFireControl(double aTimeDelta)
{
    if (m_isTargetingLaserOn == true)
    {
        CastRayLaser();
    }
    if (m_isCoolDownActive == true)
    {
        m_coolDownTimer -= static_cast<float>(aTimeDelta);
        if (m_coolDownTimer <= 0.0f)
        {
            m_coolDownTimer = 0.0f;
            m_isCoolDownActive = false;
        }
    }

    if (m_muzzleFlash.isFlashActive == true)
    {
        UpdateMuzzleFlash(m_muzzleFlash, aTimeDelta);
    }

    m_testTimer += static_cast<float>(aTimeDelta);
    UpdateProjectileVec(aTimeDelta);
    UpdateMissileVec(aTimeDelta);
    UpdateExplosionVec(aTimeDelta);

    //m_debugData->DebugPushUILineWholeNumber("m_missileVec.size() = ", m_missileVec.size(), "");
    //m_debugData->DebugPushUILineWholeNumber("m_currentTargetID = ", m_currentTargetID, "");
}

void FireControl::UpdateDynamicExplosive(struct ExplosionData& aExplosion, const double aTimeDelta)
{
    Utility::MotionObj obj;
    obj.time = aExplosion.currentDuration;
    obj.q.angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    obj.q.position = aExplosion.position;
    obj.q.velocity = aExplosion.velocity;

    float durationRatio = aExplosion.currentDuration / aExplosion.totalDuration;
    float invsDurationRatio = 1.0f - durationRatio;

    if (invsDurationRatio <= 0.0f || invsDurationRatio >= 1.0f)
    {
        int testBreak = 0;
        testBreak++;
    }
    m_debugData->DebugPushUILineDecimalNumber("invsDurationRatio ", invsDurationRatio, "");
    float explosiveMass = 4.0f;
    //float explosiveMass = m_missileConsts.mass * invsDurationRatio;
    if (explosiveMass <= 0.0f)
    {
        const float minMass = 0.002f;
        explosiveMass = minMass;
    }
    obj.mass = explosiveMass;
    obj.mass = 10.0f;
    //sphere
    float radius = aExplosion.currentRadius;
    //radius = 3.0f;
    obj.inverseInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;
    obj.inverseInertiaMatrix._11 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix._22 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix._33 = (2.0f / 3.0f) * (obj.mass) * (radius * radius);
    obj.inverseInertiaMatrix = obj.inverseInertiaMatrix.Invert();

    DirectX::SimpleMath::Vector3 linearForceSum = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 gravForce = (m_environment->GetGravityVec() * obj.mass) * m_gravityMod;
    linearForceSum += gravForce;

    //  Compute the total drag force.
    const float airDensity = m_environment->GetAirDensity();
    //const float dragCoefficient = m_missileConsts.explosionDragCoefficientBase;
    //const float dragCoefficient = m_missileConsts.explosionDragCoefficientBase + (durationRatio * m_missileConsts.explosionDragCoefficientAddMax);
    const float dragCoefficient = 0.1f;
    //float frontSurfaceArea = radius * radius;
    float frontSurfaceArea = Utility::GetPi() * (radius * radius);
    float velocity = obj.q.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = obj.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);

    DirectX::SimpleMath::Vector3 linearDragSum = DirectX::SimpleMath::Vector3::Zero;
    linearDragSum += airResistance;
    linearDragSum *= 0.1f;
    m_debugData->DebugClearUI();
    m_debugData->DebugPushUILineDecimalNumber("airResistance.x ", airResistance.x, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.y ", airResistance.y, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.z ", airResistance.z, "");
    m_debugData->DebugPushUILineDecimalNumber("airResistance.L ", airResistance.Length(), "");
    obj.linearAccumulated = linearForceSum;
    obj.linearDrag = linearDragSum;
    obj.torqueAccumulated = DirectX::SimpleMath::Vector3::Zero;
    obj.torqueDrag = DirectX::SimpleMath::Vector3::Zero;

    Utility::RungeKutta(&obj, aTimeDelta);
    aExplosion.position = obj.q.position;
    aExplosion.velocity = obj.q.velocity;

    const DirectX::SimpleMath::Vector3 updatedPos = obj.q.position;
    aExplosion.collisionSphere.Center = updatedPos;
    aExplosion.localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);

    DirectX::SimpleMath::Plane groundPlane;
    DirectX::SimpleMath::Vector3 groundNorm = DirectX::SimpleMath::Vector3::Zero;
    float height = 0.0f;
    m_environment->GetVehicleUpdateData(aExplosion.position, groundNorm, height, groundPlane);

    float altitude = aExplosion.position.y - height;

    if (altitude <= 0.0f)
    {
        DirectX::SimpleMath::Vector3 v = aExplosion.velocity;
        DirectX::SimpleMath::Vector3 n = groundNorm;
        const float b = m_environment->GetTerrainCoR();

        DirectX::SimpleMath::Vector3 postImpactVelocity = b * (-2.0f * (v.Dot(n)) * n + v);

        if (postImpactVelocity.y >= 0.0f)
        {
            aExplosion.velocity = postImpactVelocity;
        }
    }
}

void FireControl::UpdateExplosionVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        m_explosionStruct.explosionVec[i].currentDuration += aTimeDelta;
        if (m_explosionStruct.explosionVec[i].currentDuration > m_explosionStruct.explosionVec[i].totalDuration)
        {
            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
            {
                m_npcController->SetVehicleDeath(m_explosionStruct.explosionVec[i].vehicleExplosionID);
            }
            m_explosionStruct.explosionVec[i].isLifeTimeExpired = true;
        }
        else
        {
            float ratio = static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration / m_explosionStruct.explosionVec[i].totalDuration);
            float radius = m_explosionStruct.explosionVec[i].initialRadius + (ratio * m_explosionStruct.explosionVec[i].maxRadius);
            m_explosionStruct.explosionVec[i].currentRadius = radius;
            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
            {
                const DirectX::SimpleMath::Vector3 updatedPos = m_npcController->GetNpcPos(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                m_explosionStruct.explosionVec[i].position = updatedPos;
                m_explosionStruct.explosionVec[i].collisionSphere.Center = updatedPos;
                m_explosionStruct.explosionVec[i].localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
                radius *= m_explosionStruct.internalVehicleExplosionRadiusMod;
            }

            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE)
            {
                const DirectX::SimpleMath::Vector3 updatedVehiclePos = m_npcController->GetNpcPos(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                const DirectX::SimpleMath::Matrix updatedVehicleAlignment = m_npcController->GetNpcAlignment(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                DirectX::SimpleMath::Vector3 updatedExplosionPos = m_explosionStruct.explosionVec[i].localizedSurfaceExplosionPos;
                updatedExplosionPos = DirectX::SimpleMath::Vector3::Transform(updatedExplosionPos, updatedVehicleAlignment);
                updatedExplosionPos += updatedVehiclePos;
                m_explosionStruct.explosionVec[i].position = updatedExplosionPos;
                m_explosionStruct.explosionVec[i].collisionSphere.Center = updatedExplosionPos;
                m_explosionStruct.explosionVec[i].localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedExplosionPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
            }

            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_DYNAMIC)
            {
                UpdateDynamicExplosive(m_explosionStruct.explosionVec[i], aTimeDelta);
            }

            //upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
            const float rotMin = 0.0f;
            const float rotMax = DirectX::XM_2PI;
            //float yOffSet = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
            float yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            float pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            float rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat0 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat1 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat2 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat3 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat4 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat5 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat6 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat7 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            yawRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            pitchRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            rollRot = rotMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (rotMax - rotMin)));
            DirectX::SimpleMath::Matrix rotMat8 = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawRot, pitchRot, rollRot);

            //m_explosionStruct.explosionVec[i].explosionMatrix0 = DirectX::SimpleMath::Matrix::CreateScale(radius);
            //m_explosionStruct.explosionVec[i].explosionMatrix0 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            const float scale = radius * 2.0f;
            m_explosionStruct.explosionVec[i].explosionMatrix0 = DirectX::SimpleMath::Matrix::CreateScale(scale);
            m_explosionStruct.explosionVec[i].explosionMatrix0 *= rotMat0;
            m_explosionStruct.explosionVec[i].explosionMatrix0 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            m_explosionStruct.explosionVec[i].collisionSphere.Radius = radius * 1.0f;

            float baseRedVal = 0.36f;
            float scatterVal0 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 9.0f);
            m_explosionStruct.explosionVec[i].color0 = DirectX::SimpleMath::Vector4((abs(scatterVal0 * 0.5f) + baseRedVal), (abs(scatterVal0 * 0.5f)), 0.0f, 1.0f);
            DirectX::SimpleMath::Vector4 currentColor = m_explosionStruct.explosionVec[i].explosionEndColor - m_explosionStruct.explosionVec[i].explosionStartColor;
            currentColor *= ratio;
            m_explosionStruct.explosionVec[i].explosionCurrentColor = currentColor + m_explosionStruct.explosionVec[i].explosionStartColor;

            //const float scatterDistanceMod = 0.15f * radius;
            const float scatterDistanceMod = 0.15f * scale;

            baseRedVal = 0.5f;
            float scatterVal1 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 10.0f);
            m_explosionStruct.explosionVec[i].color1 = DirectX::SimpleMath::Vector4((abs(scatterVal1 * 0.5f) + baseRedVal), (abs(scatterVal1 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance1 = scatterVal1 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans1(scatterDistance1, scatterDistance1, scatterDistance1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= rotMat1;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans1);
            //m_explosionStruct.explosionVec[i].explosionMatrix1 *= rotMat1;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal2 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 11.0f);
            m_explosionStruct.explosionVec[i].color2 = DirectX::SimpleMath::Vector4((abs(scatterVal2 * 0.5f) + baseRedVal), (abs(scatterVal2 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance2 = scatterVal2 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans2(-scatterDistance2, scatterDistance2, scatterDistance2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= rotMat2;
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal3 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 12.0f);
            m_explosionStruct.explosionVec[i].color3 = DirectX::SimpleMath::Vector4((abs(scatterVal3 * 0.5f) + baseRedVal), (abs(scatterVal3 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance3 = scatterVal3 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans3(scatterDistance3, -scatterDistance3, scatterDistance3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= rotMat3;
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal4 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 13.0f);
            m_explosionStruct.explosionVec[i].color4 = DirectX::SimpleMath::Vector4((abs(scatterVal4 * 0.5f) + baseRedVal), (abs(scatterVal4 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance4 = scatterVal4 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans4(scatterDistance4, scatterDistance4, -scatterDistance4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= rotMat4;
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal5 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 14.0f);
            m_explosionStruct.explosionVec[i].color5 = DirectX::SimpleMath::Vector4((abs(scatterVal5 * 0.5f) + baseRedVal), (abs(scatterVal5 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance5 = scatterVal5 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans5(-scatterDistance5, scatterDistance5, -scatterDistance5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= rotMat5;
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal6 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 15.0f);
            m_explosionStruct.explosionVec[i].color6 = DirectX::SimpleMath::Vector4((abs(scatterVal6 * 0.5f) + baseRedVal), (abs(scatterVal6 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance6 = scatterVal6 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans6(scatterDistance6, -scatterDistance6, -scatterDistance6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= rotMat6;
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal7 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 16.0f);
            m_explosionStruct.explosionVec[i].color7 = DirectX::SimpleMath::Vector4((abs(scatterVal7 * 0.5f) + baseRedVal), (abs(scatterVal7 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance7 = scatterVal7 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans7(-scatterDistance7, -scatterDistance7, scatterDistance7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= rotMat7;
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal8 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 17.0f);
            m_explosionStruct.explosionVec[i].color8 = DirectX::SimpleMath::Vector4((abs(scatterVal8 * 0.5f) + baseRedVal), (abs(scatterVal8 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance8 = scatterVal8 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans8(-scatterDistance8, -scatterDistance8, -scatterDistance8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 = DirectX::SimpleMath::Matrix::CreateScale(scale * scatterVal8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= rotMat8;
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            const float remainingDuration = m_explosionStruct.explosionVec[i].totalDuration - m_explosionStruct.explosionVec[i].currentDuration;
            bool isNpcCollisionTrue = m_npcController->CheckExplosionCollisions(m_explosionStruct.explosionVec[i].collisionSphere, aTimeDelta, remainingDuration);
            if (isNpcCollisionTrue == true)
            {
                // to do : add collision detonation to vehicle
            }
        }
    }

    for (unsigned int i = 0; i < m_explosionStruct.explosionToPushVec.size(); ++i)
    {
        CreateExplosion(get<0>(m_explosionStruct.explosionToPushVec[i]), DirectX::SimpleMath::Vector3::Zero, ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL, get<1>(m_explosionStruct.explosionToPushVec[i]));
    }
    m_explosionStruct.explosionToPushVec.clear();

    for (unsigned int i = 0; i < m_explosionStruct.explosionVec.size(); ++i)
    {
        if (m_explosionStruct.explosionVec[i].isLifeTimeExpired == true)
        {
            std::vector<ExplosionData>::iterator it;
            it = m_explosionStruct.explosionVec.begin() + i;
            m_explosionStruct.explosionVec.erase(it);
        }
    }
}

void FireControl::UpdateMirv(ProjectileData& aProjectile, const double aTimeDelta)
{
    if (aProjectile.isFuseTriggered == true)
    {
        aProjectile.fuseTimer -= static_cast<float>(aTimeDelta);
        if (aProjectile.fuseTimer < 0.0f)
        {
            DeployMirv(aProjectile);
        }
    }

    if (aProjectile.isFuseTriggered == false && aProjectile.q.velocity.y < 0.0f)
    {
        aProjectile.isFuseTriggered = true;
        const float fuseDelay = m_mirvFuseDelayTime;
        aProjectile.fuseTimer = fuseDelay;
    }
}

void FireControl::UpdateProjectileData(ProjectileData& aProjectile, const float aTimeDelta)
{
    aProjectile.time += aTimeDelta;
    DirectX::SimpleMath::Vector3 velocityNorm = aProjectile.q.velocity;
    velocityNorm.Normalize();
    const float t = 4.0f;
    aProjectile.forward = DirectX::SimpleMath::Vector3::SmoothStep(aProjectile.forward, velocityNorm, t * aTimeDelta);
    //aProjectile.forward = DirectX::SimpleMath::Vector3::Lerp(aProjectile.forward, velocityNorm, 0.01f);
    aProjectile.up = aProjectile.right.Cross(aProjectile.forward);
    aProjectile.right = aProjectile.forward.Cross(aProjectile.up);

    DirectX::SimpleMath::Vector3 pos2 = aProjectile.q.position;
    DirectX::SimpleMath::Vector3 up2 = aProjectile.up;
    DirectX::SimpleMath::Vector3 right2 = aProjectile.right;
    //m_debugData->PushDebugLine(pos2, right2, 10.0f, 0.0f, DirectX::Colors::Orange);

    //DirectX::SimpleMath::Vector3 forward = aProjectile.q.velocity;
    DirectX::SimpleMath::Vector3 forward = aProjectile.forward;
    if (forward.Length() < 0.00001f)
    {
        forward = DirectX::SimpleMath::Vector3::UnitX;
    }
    forward.Normalize();
    //DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitY.Cross(forward);
    DirectX::SimpleMath::Vector3 right = aProjectile.right;
    //DirectX::SimpleMath::Vector3 up = right.Cross(forward);
    DirectX::SimpleMath::Vector3 up = aProjectile.up;
    //m_debugData->PushDebugLine(pos2, up, 10.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(pos2, right, 10.0f, 0.0f, DirectX::Colors::Orange);
    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -right, up);
    aProjectile.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(alignMat);
    //aProjectile.alignmentQuat.Normalize();
    aProjectile.inverseAlignmentQuat = aProjectile.alignmentQuat;
    aProjectile.inverseAlignmentQuat.Inverse(aProjectile.inverseAlignmentQuat);
    //aProjectile.inverseAlignmentQuat.Normalize();

    aProjectile.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aProjectile.alignmentQuat);
    aProjectile.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aProjectile.alignmentQuat);
    aProjectile.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aProjectile.alignmentQuat);
}

void FireControl::UpdateMissileData(MissileData& aMissile, const float aTimeDelta)
{
    aMissile.projectileData.time += aTimeDelta;
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();
    const float t = 4.0f;
    /*
    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::SmoothStep(aMissile.projectileData.forward, velocityNorm, t * aTimeDelta);
    aMissile.projectileData.up = aMissile.projectileData.right.Cross(aMissile.projectileData.forward);
    aMissile.projectileData.right = aMissile.projectileData.forward.Cross(aMissile.projectileData.up);

    DirectX::SimpleMath::Vector3 pos2 = aMissile.projectileData.q.position;
    DirectX::SimpleMath::Vector3 up2 = aMissile.projectileData.up;
    DirectX::SimpleMath::Vector3 right2 = aMissile.projectileData.right;

    DirectX::SimpleMath::Vector3 forward = aMissile.projectileData.forward;
    if (forward.Length() < 0.00001f)
    {
        forward = DirectX::SimpleMath::Vector3::UnitX;
    }
    forward.Normalize();

    DirectX::SimpleMath::Vector3 right = aMissile.projectileData.right;

    DirectX::SimpleMath::Vector3 up = aMissile.projectileData.up;

    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -right, up);
    */

    /*
    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aMissile.projectileData.right, aMissile.projectileData.up);
    //DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, aMissile.projectileData.forward, aMissile.projectileData.up);

    aMissile.projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(alignMat);

    aMissile.projectileData.inverseAlignmentQuat = aMissile.projectileData.alignmentQuat;
    aMissile.projectileData.inverseAlignmentQuat.Inverse(aMissile.projectileData.inverseAlignmentQuat);

    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.alignmentQuat);
    */


    DirectX::SimpleMath::Quaternion updateAlignQuat;
    updateAlignQuat.x = aMissile.projectileData.q.angularVelocity.x;
    updateAlignQuat.y = aMissile.projectileData.q.angularVelocity.y;
    updateAlignQuat.z = aMissile.projectileData.q.angularVelocity.z;
    updateAlignQuat.w = 0.0f;
    DirectX::SimpleMath::Quaternion updateAlignOutputQuat = aMissile.projectileData.alignmentQuat;
    updateAlignOutputQuat.Normalize();

    updateAlignOutputQuat += (0.5f * aTimeDelta) * (updateAlignQuat * aMissile.projectileData.alignmentQuat);
    updateAlignOutputQuat.Normalize();

    aMissile.projectileData.alignmentQuat = updateAlignOutputQuat;


    aMissile.projectileData.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aMissile.projectileData.alignmentQuat);

    aMissile.projectileData.inverseAlignmentQuat = aMissile.projectileData.alignmentQuat;
    aMissile.projectileData.inverseAlignmentQuat.Inverse(aMissile.projectileData.inverseAlignmentQuat);
    aMissile.projectileData.inverseAlignmentQuat.Normalize();

    /*
    DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aMissile.projectileData.right, aMissile.projectileData.up);
    //DirectX::SimpleMath::Matrix alignMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, aMissile.projectileData.forward, aMissile.projectileData.up);

    aMissile.projectileData.alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(alignMat);

    aMissile.projectileData.inverseAlignmentQuat = aMissile.projectileData.alignmentQuat;
    aMissile.projectileData.inverseAlignmentQuat.Inverse(aMissile.projectileData.inverseAlignmentQuat);

    aMissile.projectileData.forward = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitX, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.right = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitZ, aMissile.projectileData.alignmentQuat);
    aMissile.projectileData.up = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::UnitY, aMissile.projectileData.alignmentQuat);
    */
}

void FireControl::UpdateMissileForces(MissileData& aMissile, const float aTimeDelta)
{
    float thrustAngle = Utility::GetAngleBetweenVectors(aMissile.guidance.heading, aMissile.projectileData.forward);
    float rotationThrustForce = sin(thrustAngle) * m_missileConsts.rocketBoostForceMax;

    // Update rocket throttle value
    float rocketThrottle;
    if (aMissile.projectileData.time >= m_missileConsts.rocketFireFullTime + m_missileConsts.rocketFireDelay)
    {
        rocketThrottle = 1.0f;
    }
    else if (aMissile.projectileData.time < m_missileConsts.rocketFireDelay)
    {
        rocketThrottle = 0.0f;
    }
    else
    {
        rocketThrottle = (aMissile.projectileData.time - m_missileConsts.rocketFireDelay) / (m_missileConsts.rocketFireFullTime);
    }

    aMissile.guidance.throttlePercentage = (aMissile.guidance.throttlePercentage + rocketThrottle) * 0.5f;
    
    DirectX::SimpleMath::Vector3 torqueArm = DirectX::SimpleMath::Vector3(m_ammoMissile.ammoData.length * 0.5f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 torqueForce = aMissile.guidance.heading;
    torqueForce = DirectX::SimpleMath::Vector3::Transform(torqueForce, aMissile.projectileData.inverseAlignmentQuat);
    torqueForce.Normalize();
    //torqueForce *= rotationThrustForce;
    torqueForce *= m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage;

    Utility::Torque thrustTorque = Utility::GetTorqueForce(torqueArm, torqueForce);
    aMissile.projectileData.angularForceSum = thrustTorque.axis * (thrustTorque.magnitude * aTimeDelta * 0.3f);
    //aMissile.projectileData.angularForceSum = thrustTorque.axis * (thrustTorque.magnitude * aTimeDelta);

    const float angularDrageMod = m_missileConsts.angularDragMod;
    DirectX::SimpleMath::Vector3 angularDrag = DirectX::SimpleMath::Vector3::Zero;
    angularDrag = aMissile.projectileData.q.angularVelocity * -powf(angularDrageMod, aTimeDelta);

    aMissile.projectileData.angularDragSum = angularDrag;

    // update forward thrust to be fraction remaining after side thrust removed
    //float adjacent = cos(thrustAngle) * m_missileConsts.rocketBoostForceMax;
    //aMissile.guidance.forwardThrust = aMissile.projectileData.forward * (m_missileConsts.rocketBoostForceMax - aMissile.projectileData.angularForceSum.Length());
    //aMissile.guidance.forwardThrust = cos(thrustAngle) * m_missileConsts.rocketBoostForceMax;
    // Average out thrust value with previous frame to smooth 
    //aMissile.guidance.forwardThrust += cos(thrustAngle) * m_missileConsts.rocketBoostForceMax;
    //aMissile.guidance.forwardThrust *= 0.5f;

    aMissile.guidance.forwardThrust = (aMissile.guidance.forwardThrust + (cos(thrustAngle) * (m_missileConsts.rocketBoostForceMax * aMissile.guidance.throttlePercentage))) * 0.5f;

    if (aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.throttlePercentage = 0.0f;
        aMissile.guidance.forwardThrust = 0.0f;
        aMissile.projectileData.angularForceSum = DirectX::SimpleMath::Vector3::Zero;
    }
    UpdateMissileForcesLift(aMissile, aTimeDelta);
    UpdateMissileDragLinear(aMissile, aTimeDelta);
}

//float FireControl::UpdateMissileLiftCoefficient(MissileData& aMissile, const float aTimeDelta)
void FireControl::UpdateMissileCoefficients(MissileData& aMissile, const float aTimeDelta)
{
    // calculate angle of attack
    DirectX::SimpleMath::Vector3 localizedVelocityNorm = aMissile.projectileData.q.velocity;
    localizedVelocityNorm = DirectX::SimpleMath::Vector3::Transform(localizedVelocityNorm, aMissile.projectileData.inverseAlignmentQuat);
    localizedVelocityNorm.z = 0.0f;
    localizedVelocityNorm.Normalize();
    float angleOfAttack = Utility::GetAngleBetweenVectors(localizedVelocityNorm, DirectX::SimpleMath::Vector3::UnitX);
    DirectX::SimpleMath::Vector3 localVelocityNormCross = localizedVelocityNorm.Cross(DirectX::SimpleMath::Vector3::UnitX);

    aMissile.guidance.angleOfAttack = angleOfAttack;
    aMissile.guidance.angleOfAttack = Utility::WrapAngleRight(aMissile.guidance.angleOfAttack);
    //aMissile.guidance.angleOfAttack = Utility::ToDegrees(aMissile.guidance.angleOfAttack);
    aMissile.guidance.angleOfAttack = abs(aMissile.guidance.angleOfAttack);

    if (localVelocityNormCross.z < 0.0f)
    {
        angleOfAttack *= -1.0f;
    }

    const float angleMax = Utility::ToRadians(90.0f);
    const float angleMin = Utility::ToRadians(0.0f);
    const float currentCurvePos = (angleOfAttack / angleMax);
    //m_debugData->DebugPushUILineDecimalNumber("angleOfAttack ", Utility::ToDegrees(angleOfAttack), "");
    //m_debugData->DebugPushUILineDecimalNumber("currentCurvePos ", currentCurvePos, "");
    float Cl;
    float curveDeltaRate;
    float ClTarget;
    if (currentCurvePos < 0.666f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else if (currentCurvePos < 0.80f)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    else
    {
        curveDeltaRate = -0.85f;
        Cl = curveDeltaRate * currentCurvePos;
    }
    //m_debugData->DebugPushUILineDecimalNumber("Cl ******** ", Cl, "");
    // ////////////////////////////
    const float ClMax = 1.8;
    const float inflectionPoint = 0.99f;
    const float curvePos2 = currentCurvePos / inflectionPoint;
    if (currentCurvePos < inflectionPoint)
    {
        curveDeltaRate = 1.0f;
        Cl = curveDeltaRate * currentCurvePos;
        ClTarget = 1.5f;
        ClTarget = curvePos2 * ClMax;
    }
    else
    {
        float rightBound = 1.3f;
        float downCurve = rightBound - ClMax;

        const float curvePos4 = (currentCurvePos - inflectionPoint) / (1.0f - inflectionPoint);
        float ClRemove = curvePos4 * downCurve;
        ClTarget = ClMax;
        ClTarget += ClRemove;
    }
    //m_debugData->DebugPushUILineDecimalNumber("Cl ******** ", Cl, "");
    //m_debugData->DebugPushUILineDecimalNumber("ClTarget ******** ", ClTarget, "");


    ///////////////////////////////////////////////////
    // Testing lift coefficent calculations
    const float alphaClMax = 16.0f;
    float alpha = angleOfAttack;
    alpha = Utility::WrapAngleRight(alpha);
    alpha = Utility::ToDegrees(alpha);
    alpha = abs(alpha);
    const float clSlope0 = 0.0889f;
    const float clSlope1 = -0.1f;
    float cl0 = 0.178; // intercept of Cl-alpha curve
    float cl1 = 3.2;  // post-stall intercept of Cl-alpha curve
    float cl;

    if (alpha < alphaClMax)
    {
        cl = clSlope0 * alpha + cl0;
    }
    else
    {
        cl = clSlope1 * alpha + cl1;
    }

    /////////////////////////////////////////////////////////////////////
    // Drag coefficient
    //const float alphaDlMax = Utility::ToRadians(16.0f);
    const float alphaDlMax = 16.0f;
    float alphaD = angleOfAttack;
    float wrappedAlphaD = alphaD;
    wrappedAlphaD = Utility::WrapAngleRight(wrappedAlphaD);
    wrappedAlphaD = Utility::ToDegrees(wrappedAlphaD);
    m_debugData->DebugPushUILineDecimalNumber("wrappedAlphaD ", wrappedAlphaD, "");
    m_debugData->DebugPushUILineDecimalNumber("angleOfAttack ", Utility::ToDegrees(angleOfAttack), "");
    alphaD = Utility::WrapAngleRight(alphaD);
    alphaD = Utility::ToDegrees(alphaD);
    alphaD = abs(alphaD);
    m_debugData->DebugPushUILineDecimalNumber("alphaD ", alphaD, "");
    const float dlSlope0 = 0.01f;
    const float dlSlope1 = 0.04f;
    float dl0 = 0.04f; // intercept of Cl-alpha curve
    float dl1 = 0.45;  // post-stall intercept of Cl-alpha curve
    float dl;

    // streamlined body coeff = 0.04f , full lock = ~ 2.0f, or 1.98f
    // 0.04 + 0.15

    if (alphaD < alphaDlMax)
    {
        dl = dlSlope0 * alphaD + dl0;
    }
    else
    {
        dl = dlSlope1 * alphaD + dl1;
    }
    aMissile.guidance.airFoilDragMod = dl;
    m_debugData->DebugPushUILineDecimalNumber("cl ", cl, "");
    m_debugData->DebugPushUILineDecimalNumber("dl ", dl, "");
    /////////////////////////////////////////////////////////////////////
    aMissile.guidance.liftCoefficient = cl;
    //return cl;
    ///////////////////////////////////////////////////
    if (ClTarget < 0.0f)
    {
        ClTarget = 0.0f;
        //ClTarget *= -1.0f;
    }
    ClTarget = 1.0f;
    //return ClTarget;
}

void FireControl::UpdateMissileDragLinear(MissileData& aMissile, const float aTimeDelta)
{
    
    float airDensity = m_environment->GetAirDensity();
    float dragCoefficient = aMissile.projectileData.ammoData.dragCoefficient + aMissile.guidance.airFoilDragMod;
    if (aMissile.guidance.isExplodingTrue == true)
    {
        //dragCoefficient = m_missileConsts.postExplosionDragCoefficient;
        //mass = m_missileConsts.postExplosionMass;
    }

    float frontSurfaceArea = aMissile.projectileData.ammoData.frontSurfaceArea;
    float frontSurfaceAreaTest = (aMissile.projectileData.ammoData.radius * 2.0f) * 2.0f;
    float frontSurfaceAreatTest2 = Utility::GetPi() * (aMissile.projectileData.ammoData.radius * aMissile.projectileData.ammoData.radius);
    float sideSurfaceArea = aMissile.projectileData.ammoData.length * (aMissile.projectileData.ammoData.radius * 2.0f);
    float velocity = aMissile.projectileData.q.velocity.Length();
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * velocity * velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);


    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += airResistance;


    float dragSurfaceArea = 0.0f;
    dragSurfaceArea += frontSurfaceArea * cos(aMissile.guidance.angleOfAttack);
    dragSurfaceArea += sideSurfaceArea * sin(aMissile.guidance.angleOfAttack);
    aMissile.guidance.dragSurfaceArea = dragSurfaceArea;
    
    float dragCoefficentSum = aMissile.projectileData.ammoData.dragCoefficient;
    const float sideProfileDragCoefficient = 0.82f; // long cylinder shape
    const float sideMinusFrontCl = sideProfileDragCoefficient - aMissile.projectileData.ammoData.dragCoefficient;

    dragCoefficentSum += abs(sideMinusFrontCl)  * sin(aMissile.guidance.angleOfAttack);
    aMissile.guidance.dragCoefficientFull = abs(dragCoefficentSum + aMissile.guidance.airFoilDragMod);
}

void FireControl::UpdateMissileForcesLift(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.isFinsDeployEnd == false || aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.liftForce = DirectX::SimpleMath::Vector3::Zero;
        aMissile.guidance.liftForceFloat = 0.0f;
    }
    else
    {
        //const float cL = UpdateMissileLiftCoefficient(aMissile, aTimeDelta);
        UpdateMissileCoefficients(aMissile, aTimeDelta);
        const float cL = aMissile.guidance.liftCoefficient;
        const float airDensity = m_environment->GetAirDensity();
        const float wingArea = m_missileConsts.wingArea;

        DirectX::SimpleMath::Vector3 worldVelocityNorm = aMissile.projectileData.q.velocity;
        worldVelocityNorm.Normalize();
        float velocityDotProd = worldVelocityNorm.Dot(aMissile.projectileData.forward);
        float velocityLength = aMissile.projectileData.q.velocity.Length();

        float liftF = cL * ((airDensity * (velocityLength * velocityLength)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftV = cL * ((airDensity * (aMissile.projectileData.q.velocity * aMissile.projectileData.q.velocity)) * 0.5f) * wingArea;
        float liftVLength = liftV.Length();
        float rawVelocityLength = aMissile.projectileData.q.velocity.Length();

        DirectX::SimpleMath::Vector3 fowardVelocity = aMissile.projectileData.forward * (velocityDotProd * aMissile.projectileData.q.velocity);
        DirectX::SimpleMath::Vector3 lineOfFlight = aMissile.projectileData.q.velocity;
        DirectX::SimpleMath::Vector3 liftVector = cL * ((airDensity * (fowardVelocity * fowardVelocity)) * 0.5f) * wingArea;
        DirectX::SimpleMath::Vector3 liftVectorLoF = cL * ((airDensity * (lineOfFlight * lineOfFlight)) * 0.5f) * wingArea;
        float forwardVelocityLength = fowardVelocity.Length();
        float forwardLength = aMissile.projectileData.forward.Length();
        float liftVectorLength = liftVector.Length();
        //m_debugData->DebugClearUI();
        //m_debugData->DebugPushUILineDecimalNumber("liftVectorLength ", liftVectorLength, "");
        //m_debugData->DebugPushUILineDecimalNumber("liftF             ", liftF, "");
        //m_debugData->DebugPushUILineDecimalNumber("liftVectorLoF.L   ", liftVectorLoF.Length(), "");
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftVector, 35.0f, 0.3f, DirectX::Colors::Orange);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftVectorLoF, 35.0f, 0.3f, DirectX::Colors::Lime);

        DirectX::SimpleMath::Vector3 chordLine = aMissile.projectileData.right;
        DirectX::SimpleMath::Vector3 liftLine = chordLine.Cross(worldVelocityNorm);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, liftLine, 35.0f, 0.3f, DirectX::Colors::Violet);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, chordLine, 35.0f, 0.3f, DirectX::Colors::Lime);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldVelocityNorm, 35.0f, 0.3f, DirectX::Colors::Orange);

        liftLine *= abs(liftF);

        //aMissile.guidance.liftForce = liftVector;
        aMissile.guidance.liftForce = liftLine;
        aMissile.guidance.liftForceFloat = liftF;


        float testLift = liftF;
        if (testLift < 0.0f)
        {
            testLift *= -1.0f;
        }
        if (testLift > 100.0f)
        {
            testLift = 100.0f;
        }

        DirectX::SimpleMath::Vector3 localizedNormY = DirectX::SimpleMath::Vector3::UnitY;
        localizedNormY = DirectX::SimpleMath::Vector3::Transform(localizedNormY, aMissile.projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 crossProdY = localizedNormY;
        crossProdY = crossProdY.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 worldCrossProdY = crossProdY;
        worldCrossProdY = DirectX::SimpleMath::Vector3::Transform(worldCrossProdY, aMissile.projectileData.alignmentQuat);

        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, worldCrossProdY, 100.0f, 0.0f, DirectX::Colors::Lavender);
    
        //DirectX::SimpleMath::Vector3 trueRightWorld = DirectX::SimpleMath::Vector3::UnitY.Cross(aMissile.projectileData.forward);
        DirectX::SimpleMath::Vector3 trueRightWorld = aMissile.projectileData.forward.Cross(DirectX::SimpleMath::Vector3::UnitY);
        DirectX::SimpleMath::Vector3 trueRightLocal = trueRightWorld;
        trueRightLocal = DirectX::SimpleMath::Vector3::Transform(trueRightLocal, aMissile.projectileData.inverseAlignmentQuat);
        DirectX::SimpleMath::Vector3 trueUpLocal = trueRightLocal.Cross(DirectX::SimpleMath::Vector3::UnitX);
        DirectX::SimpleMath::Vector3 trueUpWorld = trueUpLocal;
        trueUpWorld = DirectX::SimpleMath::Vector3::Transform(trueUpWorld, aMissile.projectileData.alignmentQuat);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, trueRightWorld, 100.0f, 0.0f, DirectX::Colors::White);
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, trueUpWorld, 100.0f, 0.0f, DirectX::Colors::White);
        trueUpWorld.Normalize();

        //aMissile.guidance.liftForce = testLift * DirectX::SimpleMath::Vector3::UnitY;
        aMissile.guidance.liftForce = testLift * trueUpWorld;

        //m_debugData->DebugPushUILineDecimalNumber("aMissile.guidance.liftForce.L   ", aMissile.guidance.liftForce.Length(), "");
        //m_debugData->PushDebugLine(aMissile.projectileData.q.position, aMissile.guidance.liftForce, 200.0f, 0.0f, DirectX::Colors::Yellow);
        if (aMissile.guidance.liftForce.y < 0.0f)
        {
            int testBreak = 0;
            testBreak++;
        }
    }
}

void FireControl::UpdateMissileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        UpdateMissileForces(m_missileVec[i], static_cast<float>(aTimeDelta));
        RungeKutta4Missile(&m_missileVec[i], aTimeDelta);
    }

    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        UpdateMissileGuidance(m_missileVec[i], static_cast<float>(aTimeDelta));
        UpdateMissileData(m_missileVec[i], static_cast<float>(aTimeDelta));
        //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.forward, 500.0f, 0.0f, DirectX::Colors::Fuchsia);
        //m_debugData->DebugPushUILineDecimalNumber("Angular Velocity", m_missileVec[i].projectileData.q.angularVelocity.Length(), "");
        //m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, DirectX::Colors::Orange);
        //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.heading, 1000.0f, 0.9f, DirectX::Colors::Lime);
    }

    CheckCollisionsMissile();

    int deleteCount = 0;
    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        if (m_missileVec[i].projectileData.isDeleteTrue == true)
        {
            float timer = m_missileVec[i].projectileData.time;
            deleteCount++;
            DeleteMissileFromVec(i);
        }
        else
        {
            //m_debugData->DebugPushUILineDecimalNumber("m_missileVec[i].guidance.forwardThrust ", m_missileVec[i].guidance.forwardThrust, "");
        }
    }
    //TestFunc(aTimeDelta);

    /*
    float angle = m_testTimer;
    angle = Utility::WrapAngleRight(angle);
    m_debugData->DebugPushUILineDecimalNumber("angle rads ", angle, "");
    m_debugData->DebugPushUILineDecimalNumber("angle degs ", Utility::ToDegrees(angle), "");
    */
}

void FireControl::UpdateProjectileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        RungeKutta4(&m_projectileVec[i], aTimeDelta);
    }

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            UpdateMirv(m_projectileVec[i], aTimeDelta);
        }
    }

    if (m_newProjectilePushVec.size() > 0)
    {
        for (unsigned int i = 0; i < m_newProjectilePushVec.size(); ++i)
        {
            m_projectileVec.push_back(m_newProjectilePushVec[i]);
        }
        m_newProjectilePushVec.clear();
    }

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        UpdateProjectileData(m_projectileVec[i], static_cast<float>(aTimeDelta));
    }

    CheckCollisions();

    int deleteCount = 0;
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        if (m_projectileVec[i].isDeleteTrue == true)
        {
            deleteCount++;
            DeleteProjectileFromVec(i);
        }
    }
}

void FireControl::UpdateMissileGuidance2(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.postExplosionDrawCountDown -= aTimeDelta;
        if (aMissile.guidance.postExplosionDrawCountDown <= 0.0f)
        {
            aMissile.projectileData.isDeleteTrue = true;
        }
    }

    if (aMissile.guidance.isRocketFired == false && aMissile.projectileData.time >= m_missileConsts.rocketFireDelay)
    {
        aMissile.guidance.isRocketFired = true;
    }
    if (aMissile.guidance.isFinsDeployStarted == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay)
    {
        aMissile.guidance.isFinsDeployStarted = true;
    }
    if (aMissile.guidance.isFinsDeployEnd == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay + m_missileConsts.finDeployTime)
    {
        aMissile.guidance.isFinsDeployEnd = true;
    }
    if (aMissile.guidance.isRocketFired == true)
    {
        if (aMissile.guidance.isTargetLocked == true)
        {
            m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity);
            aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();

            //m_debugData->PushDebugLinePositionIndicator(aMissile.guidance.targetPosition, 100.0f, 0.0f, DirectX::Colors::Orange);

            DirectX::SimpleMath::Vector3 vecToTargetNorm = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
            vecToTargetNorm.Normalize();

            // time to target begin
            DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
            velocityNorm.Normalize();
            float velocityToTarget = velocityNorm.Dot(vecToTargetNorm) * aMissile.projectileData.q.velocity.Length();
            float timeToTarget = aMissile.guidance.targetDistance / velocityToTarget;

            m_debugData->DebugPushUILineDecimalNumber("timeToTarget     = ", timeToTarget, "");
            m_debugData->DebugPushUILineDecimalNumber("targetDistance   = ", aMissile.guidance.targetDistance, "");
            m_debugData->DebugPushUILineDecimalNumber("velocityToTarget = ", velocityToTarget, "");

            // time to target end

            // next counter verticle drop, current y velocity + estimated drop by estimated impact time
            float verticleDrop = aMissile.projectileData.q.velocity.y;
            float estimatedVerticleVelocityToCounter = aMissile.projectileData.q.velocity.y + m_environment->GetGravity() * timeToTarget;
            float dropDistance = (aMissile.projectileData.q.velocity.y * timeToTarget) + (0.5f * m_environment->GetGravity() * timeToTarget * timeToTarget);
            float dropDistance2 = (0.5f * m_environment->GetGravity() * timeToTarget * timeToTarget);
            float dropDistance3 = aMissile.projectileData.q.position.y - dropDistance2;
            m_debugData->DebugPushUILineDecimalNumber("dropDistance      = ", dropDistance, "");
            m_debugData->DebugPushUILineDecimalNumber("dropDistance2     = ", dropDistance2, "");
            m_debugData->DebugPushUILineDecimalNumber("dropDistance3     = ", dropDistance3, "");

            aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;
            //DirectX::SimpleMath::Vector3 localTargetPos = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
            DirectX::SimpleMath::Vector3 localTargetPos = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            m_debugData->PushDebugLinePositionIndicator(aMissile.guidance.targetDestination, 30.0f, 0.0f, DirectX::Colors::Blue);
            m_debugData->PushDebugLinePositionIndicator(localTargetPos, 30.0f, 0.0f, DirectX::Colors::Red);
            localTargetPos = DirectX::SimpleMath::Vector3::Transform(localTargetPos, aMissile.projectileData.inverseAlignmentQuat);

            DirectX::SimpleMath::Vector3 testPosLocalTargetPos = localTargetPos;
            m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 30.0f, 0.0f, DirectX::Colors::Lime);
            DirectX::SimpleMath::Vector3 testPosLocalTargetPos2 = localTargetPos * -1.0f;
            localTargetPos = testPosLocalTargetPos2;
            testPosLocalTargetPos2 = DirectX::SimpleMath::Vector3::Transform(testPosLocalTargetPos2, aMissile.projectileData.alignmentQuat);
            m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos2, 30.0f, 0.0f, DirectX::Colors::Aquamarine);

            //m_debugData->PushDebugLinePositionIndicator(localTargetPos, 100.0f, 0.0f, DirectX::Colors::Blue);

            DirectX::SimpleMath::Vector3 worldTargetPos = localTargetPos;
            worldTargetPos += aMissile.projectileData.q.position;
            worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, aMissile.projectileData.alignmentQuat);

            //m_debugData->PushDebugLinePositionIndicator(worldTargetPos, 100.0f, 0.0f, DirectX::Colors::Yellow);

            //const float maxAngle = Utility::ToRadians(45.0f);
            const float maxAngle = m_missileConsts.seekerHeadAngleMax;

            //DirectX::SimpleMath::Quaternion invAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
            DirectX::SimpleMath::Quaternion invAlignmentQuat = aMissile.projectileData.alignmentQuat;
            invAlignmentQuat.Normalize();
            invAlignmentQuat.Inverse(invAlignmentQuat);

            //DirectX::SimpleMath::Vector3 vecForward = m_playerVehicle->GetForward();
            DirectX::SimpleMath::Vector3 vecForward = aMissile.projectileData.forward;
            vecForward.Normalize();
            vecForward = DirectX::SimpleMath::Vector3::Transform(vecForward, invAlignmentQuat);

            DirectX::SimpleMath::Vector3 targPos = aMissile.guidance.targetPosition;
            m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, targPos, DirectX::Colors::Lime);

            //DirectX::SimpleMath::Vector3 targetOffset = aMissile.projectileData.q.velocity * (timeToTarget * 0.35f);
            DirectX::SimpleMath::Vector3 targetOffset = aMissile.projectileData.q.velocity * (timeToTarget * 1.0f);
            targetOffset = DirectX::SimpleMath::Vector3::Zero;
            //////////////////////////////////////
            // flip timeToTarget so magnitude is increased the closer to target
            const float maxTimeOffset = 45.0f;
            if (timeToTarget > maxTimeOffset)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::Gray);
            }
            else if (timeToTarget < -maxTimeOffset)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::LightGray);
                int testBreak2 = 0;
                testBreak2++;
            }
            else if (timeToTarget < 0.0f)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::Navy);
                float timeToTargetRatio = (maxTimeOffset + timeToTarget) / maxTimeOffset;
                targetOffset = -aMissile.projectileData.q.velocity * (timeToTargetRatio * 1.0f);
            }
            else
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::LightPink);
                float timeToTargetRatio = (maxTimeOffset - timeToTarget) / maxTimeOffset;
                targetOffset = aMissile.projectileData.q.velocity * (timeToTargetRatio * 1.0f);
            }
            m_debugData->DebugPushUILineDecimalNumber("targetOffset.Length() = ", targetOffset.Length(), "");
            //////////////////////////////////////

            targPos -= targetOffset;
            //targPos.y -= (dropDistance3 * 0.2f);

            DirectX::SimpleMath::Vector3 testTargPost = aMissile.guidance.targetPosition - targetOffset;
            m_debugData->PushDebugLinePositionIndicator(testTargPost, 40.0f, 0.0f, DirectX::Colors::White);
            //m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, targPos, DirectX::Colors::Blue);


            DirectX::SimpleMath::Vector3 vecToTarget = targPos - aMissile.projectileData.q.position;
            vecToTarget.Normalize();
            m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 1000.0f, 0.0f, DirectX::Colors::Orange);
            vecToTarget = DirectX::SimpleMath::Vector3::Transform(vecToTarget, invAlignmentQuat);
            m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 100.0f, 0.0f, DirectX::Colors::Blue);

            DirectX::SimpleMath::Quaternion q;
            DirectX::SimpleMath::Vector3 a = vecForward.Cross(vecToTarget);
            q.x = a.x;
            q.y = a.y;
            q.z = a.z;
            q.w = sqrt((vecForward.Length() * vecForward.Length()) * (vecToTarget.Length() * vecToTarget.Length())) + vecForward.Dot(vecToTarget);
            q.Normalize();

            //DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
            DirectX::SimpleMath::Quaternion alignmentQuat = aMissile.projectileData.alignmentQuat;
            alignmentQuat.Normalize();
            alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
            alignmentQuat.RotateTowards(q, maxAngle);

            //DirectX::SimpleMath::Vector3 testForward = m_playerVehicle->GetForward();
            DirectX::SimpleMath::Vector3 testForward = aMissile.projectileData.forward;
            testForward = DirectX::SimpleMath::Vector3::Transform(testForward, alignmentQuat);

            m_debugData->PushDebugLine(aMissile.projectileData.q.position, testForward, 3500.0f, 1.0f, DirectX::Colors::White);
            aMissile.guidance.heading = testForward;

            /*
            alignmentQuat.Inverse(alignmentQuat);

            DirectX::SimpleMath::Vector3 testForward2 = DirectX::SimpleMath::Vector3::UnitX;
            testForward2 = DirectX::SimpleMath::Vector3::Transform(testForward2, alignmentQuat);
            //testForward2 * -1.0f;
            //testForward2 = DirectX::SimpleMath::Vector3::Transform(testForward2, DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment()));
            testForward2 = DirectX::SimpleMath::Vector3::Transform(testForward2, aMissile.projectileData.alignmentQuat);
            //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), testForward2, 35.0f, 2.0f, DirectX::Colors::Red);
            //m_debugData->PushDebugLine(aMissile.projectileData.forward, testForward2, 35.0f, 2.0f, DirectX::Colors::Red);
            aMissile.guidance.heading = testForward;
            */
        }
    }
}

void FireControl::UpdateMissileGuidance(MissileData& aMissile, const float aTimeDelta)
{
    if (aMissile.guidance.isExplodingTrue == true)
    {
        aMissile.guidance.postExplosionDrawCountDown -= aTimeDelta;
        if (aMissile.guidance.postExplosionDrawCountDown <= 0.0f)
        {
            aMissile.projectileData.isDeleteTrue = true;
        }
    }
    if (aMissile.guidance.isTargetingLaserOn == false && aMissile.projectileData.time >= m_missileConsts.laserDepoyDelay)
    {
        aMissile.guidance.isTargetingLaserOn = true;
    }
    if (aMissile.guidance.isRocketFired == false && aMissile.projectileData.time >= m_missileConsts.rocketFireDelay)
    {
        aMissile.guidance.isRocketFired = true;
    }
    if (aMissile.guidance.isFinsDeployStarted == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay)
    {
        aMissile.guidance.isFinsDeployStarted = true;
    }
    if (aMissile.guidance.isFinsDeployEnd == false && aMissile.projectileData.time >= m_missileConsts.finDeployDelay + m_missileConsts.finDeployTime)
    {
        aMissile.guidance.isFinsDeployEnd = true;
    }
    if (aMissile.guidance.isRocketFired == true)
    {
        if (aMissile.guidance.isTargetLocked == true)
        {
            m_npcController->UpdateMissleGuidance(m_currentTargetID, aMissile.guidance.targetPosition, aMissile.guidance.targetVelocity);
            aMissile.guidance.targetDistance = (aMissile.projectileData.q.position - aMissile.guidance.targetPosition).Length();
  
            DirectX::SimpleMath::Vector3 vecToTargetNorm = aMissile.guidance.targetPosition - aMissile.projectileData.q.position;
            vecToTargetNorm.Normalize();

            // time to target begin
            DirectX::SimpleMath::Vector3 velocityNorm = aMissile.projectileData.q.velocity;
            velocityNorm.Normalize();
            float velocityToTarget = velocityNorm.Dot(vecToTargetNorm) * aMissile.projectileData.q.velocity.Length();
            float timeToTarget = aMissile.guidance.targetDistance / velocityToTarget;

            m_debugData->DebugPushUILineDecimalNumber("timeToTarget     = ", timeToTarget, "");
            m_debugData->DebugPushUILineDecimalNumber("targetDistance   = ", aMissile.guidance.targetDistance, "");
            m_debugData->DebugPushUILineDecimalNumber("velocityToTarget = ", velocityToTarget, "");

            float velocityToTargetDot = velocityNorm.Dot(vecToTargetNorm);
            m_debugData->DebugPushUILineDecimalNumber("velocityToTargetDot = ", velocityToTargetDot, "");
            // time to target end

            // next counter verticle drop, current y velocity + estimated drop by estimated impact time
            float verticleDrop = aMissile.projectileData.q.velocity.y;
            float estimatedVerticleVelocityToCounter = aMissile.projectileData.q.velocity.y + m_environment->GetGravity() * timeToTarget;
            float dropDistance = (aMissile.projectileData.q.velocity.y * timeToTarget) + (0.5f * m_environment->GetGravity() * timeToTarget * timeToTarget);
            float dropDistance2 = (0.5f * m_environment->GetGravity() * timeToTarget * timeToTarget);
            float dropDistance3 = aMissile.projectileData.q.position.y - dropDistance2;
            m_debugData->DebugPushUILineDecimalNumber("dropDistance      = ", dropDistance, "");
            m_debugData->DebugPushUILineDecimalNumber("dropDistance2     = ", dropDistance2, "");
            m_debugData->DebugPushUILineDecimalNumber("dropDistance3     = ", dropDistance3, "");

            aMissile.guidance.targetDestination = aMissile.guidance.targetPosition;
            //DirectX::SimpleMath::Vector3 localTargetPos = aMissile.guidance.targetDestination - aMissile.projectileData.q.position;
            DirectX::SimpleMath::Vector3 localTargetPos = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            m_debugData->PushDebugLinePositionIndicator(aMissile.guidance.targetDestination, 30.0f, 0.0f, DirectX::Colors::Blue);
            m_debugData->PushDebugLinePositionIndicator(localTargetPos, 30.0f, 0.0f, DirectX::Colors::Red);
            localTargetPos = DirectX::SimpleMath::Vector3::Transform(localTargetPos, aMissile.projectileData.inverseAlignmentQuat);

            DirectX::SimpleMath::Vector3 testPosLocalTargetPos = localTargetPos;
            m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 30.0f, 0.0f, DirectX::Colors::Lime);
            DirectX::SimpleMath::Vector3 testPosLocalTargetPos2 = localTargetPos * -1.0f;
            localTargetPos = testPosLocalTargetPos2;
            testPosLocalTargetPos2 = DirectX::SimpleMath::Vector3::Transform(testPosLocalTargetPos2, aMissile.projectileData.alignmentQuat);
            m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos2, 30.0f, 0.0f, DirectX::Colors::Aquamarine);
            //m_debugData->PushDebugLinePositionIndicator(localTargetPos, 100.0f, 0.0f, DirectX::Colors::Blue);

            DirectX::SimpleMath::Vector3 worldTargetPos = localTargetPos;
            worldTargetPos += aMissile.projectileData.q.position;
            worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, aMissile.projectileData.alignmentQuat);

            const float maxAngle = m_missileConsts.seekerHeadAngleMax;
            DirectX::SimpleMath::Quaternion invAlignmentQuat = aMissile.projectileData.alignmentQuat;
            invAlignmentQuat.Normalize();
            invAlignmentQuat.Inverse(invAlignmentQuat);

            DirectX::SimpleMath::Vector3 vecForward = aMissile.projectileData.forward;
            vecForward.Normalize();
            vecForward = DirectX::SimpleMath::Vector3::Transform(vecForward, invAlignmentQuat);

            DirectX::SimpleMath::Vector3 targPos = aMissile.guidance.targetPosition;
            m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, targPos, DirectX::Colors::Lime);

            //DirectX::SimpleMath::Vector3 targetOffset = aMissile.projectileData.q.velocity * (timeToTarget * 0.35f);
            DirectX::SimpleMath::Vector3 targetOffset = aMissile.projectileData.q.velocity * (timeToTarget * 1.0f);
            targetOffset = DirectX::SimpleMath::Vector3::Zero;
            //////////////////////////////////////
            // flip timeToTarget so magnitude is increased the closer to target
            const float maxTimeOffset = 45.0f;
            if (timeToTarget > maxTimeOffset)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::Gray);
            }
            else if (timeToTarget < -maxTimeOffset)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::LightGray);
                int testBreak2 = 0;
                testBreak2++;
            }
            else if (timeToTarget < 0.0f)
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::Navy);
                float timeToTargetRatio = (maxTimeOffset + timeToTarget) / maxTimeOffset;
                targetOffset = -aMissile.projectileData.q.velocity * (timeToTargetRatio * 1.0f);
            }
            else
            {
                m_debugData->PushDebugLinePositionIndicator(testPosLocalTargetPos, 300.0f, 0.0f, DirectX::Colors::LightPink);
                float timeToTargetRatio = (maxTimeOffset - timeToTarget) / maxTimeOffset;
                targetOffset = aMissile.projectileData.q.velocity * (timeToTargetRatio * 1.0f);
            }
            m_debugData->DebugPushUILineDecimalNumber("targetOffset.Length() = ", targetOffset.Length(), "");
            //////////////////////////////////////

            targPos -= targetOffset;
            //targPos.y -= (dropDistance3 * 0.2f);

            DirectX::SimpleMath::Vector3 testTargPost = aMissile.guidance.targetPosition - targetOffset;
            m_debugData->PushDebugLinePositionIndicator(testTargPost, 40.0f, 0.0f, DirectX::Colors::White);
            //m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, targPos, DirectX::Colors::Blue);
 
            DirectX::SimpleMath::Vector3 vecToTarget = targPos - aMissile.projectileData.q.position;
            vecToTarget.Normalize();
            m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 1000.0f, 0.0f, DirectX::Colors::Orange);
            vecToTarget = DirectX::SimpleMath::Vector3::Transform(vecToTarget, invAlignmentQuat);
            m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 100.0f, 0.0f, DirectX::Colors::Blue);

            DirectX::SimpleMath::Quaternion q;
            DirectX::SimpleMath::Vector3 a = vecForward.Cross(vecToTarget);
            q.x = a.x;
            q.y = a.y;
            q.z = a.z;
            q.w = sqrt((vecForward.Length() * vecForward.Length()) * (vecToTarget.Length() * vecToTarget.Length())) + vecForward.Dot(vecToTarget);
            q.Normalize();

            //DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
            DirectX::SimpleMath::Quaternion alignmentQuat = aMissile.projectileData.alignmentQuat;
            alignmentQuat.Normalize();
            alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
            alignmentQuat.RotateTowards(q, maxAngle);

            //DirectX::SimpleMath::Vector3 testForward = m_playerVehicle->GetForward();
            DirectX::SimpleMath::Vector3 testForward = aMissile.projectileData.forward;
            testForward = DirectX::SimpleMath::Vector3::Transform(testForward, alignmentQuat);
    
            m_debugData->PushDebugLine(aMissile.projectileData.q.position, testForward, 3500.0f, 1.0f, DirectX::Colors::White);
            aMissile.guidance.heading = testForward;

            /////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////
            //m_debugData->DebugClearUI();

            vecForward = aMissile.projectileData.forward;
            vecForward.Normalize();
            vecForward = DirectX::SimpleMath::Vector3::Transform(vecForward, invAlignmentQuat);

            DirectX::SimpleMath::Vector3 localizedTargetPosition = targPos - aMissile.projectileData.q.position;
            localTargetPos = aMissile.projectileData.q.position - aMissile.guidance.targetDestination;
            localizedTargetPosition = DirectX::SimpleMath::Vector3::Transform(localizedTargetPosition, aMissile.projectileData.inverseAlignmentQuat);
            localTargetPos = DirectX::SimpleMath::Vector3::Transform(localTargetPos, aMissile.projectileData.inverseAlignmentQuat);

            //m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, localizedTargetPosition, DirectX::Colors::Red);
            //m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, localTargetPos, DirectX::Colors::Blue);

            localizedTargetPosition = DirectX::SimpleMath::Vector3::Transform(localizedTargetPosition, aMissile.projectileData.alignmentQuat);
            localTargetPos = DirectX::SimpleMath::Vector3::Transform(localTargetPos, aMissile.projectileData.alignmentQuat);

            m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, localizedTargetPosition, DirectX::Colors::Orange);
            m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, localTargetPos, DirectX::Colors::Purple);

            targPos = aMissile.guidance.targetPosition;
            //m_debugData->PushTestDebugBetweenPoints(aMissile.projectileData.q.position, targPos, DirectX::Colors::Lime);
            //targPos -= targetOffset;

            vecToTarget = targPos - aMissile.projectileData.q.position;
            vecToTarget.Normalize();
            //m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 1000.0f, 0.0f, DirectX::Colors::Orange);
            vecToTarget = DirectX::SimpleMath::Vector3::Transform(vecToTarget, invAlignmentQuat);
            //m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 100.0f, 0.0f, DirectX::Colors::Blue);

            a = vecForward.Cross(vecToTarget);
            q.x = a.x;
            q.y = a.y;
            q.z = a.z;
            q.w = sqrt((vecForward.Length() * vecForward.Length()) * (vecToTarget.Length() * vecToTarget.Length())) + vecForward.Dot(vecToTarget);
            q.Normalize();

            //DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
            alignmentQuat = aMissile.projectileData.alignmentQuat;
            alignmentQuat.Normalize();
            alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
            alignmentQuat.RotateTowards(q, maxAngle);
            //alignmentQuat *= aMissile.projectileData.alignmentQuat;

            //DirectX::SimpleMath::Vector3 testForward = m_playerVehicle->GetForward();
            testForward = aMissile.projectileData.forward;
            testForward = DirectX::SimpleMath::Vector3::Transform(testForward, alignmentQuat);
            
            
            //m_debugData->PushDebugLine(aMissile.projectileData.q.position, testForward, 200.0f, 0.0f, DirectX::Colors::Orange);

            aMissile.guidance.heading = testForward;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Targeting Laser
            //aMissile.guidance.targetLaserAlignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(alignmentQuat);
            //aMissile.guidance.targetLaserAlignment = DirectX::SimpleMath::Matrix::CreateFromQuaternion(q);
            /*
            
            vecToTarget = targPos - aMissile.projectileData.q.position;
            vecToTarget.Normalize();
            //m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 1000.0f, 0.0f, DirectX::Colors::Orange);
            vecToTarget = DirectX::SimpleMath::Vector3::Transform(vecToTarget, invAlignmentQuat);
            //m_debugData->PushDebugLine(aMissile.projectileData.q.position, vecToTarget, 100.0f, 0.0f, DirectX::Colors::Blue);

            a = vecForward.Cross(vecToTarget);
            q.x = a.x;
            q.y = a.y;
            q.z = a.z;
            q.w = sqrt((vecForward.Length() * vecForward.Length()) * (vecToTarget.Length() * vecToTarget.Length())) + vecForward.Dot(vecToTarget);
            q.Normalize();

            //DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_playerVehicle->GetAlignment());
            alignmentQuat = aMissile.projectileData.alignmentQuat;
            alignmentQuat.Normalize();
            alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
            alignmentQuat.RotateTowards(q, maxAngle);             
            */

            /*
            DirectX::SimpleMath::Quaternion laserQuat = DirectX::SimpleMath::Quaternion::Identity;
            laserQuat *= q;
            laserQuat *= aMissile.projectileData.inverseAlignmentQuat;
            */

            vecToTarget = targPos - aMissile.projectileData.q.position;
            vecToTarget.Normalize();
            //vecToTarget = DirectX::SimpleMath::Vector3::Transform(vecToTarget, invAlignmentQuat);
            vecForward = aMissile.projectileData.forward;
            vecForward.Normalize();
            //vecForward = DirectX::SimpleMath::Vector3::Transform(vecForward, invAlignmentQuat);
            a = vecForward.Cross(vecToTarget);
            q.x = a.x;
            q.y = a.y;
            q.z = a.z;
            q.w = sqrt((vecForward.Length() * vecForward.Length()) * (vecToTarget.Length() * vecToTarget.Length())) + vecForward.Dot(vecToTarget);
            q.Normalize();
            alignmentQuat = aMissile.projectileData.alignmentQuat;
            alignmentQuat.Normalize();
            alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
            alignmentQuat.RotateTowards(q, maxAngle);
            //alignmentQuat *= aMissile.projectileData.alignmentQuat;

            //DirectX::SimpleMath::Vector3 testForward = m_playerVehicle->GetForward();
            testForward = aMissile.projectileData.forward;
            testForward = DirectX::SimpleMath::Vector3::Transform(testForward, alignmentQuat);



            DirectX::SimpleMath::Quaternion laserQuat = DirectX::SimpleMath::Quaternion::Identity;
           
            laserQuat *= alignmentQuat;
            //laserQuat *= q;
            //laserQuat *= aMissile.projectileData.alignmentQuat;
            //laserQuat *= aMissile.projectileData.inverseAlignmentQuat;
            //laserQuat *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, Utility::ToRadians(90.0f));

            aMissile.guidance.targetLaserAlignment *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(laserQuat);
        }
    }
}

void FireControl::DrawLaser(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    m_debugData->ToggleDebugOn();
    if (m_isTargetingLaserOn == true)
    {
        float scale = m_playerLaser.distance;
        DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
        updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

        const float testRot = Utility::WrapAngle(m_playerLaser.flickerRot + m_playerLaser.flickerRate);
        m_playerLaser.flickerRot = testRot;
        const float scaleTransOffset = (m_playerLaser.distance) * 0.5f;
        DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));

        float diameterScale = 1.0f;
        if (m_playerLaser.isFlickerTrue == false)
        {
            diameterScale = 0.5f;
        }

        DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(diameterScale, scale, diameterScale));
        DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

        m_playerLaser.worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
        //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
        m_playerLaser.worldBodyMatrix *= scaleMat;
        m_playerLaser.worldBodyMatrix *= scaleTransOffsetMat;
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

        m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());
        m_playerLaser.worldBodyMatrix *= updateMat;


        DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
        DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
        DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

        m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

        DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
        DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
        DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

        //lightDir0 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
        //lightDir1 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
        //lightDir2 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);

        DirectX::SimpleMath::Vector4 color1 = m_playerLaser.laserColor;
        DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        aEffect->SetEmissiveColor(color2);
        aEffect->SetLightSpecularColor(0, color2);
        aEffect->SetLightSpecularColor(1, color2);
        aEffect->SetLightSpecularColor(2, color2);

        aEffect->SetLightDiffuseColor(0, color1);
        aEffect->SetLightDiffuseColor(1, color1);
        aEffect->SetLightDiffuseColor(2, color1);

        aEffect->SetLightDirection(0, lightDir0);
        aEffect->SetLightDirection(1, lightDir1);
        aEffect->SetLightDirection(2, lightDir2);


        //aEffect->EnableDefaultLighting();
        aEffect->SetWorld(m_playerLaser.worldBodyMatrix);
        //aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
        //m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());
        /*
        if (m_playerLaser.isFlickerTrue == true)
        {
            m_playerLaser.isFlickerTrue = false;
            aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
            m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());
        }
        else
        {
            m_playerLaser.isFlickerTrue = true;
            aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
            m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
        }
        */
    }

    DirectX::SimpleMath::Vector3 localTargPos = DirectX::SimpleMath::Vector3::Zero;

    for (unsigned int i = 0; i < m_missileVec.size(); ++i)
    {
        if (m_missileVec[i].guidance.isTargetingLaserOn == true)
        {

            //float scale = m_playerLaser.distance;
            float scale = m_missileVec[i].guidance.targetDistance;
            //DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
            DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
            //updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());
            updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);

            const float testRot = Utility::WrapAngle(m_playerLaser.flickerRot + m_playerLaser.flickerRate);
            //m_playerLaser.flickerRot = testRot;
            //const float scaleTransOffset = (m_playerLaser.distance) * 0.5f;
            const float scaleTransOffset = (m_missileVec[i].guidance.targetDistance) * 0.5f;
            DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));

            float diameterScale = 1.0f;
            if (m_playerLaser.isFlickerTrue == false)
            {
                diameterScale = 0.5f;
            }
            diameterScale = 3.0f;

            DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(diameterScale, scale, diameterScale));
            DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

            DirectX::SimpleMath::Matrix worldBodyMatrix;
            worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
            //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;
            worldBodyMatrix *= scaleMat;
            worldBodyMatrix *= scaleTransOffsetMat;
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
            //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;
            //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
            //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

            //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetLocalizedMuzzlePos());

            DirectX::SimpleMath::Matrix updateMatAlign = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
            DirectX::SimpleMath::Matrix updateMatPos = DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
            //worldBodyMatrix *= updateMatAlign;
            worldBodyMatrix *= updateMatPos;

            //worldBodyMatrix *= updateMatPos;
            //////////////////////////////////////////////////////////////////////////////
            worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            worldBodyMatrix *= scaleMat;
            worldBodyMatrix *= scaleTransOffsetMat;
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));


            //DirectX::SimpleMath::Matrix lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
            DirectX::SimpleMath::Matrix lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
            DirectX::SimpleMath::Matrix testMat = DirectX::SimpleMath::Matrix::CreateRotationX(cos(m_testTimer));
            //worldBodyMatrix *= lookAtMat;
            worldBodyMatrix *= testMat;
            //worldBodyMatrix *= m_missileVec[i].guidance.targetLaserAlignment;

            DirectX::SimpleMath::Matrix createWorldMat = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
            worldBodyMatrix *= createWorldMat;
            ///////////////////////////////////////////////////////////////////////////////

            DirectX::SimpleMath::Vector3 localizedTargetPos = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
            localizedTargetPos = DirectX::SimpleMath::Vector3::Transform(localizedTargetPos, m_missileVec[i].projectileData.inverseAlignmentQuat);
            m_debugData->PushDebugLinePositionIndicator(localizedTargetPos, 200.0f, 0.0f, DirectX::Colors::Olive);
            DirectX::SimpleMath::Vector3 worldTargetPos = localizedTargetPos;
            worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, m_missileVec[i].projectileData.alignmentQuat);
            DirectX::SimpleMath::Matrix worldTargetMat = DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
            worldTargetPos = DirectX::SimpleMath::Vector3::Transform(worldTargetPos, worldTargetMat);
            //worldTargetPos = 
            
            //m_debugData->PushDebugLinePositionIndicator(m_missileVec[i].guidance.targetPosition, 320.0f, 0.0f, DirectX::Colors::Lime);
            //m_debugData->PushDebugLinePositionIndicator(worldTargetPos, 200.0f, 2.0f, DirectX::Colors::Orange);
            worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            worldBodyMatrix *= scaleMat;
            worldBodyMatrix *= scaleTransOffsetMat;
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));

            DirectX::SimpleMath::Vector3 lookAtVec = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
            lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
            lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, lookAtVec, DirectX::SimpleMath::Vector3::UnitY);
            lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, DirectX::SimpleMath::Vector3::UnitY);
            //lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, lookAtVec, m_missileVec[i].projectileData.up);
            worldBodyMatrix *= lookAtMat;
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);

            //m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, DirectX::Colors::Coral);
            lookAtMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
            DirectX::SimpleMath::Matrix testWorldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            testWorldBodyMatrix *= lookAtMat;

            DirectX::SimpleMath::Vector3 testLine = DirectX::SimpleMath::Vector3::UnitX;
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat);
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 300.0f, 0.0f, DirectX::Colors::Fuchsia);

            testLine = DirectX::SimpleMath::Vector3::UnitY;
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat);
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 300.0f, 0.0f, DirectX::Colors::Yellow);

            DirectX::SimpleMath::Matrix createWorldMat2 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
            //DirectX::SimpleMath::Matrix createWorldMat2 = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_missileVec[i].projectileData.right, m_missileVec[i].projectileData.up);
            DirectX::SimpleMath::Matrix lookAtMat2 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
            //DirectX::SimpleMath::Matrix lookAtMat2 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
            testLine = DirectX::SimpleMath::Vector3::UnitZ;
            testLine *= m_missileVec[i].guidance.targetDistance;
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat2);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.targetLaserAlignment);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, createWorldMat2);
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
            m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 400.0f, 0.0f, DirectX::Colors::SkyBlue);
            m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine, DirectX::Colors::Lime);
            m_debugData->DebugPushUILineDecimalNumber("testLine.x = ", testLine.x, "");
            m_debugData->DebugPushUILineDecimalNumber("testLine.y = ", testLine.y, "");
            m_debugData->DebugPushUILineDecimalNumber("testLine.z = ", testLine.z, "");
            m_debugData->PushDebugLinePositionIndicator(testLine, 100.0f, 0.0f, DirectX::Colors::Teal);

            //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.forward, 120.0f, 0.0f, DirectX::Colors::White);

            //DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);
            //DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::UnitY);
            DirectX::SimpleMath::Matrix lookAtMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].projectileData.q.position - m_missileVec[i].guidance.targetPosition, DirectX::SimpleMath::Vector3::UnitY);
            
            testLine = -DirectX::SimpleMath::Vector3::UnitX;
            testLine *= m_missileVec[i].guidance.targetDistance;
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, lookAtMat3);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].guidance.targetLaserAlignment);
            testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, createWorldMat2);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, m_missileVec[i].projectileData.alignmentQuat);
            //testLine = DirectX::SimpleMath::Vector3::Transform(testLine, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
            //m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine, 400.0f, 0.0f, DirectX::Colors::Red);
            //m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine, DirectX::Colors::Orange);
            //m_debugData->PushDebugLinePositionIndicator(testLine, 330.0f, 0.0f, DirectX::Colors::Coral);

            //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), DirectX::SimpleMath::Vector3::UnitZ, 100.0f, 0.0f, DirectX::Colors::White);

            //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(m_missileVec[i].projectileData.q.position, m_missileVec[i].guidance.targetPosition, m_missileVec[i].projectileData.up);

            localizedTargetPos = m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position;
            //localizedTargetPos = m_missileVec[i].projectileData.q.position - m_missileVec[i].guidance.targetPosition;
            //localizedTargetPos = DirectX::SimpleMath::Vector3::Transform(localizedTargetPos, m_missileVec[i].projectileData.inverseAlignmentQuat);

            //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_missileVec[i].guidance.targetPosition - m_missileVec[i].projectileData.q.position, DirectX::SimpleMath::Vector3::UnitY);
            DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localizedTargetPos, DirectX::SimpleMath::Vector3::UnitY);
            //DirectX::SimpleMath::Matrix lookAtMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(localizedTargetPos, DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);

            DirectX::SimpleMath::Vector3 testLine2 = DirectX::SimpleMath::Vector3::UnitX;
            testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, lookAtMat4);
            testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, m_missileVec[i].projectileData.alignmentQuat);
            //testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position));
            testLine2 = DirectX::SimpleMath::Vector3::Transform(testLine2, DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].guidance.targetPosition));
            m_debugData->DebugClearUI();
            m_debugData->PushTestDebugBetweenPoints(m_missileVec[i].projectileData.q.position, testLine2, DirectX::Colors::Orange);
            m_debugData->PushDebugLinePositionIndicator(testLine2, 330.0f, 0.0f, DirectX::Colors::Lime);
            m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, testLine2, 100.0f, 0.0f, DirectX::Colors::White);
            m_debugData->PushDebugLine(m_missileVec[i].projectileData.q.position, m_missileVec[i].projectileData.forward, 50.0f, 0.0f, DirectX::Colors::Red);

            worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            worldBodyMatrix *= scaleMat;
            worldBodyMatrix *= scaleTransOffsetMat;
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
            worldBodyMatrix *= lookAtMat4;
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_missileVec[i].projectileData.alignmentQuat);
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].guidance.targetPosition);
            //worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(m_missileVec[i].projectileData.q.position);
            //////////////////////////////////////////////////////////////////////////////
            
            ///////////////////////////////////////////////////////////////
            // From tank prototype
            DirectX::SimpleMath::Vector3 toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Zero;
            toUseLocalMuzzlePos.y = 0.5f;
            toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Transform(toUseLocalMuzzlePos, m_missileVec[i].projectileData.alignmentQuat);

            //DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(5) - (m_playerVehicle->GetPos() + toUseLocalMuzzlePos);
            DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(m_missileVec[i].guidance.targetID) - (m_missileVec[i].projectileData.q.position + toUseLocalMuzzlePos);
            forwardToTargNorm.Normalize();
            DirectX::SimpleMath::Vector3 rightCross = forwardToTargNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
            DirectX::SimpleMath::Vector3 testUp = rightCross.Cross(forwardToTargNorm);

            //DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_playerVehicle->GetPos() + toUseLocalMuzzlePos, forwardToTargNorm, testUp);
            DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_missileVec[i].projectileData.q.position + toUseLocalMuzzlePos, forwardToTargNorm, testUp);

            worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
            worldBodyMatrix *= scaleMat;
            worldBodyMatrix *= scaleTransOffsetMat;
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));

            worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toUseLocalMuzzlePos);
           
            worldBodyMatrix *= updateMat6;

            //////////////////////////////////////////////////////////////

            DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
            DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
            DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

            m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

            DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
            DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
            DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

            DirectX::SimpleMath::Vector4 color1 = m_playerLaser.laserColor;
            DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f);
            color1 = color2;
            aEffect->SetEmissiveColor(color2);
            aEffect->SetLightSpecularColor(0, color2);
            aEffect->SetLightSpecularColor(1, color2);
            aEffect->SetLightSpecularColor(2, color2);

            aEffect->SetLightDiffuseColor(0, color1);
            aEffect->SetLightDiffuseColor(1, color1);
            aEffect->SetLightDiffuseColor(2, color1);

            aEffect->SetLightDirection(0, lightDir0);
            aEffect->SetLightDirection(1, lightDir1);
            aEffect->SetLightDirection(2, lightDir2);
            aEffect->EnableDefaultLighting();
            aEffect->SetWorld(worldBodyMatrix);

            //aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
            aEffect->SetColorAndAlpha(DirectX::Colors::Purple);
            m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());

            //m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
            aEffect->EnableDefaultLighting();
            m_debugData->DebugClearUI();
            DirectX::SimpleMath::Vector3 decomTrans = DirectX::SimpleMath::Vector3::Zero;
            DirectX::SimpleMath::Vector3 decomScale = DirectX::SimpleMath::Vector3::Zero;
            DirectX::SimpleMath::Quaternion decomQuat = DirectX::SimpleMath::Quaternion::Identity;

            worldBodyMatrix.Decompose(decomScale, decomQuat, decomTrans);
            m_debugData->PushDebugLinePositionIndicator(decomTrans, 350.0f, 0.0f, DirectX::Colors::Aqua);

            DirectX::SimpleMath::Vector3 testAlignVec = DirectX::SimpleMath::Vector3::UnitX;
            testAlignVec = DirectX::SimpleMath::Vector3::Transform(testAlignVec, decomQuat);

            m_debugData->PushDebugLine(m_playerVehicle->GetPos(), testAlignVec, 250.0f, 0.0f, DirectX::Colors::BlueViolet);

            localTargPos = m_missileVec[i].guidance.targetPosition;
        }
    }


    

    //float scale = m_playerLaser.distance;
    float scale = DirectX::SimpleMath::Vector3::Distance(m_npcController->GetNpcPos(5), m_playerVehicle->GetPos());
    m_debugData->DebugPushUILineDecimalNumber("scale ", scale, "");
    DirectX::SimpleMath::Matrix updateMat = m_playerVehicle->GetAlignment();
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

    const float testRot = Utility::WrapAngle(m_playerLaser.flickerRot + m_playerLaser.flickerRate);
    //m_playerLaser.flickerRot = testRot;
    //const float scaleTransOffset = (m_playerLaser.distance) * 0.5f;
    const float scaleTransOffset = (scale) * 0.5f;
    DirectX::SimpleMath::Matrix scaleTransOffsetMat = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -scaleTransOffset, 0.0f));

    float diameterScale = 1.0f;
    if (m_playerLaser.isFlickerTrue == false)
    {
        //diameterScale = 0.5f;
    }

    DirectX::SimpleMath::Matrix scaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(diameterScale, scale, diameterScale));
    DirectX::SimpleMath::Vector3 posOffset = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f);

    localTargPos = m_npcController->GetNpcPos(5);
    localTargPos -= m_playerVehicle->GetPos();
    //localTargPos = m_playerVehicle->GetPos() - m_npcController->GetNpcPos(5);
    localTargPos = DirectX::SimpleMath::Vector3::Transform(localTargPos, m_playerVehicle->GetAlignmentInverse());
    DirectX::SimpleMath::Vector3 worldTargPos = localTargPos;
    worldTargPos = DirectX::SimpleMath::Vector3::Transform(worldTargPos, m_playerVehicle->GetAlignment());
    //worldTargPos -= m_npcController->GetNpcPos(5);
    worldTargPos += m_playerVehicle->GetPos();
    m_debugData->PushDebugLinePositionIndicator(worldTargPos, 400.0f, 0.0f, DirectX::Colors::Blue);

    
    DirectX::SimpleMath::Vector3 zTargPos = m_npcController->GetNpcPos(5);
    DirectX::SimpleMath::Vector3 zPlayerPos = m_playerVehicle->GetPos();
    DirectX::SimpleMath::Vector3 zWorldTargPos = worldTargPos;
    float zDistance1 = DirectX::SimpleMath::Vector3::Distance(zTargPos, DirectX::SimpleMath::Vector3::Zero);
    float zDistance2 = DirectX::SimpleMath::Vector3::Distance(zTargPos, worldTargPos);
    float zDistance3 = DirectX::SimpleMath::Vector3::Distance(worldTargPos, DirectX::SimpleMath::Vector3::Zero);
    float zDistance4 = DirectX::SimpleMath::Vector3::Distance(zPlayerPos, DirectX::SimpleMath::Vector3::Zero);
    float zDistance5 = DirectX::SimpleMath::Vector3::Distance(zWorldTargPos, zTargPos);

    DirectX::SimpleMath::Matrix updateMat2 = m_playerVehicle->GetAlignment();
    updateMat2 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

    //DirectX::SimpleMath::Matrix updateMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, localTargPos, DirectX::SimpleMath::Vector3::UnitY);
    //DirectX::SimpleMath::Matrix updateMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, -localTargPos, DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Matrix updateMat3 = DirectX::SimpleMath::Matrix::CreateLookAt(localTargPos, DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);
    //updateMat3 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

    DirectX::SimpleMath::Matrix updateMat4 = m_playerVehicle->GetAlignment();
    //DirectX::SimpleMath::Matrix updateMat4 = m_playerVehicle->GetAlignmentInverse();
    //DirectX::SimpleMath::Matrix updateMat4 = DirectX::SimpleMath::Matrix::Identity;
    updateMat4 *= DirectX::SimpleMath::Matrix::CreateTranslation(m_playerVehicle->GetPos());

    DirectX::SimpleMath::Matrix updateMat5 = DirectX::SimpleMath::Matrix::CreateWorld(m_playerVehicle->GetPos(), m_playerVehicle->GetForward(), m_playerVehicle->GetVehicleUp());

    DirectX::SimpleMath::Vector3 toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Zero;
    toUseLocalMuzzlePos.y = 0.5f;
    toUseLocalMuzzlePos = DirectX::SimpleMath::Vector3::Transform(toUseLocalMuzzlePos, m_playerVehicle->GetAlignment());

    DirectX::SimpleMath::Vector3 forwardToTargNorm = m_npcController->GetNpcPos(5) - (m_playerVehicle->GetPos() + toUseLocalMuzzlePos);
    forwardToTargNorm.Normalize();
    DirectX::SimpleMath::Vector3 rightCross = forwardToTargNorm.Cross(DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Vector3 testUp = rightCross.Cross(forwardToTargNorm);

    DirectX::SimpleMath::Matrix updateMat6 = DirectX::SimpleMath::Matrix::CreateWorld(m_playerVehicle->GetPos() + toUseLocalMuzzlePos, forwardToTargNorm, testUp);
    //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), rightCross, 1000.0f, 0.0f, DirectX::Colors::Blue);
    //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), forwardToTargNorm, 1000.0f, 0.0f, DirectX::Colors::OrangeRed);
    //m_debugData->PushDebugLine(m_playerVehicle->GetPos(), testUp, 1000.0f, 0.0f, DirectX::Colors::Yellow);
    DirectX::SimpleMath::Matrix updateMat7 = DirectX::SimpleMath::Matrix::CreateLookAt(localTargPos, DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);
    DirectX::SimpleMath::Vector3 localMuzzlePos = m_playerVehicle->GetLocalizedMuzzlePos();

    m_playerLaser.worldBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(testRot);
    m_playerLaser.worldBodyMatrix *= scaleMat;
    m_playerLaser.worldBodyMatrix *= scaleTransOffsetMat;
    m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(posOffset);
    m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_playerVehicle->GetWeaponPitch());
    //m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(m_playerVehicle->GetTurretYaw());

    m_playerLaser.worldBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(toUseLocalMuzzlePos);
    //m_playerLaser.worldBodyMatrix *= updateMat5;
    m_playerLaser.worldBodyMatrix *= updateMat6;

    DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
    DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
    DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);

    m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

    DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
    DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
    DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

    //lightDir0 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    //lightDir1 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    //lightDir2 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);

    DirectX::SimpleMath::Vector4 color1 = m_playerLaser.laserColor;
    DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    aEffect->SetEmissiveColor(color2);
    aEffect->SetLightSpecularColor(0, color2);
    aEffect->SetLightSpecularColor(1, color2);
    aEffect->SetLightSpecularColor(2, color2);

    aEffect->SetLightDiffuseColor(0, color1);
    aEffect->SetLightDiffuseColor(1, color1);
    aEffect->SetLightDiffuseColor(2, color1);

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);


    //aEffect->EnableDefaultLighting();
    aEffect->SetWorld(m_playerLaser.worldBodyMatrix);
    //aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
    //m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());

    aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
    m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());

    /*
    if (m_playerLaser.isFlickerTrue == true)
    {
        m_playerLaser.isFlickerTrue = false;
        aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
        m_playerLaser.laserShape->Draw(aEffect.get(), aInputLayout.Get());
    }
    else
    {
        m_playerLaser.isFlickerTrue = true;
        aEffect->SetColorAndAlpha(m_playerLaser.laserColor);
        m_playerLaser.laserShape2->Draw(aEffect.get(), aInputLayout.Get());
    }
    */

    m_debugData->ToggleDebugOff();
}
