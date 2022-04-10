#include "pch.h"
#include "NPCVehicle.h"

/*
NPCVehicle::NPCVehicle()
{

}
*/

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

void NPCVehicle::CalculateSelfRightingTorque()
{
    //DirectX::SimpleMath::Vector3 gravityTorqueArm = centerMassPos - rotorPos;
    DirectX::SimpleMath::Vector3 gravityTorqueArm = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos - m_vehicleStruct00.vehicleData.hardPoints.testArmPos;
    
    const float modVal = 0.0001f;
    DirectX::SimpleMath::Vector3 gravityForce = (DirectX::SimpleMath::Vector3(0.0f, 9.8f, 0.0f)) * modVal;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce = gravTorque;
}

void NPCVehicle::DrawNPC(const DirectX::SimpleMath::Matrix aView, const DirectX::SimpleMath::Matrix aProj)
{
    DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    if (m_vehicleStruct00.vehicleData.isCollisionTrue == true)
    {
        color = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    //m_vehicleData.npcModel.modelShape->Draw(m_vehicleData.npcModel.worldModelMatrix, aView, aProj, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_vehicleStruct00.npcModel.modelShape->Draw(m_vehicleStruct00.npcModel.worldModelMatrix, aView, aProj, color);

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
        gravForce *= -1.0f;
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

DirectX::SimpleMath::Vector3 NPCVehicle::GetHoverLift(const VehicleData& aVehicleData)
{
    //DirectX::SimpleMath::Vector3 liftForce = aVehicleData.hoverData.hoverLiftNeutralWithGrav;
    DirectX::SimpleMath::Vector3 liftForce = aVehicleData.up * 9.8f;
    const float lowerCurveBound = aVehicleData.hoverData.hoverRangeLower;
    const float midCurveBound = aVehicleData.hoverData.hoverRangeMid;
    const float upperCurveBound = aVehicleData.hoverData.hoverRangeUpper;

    if (aVehicleData.altitude < lowerCurveBound)
    {
        liftForce *= 2.0f;
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
    aVehicleStruct.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 7.0f, 9.5f);
    aVehicleStruct.vehicleData.dimensions = DirectX::SimpleMath::Vector3(14.0f, 14.0f, 14.0f);
    // to do update when transisioned from sphere to box
    //aVehicleStruct.vehicleData.collisionBox.Radius = aVehicleStruct.vehicleData.dimensions.x;
    aVehicleStruct.vehicleData.collisionBox.Extents = aVehicleStruct.vehicleData.dimensions * 0.5f;

    aVehicleStruct.vehicleData.collisionBox.Center = DirectX::SimpleMath::Vector3::Zero;
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
    // zero out impact torque
    aVehicleStruct.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.impactTorque.magnitude = 0.0f;

    aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.centerOfMassPos = aVehicleStruct.vehicleData.hardPoints.localCenterOfMassPos;
    aVehicleStruct.vehicleData.hardPoints.localWeaponDirection = DirectX::SimpleMath::Vector3::UnitX;
    aVehicleStruct.vehicleData.hardPoints.weaponDirection = aVehicleStruct.vehicleData.hardPoints.localWeaponDirection;    
    aVehicleStruct.vehicleData.hardPoints.localWeaponPos = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hardPoints.weaponPos = aVehicleStruct.vehicleData.hardPoints.localWeaponPos;

    aVehicleStruct.vehicleData.hardPoints.localTestArmPos = DirectX::SimpleMath::Vector3(0.0f, -10.0f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.testArmPos = aVehicleStruct.vehicleData.hardPoints.localTestArmPos;

    aVehicleStruct.vehicleData.hardPoints.localBasePos = DirectX::SimpleMath::Vector3(0.0f, -aVehicleStruct.vehicleData.dimensions.y * 0.5f, 0.0f);
    aVehicleStruct.vehicleData.hardPoints.basePos = aVehicleStruct.vehicleData.hardPoints.localBasePos;

    aVehicleStruct.vehicleData.testForce = DirectX::SimpleMath::Vector3::Zero;

    aVehicleStruct.vehicleData.hoverData.hoverLiftNeutralWithGrav = -aEnvironment->GetGravityVec();
    aVehicleStruct.vehicleData.hoverData.hoverLiftMax = -aEnvironment->GetGravityVec() * 1.2f;
    aVehicleStruct.vehicleData.hoverData.hoverLiftCurrent = DirectX::SimpleMath::Vector3::Zero;
    aVehicleStruct.vehicleData.hoverData.turnRateCurrent = 0.0f;
    aVehicleStruct.vehicleData.hoverData.turnRateMax = 3.0f;
}

void NPCVehicle::InitializeNPCModel(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext, NPCModel& aModel)
{
    const DirectX::SimpleMath::Vector3 vehicleSize(14.0f, 7.0f, 9.5f);
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), vehicleSize);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldModelMatrix = aModel.localModelMatrix;
}

void NPCVehicle::InitializeNPCModelStruct(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    NPCModel& aModel, const DirectX::SimpleMath::Vector3 aDimensions)
{
    aModel.modelShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), aDimensions);
    aModel.localModelMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.worldModelMatrix = aModel.localModelMatrix;
}

