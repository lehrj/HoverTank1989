#include "pch.h"
#include "FireControl.h"

FireControl::FireControl()
{
}

void FireControl::CheckCollisions()
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        bool isHitTrue = m_npcController->CheckProjectileCollisions(m_projectileVec[i].collisionData);
        if (isHitTrue == true)
        {
            m_projectileVec[i].liveTimeTick--;
            if (m_projectileVec[i].liveTimeTick <= 0)
            {
                m_projectileVec[i].isCollisionTrue = true;
            }
        }
        else if (m_projectileVec[i].time > m_maxProjectileLifeTime)
        {
            m_projectileVec[i].isCollisionTrue = true;
        }
        else if (m_environment->GetIsPosInPlay(m_projectileVec[i].q.position, 0.0f) == false)
        {
            m_projectileVec[i].isCollisionTrue = true;
        }   
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

void FireControl::DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    //m_heliModel.mainRotorHaloShape->Draw(m_heliModel.mainRotorHaloMatrix, aView, aProj, m_heliModel.bodyColor);
    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_heli.q.position, -m_heli.right, m_heli.up);
    //m_heliModel.cannonBarrelMatrix = m_heliModel.scaleCannonBarrelMatrix;
    //m_heliModel.cannonBarrelMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(0.0f));
    //m_heliModel.cannonBarrelMatrix *= m_heliModel.localCannonBarrelMatrix;
    //m_heliModel.cannonBarrelMatrix *= updateMat;
    DirectX::SimpleMath::Vector4 projectileColor(1.0f, 1.0f, 1.0f, 1.0f);
    //AmmoData projData = m_ballAmmo;
    //ProjectileModel projAmmo = m_ballAmmo->ammoModel;
    //projAmmo.projectileShape = m_ballAmmo.ammoModel.projectileShape;

    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        DirectX::SimpleMath::Matrix projMat = m_ballAmmoStruct.ammoModel.localProjectileMatrix;
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
        m_ballAmmoStruct.ammoModel.projectileShape->Draw(projMat, aView, aProj, projectileColor);
    }
}

void FireControl::FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    /*
    ProjectileData firedProjectile;
    firedProjectile.projectileAmmoType = AmmoType::AMMOTYPE_BALL01;
    firedProjectile.q.position = aLaunchPos;    
    firedProjectile.q.velocity = (m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.collisionSphere.Center = aLaunchPos;
    
    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);
    */


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
    firedProjectile.projectileAmmoType = AmmoType::AMMOTYPE_BALL01;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.collisionSphere.Center = aLaunchPos;
    firedProjectile.collisionSphere.Radius = firedAmmo.collisionSphere.Radius;
    firedProjectile.isCollisionTrue = false;
    firedProjectile.liveTimeTick = firedAmmo.tickDownCounter;

    // collision data
    firedProjectile.collisionData.collisionSphere = firedProjectile.collisionSphere;
    firedProjectile.collisionData.velocity = firedProjectile.q.velocity;
    firedProjectile.collisionData.mass = firedAmmo.mass;
    firedProjectile.collisionData.isCollisionTrue = firedProjectile.isCollisionTrue;

    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);
}

void FireControl::FireProjectileShotGun(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
    /*
    ProjectileData firedProjectile;
    firedProjectile.projectileAmmoType = AmmoType::AMMOTYPE_BALL01;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);
    */
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
    firedProjectile.projectileAmmoType = AmmoType::AMMOTYPE_BALL01;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.collisionSphere.Center = aLaunchPos;
    firedProjectile.collisionSphere.Radius = firedAmmo.collisionSphere.Radius;
    firedProjectile.isCollisionTrue = false;
    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);

    DirectX::SimpleMath::Vector3 up = aLaunchDirectionForward;

    up = aLaunchDirectionRight.Cross(aLaunchDirectionForward);
    ProjectileData shot = firedProjectile;
    float chokeAngle = 5.0f;
    float upAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    float rightAxisRot = static_cast <float> ((rand()) / (static_cast <float> (RAND_MAX / chokeAngle)) - (0.5f * chokeAngle));
    DirectX::SimpleMath::Vector3 shotCenterAim = m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward;
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

void FireControl::FireWeapon(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity)
{
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
    firedProjectile.projectileAmmoType = AmmoType::AMMOTYPE_BALL01;
    firedProjectile.q.position = aLaunchPos;
    firedProjectile.q.velocity = (m_ballAmmoStruct.ammoData.launchVelocity * aLaunchDirectionForward) + aLauncherVelocity;
    firedProjectile.collisionSphere.Center = aLaunchPos;
    firedProjectile.collisionSphere.Radius = firedAmmo.collisionSphere.Radius;
    firedProjectile.isCollisionTrue = false;
    firedProjectile.time = 0.0f;
    m_projectileVec.push_back(firedProjectile);
}

