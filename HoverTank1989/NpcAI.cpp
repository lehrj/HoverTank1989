#include "pch.h"
#include "NpcAI.h"
#include "NPCVehicle.h"
#include "Vehicle.h"


NpcAI::NpcAI(const NPCVehicle* aOwner)
{
    m_npcOwner = aOwner;
}

void NpcAI::AdjustHeadingForVelocity()
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination;
    DirectX::SimpleMath::Vector3 pos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 velocity = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vecToDest = dest - pos;
    float distanceToDest = vecToDest.Length();
    float velocityDotHeading = vecToDest.Dot(velocity);
    DirectX::SimpleMath::Vector3 velocityNorm = velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 headingNorm = vecToDest;
    headingNorm.Normalize();
    float velocityNormDotHeadingNorm = headingNorm.Dot(velocityNorm);
    //float lookAheadTime = distanceToDest / m_npcOwner->GetTopSpeedCalculated();
    //float lookAheadTime = distanceToDest / 100.0f;
    float lookAheadTime = distanceToDest / (velocity.Length() + 50.0f);
    //m_debugData->DebugPushUILineDecimalNumber("lookAheadTime =  ", lookAheadTime, "");
    DirectX::SimpleMath::Vector3 adjustedDest = m_currentDestination;
    const float adjustmentMagnitude = 15.0f;
    adjustedDest -= velocityNorm * (1.0f + lookAheadTime) * (1.0f + (1.0f - velocityNormDotHeadingNorm)) * adjustmentMagnitude;
    m_currentDestination = adjustedDest;

    //m_debugData->DebugPushUILineDecimalNumber("delta =  ", (dest - adjustedDest).Length(), "");
    //m_debugData->DebugPushTestLine(adjustedDest, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), adjustedDest, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    /*
    m_debugData->DebugPushUILineDecimalNumber("velocity =  ", velocity.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("velocityNormDotHeadingNorm ", velocityNormDotHeadingNorm, "");
    m_debugData->DebugPushTestLine(pos, vecToDest, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(dest, DirectX::SimpleMath::Vector3::UnitY, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    */
}

void NpcAI::AdjustHeadingForVelocity2()
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination;
    DirectX::SimpleMath::Vector3 pos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 velocity = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vecToDest = dest - pos;
    float distanceToDest = vecToDest.Length();
    float velocityDotHeading = vecToDest.Dot(velocity);
    DirectX::SimpleMath::Vector3 velocityNorm = velocity;
    velocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 headingNorm = vecToDest;
    headingNorm.Normalize();
    float velocityNormDotHeadingNorm = headingNorm.Dot(velocityNorm);
    //float lookAheadTime = distanceToDest / m_npcOwner->GetTopSpeedCalculated();
    float lookAheadTime = distanceToDest / 100.0f;
    m_debugData->DebugPushUILineDecimalNumber("lookAheadTime =  ", lookAheadTime, "");
    DirectX::SimpleMath::Vector3 adjustedDest = m_currentDestination;;
    adjustedDest -= velocityNorm * (1.0f + lookAheadTime) * (1.0f + (1.0f - velocityNormDotHeadingNorm)) * 50.0f;
    m_currentDestination = adjustedDest;
    m_debugData->DebugPushUILineDecimalNumber("delta =  ", (dest - adjustedDest).Length(), "");
    m_debugData->DebugPushTestLine(adjustedDest, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushUILineDecimalNumber("velocity =  ", velocity.Length(), "");
    m_debugData->DebugPushUILineDecimalNumber("velocityNormDotHeadingNorm ", velocityNormDotHeadingNorm, "");
    m_debugData->DebugPushTestLine(pos, vecToDest, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(dest, DirectX::SimpleMath::Vector3::UnitY, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int stopBreak = 0;
}

void NpcAI::AvoidPos2()
{
    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    const float distance = localPos.Length();
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    float multiplier = 1.0f + (m_avoidanceBoxLength - localPos.x) / m_avoidanceBoxLength;
    m_debugData->DebugPushUILineDecimalNumber("multiplier = ", multiplier, "");
    const float obstacleRadius = 7.0f; // temp place holder for testing, will need to pull radius from obstacle later
    float lateralVal = (obstacleRadius - localPos.z) * multiplier;
    //lateralVal = 0.0f;
    m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");
    //vecToDest = m_npcOwner->GetForward();
    DirectX::SimpleMath::Vector3 newDest = vecToDest * distToDest;
    DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(lateralVal));
    newDest = DirectX::SimpleMath::Vector3::Transform(newDest, rotMat);
    newDest += m_npcOwner->GetPos();
    //m_debugData->DebugPushTestLineBetweenPoints(m_currentDestination, m_npcOwner->GetPos(), DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
    m_currentDestination = newDest;
    //m_debugData->DebugPushTestLineBetweenPoints(m_currentDestination, m_npcOwner->GetPos(), DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));


    //start testing
    float xMod1 = (m_avoidanceBoxLength - localPos.x) / m_avoidanceBoxLength;
    float xMod2 = (xMod1 * 2.0f) - 2.0f;
    float zMod1 = (m_avoidanceBoxWidth - localPos.z) / m_avoidanceBoxWidth;
    float zMod2 = (zMod1 * 2.0f) - 2.0f;
    float zMod3 = m_avoidanceBoxWidth - localPos.z;
    float zMod4 = localPos.z / m_avoidanceBoxWidth;
    DirectX::SimpleMath::Vector3 testDest(xMod2, 0.0f, zMod2);
    testDest.Normalize();
    float testDistance = 20.0f;
    testDest *= testDistance;
    DirectX::SimpleMath::Matrix testAlignment = m_npcOwner->GetAlignment();
    testDest = DirectX::SimpleMath::Vector3::Transform(testDest, testAlignment);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testDest, 10.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    testDest += m_npcOwner->GetPos();
    //m_debugData->DebugPushTestLineBetweenPoints(testDest, m_npcOwner->GetPos(), DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
    m_currentDestination = testDest;

    testDest = DirectX::SimpleMath::Vector3::UnitX;
    testDest = m_npcOwner->GetForward();
    testDest *= testDistance;
    if (lateralVal > Utility::GetPi())
    {
        lateralVal = Utility::GetPi();
    }
    else if (lateralVal < -Utility::GetPi())
    {
        lateralVal = -Utility::GetPi();
    }
    m_debugData->DebugPushUILineDecimalNumber("lateralVal2 = ", lateralVal, "");
    //DirectX::SimpleMath::Matrix rotMat2 = DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(lateralVal));
    DirectX::SimpleMath::Matrix rotMat2 = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_npcOwner->GetUp(), lateralVal);
    testDest = DirectX::SimpleMath::Vector3::Transform(newDest, rotMat2);
    //testDest = DirectX::SimpleMath::Vector3::Transform(testDest, testAlignment);
    m_debugData->DebugPushTestLineBetweenPoints(testDest, m_npcOwner->GetPos(), DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));
    testDest += m_npcOwner->GetPos();
    m_debugData->DebugPushTestLineBetweenPoints(testDest, m_npcOwner->GetPos(), DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f));

}

