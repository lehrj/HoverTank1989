#include "pch.h"
#include "DebugData.h"


DebugData::DebugData()
{
    DebugClearUI();
}

void DebugData::DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor)
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

void DebugData::DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor)
{
    std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4> lineTup(aPoint1, aPoint2, aColor);
    m_debugLinesVec.push_back(lineTup);
}

void DebugData::DebugPushTestLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::SimpleMath::Vector4 aColor)
{
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitX, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitY, aLineLength, aOffset, aColor);
    DebugPushTestLine(aPoint, -DirectX::SimpleMath::Vector3::UnitZ, aLineLength, aOffset, aColor);
}

void DebugData::DebugPushUILine(std::string aString, float aVal)
{
    std::pair<std::string, float> newPair = std::make_pair(aString, aVal);
    m_debugUI.push_back(newPair);
}

void DebugData::DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2)
{
    std::string textLine = aString1 + " " + std::to_string(aVal) + " " + aString2;
    m_debugUIVector.push_back(textLine);
}

void DebugData::DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2)
{
    std::string textLine = aString1 + " " + std::to_string(aVal) + " " + aString2;
    m_debugUIVector.push_back(textLine);
}