void NPCVehicle::InitializeNPCVehicle2(Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext,
    const DirectX::SimpleMath::Vector3 aHeading, const DirectX::SimpleMath::Vector3 aPosition, Environment const* aEnvironment)
{
    //InitializeNPCData(m_vehicleData, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    //InitializeNPCModel(aContext, m_vehicleData.npcModel);
    m_environment = aEnvironment;
    InitializeNPCStruct(m_vehicleStruct00, aHeading, aPosition, NPCType::NPCTYPE_NPC00, aEnvironment);
    InitializeNPCModelStruct(aContext, m_vehicleStruct00.npcModel, m_vehicleStruct00.vehicleData.dimensions);
}

void NPCVehicle::RightHandSide(struct VehicleData* aVehicle, MotionNPC* aQ, MotionNPC* aDeltaQ, double aTimeDelta, float aQScale, MotionNPC* aDQ)
{
    //  Compute the intermediate values of the 
//  dependent variables.
    MotionNPC newQ;
    newQ.velocity = aQ->velocity + static_cast<float>(aQScale) * aDeltaQ->velocity;
    newQ.position = aQ->position + static_cast<float>(aQScale) * aDeltaQ->position;

    float v = sqrt(newQ.velocity.Length() * newQ.velocity.Length()) + 1.0e-8f;

    //  Compute the total drag force
    float airDensity = 1.225f; // ToDo rework to pull data from environment
    float dragCoefficient = aVehicle->dragCoefficient;
    float frontSurfaceArea = aVehicle->frontalArea;
    float frontDragResistance = 0.5f * airDensity * frontSurfaceArea * dragCoefficient * v * v;
    float mass = 1.0f;
    //mass = aVehicle->mass;
    DirectX::SimpleMath::Vector3 velocityNorm = aVehicle->q.velocity;
    velocityNorm.Normalize();

    DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance / mass));
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (static_cast<float>(aTimeDelta) * (-frontDragResistance));
    //DirectX::SimpleMath::Vector3 airResistance = velocityNorm * (-frontDragResistance);
    m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, airResistance, 15.f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, .0f));
    
    float airRLength = airResistance.Length();
    m_debugData->DebugPushUILineDecimalNumber(std::string("airRLength = "), airRLength, std::string(""));
    DirectX::SimpleMath::Vector3 velocityUpdate = DirectX::SimpleMath::Vector3::Zero;

    //velocityUpdate = aVehicle->testForce;

    if (aVehicle->altitude < 10.0f)
    {
        //DirectX::SimpleMath::Vector3 testVel = DirectX::SimpleMath::Vector3(0.0f, 10.0f, 0.0f) * aVehicle->mass;
        DirectX::SimpleMath::Vector3 testVel = DirectX::SimpleMath::Vector3(0.0f, 10.0f, 0.0f);
        //velocityUpdate += testVel;
    }
    else
    {
        //DirectX::SimpleMath::Vector3 testVel = m_environment->GetGravityVec() * aVehicle->mass;
        DirectX::SimpleMath::Vector3 testVel = m_environment->GetGravityVec() * 1.0f;
        //velocityUpdate += testVel;
    }

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

    velocityUpdate += m_vehicleStruct00.vehicleData.impactForce.impactVelocity;
    velocityUpdate += airResistance;

    //DirectX::SimpleMath::Vector3 gravForce = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    //DirectX::SimpleMath::Vector3 gravForce = GetAntiGravGravityForce(GetAltitude(), aHeli->groundNormalForceRange, aHeli->gravity, aHeli->mass);

    //velocityUpdate += gravForce;

    m_debugData->DebugPushUILineDecimalNumber(std::string("impactVelocity"), m_vehicleStruct00.vehicleData.impactForce.impactVelocity.Length(), std::string(""));
    
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
    /*
    if (m_heli.isVehicleAirborne == false)
    {
        DirectX::SimpleMath::Vector3 newUp = m_heli.terrainNormal;
        DirectX::SimpleMath::Vector3 oldUp = m_heli.up;
        DirectX::SimpleMath::Vector3 updateUp = DirectX::SimpleMath::Vector3::SmoothStep(oldUp, newUp, 0.2);
        updateUp.Normalize();
        m_heli.up = updateUp;
        m_heli.right = m_heli.forward.Cross(m_heli.up);
        m_heli.right.Normalize();
        m_heli.forward = m_heli.up.Cross(m_heli.right);
        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }
    else
    {
        DirectX::SimpleMath::Matrix preAlignment = m_heli.alignment;
        DirectX::SimpleMath::Quaternion preAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_heli.alignment);
        DirectX::SimpleMath::Matrix torqueMat;
        const float tol = 0.0001f;
        if (abs(m_heli.q.bodyTorqueForce.magnitude) < tol || m_heli.q.bodyTorqueForce.axis.Length() < tol)
        {
            //torqueMat = DirectX::SimpleMath::Matrix::Identity;
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitX, 0.0f);
        }
        else
        {
            torqueMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_heli.q.bodyTorqueForce.axis, m_heli.q.bodyTorqueForce.magnitude);
        }

        const float t = 0.5f;
        DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(torqueMat);
        DirectX::SimpleMath::Matrix postAlignment = DirectX::SimpleMath::Matrix::Transform(preAlignment, rotQuat);

        m_heli.alignment = DirectX::SimpleMath::Matrix::Lerp(preAlignment, postAlignment, t);

        m_heli.up = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitY, m_heli.alignment);
        m_heli.right = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitZ, m_heli.alignment);
        m_heli.forward = DirectX::SimpleMath::Vector3::TransformNormal(DirectX::SimpleMath::Vector3::UnitX, m_heli.alignment);

        m_heli.alignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_heli.right, m_heli.up);
    }
    */

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
    const float modVal = 0.01f;
    const DirectX::SimpleMath::Vector3 centerMassPos = m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos;
    const DirectX::SimpleMath::Vector3 rotorPos = m_vehicleStruct00.vehicleData.hardPoints.testArmPos;
    const DirectX::SimpleMath::Vector3 impactTorqueArm = rotorPos - centerMassPos;

    Utility::Torque impactTorque = m_vehicleStruct00.vehicleData.impactTorque;
    impactTorque.axis.Normalize();
    
    DirectX::SimpleMath::Vector3 gravityForce = m_vehicleStruct00.environment->GetGravityVec() * modVal;
    gravityForce = m_vehicleStruct00.environment->GetGravityVec();
    DirectX::SimpleMath::Vector3 gravityTorqueArm = rotorPos - centerMassPos;
    Utility::Torque gravTorque = Utility::GetTorqueForce(gravityTorqueArm, gravityForce);
    
    Utility::Torque turnTestTorque;
    turnTestTorque.axis = m_vehicleStruct00.vehicleData.up;
    turnTestTorque.magnitude = 0.0f;;

    DirectX::SimpleMath::Vector3 torqueAxis = (impactTorque.axis * impactTorque.magnitude) + (gravTorque.axis * gravTorque.magnitude) + (turnTestTorque.axis * turnTestTorque.magnitude);
    float torqueMag = impactTorque.magnitude + gravTorque.magnitude + turnTestTorque.magnitude;
    Utility::Torque updatedTorque;
    updatedTorque.axis = torqueAxis;
    updatedTorque.magnitude = torqueMag / m_vehicleStruct00.vehicleData.mass;
    return updatedTorque;
}

