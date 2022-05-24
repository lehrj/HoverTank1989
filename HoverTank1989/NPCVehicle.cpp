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
    //mass1 = 1.05f;
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
    PushImpactForce(repulsionForceToVec);

    PushImpactForce(impactForceToVec);

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
    //DirectX::SimpleMath::Vector3 torqueArm = aVehicleHit.q.position - m_vehicleStruct00.vehicleData.collisionBox.Center;
    DirectX::SimpleMath::Vector3 torqueArm = aImpactPos - m_vehicleStruct00.vehicleData.collisionBox.Center;
    Utility::Torque impactTorque = Utility::GetTorqueForce(torqueArm, impactForce);

    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis += impactTorque.axis * impactTorque.magnitude;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude += impactTorque.magnitude;
    // new variable for combined torque, test
    m_vehicleStruct00.vehicleData.impactTorque.axis += impactTorque.axis * impactTorque.magnitude;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude += impactTorque.magnitude;

    //impactForceToVec.impactVelocity = aImpactForce.impactVelocity;
    //impactForceToVec.impactMass = aImpactForce.impactMass;

    m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, impactForce, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    /*
    Utility::ImpactForce repulsionForceToVec;
    repulsionForceToVec.impactVelocity = GetRepulsionForce(aVehicleHit);
    repulsionForceToVec.impactMass = aVehicleHit.mass;
    PushImpactForce(repulsionForceToVec);
    */
    //PushImpactForce(impactForceToVec);

    PushImpactTorque(impactTorque);
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

