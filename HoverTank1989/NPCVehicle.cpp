#include "pch.h"
#include "NPCVehicle.h"
#include "NPCController.h"

NPCVehicle::NPCVehicle()
{
    m_npcAI = std::make_unique<NpcAI>(this);
}

void NPCVehicle::CalculateImpactForce(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    float mass1 = aImpactForce.impactMass;
    //mass1 = 1.05f;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;

    m_debugData->DebugPushTestLinePositionIndicator(aImpactPos, 15.0f, 0.0f, DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
    
    // test calc kinetic energy
    DirectX::SimpleMath::Vector3 kE = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity.Length() * aImpactForce.impactVelocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    // end kinetic energy test

    DirectX::SimpleMath::Vector3 testV = aImpactForce.impactVelocity;
    testV.Normalize();
    testV *= 4000.0f;
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;
    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;
}

void NPCVehicle::CalculateImpactForce2(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    float mass1 = aImpactForce.impactMass;
    //mass1 = 1.05f;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactMass = aImpactForce.impactMass;
    m_debugData->DebugPushTestLinePositionIndicator(aImpactPos, 15.0f, 0.0f, DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 61.0f;
    impactForceToVec.impactMass = aImpactForce.impactMass;

    // test calc kinetic energy
    DirectX::SimpleMath::Vector3 kE = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity.Length() * aImpactForce.impactVelocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    // end kinetic energy test

    DirectX::SimpleMath::Vector3 testV = aImpactForce.impactVelocity;
    testV.Normalize();
    testV *= 4000.0f;
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;
    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    //impactForceToVec.impactVelocity = aImpactForce.impactVelocity;
    //impactForceToVec.impactMass = aImpactForce.impactMass;

    PushImpactForce(impactForceToVec);
    PushImpactTorque(impactTorque);
}

void NPCVehicle::CalculateImpactForce3(const VehicleData& aVehicleHit)
{
    float mass1 = aVehicleHit.mass;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aVehicleHit.q.velocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactMass = aVehicleHit.mass;

    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    impactForceToVec.impactMass = aVehicleHit.mass;

    // test calc kinetic energy
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    // end kinetic energy test

    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    DirectX::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;
    DirectX::SimpleMath::Vector3 torqueArm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    //impactForceToVec.impactVelocity = aImpactForce.impactVelocity;
    //impactForceToVec.impactMass = aImpactForce.impactMass;

    m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, impactForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;

    repulsionForceToVec.impactVelocity *= 300.0f;
    //PushImpactForce(repulsionForceToVec);

    PushImpactForce(impactForceToVec);

    //PushImpactTorque(impactTorque);
}

void NPCVehicle::CalculateImpactForce4(const VehicleData& aVehicleHit)
{
    float mass1 = aVehicleHit.mass;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aVehicleHit.q.velocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    m_vehicleStruct00.vehicleData.impactForce.impactMass = aVehicleHit.mass;

    m_vehicleStruct00.vehicleData.q.velocity = newVx2;
    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    impactForceToVec.impactMass = aVehicleHit.mass;

    // test calc kinetic energy
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;

    impactForceToVec.kineticEnergy = kE * 0.01f;
    // end kinetic energy test

    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    DirectX::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;
    DirectX::SimpleMath::Vector3 torqueArm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    //impactForceToVec.impactVelocity = aImpactForce.impactVelocity;
    //impactForceToVec.impactMass = aImpactForce.impactMass;

    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, impactForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Vector3 testImpactVelocityNorm = impactForceToVec.impactVelocity;
    testImpactVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 testKENorm = kE;
    testKENorm.Normalize();

    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;
    
    repulsionForceToVec.impactVelocity *= 300.0f;
    repulsionForceToVec.kineticEnergy = repulsionForceToVec.impactVelocity;
    //PushImpactForce(repulsionForceToVec);

    //PushImpactForce(impactForceToVec);

    PushImpactTorque(impactTorque);
}

void NPCVehicle::CalculateImpactForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    //float mass1 = aVehicleHit.mass;
    float mass1 = aImpactForce.impactMass;
    //mass1 = 1.05f;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    //DirectX::SimpleMath::Vector3 vx1 = aVehicleHit.q.velocity;
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    //m_vehicleStruct00.vehicleData.impactForce.impactMass = aVehicleHit.mass;

    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    //impactForceToVec.impactMass = aVehicleHit.mass;
    impactForceToVec.impactMass = aImpactForce.impactMass;
    impactForceToVec.impactModifier = aImpactForce.impactModifier;
    impactForceToVec.kineticEnergy = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;

    // test calc kinetic energy
    /*
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    */
    // end kinetic energy test

    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    //::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;

    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, impactForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    impactForceToVec.impactVelocity *= aImpactForce.impactModifier;
    PushImpactForce(impactForceToVec);
    
    impactTorque.magnitude *= aImpactForce.impactModifier + 1000.0f;
    PushImpactTorque(impactTorque);
}

void NPCVehicle::CalculateImpulseForce(const VehicleData& aVehicleHit)
{
    Utility::ImpulseForce impulseToVec;
    impulseToVec.currentMagnitude = 0.0f;
    impulseToVec.currentTime = 0.0f;
    //impulseToVec.directionNorm = aImpactForce.impactVelocity;
    impulseToVec.directionNorm = aVehicleHit.q.velocity;
    impulseToVec.directionNorm.Normalize();
    impulseToVec.isActive = true;
    //impulseToVec.maxMagnitude = (0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity).Length();
    impulseToVec.maxMagnitude = (0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity).Length();
    //impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.torqueArm = aVehicleHit.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;

    //impulseToVec.totalTime = 0.1f;
    float impactVelocity = (aVehicleHit.q.velocity - m_vehicleStruct00.vehicleData.q.velocity).Length();
    float impactTime = 1 / (impactVelocity + 0.00000000001);
    impulseToVec.totalTime = impactTime;

    PushImpulseForce(impulseToVec);

    Utility::ImpulseForce repulsorToVec;
    repulsorToVec.currentMagnitude = 0.0f;
    repulsorToVec.currentTime = 0.0f;
    //repulsorToVec.directionNorm = aVehicleHit.q.velocity;
    repulsorToVec.directionNorm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.q.position;
    repulsorToVec.directionNorm.Normalize();
    repulsorToVec.isActive = true;
    repulsorToVec.maxMagnitude = GetRepulsionForce(aVehicleHit).Length() * 1.0f;
    repulsorToVec.torqueArm = DirectX::SimpleMath::Vector3::Zero;
    repulsorToVec.totalTime = 0.1f;

    PushImpulseForce(repulsorToVec);

    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;

    repulsionForceToVec.impactVelocity *= 150.0f;
    repulsionForceToVec.kineticEnergy = repulsionForceToVec.impactVelocity;
    PushImpactForce(repulsionForceToVec);

}

void NPCVehicle::CalculateImpulseForceFromProjectile(const Utility::ImpactForce aImpactForce, const DirectX::SimpleMath::Vector3 aImpactPos)
{
    float mass1 = aImpactForce.impactMass;
    float mass2 = m_vehicleStruct00.vehicleData.mass;
    float e = 0.9f;

    float tmp = 1.0f / (mass1 + mass2);
    DirectX::SimpleMath::Vector3 vx1 = aImpactForce.impactVelocity;
    DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
        (1.0 + e) * mass2 * vx2 * tmp;
    DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
        (mass2 - e * mass1) * vx2 * tmp;

    float newVX1Length = newVx1.Length();
    float newVX2Length = newVx2.Length();

    Utility::ImpactForce impactForceToVec;
    impactForceToVec.impactVelocity = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
    impactForceToVec.impactVelocity *= 21.0f;
    impactForceToVec.impactMass = aImpactForce.impactMass;
    impactForceToVec.impactModifier = aImpactForce.impactModifier;
    impactForceToVec.kineticEnergy = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity;

    Utility::ImpulseForce impulseToVec;
    impulseToVec.currentMagnitude = 0.0f;
    impulseToVec.currentTime = 0.0f;
    impulseToVec.directionNorm = aImpactForce.impactVelocity;
    impulseToVec.directionNorm.Normalize();
    impulseToVec.isActive = true;
    impulseToVec.maxMagnitude = (0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity * aImpactForce.impactVelocity).Length();
    impulseToVec.torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    impulseToVec.totalTime = 0.1f;
    PushImpulseForce(impulseToVec);

    // test calc kinetic energy
    /*
    DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity * aVehicleHit.q.velocity;
    float keLength = kE.Length();
    float kEVal = 0.5f * aVehicleHit.mass * aVehicleHit.q.velocity.Length() * aVehicleHit.q.velocity.Length();
    float newtonForce = kEVal * 10.0f;
    float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
    */
    // end kinetic energy test

    /*
    //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = testV;
    //::SimpleMath::Vector3 impactForce = aVehicleHit.q.velocity * aVehicleHit.mass;
    DirectX::SimpleMath::Vector3 impactForce = aImpactForce.impactVelocity * aImpactForce.impactMass;
    //impactForce.Normalize();
    //impactForce *= 0.01f;

    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, impactForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    impactForceToVec.impactVelocity *= aImpactForce.impactModifier;
    PushImpactForce(impactForceToVec);

    impactTorque.magnitude *= aImpactForce.impactModifier + 1000.0f;
    PushImpactTorque(impactTorque);
    */
}

void NPCVehicle::CalculateSelfRightingTorque()
{
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = centerMassPos - rotorPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos;
    
    const float modVal = 0.0001f;
    DirectX::SimpleMath::Vector3 gravityForce = (DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f)) * modVal;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce = gravTorque;
}

void NPCVehicle::CalculateTopSpeed()
{
    float terminalVelocity = sqrt((2.0f * m_vehicleStruct00.vehicleData.mass * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax) 
        / (m_environment->GetAirDensity() * m_vehicleStruct00.vehicleData.frontalArea * m_vehicleStruct00.vehicleData.dragCoefficient));
    float observedTopSpeed = 50.0f; // using observed top speed for the moment while propulsion system design is still in flux, in meters per second?
    m_vehicleStruct00.vehicleData.topSpeedCalculated = observedTopSpeed;
}

void NPCVehicle::DebugToggleAI()
{
    //m_npcAI->DebugToggle();
    if (m_isGoToggleTrue == true)
    {
        m_isGoToggleTrue = false;
    }
    else
    {
        m_isGoToggleTrue = true;
    }
}

