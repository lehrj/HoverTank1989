#pragma once
#include "Environment.h"
#include "NPCController.h"
#include "Utility.h"
#include "Vehicle.h"

class Vehicle;

enum class AmmoType
{
    AMMOTYPE_CANNON,
    AMMOTYPE_EXPLOSIVE,
    AMMOTYPE_MIRV,
    AMMOTYPE_MACHINEGUN,
    AMMOTYPE_SHOTGUN,
    AMMOTYPE_GUIDEDMISSILE,
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
    float   cooldown;
    float   dragCoefficient;
    float   impactDurration;
    float   impactModifier;
    float   launchVelocity;
    float   length;
    float   mass;
    float   frontSurfaceArea;
    float   radius;
    int tickDownCounter;  // value to tick down as collisions occure, to be used for penetration and ricochet 

    bool isGuided = false;
};

struct GuidanceSystem
{
    float detonationRadius = 5.0f;
    int targetID = 0;
    DirectX::SimpleMath::Vector3 targetDestination = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetPosition = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 targetVelocity = DirectX::SimpleMath::Vector3::Zero;
    float targetDistance = 0.0f;
    bool isRocketFired = false;
    bool isTargetLocked = false;
    float rocketFireDelay = 1.0f;
    float steeringForce = 0.0f;
    DirectX::SimpleMath::Vector3 heading = DirectX::SimpleMath::Vector3::Zero;
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
    bool isMidAirDeployAvailable = false;
    bool isFuseTriggered = false;
    float fuseTimer = 0.0f;
    GuidanceSystem guidance;

    DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 right = DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Quaternion alignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion inverseAlignmentQuat = DirectX::SimpleMath::Quaternion::Identity;
};

struct MissileModel
{
    std::unique_ptr<DirectX::GeometricPrimitive>    mainBody;
    DirectX::SimpleMath::Matrix localMatrix;
    DirectX::SimpleMath::Matrix worldMatrix;
};

struct MissileData
{
    ProjectileData projectileData;
    GuidanceSystem guidance;
};


struct MissileStruct
{
    AmmoData ammoData;  
    MissileModel modelData;
};


enum class ExplosionType
{
    EXPLOSIONTYPE_NONVEHICLE,
    EXPLOSIONTYPE_VEHICLESURFACE,
    EXPLOSIONTYPE_VEHICLEINTERNAL,
};

struct ExplosionData
{
    DirectX::BoundingSphere collisionSphere;
    DirectX::SimpleMath::Vector4 explosionCurrentColor;
    DirectX::SimpleMath::Vector4 explosionEndColor;
    DirectX::SimpleMath::Vector4 explosionStartColor;
    DirectX::SimpleMath::Vector4 color0;
    DirectX::SimpleMath::Vector4 color1;
    DirectX::SimpleMath::Vector4 color2;
    DirectX::SimpleMath::Vector4 color3;
    DirectX::SimpleMath::Vector4 color4;
    DirectX::SimpleMath::Vector4 color5;
    DirectX::SimpleMath::Vector4 color6;
    DirectX::SimpleMath::Vector4 color7;
    DirectX::SimpleMath::Vector4 color8;
    DirectX::SimpleMath::Matrix explosionMatrix0;
    DirectX::SimpleMath::Matrix explosionMatrix1;
    DirectX::SimpleMath::Matrix explosionMatrix2;
    DirectX::SimpleMath::Matrix explosionMatrix3;
    DirectX::SimpleMath::Matrix explosionMatrix4;
    DirectX::SimpleMath::Matrix explosionMatrix5;
    DirectX::SimpleMath::Matrix explosionMatrix6;
    DirectX::SimpleMath::Matrix explosionMatrix7;
    DirectX::SimpleMath::Matrix explosionMatrix8;
    DirectX::SimpleMath::Matrix localExplosionMatrix;
    DirectX::SimpleMath::Matrix rotationVariationMatrix;
    double currentDuration;
    float currentRadius;
    float initialRadius;
    bool isLifeTimeExpired;
    DirectX::SimpleMath::Vector3 localizedSurfaceExplosionPos = DirectX::SimpleMath::Vector3::Zero;
    int vehicleExplosionID = -1;
    float maxRadius;
    DirectX::SimpleMath::Vector3 position;