void NPCVehicle::DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 forwardColor = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    if (m_vehicleStruct00.vehicleData.isCollisionTrue == true)
    {
        color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    m_vehicleStruct00.npcModel.modelShape->Draw(m_vehicleStruct00.npcModel.worldModelMatrix, aView, aProj, color);
    m_vehicleStruct00.npcModel.forwardShape->Draw(m_vehicleStruct00.npcModel.worldForwardMatrix, aView, aProj, forwardColor);

    DirectX::BoundingBox avoidBox = m_npcAI->GetAiAvoidanceBox();
    DirectX::SimpleMath::Vector3 testSize = avoidBox.Extents;
    testSize *= 2.0f;
    DirectX::SimpleMath::Vector4 testColor = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    if (m_npcAI->GetIsAvoidanceTrue() == true)
    {
        testColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    m_vehicleStruct00.npcModel.avoidanceShape = DirectX::GeometricPrimitive::CreateBox(m_context.Get(), testSize);
    //m_vehicleStruct00.npcModel.avoidanceShape->Draw(m_npcAI->GetAiAvoidanceBoxAlignment(), aView, aProj, testColor);

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
        //gravForce = DirectX::SimpleMath::Vector3::Zero;
        gravForce *= -2.0f;
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
    const float damperConstant = 9.1f;
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

        impactForce += aVehicleData.impactForceVec[i].impactVelocity;
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
    float ratio = (1.0f - (distance / (m_vehicleStruct00.vehicleData.maxCollisionDetectionRange + aRepulsorVehicleData.maxCollisionDetectionRange)));
    repulsionForce = repulsionForceNorm * (ratio * repulsionForceMax);

    return repulsionForce;
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

    aVehicleStruct.vehicleData.dragCoefficient = 0.5f;
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

    aVehicleStruct.vehicleData.mass = 1000.0f;
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

    aVehicleStruct.vehicleData.hardPoints.localBasePos = DirectX::SimpleMath::Vector3(0.0f, -aVehicleStruct.vehicleData.dimensions.y * 1.0f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.basePos = aVehicleStruct.vehicleData.hardPoints.localBasePos;

    aVehicleStruct.vehicleData.testForce = DirectX::SimpleMath::Vector3::Zero;

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

void NPCVehicle::InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    NPCModel& aModel, const DirectX::SimpleMath::Vector3 aDimensions)
{   
    DirectX::SimpleMath::Matrix centerMassTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(aDimensions.x * 0.0f, -aDimensions.y * 0.0f, aDimensions.z * 0.0f);
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), aDimensions);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localModelMatrix *= centerMassTranslation;
    aModel.worldModelMatrix = aModel.localModelMatrix;

    DirectX::SimpleMath::Vector3 forwardShapeSize = aDimensions;
    forwardShapeSize.x *= 0.1f;
    forwardShapeSize.y *= 0.2f;
    forwardShapeSize.z *= 1.05f;
    aModel.forwardShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), forwardShapeSize);
    DirectX::SimpleMath::Vector3 forwardShapeTranslation;
    forwardShapeTranslation.x = (aDimensions.x * 0.5f) - (forwardShapeSize.x * 0.25f);
    forwardShapeTranslation.y = aDimensions.y * 0.2f;
    forwardShapeTranslation.z = 0.0f;
    aModel.localForwardMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localForwardMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(forwardShapeTranslation);
    aModel.localForwardMatrix *= centerMassTranslation;
    aModel.worldForwardMatrix = aModel.localModelMatrix;

    aModel.avoidanceShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

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
    m_npcAI->InitializeAI(aEnvironment, aPlayer, m_debugData);
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
    const float airDensity = 1.225f; // ToDo rework to pull data from environment
    const float dragCoefficient = aVehicle->dragCoefficient;
    const float frontSurfaceArea = aVehicle->frontalArea;
    const float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;
    float mass = 10.0f;
    //mass = aVehicle->mass;
    DirectX::SimpleMath::Vector3 velocityNorm = aVehicle->q.velocity;
    velocityNorm.Normalize();

    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance));
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, airResistance, 15.f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, .0f));
    
    float airRLength = airResistance.Length();
    
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    //velocityUpdate += GetForwardThrust(m_vehicleStruct00.vehicleData);
    //velocityUpdate += GetOmniDirectionalThrust(m_vehicleStruct00.vehicleData);

    DirectX::SimpleMath::Vector3 damperForce = GetDamperForce(m_vehicleStruct00.vehicleData);
    velocityUpdate += damperForce;
    //DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec() * aVehicle->mass;
    DirectX::SimpleMath::Vector3 gravForce = m_environment->GetGravityVec();
    velocityUpdate += gravForce;
    DirectX::SimpleMath::Vector3 antiGravForce = GetAntiGravGravityForce(m_vehicleStruct00.vehicleData);
    //gravForce = DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f);
    velocityUpdate += antiGravForce;
    DirectX::SimpleMath::Vector3 hoverForce = GetHoverLift(m_vehicleStruct00.vehicleData);
    velocityUpdate += hoverForce;

    velocityUpdate += GetImpactForceSum(m_vehicleStruct00.vehicleData);
  
    velocityUpdate += airResistance;

    Utility::Torque pendTorque;
    pendTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    pendTorque.magnitude = 0.0f;
    Utility::Torque bodyTorqueUpdate = UpdateBodyTorqueRunge(pendTorque, static_cast<float>(aTimeDelta));
    //bodyTorqueUpdate.axis = DirectX::SimpleMath::Vector3::Zero;
    //bodyTorqueUpdate.magnitude = 0.0f;
    //aDQ->bodyTorqueForce = bodyTorqueUpdate;
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
    impactTorque.magnitude *= 0.0002f;    

    const DirectX::SimpleMath::Vector3 centerMassPos = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    const DirectX::SimpleMath::Vector3 rotorPos = m_vehicleStruct00.vehicleData.hardPoints.verticalStabilizerPos;
    DirectX::SimpleMath::Vector3 gravityForce = -m_vehicleStruct00.environment->GetGravityVec();
    DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);


    DirectX::SimpleMath::Vector3 tailRotorTorqueArm = m_vehicleStruct00.vehicleData.hardPoints.steeringTorqueArmPos - centerMassPos;
    DirectX::SimpleMath::Vector3 tailForce = -m_vehicleStruct00.vehicleData.right * (m_vehicleStruct00.vehicleData.controlInput.steeringInput) * 1.01f;
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
        //m_vehicleStruct00.vehicleData.controlInput.steeringInput = 0.0f;
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }
    else
    {
        m_vehicleStruct00.vehicleData.controlInput.steeringInput = updatedYaw;
    }
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

    //m_vehicleStruct00.vehicleData.controlInput.throttleInput = m_npcAI->GetThrottleInput();
    m_vehicleStruct00.vehicleData.hoverData.forwardThrust = m_vehicleStruct00.vehicleData.controlInput.throttleInput * m_vehicleStruct00.vehicleData.hoverData.forwardThrustMax;

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