void NPCVehicle::UpdateNPC(const double aTimeDelta)
{  
 
    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.position.x"), m_vehicleStruct00.vehicleData.q.position.x, std::string(""));
    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.position.y"), m_vehicleStruct00.vehicleData.q.position.y, std::string(""));
    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.position.z"), m_vehicleStruct00.vehicleData.q.position.z, std::string(""));

    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.velocity.x"), m_vehicleStruct00.vehicleData.q.velocity.x, std::string(""));
    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.velocity.y"), m_vehicleStruct00.vehicleData.q.velocity.y, std::string(""));
    m_debugData->DebugPushUILineDecimalNumber(std::string("m_vehicleStruct00.vehicleData.q.velocity.z"), m_vehicleStruct00.vehicleData.q.velocity.z, std::string(""));
    
    m_debugData->DebugPushUILineDecimalNumber(std::string("Altitude = "), m_vehicleStruct00.vehicleData.altitude, std::string(""));

    m_vehicleStruct00.vehicleData.terrainHightAtPos = m_vehicleStruct00.environment->GetTerrainHeightAtPos(m_vehicleStruct00.vehicleData.q.position);
    m_vehicleStruct00.vehicleData.altitude = m_vehicleStruct00.vehicleData.hardPoints.basePos.y - m_vehicleStruct00.vehicleData.terrainHightAtPos;
    m_vehicleStruct00.vehicleData.terrainNormal = m_vehicleStruct00.environment->GetTerrainNormal(m_vehicleStruct00.vehicleData.q.position);
    //m_vehicleStruct00.vehicleData.terrainNormal = m_environment->GetTerrainNormal(m_vehicleStruct00.vehicleData.q.position);
    RungeKutta4(&m_vehicleStruct00.vehicleData, aTimeDelta);

    m_vehicleStruct00.vehicleData.collisionBox.Center = m_vehicleStruct00.vehicleData.q.position;

    UpdateAlignment();

    UpdateNPCModel(aTimeDelta);
    UpdateHardPoints();
    m_vehicleStruct00.vehicleData.isCollisionTrue = false;

    m_vehicleStruct00.vehicleData.impactForce.impactMass = 0.0f;
    m_vehicleStruct00.vehicleData.impactForce.impactVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude = 0.0f;
    //m_vehicleStruct00.vehicleData.q.bodyTorqueForce.magnitude *= 0.2f;

    m_vehicleStruct00.vehicleData.impactTorque.axis = DirectX::SimpleMath::Vector3::Zero;
    m_vehicleStruct00.vehicleData.impactTorque.magnitude = 0.0f;

    //CalculateSelfRightingTorque();

    m_vehicleStruct00.vehicleData.testForce = DirectX::SimpleMath::Vector3::Zero;

    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.up, 15.f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, .0f));
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.forward, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, .0f));
    //m_debugData->DebugPushTestLine(m_vehicleStruct00.vehicleData.q.position, m_vehicleStruct00.vehicleData.right, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, .0f));
}