void NpcAI::AvoidPos3()
{
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    const float distance = localPos.Length();
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());

    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    //m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);
    /*
    m_debugData->DebugPushTestLine(testCenter, DirectX::SimpleMath::Vector3::UnitY, 40.0f, 0.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    m_debugData->DebugPushTestLine(testRear, DirectX::SimpleMath::Vector3::UnitY, 40.0f, 0.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    m_debugData->DebugPushTestLine(testFront, DirectX::SimpleMath::Vector3::UnitY, 40.0f, 0.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    */
    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    m_currentDestination = avoidanceVec;
    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    testAvoidanceVec += m_npcOwner->GetPos();
    m_currentDestination = testAvoidanceVec;
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    DirectX::SimpleMath::Vector3 vecToTarget = m_npcOwner->GetPos() - m_avoidanceTargetPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    //testAvoidanceVec *= 10.0f;
    testAvoidanceVec *= testRatio;
    testAvoidanceVec += m_npcOwner->GetPos();
    m_currentDestination = testAvoidanceVec;
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos4()
{
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    const float distance = localPos.Length();
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());

    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    //m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);

    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    ///////////////////////m_currentDestination = avoidanceVec;



    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    testAvoidanceVec += m_npcOwner->GetPos();



    ///////m_currentDestination = testAvoidanceVec;





    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
    DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();

    /*
    targetPos = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    targetVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    selfPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    selfVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    */
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();

    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);

    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    //testAvoidanceVec *= 10.0f;


    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            directionToTarget.Normalize();
            DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            avoidVec += m_npcOwner->GetPos();
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }


    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;


    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos5()
{
    DirectX::SimpleMath::Vector3 preAvoidPos = m_currentDestination;
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    //avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    //owner.y += 4.5f;

    DirectX::SimpleMath::Vector3 targetPos = m_playerVehicle->GetPos();
    targetPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 targetVelocity = m_playerVehicle->GetVelocity();
    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    selfPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();

    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    const float distance = localPos.Length();
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());

    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    //m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);

    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    ///////////////////////m_currentDestination = avoidanceVec;



    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    testAvoidanceVec += m_npcOwner->GetPos();



    ///////m_currentDestination = testAvoidanceVec;





    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    //DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
    //DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    /*
    targetPos = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    targetVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    selfPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    selfVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    */
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();

    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);

    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    //testAvoidanceVec *= 10.0f;


    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            //DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - targetPos;
            directionToTarget.Normalize();
            //DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget2 = targetVelocity - m_npcOwner->GetPos();
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            avoidVec += m_npcOwner->GetPos();
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        //testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }


    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;

    m_debugData->DebugPushTestLinePositionIndicator(m_currentDestination, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos6()
{
    DirectX::SimpleMath::Vector3 preAvoidPos = m_currentDestination;
    //DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    //avoidTarget.y += 4.5f;
    //DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    //owner.y += 4.5f;
    
    DirectX::SimpleMath::Vector3 targetPos = m_playerVehicle->GetPos();
    targetPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 targetVelocity = m_playerVehicle->GetVelocity();
    //targetVelocity = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    selfPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();
    selfVelocity = DirectX::SimpleMath::Vector3::Zero;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    
    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    
    //DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 localPos = targetPos - selfPos;
    //const float distance = localPos.Length();
    
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    //DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    DirectX::SimpleMath::Vector3 vecToDest = selfPos - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    //toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-selfPos);

    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(selfPos, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    //DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal = selfPos;
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    //DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    DirectX::SimpleMath::Vector3 obsticlePos = targetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);
    m_debugData->DebugPushTestLinePositionIndicator(obsticlePos, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), avoidanceCenter, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    avoidanceCenter.y = 0.0f;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenterWorld = avoidanceCenter;
    DirectX::SimpleMath::Matrix toWorldCord = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix toWorldRotation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    avoidanceCenterWorld = DirectX::SimpleMath::Vector3::Transform(avoidanceCenterWorld, toWorldCord);
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), avoidanceCenterWorld, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    //avoidanceCenter.x += (m_avoidanceBox.Extents.x * 0.5f);
    DirectX::SimpleMath::Vector3 targetFromCenterNorm = obsticlePos - avoidanceCenter;
    targetFromCenterNorm.Normalize();
    DirectX::SimpleMath::Vector3 testFromCenterAvoidance = targetFromCenterNorm;
    //testFromCenterAvoidance.z *= -1.0f;
    m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 testFromCenterAvoidance2 = testFromCenterAvoidance;
    float testFloat = 0.0f;

    DirectX::SimpleMath::Vector3 testFromCenterAvoidance3 = DirectX::SimpleMath::Vector3::Zero;

    const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
    //const float xDistanceRatio = 1.0f - (xDistanceFromCenter / (m_avoidanceBox.Extents.x * 1.0f));
    const float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
    testFloat = xDistanceRatio;
    const float zDistanceFromCenter = obsticlePos.z - avoidanceCenter.z;
    const float zDistanceRatio = zDistanceFromCenter / m_avoidanceBox.Extents.z;
    m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", xDistanceRatio, "");
    m_debugData->DebugPushUILineDecimalNumber("zDistanceRatio = ", zDistanceRatio, "");
    testFromCenterAvoidance3.x = xDistanceRatio;
    testFromCenterAvoidance3.z = -zDistanceRatio;
    testFromCenterAvoidance3.Normalize();

    //testFromCenterAvoidance.x -= (1.0f - abs(zDistanceRatio));
    //testFromCenterAvoidance.z -= (1.0f - abs(xDistanceRatio));
    
    testFromCenterAvoidance.x = (1.0f - abs(zDistanceRatio));
    if (testFromCenterAvoidance.z <= 0.0f)
    {
        testFromCenterAvoidance.z = (1.0f + testFromCenterAvoidance.x);
    }
    else
    {
        testFromCenterAvoidance.z = (1.0f - testFromCenterAvoidance.x);
    }
    /*
    if (obsticlePos.x > avoidanceCenter.x)
    {
        if (testFromCenterAvoidance.z <= 0.0f)
        {
            testFromCenterAvoidance.x = -(1.0f - abs(zDistanceRatio));
            //testFromCenterAvoidance.z = -zDistanceRatio;
            testFromCenterAvoidance.z = (1.0f - abs(zDistanceRatio));
            testFromCenterAvoidance.z = (1.0f - testFromCenterAvoidance.x);
        }
        else
        {
            testFromCenterAvoidance.x = -(1.0f - abs(zDistanceRatio));
            //testFromCenterAvoidance.z = -zDistanceRatio;
            //testFromCenterAvoidance.z = -(1.0f - abs(zDistanceRatio));
            testFromCenterAvoidance.z = -(1.0f - testFromCenterAvoidance.x);
            //testFromCenterAvoidance.x = (1.0f - abs(xDistanceRatio));
            //testFromCenterAvoidance.z = (1.0f - abs(zDistanceRatio));
            //testFromCenterAvoidance = DirectX::SimpleMath::Vector3::UnitY;
        }
        //testFromCenterAvoidance.x -= (1.0f - abs(zDistanceRatio));
        //testFromCenterAvoidance.z -= (2.0f - abs(xDistanceRatio));
        //testFromCenterAvoidance.z *= -1.0f;
    }
    else
    {
        testFromCenterAvoidance.z *= -1.0f;
    }
    */

    if (obsticlePos.x > avoidanceCenter.x)
    {
        //testFromCenterAvoidance.x *= -1.0f;
        //testFromCenterAvoidance3.x += zDistanceRatio;
        /*
        const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
        //const float xDistanceRatio = 1.0f - (xDistanceFromCenter / (m_avoidanceBox.Extents.x * 1.0f));
        const float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
        testFloat = xDistanceRatio;
        */

        //testFromCenterAvoidance.x *= -1.0f;
        //testFromCenterAvoidance.z += xDistanceRatio;
        //testFromCenterAvoidance.x -= (2.0f - zDistanceRatio);
        if (abs(testFromCenterAvoidance.z) < abs(xDistanceRatio))
        //if (testFromCenterAvoidance.z < abs(xDistanceRatio))
        {
            //testFromCenterAvoidance.x *= -1.0f;
            /*
            const float zDelta = abs(xDistanceRatio) - abs(testFromCenterAvoidance.z);
            const float xDelta = abs(xDistanceRatio) - abs(testFromCenterAvoidance.x);
            if (testFromCenterAvoidance.z > 0.0f)
            {
                //testFromCenterAvoidance.z += 1.0f + xDistanceRatio;
                //testFromCenterAvoidance.z += 1.0 - xDistanceRatio;
                //testFromCenterAvoidance = DirectX::SimpleMath::Vector3::UnitY;
                //testFromCenterAvoidance.z += xDelta;
                testFromCenterAvoidance.x -= zDelta;
                //testFromCenterAvoidance.z += (1.0f  + xDelta);
                //testFromCenterAvoidance.x += (1.0f + zDelta);
                //testFromCenterAvoidance.x -= (1.0f + zDelta);
                testFromCenterAvoidance2.x *= -1.0f;
            }
            else if (testFromCenterAvoidance.z <= 0.0f)
            {
                //testFromCenterAvoidance.z -= 1.0 + xDistanceRatio;
                //testFromCenterAvoidance.z -= -1.0 + xDistanceRatio;
                //testFromCenterAvoidance = DirectX::SimpleMath::Vector3::UnitY;
                //testFromCenterAvoidance.z -= xDelta;
                testFromCenterAvoidance.x -= zDelta;
                //testFromCenterAvoidance.z -= (1.0f - xDelta);
                //testFromCenterAvoidance.x -= (1.0f - zDelta);
                //testFromCenterAvoidance.x -= (1.0f - zDelta);
                testFromCenterAvoidance2.x *= -1.0f;
            }
            else if (testFromCenterAvoidance.z == 0.0f)
            {
                testFromCenterAvoidance.z += 1.0 - xDistanceRatio;
                testFromCenterAvoidance = DirectX::SimpleMath::Vector3::UnitY;
  
                testFromCenterAvoidance2.x *= -1.0f;
            }      
            */
        }
        
        
    }

    //testFromCenterAvoidance.Normalize();
    testFromCenterAvoidance2.Normalize();
    //m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", testFloat, "");
    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld = testFromCenterAvoidance;
    targetFromCenterNormWorld = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld, toWorldRotation);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld2 = testFromCenterAvoidance2;
    targetFromCenterNormWorld2 = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld2, toWorldRotation);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld3 = testFromCenterAvoidance3;
    targetFromCenterNormWorld3 = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld3, toWorldRotation);

    //m_debugData->DebugPushTestLineBetweenPoints(avoidanceCenterWorld, targetFromCenterNormWorld, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld2, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld3, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_debugData->DebugPushTestLine(targetPos, DirectX::SimpleMath::Vector3::UnitY, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);

    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    ///////////////////////m_currentDestination = avoidanceVec;

    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    //testAvoidanceVec += m_npcOwner->GetPos();
    testAvoidanceVec += selfPos;

    ///////m_currentDestination = testAvoidanceVec;

    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    //DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
    //DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    /*
    targetPos = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    targetVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    selfPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    selfVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    */
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();

    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);

    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), lookAheadPos, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos() + lookAheadPos, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    
    
    
    
    /////m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidPos, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //////m_debugData->DebugPushTestLine(m_npcOwner->GetPos() + avoidPos, -DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    
    
    
    
    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    testAvoidanceVec *= 10.0f;
    
    /*
    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            //DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - targetPos;
            directionToTarget.Normalize();
            //DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget2 = targetVelocity - selfPos;
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            //avoidVec += m_npcOwner->GetPos();
            avoidVec += selfPos;
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }
    */


    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;

    testAvoidanceVec += m_npcOwner->GetPos();
    m_currentDestination = testAvoidanceVec;
    
    //m_debugData->DebugPushTestLinePositionIndicator(m_currentDestination, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos7()
{
    DirectX::SimpleMath::Vector3 preAvoidPos = m_currentDestination;
    //DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    //avoidTarget.y += 4.5f;
    //DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    //owner.y += 4.5f;

    DirectX::SimpleMath::Vector3 targetPos = m_playerVehicle->GetPos();
    targetPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 targetVelocity = m_playerVehicle->GetVelocity();
    //targetVelocity = DirectX::SimpleMath::Vector3::Zero;
    m_debugData->DebugPushUILineDecimalNumber("TargetVelocity = ", targetVelocity.Length(), "");
    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    selfPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();
    selfVelocity = -DirectX::SimpleMath::Vector3::UnitX;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();

    //DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 localPos = targetPos - selfPos;
    //const float distance = localPos.Length();

    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    //DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    DirectX::SimpleMath::Vector3 vecToDest = selfPos - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    //toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-selfPos);

    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(selfPos, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    //DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal = selfPos;
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    //DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    DirectX::SimpleMath::Vector3 obsticlePos = targetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);
    m_debugData->DebugPushTestLinePositionIndicator(obsticlePos, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), avoidanceCenter, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    avoidanceCenter.y = 0.0f;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenterWorld = avoidanceCenter;
    DirectX::SimpleMath::Matrix toWorldCord = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix toWorldRotation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    avoidanceCenterWorld = DirectX::SimpleMath::Vector3::Transform(avoidanceCenterWorld, toWorldCord);
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), avoidanceCenterWorld, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    m_debugData->DebugClearUI();


    const float distanceBetweenTest = (selfPos - targetPos).Length();
    const float velocityMagTest = (selfVelocity - targetVelocity).Length();
    const float impactTimeTest = distanceBetweenTest / (velocityMagTest + 0.000000001f);
    const float lookAheadTimeTest = impactTimeTest;
    DirectX::SimpleMath::Vector3 avoidPosTest = targetPos + (targetVelocity * lookAheadTimeTest);
    DirectX::SimpleMath::Vector3 lookAheadPosTest = selfPos + (selfVelocity * lookAheadTimeTest);
    DirectX::SimpleMath::Vector3 vecToTargetTest = lookAheadPosTest - avoidPosTest;
    vecToTargetTest += m_npcOwner->GetPos();
    m_debugData->DebugPushTestLinePositionIndicator(avoidPosTest, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    m_debugData->DebugPushTestLinePositionIndicator(lookAheadPosTest, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLinePositionIndicator(vecToTargetTest, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    DirectX::SimpleMath::Vector3 impactPosTest = avoidPosTest;
    impactPosTest = DirectX::SimpleMath::Vector3::Transform(impactPosTest, localMat);
    obsticlePos = impactPosTest;


    //avoidanceCenter.x += (m_avoidanceBox.Extents.x * 0.5f);
    DirectX::SimpleMath::Vector3 targetFromCenterNorm = obsticlePos - avoidanceCenter;
    targetFromCenterNorm.Normalize();
    DirectX::SimpleMath::Vector3 testFromCenterAvoidance = targetFromCenterNorm;
    //testFromCenterAvoidance.z *= -1.0f;

    DirectX::SimpleMath::Vector3 testFromCenterAvoidance2 = testFromCenterAvoidance;
    float testFloat = 0.0f;

    DirectX::SimpleMath::Vector3 testFromCenterAvoidance3 = DirectX::SimpleMath::Vector3::Zero;

    const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
    //const float xDistanceRatio = 1.0f - (xDistanceFromCenter / (m_avoidanceBox.Extents.x * 1.0f));
    const float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
    testFloat = xDistanceRatio;
    const float zDistanceFromCenter = obsticlePos.z - avoidanceCenter.z;
    const float zDistanceRatio = zDistanceFromCenter / m_avoidanceBox.Extents.z;
    m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", xDistanceRatio, "");
    m_debugData->DebugPushUILineDecimalNumber("zDistanceRatio = ", zDistanceRatio, "");
    testFromCenterAvoidance3.x = xDistanceRatio;
    testFromCenterAvoidance3.z = -zDistanceRatio;
    testFromCenterAvoidance3.Normalize();

    //testFromCenterAvoidance.x -= (1.0f - abs(zDistanceRatio));
    //testFromCenterAvoidance.z -= (1.0f - abs(xDistanceRatio));

    /*
    testFromCenterAvoidance.x = (1.0f - abs(zDistanceRatio));
    if (testFromCenterAvoidance.z <= 0.0f)
    {
        testFromCenterAvoidance.z = (1.0f + testFromCenterAvoidance.x);
    }
    else
    {
        testFromCenterAvoidance.z = (1.0f - testFromCenterAvoidance.x);
    }
    */

    /*
    //if (obsticlePos.x > avoidanceCenter.x)

        //testFromCenterAvoidance.x *= -1.0f;
        //testFromCenterAvoidance3.x += zDistanceRatio;

        const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
        //const float xDistanceRatio = 1.0f - (xDistanceFromCenter / (m_avoidanceBox.Extents.x * 1.0f));
        const float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
        testFloat = xDistanceRatio;


        //testFromCenterAvoidance.x *= -1.0f;
        //testFromCenterAvoidance.z += xDistanceRatio;
        //testFromCenterAvoidance.x -= (2.0f - zDistanceRatio);
        //if (abs(testFromCenterAvoidance.z) < abs(xDistanceRatio))
        //if (testFromCenterAvoidance.z < abs(xDistanceRatio))
    */


    float zRotVal1 = 0.0f;
    float xRotVal1 = 0.0f;
    float zRotVal2 = 0.0f;
    float xRotVal2 = 0.0f;
    float zRotVal3 = 0.0f;
    float xRotVal3 = 0.0f;
    m_debugData->DebugPushUILineDecimalNumber("TargetVelocity = ", targetVelocity.Length(), "");
    if (obsticlePos.x < avoidanceCenter.x)
    {
        //zRotVal1 = -testFromCenterAvoidance.z;
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = (-testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f)) * zDistanceRatio;
        //xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f);

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);

        //zRotVal3 = zDistanceRatio * (Utility::GetPi() * 0.5f);
        //xRotVal3 = xDistanceRatio * (Utility::GetPi() * 0.5f);
        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = (-testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f)) * xDistanceRatio;
    }
    if (obsticlePos.x > avoidanceCenter.x)
    {
        //zRotVal1 = -testFromCenterAvoidance.z;
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = (-testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f)) * zDistanceRatio;
        //xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f);
        //xRotVal1 = testFromCenterAvoidance.x;
        //xRotVal1 = (1.0f - abs(xDistanceRatio));

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        //xRotVal2 = (-testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f)) * zDistanceRatio;

        //zRotVal3 = zDistanceRatio * (Utility::GetPi() * 0.5f);
        //xRotVal3 = -xDistanceRatio * (Utility::GetPi() * 0.5f);
        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = (-testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f)) * xDistanceRatio;
    }

    //zRotVal1 = -zDistanceRatio * (Utility::GetPi() * 0.5f);

    DirectX::SimpleMath::Vector3 testRotAvoid1 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Matrix zRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal1);
    DirectX::SimpleMath::Matrix xRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, zRot1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, xRot1);

    DirectX::SimpleMath::Vector3 testRotAvoid2 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Matrix zRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal2);
    DirectX::SimpleMath::Matrix xRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, zRot2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, xRot2);

    DirectX::SimpleMath::Vector3 testRotAvoid3 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Matrix zRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal3);
    DirectX::SimpleMath::Matrix xRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, zRot3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, xRot3);
    //testFromCenterAvoidance.Normalize();
    testFromCenterAvoidance2.Normalize();
    //m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", testFloat, "");
    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld = testFromCenterAvoidance;
    targetFromCenterNormWorld = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld, toWorldRotation);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld2 = testFromCenterAvoidance2;
    targetFromCenterNormWorld2 = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld2, toWorldRotation);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld3 = testFromCenterAvoidance3;
    targetFromCenterNormWorld3 = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld3, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld1 = testRotAvoid1;
    testRotAvoidWorld1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld1, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld2 = testRotAvoid2;
    testRotAvoidWorld2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld2, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld3 = testRotAvoid3;
    testRotAvoidWorld3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld3, toWorldRotation);

    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld1, 20.0f, 5.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld2, 20.0f, 6.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld3, 20.0f, 7.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLine(testRotAvoidWorld, DirectX::SimpleMath::Vector3::UnitY, 50.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(avoidanceCenterWorld, targetFromCenterNormWorld, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld2, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(avoidanceCenterWorld, targetFromCenterNormWorld3, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_debugData->DebugPushTestLine(targetPos, DirectX::SimpleMath::Vector3::UnitY, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);

    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    ///////////////////////m_currentDestination = avoidanceVec;

    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    //testAvoidanceVec += m_npcOwner->GetPos();
    testAvoidanceVec += selfPos;

    ///////m_currentDestination = testAvoidanceVec;

    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    //DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
    //DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    /*
    targetPos = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    targetVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    selfPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    selfVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    */
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();

    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);

    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), lookAheadPos, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos() + lookAheadPos, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));




    /////m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidPos, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //////m_debugData->DebugPushTestLine(m_npcOwner->GetPos() + avoidPos, -DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));




    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    testAvoidanceVec *= 10.0f;

    /*
    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            //DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - targetPos;
            directionToTarget.Normalize();
            //DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget2 = targetVelocity - selfPos;
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            //avoidVec += m_npcOwner->GetPos();
            avoidVec += selfPos;
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }
    */


    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;

    testAvoidanceVec += m_npcOwner->GetPos();
    m_currentDestination = testAvoidanceVec;

    //m_debugData->DebugPushTestLinePositionIndicator(m_currentDestination, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPosOld()
{
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTargetPos;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    const float distance = localPos.Length();
    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());

    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal2 = DirectX::SimpleMath::Vector3::Transform(testLocal, localMat);

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    const float obstacleRadius = 5.0f; // temp place holder for testing, will need to pull radius from obstacle later
    const float avoidanceBoxWidth = m_avoidanceBoxWidth * 2.0f;
    float lateralVal;
    // get lateral force val
    if (obsticlePos.z <= 0.0f) //obstacle to the left 
    {
        float lateralAvoidPos = obsticlePos.z + obstacleRadius;
        float testPosMod = lateralAvoidPos + (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        //float testLateralVal = testPosMod / (avoidanceBoxWidth * 0.5f);
        lateralVal = testLateralVal;
    }
    else
    {
        float lateralAvoidPos = obsticlePos.z - obstacleRadius;
        float testPosMod = lateralAvoidPos - (avoidanceBoxWidth * 0.5f);
        float testLateralVal = testPosMod / avoidanceBoxWidth;
        lateralVal = testLateralVal;
    }
    //m_debugData->DebugPushUILineDecimalNumber("lateralVal = ", lateralVal, "");

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    const float testOffest = 14.0f - 10.0;
    float obstacleX = obsticlePos.x - avoidanceCenter.x + frontOffset - testOffest;
    float brakeVal;
    if (obstacleX <= 0.0f)
    {
        /*
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        float testForwardVal = testPosMod / avoidanceBoxLength;
        brakeVal = testForwardVal;
        */
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos + (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }
    else
    {
        //float forwardAvoidPos = obsticlePos.x; // + obstacleRadius;
        float forwardAvoidPos = obstacleX; // + obstacleRadius;
        float testPosMod = forwardAvoidPos - (avoidanceBoxLength * 0.5f);
        //float testForwardVal = testPosMod / avoidanceBoxLength;
        float testForwardVal = testPosMod / (avoidanceBoxLength * 0.5f);
        brakeVal = testForwardVal;
    }

    DirectX::SimpleMath::Vector3 testCenter = avoidanceCenter;
    DirectX::SimpleMath::Vector3 testRear = avoidanceCenter;
    testRear.x -= m_avoidanceBoxLength;
    DirectX::SimpleMath::Vector3 testFront = avoidanceCenter;
    testFront.x += m_avoidanceBoxLength;

    testCenter = DirectX::SimpleMath::Vector3::Transform(testCenter, updateMat);
    testRear = DirectX::SimpleMath::Vector3::Transform(testRear, updateMat);
    testFront = DirectX::SimpleMath::Vector3::Transform(testFront, updateMat);

    DirectX::SimpleMath::Vector3 avoidanceVec = avoidanceCenter - obsticlePos;
    DirectX::SimpleMath::Vector3 avoidanceVec2 = obsticlePos - avoidanceCenter;
    //avoidanceVec.x *= -1.0f;
    avoidanceVec.y = 0.0f;
    avoidanceVec.Normalize();
    //avoidanceVec2.x *= -1.0f;
    avoidanceVec2.y = 0.0f;
    avoidanceVec2.Normalize();

    DirectX::SimpleMath::Vector3 aaTestForward = m_npcOwner->GetForward();
    //avoidanceVec = DirectX::SimpleMath::Vector3::Transform(avoidanceVec, updateMat);
    //avoidanceVec2 = DirectX::SimpleMath::Vector3::Transform(avoidanceVec2, updateMat);
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec, 40.0f, 8.0f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), avoidanceVec2, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    ///////////////////////m_currentDestination = avoidanceVec;



    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    testAvoidanceVec += m_npcOwner->GetPos();



    ///////m_currentDestination = testAvoidanceVec;





    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));



    DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTargetNpc->GetPos();
    DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();

    /*
    targetPos = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    targetVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    selfPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    selfVelocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    */
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();

    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);

    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), vecToTarget, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    //testAvoidanceVec *= 10.0f;


    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            directionToTarget.Normalize();
            DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            avoidVec += m_npcOwner->GetPos();
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }


    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;


    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos()
{
    DirectX::SimpleMath::Vector3 targetPos = m_playerVehicle->GetPos();
    //targetPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 targetVelocity = m_playerVehicle->GetVelocity();

    targetPos = m_avoidanceTargetNpc->GetPos();
    targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    //selfPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();
    //selfVelocity = -DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();

    //DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 localPos = targetPos - selfPos;
    //const float distance = localPos.Length();

    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    //DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    DirectX::SimpleMath::Vector3 vecToDest = selfPos - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    //toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-selfPos);

    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(selfPos, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    //DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal = selfPos;

    //DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    DirectX::SimpleMath::Vector3 obsticlePos = targetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);
    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter.y = 0.0f;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenterWorld = avoidanceCenter;
    DirectX::SimpleMath::Matrix toWorldCord = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix toWorldRotation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    avoidanceCenterWorld = DirectX::SimpleMath::Vector3::Transform(avoidanceCenterWorld, toWorldCord);

    const float distanceBetweenTest = (selfPos - targetPos).Length();
    const float velocityMagTest = (selfVelocity - targetVelocity).Length();
    const float impactTimeTest = distanceBetweenTest / (velocityMagTest + 0.000000001f);
    const float lookAheadTimeTest = impactTimeTest;
    DirectX::SimpleMath::Vector3 avoidPosTest = targetPos + (targetVelocity * lookAheadTimeTest);
    DirectX::SimpleMath::Vector3 lookAheadPosTest = selfPos + (selfVelocity * lookAheadTimeTest);
    //DirectX::SimpleMath::Vector3 vecToTargetTest = lookAheadPosTest - avoidPosTest;
    //vecToTargetTest += m_npcOwner->GetPos();

    //m_debugData->DebugPushTestLinePositionIndicator(avoidPosTest, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLinePositionIndicator(lookAheadPosTest, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    DirectX::SimpleMath::Vector3 impactPosTest = avoidPosTest;

    impactPosTest = targetPos; ////////////////

    impactPosTest = DirectX::SimpleMath::Vector3::Transform(impactPosTest, localMat);
    obsticlePos = impactPosTest;

    DirectX::SimpleMath::Vector3 targetFromCenterNorm = obsticlePos - avoidanceCenter;
    targetFromCenterNorm.Normalize();
    DirectX::SimpleMath::Vector3 testFromCenterAvoidance = targetFromCenterNorm;
    
    const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
    float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
    const float zDistanceFromCenter = obsticlePos.z - avoidanceCenter.z;
    float zDistanceRatio = zDistanceFromCenter / m_avoidanceBox.Extents.z;

    //m_debugData->DebugPushUILineDecimalNumber("testFromCenterAvoidance.z = ", testFromCenterAvoidance.z, "");
    //m_debugData->DebugPushUILineDecimalNumber("testFromCenterAvoidance.x = ", testFromCenterAvoidance.x, "");

    //xDistanceRatio = -1.0f;
    //zDistanceRatio = -0.1f;
    if (xDistanceRatio < -1.0f)
    {
        xDistanceRatio = -1.0f;
    }
    if (xDistanceRatio > 1.0f)
    {
        xDistanceRatio = 1.0f;
    }
  
    if (zDistanceRatio < -1.0f)
    {
        zDistanceRatio = -1.0f;
    }
    if (zDistanceRatio > 1.0f)
    {
        zDistanceRatio = 1.0f;
    }
    
    float xDistanceRatioInverse = xDistanceRatio;
    
    if (xDistanceRatioInverse < 0.0f)
    {
        xDistanceRatioInverse = -1.0f - xDistanceRatioInverse;
    }
    else
    {
        xDistanceRatioInverse = 1.0f - xDistanceRatioInverse;
    }

    float zDistanceRatioInverse = zDistanceRatio;
    
    if (zDistanceRatioInverse < 0.0f)
    {
        zDistanceRatioInverse = -1.0f - zDistanceRatioInverse;
    }
    else
    {
        zDistanceRatioInverse = 1.0f - zDistanceRatioInverse;
    }
    
    float xDistanceRatioMed = xDistanceRatio;
    xDistanceRatioMed += 1.0f;
    xDistanceRatioMed *= 0.5f;
    float zDistanceRatioMed = zDistanceRatio;
    zDistanceRatioMed += 1.0f;
    zDistanceRatioMed *= 0.5f;

    float xDistanceRatioMedInverse = xDistanceRatioInverse;
    xDistanceRatioMedInverse += 1.0f;
    xDistanceRatioMedInverse *= 0.5f;
    float zDistanceRatioMedInverse = zDistanceRatioInverse;
    zDistanceRatioMedInverse += 1.0f;
    zDistanceRatioMedInverse *= 0.5f;

    //m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", xDistanceRatio, "");
    //m_debugData->DebugPushUILineDecimalNumber("zDistanceRatio = ", zDistanceRatio, "");

    float zRotVal1 = 0.0f;
    float xRotVal1 = 0.0f;
    float zRotVal2 = 0.0f;
    float xRotVal2 = 0.0f;
    float zRotVal3 = 0.0f;
    float xRotVal3 = 0.0f;
    float zRotVal4 = 0.0f;
    float xRotVal4 = 0.0f;

    if (obsticlePos.x < avoidanceCenter.x)
    {
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal2 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    }
    if (obsticlePos.x > avoidanceCenter.x)
    {
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal2 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    }

    zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);// *xDistanceRatio;
    zRotVal1 = 0.0f;
    xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    if (obsticlePos.x < avoidanceCenter.x)
    {
        xRotVal1 *= -1.0f;
    }

    xRotVal4 = 0.0f;
    //xRotVal4 = xDistanceRatioMed * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    //zRotVal4 = zDistanceRatio * (Utility::GetPi() * 0.5f) * xDistanceRatio;

    //xRotVal4 = xDistanceRatioMed * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    zRotVal4 = -zDistanceRatio * (Utility::GetPi() * 0.5f) * xDistanceRatio;
    //zRotVal1 = -zDistanceRatio * (Utility::GetPi() * 0.5f);

    if (obsticlePos.x < avoidanceCenter.x)
    {
        //zRotVal4 *= -1.0f;
    }

    float xDistanceRatioMed2 = xDistanceRatioMed;
    xDistanceRatioMed2 = 1.0f - xDistanceRatioMed2;
    float xDistanceRatioMed3 = xDistanceRatioMedInverse;
    xDistanceRatioMed3 = 1.0f - xDistanceRatioMed3;
    xRotVal4 = -xDistanceRatioMed2 * (Utility::GetPi() * 1.0f) * zDistanceRatio;
    zRotVal4 = -zDistanceRatioMed * (Utility::GetPi() * 1.0f);

    zRotVal4 = - zDistanceRatio * (Utility::GetPi() * 0.5f);

    xRotVal4 = -zRotVal4;
    //xRotVal4 *= xDistanceRatioMed3;
    xRotVal4 *= xDistanceRatioInverse;

    zRotVal4 = 0.0f;
    xRotVal4 = 0.0f;

    zRotVal4 = -(1.0f + zDistanceRatio) * (Utility::GetPi() * 0.5f);
    
    zRotVal4 = (1.0f + 0.0) * -(Utility::GetPi() * 0.5f);

    const float avoidanceLimit = 0.9f;
    zRotVal4 = (zDistanceRatio) * -(Utility::GetPi() * avoidanceLimit);
    //zRotVal4 = (1.0f + zDistanceRatio) * -(Utility::GetPi() * 0.5f);
    xRotVal4 = zRotVal4 * xDistanceRatioMed;

    zRotVal4 = 0.0f;
    //xRotVal4 = 0.0f;
    DirectX::SimpleMath::Vector3 testRotAvoid1 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid2 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid3 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid4 = -DirectX::SimpleMath::Vector3::UnitX;
    /*
    zRotVal1 = Utility::WrapAngleTest(zRotVal1);
    zRotVal2 = Utility::WrapAngleTest(zRotVal2);
    zRotVal3 = Utility::WrapAngleTest(zRotVal3);
    zRotVal4 = Utility::WrapAngleTest(zRotVal4);

    xRotVal1 = Utility::WrapAngleTest(xRotVal1);
    xRotVal2 = Utility::WrapAngleTest(xRotVal2);
    xRotVal3 = Utility::WrapAngleTest(xRotVal3);
    xRotVal4 = Utility::WrapAngleTest(xRotVal4);
    */
    /*
    m_debugData->DebugPushUILineDecimalNumber("zRotVal1 = ", Utility::ToDegrees(zRotVal1), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal1 = ", Utility::ToDegrees(xRotVal1), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal2 = ", Utility::ToDegrees(zRotVal2), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal2 = ", Utility::ToDegrees(xRotVal2), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal3 = ", Utility::ToDegrees(zRotVal3), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal3 = ", Utility::ToDegrees(xRotVal3), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal4 = ", Utility::ToDegrees(zRotVal4), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal4 = ", Utility::ToDegrees(xRotVal4), "");
    */

    DirectX::SimpleMath::Matrix zRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal1);
    DirectX::SimpleMath::Matrix xRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, zRot1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, xRot1);

    DirectX::SimpleMath::Matrix zRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal2);
    DirectX::SimpleMath::Matrix xRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, zRot2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, xRot2);

    DirectX::SimpleMath::Matrix zRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal3);
    DirectX::SimpleMath::Matrix xRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, zRot3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, xRot3);

    DirectX::SimpleMath::Matrix zRot4 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal4);
    DirectX::SimpleMath::Matrix xRot4 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal4);
    testRotAvoid4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid4, zRot4);
    testRotAvoid4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid4, xRot4);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld = testFromCenterAvoidance;
    targetFromCenterNormWorld = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld1 = testRotAvoid1;
    testRotAvoidWorld1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld1, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld2 = testRotAvoid2;
    testRotAvoidWorld2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld2, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld3 = testRotAvoid3;
    testRotAvoidWorld3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld3, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld4 = testRotAvoid4;
    testRotAvoidWorld4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld4, toWorldRotation);
  
    /*
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld1, 20.0f, 8.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld2, 20.0f, 10.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld3, 20.0f, 12.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));   
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld4, 20.0f, 14.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    m_debugData->DebugPushTestLine(targetPos, DirectX::SimpleMath::Vector3::UnitY, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    */
    //m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 updatedDest = testRotAvoid4;
    //updatedDest *= 25.0f;
    updatedDest *= 15.0f + (100.0f * (1.0f - xDistanceRatioMed));
    updatedDest = DirectX::SimpleMath::Vector3::Transform(updatedDest, toWorldCord);
    //m_debugData->DebugPushTestLinePositionIndicator(updatedDest, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();
    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //vecToTarget = selfPos - targetPos;
    DirectX::SimpleMath::Vector3 testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    testAvoidanceVec *= 10.0f;

    /*
    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            //DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - targetPos;
            directionToTarget.Normalize();
            //DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget2 = targetVelocity - selfPos;
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            //avoidVec += m_npcOwner->GetPos();
            avoidVec += selfPos;
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }
    */
  
    testAvoidanceVec += m_npcOwner->GetPos();
    m_currentDestination = testAvoidanceVec;
    m_currentDestination = updatedDest;

    //m_debugData->DebugPushTestLinePositionIndicator(m_currentDestination, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPosNewAlt1()
{
    DirectX::SimpleMath::Vector3 targetPos = m_playerVehicle->GetPos();
    //targetPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 targetVelocity = m_playerVehicle->GetVelocity();

    targetPos = m_avoidanceTargetNpc->GetPos();
    targetVelocity = m_avoidanceTargetNpc->GetVelocity();

    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    //selfPos.y = 0.0f;
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();
    //selfVelocity = -DirectX::SimpleMath::Vector3::UnitX;

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();

    //DirectX::SimpleMath::Vector3 localPos = m_avoidanceTargetPos - m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 localPos = targetPos - selfPos;
    //const float distance = localPos.Length();

    DirectX::SimpleMath::Matrix inverse = alignment;
    inverse = inverse.Invert();
    localPos = DirectX::SimpleMath::Vector3::Transform(localPos, inverse);

    //DirectX::SimpleMath::Vector3 vecToDest = m_npcOwner->GetPos() - m_currentDestination;
    DirectX::SimpleMath::Vector3 vecToDest = selfPos - m_currentDestination;
    const float distToDest = vecToDest.Length();
    vecToDest.Normalize();

    DirectX::SimpleMath::Matrix toLocalCordMat = DirectX::SimpleMath::Matrix::Identity;
    toLocalCordMat *= inverse;
    //toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-m_npcOwner->GetPos());
    toLocalCordMat *= DirectX::SimpleMath::Matrix::CreateTranslation(-selfPos);

    //DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix updateMat = DirectX::SimpleMath::Matrix::CreateWorld(selfPos, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localMat = updateMat;
    localMat = localMat.Invert();

    //DirectX::SimpleMath::Vector3 testLocal = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 testLocal = selfPos;

    //DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTargetPos;
    DirectX::SimpleMath::Vector3 obsticlePos = targetPos;
    obsticlePos = DirectX::SimpleMath::Vector3::Transform(obsticlePos, localMat);
    DirectX::SimpleMath::Vector3 avoidanceCenter = m_avoidanceBox.Center;
    avoidanceCenter.y = 0.0f;
    avoidanceCenter = DirectX::SimpleMath::Vector3::Transform(avoidanceCenter, localMat);

    DirectX::SimpleMath::Vector3 avoidanceCenterWorld = avoidanceCenter;
    DirectX::SimpleMath::Matrix toWorldCord = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix toWorldRotation = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    avoidanceCenterWorld = DirectX::SimpleMath::Vector3::Transform(avoidanceCenterWorld, toWorldCord);

    const float distanceBetweenTest = (selfPos - targetPos).Length();
    const float velocityMagTest = (selfVelocity - targetVelocity).Length();
    const float impactTimeTest = distanceBetweenTest / (velocityMagTest + 0.000000001f);
    const float lookAheadTimeTest = impactTimeTest;
    DirectX::SimpleMath::Vector3 avoidPosTest = targetPos + (targetVelocity * lookAheadTimeTest);
    DirectX::SimpleMath::Vector3 lookAheadPosTest = selfPos + (selfVelocity * lookAheadTimeTest);
    //DirectX::SimpleMath::Vector3 vecToTargetTest = lookAheadPosTest - avoidPosTest;
    //vecToTargetTest += m_npcOwner->GetPos();

    //m_debugData->DebugPushTestLinePositionIndicator(avoidPosTest, 7.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLinePositionIndicator(lookAheadPosTest, 3.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    DirectX::SimpleMath::Vector3 impactPosTest = avoidPosTest;

    //impactPosTest = targetPos; ////////////////

    impactPosTest = DirectX::SimpleMath::Vector3::Transform(impactPosTest, localMat);
    obsticlePos = impactPosTest;

    DirectX::SimpleMath::Vector3 targetFromCenterNorm = obsticlePos - avoidanceCenter;
    targetFromCenterNorm.Normalize();
    DirectX::SimpleMath::Vector3 testFromCenterAvoidance = targetFromCenterNorm;

    const float xDistanceFromCenter = obsticlePos.x - avoidanceCenter.x;
    float xDistanceRatio = xDistanceFromCenter / m_avoidanceBox.Extents.x;
    const float zDistanceFromCenter = obsticlePos.z - avoidanceCenter.z;
    float zDistanceRatio = zDistanceFromCenter / m_avoidanceBox.Extents.z;

    //m_debugData->DebugPushUILineDecimalNumber("testFromCenterAvoidance.z = ", testFromCenterAvoidance.z, "");
    //m_debugData->DebugPushUILineDecimalNumber("testFromCenterAvoidance.x = ", testFromCenterAvoidance.x, "");

    //xDistanceRatio = -1.0f;
    //zDistanceRatio = -0.1f;
    if (xDistanceRatio < -1.0f)
    {
        xDistanceRatio = -1.0f;
    }
    if (xDistanceRatio > 1.0f)
    {
        xDistanceRatio = 1.0f;
    }

    if (zDistanceRatio < -1.0f)
    {
        zDistanceRatio = -1.0f;
    }
    if (zDistanceRatio > 1.0f)
    {
        zDistanceRatio = 1.0f;
    }

    float xDistanceRatioInverse = xDistanceRatio;

    if (xDistanceRatioInverse < 0.0f)
    {
        xDistanceRatioInverse = -1.0f - xDistanceRatioInverse;
    }
    else
    {
        xDistanceRatioInverse = 1.0f - xDistanceRatioInverse;
    }

    float zDistanceRatioInverse = zDistanceRatio;

    if (zDistanceRatioInverse < 0.0f)
    {
        zDistanceRatioInverse = -1.0f - zDistanceRatioInverse;
    }
    else
    {
        zDistanceRatioInverse = 1.0f - zDistanceRatioInverse;
    }

    float xDistanceRatioMed = xDistanceRatio;
    xDistanceRatioMed += 1.0f;
    xDistanceRatioMed *= 0.5f;
    float zDistanceRatioMed = zDistanceRatio;
    zDistanceRatioMed += 1.0f;
    zDistanceRatioMed *= 0.5f;

    float xDistanceRatioMedInverse = xDistanceRatioInverse;
    xDistanceRatioMedInverse += 1.0f;
    xDistanceRatioMedInverse *= 0.5f;
    float zDistanceRatioMedInverse = zDistanceRatioInverse;
    zDistanceRatioMedInverse += 1.0f;
    zDistanceRatioMedInverse *= 0.5f;

    //m_debugData->DebugPushUILineDecimalNumber("xDistanceRatio = ", xDistanceRatio, "");
    //m_debugData->DebugPushUILineDecimalNumber("zDistanceRatio = ", zDistanceRatio, "");

    float zRotVal1 = 0.0f;
    float xRotVal1 = 0.0f;
    float zRotVal2 = 0.0f;
    float xRotVal2 = 0.0f;
    float zRotVal3 = 0.0f;
    float xRotVal3 = 0.0f;
    float zRotVal4 = 0.0f;
    float xRotVal4 = 0.0f;

    if (obsticlePos.x < avoidanceCenter.x)
    {
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal2 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    }
    if (obsticlePos.x > avoidanceCenter.x)
    {
        zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal2 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal2 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;

        zRotVal3 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);
        xRotVal3 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    }

    zRotVal1 = -testFromCenterAvoidance.z * (Utility::GetPi() * 0.5f);// *xDistanceRatio;
    zRotVal1 = 0.0f;
    xRotVal1 = -testFromCenterAvoidance.x * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    if (obsticlePos.x < avoidanceCenter.x)
    {
        xRotVal1 *= -1.0f;
    }

    xRotVal4 = 0.0f;
    //xRotVal4 = xDistanceRatioMed * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    //zRotVal4 = zDistanceRatio * (Utility::GetPi() * 0.5f) * xDistanceRatio;

    //xRotVal4 = xDistanceRatioMed * (Utility::GetPi() * 0.5f) * zDistanceRatio;
    zRotVal4 = -zDistanceRatio * (Utility::GetPi() * 0.5f) * xDistanceRatio;
    //zRotVal1 = -zDistanceRatio * (Utility::GetPi() * 0.5f);

    if (obsticlePos.x < avoidanceCenter.x)
    {
        //zRotVal4 *= -1.0f;
    }

    float xDistanceRatioMed2 = xDistanceRatioMed;
    xDistanceRatioMed2 = 1.0f - xDistanceRatioMed2;
    float xDistanceRatioMed3 = xDistanceRatioMedInverse;
    xDistanceRatioMed3 = 1.0f - xDistanceRatioMed3;
    xRotVal4 = -xDistanceRatioMed2 * (Utility::GetPi() * 1.0f) * zDistanceRatio;
    zRotVal4 = -zDistanceRatioMed * (Utility::GetPi() * 1.0f);

    zRotVal4 = -zDistanceRatio * (Utility::GetPi() * 0.5f);

    xRotVal4 = -zRotVal4;
    //xRotVal4 *= xDistanceRatioMed3;
    xRotVal4 *= xDistanceRatioInverse;

    zRotVal4 = 0.0f;
    xRotVal4 = 0.0f;

    zRotVal4 = -(1.0f + zDistanceRatio) * (Utility::GetPi() * 0.5f);

    zRotVal4 = (1.0f + 0.0) * -(Utility::GetPi() * 0.5f);

    const float avoidanceLimit = 0.9f;
    zRotVal4 = (zDistanceRatio) * -(Utility::GetPi() * avoidanceLimit);
    //zRotVal4 = (1.0f + zDistanceRatio) * -(Utility::GetPi() * 0.5f);
    xRotVal4 = zRotVal4 * xDistanceRatioMed;

    zRotVal4 = 0.0f;
    //xRotVal4 = 0.0f;
    DirectX::SimpleMath::Vector3 testRotAvoid1 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid2 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid3 = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 testRotAvoid4 = -DirectX::SimpleMath::Vector3::UnitX;
    /*
    zRotVal1 = Utility::WrapAngleTest(zRotVal1);
    zRotVal2 = Utility::WrapAngleTest(zRotVal2);
    zRotVal3 = Utility::WrapAngleTest(zRotVal3);
    zRotVal4 = Utility::WrapAngleTest(zRotVal4);

    xRotVal1 = Utility::WrapAngleTest(xRotVal1);
    xRotVal2 = Utility::WrapAngleTest(xRotVal2);
    xRotVal3 = Utility::WrapAngleTest(xRotVal3);
    xRotVal4 = Utility::WrapAngleTest(xRotVal4);
    */
    /*
    m_debugData->DebugPushUILineDecimalNumber("zRotVal1 = ", Utility::ToDegrees(zRotVal1), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal1 = ", Utility::ToDegrees(xRotVal1), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal2 = ", Utility::ToDegrees(zRotVal2), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal2 = ", Utility::ToDegrees(xRotVal2), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal3 = ", Utility::ToDegrees(zRotVal3), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal3 = ", Utility::ToDegrees(xRotVal3), "");
    m_debugData->DebugPushUILineDecimalNumber("zRotVal4 = ", Utility::ToDegrees(zRotVal4), "");
    m_debugData->DebugPushUILineDecimalNumber("xRotVal4 = ", Utility::ToDegrees(xRotVal4), "");
    */

    DirectX::SimpleMath::Matrix zRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal1);
    DirectX::SimpleMath::Matrix xRot1 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, zRot1);
    testRotAvoid1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid1, xRot1);


    DirectX::SimpleMath::Matrix zRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal2);
    DirectX::SimpleMath::Matrix xRot2 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, zRot2);
    testRotAvoid2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid2, xRot2);

    DirectX::SimpleMath::Matrix zRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal3);
    DirectX::SimpleMath::Matrix xRot3 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, zRot3);
    testRotAvoid3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid3, xRot3);

    DirectX::SimpleMath::Matrix zRot4 = DirectX::SimpleMath::Matrix::CreateRotationY(zRotVal4);
    DirectX::SimpleMath::Matrix xRot4 = DirectX::SimpleMath::Matrix::CreateRotationY(xRotVal4);
    testRotAvoid4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid4, zRot4);
    testRotAvoid4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoid4, xRot4);

    DirectX::SimpleMath::Vector3 targetFromCenterNormWorld = testFromCenterAvoidance;
    targetFromCenterNormWorld = DirectX::SimpleMath::Vector3::Transform(targetFromCenterNormWorld, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld1 = testRotAvoid1;
    testRotAvoidWorld1 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld1, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld2 = testRotAvoid2;
    testRotAvoidWorld2 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld2, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld3 = testRotAvoid3;
    testRotAvoidWorld3 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld3, toWorldRotation);

    DirectX::SimpleMath::Vector3 testRotAvoidWorld4 = testRotAvoid4;
    testRotAvoidWorld4 = DirectX::SimpleMath::Vector3::Transform(testRotAvoidWorld4, toWorldRotation);

    /*
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld1, 20.0f, 8.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld2, 20.0f, 10.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld3, 20.0f, 12.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    m_debugData->DebugPushTestLine(avoidanceCenterWorld, testRotAvoidWorld4, 20.0f, 14.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    m_debugData->DebugPushTestLine(targetPos, DirectX::SimpleMath::Vector3::UnitY, 20.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    */

    //m_debugData->DebugClearUI();
    DirectX::SimpleMath::Vector3 updatedDest = testRotAvoid4;
    //updatedDest *= 25.0f;
    updatedDest *= 25.0f + (100.0f * (1.0f - xDistanceRatioMed));
    updatedDest = DirectX::SimpleMath::Vector3::Transform(updatedDest, toWorldCord);
    //m_debugData->DebugPushTestLinePositionIndicator(updatedDest, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    /*
    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();
    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    //avoidPos = m_avoidanceTarget;
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    //lookAheadPos = m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    //vecToTarget = selfPos - targetPos;
    DirectX::SimpleMath::Vector3 testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;
    testAvoidanceVec *= 10.0f;
    */

    /*
    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
        if (m_avoidanceTargetNpc->GetIsJumpActive() == true)
        {
            DirectX::SimpleMath::Vector3 testSelfVelocity = m_npcOwner->GetVelocity();
            testSelfVelocity.Normalize();
            testSelfVelocity *= -1.0f;
            //DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - m_avoidanceTargetNpc->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget = m_npcOwner->GetVelocity() - targetPos;
            directionToTarget.Normalize();
            //DirectX::SimpleMath::Vector3 directionToTarget2 = m_avoidanceTargetNpc->GetVelocity() - m_npcOwner->GetPos();
            DirectX::SimpleMath::Vector3 directionToTarget2 = targetVelocity - selfPos;
            directionToTarget2.Normalize();
            //DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity + directionToTarget;
            DirectX::SimpleMath::Vector3 avoidVec = testSelfVelocity;
            avoidVec *= 130.0f;
            //avoidVec += m_npcOwner->GetPos();
            avoidVec += selfPos;
            m_currentDestination = avoidVec;
            //testAvoidanceVec *= testRatio;

            m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos() + DirectX::SimpleMath::Vector3(0.0f, 10.f, 0.0f), m_avoidanceTargetNpc->GetPos(), DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        else
        {
            m_isJumpTriggered = true;
        }
    }
    else
    {
        testAvoidanceVec += m_npcOwner->GetPos();
        m_currentDestination = testAvoidanceVec;
    }
    */
    //testAvoidanceVec += m_npcOwner->GetPos();
    //m_currentDestination = testAvoidanceVec;
    m_currentDestination = updatedDest;

    //m_debugData->DebugPushTestLinePositionIndicator(m_currentDestination, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::CreateWayPath()
{
    Utility::ClearWayPath(m_currentWayPath);
    m_currentWayPath.isPathLooped = true;
    const float radius = 75.0f;

    DirectX::SimpleMath::Vector3 pos;
    Utility::Waypoint wp;

    const float low = -10.0f;
    const float high = 10.0f;
    float xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    float zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    Utility::Waypoint wp1;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp1 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp2;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp2 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp3;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 100.0f + zOffset);
    wp3 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp4;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -100.0f + zOffset);
    wp4 = Utility::CreateWaypoint(pos, radius + 15.0f);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp5;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp5 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp6;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, -300.0f + zOffset);
    wp6 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp7;
    pos = DirectX::SimpleMath::Vector3(300.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp7 = Utility::CreateWaypoint(pos, radius);
    xOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
    zOffset = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

    Utility::Waypoint wp8;
    pos = DirectX::SimpleMath::Vector3(75.0f + xOffset, 3.0f, 300.0f + zOffset);
    wp8 = Utility::CreateWaypoint(pos, radius);


    //Utility::PushWaypointToPath(m_currentWayPath, wp7);
    //Utility::PushWaypointToPath(m_currentWayPath, wp2);
    /*
    Utility::PushWaypointToPath(m_currentWayPath, wp4);
    Utility::PushWaypointToPath(m_currentWayPath, wp5);
    Utility::PushWaypointToPath(m_currentWayPath, wp6);
    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp3);
    Utility::PushWaypointToPath(m_currentWayPath, wp8);
    Utility::PushWaypointToPath(m_currentWayPath, wp7);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);
    */
    
    /*
    if (m_npcOwner->GetID() % 2)
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
        Utility::PushWaypointToPath(m_currentWayPath, wp5);
        Utility::PushWaypointToPath(m_currentWayPath, wp6);
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp8);
        Utility::PushWaypointToPath(m_currentWayPath, wp7);
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
    }
    else
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
        Utility::PushWaypointToPath(m_currentWayPath, wp7);
        Utility::PushWaypointToPath(m_currentWayPath, wp8);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
        Utility::PushWaypointToPath(m_currentWayPath, wp6);
        Utility::PushWaypointToPath(m_currentWayPath, wp5);
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
    }
    */

    Utility::PushWaypointToPath(m_currentWayPath, wp4);
    Utility::PushWaypointToPath(m_currentWayPath, wp5);
    Utility::PushWaypointToPath(m_currentWayPath, wp6);
    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp3);
    Utility::PushWaypointToPath(m_currentWayPath, wp8);
    Utility::PushWaypointToPath(m_currentWayPath, wp7);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);

    m_currentWayPath.targetNode = 0;
}

