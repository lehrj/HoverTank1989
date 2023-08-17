#include "pch.h"
#include "DebugData.h"


DebugData::DebugData()
{
    DebugClearUI();
}

void DebugData::DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor)
{
    if (m_isDebugOn == true)
    {
        DirectX::SimpleMath::Vector3 scaledLineBase = aLineBase;
        scaledLineBase.y += aYOffset;
        DirectX::SimpleMath::Vector3 scaledLineEnd = aLineEnd;
        scaledLineEnd.Normalize();
        scaledLineEnd *= aLength;
        scaledLineEnd += scaledLineBase;
        std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4> lineTup(scaledLineBase, scaledLineEnd, aColor);
        m_debugLinesVec.push_back(lineTup);
    }
}

void DebugData::PushDebugLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::XMVECTORF32 aColor)
{
    if (m_isDebugOn == true)
    {
        DirectX::SimpleMath::Vector3 scaledLineBase = aLineBase;
        scaledLineBase.y += aYOffset;
        DirectX::SimpleMath::Vector3 scaledLineEnd = aLineEnd;
        scaledLineEnd.Normalize();
        scaledLineEnd *= aLength;
        scaledLineEnd += scaledLineBase;
        std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32> lineTup(scaledLineBase, scaledLineEnd, aColor);
        m_debugLinesVector.push_back(lineTup);
    }
}

void DebugData::PushDebugLineScaled(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLengthMin, float aLengthMod,  float aYOffset, DirectX::XMVECTORF32 aColor)
{
    if (m_isDebugOn == true)
    {
        DirectX::SimpleMath::Vector3 scaledLineBase = aLineBase;
        scaledLineBase.y += aYOffset;
        DirectX::SimpleMath::Vector3 scaledLineEnd = aLineEnd;
        scaledLineEnd.Normalize();
        //scaledLineEnd *= aLengthMin;
        //scaledLineEnd *= aLengthMod * aLineEnd.Length();
        scaledLineEnd *= aLengthMin + (aLengthMod * aLineEnd.Length());
        scaledLineEnd += scaledLineBase;
        std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32> lineTup(scaledLineBase, scaledLineEnd, aColor);
        m_debugLinesVector.push_back(lineTup);
    }
}

void DebugData::DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor)
{
    if (m_isDebugOn == true)
    {
        std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4> lineTup(aPoint1, aPoint2, aColor);
        m_debugLinesVec.push_back(lineTup);
    }
}

void DebugData::PushTestDebugBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::XMVECTORF32 aColor)
{
    if (m_isDebugOn == true)
    {
        std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::XMVECTORF32> lineTup(aPoint1, aPoint2, aColor);
        m_debugLinesVector.push_back(lineTup);
    }
}

void DebugData::DebugPushTestLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::SimpleMath::Vector4 aColor)
{
    if (m_isDebugOn == true)
    {
        DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
        DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
        DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
        DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
        DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
        DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
    }
}

void DebugData::PushDebugLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::XMVECTORF32 aColor)
{
    if (m_isDebugOn == true)
    {
        PushDebugLine(aPoint, DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
        PushDebugLine(aPoint, DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
        PushDebugLine(aPoint, DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
        PushDebugLine(aPoint, -DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
        PushDebugLine(aPoint, -DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
        PushDebugLine(aPoint, -DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
    }
}

void DebugData::DebugPushUILine(std::string aString, float aVal)
{
    if (m_isDebugOn == true)
    {
        std::pair<std::string, float> newPair = std::make_pair(aString, aVal);
        m_debugUI.push_back(newPair);
    }
}

void DebugData::DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2)
{
    if (m_isDebugOn == true)
    {
        std::string textLine = aString1 + " " + std::to_string(aVal) + " " + aString2;
        m_debugUIVector.push_back(textLine);
    }
}

void DebugData::DebugPushUILineString(std::string aString)
{
    if (m_isDebugOn == true)
    {
        m_debugUIVector.push_back(aString);
    }
}

void DebugData::DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2)
{
    if (m_isDebugOn == true)
    {
        std::string textLine = aString1 + " " + std::to_string(aVal) + " " + aString2;
        m_debugUIVector.push_back(textLine);
    }
}

void DebugData::ToggleDebugOff()
{
    m_isDebugOn = false;
}

void DebugData::ToggleDebugOn()
{
    m_isDebugOn = true;
}