void NPCVehicle::UpdateNPCModel(const double aTimeDelta)
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    m_vehicleStruct00.npcModel.worldModelMatrix = m_vehicleStruct00.npcModel.localModelMatrix;
    m_vehicleStruct00.npcModel.worldModelMatrix *= updateMat;
}

void NPCVehicle::UpdateHardPoints()
{
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleStruct00.vehicleData.q.position, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);
    DirectX::SimpleMath::Matrix updateDirectionMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_vehicleStruct00.vehicleData.right, m_vehicleStruct00.vehicleData.up);

    m_vehicleStruct00.vehicleData.hardPoints.centerOfMassPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localCenterOfMassPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponDirection = DirectX::SimpleMath::Vector3::TransformNormal(m_vehicleStruct00.vehicleData.hardPoints.localWeaponDirection, updateDirectionMat);
    m_vehicleStruct00.vehicleData.hardPoints.weaponPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localWeaponPos, updateMat);

    m_vehicleStruct00.vehicleData.hardPoints.testArmPos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localTestArmPos, updateMat);
    m_vehicleStruct00.vehicleData.hardPoints.basePos = DirectX::SimpleMath::Vector3::Transform(m_vehicleStruct00.vehicleData.hardPoints.localBasePos, updateMat);
}

void NPCVehicle::UpdateTestForce(const DirectX::SimpleMath::Vector3 aForce, const float aVal)
{
    //m_vehicleStruct00.vehicleData.testForce += aForce;
    m_vehicleStruct00.vehicleData.testForce += m_vehicleStruct00.vehicleData.forward * aVal * 10.0f;
}