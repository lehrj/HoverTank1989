#pragma once


class Lighting
{
public:
    enum class LightingState
    {
        LIGHTINGSTATE_JI,
        LIGHTINGSTATE_BMW,
        LIGHTINGSTATE_STARTSCREEN,
        LIGHTINGSTATE_TEASERSCREEN,
        LIGHTINGSTATE_MANUAL,
        LIGHTINGSTATE_NULL,
        LIGHTINGSTATE_STARTUP,
        LIGHTINGSTATE_TEST01,
        LIGHTINGSTATE_,
    };

    //Lighting();

    LightingState GetLightingState();

    void SetLighting(LightingState aLightState);  
    void SetLightingNormColorTextureVertex(LightingState aLightState);
    void SetLightingNormColorVertex2(LightingState aLightState);
    void SetLightingColorVertex3(LightingState aLightState);

    void SetFogVals1(std::unique_ptr<DirectX::NormalMapEffect>  aEffect, const DirectX::SimpleMath::Vector3 aCamPos, const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals2(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);
    void SetFogVals3(const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal);

    void UpdateLighting(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer);
    void UpdateLightingNormColorTextureVertex(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer);
    void UpdateLightingNormColorVertex2(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer);
    void UpdateLightingColorVertex3(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer);

private:

    LightingState                               m_currentLightingState;
    LightingState                               m_currentLightingStateNormColorTextureVertex;
    LightingState                               m_currentLightingStateNormColorVertex2;
    LightingState                               m_currentLightingStateColorVertex3;

    DirectX::SimpleMath::Vector3                m_cameraFocusPos = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3                m_lightPos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightPos2 = DirectX::SimpleMath::Vector3::UnitX;

    // light pos for m_effect2
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos0 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos1 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3                m_lightEffect2Pos2 = DirectX::SimpleMath::Vector3::UnitX;

};

