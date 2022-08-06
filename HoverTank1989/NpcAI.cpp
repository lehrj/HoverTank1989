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
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTarget - m_npcOwner->GetPos();
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
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTarget;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTarget - m_npcOwner->GetPos();
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

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTarget;
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

    DirectX::SimpleMath::Vector3 vecToTarget = m_npcOwner->GetPos() - m_avoidanceTarget;
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
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTarget;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTarget - m_npcOwner->GetPos();
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

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTarget;
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



    DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTarget2->GetPos();
    DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTarget2->GetVelocity();

    targetPos = m_playerVehicle->GetPos();
    targetVelocity = m_playerVehicle->GetVelocity();

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
    testAvoidanceVec *= testRatio;
    testAvoidanceVec += m_npcOwner->GetPos();
    testAvoidanceVec.y = 0.0f;
    m_currentDestination = testAvoidanceVec;
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    int testBreak = 0;
    testBreak++;
}

void NpcAI::AvoidPos5()
{
    //DirectX::SimpleMath::Vector3 targetWorldPos = m_playerVehicle->GetPos();
    //DirectX::SimpleMath::Vector3 targetWorldVelocity = m_playerVehicle->GetVelocity();
    DirectX::SimpleMath::Vector3 targetWorldPos = m_avoidanceTarget2->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = m_avoidanceTarget2->GetVelocity();

    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVeloctiy = m_npcOwner->GetVelocity();
    //DirectX::SimpleMath::Vector3 selfWorldVeloctiy = DirectX::SimpleMath::Vector3::UnitX;


    //DirectX::SimpleMath::Matrix selfWorldAlignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Matrix selfWorldAlignment = m_avoidanceAlignment;
    DirectX::SimpleMath::Matrix boxWorldAlignment = m_avoidanceAlignment;

    DirectX::SimpleMath::Matrix localizeMat = selfWorldAlignment;
    localizeMat = localizeMat.Invert();

    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVeloctiy = DirectX::SimpleMath::Vector3::Transform(selfWorldVeloctiy, localizeMat);
    DirectX::SimpleMath::Vector3 localDesitinationPos = DirectX::SimpleMath::Vector3::Transform(m_currentDestination, localizeMat);
    DirectX::SimpleMath::Vector3 localBoxInflectionPoint = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, localizeMat);


    const float distanceBetween = (selfLocalPos - targetLocalPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfLocalVeloctiy - targetLocalVelocity;
    const float velocityMag = (selfLocalVeloctiy - targetLocalVelocity).Length();
    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetLocalPos + (targetLocalVelocity * lookAheadTime);
    DirectX::SimpleMath::Vector3 lookAheadPos = selfLocalPos + (selfLocalVeloctiy * lookAheadTime);
    
    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;



    DirectX::SimpleMath::Vector3 testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;
    
    testAvoidanceVec *= testRatio;
    testAvoidanceVec += m_npcOwner->GetPos();

    testAvoidanceVec.y = 0.0f;
    DirectX::SimpleMath::Vector3 updatedCurrentDest = testAvoidanceVec;
    //DirectX::SimpleMath::Vector3 updatedCurrentDest = vecToTarget;
    
    updatedCurrentDest = DirectX::SimpleMath::Vector3::Transform(updatedCurrentDest, selfWorldAlignment);
    updatedCurrentDest.y = 0.0f;
    //m_currentDestination = testAvoidanceVec;
    m_currentDestinationTest5 = updatedCurrentDest;

    /*
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), updatedCurrentDest, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(updatedCurrentDest, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 10.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_debugData->DebugPushUILineDecimalNumber("distanceBetween = ", distanceBetween, "");
    m_debugData->DebugPushUILineDecimalNumber("velocityMag = ", velocityMag, "");
    m_debugData->DebugPushUILineDecimalNumber("impactTime = ", impactTime, "");
    m_debugData->DebugPushUILineDecimalNumber("lookAheadTime = ", lookAheadTime, "");

    DirectX::SimpleMath::Vector4 colorWhite(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorRed(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorBlue(0.0f, 1.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorGreen(0.0f, 0.0f, 1.0f, 1.0f);

    m_debugData->DebugPushTestLine(targetWorldPos, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0, colorWhite);
    m_debugData->DebugPushTestLine(targetLocalPos, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0, colorWhite);
    */
}

