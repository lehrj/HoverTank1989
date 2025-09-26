#pragma once
#include "DebugData.h"
#include "Utility.h"

class Lighting
{
public:
    enum class LightingState
    {
        LIGHTINGSTATE_JI,
        LIGHTINGSTATE_JI_TEST,
        LIGHTINGSTATE_BMW,
        LIGHTINGSTATE_BMWALT,
        LIGHTINGSTATE_STARTSCREEN,
        LIGHTINGSTATE_TEASERSCREEN,
        LIGHTINGSTATE_MANUAL,
        LIGHTINGSTATE_NULL,
        LIGHTINGSTATE_STARTUP,
        LIGHTINGSTATE_TEST01,
        LIGHTINGSTATE_,
        LIGHTINGSTATE_DEFAULT,
    };

    //Lighting();

    LightingState GetLightingState();
    DirectX::SimpleMath::Vector3 GetLightDir();
    float GetLightAngle() const { return m_logoLightingAngle; };


    void SetCamPos(const DirectX::SimpleMath::Vector3 aCamPos);
    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr);

    void SetFogVals1(std::unique_ptr<DirectX::NormalMapEffect>  aEffect, const DirectX::SimpleMath::Vector3 aCamPos, const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals2(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals3(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);

    void SetLighting(LightingState aLightState);  
    void SetLightingNormColorTextureVertex(LightingState aLightState);
    void SetLightingNormColorVertex2(LightingState aLightState);
    void SetLightingColorVertex3(LightingState aLightState);


    void SetTimerVal(const float aVal);

    void ResetTimer();

    void SetTargPos(const DirectX::SimpleMath::Vector3 aTargetPos);

    void UpdateLighting(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer);
    void UpdateLightingNormColorTextureVertex(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer);
    void UpdateLightingNormColorVertex2(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer);
    void UpdateLightingColorVertex3(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer);

private:

    void UpdateTimer(const double aTimer);

    LightingState                               m_currentLightingState;
    LightingState                               m_currentLightingStateNormColorTextureVertex;
    LightingState                               m_currentLightingStateNormColorVertex2;
    LightingState                               m_currentLightingStateColorVertex3;

    DirectX::SimpleMath::Vector3                m_cameraFocusPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3                m_cameraPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3                m_lightPos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos2 = DirectX::SimpleMath::Vector3::UnitX;

    // light pos for m_effect2
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos2 = DirectX::SimpleMath::Vector3::UnitX;

    std::shared_ptr<DebugData> m_debugData;

    float m_logoLightingAngle = Utility::ToRadians(0.0f);

    float m_testTimerTotal = 0.0f;
    float m_testTimer = 0.0f;
    float m_testVal0 = 0.0f;
    float m_testVal1 = 0.0f;
    float m_testVal2 = 0.0f;

    float m_lightTimer = 0.0f;
    float m_lightTimerMax = 3.0f;
    bool m_isTimerOn = false;

    const DirectX::SimpleMath::Vector3                m_defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
    const DirectX::SimpleMath::Vector3                m_defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
    const DirectX::SimpleMath::Vector3                m_defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);


};