void NpcAI::DebugToggle()
{
    if (m_debugToggle == true)
    {
        m_debugToggle = false;
    }
    else
    {
        m_debugToggle = true;
    }
}

float NpcAI::GetAngleToDestination(DirectX::SimpleMath::Vector3 aForward, DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3 aUp, DirectX::SimpleMath::Vector3 aDest)
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination - m_npcOwner->GetPos();
    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = m_npcOwner->GetForward();
    const DirectX::SimpleMath::Vector3 two = dest;
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    const float det = one.x * two.y * aUp.z + two.x * aUp.y * one.z + aUp.x * one.y * two.z - one.z * two.y * aUp.x - two.z * aUp.y * one.x - aUp.z * one.y * two.x;
    const float angle = atan2(det, dot);

    return angle;
}

DirectX::SimpleMath::Vector3 NpcAI::GetVecToDestination()
{
    DirectX::SimpleMath::Vector3 vecToDestination = m_currentDestination - m_npcOwner->GetPos();
    vecToDestination.Normalize();
    return vecToDestination;
    //return m_desiredHeading;
}

float NpcAI::GetThrottleInput()
{
    DirectX::SimpleMath::Vector3 pos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 directionToDest = m_currentDestination - pos;
    DirectX::SimpleMath::Vector3 directionNorm = directionToDest;
    directionNorm.Normalize();

    DirectX::SimpleMath::Vector3 crossProd = directionNorm.Cross(m_npcOwner->GetRight());
    bool isFacingDest;
    if (directionNorm.Cross(m_npcOwner->GetRight()).y >= 0.0f)
    {
        isFacingDest = false;
    }
    else
    {
        isFacingDest = true;
    }

    if (m_behavior.none == true)
    {
        //return 0.0f;
    }
    else if (isFacingDest == false)
    {
        //return 0.0f;
    }
    else
    {
        //return 1.0f;
    }
    return m_aiControls.aiOutput.forwardThrust;
}