void FireControl::InitializeAmmoStruct(AmmoStruct& aAmmo)
{
    aAmmo.ammoData.ammoType = AmmoType::AMMOTYPE_BALL01;
    aAmmo.ammoData.airDensity = 1.25f;
    aAmmo.ammoData.baseDamage = 1.0f;
    aAmmo.ammoData.dragCoefficient = 0.3f;
    aAmmo.ammoData.gravity = -9.8f;
    aAmmo.ammoData.launchVelocity = 85.0f;
    aAmmo.ammoData.length = 1.0f;
    aAmmo.ammoData.mass = 0.2f;
    aAmmo.ammoData.surfaceArea = 0.15f;
    aAmmo.ammoData.radius = 0.15f;
    aAmmo.ammoData.tickDownCounter = 5;
    aAmmo.ammoData.collisionSphere.Radius = aAmmo.ammoData.radius;
    aAmmo.ammoData.collisionSphere.Center = DirectX::SimpleMath::Vector3::Zero;
}

void FireControl::InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
    const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection, 
    Environment const* aEnvironment)
{
    m_projectileVec.clear();
    m_environment = aEnvironment;
    InitializeAmmoStruct(m_ballAmmoStruct);
    InitializeProjectileModelStruct(aContext, m_ballAmmoStruct);
    InitializeLauncherData(m_launcherData, aLaunchPos, aLaunchDirection);
}

void FireControl::InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection)
{
    aLauncher.launchDirectionNorm = aDirection;
    //aLauncher.launchVelocity = 10.0f;
    aLauncher.launcherPosition = aPosition;
    aLauncher.reloadCoolDown = 3.0f;
    aLauncher.coolDownTimer = 0.0f;
}

void FireControl::InitializeProjectileModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo)
{
    const float ammoSize = aAmmo.ammoData.radius;
    const float ammoLength = aAmmo.ammoData.length;
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), ammoLength, ammoSize);
    //aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), ammoSize, ammoLength);
    aAmmo.ammoModel.projectileShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), ammoSize);
   
    aAmmo.ammoModel.projectileMatrix = DirectX::SimpleMath::Matrix::Identity;
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 9.0f, 1.0f));
    aAmmo.ammoModel.projectileMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    aAmmo.ammoModel.localProjectileMatrix = aAmmo.ammoModel.projectileMatrix;
}

void FireControl::RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ)
{
    //  Compute the intermediate values of the 
    //  dependent variables.
    ProjectileMotion newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 gravForce = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    velocityUpdate += gravForce;

    const float mass = aProjectile->collisionData.mass;

    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void FireControl::RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta)
{
    //  Define a convenience variables
    //const float numEqns = static_cast<float>(aHeli->numEqns);
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
    aProjectile->time = aProjectile->time + aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    q.velocity += velocityUpdate;
    q.position += posUpdate;
    aProjectile->q.velocity = q.velocity;
    //aProjectile->q.velocity = DirectX::SimpleMath::Vector3::Zero;
    aProjectile->q.position = q.position;
    aProjectile->collisionData.collisionSphere.Center = q.position;
    aProjectile->collisionData.velocity = q.velocity;
}

void FireControl::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

//void FireControl::SetNPCController(NPCController* aNPCController)
void FireControl::SetNPCController(std::shared_ptr<NPCController> aNPCController)
{
    m_npcController = aNPCController;
}

void FireControl::UpdateProjectileVec(double aTimeDelta)
{
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        RungeKutta4(&m_projectileVec[i], aTimeDelta);
    }

    CheckCollisions();
    int deleteCount = 0;
    for (unsigned int i = 0; i < m_projectileVec.size(); ++i)
    {
        /*
        if (m_projectileVec[i].time > 20.02f)
        {
            //DeleteProjectileFromVec(i);
            m_projectileVec[i].isCollisionTrue == true;
        }

        //float altitude = m_environment->GetTerrainHeightAtPos(m_projectileVec[i].q.position);
        if (m_projectileVec[i].q.position.y < m_environment->GetTerrainHeightAtPos(m_projectileVec[i].q.position))
        {
            //DeleteProjectileFromVec(i);
            m_projectileVec[i].isCollisionTrue == true;
        }
        */
        if (m_projectileVec[i].isCollisionTrue == true)
        {
            deleteCount++;
            DeleteProjectileFromVec(i);
        }
    }
}