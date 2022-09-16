#pragma once
#include "Environment.h"
#include "NPCController.h"
#include "Utility.h"

enum class AmmoType
{
    AMMOTYPE_BALL01,
    AMMOTYPE_CANNON,
    AMMOTYPE_EXPLOSIVE,
    AMMOTYPE_SHOTGUN,
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
    Utility::CollisionData  collisionData;
    bool isCollisionTrue;
    bool isDeleteTrue;
    float time;
    int liveTimeTick;
};

struct ExplosionData
{   
    DirectX::BoundingSphere collisionSphere;
    DirectX::SimpleMath::Vector4 explosionCurrantColor;
    DirectX::SimpleMath::Vector4 explosionEndColor;
    DirectX::SimpleMath::Vector4 explosionStartColor;
    DirectX::SimpleMath::Matrix explosionMatrix;
    DirectX::SimpleMath::Matrix explosionMatrix1;
    DirectX::SimpleMath::Matrix explosionMatrix2;
    DirectX::SimpleMath::Matrix localExplosionMatrix;
    double totalDuration;
    double currentDuration;
    float currentRadius;
    float initialRadius;
    float maxRadius;
    DirectX::SimpleMath::Vector3 position;
    bool isLifeTimeExpired;
};

class FireControl
{
public:
    void DrawExplosion(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawProjectile(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
        const DirectX::SimpleMath::Vector3 aLaunchPos, 
        const DirectX::SimpleMath::Vector3 aLaunchDirection, 
        Environment const* aEnvironment);
    void FireProjectile(AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCController(std::shared_ptr<NPCController> aNPCController);

    void UpdateFireControl(double aTimeDelta);
private:
    void CreateExplosion(DirectX::SimpleMath::Vector3 aPos);
    void CheckCollisions();
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void InitializeAmmo(AmmoStruct& aAmmo);
    void InitializeAmmoCannon(AmmoStruct& aAmmo);
    void InitializeAmmoExplosive(AmmoStruct& aAmmo);
    void InitializeAmmoShotgun(AmmoStruct& aAmmo);
    void InitializeExplosionData(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, ExplosionData& aExplosionData);
    void InitializeProjectileModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection);
    
    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);
   
    void UpdateExplosionVec(double aTimeDelta);
    void UpdateProjectileVec(double aTimeDelta);

    Environment const* m_environment;
    std::shared_ptr<NPCController> m_npcController;
    std::shared_ptr<DebugData> m_debugData;

    LauncherData m_launcherData;

    //AmmoData m_ballAmmo;
    AmmoStruct m_ammoCannon;
    AmmoStruct m_ammoExplosive;
    AmmoStruct m_ammoShotgun;
    AmmoStruct m_ballAmmoStruct;

    ExplosionData m_explosionData;
    std::unique_ptr<DirectX::GeometricPrimitive> m_explosionShape;
    std::vector<ExplosionData> m_explosionVec;

    const float m_maxProjectileLifeTime = 10.0f;
    std::vector<ProjectileData> m_projectileVec;
};