void NpcAI::InitializeAI(Environment const* aEnvironment, Vehicle const* aPlayer, std::shared_ptr<DebugData> aDebugPtr, std::shared_ptr<NPCController> aNpcController)
{
    m_debugData = aDebugPtr;
    m_environment = aEnvironment;
    m_playerVehicle = aPlayer;
    m_npcController = aNpcController;

    InitializeControlOutput();
    InitializeBehavior();
    InitializeDestinationTargets();
    m_currentWaypoint.waypointPos = DirectX::SimpleMath::Vector3::Zero;
    m_currentWaypoint.waypointRadius = 1.0f;

    m_desiredHeading = DirectX::SimpleMath::Vector3::Zero;
    m_desiredVelocity = 0.0f;

    Utility::ClearWayPath(m_currentWayPath);

    CreateWayPath();
    m_currentDestination = DirectX::SimpleMath::Vector3::Zero;

    m_destinationSmoothing.clear();
    for (int i = 0; i < m_destinationSmoothingSize; ++i)
    {
        m_destinationSmoothing.push_back(m_currentDestination);
    }

    m_avoidanceTargetPos = DirectX::SimpleMath::Vector3::Zero;
    m_isAvoidanceTrue = false;
    UpdateAvoidanceBox();
}

void NpcAI::InitializeControlOutput()
{
    m_aiControls.aiOutput.angleToDestination = 0.0f;
    m_aiControls.aiOutput.steeringAngle = 0.0f;
    m_aiControls.aiOutput.steeringDirection = DirectX::SimpleMath::Vector3::Zero;
    m_aiControls.aiOutput.forwardThrust = 0.0f;
    m_aiControls.aiOutput.omniThrust = 0.0f;
    m_aiControls.aiOutput.omniDirection = DirectX::SimpleMath::Vector3::Zero;
}

