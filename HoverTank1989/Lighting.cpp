#include "pch.h"
#include "Lighting.h"


Lighting::LightingState Lighting::GetLightingState()
{
    return m_currentLightingState;
}

void Lighting::SetLighting(LightingState aLightState)
{
    m_currentLightingState = aLightState;
}

void Lighting::SetLightingNormColorTextureVertex(LightingState aLightState)
{
    m_currentLightingStateNormColorTextureVertex = aLightState;
}

void Lighting::SetLightingNormColorVertex2(LightingState aLightState)
{
    m_currentLightingStateNormColorVertex2 = aLightState;
}

void Lighting::SetLightingColorVertex3(LightingState aLightState)
{
    m_currentLightingStateColorVertex3 = aLightState;
}

void Lighting::SetFogVals1(std::unique_ptr<DirectX::NormalMapEffect>  aEffect, const DirectX::SimpleMath::Vector3 aCamPos, const DirectX::SimpleMath::Vector3 aTargetPos, const float aDimmerVal)
{
    const float fogStartStopGap = 1.0;
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(aCamPos, aTargetPos);

    float fogStart = distanceToTarget - aDimmerVal;
    float fogEnd = distanceToTarget + (fogStartStopGap - aDimmerVal);

    aEffect->SetFogEnabled(true);
    aEffect->SetFogStart(fogEnd);
    aEffect->SetFogEnd(fogStart);
}