void NPCVehicle::DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 mainBodyColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 forwardColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);   
    DirectX::SimpleMath::Vector4 rearColor = DirectX::SimpleMath::Vector4(0.3f, 0.3f, 0.3f, 1.0f);
    DirectX::SimpleMath::Vector4 skirtColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    DirectX::SimpleMath::Vector4 steeringColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 jetHousingColor = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 jetHousingShellColor = DirectX::SimpleMath::Vector4(0.4f, 0.4f, 0.4f, 1.0f);
    DirectX::SimpleMath::Vector4 afterBurnColor = DirectX::SimpleMath::Vector4(1.000000000f, 0.270588249f, 0.000000000f, 1.40000000f);
    DirectX::SimpleMath::Vector4 ventColor = DirectX::SimpleMath::Vector4(0.3f, 0.3f, 0.3f, 1.0f);
    ventColor = color;
    DirectX::SimpleMath::Vector4 testColorRed = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 testColorBlue = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    //afterBurnColor = DirectX::SimpleMath::Vector4(0.000000000f, 0.0f, 0.000000000f, 1.0000000f);
    if (m_vehicleStruct00.vehicleData.isCollisionTrue == true)
    {
        color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix, aView, aProj, jetHousingColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix, aView, aProj, jetHousingShellColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix, aView, aProj, afterBurnColor);
    //m_vehicleStruct00.npcModel.afterBurnShape->Draw(m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix, aView, aProj, afterBurnColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(m_vehicleStruct00.npcModel.worldJetHousingRightMatrix, aView, aProj, jetHousingColor);
    m_vehicleStruct00.npcModel.jetHousingShape->Draw(m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix, aView, aProj, jetHousingShellColor);
    m_vehicleStruct00.npcModel.afterBurnShape->Draw(m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix, aView, aProj, afterBurnColor);
    m_vehicleStruct00.npcModel.modelShape->Draw(m_vehicleStruct00.npcModel.worldModelMatrix, aView, aProj, mainBodyColor);
    m_vehicleStruct00.npcModel.rearBodyShape->Draw(m_vehicleStruct00.npcModel.worldRearBodyMatrix, aView, aProj, jetHousingShellColor);
    m_vehicleStruct00.npcModel.rearDeckShape->Draw(m_vehicleStruct00.npcModel.worldRearDeckMatrix, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.skirtShape->Draw(m_vehicleStruct00.npcModel.worldSkirtMatrix, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.eyeShape->Draw(m_vehicleStruct00.npcModel.worldEyeLeftMatrix, aView, aProj, skirtColor);
    m_vehicleStruct00.npcModel.eyeShape->Draw(m_vehicleStruct00.npcModel.worldEyeRightMatrix, aView, aProj, skirtColor);
    m_vehicleStruct00.npcModel.forwardShape->Draw(m_vehicleStruct00.npcModel.worldForwardMatrix, aView, aProj, forwardColor);
    m_vehicleStruct00.npcModel.noseConeShape->Draw(m_vehicleStruct00.npcModel.worldNoseConeMatrix, aView, aProj, color);
    m_vehicleStruct00.npcModel.frontAirDamShape->Draw(m_vehicleStruct00.npcModel.worldFrontAirDamMatrix, aView, aProj, skirtColor);
    m_vehicleStruct00.npcModel.omniBaseShape->Draw(m_vehicleStruct00.npcModel.worldOmniBaseMatrix, aView, aProj, DirectX::Colors::Black);
    m_vehicleStruct00.npcModel.omniDialShape->Draw(m_vehicleStruct00.npcModel.worldOmniDialMatrix, aView, aProj, steeringColor);

    //m_vehicleStruct00.npcModel.rearShape->Draw(m_vehicleStruct00.npcModel.worldRearMatrix, aView, aProj, color);
    m_vehicleStruct00.npcModel.wingArmShape->Draw(m_vehicleStruct00.npcModel.worldWingArmMatrix, aView, aProj, jetHousingShellColor);
    m_vehicleStruct00.npcModel.wingShape->Draw(m_vehicleStruct00.npcModel.worldWingMatrix, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.wingFinShape->Draw(m_vehicleStruct00.npcModel.worldWingFinLeftMatrix, aView, aProj, jetHousingShellColor);
    m_vehicleStruct00.npcModel.wingFinShape->Draw(m_vehicleStruct00.npcModel.worldWingFinRightMatrix, aView, aProj, jetHousingShellColor);

    m_vehicleStruct00.npcModel.steeringShape->Draw(m_vehicleStruct00.npcModel.worldSteeringMatrix, aView, aProj, steeringColor);

    DirectX::XMVECTORF32 throttleColor = DirectX::Colors::Green;
    /*
    if (m_vehicleStruct00.vehicleData.controlInput.throttleInput < 0.0f)
    {
        throttleColor = DirectX::Colors::Red;
    }
    */

    //m_vehicleStruct00.npcModel.throttleShape->Draw(m_vehicleStruct00.npcModel.worldThrottleMatrix, aView, aProj, rearColor);

    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix1, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix2, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix3, aView, aProj, testColorRed);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix4, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix5, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix6, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix7, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix8, aView, aProj, ventColor);
    m_vehicleStruct00.npcModel.ventShape->Draw(m_vehicleStruct00.npcModel.worldVentMatrix9, aView, aProj, ventColor);

    DirectX::SimpleMath::Vector4 testColor = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    if (m_npcAI->GetIsAvoidanceTrue() == true)
    {
        testColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    DirectX::BoundingBox avoidBox = m_npcAI->GetAiAvoidanceBox();
    DirectX::SimpleMath::Vector3 testSize = avoidBox.Extents;
    testSize *= 2.0f;
    m_vehicleStruct00.npcModel.avoidanceShape = DirectX::GeometricPrimitive::CreateBox(m_context.Get(), testSize);
    //m_vehicleStruct00.npcModel.avoidanceShape->Draw(m_npcAI->GetAiAvoidanceBoxAlignment(), aView, aProj, testColor, nullptr, true);

    /*
    DirectX::SimpleMath::Vector4 testColor2 = DirectX::SimpleMath::Vector4(0.5f, 1.0f, 0.5f, 1.0f);
    DirectX::SimpleMath::Vector3 testSize2 = m_vehicleStruct00.vehicleData.collisionBox.Extents;
    testSize2 *= 2.0f;
    m_vehicleStruct00.npcModel.collisionShape = DirectX::GeometricPrimitive::CreateBox(m_context.Get(), testSize2);
    m_vehicleStruct00.npcModel.collisionShape->Draw(m_vehicleStruct00.npcModel.worldModelMatrix, aView, aProj, testColor2);
    */
}

bool NPCVehicle::CheckVehiclePenetration(DirectX::SimpleMath::Vector3 aPos)
{
    DirectX::BoundingBox updatedCollision = m_vehicleStruct00.vehicleData.collisionBox;
    updatedCollision.Center = aPos;

    bool isPosPenetrating = false;
    std::vector<NPCVehicle*>& npcVec = m_npcController->GetVec();
    for (int i = 0; i < npcVec.size(); ++i)
    {
        int vID = npcVec[i]->GetID();
        if (m_vehicleStruct00.vehicleData.id != npcVec[i]->GetID())
        {
            if (updatedCollision.Intersects(npcVec[i]->GetCollisionData()) == true)
            {
                isPosPenetrating = true;
            }
        }
    }

    return isPosPenetrating;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetAntiGravGravityForce(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;
    if (aVehicleData.altitude < lowerCurveBound)
    {
        float metersBelowLowerBound = aVehicleData.altitude - lowerCurveBound;
        float reboundMod = 0.1f;
        gravForce *= metersBelowLowerBound * reboundMod;
    }
    else if (aVehicleData.altitude > upperCurveBound)
    {
        gravForce = m_environment->GetGravityVec();
    }
    else
    {
        const float currentCurvePos = (aVehicleData.altitude / upperCurveBound);
        gravForce = gravForce * currentCurvePos;
    }

    //return gravForce * aVehicleData.mass;
    return gravForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetDamperForce(const VehicleData& aVehicleData)
{
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverNeutralBoyantAlt;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;
    const float damperConstant = 19.1f;
    DirectX::SimpleMath::Vector3 currentVelocity = aVehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 damperForce = DirectX::SimpleMath::Vector3::Zero;
    if (aVehicleData.altitude > midCurveBound && aVehicleData.altitude < upperCurveBound)
    {
        if (currentVelocity.y < 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }
    else if (aVehicleData.altitude < midCurveBound && aVehicleData.altitude > lowerCurveBound)
    {
        if (currentVelocity.y > 0.0f)
        {
            damperForce.y = -currentVelocity.y * damperConstant;

        }
    }
    //DebugPushUILineDecimalNumber("damperForce.y : ", damperForce.y, "");
    //return damperForce * aVehicleData.mass;
    return damperForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetForwardThrust(const VehicleData& aVehicleData)
{
    DirectX::SimpleMath::Vector3 thrustUpdate = aVehicleData.forward * (aVehicleData.hoverData.forwardThrust);
    return thrustUpdate;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetHoverLift(const VehicleData& aVehicleData)
{
    //DirectX::SimpleMath::Vector3 liftForce = aVehicleData.hoverData.hoverLiftNeutralWithGrav;
    DirectX::SimpleMath::Vector3 liftForce = aVehicleData.up * 9.8f;
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;

    if (aVehicleData.altitude < lowerCurveBound)
    {
        liftForce *= 3.0f;
    }
    else if (aVehicleData.altitude > upperCurveBound)
    {
        liftForce = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        const float currentCurvePos = (aVehicleData.altitude / upperCurveBound);
        liftForce *= currentCurvePos;
    }

    return liftForce;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetImpactForceSum(const VehicleData& aVehicleData)
{
    const float timeStep = 1.0f;
    DirectX::SimpleMath::Vector3 impactForce = DirectX::SimpleMath::Vector3::Zero;
    for (int i = 0; i < aVehicleData.impactForceVec.size(); ++i)
    {
        //DirectX::SimpleMath::Vector3 force = (2.0 * aVehicleData.impactForceVec[i].impactMass * aVehicleData.impactForceVec[i].impactVelocity) / timeStep;
        //impactForce += aVehicleData.impactForceVec[i].impactVelocity;

        DirectX::SimpleMath::Vector3 force;

        // test calc kinetic energy
        DirectX::SimpleMath::Vector3 kE = 0.5f * aVehicleData.impactForceVec[i].impactMass * aVehicleData.impactForceVec[i].impactVelocity * aVehicleData.impactForceVec[i].impactVelocity;
        float keLength = kE.Length();
        //float kEVal = 0.5f * aImpactForce.impactMass * aImpactForce.impactVelocity.Length() * aImpactForce.impactVelocity.Length();
        //float newtonForce = kEVal * 10.0f;
        //float testTwMass = newtonForce / m_vehicleStruct00.vehicleData.mass;
        // end kinetic energy test

        float mass1 = aVehicleData.impactForceVec[i].impactMass;
        //mass1 = 1.05f;
        float mass2 = m_vehicleStruct00.vehicleData.mass;
        float e = 0.1f;

        float tmp = 1.0f / (mass1 + mass2);
        DirectX::SimpleMath::Vector3 vx1 = aVehicleData.impactForceVec[i].impactVelocity;
        DirectX::SimpleMath::Vector3 vx2 = m_vehicleStruct00.vehicleData.q.velocity;
        DirectX::SimpleMath::Vector3 newVx1 = (mass1 - e * mass2) * vx1 * tmp +
            (1.0 + e) * mass2 * vx2 * tmp;
        DirectX::SimpleMath::Vector3 newVx2 = (1.0 + e) * mass1 * vx1 * tmp +
            (mass2 - e * mass1) * vx2 * tmp;

        float newVX1Length = newVx1.Length();
        float newVX2Length = newVx2.Length();
        //m_vehicleStruct00.vehicleData.impactForce.impactVelocity = newVx1 - m_vehicleStruct00.vehicleData.q.velocity;
        force = newVx2 - m_vehicleStruct00.vehicleData.q.velocity;
        //m_vehicleStruct00.vehicleData.impactForce.impactMass = aImpactForce.impactMass;
        //m_debugData->DebugPushTestLinePositionIndicator(aImpactPos, 15.0f, 0.0f, DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));

        //impactForce += force;
        //impactForce += aVehicleData.impactForceVec[i].impactVelocity;
        impactForce += aVehicleData.impactForceVec[i].kineticEnergy;
    }
    
    return impactForce;
}

Utility::Torque NPCVehicle::GetImpactTorqueSum(const VehicleData& aVehicleData)
{
    Utility::Torque impactTorque;
    impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    impactTorque.magnitude = 0.0f;

    for (int i = 0; i < aVehicleData.impactTorqueVec.size(); ++i)
    {
        impactTorque.axis += aVehicleData.impactTorqueVec[i].axis * aVehicleData.impactTorqueVec[i].magnitude;
        impactTorque.magnitude += aVehicleData.impactTorqueVec[i].magnitude;
    }
    impactTorque.axis.Normalize();
    //m_debugData->DebugPushUILineDecimalNumber("impactTorqueMag = ", impactTorque.magnitude, "");
    return impactTorque;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetOmniDirectionalThrust(const VehicleData& aVehicleData)
{
    //DirectX::SimpleMath::Vector3 thrustUpdate = aVehicleData.controlInput.steeringVec * (aVehicleData.hoverData.forwardThrust);

    DirectX::SimpleMath::Vector3 thrustDir = aVehicleData.forward;
    thrustDir = DirectX::SimpleMath::Vector3::Transform(thrustDir, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aVehicleData.up, aVehicleData.controlInput.angleToDestination));
    thrustDir.Normalize();
    //m_debugData->DebugPushTestLine(aVehicleData.q.position, thrustDir, 15.0f, 20.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    thrustDir *= aVehicleData.hoverData.omniThrustMax;

    DirectX::SimpleMath::Vector3 omniThrust = aVehicleData.hoverData.omniThrustVec;
    return omniThrust;
}

DirectX::SimpleMath::Vector3 NPCVehicle::GetRepulsionForce(const VehicleData& aRepulsorVehicleData)
{
    DirectX::SimpleMath::Vector3 repulsionForce = DirectX::SimpleMath::Vector3::Zero;
    const float repulsionForceMax = 1.0f * m_vehicleStruct00.vehicleData.mass;
    DirectX::SimpleMath::Vector3 repulsionForceNorm = m_vehicleStruct00.vehicleData.q.position - aRepulsorVehicleData.q.position;
    const float distance = repulsionForceNorm.Length();
    repulsionForceNorm.Normalize();
    float ratio = (1.0f + (distance / (m_vehicleStruct00.vehicleData.maxCollisionDetectionRange + aRepulsorVehicleData.maxCollisionDetectionRange)));
    //float ratio = (1.0f + (distance / (m_vehicleStruct00.vehicleData.maxCollisionDetectionRange)));
    repulsionForce = repulsionForceNorm * (ratio * repulsionForceMax);

    return repulsionForce;
}

void NPCVehicle::InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    NPCModel& aModel, const DirectX::SimpleMath::Vector3 aDimensions)
{
    const float zFightOffset = 0.02f;
    DirectX::SimpleMath::Matrix centerMassTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(aDimensions.x * 0.0f, -aDimensions.y * 0.0f, aDimensions.z * 0.0f);

    DirectX::SimpleMath::Vector3 mainBodySize = aDimensions;
    mainBodySize.x -= zFightOffset * 2.0f;
    mainBodySize.y -= zFightOffset * 2.0f;
    mainBodySize.z *= 0.925f;

    const float mainBodyOffset = 2.0f;
    mainBodySize.x -= mainBodyOffset;
    const float mainBodyCutOff = 3.0f;
    mainBodySize.x -= mainBodyCutOff;
    DirectX::SimpleMath::Vector3 mainBodyTranslation(-(mainBodyOffset * 0.5f) + (mainBodyCutOff * 0.5f), 0.0f, 0.0f);
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), mainBodySize);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localModelMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(mainBodyTranslation);
    aModel.localModelMatrix *= centerMassTranslation;
    aModel.worldModelMatrix = aModel.localModelMatrix;

    // rear body
    DirectX::SimpleMath::Vector3 rearBodySize = aDimensions;
    rearBodySize.x *= 0.3f;
    rearBodySize.y *= 0.95f;
    rearBodySize.z *= 1.02f;
    const float rearBodyOffset = 2.0f;
    rearBodySize.x -= rearBodyOffset;
    DirectX::SimpleMath::Vector3 rearBodyTranslation = DirectX::SimpleMath::Vector3(-rearBodyOffset * 0.5f, 0.0f, 0.0f);
    rearBodyTranslation.x += -3.0f;
    rearBodyTranslation.y += 0.3f;
    rearBodyTranslation.z += 0.0f;
    aModel.rearBodyShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearBodySize);
    aModel.localRearBodyMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localRearBodyMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearBodyTranslation);
    aModel.localRearBodyMatrix *= centerMassTranslation;
    aModel.worldRearBodyMatrix = aModel.localRearBodyMatrix;

    // rear deck
    const float rearDeckOffset = 2.0f;
    const float rearDeckSize = 5.5f;
    DirectX::SimpleMath::Vector3 rearDeckScale(0.9f, 1.0f, 0.6f);
    DirectX::SimpleMath::Vector3 rearDeckTranslation = DirectX::SimpleMath::Vector3(-rearDeckOffset * 0.5f, 0.0f, 0.0f);
    rearDeckTranslation.x += -4.0f;
    rearDeckTranslation.x += -(rearDeckSize * rearDeckScale.x) * 0.1f;
    rearDeckTranslation.y += -1.8f;
    rearDeckTranslation.y += (rearDeckSize * rearDeckScale.y) * 0.5f;
    rearDeckTranslation.z += 0.0f;
    //const float rearDeckSize = (aDimensions.z * 0.5f) * sqrt(2.0f);
    
    aModel.rearDeckShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), rearDeckSize);
    aModel.localRearDeckMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(45.0f));
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(55.0f));
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateScale(rearDeckScale);
    /*
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(14.0f));
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    */
    aModel.localRearDeckMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearDeckTranslation);
    aModel.localRearDeckMatrix *= centerMassTranslation;
    aModel.worldRearDeckMatrix = aModel.localRearDeckMatrix;

    // Jet housing left
    const float jetHousingDiameter = aDimensions.y * 0.3f;
    const float jetHousingThickness = 0.5f;
    aModel.jetHousingShape = DirectX::GeometricPrimitive::CreateTorus(aContext.Get(), jetHousingDiameter, jetHousingThickness);
    DirectX::SimpleMath::Vector3 jetHousingLeftTranslation;
    jetHousingLeftTranslation.x = -aDimensions.x * 0.325f;
    jetHousingLeftTranslation.y = aDimensions.y * 0.2f;
    jetHousingLeftTranslation.z = -(aDimensions.z * 0.5f) - (jetHousingDiameter * 0.5f) + (jetHousingThickness * -0.3f);

    //jetHousingLeftTranslation.x = 0.0f;
    //jetHousingLeftTranslation.y = 0.5f;
    //jetHousingLeftTranslation.z = -5.5f;

    aModel.localJetHousingLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    DirectX::SimpleMath::Matrix jetHousingScale = DirectX::SimpleMath::Matrix::CreateScale(11.0f, 1.0f, 1.0f);
    //DirectX::SimpleMath::Matrix jetHousingScale = DirectX::SimpleMath::Matrix::CreateScale(36.0f, 1.0f, 1.0f);
    aModel.localJetHousingLeftMatrix *= jetHousingScale;
    aModel.localJetHousingLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);
    aModel.worldJetHousingLeftMatrix = aModel.localJetHousingLeftMatrix;

    // Jet housing shell left
    aModel.localJetHousingShellLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingShellLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingShellLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    DirectX::SimpleMath::Matrix jetHousingShellScale = DirectX::SimpleMath::Matrix::CreateScale(8.0f, 1.1f, 1.1f);
    //DirectX::SimpleMath::Matrix jetHousingShellScale = DirectX::SimpleMath::Matrix::CreateScale(32.0f, 1.1f, 1.1f);
    aModel.localJetHousingShellLeftMatrix *= jetHousingShellScale;
    aModel.localJetHousingShellLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingLeftTranslation);
    aModel.worldJetHousingShellLeftMatrix = aModel.localJetHousingShellLeftMatrix;

    // Jet housing right
    DirectX::SimpleMath::Vector3 jetHousingRightTranslation = jetHousingLeftTranslation;

    //jetHousingRightTranslation.x = -aDimensions.x * 0.25f;
    //jetHousingRightTranslation.y = aDimensions.y * 0.2f;
    jetHousingRightTranslation.z = -jetHousingLeftTranslation.z;

    /*
    jetHousingRightTranslation.x = -6.5f;
    jetHousingRightTranslation.y = 0.0f;
    jetHousingRightTranslation.z = 3.5f;
    */

    aModel.localJetHousingRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    aModel.localJetHousingRightMatrix *= jetHousingScale;
    aModel.localJetHousingRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingRightTranslation);
    aModel.worldJetHousingRightMatrix = aModel.localJetHousingRightMatrix;

    // Jet housing shell right
    aModel.localJetHousingShellRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localJetHousingShellRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localJetHousingShellRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(30.0f));
    aModel.localJetHousingShellRightMatrix *= jetHousingShellScale;
    aModel.localJetHousingShellRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(jetHousingRightTranslation);
    aModel.worldJetHousingShellRightMatrix = aModel.localJetHousingShellRightMatrix;

    aModel.afterBurnLeftFlicker = 0.0f;
    aModel.afterBurnRightFlicker = 0.0f;
    aModel.afterBurnFlickerRate = 100.1f;
    // Jet after burn left
    const float afterBurnDiameter = jetHousingDiameter * 0.9f;
    const float afterBurnHeight = 0.1f;
    aModel.afterBurnShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), afterBurnDiameter, afterBurnHeight, 3);
    DirectX::SimpleMath::Vector3 afterBurnLowerLeftTranslation;
    afterBurnLowerLeftTranslation = jetHousingLeftTranslation;
    //afterBurnLowerLeftTranslation.x += -jetHousingDiameter * 0.7f;
    afterBurnLowerLeftTranslation.y += 0.0f;
    //afterBurnLowerLeftTranslation.z = 0.0f;
    aModel.localAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localAfterBurnLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(afterBurnLowerLeftTranslation);
    aModel.worldAfterBurnLeftMatrix = aModel.localAfterBurnLeftMatrix;

    // foward burn left
    DirectX::SimpleMath::Vector3 forwardBurnLeftTranslation;
    forwardBurnLeftTranslation = jetHousingLeftTranslation;
    forwardBurnLeftTranslation.x += jetHousingDiameter * 0.7f;
    forwardBurnLeftTranslation.y += 0.0f;
    //forwardBurnLeftTranslation.z += 0.0f;
    aModel.localForwardBurnLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localForwardBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localForwardBurnLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(forwardBurnLeftTranslation);
    aModel.worldForwardBurnLeftMatrix = aModel.localForwardBurnLeftMatrix;

    // Jet after burn right
    DirectX::SimpleMath::Vector3 afterBurnRightTranslation;
    afterBurnRightTranslation = jetHousingRightTranslation;
    afterBurnRightTranslation.y += 0.0f;

    aModel.localAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(90.0f));
    aModel.localAfterBurnRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
    aModel.localAfterBurnRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(afterBurnRightTranslation);
    aModel.worldAfterBurnRightMatrix = aModel.localAfterBurnRightMatrix;
    // end

    DirectX::SimpleMath::Vector3 skirtSize(aDimensions);
    skirtSize.x *= 1.1f;
    //skirtSize.x *= 0.4f;
    //skirtSize.x *= 0.45f;
    skirtSize.y *= 0.5f;
    skirtSize.z *= 1.01f;
    DirectX::SimpleMath::Vector3 skirtTranslation;
    skirtTranslation.x = -0.3f;
    skirtTranslation.y = (-aDimensions.y * 0.5f) + (skirtSize.y * 0.5f) - zFightOffset;
    skirtTranslation.z = 0.0f;
    aModel.skirtShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), skirtSize);
    aModel.skirtShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), skirtSize.z, 1.0f, 4);
    aModel.localSkirtMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localSkirtMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-55.0f));
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(90.0f));
    const float skirtRot = 15.0f;
    //aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-skirtRot));
    //aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.5f, 1.0f, 1.0f);
    //aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(6.0f));
    //aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(180.0f));
    //aModel.localSkirtMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-55.0f));
    //aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateScale(16.5f, 3.5f, 1.0f);
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateScale(19.8f, 5.0f, 1.0f);
    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(3.0f));
    //skirtTranslation.y = (-aDimensions.y * 0.4f);
    skirtTranslation.y = (-aDimensions.y * 0.15f);
    skirtTranslation.y = -1.85f;

    aModel.localSkirtMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(skirtTranslation);
    aModel.localSkirtMatrix *= centerMassTranslation;
    aModel.worldSkirtMatrix = aModel.localSkirtMatrix;

    DirectX::SimpleMath::Vector3 noseConeSize(aDimensions);
    noseConeSize.x *= 1.0f;
    noseConeSize.y *= 1.0f;
    noseConeSize.z *= 1.0f;
    DirectX::SimpleMath::Vector3 noseConeTranslation;
    noseConeTranslation.x = (aDimensions.x * 0.5f) - (mainBodyOffset * 1.0f);
    noseConeTranslation.y = 0.0f;
    noseConeTranslation.z = 0.0f;
    const float noseSize = (aDimensions.z * 0.5f) * sqrt(2.0f);
    aModel.noseConeShape = DirectX::GeometricPrimitive::CreateOctahedron(aContext.Get(), noseSize);
    //aModel.noseConeShape = DirectX::GeometricPrimitive::CreateCone(aContext.Get(), 10.0, 3.0f, 4);
    aModel.localNoseConeMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(45.0f));
    //aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-90.0f));
    //const float yScale = 1.0f / (aDimensions.y * 0.5f) * sqrt(6.0f);
    const float yScale = 1.0f / (aDimensions.y * 0.5f) * sqrt(6.0f);
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateScale(0.38f, yScale, 1.0f);
    aModel.localNoseConeMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(noseConeTranslation);
    aModel.localNoseConeMatrix *= centerMassTranslation;
    aModel.worldNoseConeMatrix = aModel.localNoseConeMatrix;

    // eye left
    DirectX::SimpleMath::Vector3 eyeSize;
    eyeSize.x = 2.5f;
    eyeSize.y = 1.0f;
    eyeSize.z = 1.0f;
    DirectX::SimpleMath::Vector3 eyeLeftTranslation = noseConeTranslation;
    eyeLeftTranslation.x = aDimensions.x * 0.5f;
    eyeLeftTranslation.x += -1.2f;
    eyeLeftTranslation.y = aDimensions.y * 0.14f;
    eyeLeftTranslation.z = -aDimensions.z * 0.25f;
    aModel.eyeShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), eyeSize);
    //aModel.eyeShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), eyeSize.z, eyeSize.x, 3);
    aModel.localEyeLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(24.0f));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-12.0f));
    const float eyeInflection = 33.0f;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-eyeInflection));
    const float eyeRotation = 62.0f;
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-eyeRotation));
    aModel.localEyeLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(eyeLeftTranslation);
    aModel.localEyeLeftMatrix *= centerMassTranslation;
    aModel.worldEyeLeftMatrix = aModel.localEyeLeftMatrix;

    // eye right
    DirectX::SimpleMath::Vector3 eyeRightTranslation = eyeLeftTranslation;
    //eyeRightTranslation.x = aDimensions.x * 0.47f;
    //eyeRightTranslation.y = aDimensions.y * 0.1f;
    eyeRightTranslation.z *= -1.0f;
    aModel.localEyeRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(24.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-12.0f));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-eyeInflection));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(eyeRotation));
    aModel.localEyeRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(eyeRightTranslation);
    aModel.localEyeRightMatrix *= centerMassTranslation;
    aModel.worldEyeRightMatrix = aModel.localEyeRightMatrix;

    // front air dam
    DirectX::SimpleMath::Vector3 frontAirDamSize(aDimensions);
    frontAirDamSize.x *= 0.2f;
    frontAirDamSize.y *= 0.4f;
    //frontAirDamSize.z *= 0.75f;
    frontAirDamSize.z *= 0.9f;
    DirectX::SimpleMath::Vector3 frontAirDamTranslation;
    frontAirDamTranslation.x = aDimensions.x * 0.37f;
    frontAirDamTranslation.x = aDimensions.x * 0.45f;
    frontAirDamTranslation.y = -aDimensions.y * 0.4f;
    //frontAirDamTranslation.y = -aDimensions.y * 0.3f;
    frontAirDamTranslation.z = 0.0f;
    //aModel.frontAirDamShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), frontAirDamSize);
    aModel.frontAirDamShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), frontAirDamSize.z, frontAirDamSize.x, 3);
    aModel.localFrontAirDamMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(8.0f));
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(-90.0f));
    aModel.localFrontAirDamMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(frontAirDamTranslation);
    aModel.localFrontAirDamMatrix *= centerMassTranslation;
    aModel.worldFrontAirDamMatrix = aModel.localFrontAirDamMatrix;


    const float omniBaseDiameter = aDimensions.x * 0.32f;
    const float omniBaseHeight = aDimensions.y * 1.05f;
    aModel.omniBaseShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), omniBaseHeight, omniBaseDiameter);
    DirectX::SimpleMath::Vector3 omniBaseTranslation;
    omniBaseTranslation.x = -omniBaseDiameter * 0.18f;
    omniBaseTranslation.y = 0.0f;
    omniBaseTranslation.z = 0.0f;
    aModel.localOmniBaseMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localOmniBaseMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(omniBaseTranslation);
    aModel.localOmniBaseMatrix *= centerMassTranslation;
    aModel.worldForwardMatrix = aModel.localOmniBaseMatrix;

    const float omniDialDiameter = omniBaseDiameter * 0.3f;
    aModel.omniDialShape = DirectX::GeometricPrimitive::CreateSphere(aContext.Get(), omniDialDiameter);
    DirectX::SimpleMath::Vector3 omniDialTranslation = omniBaseTranslation;
    omniDialTranslation.y = aDimensions.y * 0.5f;
    aModel.localOmniDialMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localOmniDialMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(omniDialTranslation);
    aModel.localOmniDialMatrix *= centerMassTranslation;
    aModel.worldOmniDialMatrix = aModel.localOmniDialMatrix;
    aModel.omniDialRadius = (omniBaseDiameter * 0.5f) - (omniDialDiameter * 0.5f);

    // wing arm
    DirectX::SimpleMath::Vector3 wingArmSize(2.0f, 1.0f, 1.0f);
    wingArmSize.x *= 1.0f;
    wingArmSize.y *= 1.0f;
    wingArmSize.z *= 1.0f;
    aModel.wingArmShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingArmSize);
    DirectX::SimpleMath::Vector3 wingArmTranslation = rearDeckTranslation;
    wingArmTranslation.x += 1.6f;
    wingArmTranslation.y += rearDeckSize * 0.6f;
    wingArmTranslation.z = 0.0f;

    DirectX::SimpleMath::Vector3 wingArmLocalTranslation;
    wingArmLocalTranslation.x = - wingArmSize.x * 0.85f;
    wingArmLocalTranslation.y = 0.0f;
    wingArmLocalTranslation.z = 0.0f;

    aModel.localWingArmMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingArmLocalTranslation);
    //aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingArmTranslation);
    aModel.localWingArmTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingArmTranslation);
    aModel.localWingArmMatrix *= centerMassTranslation;
    aModel.worldWingArmMatrix = aModel.localWingArmMatrix;

    DirectX::SimpleMath::Vector3 testVec = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f)));
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f));
    testVec = DirectX::SimpleMath::Vector3::Transform(testVec, DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f)));


    // wing
    DirectX::SimpleMath::Vector3 wingSize(2.0f, 1.0f, 1.0f);
    wingSize.x *= 1.0f;
    wingSize.y *= 0.5f;
    wingSize.z *= aDimensions.z;
    aModel.wingShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingSize);
    DirectX::SimpleMath::Vector3 wingTranslation = wingArmTranslation;
    //wingTranslation.x += -0.36 * wingSize.x;
    wingTranslation.y += wingSize.y * 0.5f;
    wingTranslation.z += 0.0f;

    DirectX::SimpleMath::Vector3 wingLocalTranslation = wingArmLocalTranslation;
    //wingLocalTranslation.x += -(wingSize.x * 1.0f) * 0.02564895;
    wingLocalTranslation.x += -0.36 * 1.0f;
    wingLocalTranslation.y += wingSize.y * 1.0f;
    wingLocalTranslation.z += 0.0f;

    aModel.localWingMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingLocalTranslation);
    //aModel.localWingArmMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingArmTranslation);
    aModel.localWingTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingTranslation);
    aModel.localWingMatrix *= centerMassTranslation;
    aModel.worldWingMatrix = aModel.localWingMatrix;

    // wing fin left
    DirectX::SimpleMath::Vector3 wingFinSize(2.0f, 2.0f, 1.0f);
    wingFinSize.x *= 1.0f;
    wingFinSize.y *= 1.0f;
    wingFinSize.z *= 0.5f;
    aModel.wingFinShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), wingFinSize);
    DirectX::SimpleMath::Vector3 wingFinLeftTranslation = wingTranslation;
    wingFinLeftTranslation.x += 0.0f;
    wingFinLeftTranslation.y += 0.0f;
    wingFinLeftTranslation.z += 0.0f;

    DirectX::SimpleMath::Vector3 wingFinLeftLocalTranslation = wingLocalTranslation;
    //wingFinLeftLocalTranslation.x += -0.36 * 1.0f;
    wingFinLeftLocalTranslation.x += 0.0f;
    //wingFinLeftLocalTranslation.y += wingSize.y * 1.0f;
    wingFinLeftLocalTranslation.y += 0.0f;
    wingFinLeftLocalTranslation.z += (-wingSize.z * 0.5f) + (-wingFinSize.z * 0.5f);

    aModel.localWingFinLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingFinLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingFinLeftLocalTranslation);
    aModel.localWingFinLeftTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingFinLeftTranslation);
    aModel.localWingFinLeftMatrix *= centerMassTranslation;
    aModel.worldWingFinLeftMatrix = aModel.localWingFinLeftMatrix;

    // wing fin right
    DirectX::SimpleMath::Vector3 wingFinRightTranslation = wingTranslation;
    wingFinRightTranslation.x += 0.0f;
    wingFinRightTranslation.y += 0.0f;
    wingFinRightTranslation.z += 0.0f;

    DirectX::SimpleMath::Vector3 wingFinRightLocalTranslation = wingLocalTranslation;
    //wingFinRightLocalTranslation.x += -0.36 * 1.0f;
    wingFinRightLocalTranslation.x += 0.0f;
    //wingFinRightLocalTranslation.y += wingSize.y * 1.0f;
    wingFinRightLocalTranslation.y += 0.0f;
    wingFinRightLocalTranslation.z += (wingSize.z * 0.5f) + (wingFinSize.z * 0.5f);

    aModel.localWingFinRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(14.0f));
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 1.0f);
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-7.0f));
    aModel.localWingFinRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(wingFinRightLocalTranslation);
    aModel.localWingFinRightTranslationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(wingFinRightTranslation);
    aModel.localWingFinRightMatrix *= centerMassTranslation;
    aModel.worldWingFinRightMatrix = aModel.localWingFinRightMatrix;

    // old wing
    DirectX::SimpleMath::Vector3 rearShapeSize = aDimensions;
    rearShapeSize.x *= 0.2f;
    rearShapeSize.y *= 0.1f;
    rearShapeSize.z *= 1.05f;
    aModel.rearShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearShapeSize);
    DirectX::SimpleMath::Vector3 rearShapeTranslation;
    rearShapeTranslation.x = -(aDimensions.x * 0.5f) + (rearShapeSize.x * 0.45f);
    rearShapeTranslation.x += -0.3f;
    rearShapeTranslation.y = aDimensions.y * 0.51f;
    rearShapeTranslation.y += .5f;
    rearShapeTranslation.z = 0.0f;
    aModel.localRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-0.0f));
    aModel.localRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearShapeTranslation);
    aModel.localRearMatrix *= centerMassTranslation;
    aModel.worldRearMatrix = aModel.localRearMatrix;

    DirectX::SimpleMath::Vector3 throttleShapeSize = rearShapeSize;
    throttleShapeSize.x *= 1.05f;
    throttleShapeSize.y *= 1.05f;
    throttleShapeSize.z *= 1.05f;
    aModel.throttleShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), throttleShapeSize);
    DirectX::SimpleMath::Vector3 throttleShapeTranslation;
    throttleShapeTranslation = rearShapeTranslation;
    aModel.localThrottleMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(-15.0f));
    aModel.localThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(throttleShapeTranslation);
    aModel.localThrottleMatrix *= centerMassTranslation;
    aModel.worldThrottleMatrix = aModel.localThrottleMatrix;

    DirectX::SimpleMath::Vector3 forwardShapeSize = aDimensions;
    forwardShapeSize.x *= 0.85f;
    //forwardShapeSize.y *= 1.05f;
    forwardShapeSize.y *= 0.52f;
    forwardShapeSize.z *= 1.05f;
    aModel.forwardShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), forwardShapeSize.y, forwardShapeSize.x, 3);
    DirectX::SimpleMath::Vector3 forwardShapeTranslation;
    //forwardShapeTranslation.x = (aDimensions.x * 0.5f) - (forwardShapeSize.x * 0.34f);
    forwardShapeTranslation.x = (aDimensions.x * 0.5f) - (forwardShapeSize.x * 0.292f);
    forwardShapeTranslation.y = aDimensions.y * 0.26f;
    forwardShapeTranslation.z = 0.0f;
    aModel.localForwardMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.7f, 1.0f, 0.9f));
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(forwardShapeTranslation);
    aModel.localForwardMatrix *= centerMassTranslation;
    aModel.worldForwardMatrix = aModel.localModelMatrix;

    DirectX::SimpleMath::Vector3 steeringShapeSize = forwardShapeSize;
    steeringShapeSize.x *= 0.85f;
    steeringShapeSize.y *= 1.05f;
    steeringShapeSize.z *= 1.05f;
    aModel.steeringShape = DirectX::GeometricPrimitive::CreateCylinder(aContext.Get(), steeringShapeSize.y, steeringShapeSize.x, 3);
    DirectX::SimpleMath::Vector3 steeringShapeTranslation = forwardShapeTranslation;
    steeringShapeTranslation.x *= 1.1f;
    aModel.localSteeringMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateRotationX(Utility::ToRadians(0.0f));
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-30.0f));
    aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(0.45f, 1.0f, 0.25f));
    //aModel.localSteeringMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.45f, 1.5f, 0.25f));
    aModel.translationSteeringMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(steeringShapeTranslation);
    aModel.translationSteeringMatrix *= centerMassTranslation;
    aModel.worldSteeringMatrix = aModel.localSteeringMatrix;

    DirectX::SimpleMath::Vector3 ventSize = aDimensions;
    //ventSize.x *= 0.8f;
    //ventSize.x *= 0.5f;
    ventSize.x = mainBodySize.x;
    ventSize.y *= 0.02f;
    //ventSize.z *= 0.95f;
    ventSize.z *= 1.0f;
    aModel.ventShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), ventSize);

    const float ventOffsetY = 0.7f;
    const DirectX::SimpleMath::Vector3 ventOffSet(0.0f, -0.5f, 0.0f);
    DirectX::SimpleMath::Vector3 ventScale(1.0f, 1.0f, 1.0f);
    //DirectX::SimpleMath::Vector3 ventScaleOffSet(0.0f, 0.0f, -0.1f);
    DirectX::SimpleMath::Vector3 ventScaleOffSet(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 ventTranslation(-0.3f, 0.0f, 0.0f);
    ventTranslation.x = (-aDimensions.x * 0.5f) + (ventSize.x * 0.49f);
    ventTranslation.x = -mainBodyOffset * 1.0f;
    ventTranslation.x = mainBodyTranslation.x;
    ventTranslation.y = (aDimensions.y * 0.5f) - (ventSize.y * 0.5f) + zFightOffset;
    ventTranslation.z = 0.0f;

    //ventTranslation.y += ventOffsetY;
    //ventTranslation.y += ventOffsetY;
    const float topVentScale = 5.0f;
    DirectX::SimpleMath::Vector3 topVentTranslation = ventTranslation;
    const float topVentOffsetY = -((ventSize.y * 0.5f) * topVentScale) + (zFightOffset * 2.5f);
    topVentTranslation.y += topVentOffsetY;
    topVentTranslation.z += 0.0f;
    aModel.localVentMatrix1 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix1 *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, topVentScale, 1.0f);
    aModel.localVentMatrix1 *= DirectX::SimpleMath::Matrix::CreateTranslation(topVentTranslation);
    aModel.worldVentMatrix1 = aModel.localVentMatrix1;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    //ventTranslation.y -= -topVentOffsetY - ventOffSet.y;
    ventTranslation.y -= -topVentOffsetY * 2.0f;
    aModel.localVentMatrix2 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix2 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix2 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix2 = aModel.localVentMatrix2;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix3 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix3 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix3 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix3 = aModel.localVentMatrix3;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix4 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix4 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix4 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix4 = aModel.localVentMatrix4;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix5 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix5 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix5 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix5 = aModel.localVentMatrix5;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix6 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix6 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix6 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix6 = aModel.localVentMatrix6;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix7 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix7 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix7 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix7 = aModel.localVentMatrix7;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix8 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix8 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix8 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix8 = aModel.localVentMatrix8;

    ventScale += ventScaleOffSet;
    ventTranslation += ventOffSet;
    aModel.localVentMatrix9 = DirectX::SimpleMath::Matrix::Identity;
    aModel.localVentMatrix9 *= DirectX::SimpleMath::Matrix::CreateScale(ventScale);
    aModel.localVentMatrix9 *= DirectX::SimpleMath::Matrix::CreateTranslation(ventTranslation);
    aModel.worldVentMatrix9 = aModel.localVentMatrix9;

    aModel.avoidanceShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
}

