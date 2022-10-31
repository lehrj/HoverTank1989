#include "pch.h"
#include "FireControl.h"


void FireControl::CheckCollisions()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        unsigned int vehicleHitId = 0;
        bool isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData, vehicleHitId);
        if (isHitTrue == true)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
            {
                CreateExplosion(m_projectileVec[i].q.position, ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE, static_cast<unsigned int>(vehicleHitId));
            }
            m_projectileVec[i].isCollisionTrue = true;
            m_projectileVec[i].liveTimeTick--;
            if (m_projectileVec[i].liveTimeTick <= 0)
            {
                m_projectileVec[i].isDeleteTrue = true;
            }
        }
        else if (m_projectileVec[i].time > m_maxProjectileLifeTime)
        {
            m_projectileVec[i].isDeleteTrue = true;
        }
        else if (m_environment->GetIsPosInPlay(m_projectileVec[i].q.position) == false)
        {
            if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
            {
                CreateExplosion(m_projectileVec[i].q.position, ExplosionType::EXPLOSIONTYPE_NONVEHICLE, -1);
            }
            m_projectileVec[i].isDeleteTrue = true;
        }   
    }
}

void FireControl::CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, ExplosionType aExplosionType, const int aVehicleId)
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
    else
    {
        createdExplosion.explosionType = ExplosionType::EXPLOSIONTYPE_NONVEHICLE;
        createdExplosion.vehicleExplosionID = -1;
    }
    m_explosionStruct.explosionVec.push_back(createdExplosion);
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
    DirectX::SimpleMath::Vector3 up = velocityNorm.Cross(-right);
    right = velocityNorm.Cross(up);

    const int mirvCount = 12;
    const float chokeAngle = 35.0f;
    const float deployAnglePitch = 35.0f;
    const float deployAngleYaw = 35.0f;

    const float deployAnglePitchMin = -30.0f;
    const float deployAnglePitchMax = 3.0f;
    const float deployAngleYawMin = -20.0f;    
    const float deployAngleYawMax = 20.0f;

    const float velocityBoostMod = 2.0f;
    const float redirectVelocityDownAngle = 10.0f;
    for (int i = 0; i < mirvCount; i++)
    {
        ProjectileData mirv = firedProjectile;

        float upAxisRot = deployAngleYawMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (deployAngleYawMax - deployAngleYawMin)));
        float rightAxisRot = deployAnglePitchMin + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (deployAnglePitchMax - deployAnglePitchMin)));

        DirectX::SimpleMath::Matrix upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
        DirectX::SimpleMath::Matrix rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(right, Utility::ToRadians(rightAxisRot));

        mirv.q.velocity = DirectX::SimpleMath::Vector3::Transform(mirv.q.velocity, upAxisRotMat);
        mirv.q.velocity = DirectX::SimpleMath::Vector3::Transform(mirv.q.velocity, rightAxisRotMat);
        mirv.q.velocity *= velocityBoostMod;
        m_projectileVec.push_back(mirv);
    }

    aProjectile.isDeleteTrue = true;
}

void FireControl::DrawExplosion(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
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

void FireControl::DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        //DirectX::SimpleMath::Matrix projMat = m_ballAmmoStruct.ammoModel.localProjectileMatrix;
        DirectX::SimpleMath::Matrix projMat = m_ammoMachineGun.ammoModel.localProjectileMatrix;
        //
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
        //
        projMat *= DirectX::SimpleMath::Matrix::CreateTranslation(m_projectileVec[i].q.position);

        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_BALL01)
        {
            m_ballAmmoStruct.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_CANNON)
        {
            m_ammoCannon.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_EXPLOSIVE)
        {
            m_ammoExplosive.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MACHINEGUN)
        {
            m_ammoMachineGun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_MIRV)
        {
            m_ammoMirv.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
        if (m_projectileVec[i].ammoData.ammoType == AmmoType::AMMOTYPE_SHOTGUN)
        {
            m_ammoShotgun.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
        }
    }
}

