#pragma once
#include "Environment.h"

enum class AmmoType
{
    AMMOTYPE_BALL01,
};

struct LauncherData
{
    DirectX::SimpleMath::Vector3 launchDirectionNorm;
    //float launchVelocity;
    DirectX::SimpleMath::Vector3 launcherPosition;
    float reloadCoolDown;
    float coolDownTimer;
};

struct ProjectileMotion
{
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 velocity;
};

struct ProjectileModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    projectileShape;
    DirectX::SimpleMath::Matrix projectileMatrix;
    DirectX::SimpleMath::Matrix localProjectileMatrix;
};

struct AmmoData
{
    AmmoType ammoType;
    float   airDensity;
    float   dragCoefficient;
    float   gravity;
    float   launchVelocity;
    float   mass;
    float   surfaceArea;
    float   radius;
    ProjectileModel ammoModel;
};

struct ProjectileData
{
    AmmoType projectileAmmoType;
    //AmmoData& const ammo;
    ProjectileMotion q;
    float time;
};

class FireControl
{
public:
    void DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirection, Environment const* aEnvironment);
    void FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileShotGun(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void UpdateProjectileVec(double aTimeDelta);
private:
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void InitializeAmmo(AmmoData& aAmmo);
    void InitializeProjectileModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoData& aModel);
    void InitializeProjectileData(const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aVelocity);
    void InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection);
    
    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);

    

    Environment const* m_environment;
    LauncherData m_launcherData;

    AmmoData m_ballAmmo;

    std::vector<ProjectileData> m_projectileVec;
};