void NPCVehicle::InitializeNPCStruct(VehicleStruct& aVehicleStruct,
    const DirectX::SimpleMath::Vector3 aHeading,
    const DirectX::SimpleMath::Vector3 aPosition,
    const NPCType aNPCType, Environment const* aEnvironment)
{
    aVehicleStruct.environment = aEnvironment;
    aVehicleStruct.vehicleData.q.position = aPosition;
    aVehicleStruct.vehicleData.q.velocity = DirectX::SimpleMath::Vector3::Zero;

    aVehicleStruct.vehicleData.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.q.bodyTorqueForce.magnitude = 0.0f;

    aVehicleStruct.vehicleData.dragCoefficient = 0.2f;
    aVehicleStruct.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 10.0f);
    //aVehicleStruct.vehicleData.dimensions = DirectX::SimpleMath::Vector3(44.0f, 27.0f, 30.0f);
    // to do update when transisioned from sphere to box
    aVehicleStruct.vehicleData.collisionBox.Extents = aVehicleStruct.vehicleData.dimensions * 0.5f;

    aVehicleStruct.vehicleData.collisionBox.Center = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 cornerVertex = aVehicleStruct.vehicleData.dimensions;
    cornerVertex.x *= 0.5f;
    cornerVertex.y *= 0.5f;
    cornerVertex.z *= 0.5f;
    // set max collision detection range slightly larger than box radius  
    aVehicleStruct.vehicleData.maxCollisionDetectionRange = (cornerVertex - aVehicleStruct.vehicleData.collisionBox.Center).Length() * 1.2f;
    aVehicleStruct.vehicleData.maxCollisionDetectionRange = 50.0f;

    aVehicleStruct.vehicleData.isCollisionTrue = false;

    aVehicleStruct.vehicleData.frontalArea = aVehicleStruct.vehicleData.dimensions.z * aVehicleStruct.vehicleData.dimensions.y;
    aVehicleStruct.vehicleData.hitPoints = 100.0f;

    aVehicleStruct.vehicleData.mass = 700.0f;
    aVehicleStruct.vehicleData.npcType = NPCType::NPCTYPE_NPC00;
    aVehicleStruct.vehicleData.terrainHightAtPos = 0.0f;
    aVehicleStruct.vehicleData.altitude = 0.0f;
    aVehicleStruct.vehicleData.terrainNormal = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.time = 0.0f;
    aVehicleStruct.vehicleData.forward = aHeading;
    aVehicleStruct.vehicleData.up = DirectX::SimpleMath::Vector3::UnitY;
    aVehicleStruct.vehicleData.right = aVehicleStruct.vehicleData.forward.Cross(aVehicleStruct.vehicleData.up);
    aVehicleStruct.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -aVehicleStruct.vehicleData.right, aVehicleStruct.vehicleData.up);
    
    // zero out impact force
    aVehicleStruct.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.impactForce.impactMass = 0.0f;
    aVehicleStruct.vehicleData.impactForceVec.clear();
    // zero out impact torque
    aVehicleStruct.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.impactTorque.magnitude = 0.0f;
    aVehicleStruct.vehicleData.impactTorqueVec.clear();
    aVehicleStruct.vehicleData.impulseForceVec.clear();

    aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.centerOfMassPos = aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos;
    aVehicleStruct.vehicleData.hardPoints.localWeaponDirection = DirectX::SimpleMath::Vector3::UnitX;
    aVehicleStruct.vehicleData.hardPoints.weaponDirection = aVehicleStruct.vehicleData.hardPoints.localWeaponDirection;    
    aVehicleStruct.vehicleData.hardPoints.localWeaponPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.weaponPos = aVehicleStruct.vehicleData.hardPoints.localWeaponPos;

    aVehicleStruct.vehicleData.hardPoints.localVerticalStabilizerPos = DirectX::SimpleMath::Vector3(0.0f, 30.5f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.verticalStabilizerPos = aVehicleStruct.vehicleData.hardPoints.localVerticalStabilizerPos;
    aVehicleStruct.vehicleData.hardPoints.localSteeringTorqueArmPos = DirectX::SimpleMath::Vector3(aVehicleStruct.vehicleData.dimensions.x * 0.5f, 0.0f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.steeringTorqueArmPos = aVehicleStruct.vehicleData.hardPoints.localSteeringTorqueArmPos;

    aVehicleStruct.vehicleData.hardPoints.localBasePos = DirectX::SimpleMath::Vector3(0.0f, -aVehicleStruct.vehicleData.dimensions.y * 0.5f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.basePos = aVehicleStruct.vehicleData.hardPoints.localBasePos;

    aVehicleStruct.vehicleData.hoverData.forwardThrust = 0.0f;
    aVehicleStruct.vehicleData.hoverData.omniThrust = 0.0f;
    aVehicleStruct.vehicleData.hoverData.omniThrustVec = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hoverData.hoverLiftNeutralWithGrav = -aEnvironment->GetGravityVec();
    aVehicleStruct.vehicleData.hoverData.hoverLiftMax = -aEnvironment->GetGravityVec() * 1.2f;
    aVehicleStruct.vehicleData.hoverData.hoverLiftCurrent = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hoverData.turnRateCurrent = 0.0f;
    aVehicleStruct.vehicleData.hoverData.turnRateMax = 3.0f;

    aVehicleStruct.vehicleData.controlInput.angleToDestination = 0.0f;
    aVehicleStruct.vehicleData.controlInput.omniDirection = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.controlInput.omniThrust = 0.0f;
    aVehicleStruct.vehicleData.controlInput.stearingIsPressed = false;
    aVehicleStruct.vehicleData.controlInput.steeringInput = 0.0f;
    aVehicleStruct.vehicleData.controlInput.steeringVec = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.controlInput.throttleInput = 0.0f;

    aVehicleStruct.vehicleData.playerPos = DirectX::SimpleMath::Vector3::Zero;
}

void NPCVehicle::InitializeNPCVehicle(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment, 
    std::shared_ptr<NPCController> aNpcController, Vehicle const* aPlayer, const unsigned int aID)
{
    m_context = aContext;
    m_environment = aEnvironment;
    m_npcController = aNpcController;
    m_vehicleStruct00.vehicleData.id = aID;
    InitializeNPCStruct(m_vehicleStruct00, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    CalculateTopSpeed();
    InitializeNPCModelStruct(aContext, m_vehicleStruct00.npcModel, m_vehicleStruct00.vehicleData.dimensions);
    m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData, aNpcController);
}

void NPCVehicle::RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ)
{
    //  Compute the intermediate values of the 
//  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;
   
    //  Compute the total drag force
    const float v = newQ.velocity.Length();
    //const float airDensity = 1.225f; // ToDo rework to pull data from environment
    const float airDensity = m_environment->GetAirDensity(); // ToDo rework to pull data from environment
    const float dragCoefficient = aVehicle->dragCoefficient;
    const float frontSurfaceArea = aVehicle->frontalArea;
    const float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;
    float mass = 10.0f;
    mass = aVehicle->mass;
    //DirectX::SimpleMath::Vector3 velocityNorm = aVehicle->q.velocity;
    DirectX::SimpleMath::Vector3 velocityNorm = newQ.velocity;
    velocityNorm.Normalize();

    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance));
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, airResistance, 15.f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, .0f));
    
    float airRLength = airResistance.Length();
    
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    velocityUpdate += GetForwardThrust(m_vehicleStruct00.vehicleData);
    //velocityUpdate += GetOmniDirectionalThrust(m_vehicleStruct00.vehicleData);

    DirectX::SimpleMath::Vector3 damperForce = GetDamperForce(m_vehicleStruct00.vehicleData);
    velocityUpdate += damperForce * mass;
    //DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec() * aVehicle->mass;
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    //velocityUpdate += gravForce;
    DirectX::SimpleMath::Vector3 antiGravForce = GetAntiGravGravityForce(m_vehicleStruct00.vehicleData);

    velocityUpdate += antiGravForce * mass;
    DirectX::SimpleMath::Vector3 hoverForce = GetHoverLift(m_vehicleStruct00.vehicleData);
    //velocityUpdate += hoverForce;

    //velocityUpdate += GetImpactForceSum(m_vehicleStruct00.vehicleData);
    velocityUpdate += m_vehicleStruct00.vehicleData.impactForceSum;

    velocityUpdate += airResistance;

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueRunge(pendTorque, static_cast<float>(aTimeDelta));

    aDQ->bodyTorqueForce = bodyTorqueUpdate;
    aDQ->velocity = static_cast<float>(aTimeDelta) * (velocityUpdate / mass);
    aDQ->position = static_cast<float>(aTimeDelta) * newQ.velocity;
}