void NpcAI::InitializeBehavior()
{
    m_behavior.arrive = false;
    m_behavior.followWayPath = false;
    m_behavior.none = false;
    m_behavior.seek = false;
    m_behavior.separation = false;
    m_behavior.stop = true;
    m_behavior.wallAvoidance = false;
    m_behavior.wander = false;
}

void NpcAI::InitializeDestinationTargets()
{
    m_destinationTargets.separationTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.currentTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.seekTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.wanderTarget = DirectX::SimpleMath::Vector3::Zero;
    m_destinationTargets.separationMagnitude = 15.0f;
    m_destinationTargets.separationRadius = 15.0f;
    m_destinationTargets.wanderDistance = 100.0f;
    m_destinationTargets.wanderJitter = 0.001f;
    m_destinationTargets.wanderRadius = 10.0f;
}

void NpcAI::PushAiAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    bool isPushTarget = true;
    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVelocity = m_npcOwner->GetVelocity();
    //DirectX::SimpleMath::Vector3 selfWorldVelocity = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 targetWorldPos = aVehicle->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = aVehicle->GetVelocity();
    //DirectX::SimpleMath::Vector3 targetWorldVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 1.0f);
    
    //DirectX::SimpleMath::Matrix localizeMat = m_avoidanceAlignment;
    //DirectX::SimpleMath::Matrix localizeMat = m_npcOwner->GetAlignment();
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_avoidanceBoxBase, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeMat = localizeMat.Invert();

    //DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeDirMat = localizeDirMat.Invert();

    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVelocity = DirectX::SimpleMath::Vector3::Transform(selfWorldVelocity, localizeDirMat);
    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeDirMat);

    float testDot1 = selfWorldVelocity.Dot(targetWorldVelocity);
    float testDot2 = targetWorldVelocity.Dot(selfWorldVelocity);
    float testDot3 = selfLocalVelocity.Dot(targetLocalVelocity);
    float testDot4 = targetLocalVelocity.Dot(selfLocalVelocity);

    DirectX::SimpleMath::Vector3 testSelfLocalVelocityNorm = selfLocalVelocity;
    testSelfLocalVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 testTargetLocalVelocityNorm = targetLocalVelocity;
    testTargetLocalVelocityNorm.Normalize();
    float testDot5 = testSelfLocalVelocityNorm.Dot(testTargetLocalVelocityNorm);
    float testDot6 = testTargetLocalVelocityNorm.Dot(testSelfLocalVelocityNorm);

    float testAngle = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testTargetLocalVelocityNorm, testSelfLocalVelocityNorm));

    DirectX::SimpleMath::Vector3 approchVelocity = selfLocalVelocity - targetLocalVelocity;

    bool isCurrentTargetBlue = false;
    bool isPrevTargetBlue = m_isAvoidanceTrueTest1;
    if (targetLocalVelocity.x >= selfLocalVelocity.x)
    {
        m_isAvoidanceTrueTest2 = true;
        isPushTarget = false;
    }
    if (testDot5 < 0.0f && targetLocalVelocity.x <= selfLocalVelocity.x)
    {
        isCurrentTargetBlue = true;
        m_isAvoidanceTrueTest1 = true;
        isPushTarget = true;
    }

    if (isCurrentTargetBlue == true && isPushTarget == false)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (isPushTarget == true)
    {
        if (m_isAvoidanceTrue == false)
        {
            m_isAvoidanceTrue = true;
            m_avoidanceTargetPos = aAvoidancePos;
            m_avoidanceTargetNpc = aVehicle;
        }
        else
        {
            const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTargetPos).Length();
            const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();

            if (isCurrentTargetBlue == false && isPrevTargetBlue == true)
            {
                int testBreak = 0;
                testBreak++;
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == true)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == false)
            {
                m_avoidanceTargetPos = aAvoidancePos;
                m_avoidanceTargetNpc = aVehicle;
            }
            else if (distanceToNewTarget < distanceToPriorTarget)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (distanceToNewTarget > distanceToPriorTarget)
            {

            }
            else
            {
                int testBreak = 0;
                testBreak++;
            }
        }
    }

}