void NpcAI::AvoidPos6()
{
    //DirectX::SimpleMath::Vector3 targetWorldPos = m_playerVehicle->GetPos();
    //DirectX::SimpleMath::Vector3 targetWorldVelocity = m_playerVehicle->GetVelocity();
    DirectX::SimpleMath::Vector3 targetWorldPos = m_avoidanceTarget2->GetPos();
    DirectX::SimpleMath::Vector3 targetWorldVelocity = m_avoidanceTarget2->GetVelocity();

    DirectX::SimpleMath::Vector3 selfWorldPos = m_npcOwner->GetPos();
    DirectX::SimpleMath::Vector3 selfWorldVeloctiy = m_npcOwner->GetVelocity();
    //DirectX::SimpleMath::Vector3 selfWorldVeloctiy = DirectX::SimpleMath::Vector3::UnitX;


    //DirectX::SimpleMath::Matrix selfWorldAlignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Matrix selfWorldAlignment = m_avoidanceAlignment;
    DirectX::SimpleMath::Matrix boxWorldAlignment = m_avoidanceAlignment;

    DirectX::SimpleMath::Matrix localizeMat = selfWorldAlignment;
    localizeMat = localizeMat.Invert();

    DirectX::SimpleMath::Vector3 targetLocalPos = DirectX::SimpleMath::Vector3::Transform(targetWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 targetLocalVelocity = DirectX::SimpleMath::Vector3::Transform(targetWorldVelocity, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalPos = DirectX::SimpleMath::Vector3::Transform(selfWorldPos, localizeMat);
    DirectX::SimpleMath::Vector3 selfLocalVeloctiy = DirectX::SimpleMath::Vector3::Transform(selfWorldVeloctiy, localizeMat);
    DirectX::SimpleMath::Vector3 localDesitinationPos = DirectX::SimpleMath::Vector3::Transform(m_currentDestination, localizeMat);
    DirectX::SimpleMath::Vector3 localBoxInflectionPoint = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, localizeMat);


    const float distanceBetween = (selfLocalPos - targetLocalPos).Length();
    DirectX::SimpleMath::Vector3 intersectVelocity = selfLocalVeloctiy - targetLocalVelocity;
    const float velocityMag = (selfLocalVeloctiy - targetLocalVelocity).Length();
    const float impactTime = distanceBetween / (velocityMag + 0.000000001f);
    const float lookAheadTime = impactTime;
    DirectX::SimpleMath::Vector3 avoidPos = targetLocalPos + (targetLocalVelocity * lookAheadTime);
    DirectX::SimpleMath::Vector3 lookAheadPos = selfLocalPos + (selfLocalVeloctiy * lookAheadTime);

    DirectX::SimpleMath::Vector3 vecToTarget = lookAheadPos - avoidPos;

    DirectX::SimpleMath::Vector3 interceptPosTest = vecToTarget;

    DirectX::SimpleMath::Vector3 inflectionToInterceptNorm = interceptPosTest - localBoxInflectionPoint;
    inflectionToInterceptNorm.Normalize();

    if (inflectionToInterceptNorm.x > 0.0)
    {
        inflectionToInterceptNorm.x -= 1.0f;
    }
    else
    {
        inflectionToInterceptNorm.x += 1.0f;
    }
    if (inflectionToInterceptNorm.z > 0.0)
    {
        inflectionToInterceptNorm.z += 1.0f;
    }
    else
    {
        inflectionToInterceptNorm.z -= 1.0f;
    }


    inflectionToInterceptNorm *= 50.0f;
    inflectionToInterceptNorm += selfWorldPos;
    DirectX::SimpleMath::Vector3 testUpdatePos = inflectionToInterceptNorm;
    testUpdatePos = DirectX::SimpleMath::Vector3::Transform(testUpdatePos, selfWorldAlignment);
    //testUpdatePos += selfWorldPos;

    DirectX::SimpleMath::Vector3 testAvoidanceVec = vecToTarget;
    float distanceRatio = vecToTarget.Length();
    distanceRatio /= m_avoidanceBoxLength;

    const float avoidanceBoxLength = m_avoidanceBoxLength * 2.0f;
    const float frontOffset = 7.0f;
    float testRatio = abs((avoidanceBoxLength + frontOffset) - vecToTarget.Length()) + 1.0f;

    testAvoidanceVec *= testRatio;
    //testAvoidanceVec += m_npcOwner->GetPos();

    testAvoidanceVec.y = 0.0f;
    DirectX::SimpleMath::Vector3 updatedCurrentDest = testAvoidanceVec;
    //DirectX::SimpleMath::Vector3 updatedCurrentDest = vecToTarget;

    updatedCurrentDest = DirectX::SimpleMath::Vector3::Transform(updatedCurrentDest, selfWorldAlignment);
    updatedCurrentDest.y = 0.0f;
    //m_currentDestination = testAvoidanceVec;
    m_currentDestinationTest5 = updatedCurrentDest;

    m_currentDestinationTest5 = testUpdatePos;

    /*
    m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), updatedCurrentDest, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(updatedCurrentDest, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 10.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    m_debugData->DebugPushUILineDecimalNumber("distanceBetween = ", distanceBetween, "");
    m_debugData->DebugPushUILineDecimalNumber("velocityMag = ", velocityMag, "");
    m_debugData->DebugPushUILineDecimalNumber("impactTime = ", impactTime, "");
    m_debugData->DebugPushUILineDecimalNumber("lookAheadTime = ", lookAheadTime, "");

    DirectX::SimpleMath::Vector4 colorWhite(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorRed(1.0f, 0.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorBlue(0.0f, 1.0f, 0.0f, 1.0f);
    DirectX::SimpleMath::Vector4 colorGreen(0.0f, 0.0f, 1.0f, 1.0f);

    m_debugData->DebugPushTestLine(targetWorldPos, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0, colorWhite);
    m_debugData->DebugPushTestLine(targetLocalPos, DirectX::SimpleMath::Vector3::UnitY, 25.0f, 0.0, colorWhite);
    */
}

void NpcAI::AvoidPos()
{
    DirectX::SimpleMath::Vector3 avoidTarget = m_avoidanceTarget;
    avoidTarget.y += 4.5f;
    DirectX::SimpleMath::Vector3 owner = m_npcOwner->GetPos();
    owner.y += 4.5f;
    //m_debugData->DebugPushTestLine(avoidTarget, DirectX::SimpleMath::Vector3::UnitY, 15.f, 0.0f, DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLineBetweenPoints(owner, avoidTarget, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    DirectX::SimpleMath::Matrix alignment = m_npcOwner->GetAlignment();
    DirectX::SimpleMath::Vector3 localPos = m_avoidanceTarget - m_npcOwner->GetPos();
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

    DirectX::SimpleMath::Vector3 obsticlePos = m_avoidanceTarget;
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
////    m_currentDestination = avoidanceVec;
    //DirectX::SimpleMath::Vector3 testForward = m_npcOwner->GetForward();
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, localMat);
    //testForward = DirectX::SimpleMath::Vector3::Transform(testForward, updateMat);

    DirectX::SimpleMath::Vector3 testAvoidanceVec(brakeVal, 0.0f, lateralVal);
    testAvoidanceVec.Normalize();
    testAvoidanceVec *= 20.0f;
    //testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, updateMat);
    testAvoidanceVec = DirectX::SimpleMath::Vector3::Transform(testAvoidanceVec, m_npcOwner->GetAlignment());
    testAvoidanceVec += m_npcOwner->GetPos();
/////    m_currentDestination = testAvoidanceVec;
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), testAvoidanceVec, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    //m_debugData->DebugPushTestLine(m_npcOwner->GetPos(), testAvoidanceVec, 40.0f, 8.5f, DirectX::SimpleMath::Vector4(0.9f, 0.9f, 0.9f, 1.0f));


    
    DirectX::SimpleMath::Vector3 targetPos = m_avoidanceTarget2->GetPos();
    DirectX::SimpleMath::Vector3 targetVelocity = m_avoidanceTarget2->GetVelocity();

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
    testAvoidanceVec *= testRatio;
    testAvoidanceVec += m_npcOwner->GetPos();

    //m_currentDestination = testAvoidanceVec;
    m_currentDestinationTest0 = testAvoidanceVec;


    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    //m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

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
    wp4 = Utility::CreateWaypoint(pos, radius);
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


    //Utility::PushWaypointToPath(m_currentWayPath, wp2);
    Utility::PushWaypointToPath(m_currentWayPath, wp4);
    Utility::PushWaypointToPath(m_currentWayPath, wp5);
    Utility::PushWaypointToPath(m_currentWayPath, wp6);
    Utility::PushWaypointToPath(m_currentWayPath, wp1);
    Utility::PushWaypointToPath(m_currentWayPath, wp3);
    Utility::PushWaypointToPath(m_currentWayPath, wp8);
    Utility::PushWaypointToPath(m_currentWayPath, wp7);
    Utility::PushWaypointToPath(m_currentWayPath, wp2);
    
    /*
    if (m_npcOwner->GetID() % 2)
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp8);
        Utility::PushWaypointToPath(m_currentWayPath, wp7);
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
        Utility::PushWaypointToPath(m_currentWayPath, wp5);
        Utility::PushWaypointToPath(m_currentWayPath, wp6);
    }
    else
    {
        Utility::PushWaypointToPath(m_currentWayPath, wp6);
        Utility::PushWaypointToPath(m_currentWayPath, wp5);
        Utility::PushWaypointToPath(m_currentWayPath, wp4);
        Utility::PushWaypointToPath(m_currentWayPath, wp2);
        Utility::PushWaypointToPath(m_currentWayPath, wp7);
        Utility::PushWaypointToPath(m_currentWayPath, wp8);
        Utility::PushWaypointToPath(m_currentWayPath, wp3);
        Utility::PushWaypointToPath(m_currentWayPath, wp1);
    }
    */
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

    m_avoidanceTarget = DirectX::SimpleMath::Vector3::Zero;
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
    m_destinationTargets.separationRadius = 25.0f;
    m_destinationTargets.wanderDistance = 100.0f;
    m_destinationTargets.wanderJitter = 0.001f;
    m_destinationTargets.wanderRadius = 10.0f;
}

