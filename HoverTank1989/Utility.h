#pragma once


// Class to handle miscellaneous functions and data structures needed across multiple classes
class Utility
{
public:
    static inline int GetNumericalPrecisionForUI() { return 2; }; // For setting the numerical precison displayed by UI
    static inline float GetVehicleImpactReboundCoefficient() { return 0.4f; };
    static inline float GetPi() { return 3.1415926535897931; };
    static inline float ToDegrees(float r) { return r * 180.0f / GetPi(); };
    static inline float ToRadians(float d) { return d / 180.0f * GetPi(); };

    struct Torque
    {
        DirectX::SimpleMath::Vector3 axis;
        float                        magnitude;
    };

    struct CollisionData
    {
        float                        collisionDurationMod = 1.0f;
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

    struct ImpulseForce
    {
        float currentMagnitude = 0.0f;
        float currentTime = 0.0f;
        DirectX::SimpleMath::Vector3 directionNorm = DirectX::SimpleMath::Vector3::Zero;
        bool isActive = false;
        float maxMagnitude = 0.0f;
        DirectX::SimpleMath::Vector3 torqueArm = DirectX::SimpleMath::Vector3::Zero;
        float totalTime = 0.0f;
        int tickCount = 0;
    };

    static void UpdateImpulseForceBellCurve(ImpulseForce& aImpulseForce, const float aTimeDelta)
    {
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
            if (aImpulseForce.currentTime == 0.0f && aTimeDelta > aImpulseForce.totalTime)
            {
                //aImpulseForce.isActive = false;
                aImpulseForce.currentMagnitude = aImpulseForce.maxMagnitude;
                aImpulseForce.currentTime += aTimeDelta;
            }
            else if (aImpulseForce.tickCount == 1 && (aImpulseForce.currentTime + aTimeDelta) > aImpulseForce.totalTime)
            {
                //aImpulseForce.isActive = false;
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

private:


};