void FireControl::FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    if (m_isCoolDownActive == false)
    {
        m_isCoolDownActive = true;
        AmmoData firedAmmo;
        if (aAmmoType == AmmoType::AMMOTYPE_BALL01)
        {
            firedAmmo = m_ballAmmoStruct.ammoData;
        }
        else
        {
            firedAmmo = m_ballAmmoStruct.ammoData;
        }

        ProjectileData firedProjectile;
        firedProjectile.ammoData = firedAmmo;
        firedProjectile.q.position = aLaunchPos;
        firedProjectile.q.velocity = (firedProjectile.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
        firedProjectile.isCollisionTrue = false;
        firedProjectile.isDeleteTrue = false;
        firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

        // collision data
        firedProjectile.collisionData.collisionModifier = firedProjectile.ammoData.impactModifier;
        firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
        firedProjectile.collisionData.mass = firedAmmo.mass;
        firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

        firedProjectile.time = 0.0f;
        m_projectileVec.push_back(firedProjectile);
    }
}

void FireControl::FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    AmmoData firedAmmo = m_ammoExplosive.ammoData;

    ProjectileData firedProjectile;
    firedProjectile.ammoData = firedAmmo;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ammoExplosive.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.isCollisionTrue = false;
    firedProjectile.isDeleteTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

    // collision data
    firedProjectile.collisionData.collisionModifier = firedProjectile.ammoData.impactModifier;
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

    firedProjectile.time = 0.0f;

    firedProjectile.isMidAirDeployAvailable = false;

    m_projectileVec.push_back(firedProjectile);
}

void FireControl::FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    AmmoData firedAmmo = m_ammoMirv.ammoData;

    ProjectileData firedProjectile;
    firedProjectile.ammoData = firedAmmo;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ammoMirv.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.isCollisionTrue = false;
    firedProjectile.isDeleteTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

    // collision data
    firedProjectile.collisionData.collisionModifier = firedProjectile.ammoData.impactModifier;
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

    firedProjectile.time = 0.0f;

    firedProjectile.isMidAirDeployAvailable = true;
    firedProjectile.isFuseTriggered = false;

    m_projectileVec.push_back(firedProjectile);
}

void FireControl::FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    AmmoData firedAmmo = m_ammoShotgun.ammoData;

    ProjectileData firedProjectile;
    firedProjectile.ammoData = firedAmmo;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;

    firedProjectile.isCollisionTrue = false;
    firedProjectile.isDeleteTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;
    firedProjectile.time = 0.0f;

    // collision data
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;
   
    m_projectileVec.push_back(firedProjectile);

    DirectX::SimpleMath::Vector3 up = aLaunchDirectionForward;

    up = aLaunchDirectionRight.Cross(aLaunchDirectionForward);
    ProjectileData shot = firedProjectile;
    float chokeAngle = 5.0f;
    float upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    float rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    DirectX::SimpleMath::Vector3 shotCenterAim = m_ammoShotgun.ammoData.launchVelocity * aLaunchDirectionForward;
    DirectX::SimpleMath::Matrix upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    DirectX::SimpleMath::Matrix rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 2

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 3

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 4

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 5

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 6

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 7

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 8

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 9

    upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    upAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(up, Utility::ToRadians(upAxisRot));
    rightAxisRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aLaunchDirectionRight, Utility::ToRadians(rightAxisRot));
    shot.q.velocity = shotCenterAim;
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, upAxisRotMat);
    shot.q.velocity = DirectX::SimpleMath::Vector3::Transform(shot.q.velocity, rightAxisRotMat);
    shot.q.velocity += aLauncherVelocity;
    m_projectileVec.push_back(shot);
    // 10
}