void NPCVehicle::RungeKutta4(struct VehicleData* aVehicle, double aTimeDelta)
{
    //  Define a convenience variables

    const float numEqns = static_cast<float>(6);
    //  Retrieve the current values of the dependent and independent variables.
    MotionNPC q = aVehicle->q;
    MotionNPC dq1;
    MotionNPC dq2;
    MotionNPC dq3;
    MotionNPC dq4;

    DirectX::SimpleMath::Vector3 testImpact = GetImpactForceSum(m_vehicleStruct00.vehicleData);
    if (testImpact.Length() > 0.001f)
    {
        int testBreak = 0;
        testBreak++;
    }
    //aVehicle->impactForceSum = GetImpactForceSum(m_vehicleStruct00.vehicleData);
    aVehicle->impactForceSum = m_vehicleStruct00.vehicleData.testProjectileImpulse;
    aVehicle->impactForceSum += GetImpactForceSum(m_vehicleStruct00.vehicleData);

    // Compute the four Runge-Kutta steps, The return 
    // value of RightHandSide method is an array
    // of delta-q values for each of the four steps.
    RightHandSide(aVehicle, &q, &q, aTimeDelta, 0.0, &dq1);
    RightHandSide(aVehicle, &q, &dq1, aTimeDelta, 0.5, &dq2);
    RightHandSide(aVehicle, &q, &dq2, aTimeDelta, 0.5, &dq3);
    RightHandSide(aVehicle, &q, &dq3, aTimeDelta, 1.0, &dq4);
    aVehicle->time = aVehicle->time + aTimeDelta;

    DirectX::SimpleMath::Vector3 posUpdate = (dq1.position + 2.0 * dq2.position + 2.0 * dq3.position + dq4.position) / numEqns;
    DirectX::SimpleMath::Vector3 velocityUpdate = (dq1.velocity + 2.0 * dq2.velocity + 2.0 * dq3.velocity + dq4.velocity) / numEqns;

    Utility::Torque bodyTorqueUpdate;
    bodyTorqueUpdate.axis = (dq1.bodyTorqueForce.axis + 2.0 * dq2.bodyTorqueForce.axis + 2.0 * dq3.bodyTorqueForce.axis + dq4.bodyTorqueForce.axis) / numEqns;
    bodyTorqueUpdate.magnitude = (dq1.bodyTorqueForce.magnitude + 2.0f * dq2.bodyTorqueForce.magnitude + 2.0f * dq3.bodyTorqueForce.magnitude + dq4.bodyTorqueForce.magnitude) / numEqns;
    q.bodyTorqueForce.axis += bodyTorqueUpdate.axis;
    q.bodyTorqueForce.magnitude += bodyTorqueUpdate.magnitude;

    q.velocity += velocityUpdate;
    q.position += posUpdate;

    bool isPenetrating = CheckVehiclePenetration(q.position);
    if (isPenetrating == true)
    {
        int testBreak = 0;
        testBreak++;
    }

    aVehicle->q.velocity = q.velocity;
    aVehicle->q.position = q.position;
    aVehicle->q.bodyTorqueForce = q.bodyTorqueForce;
}

