#pragma once


// Class to handle miscellaneous functions and data structures needed across multiple classes
class Utility
{
public:
    static inline int GetNumericalPrecisionForUI() { return 2; }; // For setting the numerical precison displayed by UI
    static inline float GetVehicleImpactReboundCoefficient() { return 0.4f; };
    static inline float GetMaxAngularForce() { return 1000000.0f; };
    static inline float GetPi() { return 3.1415926535897931f; };
    static inline float GetHalfPi() { return 1.57079632679f; };
    static inline float GetQuarterPi() { return 0.785398163395f; };
    static inline float ToDegrees(float r) { return r * 180.0f / GetPi(); };
    static inline float ToRadians(float d) { return d / 180.0f * GetPi(); };
    
    static void AddForceAtPoint(const DirectX::SimpleMath::Vector3& aForce, const DirectX::SimpleMath::Vector3& aPoint, const DirectX::SimpleMath::Vector3& aCenterOfMass,
        DirectX::SimpleMath::Vector3& aForceAccum, DirectX::SimpleMath::Vector3& aTorqueAccum)
    {
        DirectX::SimpleMath::Vector3 pt = aPoint;
        pt -= aCenterOfMass;
        aForceAccum += aForce;
        aTorqueAccum += pt.Cross(aForce);
    }

    static void AddScaledVectorToQuat(const DirectX::SimpleMath::Vector3& aVector, const float aScale, DirectX::SimpleMath::Quaternion& aQuat)
    {
        DirectX::SimpleMath::Quaternion q;
        q.w = 0.0f;
        q.x = aVector.x * aScale;
        q.y = aVector.y * aScale;
        q.z = aVector.z * aScale;

        q *= aQuat;
        aQuat.w += q.w * (0.5f);
        aQuat.x += q.x * (0.5f);
        aQuat.y += q.y * (0.5f);
        aQuat.z += q.z * (0.5f);
    };

    struct Torque
    {
        DirectX::SimpleMath::Vector3 axis;
        float                        magnitude;
    };

    struct CollisionData
    {
        float                        collisionDurationMod = 5.0f;
        float                        collisionMagnitudeMod = 1.0f;
        DirectX::BoundingSphere      collisionSphere;
        DirectX::SimpleMath::Vector3 velocity;
        float                        mass;
        bool                         isCollisionTrue;
    };

    struct ImpactForce
    {
        float impactModifier = 1.0f;
        DirectX::SimpleMath::Vector3 impactVelocity;
        float                        impactMass;
        DirectX::SimpleMath::Vector3 kineticEnergy = DirectX::SimpleMath::Vector3::Zero;
    };

    enum class ImpulseType
    {
        IMPULSETYPE_BELLCURVE,
        IMPULSETYPE_FLAT,
        IMPULSETYPE_FRONTLOADCURVE,
        IMPULSETYPE_LAGCURVE,
    };

    struct ImpulseForce
    {
        ImpulseType impulseType = ImpulseType::IMPULSETYPE_BELLCURVE;
        float currentMagnitude = 0.0f;
        float currentTime = 0.0f;
        DirectX::SimpleMath::Vector3 directionNorm = DirectX::SimpleMath::Vector3::Zero;
        bool isActive = false;
        float maxMagnitude = 0.0f;
        DirectX::SimpleMath::Vector3 torqueArm = DirectX::SimpleMath::Vector3::Zero;
        float currentTorqueMagnitude = 0.0f;
        //float torqueForceMod = 0.1f;
        float torqueForceMod = 0.6f;
        DirectX::SimpleMath::Vector3 torqueForceNorm = DirectX::SimpleMath::Vector3::Zero;
        float totalTime = 0.0f;
        int tickCount = 0;

        float forceRemaining = 0.0f;
        float forceSum = 0.0f;
    };

    static void UpdateImpulseForceCurveProjectile(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        aImpulseForce.currentMagnitude = 0.0f;
        aImpulseForce.currentTorqueMagnitude = aImpulseForce.maxMagnitude * aImpulseForce.torqueForceMod;
        aImpulseForce.currentTorqueMagnitude = 1000000.0f;
        aImpulseForce.torqueArm = DirectX::SimpleMath::Vector3::UnitZ;

        aImpulseForce.currentTime += aTimeDelta;
        if (aImpulseForce.currentTime > aImpulseForce.totalTime)
        {
            //aImpulseForce.isActive = false;
        }
        if (aImpulseForce.tickCount > 1)
        {
            aImpulseForce.isActive = false;
        }
        ++aImpulseForce.tickCount;
        //aImpulseForce.isActive = false;
    }

