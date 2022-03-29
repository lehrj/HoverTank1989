#pragma once


// Class to handle miscellaneous functions and data structures needed across multiple classes
class Utility
{
public:
    Utility();
    static inline int GetNumericalPrecisionForUI() { return 2; }; // For setting the numerical precison displayed by UI
    static inline float GetPi() { return 3.1415926535897931; };
    static inline float ToDegrees(float r) { return r * 180.0f / GetPi(); };
    static inline float ToRadians(float d) { return d / 180.0f * GetPi(); };

    struct Torque
    {
        DirectX::SimpleMath::Vector3 axis;
        float                        magnitude;
    };

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