void NpcAI::PushAiAvoidanceTarget(DirectX::SimpleMath::Vector3 aAvoidancePos)
{
    if (m_isAvoidanceTrue == false)
    {
        m_isAvoidanceTrue = true;
        m_avoidanceTarget = aAvoidancePos;
    }
    else
    {
        const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTarget).Length();
        const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();
        if (distanceToNewTarget < distanceToPriorTarget)
        {
            m_avoidanceTarget = aAvoidancePos;
        }
    }
}

void NpcAI::PushAiAvoidanceTarget2(DirectX::SimpleMath::Vector3 aAvoidancePos, NPCVehicle const* aVehicle)
{
    if (m_isAvoidanceTrue == false)
    {
        m_isAvoidanceTrue = true;
        m_avoidanceTarget = aAvoidancePos;
        m_avoidanceTarget2 = aVehicle;
    }
    else
    {
        const float distanceToPriorTarget = (m_npcOwner->GetPos() - m_avoidanceTarget).Length();
        const float distanceToNewTarget = (m_npcOwner->GetPos() - aAvoidancePos).Length();
        if (distanceToNewTarget < distanceToPriorTarget)
        {
            m_avoidanceTarget = aAvoidancePos;
            m_avoidanceTarget2 = aVehicle;
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
        forwardThrustVal = directionToDest.Dot(m_npcOwner->GetForward());
        forwardThrustVal *= m_aiControls.throttleOutputMax;
    }
    else
    {
        forwardThrustVal = - directionToDest.Dot( - m_npcOwner->GetForward());
        forwardThrustVal *= - m_aiControls.throttleOutputMin;
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
    else if (angle < - m_npcOwner->GetMaxSteeringAngle())
    {
        steeringAngle = - m_npcOwner->GetMaxSteeringAngle();
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
    }
    m_currentWaypoint = Utility::GetWaypointFromPath(m_currentWayPath);
    m_currentDestination = m_currentWaypoint.waypointPos;

    AdjustHeadingForVelocity();
    if (m_isAvoidanceTrue == true)
    {
        AvoidPos();
        AvoidPos6();

        /*
        m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestinationTest0, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        m_debugData->DebugPushTestLine(m_currentDestinationTest0, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

        m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestinationTest5, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        m_debugData->DebugPushTestLine(m_currentDestinationTest5, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        */

        m_currentDestination = m_currentDestinationTest0;

        m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_currentDestination, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        m_debugData->DebugPushTestLine(m_currentDestination, DirectX::SimpleMath::Vector3::UnitY, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));


        const float delta = (m_currentDestinationTest0 - m_currentDestinationTest5).Length();
        m_debugData->DebugPushUILineDecimalNumber("delta = ", delta, "");
        if (m_debugToggle == false)
        {
            
        }
    }

    /*
    m_isAvoidanceTrue = true;
    m_avoidanceTarget = m_playerVehicle->GetPos();
    m_avoidanceTarget2 = m_npcOwner;
    AvoidPos5();
    AvoidPos4();
    */

    if (m_debugToggle == true)
    {
        UpdateSeparation();
        if (m_destinationTargets.isSeparationTargetInRadius == true)
        {
            m_currentDestination = m_destinationTargets.separationTarget;
        }
        UpdateDestinationSmoothing();
        UpdateControlOutput();
    }
    
    m_destinationTargets.isSeparationTargetInRadius = false;
    m_isAvoidanceTrue = false;
}

void NpcAI::UpdateAvoidanceBox()
{
    const DirectX::SimpleMath::Vector3 vehicleDimensions = m_npcOwner->GetDimensions();
    const float avoidanceWidthMod = 1.0f;
    const float avoidanceWidthMin = vehicleDimensions.z * avoidanceWidthMod;

    float avoidanceWidth = avoidanceWidthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * avoidanceWidthMin;
    //avoidanceWidth = 60.0f;
    m_avoidanceBoxWidth = avoidanceWidth;
    const float avoidanceHeight = vehicleDimensions.y * 0.5f;
    float avoidanceLength = m_avoidanceBoxLengthMin + (m_npcOwner->GetVelocity().Length() / m_npcOwner->GetTopSpeedCalculated()) * m_avoidanceBoxLengthMin;

    //avoidanceLength = 130.0f;
    m_avoidanceBoxLength = avoidanceLength;
    m_avoidanceBox.Extents = DirectX::SimpleMath::Vector3(avoidanceLength, avoidanceHeight, avoidanceWidth);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3(vehicleDimensions.x + avoidanceLength, 0.0f, 0.0f);
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3((vehicleDimensions.x * 0.5f) + (avoidanceLength * 1.0f), -avoidanceHeight * 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3 velocityForward = m_npcOwner->GetVelocity();
    DirectX::SimpleMath::Vector3 vehicleForward = m_npcOwner->GetForward();
    DirectX::SimpleMath::Vector3  destination = m_currentDestination - m_npcOwner->GetPos();
    destination.Normalize();
    vehicleForward.Normalize();
    velocityForward.Normalize();

    DirectX::SimpleMath::Vector3 testAlign = DirectX::SimpleMath::Vector3::Zero;
     
    testAlign += velocityForward + vehicleForward + destination;
    testAlign.Normalize();
    velocityForward = testAlign;


    velocityForward = vehicleForward;


    DirectX::SimpleMath::Vector3 velocityUp = m_npcOwner->GetUp();
    DirectX::SimpleMath::Vector3 velocityRight = velocityForward.Cross(velocityUp);
    velocityUp = velocityForward.Cross(-velocityRight);

    DirectX::SimpleMath::Matrix velocityAlignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -velocityRight, velocityUp);
    //velocityAlignment = DirectX::SimpleMath::Matrix::CreateWorld(DirectX::SimpleMath::Vector3::Zero, -m_npcOwner->GetRight(), m_npcOwner->GetUp());
    //m_avoidanceBox.Center = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, m_npcOwner->GetAlignment());
    m_avoidanceBox.Center = DirectX::SimpleMath::Vector3::Transform(m_avoidanceBox.Center, velocityAlignment);
    m_avoidanceBox.Center = m_avoidanceBox.Center + m_npcOwner->GetPos();
    //m_debugData->DebugPushUILineDecimalNumber("avoidanceLength = ", avoidanceLength, "");
    //m_debugData->DebugPushTestLineBetweenPoints(m_npcOwner->GetPos(), m_avoidanceBox.Center, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    //m_debugData->DebugPushTestLinePositionIndicator(m_avoidanceBox.Center, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    DirectX::SimpleMath::Vector3 testPos = m_avoidanceBox.Center;
    m_avoidanceAlignment = velocityAlignment;
    m_avoidanceAlignment *= DirectX::SimpleMath::Matrix::CreateTranslation(m_avoidanceBox.Center);
}

void NpcAI::UpdateControlOutput()
{
    SetForwardThrustOutput();
    SetOmniOutput();
    SetSteeringOutput();
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
        //m_destinationTargets.separationTarget = m_currentDestination;
    }
    else
    {
        m_destinationTargets.isSeparationTargetInRadius = true;
        DirectX::SimpleMath::Vector3 testPos = DirectX::SimpleMath::Vector3::Zero;
        for (int i = 0; i < inDistanceVec.size(); ++i)
        {
            DirectX::SimpleMath::Vector3 vecToTarg = inDistanceVec[i] - m_npcOwner->GetPos();
            float distance = vecToTarg.Length();
            //distance = DirectX::SimpleMath::Vector3::Distance(inDistanceVec[i], m_npcOwner->GetPos());
            vecToTarg.Normalize();
            
            float ratio = distance / m_destinationTargets.separationRadius;
            float ratio2 = ratio;
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