    static void UpdateImpulseForceCurve(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_BELLCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            else
            {
                // check if tick time is less than total time and set to max magnatiude if so
                // so force occurs if impact time is very low
                if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
                {
                    aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                    aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    aImpulseForce.currentTime += aTimeDelta;
                }
                else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
                {
                    const float magMod = 0.5f;
                    aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                    aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    aImpulseForce.currentTime += aTimeDelta;
                }
                else
                {
                    aImpulseForce.currentTime += aTimeDelta;
                    if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
                    {
                        aImpulseForce.isActive = true;
                    }
                    else
                    {
                        aImpulseForce.isActive = false;
                        aImpulseForce.currentMagnitude = 0.0f;
                        aImpulseForce.currentTorqueMagnitude = 0.0f;
                    }
                    if (aImpulseForce.isActive == true)
                    {
                        const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                        float ratio;

                        if (aImpulseForce.currentTime <= maxForceTime)
                        {
                            ratio = aImpulseForce.currentTime / maxForceTime;
                        }
                        else
                        {
                            ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                            ratio = 1.0f - ratio;
                        }
                        aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
                        aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    }
                }

                if (aImpulseForce.isActive == true)
                {
                    aImpulseForce.tickCount++;
                }
                else
                {
                    aImpulseForce.tickCount = 0;
                }
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_FLAT)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.isActive == true)
            {
                aImpulseForce.currentTime += aTimeDelta;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                float magMod = aTimeDelta / aImpulseForce.totalTime;
                //aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.forceRemaining -= aImpulseForce.currentMagnitude;

                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                ++aImpulseForce.tickCount;
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_FRONTLOADCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
            {
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) >= aImpulseForce.totalTime)
            {
                const float magMod = 0.5f;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.isActive == true)
            {
                float ratio;
                if (aImpulseForce.totalTime != 0.0f)
                {
                    ratio = 1.0f - (aImpulseForce.currentTime / aImpulseForce.totalTime);
                    if (ratio < 0.0f)
                    {
                        ratio = 0.0f;
                    }
                }
                else
                {
                    ratio = 0.0f;
                }
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * ratio;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_LAGCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
            {
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) >= aImpulseForce.totalTime)
            {
                const float magMod = 0.5f;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.isActive == true)
            {
                float ratio;
                if (aImpulseForce.totalTime != 0.0f)
                {
                    ratio = aImpulseForce.currentTime / aImpulseForce.totalTime;
                    if (ratio > 1.0f)
                    {
                        ratio = 1.0f;
                    }
                }
                else
                {
                    ratio = 0.0f;
                }
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * ratio;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
        }
    }

    static void UpdateImpulseForceCurve2(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_BELLCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            else
            {
                // check if tick time is less than total time and set to max magnatiude if so
                // so force occurs if impact time is very low
                if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
                {
                    aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                    aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    aImpulseForce.currentTime += aTimeDelta;
                }
                else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
                {
                    const float magMod = 0.5f;
                    aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                    aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    aImpulseForce.currentTime += aTimeDelta;
                }
                else
                {
                    aImpulseForce.currentTime += aTimeDelta;
                    if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
                    {
                        aImpulseForce.isActive = true;
                    }
                    else
                    {
                        aImpulseForce.isActive = false;
                        aImpulseForce.currentMagnitude = 0.0f;
                        aImpulseForce.currentTorqueMagnitude = 0.0f;
                    }
                    if (aImpulseForce.isActive == true)
                    {
                        const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                        float ratio;

                        if (aImpulseForce.currentTime <= maxForceTime)
                        {
                            ratio = aImpulseForce.currentTime / maxForceTime;
                        }
                        else
                        {
                            ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                            ratio = 1.0f - ratio;
                        }
                        aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
                        aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                    }
                }

                if (aImpulseForce.isActive == true)
                {
                    aImpulseForce.tickCount++;
                }
                else
                {
                    aImpulseForce.tickCount = 0;
                }
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_FLAT)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.isActive == true)
            {
                aImpulseForce.currentTime += aTimeDelta;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                ++aImpulseForce.tickCount;
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_FRONTLOADCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
            {
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) >= aImpulseForce.totalTime)
            {
                const float magMod = 0.5f;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.isActive == true)
            {
                float ratio;
                if (aImpulseForce.totalTime != 0.0f)
                {
                    ratio = 1.0f - (aImpulseForce.currentTime / aImpulseForce.totalTime);
                    if (ratio < 0.0f)
                    {
                        ratio = 0.0f;
                    }
                }
                else
                {
                    ratio = 0.0f;
                }
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * ratio;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
        }
        else if (aImpulseForce.impulseType == ImpulseType::IMPULSETYPE_LAGCURVE)
        {
            if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;
            }
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
            {
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) >= aImpulseForce.totalTime)
            {
                const float magMod = 0.5f;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
            else if (aImpulseForce.isActive == true)
            {
                float ratio;
                if (aImpulseForce.totalTime != 0.0f)
                {
                    ratio = aImpulseForce.currentTime / aImpulseForce.totalTime;
                    if (ratio > 1.0f)
                    {
                        ratio = 1.0f;
                    }
                }
                else
                {
                    ratio = 0.0f;
                }
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * ratio;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
                aImpulseForce.currentTime += aTimeDelta;
                ++aImpulseForce.tickCount;
            }
        }
    }

    static void UpdateImpulseForceBellCurve(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        // check if tick time is less than total time and set to max magnatiude if so
        // so force occurs if impact time is very low
        if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
            aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
        }
        else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            const float magMod = 0.5f;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
            aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
        }
        else
        {
            aImpulseForce.currentTime += aTimeDelta;
            if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
            {
                aImpulseForce.isActive = true;
            }
            else
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;// aImpulseForce.currentMagnitude* aImpulseForce.torqueForceMod;
            }
            if (aImpulseForce.isActive == true)
            {
                const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                float ratio;

                if (aImpulseForce.currentTime <= maxForceTime)
                {
                    ratio = aImpulseForce.currentTime / maxForceTime;
                }
                else
                {
                    ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                    ratio = 1.0f - ratio;
                }
                aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
            }
        }
        if (aImpulseForce.isActive == true)
        {
            aImpulseForce.tickCount++;
        }
        else
        {
            aImpulseForce.tickCount = 0;
        }
        if (aImpulseForce.tickCount > 30)
        {
            int testBreak = 0;
            testBreak++;
        }
    }

    static void UpdateImpulseForceBellCurve3(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        // check if tick time is less than total time and set to max magnatiude if so
        // so force occurs if impact time is very low
        if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
            aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
        }
        else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            const float magMod = 0.5f;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
            aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
        }
        else
        {
            aImpulseForce.currentTime += aTimeDelta;
            if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
            {
                aImpulseForce.isActive = true;
            }
            else
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
                aImpulseForce.currentTorqueMagnitude = 0.0f;// aImpulseForce.currentMagnitude* aImpulseForce.torqueForceMod;
            }
            if (aImpulseForce.isActive == true)
            {
                const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                float ratio;

                if (aImpulseForce.currentTime <= maxForceTime)
                {
                    ratio = aImpulseForce.currentTime / maxForceTime;
                }
                else
                {
                    ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                    ratio = 1.0f - ratio;
                }
                aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
                aImpulseForce.currentTorqueMagnitude = aImpulseForce.currentMagnitude * aImpulseForce.torqueForceMod;
            }
        }
        if (aImpulseForce.isActive == true)
        {
            aImpulseForce.tickCount++;
        }
        else
        {
            aImpulseForce.tickCount = 0;
        }
        if (aImpulseForce.tickCount > 30)
        {
            int testBreak = 0;
            testBreak++;
        }
    }

    static void UpdateImpulseForceBellCurve2(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
        if (aImpulseForce.currentTime >= aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            aImpulseForce.currentMagnitude = 0.0f;
        }
        else
        {
            // check if tick time is less than total time and set to max magnatiude if so
            // so force occurs if impact time is very low
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta >= aImpulseForce.totalTime)
            {
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTime += aTimeDelta;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
            {
                const float magMod = 0.5f;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
                aImpulseForce.currentTime += aTimeDelta;
            }
            else
            {
                aImpulseForce.currentTime += aTimeDelta;
                if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
                {
                    aImpulseForce.isActive = true;
                }
                else
                {
                    aImpulseForce.isActive = false;
                    aImpulseForce.currentMagnitude = 0.0f;
                }
                if (aImpulseForce.isActive == true)
                {
                    const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                    float ratio;

                    if (aImpulseForce.currentTime <= maxForceTime)
                    {
                        ratio = aImpulseForce.currentTime / maxForceTime;
                    }
                    else
                    {
                        ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                        ratio = 1.0f - ratio;
                    }
                    aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
                }
            }

            if (aImpulseForce.isActive == true)
            {
                aImpulseForce.tickCount++;
            }
            else
            {
                aImpulseForce.tickCount = 0;
            }
            if (aImpulseForce.tickCount > 30)
            {
                int testBreak = 0;
                testBreak++;
            }
        }
        /*
        // check if tick time is less than total time and set to max magnatiude if so
        // so force occurs if impact time is very low
        if (aImpulseForce.currentTime == 0.0f && aTimeDelta > aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
        }
        else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
        {
            aImpulseForce.isActive = false;
            const float magMod = 0.5f;
            aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude * magMod;
        }
        else
        {
            aImpulseForce.currentTime += aTimeDelta;
            if (aImpulseForce.currentTime < aImpulseForce.totalTime && aImpulseForce.currentTime >= 0.0f)
            {
                aImpulseForce.isActive = true;
            }
            else
            {
                aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = 0.0f;
            }
            if (aImpulseForce.isActive == true)
            {
                const float maxForceTime = aImpulseForce.totalTime * 0.5f;
                float ratio;

                if (aImpulseForce.currentTime <= maxForceTime)
                {
                    ratio = aImpulseForce.currentTime / maxForceTime;
                }
                else
                {
                    ratio = (aImpulseForce.currentTime - maxForceTime) / (aImpulseForce.totalTime - maxForceTime);
                    ratio = 1.0f - ratio;
                }
                aImpulseForce.currentMagnitude = ratio * aImpulseForce.maxMagnitude;
            }
        }
        if (aImpulseForce.isActive == true)
        {
            aImpulseForce.tickCount++;
        }
        else
        {
            aImpulseForce.tickCount = 0;
        }
        if (aImpulseForce.tickCount > 30)
        {
            int testBreak = 0;
            testBreak++;
        }
        */
    }
    
    static void AddForce(DirectX::SimpleMath::Vector3& aForceSum, const DirectX::SimpleMath::Vector3 aForceAdded)
    {
        const float tolMax = 1000000.0f;
        const float tolMin = 0.00000001f;
        const float forceLength = aForceAdded.Length();
        if (forceLength > tolMin && forceLength < tolMax)
        {
            aForceSum += aForceAdded;
        }
        else
        {
            // add error checking
            int errorBreak = 0;
            errorBreak++;
        }
    }

    static void AddTorqueToForceSum(DirectX::SimpleMath::Vector3& aForceSum, const Utility::Torque aTorque)
    {
        const float tolMax = 1000000.0f;
        const float tolMin = 0.00000001f;
        const float torqueAxisLength = aTorque.axis.Length();
        if (torqueAxisLength > 0.9f && torqueAxisLength < 1.1f)
        {
            if (aTorque.magnitude > tolMin && aTorque.magnitude < tolMax)
            {
                aForceSum += aTorque.axis * aTorque.magnitude;
            }
        }
        else
        {
            // add error checking
            int errorBreak = 0;
            errorBreak++;
        }
    }
    
    static void TransformInertiaTensor(const DirectX::SimpleMath::Vector3& invD, const DirectX::SimpleMath::Matrix& M, DirectX::SimpleMath::Matrix& mIInv)
    {
        const float	axx = invD.x * M(0, 0), axy = invD.x * M(1, 0), axz = invD.x * M(2, 0);

        const float	byx = invD.y * M(0, 1), byy = invD.y * M(1, 1), byz = invD.y * M(2, 1);
        const float	czx = invD.z * M(0, 2), czy = invD.z * M(1, 2), czz = invD.z * M(2, 2);

        mIInv(0, 0) = axx * M(0, 0) + byx * M(0, 1) + czx * M(0, 2);
        mIInv(1, 1) = axy * M(1, 0) + byy * M(1, 1) + czy * M(1, 2);
        mIInv(2, 2) = axz * M(2, 0) + byz * M(2, 1) + czz * M(2, 2);

        mIInv(0, 1) = mIInv(1, 0) = axx * M(1, 0) + byx * M(1, 1) + czx * M(1, 2);
        mIInv(0, 2) = mIInv(2, 0) = axx * M(2, 0) + byx * M(2, 1) + czx * M(2, 2);
        mIInv(1, 2) = mIInv(2, 1) = axy * M(2, 0) + byy * M(2, 1) + czy * M(2, 2);
    }

    static DirectX::SimpleMath::Vector3 ProcessTorqueForce(const DirectX::SimpleMath::Vector3 aArmPos, const DirectX::SimpleMath::Vector3 aFulcrumPos, const DirectX::SimpleMath::Vector3 aForce)
    {
        DirectX::SimpleMath::Vector3 torqueArm = aArmPos - aFulcrumPos;

        DirectX::SimpleMath::Vector3 forceVec = (aArmPos - aFulcrumPos).Cross(aForce);
        forceVec.Normalize();
        forceVec *= torqueArm.Length() * aForce.Length() * sin(GetAngleBetweenVectors(torqueArm, aForce));
        //Torque torqueForce;
        //torqueForce.axis = aArm.Cross(aForce);
        //torqueForce.axis.Normalize();
        //torqueForce.magnitude = aArm.Length() * aForce.Length() * sin(GetAngleBetweenVectors(aArm, aForce));
        return forceVec;
    }

    static void ClearForceSum(DirectX::SimpleMath::Vector3& aForceSum)
    {
        aForceSum = DirectX::SimpleMath::Vector3::Zero;
    }

    struct Waypoint
    {
        DirectX::SimpleMath::Vector3 waypointPos;
        float                        waypointRadius;
    };
    
    static Waypoint CreateWaypoint(const DirectX::SimpleMath::Vector3 aPos, const float aRadius)
    {
        Waypoint wp;
        wp.waypointPos = aPos;
        wp.waypointRadius = aRadius;
        return wp;
    }

    struct WayPath
    {
        int nodeCount;
        int targetNode;
        bool                  isPathLooped;
        std::vector<Waypoint> wayPath;       
    };

    static void ClearWayPath(WayPath& aWayPath)
    {
        aWayPath.nodeCount = 0;
        aWayPath.targetNode = 0;
        aWayPath.wayPath.clear();
        aWayPath.isPathLooped = false;
    }

    static Waypoint GetWaypointFromPath(WayPath& aWayPath)
    {
        Waypoint wp;
        wp.waypointPos = aWayPath.wayPath[aWayPath.targetNode].waypointPos;
        wp.waypointRadius = aWayPath.wayPath[aWayPath.targetNode].waypointRadius;
        return wp;
    }

    static void IncrementWayPath(WayPath& aWayPath)
    {
        if (aWayPath.targetNode < aWayPath.nodeCount - 1)
        {
            aWayPath.targetNode++;
        }
        else if (aWayPath.targetNode == aWayPath.nodeCount - 1 && aWayPath.isPathLooped == true)
        {
            aWayPath.targetNode = 0;
        }
        else
        {
            // leave target node as last node
            // add error checking
        }
    }

    static void PushWaypointToPath(WayPath& aWayPath, Waypoint aWaypoint)
    {
        aWayPath.wayPath.push_back(aWaypoint);
        aWayPath.nodeCount++;
    }

    static float RandFloat()
    {
        return ((rand()) / (RAND_MAX + 1.0f));
    }

    static float RandomClamped()
    {
        return RandFloat() - RandFloat();
    }

    template<typename T>
    static T WrapAngle(T aTheta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T mod = fmod(aTheta, twoPi);
        if (mod > (T)Utility::GetPi())
        {
            return mod - twoPi;
        }
        else if (mod < -(T)Utility::GetPi())
        {
            return mod + twoPi;
        }
        return mod;
    }

    template<typename T>
    static T WrapAngleOnePi(T aTheta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T onePi = (T)1 * (T)Utility::GetPi();
        const T halfPi = (T)1 * (T)Utility::GetHalfPi();
        const T mod = fmod(aTheta, onePi);
        if (mod > halfPi)
        {
            return mod - onePi;
        }
        else if (mod < -halfPi)
        {
            return mod + onePi;
        }
        return mod;
    }

    template<typename T>
    static T WrapAngleOnePiPositive(T aTheta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T onePi = (T)1 * (T)Utility::GetPi();
        const T halfPi = (T)1 * (T)Utility::GetHalfPi();
        const T pi = (T)1 * (T)Utility::GetPi();
        const T mod = fmod(aTheta, onePi);
        if (mod > (T)Utility::GetPi())
        {
            return mod - pi;
        }
        else if (mod < -(T)0.0)
        {
            return mod + pi;
        }

        if (mod > (T)Utility::GetPi())
        {
            return mod - pi;
        }
        else if (mod < -(T)0.0)
        {
            return mod + pi;
        }
        return mod;
    }

    template<typename T>
    static T WrapAngleHalfPi(T aTheta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T onePi = (T)1 * (T)Utility::GetPi();
        const T halfPi = (T)1 * (T)Utility::GetHalfPi();
        const T quarterPi = (T)1 * (T)Utility::GetQuarterPi();
        //const T mod = fmod(aTheta, twoPi);
        const T mod = fmod(aTheta, halfPi);
        //if (mod > (T)Utility::GetPi())
        if (mod > quarterPi)
        {
            //return mod - twoPi;
            return mod - halfPi;
        }
        //else if (mod < -(T)Utility::GetPi())
        else if (mod < -quarterPi)
        {
            //return mod + twoPi;
            return mod + halfPi;
        }
        return mod;
    }

    template<typename T>
    static T WrapAnglePositive(T aTheta) noexcept
    {
        const T pi = (T)1 * (T)Utility::GetPi();
        const T mod = fmod(aTheta, pi);
        if (mod > (T)Utility::GetPi())
        {
            return mod - pi;
        }
        else if (mod < -(T)0.0)
        {
            return mod + pi;
        }
        return mod;
    }

    template<typename T>
    static T WrapAngleRight(T aTheta) noexcept
    {
        const T twoPi = (T)2 * (T)Utility::GetPi();
        const T pi = (T)1 * (T)Utility::GetPi();
        const T halfPi = (T)1 * (T)Utility::GetHalfPi();
        const T quarterPi = (T)0.5 * (T)Utility::GetHalfPi();

        const T mod = fmod(aTheta, pi);
        const T modHalf = fmod(aTheta, halfPi);
        if (mod > halfPi)
        {
            return halfPi - modHalf;
        }
        else if (mod < -(T)Utility::GetPi())
        {
            return mod + twoPi;
        }
        return mod;
    }

    static float GetAngleBetweenVectors(DirectX::SimpleMath::Vector3 aVec1, DirectX::SimpleMath::Vector3 aVec2)
    {
        aVec1.Normalize();
        aVec2.Normalize();
        float dot = aVec1.Dot(aVec2);
        if (dot > 1.0f)
        {
            dot = 1.0f;
        }
        else if (dot < -1.0f)
        {
            dot = -1.0f;
        }

        const float angle = acos(dot);
        return angle;
    }
    
    static Torque GetTorqueForce(const DirectX::SimpleMath::Vector3 aArm, const DirectX::SimpleMath::Vector3 aForce)
    {
        Torque torqueForce;
        torqueForce.axis = aArm.Cross(aForce);
        torqueForce.axis.Normalize();
        torqueForce.magnitude = aArm.Length() * aForce.Length() * sin(GetAngleBetweenVectors(aArm, aForce));

        return torqueForce;
    }
    
    static Torque GetTorqueForce2(const DirectX::SimpleMath::Vector3 aArm, const DirectX::SimpleMath::Vector3 aForce)
    {
        Torque torqueForce;
        //torqueForce.axis = aArm.Cross(aForce);
        torqueForce.axis = aForce.Cross(aArm);
        torqueForce.axis.Normalize();
        torqueForce.magnitude = aArm.Length() * aForce.Length() * sin(GetAngleBetweenVectors(aArm, aForce));

        return torqueForce;
    }

    static void GetDispersedLightDirections(const DirectX::SimpleMath::Vector3 aPrimeDirection, const float aOffsetAngle, DirectX::SimpleMath::Vector3& aDir1, DirectX::SimpleMath::Vector3& aDir2, DirectX::SimpleMath::Vector3& aDir3)
    {
        DirectX::SimpleMath::Vector3 rightVec;
        if (aPrimeDirection == DirectX::SimpleMath::Vector3::UnitY)
        {
            rightVec = DirectX::SimpleMath::Vector3::UnitZ;
        }
        else if (aPrimeDirection == -DirectX::SimpleMath::Vector3::UnitY)
        {
            rightVec = -DirectX::SimpleMath::Vector3::UnitZ;
        }
        else
        {
            rightVec = aPrimeDirection.Cross(DirectX::SimpleMath::Vector3::UnitY);
        }

        aDir1 = aPrimeDirection;
        aDir1 = DirectX::SimpleMath::Vector3::TransformNormal(aDir1, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(rightVec, aOffsetAngle));
        aDir2 = aDir1;
        aDir2 = DirectX::SimpleMath::Vector3::TransformNormal(aDir2, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aPrimeDirection, Utility::ToRadians(120.0f)));
        aDir3 = aDir1;
        aDir3 = DirectX::SimpleMath::Vector3::TransformNormal(aDir3, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aPrimeDirection, Utility::ToRadians(240.0f)));
    }

    static void GetDispersedLightDirectionsRotation(const DirectX::SimpleMath::Vector3 aPrimeDirection, const float aOffsetAngle, const float aRotation, DirectX::SimpleMath::Vector3& aDir1, DirectX::SimpleMath::Vector3& aDir2, DirectX::SimpleMath::Vector3& aDir3)
    {
        DirectX::SimpleMath::Vector3 rightVec;
        if (aPrimeDirection == DirectX::SimpleMath::Vector3::UnitY)
        {
            rightVec = DirectX::SimpleMath::Vector3::UnitZ;
        }
        else if (aPrimeDirection == -DirectX::SimpleMath::Vector3::UnitY)
        {
            rightVec = -DirectX::SimpleMath::Vector3::UnitZ;
        }
        else
        {
            rightVec = aPrimeDirection.Cross(DirectX::SimpleMath::Vector3::UnitY);
        }

        aDir1 = aPrimeDirection;
        aDir1 = DirectX::SimpleMath::Vector3::TransformNormal(aDir1, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(rightVec, aOffsetAngle));
        aDir1 = DirectX::SimpleMath::Vector3::TransformNormal(aDir1, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aPrimeDirection, aRotation));
        aDir2 = aDir1;
        aDir2 = DirectX::SimpleMath::Vector3::TransformNormal(aDir2, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aPrimeDirection, Utility::ToRadians(120.0f)));
        aDir3 = aDir1;
        aDir3 = DirectX::SimpleMath::Vector3::TransformNormal(aDir3, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aPrimeDirection, Utility::ToRadians(240.0f)));
    }

    struct ForceAccum
    {
        DirectX::SimpleMath::Vector3 linear = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 torque = DirectX::SimpleMath::Vector3::Zero;

        static void ZeroValues(ForceAccum& aAccum)
        {
            aAccum.linear = DirectX::SimpleMath::Vector3::Zero;
            aAccum.torque = DirectX::SimpleMath::Vector3::Zero;
        }

        static void AlignLinear(ForceAccum& aAccum, const DirectX::SimpleMath::Quaternion& aQuat)
        {
            aAccum.linear = DirectX::SimpleMath::Vector3::Transform(aAccum.linear, aQuat);
        }

        static void AlignLinearAndTorque(ForceAccum& aAccum, const DirectX::SimpleMath::Quaternion& aQuat)
        {
            aAccum.linear = DirectX::SimpleMath::Vector3::Transform(aAccum.linear, aQuat);
            aAccum.torque = DirectX::SimpleMath::Vector3::Transform(aAccum.torque, aQuat);
        }

        inline Utility::ForceAccum& operator+= (const ForceAccum& aAccum) noexcept
        {
            this->linear += aAccum.linear;
            this->torque += aAccum.torque;
            return *this;
        }

        inline Utility::ForceAccum& operator-= (const ForceAccum& aAccum) noexcept
        {
            this->linear -= aAccum.linear;
            this->torque -= aAccum.torque;
            return *this;
        }

        inline Utility::ForceAccum& operator*= (const ForceAccum& aAccum) noexcept
        {
            this->linear *= aAccum.linear;
            this->torque *= aAccum.torque;
            return *this;
        }
    };

    struct MotionData
    {
        DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 velocity = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 angularVelocity = DirectX::SimpleMath::Vector3::Zero;
    };

    struct MotionObj
    {
        MotionData q;
        float mass;
        DirectX::SimpleMath::Vector3 linearAccumulated = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 linearDrag = DirectX::SimpleMath::Vector3::Zero;

        DirectX::SimpleMath::Matrix inverseInertiaMatrix = DirectX::SimpleMath::Matrix::Identity;

        DirectX::SimpleMath::Vector3 torqueAccumulated = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 torqueDrag = DirectX::SimpleMath::Vector3::Zero;
        double time = 0.0f;
    };

    static void RungeKutta(struct MotionObj* aObj, double aTimeDelta)
    {
        //  Define a convenience variables
        const float numEqns = static_cast<float>(6);
        //  Retrieve the current values of the dependent and independent variables.
        MotionData q = aObj->q;
        MotionData dq1;
        MotionData dq2;
        MotionData dq3;
        MotionData dq4;

        // Compute the four Runge-Kutta steps, The return 
        // value of RightHandSide method is an array
        // of delta-q values for each of the four steps.
        RightHandSide(aObj, &q, &q, aTimeDelta, 0.0, &dq1);
        RightHandSide(aObj, &q, &dq1, aTimeDelta, 0.5, &dq2);
        RightHandSide(aObj, &q, &dq2, aTimeDelta, 0.5, &dq3);
        RightHandSide(aObj, &q, &dq3, aTimeDelta, 1.0, &dq4);

        aObj->time = aObj->time + aTimeDelta;

        DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
        DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;
        //DirectX::SimpleMath::Vector3 angularMomentumUpdate = (dq1.angularMomentum + 2.0 * dq2.angularMomentum + 2.0 * dq3.angularMomentum + dq4.angularMomentum) / numEqns;
        DirectX::SimpleMath::Vector3 angularVelocityUpdate = (dq1.angularVelocity + 2.0 * dq2.angularVelocity + 2.0 * dq3.angularVelocity + dq4.angularVelocity) / numEqns;

        q.velocity += velocityUpdate;
        q.position += posUpdate;
        //q.angularMomentum += angularMomentumUpdate;
        q.angularVelocity += angularVelocityUpdate;

        aObj->q.velocity = q.velocity;
        aObj->q.position = q.position;
        //aObj->projectileData.collisionData.collisionSphere.Center = q.position;
        //aObj->projectileData.collisionData.velocity = q.velocity;
        aObj->q.angularVelocity = q.angularVelocity;
        //aObj->projectileData.q.angularMomentum = q.angularMomentum;
    };

    static void RightHandSide(struct MotionObj* aObj, MotionData* aQ, MotionData* aDeltaQ, double aTimeDelta, float aQScale, MotionData* aDQ)
    {
        //  Compute the intermediate values of the 
        //  dependent variables.
        MotionData newQ;
        newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
        newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

        const float mass = aObj->mass;

        // Linear update
        DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
        velocityNorm.Normalize();
        //DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Vector3 velocityUpdate = newQ.velocity;
        velocityUpdate += aObj->linearAccumulated;
        velocityUpdate += aObj->linearDrag;
        
        aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
        aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;

        // angular update
        
        DirectX::SimpleMath::Vector3 torqueAccum = aObj->torqueAccumulated;
        torqueAccum += newQ.angularVelocity;
        torqueAccum = DirectX::SimpleMath::Vector3::Transform(torqueAccum, aObj->inverseInertiaMatrix);
        torqueAccum += aObj->torqueDrag;;
        
        aDQ->angularVelocity = static_cast<float>(aTimeDelta) * torqueAccum;
    };

    static int IntervalComparison(float x, float lowerBound, float upperBound)
    {
        if (x < lowerBound)
        {
            return -1;
        }
        else if (x > upperBound)
        {
            return +1;
        }
        else
        {
            return 0;
        }
    };


    enum class SoundFxType
    {
        SOUNDFXTYPE_DEBUG,
        SOUNDFXTYPE_EXPLOSION,
        SOUNDFXTYPE_SHOTBANG,
        SOUNDFXTYPE_BEACON_01,
        SOUNDFXTYPE_BEACON_02,
        SOUNDFXTYPE_BEACON_03,
        SOUNDFXTYPE_BEACON,
        SOUNDFXTYPE_BEACON_ALT,
        SOUNDFXTYPE_POOF,
    };

    struct SoundFx
    {
        std::shared_ptr<DirectX::AudioEmitter> emitter;
        std::shared_ptr<DirectX::SoundStreamInstance> fx;

        DirectX::SimpleMath::Vector3 forward;
        DirectX::SimpleMath::Vector3 pos;
        DirectX::SimpleMath::Vector3 up;
        bool isDestroyTrue;
        bool isTriggeredTrue = true;
        SoundFxType fxType = SoundFxType::SOUNDFXTYPE_DEBUG;

        void SetPos(DirectX::SimpleMath::Vector3 aPos)
        {
            pos = aPos;
        }

        void SetEmitter(std::shared_ptr<DirectX::AudioEmitter> aEmitter)
        {
            emitter = aEmitter;
        }
    };

private:


};

