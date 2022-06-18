#pragma once


// Class to handle miscellaneous functions and data structures needed across multiple classes
class Utility
{
public:
    //Utility();
    static inline int GetNumericalPrecisionForUI() { return 2; }; // For setting the numerical precison displayed by UI
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
        float                        collisionModifier = 1.0f;
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

    static DirectX::SimpleMath::Vector3 GetTorqueVec(const DirectX::SimpleMath::Vector3 aVec1, const DirectX::SimpleMath::Vector3 aVec2)
    {
        const DirectX::SimpleMath::Vector3 torqueVec = aVec1.Cross(aVec2) * sin(GetAngleBetweenVectors(aVec1, aVec2));
        return torqueVec;
    }
    
    static DirectX::SimpleMath::Vector4 GetTorqueVec4(const DirectX::SimpleMath::Vector3 aVec1, const DirectX::SimpleMath::Vector3 aVec2)
    {
        DirectX::SimpleMath::Vector3 torqueVec = aVec1.Cross(aVec2);

        DirectX::SimpleMath::Vector3 torqueVec2 = aVec1.Cross(aVec2) * sin(GetAngleBetweenVectors(aVec1, aVec2));

        torqueVec.Normalize();
        DirectX::SimpleMath::Vector4 torqueVec4;
        torqueVec4.x = torqueVec.x;
        torqueVec4.y = torqueVec.y;
        torqueVec4.z = torqueVec.z;
        torqueVec4.w = torqueVec2.Length();
        return torqueVec4;
    }

    static DirectX::SimpleMath::Matrix GetTorqueMat(const DirectX::SimpleMath::Vector3 aVec1, const DirectX::SimpleMath::Vector3 aVec2, const float aStepMod)
    {
        DirectX::SimpleMath::Vector3 torqueVec = GetTorqueVec(aVec1, aVec2);

        DirectX::SimpleMath::Matrix torqueMat = DirectX::SimpleMath::Matrix::Identity;
        if (torqueVec != DirectX::SimpleMath::Vector3::Zero)
        {
            torqueMat *= DirectX::SimpleMath::Matrix::CreateFromAxisAngle(torqueVec, torqueVec.Length() * aStepMod);
        }

        return torqueMat;
    }

    static DirectX::SimpleMath::Quaternion GetTorqueQuat(const DirectX::SimpleMath::Vector3 aVec1, const DirectX::SimpleMath::Vector3 aVec2, const float aStepMod)
    {
        DirectX::SimpleMath::Vector4 torqueVec4 = GetTorqueVec4(aVec1, aVec2);
        DirectX::SimpleMath::Vector3 torqueVec;
        torqueVec.x = torqueVec4.x;
        torqueVec.y = torqueVec4.y;
        torqueVec.z = torqueVec4.z;
        const float magnatude = torqueVec4.w;
        
        DirectX::SimpleMath::Quaternion torqueQuat = DirectX::SimpleMath::Quaternion::Identity;
        if (torqueVec != DirectX::SimpleMath::Vector3::Zero)
        {
            //torqueQuat *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(torqueVec, torqueVec.Length() * aStepMod);
            torqueQuat *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(torqueVec, magnatude * aStepMod);
        }
        return torqueQuat;
    }

    static Torque GetTorqueForce(const DirectX::SimpleMath::Vector3 aArm, const DirectX::SimpleMath::Vector3 aForce)
    {
        Torque torqueForce;
        torqueForce.axis = aArm.Cross(aForce);
        torqueForce.axis.Normalize();
        torqueForce.magnitude = aArm.Length() * aForce.Length() * sin(GetAngleBetweenVectors(aArm, aForce));

        return torqueForce;
    }

private:


};