void NPCVehicle::SetCollisionVal(const bool aIsCollisionTrue)
{
    m_vehicleStruct00.vehicleData.isCollisionTrue = aIsCollisionTrue;
}

void NPCVehicle::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void NPCVehicle::TerrainImpactHandling()
{
    DirectX::SimpleMath::Vector3 v = m_vehicleStruct00.vehicleData.q.velocity;
    DirectX::SimpleMath::Vector3 n = m_vehicleStruct00.vehicleData.terrainNormal;
    const float b = 0.9f;

    DirectX::SimpleMath::Vector3 postImpactVelocity = b * (-2.0f * (v.Dot(n)) * n + v);

    m_vehicleStruct00.vehicleData.q.velocity = postImpactVelocity;
}

void NPCVehicle::UpdateAlignment()
{
    DirectX::SimpleMath::Matrix preAlignment = m_vehicleStruct00.vehicleData.alignment;
    DirectX::SimpleMath::Quaternion preAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleStruct00.vehicleData.alignment);
    DirectX::SimpleMath::Matrix torqueMat;
    const float tol = 0.0001f;
    if (abs(m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude) < tol || m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis.Length() < tol)
    {
        torqueMat = DirectX::SimpleMath::Matrix::Identity;
    }
    else
    {
        m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis.Normalize();
        torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis, m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude);
    }

    const float t = 0.5f;
    DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(torqueMat);
    DirectX::SimpleMath::Matrix postAlignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, rotQuat);
    
    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::Lerp(preAlignment, postAlignment, t);

    m_vehicleStruct00.vehicleData.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_vehicleStruct00.vehicleData.alignment);
    m_vehicleStruct00.vehicleData.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_vehicleStruct00.vehicleData.alignment);

    m_vehicleStruct00.vehicleData.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
}