void NpcAI::PushAiAvoidanceTarget2(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    DirectX::SimpleMath::Vector3 selfPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfVelocity = m_npcOwner->GetVelocity();
    //selfVelocity = -DirectX::SimpleMath::Vector3::UnitZ;
    DirectX::SimpleMath::Vector3 targetPos = aVehicle->GetPos();
    DirectX::SimpleMath::Vector3 targetVelocity = aVehicle->GetVelocity();
    //targetVelocity = DirectX::SimpleMath::Vector3::UnitZ;


    const float distanceBetween = (selfPos - targetPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfVelocity - targetVelocity;
    const float velocityMag = (selfVelocity - targetVelocity).Length();
    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
    const float lookAheadTime = impactTime;

    DirectX::SimpleMath::Vector3 maxSensorPos;
    //DirectX::BoundingBox selfAvoidanceBox = m_npcOwner->GetAvoidanceBox();
    DirectX::BoundingOrientedBox selfAvoidanceBox = m_npcOwner->GetAvoidanceBox();

    float maxSensorRange = DirectX::SimpleMath::Vector3::Distance(selfPos, selfAvoidanceBox.Center) + selfAvoidanceBox.Extents.x;
    maxSensorRange = 100.0f;
    DirectX::SimpleMath::Vector3 avoidPos = targetPos + (targetVelocity * lookAheadTime);
    DirectX::SimpleMath::Vector3 lookAheadPos = selfPos + (selfVelocity * lookAheadTime);
    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;
    float distanceToImpact = DirectX::SimpleMath::Vector3::Distance(selfPos, vecToTarget);
    bool addToAvoidance = false;
    if (distanceToImpact <= maxSensorRange)
    {
        addToAvoidance = true;
    }

    if (m_isAvoidanceTrue == false)
    {
        m_isAvoidanceTrue = true;
        m_avoidanceTargetPos = aAvoidancePos;
        m_avoidanceTargetNpc = aVehicle;
    }
    else
    {
        const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTargetPos).Length();
        const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();
        if (distanceToNewTarget < distanceToPriorTarget)
        {
            m_avoidanceTargetPos = aAvoidancePos;
            m_avoidanceTargetNpc = aVehicle;
        }
    }
}