void NPCVehicle::UpdateNPC(const double aTimeDelta)
{
    DirectX::SimpleMath::Vector3 preVelocity = m_vehicleStruct00.vehicleData.q.velocity;

    DirectX::SimpleMath::Vector3 preThrust = m_vehicleStruct00.vehicleData.controlInput.steeringVec * (m_vehicleStruct00.vehicleData.hoverData.forwardThrust);
    float preThrustLength = preThrust.Length();


    m_vehicleStruct00.vehicleData.terrainHightAtPos = m_vehicleStruct00.environment->GetTerrainHeightAtPos(m_vehicleStruct00.vehicleData.q.position);
    m_vehicleStruct00.vehicleData.altitude = m_vehicleStruct00.vehicleData.hardPoints.basePos.y - m_vehicleStruct00.vehicleData.terrainHightAtPos;
    m_vehicleStruct00.vehicleData.terrainNormal = m_vehicleStruct00.environment->GetTerrainNormal(m_vehicleStruct00.vehicleData.q.position);

    m_npcAI->UpdateAI(static_cast<float>(aTimeDelta));
    //UpdateControlInput();
    UpdateControlInputFromAi();

    RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);

    m_vehicleStruct00.vehicleData.collisionBox.Center = m_vehicleStruct00.vehicleData.q.position;

    UpdateAlignment();

    UpdateNPCModel(aTimeDelta);
    UpdateHardPoints();

    if (m_vehicleStruct00.vehicleData.isCollisionTrue == true)
    {
        const int id = m_vehicleStruct00.vehicleData.id;
        DirectX::SimpleMath::Vector3 impact = GetImpactForceSum(m_vehicleStruct00.vehicleData);
        DirectX::SimpleMath::Vector3 impactNorm = impact;
        impactNorm.Normalize();
        const float impactLength = impact.Length();
        const float preVelocityLength = preVelocity.Length();
        DirectX::SimpleMath::Vector3 postVelocity = m_vehicleStruct00.vehicleData.q.velocity;
        const float postVelocityLength = postVelocity.Length();

        DirectX::SimpleMath::Vector3 postThrust = m_vehicleStruct00.vehicleData.controlInput.steeringVec * (m_vehicleStruct00.vehicleData.hoverData.forwardThrust);
        float postThrustLength = postThrust.Length();

        int xxx = 0;
        xxx++;
    }

    m_vehicleStruct00.vehicleData.isCollisionTrue = false;

    m_vehicleStruct00.vehicleData.impactForce.impactMass = 0.0f;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude = 0.0f;

    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;

    //CalculateSelfRightingTorque();

    m_vehicleStruct00.vehicleData.testForce = DirectX::SimpleMath::Vector3::Zero;

    //m_npcAI->UpdateAI(static_cast<float>(aTimeDelta));
    //UpdateControlInput();
    //m_debugData->DebugPushUILineDecimalNumber("Velocity = ", m_vehicleStruct00.vehicleData.q.velocity.Length(), "");

    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, .0f));

    UpdateForceTorqueVecs();
}

void NPCVehicle::UpdateNPCModel(const double aTimeDelta)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.npcModel.worldModelMatrix = m_vehicleStruct00.npcModel.localModelMatrix;
    m_vehicleStruct00.npcModel.worldModelMatrix *= updateMat;

    m_vehicleStruct00.npcModel.worldForwardMatrix = m_vehicleStruct00.npcModel.localForwardMatrix;
    m_vehicleStruct00.npcModel.worldForwardMatrix *= updateMat;
}

void NPCVehicle::UpdatePlayerPos(const DirectX::SimpleMath::Vector3 aPlayerPos)
{
    m_vehicleStruct00.vehicleData.playerPos = aPlayerPos;
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

void NPCVehicle::UpdateTestForce(const DirectX::SimpleMath::Vector3 aForce, const float aVal)
{
    m_vehicleStruct00.vehicleData.testForce += m_vehicleStruct00.vehicleData.forward * aVal * 10.0f;
}