    ExplosionType explosionType;

    double totalDuration;

    DirectX::SimpleMath::Vector3 lightDir0 = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 lightDir1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 lightDir2 = DirectX::SimpleMath::Vector3::UnitZ;
};

struct ExplosionStruct
{
    ExplosionData explosionRefData;
    std::unique_ptr<DirectX::GeometricPrimitive> explosionShape;
    std::unique_ptr<DirectX::GeometricPrimitive> explosionShape1;
    std::vector<ExplosionData> explosionVec;
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, int>> explosionToPushVec;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureExplosion;  
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapExplosion;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularExplosion;

    const float internalVehicleExplosionRadiusMod = 1.5f;
    float maxExplosionForce;
    float maxExplosionImpactRadius;
};

struct MuzzleFlash
{
    DirectX::SimpleMath::Vector4 color1 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    DirectX::SimpleMath::Vector4 startColor = DirectX::SimpleMath::Vector4(0.7f, 0.0f, 0.0f, 1.3f);
    DirectX::SimpleMath::Vector4 endColor = DirectX::SimpleMath::Vector4(1.f, 0.4f, 0.f, 0.7f);
    DirectX::SimpleMath::Vector4 currentColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape;
    std::unique_ptr<DirectX::GeometricPrimitive>    muzzleFlashConeShape2;
    DirectX::SimpleMath::Matrix worldMuzzleFlashConeMatrix;
    DirectX::SimpleMath::Matrix localMuzzleConeMatrix;
    DirectX::SimpleMath::Matrix worldTestMatrix;
    DirectX::SimpleMath::Matrix localTestMatrix;
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::UnitX;
    float flashDuration = 0.15f;
    float flashTimer = 0.0f;
    bool isFlashActive = false;
    float sizeMod = 0.0f;
    float growthRate = 20.0f;

    bool isFlickerTrue = false;
    float flickerScale = 0.5f;
    float baseConeHeight = 1.0f;
    float baseConeDiameter = 0.3f;
};