void NpcAI::PushAiAvoidanceTarget3(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    bool isPushTarget = true;
    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVelocity = m_npcOwner->GetVelocity();
    //DirectX::SimpleMath::Vector3 selfWorldVelocity = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 targetWorldPos = aVehicle->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = aVehicle->GetVelocity();
    //DirectX::SimpleMath::Vector3 targetWorldVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 1.0f);

    //DirectX::SimpleMath::Matrix localizeMat = m_avoidanceAlignment;
    //DirectX::SimpleMath::Matrix localizeMat = m_npcOwner->GetAlignment();
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_avoidanceBoxBase, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeMat = localizeMat.Invert();

    //DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeDirMat = localizeDirMat.Invert();

    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVelocity = DirectX::SimpleMath::Vector3::Transform(selfWorldVelocity, localizeDirMat);
    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeDirMat);

    float testDot1 = selfWorldVelocity.Dot(targetWorldVelocity);
    float testDot2 = targetWorldVelocity.Dot(selfWorldVelocity);
    float testDot3 = selfLocalVelocity.Dot(targetLocalVelocity);
    float testDot4 = targetLocalVelocity.Dot(selfLocalVelocity);

    DirectX::SimpleMath::Vector3 testSelfLocalVelocityNorm = selfLocalVelocity;
    testSelfLocalVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 testTargetLocalVelocityNorm = targetLocalVelocity;
    testTargetLocalVelocityNorm.Normalize();
    float testDot5 = testSelfLocalVelocityNorm.Dot(testTargetLocalVelocityNorm);
    float testDot6 = testTargetLocalVelocityNorm.Dot(testSelfLocalVelocityNorm);

    float testAngle = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testTargetLocalVelocityNorm, testSelfLocalVelocityNorm));

    DirectX::SimpleMath::Vector3 approchVelocity = selfLocalVelocity - targetLocalVelocity;

    bool isCurrentTargetBlue = false;
    bool isPrevTargetBlue = m_isAvoidanceTrueTest1;
    if (targetLocalVelocity.x >= selfLocalVelocity.x)
    {
        m_isAvoidanceTrueTest2 = true;
        isPushTarget = false;
    }
    if (testDot5 < 0.0f && targetLocalVelocity.x <= selfLocalVelocity.x)
    {
        isCurrentTargetBlue = true;
        m_isAvoidanceTrueTest1 = true;
        isPushTarget = true;
    }

    if (isCurrentTargetBlue == true && isPushTarget == false)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (isPushTarget == true)
    {
        if (m_isAvoidanceTrue == false)
        {
            m_isAvoidanceTrue = true;
            m_avoidanceTargetPos = aAvoidancePos;
            m_avoidanceTargetNpc = aVehicle;
        }
        else
        {
            const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTargetPos).Length();
            const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();

            if (isCurrentTargetBlue == false && isPrevTargetBlue == true)
            {
                int testBreak = 0;
                testBreak++;
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == true)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == false)
            {
                m_avoidanceTargetPos = aAvoidancePos;
                m_avoidanceTargetNpc = aVehicle;
            }
            else if (distanceToNewTarget < distanceToPriorTarget)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (distanceToNewTarget > distanceToPriorTarget)
            {

            }
            else
            {
                int testBreak = 0;
                testBreak++;
            }
        }
    }

}

void NpcAI::PushAiAvoidanceTarget4(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    bool isPushTarget = true;
    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVelocity = m_npcOwner->GetVelocity();
    //DirectX::SimpleMath::Vector3 selfWorldVelocity = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 targetWorldPos = aVehicle->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = aVehicle->GetVelocity();
    //DirectX::SimpleMath::Vector3 targetWorldVelocity = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 1.0f);

    //DirectX::SimpleMath::Matrix localizeMat = m_avoidanceAlignment;
    //DirectX::SimpleMath::Matrix localizeMat = m_npcOwner->GetAlignment();
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    //DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeMat = DirectX::SimpleMath::Matrix::CreateWorld(m_avoidanceBoxBase, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeMat = localizeMat.Invert();

    //DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Matrix localizeDirMat = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_avoidanceBoxRight, m_avoidaneBoxUp);
    localizeDirMat = localizeDirMat.Invert();

    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVelocity = DirectX::SimpleMath::Vector3::Transform(selfWorldVelocity, localizeDirMat);
    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeDirMat);

    float testDot1 = selfWorldVelocity.Dot(targetWorldVelocity);
    float testDot2 = targetWorldVelocity.Dot(selfWorldVelocity);
    float testDot3 = selfLocalVelocity.Dot(targetLocalVelocity);
    float testDot4 = targetLocalVelocity.Dot(selfLocalVelocity);

    DirectX::SimpleMath::Vector3 testSelfLocalVelocityNorm = selfLocalVelocity;
    testSelfLocalVelocityNorm.Normalize();
    DirectX::SimpleMath::Vector3 testTargetLocalVelocityNorm = targetLocalVelocity;
    testTargetLocalVelocityNorm.Normalize();
    float testDot5 = testSelfLocalVelocityNorm.Dot(testTargetLocalVelocityNorm);
    float testDot6 = testTargetLocalVelocityNorm.Dot(testSelfLocalVelocityNorm);

    float testAngle = Utility::ToDegrees(Utility::GetAngleBetweenVectors(testTargetLocalVelocityNorm, testSelfLocalVelocityNorm));

    DirectX::SimpleMath::Vector3 approchVelocity = selfLocalVelocity - targetLocalVelocity;

    bool isCurrentTargetBlue = false;
    bool isPrevTargetBlue = m_isAvoidanceTrueTest1;
    if (targetLocalVelocity.x >= selfLocalVelocity.x)
    {
        m_isAvoidanceTrueTest2 = true;
        isPushTarget = false;
    }
    if (testDot5 < 0.0f && targetLocalVelocity.x <= selfLocalVelocity.x)
    {
        isCurrentTargetBlue = true;
        m_isAvoidanceTrueTest1 = true;
        isPushTarget = true;
    }

    if (isCurrentTargetBlue == true && isPushTarget == false)
    {
        int testBreak = 0;
        testBreak++;
    }

    if (isPushTarget == true)
    {
        if (m_isAvoidanceTrue == false)
        {
            m_isAvoidanceTrue = true;
            m_avoidanceTargetPos = aAvoidancePos;
            m_avoidanceTargetNpc = aVehicle;
        }
        else
        {
            const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTargetPos).Length();
            const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();

            if (isCurrentTargetBlue == false && isPrevTargetBlue == true)
            {
                int testBreak = 0;
                testBreak++;
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == true)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (isCurrentTargetBlue == true && isPrevTargetBlue == false)
            {
                m_avoidanceTargetPos = aAvoidancePos;
                m_avoidanceTargetNpc = aVehicle;
            }
            else if (distanceToNewTarget < distanceToPriorTarget)
            {
                if (distanceToNewTarget < distanceToPriorTarget)
                {
                    m_avoidanceTargetPos = aAvoidancePos;
                    m_avoidanceTargetNpc = aVehicle;
                }
            }
            else if (distanceToNewTarget > distanceToPriorTarget)
            {

            }
            else
            {
                int testBreak = 0;
                testBreak++;
            }
        }
    }

}

void NpcAI::SetForwardThrustOutput()
{
    DirectX::SimpleMath::Vector3 directionToDest = m_currentDestination - m_npcOwner->GetPos();
    directionToDest.Normalize();
    bool isFacingDest;
    if (directionToDest.Cross(m_npcOwner->GetRight()).y >= 0.0f)
    {
        isFacingDest = false;
    }
    else
    {
        isFacingDest = true;
    }
    float forwardThrustVal;
    if (m_behavior.none == true)
    {
        forwardThrustVal = 0.0f;
    }
    else if (isFacingDest == true)
    {
        if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
        {
            forwardThrustVal = directionToDest.Dot(m_npcOwner->GetForward());
            forwardThrustVal *= m_aiControls.throttleOutputMax;
        }
        else
        {
            forwardThrustVal = directionToDest.Dot(m_npcOwner->GetForward());
            forwardThrustVal *= m_aiControls.throttleOutputMax;
        }
    }
    else
    {
        if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
        {
            //forwardThrustVal = -directionToDest.Dot(-m_npcOwner->GetForward());
            //forwardThrustVal *= -m_aiControls.throttleOutputMin;
            forwardThrustVal = m_aiControls.throttleOutputMin;
        }
        else
        {
            forwardThrustVal = -directionToDest.Dot(-m_npcOwner->GetForward());
            forwardThrustVal *= -m_aiControls.throttleOutputMin;
        }
    }
    //m_debugData->DebugPushUILineDecimalNumber("forwardThrustVal = ", forwardThrustVal, "");
    m_aiControls.aiOutput.forwardThrust = forwardThrustVal;
}

void NpcAI::SetOmniOutput()
{
    DirectX::SimpleMath::Vector3 omniThrustDirection = m_npcOwner->GetForward();
    omniThrustDirection = DirectX::SimpleMath::Vector3::Transform(omniThrustDirection, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_npcOwner->GetUp(), m_aiControls.aiOutput.angleToDestination));
    omniThrustDirection.Normalize();
    m_aiControls.aiOutput.omniDirection = omniThrustDirection;

    // Temp throttle settings till AI is built out to control thrust based on distance to destination
    const float distanceToDest = (m_npcOwner->GetPos() - m_currentDestination).Length();

    const float slowDownRange = 50.0f;
    if (distanceToDest > slowDownRange)
    {
        m_aiControls.aiOutput.omniThrust = 1.0f;
    }
    else
    {
        m_aiControls.aiOutput.omniThrust = distanceToDest / slowDownRange;
    }
}

void NpcAI::SetSteeringOutput()
{
    DirectX::SimpleMath::Vector3 dest = m_currentDestination - m_npcOwner->GetPos();
    dest.Normalize();
    const DirectX::SimpleMath::Vector3 one = m_npcOwner->GetForward();
    const DirectX::SimpleMath::Vector3 two = dest;
    const DirectX::SimpleMath::Vector3 up = m_npcOwner->GetUp();
    //const DirectX::SimpleMath::Vector3 one = DirectX::SimpleMath::Vector3::UnitX;
    //const DirectX::SimpleMath::Vector3 two = -DirectX::SimpleMath::Vector3::UnitX;
    //const DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
    const float dot = one.x * two.x + one.y * two.y + one.z * two.z;
    //det =        x1 * y2    * zn    + x2    * yn    * z1    +   xn  * y1    * z2    - z1    * y2    * xn    - z2    * yn    * x1    - zn    * y1    * x2
    //const float det = one.x * two.y * aUp.z + two.x * aUp.y * one.z + aUp.x * one.y * two.z - one.z * two.y * aUp.x - two.z * aUp.y * one.x - aUp.z * one.y * two.x;
    const float det = one.x * two.y * up.z + two.x * up.y * one.z + up.x * one.y * two.z - one.z * two.y * up.x - two.z * up.y * one.x - up.z * one.y * two.x;
    const float angle = atan2(det, dot);
    float angleDegrees = Utility::ToDegrees(angle);

    float steeringAngle;
    if (angle > m_npcOwner->GetMaxSteeringAngle())
    {
        steeringAngle = m_npcOwner->GetMaxSteeringAngle();
    }
    else if (angle < -m_npcOwner->GetMaxSteeringAngle())
    {
        steeringAngle = -m_npcOwner->GetMaxSteeringAngle();
    }
    else
    {
        steeringAngle = angle;
    }
    m_aiControls.aiOutput.steeringAngle = steeringAngle;
    DirectX::SimpleMath::Vector3 vecToDestination = m_currentDestination - m_npcOwner->GetPos();
    vecToDestination.Normalize();
    m_aiControls.aiOutput.steeringDirection = vecToDestination;
    m_aiControls.aiOutput.angleToDestination = angle;

    //m_debugData->DebugPushUILineDecimalNumber("m_aiControls.aiOutput.angleToDestination ", m_aiControls.aiOutput.angleToDestination, "");
    //m_debugData->DebugPushUILineDecimalNumber("m_aiControls.aiOutput.steeringAngle ", Utility::ToDegrees(m_aiControls.aiOutput.steeringAngle), "");

    DirectX::SimpleMath::Vector3 steeringLine = DirectX::SimpleMath::Vector3::UnitX;
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, DirectX::SimpleMath::Matrix::CreateRotationY(m_aiControls.aiOutput.steeringAngle));
    steeringLine = DirectX::SimpleMath::Vector3::Transform(steeringLine, m_npcOwner->GetAlignment());
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), steeringLine, 75.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    /*
    //return vecToDestination;
    return m_desiredHeading;
    */
}