Utility::Torque NPCVehicle::UpdateBodyTorqueRunge(Utility::Torque aPendulumTorque, const float aTimeStep)
{
    Utility::Torque impactTorque = GetImpactTorqueSum(m_vehicleStruct00.vehicleData);
    impactTorque.axis.Normalize();
    //impactTorque.magnitude *= 0.0002f;    
    impactTorque.magnitude *= 0.00002f;

    const DirectX::SimpleMath::Vector3 centerMassPos = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    const DirectX::SimpleMath::Vector3 rotorPos = m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos;
    DirectX::SimpleMath::Vector3 gravityForce = -m_vehicleStruct00.environment->GetGravityVec();
    DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);


    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos - centerMassPos;
    DirectX::SimpleMath::Vector3 tailForce = -m_vehicleStruct00.vehicleData.right * (m_vehicleStruct00.vehicleData.controlInput.steeringInput) * 1.0f;
    Utility::Torque tailTorque = Utility::GetTorqueForce(tailRotorTorqueArm, tailForce);
    
    Utility::Torque turnTestTorque;
    if (m_vehicleStruct00.vehicleData.controlInput.steeringInput < 0.0f)
    {
        turnTestTorque.axis = -m_vehicleStruct00.vehicleData.up;
    }
    else
    {
        turnTestTorque.axis = m_vehicleStruct00.vehicleData.up;
    }
    turnTestTorque.magnitude = m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    turnTestTorque = tailTorque;
    //DirectX::SimpleMath::Vector3 torqueAxis = (impactTorque.axis * impactTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (turnTestTorque.axis * turnTestTorque.magnitude);
    //float torqueMag = impactTorque.magnitude + gravTorque.magnitude + turnTestTorque.magnitude;
    // 
    impactTorque.axis = m_vehicleStruct00.vehicleData.testProjectileTorque.axis;
    impactTorque.magnitude = m_vehicleStruct00.vehicleData.testProjectileTorque.magnitude;
    impactTorque.magnitude *= 0.00002f;
    DirectX::SimpleMath::Vector3 torqueAxis = (impactTorque.axis * impactTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (turnTestTorque.axis * turnTestTorque.magnitude);
    float torqueMag = impactTorque.magnitude + gravTorque.magnitude + turnTestTorque.magnitude;

    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.axis.Normalize();
    updatedTorque.magnitude = torqueMag / m_vehicleStruct00.vehicleData.mass;
    return updatedTorque;
}

void NPCVehicle::UpdateControlInput()
{
    m_vehicleStruct00.vehicleData.controlInput.steeringVec = m_npcAI->GetVecToDestination();
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.controlInput.steeringVec, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, .0f));

    m_vehicleStruct00.vehicleData.controlInput.angleToDestination = m_npcAI->GetAngleToDestination(m_vehicleStruct00.vehicleData.forward, m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, m_vehicleStruct00.vehicleData.playerPos);

    const float yawInput = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;
    const float updatedYaw = (yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    if (updatedYaw > m_vehicleStruct00.vehicleData.controlInput.steeringInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.steeringInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMin;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.inputDeadZone && updatedYaw > -m_vehicleStruct00.vehicleData.controlInput.inputDeadZone)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }

    // velocity
    float absAngleToDest = abs(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    float rawThrottleInput = m_npcAI->GetThrottleInput();
    float modThrottleInput = rawThrottleInput - absAngleToDest;
    if (modThrottleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = modThrottleInput;
    }
    
    //m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_npcAI->GetThrottleInput();
    m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
}

