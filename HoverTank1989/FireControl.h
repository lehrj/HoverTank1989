#pragma once
#include "Environment.h"
#include "NPCController.h"
#include "Utility.h"

enum class AmmoType
{
    AMMOTYPE_BALL01,
    AMMOTYPE_SHOTGUNBALL01,
    AMMOTYPE_EXPLOSIVE,
};

struct LauncherData
{
    DirectX::SimpleMath::Vector3 launchDirectionNorm;
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
    float   baseDamage;
    float   dragCoefficient;
    float   impactDurration;
    float   impactModifier;
    float   launchVelocity;
    float   length;
    float   mass;
    float   frontSurfaceArea;
    float   radius;
    int tickDownCounter;  // value to tick down as collisions occure, to be used for penetration and ricochet 

    DirectX::BoundingSphere collisionSphere;
};

struct AmmoStruct
{
    AmmoData ammoData;
    ProjectileModel ammoModel;
};

struct ProjectileData
{
    AmmoData ammoData;
    ProjectileMotion q;
    DirectX::BoundingSphere collisionSphere;
    Utility::CollisionData  collisionData;
    bool isCollisionTrue;
    bool isDeleteTrue;
    float time;
    int liveTimeTick;
};



class FireControl
{
public:



    void DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
        const DirectX::SimpleMath::Vector3 aLaunchPos, 
        const DirectX::SimpleMath::Vector3 aLaunchDirection, 
        Environment const* aEnvironment);
    void FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileShotGun(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireWeapon(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCController(std::shared_ptr<NPCController> aNPCController);
    void UpdateProjectileVec(double aTimeDelta);
private:
    void CheckCollisions();
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void InitializeAmmo(AmmoStruct& aAmmo);
    void InitializeProjectileModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection);
    
    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);

    Environment const* m_environment;
    std::shared_ptr<NPCController> m_npcController;
    std::shared_ptr<DebugData> m_debugData;

    LauncherData m_launcherData;

    AmmoData m_ballAmmo;
    AmmoStruct m_ballAmmoStruct;
    const float m_maxProjectileLifeTime = 10.0f;
    std::vector<ProjectileData> m_projectileVec;
};