void FireControl::InitializeAmmo(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_BALL01;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.0f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 4.0f;
    aAmmo.ammoData.launchVelocity = 135.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 45.0f;    
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;   
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeAmmoCannon(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_BALL01;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.2f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 4.0f;
    aAmmo.ammoData.launchVelocity = 135.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 45.0f;
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeAmmoExplosive(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_EXPLOSIVE;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.7f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 4.0f;
    aAmmo.ammoData.launchVelocity = 105.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 45.0f;
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeAmmoMachineGun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MACHINEGUN;
    aAmmo.ammoData.baseDamage = 0.5f;
    aAmmo.ammoData.cooldown = 0.02f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 1.0f;
    aAmmo.ammoData.launchVelocity = 145.0f;
    aAmmo.ammoData.length = 0.4f;
    aAmmo.ammoData.mass = 45.0f;
    aAmmo.ammoData.radius = 0.12f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeAmmoMirv(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_MIRV;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.9f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 4.0f;
    aAmmo.ammoData.launchVelocity = 55.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 45.0f;
    aAmmo.ammoData.radius = 0.2f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeAmmoShotgun(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_BALL01;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.cooldown = 1.3f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.impactDurration = 0.4f;
    aAmmo.ammoData.impactModifier = 4.0f;
    aAmmo.ammoData.launchVelocity = 135.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 45.0f;
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.frontSurfaceArea = Utility::GetPi() * (aAmmo.ammoData.radius * aAmmo.ammoData.radius);
    aAmmo.ammoData.tickDownCounter = 1;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
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
    aExplosionData.maxRadius = 10.f;
    aExplosionData.position = DirectX::SimpleMath::Vector3::Zero;
    m_explosionStruct.explosionShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), -1.0f);
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

    m_explosionStruct.maxExplosionForce = 10000000.0f;
    m_explosionStruct.maxExplosionImpactRadius = aExplosionData.maxRadius * 2.0f;

    m_explosionStruct.explosionVec.clear();
}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
    const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection, 
    Environment const* aEnvironment)
{   
    m_explosionStruct.explosionToPushVec.clear();
    m_projectileVec.clear();
    m_environment = aEnvironment;
    InitializeAmmo(m_ballAmmoStruct);
    InitializeAmmoCannon(m_ammoCannon);
    InitializeAmmoExplosive(m_ammoExplosive);
    InitializeAmmoMachineGun(m_ammoMachineGun);
    InitializeAmmoMirv(m_ammoMirv);
    InitializeAmmoShotgun(m_ammoShotgun);
    InitializeExplosionData(aContext, m_explosionStruct.explosionRefData);
    InitializeProjectileModel(aContext, m_ballAmmoStruct);
    InitializeProjectileModelCannon(aContext, m_ammoCannon);
    InitializeProjectileModelExplosive(aContext, m_ammoExplosive);
    InitializeProjectileModelMachineGun(aContext, m_ammoMachineGun);
    InitializeProjectileModelMirv(aContext, m_ammoMirv);
    InitializeProjectileModelShotgun(aContext, m_ammoShotgun);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeProjectileModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize); 
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMachineGun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoDiameter = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoDiameter, ammoLength);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelMirv(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
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
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (frontDragResistance);

    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec() * mass;
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    velocityUpdate += airResistance;
    velocityUpdate += gravForce;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
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

void FireControl::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void FireControl::SetNPCController(std::shared_ptr<NPCController> aNPCController)
{
    m_npcController = aNPCController;
}

void FireControl::UpdateFireControl(double aTimeDelta)
{
    if (m_isCoolDownActive == true)
    {
        m_testCoolDownTimer += static_cast<float>(aTimeDelta);
        if (m_testCoolDownTimer >= m_testCoolDown)
        {
            m_testCoolDownTimer = 0.0f;
            m_isCoolDownActive = false;
        }
    }


    m_testTimer += static_cast<float>(aTimeDelta);
    UpdateProjectileVec(aTimeDelta);
    UpdateExplosionVec(aTimeDelta);
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

            if (m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL)
            {
                const DirectX::SimpleMath::Vector3 updatedPos = m_npcController->GetNpcPos(m_explosionStruct.explosionVec[i].vehicleExplosionID);
                m_explosionStruct.explosionVec[i].position = updatedPos;
                m_explosionStruct.explosionVec[i].collisionSphere.Center = updatedPos;
                m_explosionStruct.explosionVec[i].localExplosionMatrix = DirectX::SimpleMath::Matrix::CreateWorld(updatedPos, DirectX::SimpleMath::Vector3::UnitX, DirectX::SimpleMath::Vector3::UnitY);
                radius *= m_explosionStruct.internalVehicleExplosionRadiusMod;
            }

            if(m_explosionStruct.explosionVec[i].explosionType == ExplosionType::EXPLOSIONTYPE_VEHICLESURFACE)
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

            m_explosionStruct.explosionVec[i].explosionMatrix0 = DirectX::SimpleMath::Matrix::CreateScale(radius);
            m_explosionStruct.explosionVec[i].explosionMatrix0 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;
            
            m_explosionStruct.explosionVec[i].collisionSphere.Radius = radius * 1.0f;

            DirectX::SimpleMath::Vector4 currentColor = m_explosionStruct.explosionVec[i].explosionEndColor - m_explosionStruct.explosionVec[i].explosionStartColor;
            currentColor *= ratio;
            m_explosionStruct.explosionVec[i].explosionCurrentColor = currentColor + m_explosionStruct.explosionVec[i].explosionStartColor;
                  
            const float scatterDistanceMod = 0.15f * radius;
            float scatterVal1 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 10.0f);
            m_explosionStruct.explosionVec[i].color1 = DirectX::SimpleMath::Vector4((abs(scatterVal1 * 0.5f) + 0.5f), (abs(scatterVal1 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance1 = scatterVal1 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans1(scatterDistance1, scatterDistance1, scatterDistance1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 = DirectX::SimpleMath::Matrix::CreateScale(radius *scatterVal1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans1);
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix1 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal2 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 11.0f);
            m_explosionStruct.explosionVec[i].color2 = DirectX::SimpleMath::Vector4((abs(scatterVal2 * 0.5f) + 0.5f), (abs(scatterVal2 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance2 = scatterVal2 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans2(-scatterDistance2, scatterDistance2, scatterDistance2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans2);
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix2 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal3 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 12.0f);
            m_explosionStruct.explosionVec[i].color3 = DirectX::SimpleMath::Vector4((abs(scatterVal3 * 0.5f) + 0.5f), (abs(scatterVal3 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance3 = scatterVal3 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans3(scatterDistance3, -scatterDistance3, scatterDistance3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans3);
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix3 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal4 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 13.0f);
            m_explosionStruct.explosionVec[i].color4 = DirectX::SimpleMath::Vector4((abs(scatterVal4 * 0.5f) + 0.5f), (abs(scatterVal4 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance4 = scatterVal4 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans4(scatterDistance4, scatterDistance4, -scatterDistance4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans4);
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix4 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal5 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 14.0f);
            m_explosionStruct.explosionVec[i].color5 = DirectX::SimpleMath::Vector4((abs(scatterVal5 * 0.5f) + 0.5f), (abs(scatterVal5 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance5 = scatterVal5 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans5(-scatterDistance5, scatterDistance5, -scatterDistance5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans5);
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix5 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal6 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 15.0f);
            m_explosionStruct.explosionVec[i].color6 = DirectX::SimpleMath::Vector4((abs(scatterVal6 * 0.5f) + 0.5f), (abs(scatterVal6 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance6 = scatterVal6 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans6(scatterDistance6, -scatterDistance6, -scatterDistance6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans6);
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix6 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal7 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 16.0f);
            m_explosionStruct.explosionVec[i].color7 = DirectX::SimpleMath::Vector4((abs(scatterVal7 * 0.5f) + 0.5f), (abs(scatterVal7 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance7 = scatterVal7 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans7(-scatterDistance7, -scatterDistance7, scatterDistance7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans7);
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix7 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            float scatterVal8 = cos(static_cast<float>(m_explosionStruct.explosionVec[i].currentDuration) * 17.0f);
            m_explosionStruct.explosionVec[i].color8 = DirectX::SimpleMath::Vector4((abs(scatterVal8 * 0.5f) + 0.5f), (abs(scatterVal8 * 0.5f)), 0.0f, 1.0f);
            float scatterDistance8 = scatterVal8 * scatterDistanceMod;
            DirectX::SimpleMath::Vector3 scatterTrans8(-scatterDistance8, -scatterDistance8, -scatterDistance8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 = DirectX::SimpleMath::Matrix::CreateScale(radius * scatterVal8);           
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(scatterTrans8);
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].rotationVariationMatrix;
            m_explosionStruct.explosionVec[i].explosionMatrix8 *= m_explosionStruct.explosionVec[i].localExplosionMatrix;

            bool isNpcCollisionTrue = m_npcController->CheckExplosionCollisions(m_explosionStruct.explosionVec[i].collisionSphere);
            if (isNpcCollisionTrue == true)
            {
                // to do : add collision detonation to vehicle
            }
        }
    }

    for (unsigned int i = 0; i < m_explosionStruct.explosionToPushVec.size(); ++i)
    {
        CreateExplosion(get<0>(m_explosionStruct.explosionToPushVec[i]), ExplosionType::EXPLOSIONTYPE_VEHICLEINTERNAL, get<1>(m_explosionStruct.explosionToPushVec[i]));
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
        const float fuseDelay = 0.7f;
        aProjectile.fuseTimer = fuseDelay;
    }
}

void FireControl::UpdateProjectileVec(double aTimeDelta)
{
    CheckCollisions();
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