void NPCVehicle::UpdateControlInputFromAi()
{
    AIOutput aiInput = m_npcAI->GetAiControlOutput();

    //m_vehicleStruct00.vehicleData.controlInput.steeringVec = m_npcAI->GetVecToDestination();
    m_vehicleStruct00.vehicleData.controlInput.steeringVec = aiInput.steeringDirection;
    //m_vehicleStruct00.vehicleData.controlInput.angleToDestination = m_npcAI->GetAngleToDestination(m_vehicleStruct00.vehicleData.forward, m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, m_vehicleStruct00.vehicleData.playerPos);
    m_vehicleStruct00.vehicleData.controlInput.angleToDestination = aiInput.angleToDestination;

    //m_debugData->DebugPushUILineDecimalNumber("angleToDestination ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.angleToDestination), "");
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.controlInput.steeringVec, 25.f, 10.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.forward, 45.f, 7.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    const float yawInput = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;
    float updatedYaw;
    if (abs(yawInput) < abs(m_vehicleStruct00.vehicleData.controlInput.steeringInput))
    {
        updatedYaw = yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate;
    }
    else
    {
        updatedYaw = (yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    }
    //const float updatedYaw = (yawInput) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    //const float updatedYaw = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;


    if (updatedYaw > m_vehicleStruct00.vehicleData.controlInput.steeringInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.steeringInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMin;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.inputDeadZone && updatedYaw > -m_vehicleStruct00.vehicleData.controlInput.inputDeadZone)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
        //m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }

    //m_vehicleStruct00.vehicleData.controlInput.steeringInput = aiInput.steeringAngle;

    //m_vehicleStruct00.vehicleData.controlInput.steeringInput = aiInput.angleToDestination;
    //m_debugData->DebugPushUILineDecimalNumber("steeringInput ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.steeringInput), "");
    //m_debugData->DebugPushUILineDecimalNumber("inputDeadZone ", m_vehicleStruct00.vehicleData.controlInput.inputDeadZone, "");
    //m_debugData->DebugPushUILineDecimalNumber("inputDeadZone degrees ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.inputDeadZone), "");
    // velocity
    float absAngleToDest = abs(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    //float rawThrottleInput = m_npcAI->GetThrottleInput();
    float rawThrottleInput = aiInput.forwardThrust;
    float modThrottleInput = rawThrottleInput - absAngleToDest;
    /*
    if (modThrottleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = modThrottleInput;
    }
    */

    float aiForwardThrottle = aiInput.forwardThrust;
    //m_vehicleStruct00.vehicleData.controlInput.throttleInput = aiInput.forwardThrust;
    float updateThrottleDelta = aiForwardThrottle - m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    float updateThrottle = 0.0f;
    if (abs(updateThrottleDelta) > m_vehicleStruct00.vehicleData.controlInput.throttleInputRate)
    {
        if (updateThrottleDelta > 0.0f)
        {
            updateThrottle = m_vehicleStruct00.vehicleData.controlInput.throttleInputRate;
        }
        else
        {
            updateThrottle = - m_vehicleStruct00.vehicleData.controlInput.throttleInputRate;
        }
    }
    else
    {
        updateThrottle = updateThrottleDelta;
    }
    float throttleSum = m_vehicleStruct00.vehicleData.controlInput.throttleInput + updateThrottle;
    if (throttleSum > m_vehicleStruct00.vehicleData.controlInput.throttleInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_vehicleStruct00.vehicleData.controlInput.throttleInputMax;
    }
    else if (throttleSum < m_vehicleStruct00.vehicleData.controlInput.throttleInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_vehicleStruct00.vehicleData.controlInput.throttleInputMin;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput += updateThrottle;
    }

    if (m_vehicleStruct00.vehicleData.controlInput.throttleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.reverseThrustMax;
    }
    else
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
    }

    // omni trust
    m_vehicleStruct00.vehicleData.controlInput.omniDirection = aiInput.omniDirection;
    m_vehicleStruct00.vehicleData.controlInput.omniThrust = aiInput.omniThrust * m_vehicleStruct00.vehicleData.hoverData.omniThrustMax;
    m_vehicleStruct00.vehicleData.hoverData.omniThrustVec = m_vehicleStruct00.vehicleData.controlInput.omniDirection * m_vehicleStruct00.vehicleData.controlInput.omniThrust;
}

void NPCVehicle::UpdateControlInputFromAi2()
{
    AIOutput aiInput = m_npcAI->GetAiControlOutput();

    //m_vehicleStruct00.vehicleData.controlInput.steeringVec = m_npcAI->GetVecToDestination();
    m_vehicleStruct00.vehicleData.controlInput.steeringVec = aiInput.steeringDirection;
    //m_vehicleStruct00.vehicleData.controlInput.angleToDestination = m_npcAI->GetAngleToDestination(m_vehicleStruct00.vehicleData.forward, m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, m_vehicleStruct00.vehicleData.playerPos);
    m_vehicleStruct00.vehicleData.controlInput.angleToDestination = aiInput.angleToDestination;

    //m_debugData->DebugPushUILineDecimalNumber("angleToDestination ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.angleToDestination), "");
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.controlInput.steeringVec, 25.f, 10.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.forward, 45.f, 7.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    const float yawInput = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;
    float updatedYaw;
    if (abs(yawInput) < abs(m_vehicleStruct00.vehicleData.controlInput.steeringInput))
    {
        updatedYaw = yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate;
    }
    else
    {
        updatedYaw = (yawInput * m_vehicleStruct00.vehicleData.controlInput.steeringInputRate) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    }
    //const float updatedYaw = (yawInput) + m_vehicleStruct00.vehicleData.controlInput.steeringInput;
    //const float updatedYaw = m_vehicleStruct00.vehicleData.controlInput.angleToDestination;


    if (updatedYaw > m_vehicleStruct00.vehicleData.controlInput.steeringInputMax)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.steeringInputMin)
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = m_vehicleStruct00.vehicleData.controlInput.steeringInputMin;
    }
    else if (updatedYaw < m_vehicleStruct00.vehicleData.controlInput.inputDeadZone && updatedYaw > -m_vehicleStruct00.vehicleData.controlInput.inputDeadZone)
    {
        //m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }

    //m_vehicleStruct00.vehicleData.controlInput.steeringInput = aiInput.steeringAngle;

    //m_vehicleStruct00.vehicleData.controlInput.steeringInput = aiInput.angleToDestination;
    //m_debugData->DebugPushUILineDecimalNumber("steeringInput ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.steeringInput), "");
    //m_debugData->DebugPushUILineDecimalNumber("inputDeadZone ", m_vehicleStruct00.vehicleData.controlInput.inputDeadZone, "");
    //m_debugData->DebugPushUILineDecimalNumber("inputDeadZone degrees ", Utility::ToDegrees(m_vehicleStruct00.vehicleData.controlInput.inputDeadZone), "");
    // velocity
    float absAngleToDest = abs(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    //float rawThrottleInput = m_npcAI->GetThrottleInput();
    float rawThrottleInput = aiInput.forwardThrust;
    float modThrottleInput = rawThrottleInput - absAngleToDest;
    if (modThrottleInput < 0.0f)
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = 0.0f;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.throttleInput = modThrottleInput;
    }

    m_vehicleStruct00.vehicleData.controlInput.throttleInput = aiInput.forwardThrust;
    if (aiInput.forwardThrust < 0.0f)
    {
        int testBreak = 0;
        testBreak++;
    }
    //m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_npcAI->GetThrottleInput();
    //m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
    if (aiInput.forwardThrust < 0.0f)
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = aiInput.forwardThrust * m_vehicleStruct00.vehicleData.hoverData.reverseThrustMax;
    }
    else
    {
        m_vehicleStruct00.vehicleData.hoverData.forwardThrust = aiInput.forwardThrust * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;
    }
    
    // omni trust
    m_vehicleStruct00.vehicleData.controlInput.omniDirection = aiInput.omniDirection;
    m_vehicleStruct00.vehicleData.controlInput.omniThrust = aiInput.omniThrust * m_vehicleStruct00.vehicleData.hoverData.omniThrustMax;
    m_vehicleStruct00.vehicleData.hoverData.omniThrustVec = m_vehicleStruct00.vehicleData.controlInput.omniDirection * m_vehicleStruct00.vehicleData.controlInput.omniThrust;
}

void NPCVehicle::UpdateForceTorqueVecs() // update when force and torque over time is implemented
{
    m_vehicleStruct00.vehicleData.impactForceVec.clear();
    m_vehicleStruct00.vehicleData.impactTorqueVec.clear();
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;
    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::UnitY;
}

void NPCVehicle::UpdateHardPoints()
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    DirectX::SimpleMath::Matrix updateDirectionMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);

    m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localCenterOfMassPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponDirection = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.vehicleData.hardPoints.localWeaponDirection, updateDirectionMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localWeaponPos, updateMat);

    m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localVerticalStabilizerPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localSteeringTorqueArmPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.basePos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localBasePos, updateMat);
}

void NPCVehicle::UpdateImpulseForces(const float aTimeDelta)
{
    DirectX::SimpleMath::Vector3 forceSum = DirectX::SimpleMath::Vector3::Zero;
    Utility::Torque torqueSum;
    torqueSum.axis = DirectX::SimpleMath::Vector3::Zero;
    torqueSum.magnitude = 0.0f;
    for (int i = 0; i < m_vehicleStruct00.vehicleData.impulseForceVec.size(); ++i)
    {
        Utility::UpdateImpulseForceBellCurve(m_vehicleStruct00.vehicleData.impulseForceVec[i], static_cast<float>(aTimeDelta));
        if (m_vehicleStruct00.vehicleData.impulseForceVec[i].isActive == true)
        {
            m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude *= aTimeDelta;
            forceSum += m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm;

            Utility::Torque torqueImpulse = Utility::GetTorqueForce(m_vehicleStruct00.vehicleData.impulseForceVec[i].torqueArm,
                (m_vehicleStruct00.vehicleData.impulseForceVec[i].currentMagnitude * m_vehicleStruct00.vehicleData.impulseForceVec[i].directionNorm));
            torqueSum.axis += torqueImpulse.axis * torqueImpulse.magnitude;
            torqueSum.magnitude += torqueImpulse.magnitude;
        }
    }
    m_vehicleStruct00.vehicleData.testProjectileImpulse = forceSum;
    m_vehicleStruct00.vehicleData.testProjectileTorque = torqueSum;

    m_vehicleStruct00.vehicleData.impulseForceVec.erase(
        std::remove_if(
            m_vehicleStruct00.vehicleData.impulseForceVec.begin(),
            m_vehicleStruct00.vehicleData.impulseForceVec.end(),
            [](Utility::ImpulseForce const& p) {return p.isActive == false; }
        ),
        m_vehicleStruct00.vehicleData.impulseForceVec.end()
    );
}

void NPCVehicle::UpdateNPC(const double aTimeDelta)
{
    m_testTimer += aTimeDelta;
    DirectX::SimpleMath::Vector3 preVelocity = m_vehicleStruct00.vehicleData.q.velocity;

    DirectX::SimpleMath::Vector3 preThrust = m_vehicleStruct00.vehicleData.controlInput.steeringVec * (m_vehicleStruct00.vehicleData.hoverData.forwardThrust);
    float preThrustLength = preThrust.Length();

    bool testBool = m_vehicleStruct00.environment->GetVehicleUpdateData(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.terrainNormal, m_vehicleStruct00.vehicleData.terrainHightAtPos);

    m_vehicleStruct00.vehicleData.altitude = m_vehicleStruct00.vehicleData.hardPoints.basePos.y - m_vehicleStruct00.vehicleData.terrainHightAtPos;

    if (m_vehicleStruct00.vehicleData.altitude <= 0.0f && m_vehicleStruct00.vehicleData.q.velocity.y < 0.0f)
    {
        TerrainImpactHandling();
    }

    m_npcAI->UpdateAI(static_cast<float>(aTimeDelta));
    UpdateControlInputFromAi();
    UpdateImpulseForces(static_cast<float>(aTimeDelta));
    if (m_isGoToggleTrue == true)
    {
        RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);
    }

    m_vehicleStruct00.vehicleData.collisionBox.Center = m_vehicleStruct00.vehicleData.q.position;

    UpdateAlignment();
    UpdateNPCModel(aTimeDelta);
    UpdateHardPoints();

    m_vehicleStruct00.vehicleData.isCollisionTrue = false;

    m_vehicleStruct00.vehicleData.impactForce.impactMass = 0.0f;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude = 0.0f;

    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;

    //CalculateSelfRightingTorque();

    //m_npcAI->UpdateAI(static_cast<float>(aTimeDelta));
    //UpdateControlInput();
    //m_debugData->DebugPushUILineDecimalNumber("Velocity = ", m_vehicleStruct00.vehicleData.q.velocity.Length(), "");

    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, .0f));
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_prevImpact, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, .0f));

    UpdateForceTorqueVecs();
}