void Lighting::UpdateLighting(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer)
{
    const float timeStamp = static_cast<float>(aTimer);

    aEffect->EnableDefaultLighting();


    if (m_currentLightingState == LightingState::LIGHTINGSTATE_JI)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);
            auto time = static_cast<float>(aTimer);
            float yaw = time * 0.4f;
            float pitch = time * 0.7f;
            float roll = time * 1.1f;

            yaw = roll;
            pitch = roll;

            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, -yaw);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(pitch, 0.0, -pitch);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(roll, 0.0, -roll);

            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat2);

            const float lightMod = 0.8;
            light0.x += lightMod;
            light1.x += lightMod;
            light2.x += lightMod;
            light0.Normalize();
            light1.Normalize();
            light2.Normalize();

            light0 *= .3;
            light1 *= .3;
            light2 *= .3;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);
            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_BMW)
    {
        // BMW effects
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            auto time = static_cast<float>(aTimer);

            const float pitch = -time * 1.1f;
            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch + 3.14f, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch + 1.25f, 0.0);

            auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, -pitch, 0.0);

            DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;

            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);

            light0.x += 1.0;
            light0.Normalize();
            light1.x += 1.0;
            light1.Normalize();
            light2.x += 1.0;
            light2.Normalize();

            DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);
            light.x += 1.0;

            light.Normalize();

            float val = 0.0;
            DirectX::SimpleMath::Vector4 test(val, val, val, 1.0);
            aEffect->SetAmbientLightColor(test);

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_STARTSCREEN)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            auto time = static_cast<float>(aTimer);

            float yaw = time * 0.4f;
            float pitch = time * 0.7f;
            float roll = time * 1.1f;

            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, yaw, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);

            auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, -roll, 0.0);

            DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3::UnitZ;

            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);
            light0.x += 1.0;
            light0.Normalize();
            light1.x += 1.0;
            light1.Normalize();
            light2.x += 1.0;
            light2.Normalize();

            quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(roll, 0.0, 0.0);
            DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);

            light0 = light;
            light1 = light;
            light2 = light;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_TEASERSCREEN)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            DirectX::XMVECTORF32 red = DirectX::Colors::Red;
            DirectX::XMVECTORF32 white = DirectX::Colors::White;
            DirectX::XMVECTORF32 blue = DirectX::Colors::Blue;
            DirectX::XMVECTORF32 green = DirectX::Colors::Green;
            DirectX::XMVECTORF32 black = DirectX::Colors::Black;
            DirectX::XMVECTORF32 gray = DirectX::Colors::Gray;

            DirectX::SimpleMath::Vector4 test(0.05, 0.05, 0.05, 0.5);

            aEffect->SetSpecularColor(white);

            aEffect->SetLightSpecularColor(0, red);
            aEffect->SetLightSpecularColor(1, red);
            aEffect->SetLightSpecularColor(2, red);

            aEffect->SetColorAndAlpha(test);
            aEffect->SetEmissiveColor(test);

            const float yaw = cosf(static_cast<float>(timeStamp) * 3.f) * .7f;
            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, 0.0);

            DirectX::SimpleMath::Vector3 axis = DirectX::SimpleMath::Vector3::UnitX;
            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);

            float shift = -0.2;
            light0.y += shift;
            light0.Normalize();
            light1.y += shift;
            light1.Normalize();
            light2.y += shift;
            light2.Normalize();

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);
            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_MANUAL)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightDirection(0, m_lightPos0);
            ilights->SetLightDirection(1, m_lightPos1);
            ilights->SetLightDirection(2, m_lightPos2);
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_NULL)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            DirectX::SimpleMath::Vector3 light = -DirectX::SimpleMath::Vector3::UnitX;
            light.x = -0.8;
            DirectX::SimpleMath::Vector3 light0 = light;
            DirectX::SimpleMath::Vector3 light1 = light;
            DirectX::SimpleMath::Vector3 light2 = light;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }

    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_STARTUP)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            auto time = static_cast<float>(aTimer);

            float yaw = time * 0.4f;
            float pitch = time * 0.7f;
            float roll = time * 1.1f;

            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, yaw, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);

            auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, -roll, 0.0);

            DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;

            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);
            light0.x += 1.0;
            light0.Normalize();
            light1.x += 1.0;
            light1.Normalize();
            light2.x += 1.0;
            light2.Normalize();

            quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(roll, 0.0, 0.0);
            DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);

            light.Normalize();

            light0 = light;
            light1 = light;
            light2 = light;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_)
    {
        aEffect->EnableDefaultLighting();

        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->SetLightEnabled(0, true);
            ilights->SetLightEnabled(1, true);
            ilights->SetLightEnabled(2, true);

            auto time = static_cast<float>(aTimer);

            float yaw = time * 0.4f;
            float pitch = time * 0.7f;
            float roll = time * 11.1f;

            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, pitch, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, yaw, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll, 0.0);

            auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, -roll, 0.0);

            DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;

            DirectX::SimpleMath::Vector3 light0 = XMVector3Rotate(axis, quat0);
            DirectX::SimpleMath::Vector3 light1 = XMVector3Rotate(axis, quat1);
            DirectX::SimpleMath::Vector3 light2 = XMVector3Rotate(axis, quat2);
            float offset = 7.0;
            light0.x -= offset;
            light0.Normalize();
            light1.x -= offset;
            light1.Normalize();
            light2.x -= offset;
            light2.Normalize();

            auto light = -DirectX::SimpleMath::Vector3::UnitX;

            light0 = light;
            light1 = light;
            light2 = light;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
    else if (m_currentLightingState == LightingState::LIGHTINGSTATE_TEST01)
    {
        auto ilights = dynamic_cast<DirectX::IEffectLights*>(aEffect.get());
        if (ilights)
        {
            ilights->EnableDefaultLighting();
            ilights->SetLightEnabled(0, false);
            ilights->SetLightEnabled(1, false);
            ilights->SetLightEnabled(2, false);

            
            const float yaw = cosf(-timeStamp * 1.2f);
            const float roll = cosf(-timeStamp * 1.2f);
            auto quat0 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(-yaw, 0.0, 0.0);
            auto quat1 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, 0.0);
            auto quat2 = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, 0.0, roll);
            auto light0 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat0);
            auto light1 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat1);
            auto light2 = XMVector3Rotate(DirectX::SimpleMath::Vector3::UnitX, quat2);

            auto time = static_cast<float>(aTimer);
            const float roll2 = time * 3.1f;
            auto quat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(0.0, roll2, 0.0);
            DirectX::SimpleMath::Vector3 axis = -DirectX::SimpleMath::Vector3::UnitZ;
            DirectX::SimpleMath::Vector3 light = XMVector3Rotate(axis, quat);
            light.x += 1.0;

            light.Normalize();
            light0 = light;

            light0 = DirectX::SimpleMath::Vector3::UnitX;
            light1 = DirectX::SimpleMath::Vector3::UnitX;
            light2 = DirectX::SimpleMath::Vector3::UnitX;

            ilights->SetLightDirection(0, light0);
            ilights->SetLightDirection(1, light1);
            ilights->SetLightDirection(2, light2);

            ilights->SetAmbientLightColor(DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            m_lightPos0 = light0;
            m_lightPos1 = light1;
            m_lightPos2 = light2;
        }
    }
}

void Lighting::UpdateLightingNormColorTextureVertex(std::shared_ptr<DirectX::NormalMapEffect> aEffect, const double aTimer)
{
    const float timeStamp = static_cast<float>(aTimer);

    aEffect->EnableDefaultLighting();
}

void Lighting::UpdateLightingNormColorVertex2(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer)
{
    const float timeStamp = static_cast<float>(aTimer);

    aEffect->EnableDefaultLighting();
}

void Lighting::UpdateLightingColorVertex3(std::shared_ptr<DirectX::BasicEffect> aEffect, const double aTimer)
{
    const float timeStamp = static_cast<float>(aTimer);

    //aEffect->EnableDefaultLighting();
}