void NpcAI::UpdateAI(const float aTimeStep)
{
    UpdateAvoidanceBox();
    if ((m_npcOwner->GetPos() - m_currentWaypoint.waypointPos).Length() < m_currentWaypoint.waypointRadius)
    {
        Utility::IncrementWayPath(m_currentWayPath);
        ++m_lapCounter;
    }

    //std::string textLine = "Timer  " + std::to_string(m_timer.GetTotalSeconds());
    //std::string lapCount = "Car " + std::to_string(m_npcOwner->GetID()) + " = ";
    //m_debugData->DebugPushUILineWholeNumber(lapCount, m_lapCounter, "");

    m_currentWaypoint = Utility::GetWaypointFromPath(m_currentWayPath);
    m_currentDestination = m_currentWaypoint.waypointPos;

    AdjustHeadingForVelocity();

    UpdateSeparation();
    if (m_destinationTargets.isSeparationTargetInRadius == true)
    {
        m_currentDestination = m_destinationTargets.separationTarget;
    }

    m_avoidanceTargetPos = m_playerVehicle->GetPos();


    if (m_isAvoidanceTrue == true)
    {
        /*
        if (m_npcOwner->GetID() % 2)
        {
            AvoidPos();
        }
        else
        {
            AvoidPosNewAlt1();
        }
        */
        if ((m_npcOwner->GetID() +1) % 3 == 0)
        {
            AvoidPosOld();
        }
        else if ((m_npcOwner->GetID() + 0) % 3 == 0)
        {
            AvoidPos();
        }
        else
        {
            AvoidPosNewAlt1();
        }
        //AvoidPos();
        if (m_debugToggle == false)
        {
            
        }
    }

    if (m_isAvoidanceTrue == true && m_isAvoidanceTrueTest1 == true)
    {
    }
    else
    {
        //UpdateDestinationSmoothing();
    }
    UpdateDestinationSmoothing();
    if (m_debugToggle == true)
    {
        UpdateControlOutput();
    }
    //UpdateControlOutput();

    m_emergencyToggle = m_isAvoidanceTrueTest1;
    m_isJumpTriggered = false;
    m_destinationTargets.isSeparationTargetInRadius = false;
    m_isAvoidanceTrue = false;
    m_isAvoidanceTrueTest1 = false;
    m_isAvoidanceTrueTest2 = false;
    m_avoidanceTargetNpc = nullptr;
}

void NpcAI::UpdateAvoidanceBox()
{
    const DirectX::SimpleMath::Vector3 vehicleDimensions = m_npcOwner->GetDimensions();
    const float avoidanceWidthMod = 1.0f;
    const float avoidanceWidthMin = vehicleDimensions.z * avoidanceWidthMod;

    const float avoidanceWidth = avoidanceWidthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * avoidanceWidthMin;;
    m_avoidanceBoxWidth = avoidanceWidth;
    const float avoidanceHeight = vehicleDimensions.y * 0.5f;
    float avoidanceLength = m_avoidanceBoxLengthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * m_avoidanceBoxLengthMin;
    //avoidanceLength = 30.0f;
    m_avoidanceBoxLength = avoidanceLength;
    m_avoidanceBox.Extents = DirectX::SimpleMath::Vector3(avoidanceLength, avoidanceHeight, avoidanceWidth);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3(vehicleDimensions.x + avoidanceLength, 0.0f, 0.0f);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 1.0f), -avoidanceHeight * 0.0f, 0.0f);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 0.5f), -avoidanceHeight * 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3 velocityForward = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vehicleForward = m_npcOwner->GetForward();
    DirectX::SimpleMath::Vector3  destination = m_currentDestination - m_npcOwner->GetPos();
    destination.Normalize();
    vehicleForward.Normalize();
    velocityForward.Normalize();

    velocityForward = m_npcOwner->GetVelocity();
    //velocityForward = DirectX::SimpleMath::Vector3::UnitX;
    //velocityForward = m_npcOwner->GetForward();
    DirectX::SimpleMath::Vector3 velocityUp = m_npcOwner->GetUp();
    DirectX::SimpleMath::Vector3 velocityRight = velocityForward.Cross(velocityUp);
    velocityUp = velocityForward.Cross(-velocityRight);

    m_avoidanceBoxRight = velocityRight;
    m_avoidanceBoxRight.Normalize();
    m_avoidaneBoxUp = velocityUp;
    m_avoidaneBoxUp.Normalize();

    DirectX::SimpleMath::Matrix velocityAlignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -velocityRight, velocityUp);
    //velocityAlignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    //m_avoidanceBox.Center = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, m_npcOwner->GetAlignment());

    m_avoidanceBox.Center.x = avoidanceLength + vehicleDimensions.x * 0.5f;
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, velocityAlignment);
    m_avoidanceBox.Center = m_avoidanceBox.Center + m_npcOwner->GetPos();

    m_avoidanceBoxBase = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 0.5f), 0.0f, 0.0f);
    m_avoidanceBoxBase = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBoxBase, velocityAlignment);
    m_avoidanceBoxBase = m_avoidanceBoxBase + m_npcOwner->GetPos();

    DirectX::SimpleMath::Vector3 testPos = m_avoidanceBox.Center;
    m_avoidanceAlignment = velocityAlignment;
    //m_avoidanceAlignment *= DirectX::SimpleMath::Matrix::CreateTranslation(m_avoidanceBox.Center);

    DirectX::SimpleMath::Quaternion velocityAlignmentQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(velocityAlignment);
    m_avoidanceBox.Orientation = velocityAlignmentQuat;

    DirectX::SimpleMath::Vector3 testPos2 = m_npcOwner->GetPos();
    float testRadius = (m_avoidanceBox.Center - m_npcOwner->GetPos()).Length();
    m_avoidanceRadius = (avoidanceLength + avoidanceLength + (m_avoidanceBoxWidth * 0.5f) + (vehicleDimensions.x * 0.5f)) * 0.9f;

    int testBreak = 0;
    testBreak++;
}

void NpcAI::UpdateControlOutput()
{
    SetForwardThrustOutput();
    SetOmniOutput();
    SetSteeringOutput();
    if (m_isJumpTriggered == true)
    {
        m_aiControls.aiOutput.isTriggerJumpTrue = true;
    }
    else
    {
        m_aiControls.aiOutput.isTriggerJumpTrue = false;
    }
}

void NpcAI::UpdateDestinationSmoothing()
{
    std::vector<DirectX::SimpleMath::Vector3> prevSmoothingVec;
    prevSmoothingVec.clear();
    prevSmoothingVec.resize(m_destinationSmoothingSize);
    prevSmoothingVec = m_destinationSmoothing;
    m_destinationSmoothing[0] = m_currentDestination;
    for (int i = 1; i < m_destinationSmoothingSize - 1; ++i)
    {
        m_destinationSmoothing[i] = prevSmoothingVec[i - 1];
    }

    //DirectX::SimpleMath::Vector3 smoothedDest = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 smoothedDest = m_currentDestination;
    for (int i = 0; i < m_destinationSmoothingSize; ++i)
    {
        smoothedDest += m_destinationSmoothing[i];
    }
    m_currentDestination = smoothedDest / m_destinationSmoothingSize;
}

void NpcAI::UpdateDesiredHeading()
{
    DirectX::SimpleMath::Vector3 updateHeading = DirectX::SimpleMath::Vector3::Zero;
    float updateVelocity = 0.0f;
    if (m_behavior.followWayPath == true)
    {
        DirectX::SimpleMath::Vector3 headingToWaypoint = m_currentWaypoint.waypointPos - m_npcOwner->GetPos();
        headingToWaypoint.Normalize();
        headingToWaypoint *= m_behavior.wayPathWeight;
        updateHeading += headingToWaypoint;
    }
    if (m_behavior.wander == true)
    {
        DirectX::SimpleMath::Vector3 headingToWander = m_destinationTargets.wanderTarget - m_npcOwner->GetPos();
        headingToWander.Normalize();
        headingToWander *= m_behavior.wanderWeight;
        updateHeading += headingToWander;
    }
    if (m_behavior.stop == true)
    {
        DirectX::SimpleMath::Vector3 reverseHeading = m_npcOwner->GetVelocity() * -1.0f;
        reverseHeading.y = 0.0f;
        reverseHeading.Normalize();
        reverseHeading *= m_behavior.stopWeight;
        updateHeading += reverseHeading;
    }
    if (m_behavior.none == false)
    {
        updateHeading.Normalize();
        m_desiredHeading = updateHeading;
        m_desiredVelocity = updateVelocity;
    }
    else if (m_behavior.none == true)
    {
        m_desiredHeading = DirectX::SimpleMath::Vector3::Zero;
        m_desiredVelocity = 0.0f;
    }
}

void NpcAI::UpdateSeparation()
{
    std::vector<DirectX::SimpleMath::Vector3> npcPos = m_npcController->GetVecOfNpcPos(m_npcOwner->GetID());
    std::vector<DirectX::SimpleMath::Vector3> inDistanceVec;

    for (int i = 0; i < npcPos.size(); ++i)
    {
        float distance = DirectX::SimpleMath::Vector3::Distance(npcPos[i], m_npcOwner->GetPos());

        if (distance <= m_destinationTargets.separationRadius)
        {
            inDistanceVec.push_back(npcPos[i]);
        }
    }

    if (inDistanceVec.size() == 0)
    {
        m_destinationTargets.isSeparationTargetInRadius = false;
        m_destinationTargets.separationTarget = DirectX::SimpleMath::Vector3::Zero;
    }
    else
    {
        m_destinationTargets.isSeparationTargetInRadius = true;
        DirectX::SimpleMath::Vector3 testPos = DirectX::SimpleMath::Vector3::Zero;
        for (int i = 0; i < inDistanceVec.size(); ++i)
        {
            DirectX::SimpleMath::Vector3 vecToTarg = inDistanceVec[i] - m_npcOwner->GetPos();
            float distance = vecToTarg.Length();
            vecToTarg.Normalize();

            float ratio = distance / m_destinationTargets.separationRadius;
            ratio = 1.0f - ratio;
            testPos += vecToTarg * ratio;
        }
        testPos /= inDistanceVec.size();
        testPos *= -1.0f;
        testPos *= m_destinationTargets.separationMagnitude;
        testPos += m_npcOwner->GetPos();
        m_destinationTargets.separationTarget = testPos;
    }
}

DirectX::SimpleMath::Vector3 NpcAI::Wander()
{
    float xWander = Utility::RandomClamped() * m_destinationTargets.wanderJitter;
    float zWander = Utility::RandomClamped() * m_destinationTargets.wanderJitter;
    m_destinationTargets.wanderTarget == DirectX::SimpleMath::Vector3(xWander, m_npcOwner->GetHeight(), zWander);
    m_destinationTargets.wanderTarget.Normalize();

    m_destinationTargets.wanderTarget *= m_destinationTargets.wanderRadius;
    DirectX::SimpleMath::Vector3 localTarget = m_destinationTargets.wanderTarget + DirectX::SimpleMath::Vector3(m_destinationTargets.wanderDistance, 0.0f, 0.0f);

    DirectX::SimpleMath::Matrix worldMat = DirectX::SimpleMath::Matrix::CreateWorld(m_npcOwner->GetPos(), m_npcOwner->GetForward(), m_npcOwner->GetUp());
    DirectX::SimpleMath::Vector3 worldTarget = DirectX::SimpleMath::Vector3::Transform(localTarget, worldMat);

    return worldTarget - m_npcOwner->GetPos();
}