void NPCVehicle::UpdateNPCModel(const double aTimeDelta)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.npcModel.worldModelMatrix = m_vehicleStruct00.npcModel.localModelMatrix;
    m_vehicleStruct00.npcModel.worldModelMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldRearBodyMatrix = m_vehicleStruct00.npcModel.localRearBodyMatrix;
    m_vehicleStruct00.npcModel.worldRearBodyMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldFrontAirDamMatrix = m_vehicleStruct00.npcModel.localFrontAirDamMatrix;
    m_vehicleStruct00.npcModel.worldFrontAirDamMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldEyeLeftMatrix = m_vehicleStruct00.npcModel.localEyeLeftMatrix;
    m_vehicleStruct00.npcModel.worldEyeLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldEyeRightMatrix = m_vehicleStruct00.npcModel.localEyeRightMatrix;
    m_vehicleStruct00.npcModel.worldEyeRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldRearDeckMatrix = m_vehicleStruct00.npcModel.localRearDeckMatrix;
    m_vehicleStruct00.npcModel.worldRearDeckMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix = m_vehicleStruct00.npcModel.localJetHousingLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix = m_vehicleStruct00.npcModel.localJetHousingShellLeftMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingShellLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix = m_vehicleStruct00.npcModel.localJetHousingRightMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingRightMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix = m_vehicleStruct00.npcModel.localJetHousingShellRightMatrix;
    m_vehicleStruct00.npcModel.worldJetHousingShellRightMatrix *= updateMat;


    float turnRatio = abs(m_vehicleStruct00.vehicleData.controlInput.steeringInput) / m_vehicleStruct00.vehicleData.controlInput.steeringInputMax;
    const float afterBurnLengthSum = 400.0f;
    float afterBurnLengthModLeft;
    float afterBurnLengthModRight;
    if (m_vehicleStruct00.vehicleData.controlInput.steeringInput > 0.0f)
    {
        afterBurnLengthModLeft = turnRatio;
        afterBurnLengthModRight = 1.0f - turnRatio;
    }
    else if (m_vehicleStruct00.vehicleData.controlInput.steeringInput < 0.0f)
    {
        afterBurnLengthModRight = turnRatio;
        afterBurnLengthModLeft = 1.0f - turnRatio;
    }
    else
    {
        afterBurnLengthModLeft = 0.5f;
        afterBurnLengthModRight = 0.5f;
    }

    //m_testTimer += aTimeDelta;
    float testThrottle = cos(m_testTimer);
    //testThrottle = -1.0f;
    testThrottle = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    // jet afterburn exhaust  
    //float afterBurnLength = m_vehicleStruct00.vehicleData.controlInput.throttleInput * 80.0f;
    //float afterBurnLength = testThrottle * 180.0f;
    float afterBurnLength = testThrottle * (afterBurnLengthModLeft * afterBurnLengthSum);
    //const float afterBurnHeight = 0.1f;
    float afterBurnY = (afterBurnLength * 0.5f) * testThrottle;
    afterBurnY = (afterBurnLength * 0.01f);
    afterBurnY = (afterBurnLength * 0.5f) * 0.1f;
    //afterBurnY = (2.9f / (afterBurnLength) );
    //afterBurnY = 10.0f;
    //afterBurnY = (afterBurnLength * (1.0f / 0.1f));
    //if (m_vehicleStruct00.vehicleData.controlInput.throttleInput <= 0.0f)
    if (testThrottle <= 0.0f)
    {
        //afterBurnY = -(afterBurnLength * 0.5f) * 0.1f;
        //afterBurnLength = 0.0f;
    }
    //const DirectX::SimpleMath::Matrix afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, (afterBurnLength * 0.5f) * 0.004f, 0.0f);
    DirectX::SimpleMath::Matrix afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);
    if (testThrottle <= 0.0f)
    {
        //afterBurnY = -(afterBurnLength * 0.5f) * 0.1f;
        //afterBurnLength = 0.0f;
        afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);
    }


    m_vehicleStruct00.npcModel.afterBurnLeftFlicker += (afterBurnLengthModLeft * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnLeftFlicker = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    //m_vehicleStruct00.npcModel.afterBurnLeftFlicker = 0.0f;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);

    DirectX::SimpleMath::Matrix afterBurnScale = DirectX::SimpleMath::Matrix::CreateScale(1.0f, abs(afterBurnLength), 1.0f);
    if (testThrottle <= 0.0f)
    {
        afterBurnY = -(afterBurnLength * 0.5f) * 0.1f;
        //afterBurnLength = 0.0f;
        m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
        m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnScale;
    }
    else
    {
        m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnScale;
    }
    //m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix = afterBurnTranslation;
    //m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnScale;
    //m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix = afterBurnScale;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= m_vehicleStruct00.npcModel.localAfterBurnLeftMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= updateMat;
    //m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix = m_vehicleStruct00.npcModel.localAfterBurnLeftMatrix;
    //m_vehicleStruct00.npcModel.worldAfterBurnLeftMatrix *= updateMat;

    //////////////////
    //////////////////
    m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnLeftFlicker);
    if (testThrottle <= 0.0f)
    {
        afterBurnY = -(afterBurnLength * 0.5f) * 0.1f;
        //afterBurnLength = 0.0f;
        m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
        m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= afterBurnScale;
        m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= afterBurnTranslation;
    }
    else
    {
        //m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= afterBurnScale;
    }
    //m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= m_vehicleStruct00.npcModel.localForwardBurnLeftMatrix;
    m_vehicleStruct00.npcModel.worldForwardBurnLeftMatrix *= updateMat;
    //////////////////

    afterBurnLength = testThrottle * (afterBurnLengthModRight * afterBurnLengthSum);
    //const float afterBurnHeight = 0.1f;
    //float afterBurnY = (afterBurnLength * 0.5f) * testThrottle;
    //afterBurnY = (afterBurnLength * 0.01f);
    afterBurnY = (afterBurnLength * 0.5f) * 0.1f;
    afterBurnScale = DirectX::SimpleMath::Matrix::CreateScale(1.0f, abs(afterBurnLength), 1.0f);
    afterBurnTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, afterBurnY, 0.0f);

    m_vehicleStruct00.npcModel.afterBurnRightFlicker -= (afterBurnLengthModRight * m_vehicleStruct00.npcModel.afterBurnFlickerRate);
    m_vehicleStruct00.npcModel.afterBurnRightFlicker = Utility::WrapAngle(m_vehicleStruct00.npcModel.afterBurnRightFlicker);
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.afterBurnRightFlicker);

    if (testThrottle <= 0.0f)
    {
        afterBurnY = -(afterBurnLength * 0.5f) * 0.1f;
        //afterBurnLength = 0.0f;
        m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(Utility::ToRadians(180.0f));
        m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnScale;
    }
    else
    {
        m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnScale;
    }

    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= afterBurnTranslation;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= m_vehicleStruct00.npcModel.localAfterBurnRightMatrix;
    m_vehicleStruct00.npcModel.worldAfterBurnRightMatrix *= updateMat;


    ////////////////////

    const float speed = m_vehicleStruct00.vehicleData.q.velocity.Length();
    const float rotMod = 0.001f;
    bool isVelocityBackwards;
    if (m_vehicleStruct00.vehicleData.forward.Dot(m_vehicleStruct00.vehicleData.q.velocity) < 0.0)
    {
        isVelocityBackwards = true;
        m_vehicleStruct00.npcModel.skirtRotation += speed * rotMod;
    }
    else
    {
        isVelocityBackwards = false;
        m_vehicleStruct00.npcModel.skirtRotation -= speed * rotMod;
    }
    m_vehicleStruct00.npcModel.skirtRotation = Utility::ToRadians(-55.0f);
    m_vehicleStruct00.npcModel.skirtRotation = Utility::WrapAngle(m_vehicleStruct00.npcModel.skirtRotation);

    //m_vehicleStruct00.npcModel.worldSkirtMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.npcModel.skirtRotation);
    //m_vehicleStruct00.npcModel.worldSkirtMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-55.0f));
    m_vehicleStruct00.npcModel.worldSkirtMatrix = m_vehicleStruct00.npcModel.localSkirtMatrix;
    m_vehicleStruct00.npcModel.worldSkirtMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldForwardMatrix = m_vehicleStruct00.npcModel.localForwardMatrix;
    m_vehicleStruct00.npcModel.worldForwardMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldNoseConeMatrix = m_vehicleStruct00.npcModel.localNoseConeMatrix;
    m_vehicleStruct00.npcModel.worldNoseConeMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldOmniBaseMatrix = m_vehicleStruct00.npcModel.localOmniBaseMatrix;
    m_vehicleStruct00.npcModel.worldOmniBaseMatrix *= updateMat;
  
    DirectX::SimpleMath::Vector3 omniDialTranslation = m_vehicleStruct00.vehicleData.controlInput.omniDirection;
    //DirectX::SimpleMath::Vector3 omniDialTranslation = m_vehicleStruct00.vehicleData.controlInput.steeringVec;
    omniDialTranslation.Normalize();
    omniDialTranslation *= m_vehicleStruct00.npcModel.omniDialRadius;
    m_vehicleStruct00.npcModel.worldOmniDialMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(omniDialTranslation);
    m_vehicleStruct00.npcModel.worldOmniDialMatrix *= m_vehicleStruct00.npcModel.localOmniDialMatrix;
    m_vehicleStruct00.npcModel.worldOmniDialMatrix *= updateMat;


    m_vehicleStruct00.npcModel.worldRearMatrix = DirectX::SimpleMath::Matrix::Identity;
    m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(-1.4f, 0.0f, 0.0f);
    if (m_vehicleStruct00.vehicleData.controlInput.throttleInput < 0.0f)
    //if (cos(m_testTimer) < 0.0f)
    {
        
        //m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(m_vehicleStruct00.vehicleData.controlInput.throttleInput);
        //m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(cos(m_testTimer));
    }
    m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ((m_vehicleStruct00.vehicleData.controlInput.throttleInput - 1.0f) * 0.5f);
    //m_vehicleStruct00.npcModel.worldRearMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(-cos(m_testTimer)-1.0f);
    m_vehicleStruct00.npcModel.worldRearMatrix *= m_vehicleStruct00.npcModel.localRearMatrix;
    m_vehicleStruct00.npcModel.worldThrottleMatrix = m_vehicleStruct00.npcModel.worldRearMatrix;
    m_vehicleStruct00.npcModel.worldRearMatrix *= updateMat;

    const float testRot = cos(m_testTimer);
    float wingRotationAngle = 0.0f;
    //float testAngle = cos(m_testTimer);
    float testAngle = m_vehicleStruct00.vehicleData.controlInput.throttleInput;
    //if (m_vehicleStruct00.vehicleData.controlInput.throttleInput < 0.0f)
    if (testAngle < 0.0f)
    {
        //wingRotationAngle = (m_vehicleStruct00.vehicleData.controlInput.throttleInput) * 0.5f;
        wingRotationAngle = (testAngle) * 1.6f;
    }


    DirectX::SimpleMath::Matrix wingRotation = DirectX::SimpleMath::Matrix::CreateRotationZ(wingRotationAngle);
    
    m_vehicleStruct00.npcModel.worldWingArmMatrix = m_vehicleStruct00.npcModel.localWingArmMatrix;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= m_vehicleStruct00.npcModel.localWingArmTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingArmMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingMatrix = m_vehicleStruct00.npcModel.localWingMatrix;
    m_vehicleStruct00.npcModel.worldWingMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingMatrix *= m_vehicleStruct00.npcModel.localWingTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix = m_vehicleStruct00.npcModel.localWingFinLeftMatrix;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= m_vehicleStruct00.npcModel.localWingFinLeftTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingFinLeftMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldWingFinRightMatrix = m_vehicleStruct00.npcModel.localWingFinRightMatrix;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= wingRotation;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= m_vehicleStruct00.npcModel.localWingFinRightTranslationMatrix;
    m_vehicleStruct00.npcModel.worldWingFinRightMatrix *= updateMat;

    //m_vehicleStruct00.npcModel.worldThrottleMatrix = m_vehicleStruct00.npcModel.localThrottleMatrix;
    //m_vehicleStruct00.npcModel.worldThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, abs(m_vehicleStruct00.vehicleData.controlInput.throttleInput)));
    m_vehicleStruct00.npcModel.worldThrottleMatrix *= DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 0.8f)); 
    m_vehicleStruct00.npcModel.worldThrottleMatrix *= updateMat;

    //DirectX::SimpleMath::Matrix steeringRotation = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.vehicleData.controlInput.steeringInput);
    DirectX::SimpleMath::Matrix steeringRotation = DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleStruct00.vehicleData.controlInput.angleToDestination);
    m_vehicleStruct00.npcModel.worldSteeringMatrix = m_vehicleStruct00.npcModel.localSteeringMatrix;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= steeringRotation;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= m_vehicleStruct00.npcModel.translationSteeringMatrix;
    m_vehicleStruct00.npcModel.worldSteeringMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldVentMatrix1 = m_vehicleStruct00.npcModel.localVentMatrix1;
    m_vehicleStruct00.npcModel.worldVentMatrix1 *= updateMat;

    m_vehicleStruct00.npcModel.worldVentMatrix2 = m_vehicleStruct00.npcModel.localVentMatrix2;
    m_vehicleStruct00.npcModel.worldVentMatrix2 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix3 = m_vehicleStruct00.npcModel.localVentMatrix3;
    m_vehicleStruct00.npcModel.worldVentMatrix3 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix4 = m_vehicleStruct00.npcModel.localVentMatrix4;
    m_vehicleStruct00.npcModel.worldVentMatrix4 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix5 = m_vehicleStruct00.npcModel.localVentMatrix5;
    m_vehicleStruct00.npcModel.worldVentMatrix5 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix6 = m_vehicleStruct00.npcModel.localVentMatrix6;
    m_vehicleStruct00.npcModel.worldVentMatrix6 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix7 = m_vehicleStruct00.npcModel.localVentMatrix7;
    m_vehicleStruct00.npcModel.worldVentMatrix7 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix8 = m_vehicleStruct00.npcModel.localVentMatrix8;
    m_vehicleStruct00.npcModel.worldVentMatrix8 *= updateMat;
    m_vehicleStruct00.npcModel.worldVentMatrix9 = m_vehicleStruct00.npcModel.localVentMatrix9;
    m_vehicleStruct00.npcModel.worldVentMatrix9 *= updateMat;
}

void NPCVehicle::UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos)
{
    m_vehicleStruct00.vehicleData.playerPos = aPlayerPos;
}