class FireControl
{
public:
    void CycleLoadedAmmo();
    void DrawFireControlObjects(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawFireControlObjects2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void InitializeFireControl(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, 
        const DirectX::SimpleMath::Vector3 aLaunchPos, 
        const DirectX::SimpleMath::Vector3 aLaunchDirection, 
        Environment const* aEnvironment, std::shared_ptr<Vehicle> aVehicle);

    void InitializeTextureMapsExplosion(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aTexture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aNormalMap, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& aSpecularMap);

    void FireProjectileCannon(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileExplosive(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileGuidedMissile(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMachineGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileMirv(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireProjectileShotGun(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileShotGun2(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    void FireProjectileShotGun3(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncheraLaunchDirectionRight, const DirectX::SimpleMath::Vector3 aLauncherVelocity);

    void FireSelectedAmmo(const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity, const DirectX::SimpleMath::Vector3 aUp);
    void FireDefaultProjectile(const AmmoType aAmmoType, const DirectX::SimpleMath::Vector3 aLaunchPos, const DirectX::SimpleMath::Vector3 aLaunchDirectionForward, const DirectX::SimpleMath::Vector3 aLauncherVelocity);
    AmmoType GetCurrentAmmoType() const { return m_currentAmmoType; };
    Utility::ImpulseForce GetRecoilImpulseForce(DirectX::SimpleMath::Vector3 aDirectionNorm);
    void PushVehicleExplosion(const DirectX::SimpleMath::Vector3 aPos, const int aVehicleId);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);
    void SetNPCController(std::shared_ptr<NPCController> aNPCController);

    void ToggleTargetingLaser();
    void TriggerMirvDeploy();

    void UpdateFireControl(double aTimeDelta);

private:
    void ActivateMuzzleFlash(AmmoType aAmmoType);
    void CastRayLaser();
    void CreateExplosion(const DirectX::SimpleMath::Vector3 aPos, ExplosionType aExplosionType, const int aVehicleId);
    void CheckCollisions();
    void DeleteProjectileFromVec(const unsigned int aIndex);
    void DeployMirv(ProjectileData& aProjectile);
    void DeployMirv2(ProjectileData& aProjectile);
    void DeployMirv3(ProjectileData& aProjectile);

    void DrawExplosions(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawExplosions2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);
    void DrawExplosions3(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void DrawMuzzleFlash(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawMuzzleFlash2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout);

    void DrawProjectiles(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void DrawProjectiles2(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj);
    void InitializeAmmoCannon(AmmoStruct& aAmmo);
    void InitializeAmmoExplosive(AmmoStruct& aAmmo);
    void InitializeAmmoGuidedMissile(AmmoStruct& aAmmo);
    void InitializeAmmoMachineGun(AmmoStruct& aAmmo);
    void InitializeAmmoMirv(AmmoStruct& aAmmo);
    void InitializeAmmoShotgun(AmmoStruct& aAmmo);

    void InitializeAmmoMissile(MissileStruct& aAmmo);

    void InitializeExplosionData(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, ExplosionData& aExplosionData);
    void InitializeMuzzleFlashModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, MuzzleFlash& aMuzzleFlash);
    void InitializeProjectileModelCannon(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelExplosive(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelGuidedMissile(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelMachineGun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelMirv(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeProjectileModelShotgun(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, AmmoStruct& aAmmo);
    void InitializeLauncherData(LauncherData& aLauncher, const DirectX::SimpleMath::Vector3 aPosition, const DirectX::SimpleMath::Vector3 aDirection);
    
    void RightHandSide(struct ProjectileData* aProjectile, ProjectileMotion* aQ, ProjectileMotion* aDeltaQ, double aTimeDelta, float aQScale, ProjectileMotion* aDQ);
    void RungeKutta4(struct ProjectileData* aProjectile, double aTimeDelta);
   
    void UpdateExplosionVec(double aTimeDelta);
    void UpdateMirv(ProjectileData& aProjectile, const double aTimeDelta);
    void UpdateMuzzleFlash(MuzzleFlash& aMuzzleFlash, const double aTimeDelta);
    void UpdateProjectileVec(double aTimeDelta);
    void UpdateProjectileData(ProjectileData& aProjectile, const float aTimeDelta);

    Environment const* m_environment;
    std::shared_ptr<DebugData> m_debugData;
    std::shared_ptr<NPCController> m_npcController;
    std::shared_ptr<Vehicle> m_playerVehicle;

    LauncherData m_launcherData;

    AmmoStruct m_ammoCannon;
    AmmoStruct m_ammoExplosive;
    AmmoStruct m_ammoGuidedMissile;
    AmmoStruct m_ammoMachineGun;
    AmmoStruct m_ammoMirv;
    AmmoStruct m_ammoShotgun;

    MissileStruct m_ammoMissile;

    ExplosionStruct m_explosionStruct;

    const float m_maxProjectileLifeTime = 10.0f;
    std::vector<ProjectileData> m_projectileVec;
    std::vector<ProjectileData> m_newProjectilePushVec;

    float m_testTimer = 0.0f;

    float m_coolDownTimer = 0.0f;
    bool m_isCoolDownActive = false;

    const float m_mirvFuseDelayTime = 0.7f;

    AmmoType m_currentAmmoType;
    MuzzleFlash m_muzzleFlash;

    const float m_gravityMod = 1.0f;
    const float m_explosiveTorqueArmMod = 1.0f;
    const float m_maxExplosiveForce = 500000.0f;

    bool m_isTestBoolTrue = false;

    bool m_isTargetingLaserOn = false;

public:
    float GetExplosiveTorqueArmMod() const { return m_explosiveTorqueArmMod; };
    bool GetIsCoolDownActive() const { return m_isCoolDownActive; };
    float GetMaxExplosionForce() const { return m_explosionStruct.maxExplosionForce; };
    float GetMaxExplosionImpactRadius() const { return m_explosionStruct.maxExplosionImpactRadius; };

};

