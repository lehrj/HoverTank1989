#pragma once


class DebugData
{
public:
    DebugData();

    std::vector<std::pair<std::string, float>> DebugGetUI() { return m_debugUI; };
    std::vector<std::string> DebugGetUIVector() { return m_debugUIVector; };
    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> DebugGetTestLines() const { return m_debugLinesVec; };

    void DebugClearUI() {
        m_debugUI.clear();
        m_debugUIVector.clear();
        m_debugLinesVec.clear();
    };

    void DebugPushUILine(std::string aString, float aVal);
    void DebugPushUILineDecimalNumber(std::string aString1, float aVal, std::string aString2);
    void DebugPushUILineWholeNumber(std::string aString1, int aVal, std::string aString2);
    void DebugPushTestLine(DirectX::SimpleMath::Vector3 aLineBase, DirectX::SimpleMath::Vector3 aLineEnd, float aLength, float aYOffset, DirectX::SimpleMath::Vector4 aColor);
    void DebugPushTestLineBetweenPoints(DirectX::SimpleMath::Vector3 aPoint1, DirectX::SimpleMath::Vector3 aPoint2, DirectX::SimpleMath::Vector4 aColor);
    void DebugPushTestLinePositionIndicator(const DirectX::SimpleMath::Vector3 aPoint, const float aLineLength, const float aOffset, const DirectX::SimpleMath::Vector4 aColor);

private:

    std::vector<std::tuple<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector4>> m_debugLinesVec;
    std::vector<std::pair<std::string, float>> m_debugUI;
    std::vector<std::string> m_debugUIVector;
};

