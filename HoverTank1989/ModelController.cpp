#include "pch.h"
#include "ModelController.h"


void ModelController::DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    DrawTank(m_playerModel, deviceContext, states, aView, aProjection, aEffect, aInputLayout);
}

void ModelController::DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection, std::shared_ptr<DirectX::NormalMapEffect> aEffect, Microsoft::WRL::ComPtr<ID3D11InputLayout> aInputLayout)
{
    DirectX::SimpleMath::Vector3 defaultLightDir0 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
    DirectX::SimpleMath::Vector3 defaultLightDir1 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
    DirectX::SimpleMath::Vector3 defaultLightDir2 = DirectX::SimpleMath::Vector3(0.4545195f, -0.7660444f, 0.4545195f);
    
    m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

    DirectX::SimpleMath::Vector3 lightDir0 = defaultLightDir0;
    DirectX::SimpleMath::Vector3 lightDir1 = defaultLightDir1;
    DirectX::SimpleMath::Vector3 lightDir2 = defaultLightDir2;

    lightDir0 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    lightDir1 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    lightDir2 = DirectX::SimpleMath::Vector3::Lerp(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    
    DirectX::SimpleMath::Vector4 color1 = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::SimpleMath::Vector4 color2 = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    aEffect->SetEmissiveColor(color2);
    aEffect->SetLightSpecularColor(0, color2);
    aEffect->SetLightSpecularColor(1, color2);
    aEffect->SetLightSpecularColor(2, color2);

    aEffect->SetLightDiffuseColor(0, color1);
    aEffect->SetLightDiffuseColor(1, color1);
    aEffect->SetLightDiffuseColor(2, color1);

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);
    aEffect->SetWorld(aModel.worldRearGlowCenterMatrix);
    aEffect->SetColorAndAlpha(aModel.rearGlowCenterColor);
    aModel.rearGlowCenterShape->Draw(aEffect.get(), aInputLayout.Get());

    lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(m_playerModel.glowLightDirectionBase, -m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    lightDir1 = DirectX::SimpleMath::Vector3::SmoothStep(m_playerModel.glowLightDirectionBase, -m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);
    lightDir2 = DirectX::SimpleMath::Vector3::SmoothStep(m_playerModel.glowLightDirectionBase, -m_playerModel.glowLightDirectionBase, m_playerModel.glowCenterVal);

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);
    aEffect->SetWorld(aModel.worldFrontGlowCenterMatrix);
    aEffect->SetColorAndAlpha(aModel.frontGlowCenterColor);
    aModel.frontGlowCenterShape->Draw(aEffect.get(), aInputLayout.Get());
    aModel.frontGlowCenterShape->Draw(aModel.worldFrontGlowCenterMatrix, aView, aProjection, aModel.frontGlowCenterColor);

    lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowLeftVal);
    lightDir1 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowLeftVal);
    lightDir2 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowLeftVal);

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);
    aEffect->SetWorld(aModel.worldRearGlowLeftMatrix);
    aEffect->SetColorAndAlpha(aModel.rearGlowLeftColor);
    aModel.rearGlowSideShape->Draw(aEffect.get(), aInputLayout.Get());

    lightDir0 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir0, m_playerModel.glowLightDirectionBase, m_playerModel.glowRightVal);
    lightDir1 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir1, m_playerModel.glowLightDirectionBase, m_playerModel.glowRightVal);
    lightDir2 = DirectX::SimpleMath::Vector3::SmoothStep(defaultLightDir2, m_playerModel.glowLightDirectionBase, m_playerModel.glowRightVal);

    aEffect->SetLightDirection(0, lightDir0);
    aEffect->SetLightDirection(1, lightDir1);
    aEffect->SetLightDirection(2, lightDir2);
    aEffect->SetWorld(aModel.worldRearGlowRightMatrix);
    aEffect->SetColorAndAlpha(aModel.rearGlowRightColor);
    aModel.rearGlowSideShape->Draw(aEffect.get(), aInputLayout.Get());

    m_environment->GetLightDirectionalVectors(defaultLightDir0, defaultLightDir1, defaultLightDir2);

    Utility::GetDispersedLightDirectionsRotation(-DirectX::SimpleMath::Vector3::UnitY, Utility::ToRadians(45.0), Utility::ToRadians(0.0f), defaultLightDir0, defaultLightDir1, defaultLightDir2);

    aModel.bodyModel->UpdateEffects([&](DirectX::IEffect* effect)
    {
        auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
        if (lights)
        {
            lights->EnableDefaultLighting();
            lights->SetLightEnabled(0, true);
            lights->SetLightEnabled(1, true);
            lights->SetLightEnabled(2, true);

            lights->SetLightDirection(0, defaultLightDir0);
            lights->SetLightDirection(1, defaultLightDir1);
            lights->SetLightDirection(2, defaultLightDir2);
        }
    });
    aModel.bodyModel->Modified();

    aEffect->SetLightDirection(0, defaultLightDir0);
    aEffect->SetLightDirection(1, defaultLightDir1);
    aEffect->SetLightDirection(2, defaultLightDir2);

    aModel.bodyModel->Draw(deviceContext, states, aModel.bodyWorldMatrix, aView, aProjection);
  
    aModel.turretModel->UpdateEffects([&](DirectX::IEffect* effect)
        {
            auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
            if (lights)
            {
                lights->EnableDefaultLighting();
                lights->SetLightEnabled(0, true);
                lights->SetLightEnabled(1, true);
                lights->SetLightEnabled(2, true);

                lights->SetLightDirection(0, defaultLightDir0);
                lights->SetLightDirection(1, defaultLightDir1);
                lights->SetLightDirection(2, defaultLightDir2);
            }
        });
    //aModel.turretModel->Modified();

    aEffect->SetLightDirection(0, defaultLightDir0);
    aEffect->SetLightDirection(1, defaultLightDir1);
    aEffect->SetLightDirection(2, defaultLightDir2);

    aModel.turretModel->Draw(deviceContext, states, aModel.turretWorldMatrix, aView, aProjection);
    aModel.barrelModel->Draw(deviceContext, states, aModel.barrelWorldMatrix, aView, aProjection);
   
    aModel.bodyModel->UpdateEffects([&](DirectX::IEffect* effect)
    {
        auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
        if (lights)
        {
            lights->SetLightEnabled(0, false);
            lights->SetLightEnabled(1, false);
            lights->SetLightEnabled(2, false);
            lights->SetAmbientLightColor(DirectX::Colors::Black);
        }
    });
    aModel.bodyModel->Modified();
    aModel.barrelModel->UpdateEffects([&](DirectX::IEffect* effect)
        {
            auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
            if (lights)
            {
                lights->SetLightEnabled(0, false);
                lights->SetLightEnabled(1, false);
                lights->SetLightEnabled(2, false);
                lights->SetAmbientLightColor(DirectX::Colors::Black);
            }
        });
    aModel.barrelModel->Modified();

    aModel.turretModel->UpdateEffects([&](DirectX::IEffect* effect)
        {
            auto lights = dynamic_cast<DirectX::IEffectLights*>(effect);
            if (lights)
            {
                lights->SetLightEnabled(0, false);
                lights->SetLightEnabled(1, false);
                lights->SetLightEnabled(2, false);
                lights->SetAmbientLightColor(DirectX::Colors::Black);
            }
        });
    aModel.turretModel->Modified();

    aModel.bodyModel->Draw(deviceContext, states, aModel.bodyShadowMatrix, aView, aProjection);
    aModel.barrelModel->Draw(deviceContext, states, aModel.barrelShadowMatrix, aView, aProjection);
    aModel.turretModel->Draw(deviceContext, states, aModel.turretShadowMatrix, aView, aProjection);
}

void ModelController::InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    aModel.barrelModel = aBarrel;
    aModel.bodyModel = aBody;
    aModel.turretModel = aTurret;
    
    DirectX::SimpleMath::Vector3 cogOffset = m_centerOfMassOffset;
    
    // body
    aModel.bodyLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.bodyLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    aModel.bodyLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;
    aModel.bodyShadowLocalMatrix = aModel.bodyLocalMatrix;
    aModel.bodyShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;

    // turret
    aModel.turretLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    const DirectX::SimpleMath::Vector3 turretOffSet = m_turretPosOffset;
    aModel.turretOffSetMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(turretOffSet);
    const DirectX::SimpleMath::Vector3 turretTrans = m_turretTrans;
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(turretTrans);
    aModel.turretWorldMatrix = aModel.turretLocalMatrix;

    // barrel
    aModel.barrelLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.barrelTransMatrix = aModel.turretLocalMatrix;
    const DirectX::SimpleMath::Vector3 barrelTrans = m_barrelTrans;
    aModel.barrelLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(barrelTrans);
    aModel.barrelLocalMatrix *= aModel.turretOffSetMatrix;
    aModel.barrelWorldMatrix = aModel.barrelLocalMatrix;
    
    aModel.weaponPosLocal = m_barrelPosOffset;
    aModel.weaponPosWorld = aModel.weaponPosLocal;

    aModel.weaponDirLocal = DirectX::SimpleMath::Vector3::UnitX;
    aModel.weaponDirWorld = aModel.weaponDirLocal;
    aModel.weaponUpLocal = DirectX::SimpleMath::Vector3::UnitY;
    aModel.weaponUpWorld = aModel.weaponUpLocal;

    aModel.muzzlePosLocal = aModel.weaponPosLocal;
    aModel.muzzlePosLocal += DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    aModel.localizedMuzzlePos = aModel.muzzlePosLocal;
    aModel.muzzlePosWorld = aModel.muzzlePosLocal;
    aModel.muzzleTransMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.muzzleTransMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.64f));
    aModel.muzzleLocalMatrix = aModel.barrelLocalMatrix;
    aModel.muzzleLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
    aModel.muzzleWorldMatrix = aModel.muzzleLocalMatrix;

    // missile tubes
    const float missileTubeVerticalRot = m_missileTubeVerticalRot;
    const float missileTubeHorizontalRot = m_missileTubeHorizontalRot;

    aModel.localMissileTubeLeftDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeLeftDir, DirectX::SimpleMath::Matrix::CreateRotationY(missileTubeHorizontalRot));
    aModel.localMissileTubeLeftDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeLeftDir, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));

    //aModel.localMissileTubeRightDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightDir, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));
    //aModel.localMissileTubeRightDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightDir, DirectX::SimpleMath::Matrix::CreateRotationY(-missileTubeHorizontalRot));
    
    aModel.localMissileTubeRightDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightDir, DirectX::SimpleMath::Matrix::CreateRotationY(-missileTubeHorizontalRot));
    aModel.localMissileTubeRightDir = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightDir, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));

    aModel.localMissileTubeLeftUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeLeftUp, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));
    aModel.localMissileTubeLeftUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeLeftUp, DirectX::SimpleMath::Matrix::CreateRotationY(missileTubeHorizontalRot));

    //aModel.localMissileTubeRightUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightUp, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));
    //aModel.localMissileTubeRightUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightUp, DirectX::SimpleMath::Matrix::CreateRotationY(-missileTubeHorizontalRot));

    aModel.localMissileTubeRightUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightUp, DirectX::SimpleMath::Matrix::CreateRotationZ(missileTubeVerticalRot));
    aModel.localMissileTubeRightUp = DirectX::SimpleMath::Vector3::TransformNormal(aModel.localMissileTubeRightUp, DirectX::SimpleMath::Matrix::CreateRotationY(-missileTubeHorizontalRot));

    //aModel.localMissileTubeLeftUp = aModel.localMissileTubeRightUp;
    //aModel.localMissileTubeLeftDir = aModel.localMissileTubeRightDir;

    // front center glow
    DirectX::SimpleMath::Vector3 frontGlowCenterSize = DirectX::SimpleMath::Vector3(0.2f, 0.5f, 1.75f);
    aModel.frontGlowCenterShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), frontGlowCenterSize);
    DirectX::SimpleMath::Vector3 frontGlowCenterTranslation = DirectX::SimpleMath::Vector3(2.666f, -0.35f, 0.0f);
    float glowFrontRotationZ = Utility::ToRadians(41.0);
    aModel.localFrontGlowCenterMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localFrontGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(glowFrontRotationZ);
    aModel.localFrontGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(frontGlowCenterTranslation);
    aModel.localFrontGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.worldFrontGlowCenterMatrix = aModel.localFrontGlowCenterMatrix;

    aModel.frontGlowCenterColor = DirectX::Colors::Red;

    // rear center glow
    DirectX::SimpleMath::Vector3 rearGlowCenterSize = DirectX::SimpleMath::Vector3(0.2f, 0.5f, 0.6f);
    aModel.rearGlowCenterShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearGlowCenterSize);
    DirectX::SimpleMath::Vector3 rearGlowCenterTranslation = DirectX::SimpleMath::Vector3(-2.35f, -0.15f, 0.0f);
    float glowRotationZ = Utility::ToRadians(21.8f);
    aModel.localRearGlowCenterMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(glowRotationZ);
    aModel.localRearGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearGlowCenterTranslation);
    aModel.localRearGlowCenterMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.worldRearGlowCenterMatrix = aModel.localRearGlowCenterMatrix;

    aModel.rearGlowCenterColor = DirectX::Colors::Red;

    // rear left glow
    DirectX::SimpleMath::Vector3 rearGlowSideSize = DirectX::SimpleMath::Vector3(0.2f, 0.8f, 0.98f);
    aModel.rearGlowSideShape = DirectX::GeometricPrimitive::CreateBox(aContext.Get(), rearGlowSideSize);
    DirectX::SimpleMath::Vector3 rearGlowLeftTranslation = DirectX::SimpleMath::Vector3(-2.26f, -0.15f, -0.83f);

    aModel.localRearGlowLeftMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearGlowLeftMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(glowRotationZ);
    aModel.localRearGlowLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearGlowLeftTranslation);
    aModel.localRearGlowLeftMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.worldRearGlowLeftMatrix = aModel.localRearGlowLeftMatrix;

    aModel.rearGlowLeftColor = DirectX::Colors::Red;

    // rear right glow
    DirectX::SimpleMath::Vector3 rearGlowRightTranslation = rearGlowLeftTranslation;
    rearGlowRightTranslation.z *= -1.0f;

    aModel.localRearGlowRightMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.localRearGlowRightMatrix *= DirectX::SimpleMath::Matrix::CreateRotationZ(glowRotationZ);
    aModel.localRearGlowRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(rearGlowRightTranslation);
    aModel.localRearGlowRightMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(cogOffset);
    aModel.worldRearGlowRightMatrix = aModel.localRearGlowRightMatrix;

    aModel.rearGlowRightColor = DirectX::Colors::Yellow;

}

void ModelController::InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> aContext)
{
    InitializeModel(m_playerModel, aBarrel, aBody, aTurret, aContext);
}

void ModelController::UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane)
{ 
    //////// Update Glow Colors;
    float baseColor = m_playerModel.glowColorMin.x;
    float redBase = m_playerModel.glowColorMin.x;
    float blueBase = m_playerModel.glowColorMin.y;
    float greenBase = m_playerModel.glowColorMin.z;

    float redMaxMod = m_playerModel.glowColorMax.x - baseColor;
    float blueMaxMod = m_playerModel.glowColorMax.y - baseColor;
    float greenMaxMod = m_playerModel.glowColorMax.z - baseColor;

    // center rear
    float redVal = baseColor + (m_playerModel.glowCenterVal * redMaxMod);
    float blueVal = baseColor + (m_playerModel.glowCenterVal * blueMaxMod);
    float greenVal = baseColor + (m_playerModel.glowCenterVal * greenMaxMod);
    m_playerModel.rearGlowCenterColor = DirectX::SimpleMath::Vector4(redVal, blueVal, greenVal, 1.0f);

    // center front
    redVal = baseColor + (m_playerModel.glowCenterVal * redMaxMod);
    blueVal = baseColor + (m_playerModel.glowCenterVal * blueMaxMod);
    greenVal = baseColor + (m_playerModel.glowCenterVal * greenMaxMod);
    m_playerModel.frontGlowCenterColor = DirectX::SimpleMath::Vector4(redVal, blueVal, greenVal, 1.0f);

    // left
    redVal = baseColor + (m_playerModel.glowLeftVal * redMaxMod);
    blueVal = baseColor + (m_playerModel.glowLeftVal * blueMaxMod);
    greenVal = baseColor + (m_playerModel.glowLeftVal * greenMaxMod);
    m_playerModel.rearGlowLeftColor = DirectX::SimpleMath::Vector4(redVal, blueVal, greenVal, 1.0f);

    // right
    redVal = baseColor + (m_playerModel.glowRightVal * redMaxMod);
    blueVal = baseColor + (m_playerModel.glowRightVal * blueMaxMod);
    greenVal = baseColor + (m_playerModel.glowRightVal * greenMaxMod);
    m_playerModel.rearGlowRightColor = DirectX::SimpleMath::Vector4(redVal, blueVal, greenVal, 1.0f);

    //////// Update Shapes
    DirectX::SimpleMath::Matrix updateMat = aAlignment;
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);

    aModel.worldFrontGlowCenterMatrix = aModel.localFrontGlowCenterMatrix;
    aModel.worldFrontGlowCenterMatrix *= updateMat;

    aModel.worldRearGlowCenterMatrix = aModel.localRearGlowCenterMatrix;
    aModel.worldRearGlowCenterMatrix *= updateMat;

    aModel.worldRearGlowLeftMatrix = aModel.localRearGlowLeftMatrix;
    aModel.worldRearGlowLeftMatrix *= updateMat;

    aModel.worldRearGlowRightMatrix = aModel.localRearGlowRightMatrix;
    aModel.worldRearGlowRightMatrix *= updateMat;

    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;
    aModel.bodyWorldMatrix *= updateMat;

    DirectX::SimpleMath::Matrix turretMat = DirectX::SimpleMath::Matrix::CreateRotationY(aTurretRotation);
    aModel.turretWorldMatrix = aModel.turretOffSetMatrix;
    aModel.turretWorldMatrix *= turretMat;
    aModel.turretWorldMatrix *= aModel.turretLocalMatrix;
    aModel.turretWorldMatrix *= updateMat;

    DirectX::SimpleMath::Matrix barrelMat = DirectX::SimpleMath::Matrix::CreateRotationX(aBarrelPitch);
    aModel.barrelWorldMatrix = barrelMat;
    aModel.barrelWorldMatrix *= aModel.barrelLocalMatrix;
    aModel.barrelWorldMatrix *= turretMat;
    aModel.barrelWorldMatrix *= aModel.barrelTransMatrix; 
    aModel.barrelWorldMatrix *= updateMat; 

    DirectX::SimpleMath::Matrix muzzleMat = DirectX::SimpleMath::Matrix::CreateRotationZ(aBarrelPitch);
    aModel.weaponDirLocal = DirectX::SimpleMath::Vector3::UnitX;
    aModel.weaponDirLocal = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirLocal, muzzleMat);
    aModel.weaponDirLocal = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirLocal, turretMat);
    
    aModel.weaponDirWorld = aModel.weaponDirLocal;
    aModel.weaponDirWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirWorld, aAlignment);

    aModel.weaponUpLocal = DirectX::SimpleMath::Vector3::UnitY;
    aModel.weaponUpLocal = DirectX::SimpleMath::Vector3::Transform(aModel.weaponUpLocal, muzzleMat);
    aModel.weaponUpLocal = DirectX::SimpleMath::Vector3::Transform(aModel.weaponUpLocal, turretMat);

    aModel.weaponUpWorld = aModel.weaponUpLocal;
    aModel.weaponUpWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponUpWorld, aAlignment);

    aModel.weaponPosWorld = aModel.weaponPosLocal;
    aModel.weaponPosWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponPosWorld, aModel.barrelWorldMatrix);

    aModel.muzzleWorldMatrix = aModel.muzzleLocalMatrix;
    aModel.muzzleWorldMatrix = aModel.barrelWorldMatrix;

    aModel.muzzleWorldMatrix = aModel.muzzleTransMatrix;
    aModel.muzzleWorldMatrix *= barrelMat;
    aModel.muzzleWorldMatrix *= aModel.muzzleLocalMatrix;
    aModel.muzzleWorldMatrix *= turretMat;
    aModel.muzzleWorldMatrix *= aModel.barrelTransMatrix;
    aModel.localizedMuzzlePos = DirectX::SimpleMath::Vector3::Transform(aModel.muzzlePosLocal, aModel.muzzleWorldMatrix);

    aModel.targetingMatrix = aModel.muzzleWorldMatrix;
    aModel.muzzleWorldMatrix *= updateMat;

    aModel.muzzlePosWorld = aModel.muzzlePosLocal;
    aModel.muzzlePosWorld = DirectX::SimpleMath::Vector3::Transform(aModel.muzzlePosWorld, aModel.muzzleWorldMatrix);
  
    // missile tubes
    DirectX::SimpleMath::Matrix missileTubMat = DirectX::SimpleMath::Matrix::Identity;
    missileTubMat = aModel.turretOffSetMatrix;
    missileTubMat *= turretMat;
    missileTubMat *= aModel.turretLocalMatrix;
    DirectX::SimpleMath::Matrix localizedMissileTubeMat = missileTubMat;
    missileTubMat *= updateMat;
    aModel.worldMissileTubeLeftPos = aModel.localMissileTubeLeftPos;
    aModel.worldMissileTubeLeftPos = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeLeftPos, missileTubMat);
    aModel.worldMissileTubeRightPos = aModel.localMissileTubeRightPos;
    aModel.worldMissileTubeRightPos = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeRightPos, missileTubMat);

    aModel.localizedTubeLeftPos = aModel.localMissileTubeLeftPos;
    aModel.localizedTubeLeftPos = DirectX::SimpleMath::Vector3::Transform(aModel.localizedTubeLeftPos, localizedMissileTubeMat);
    aModel.localizedTubeRightPos = aModel.localMissileTubeRightPos;
    aModel.localizedTubeRightPos = DirectX::SimpleMath::Vector3::Transform(aModel.localizedTubeRightPos, localizedMissileTubeMat);

    aModel.worldMissileTubeLeftDir = aModel.localMissileTubeLeftDir;
    aModel.worldMissileTubeLeftDir = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeLeftDir, turretMat);
    aModel.worldMissileTubeLeftDir = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeLeftDir, aAlignment);

    aModel.worldMissileTubeRightDir = aModel.localMissileTubeRightDir;
    aModel.worldMissileTubeRightDir = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeRightDir, turretMat);
    aModel.worldMissileTubeRightDir = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeRightDir, aAlignment);

    aModel.worldMissileTubeLeftUp = aModel.localMissileTubeLeftUp;
    aModel.worldMissileTubeLeftUp = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeLeftUp, turretMat);
    aModel.worldMissileTubeLeftUp = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeLeftUp, aAlignment);

    aModel.worldMissileTubeRightUp = aModel.localMissileTubeRightUp;
    aModel.worldMissileTubeRightUp = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeRightUp, turretMat);
    aModel.worldMissileTubeRightUp = DirectX::SimpleMath::Vector3::Transform(aModel.worldMissileTubeRightUp, aAlignment);

    aModel.turretLocalMissileTubeLeftUp = aModel.localMissileTubeLeftUp;
    aModel.turretLocalMissileTubeLeftUp = DirectX::SimpleMath::Vector3::Transform(aModel.turretLocalMissileTubeLeftUp, turretMat);

    aModel.turretLocalMissileTubeRightUp = aModel.localMissileTubeRightUp;
    aModel.turretLocalMissileTubeRightUp = DirectX::SimpleMath::Vector3::Transform(aModel.turretLocalMissileTubeRightUp, turretMat);

    aModel.turretLocalMissileTubeLeftDir = aModel.localMissileTubeLeftDir;
    aModel.turretLocalMissileTubeLeftDir = DirectX::SimpleMath::Vector3::Transform(aModel.turretLocalMissileTubeLeftDir, turretMat);

    aModel.turretLocalMissileTubeRightDir = aModel.localMissileTubeRightDir;
    aModel.turretLocalMissileTubeRightDir = DirectX::SimpleMath::Vector3::Transform(aModel.turretLocalMissileTubeRightDir, turretMat);

    // shadows
    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();
    DirectX::SimpleMath::Plane groundPlane = aPlane;
    DirectX::SimpleMath::Vector3 modelPos = aPos;

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * aModel.zOffsetShadowMod;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    planeTrans = planeTrans.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans);
    groundPlane.Normalize();
    DirectX::SimpleMath::Matrix shadowMat = DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    const float maxShadowRange = m_environment->GetMaxShadowCastRange();
    float shadowScale;
    float inverseShadowScale;
    if (aAltitude > maxShadowRange)
    {
        shadowScale = 0.0f;
        inverseShadowScale = 1.0f;
    }
    else
    {
        inverseShadowScale = aAltitude / maxShadowRange;
        shadowScale = 1.0f - inverseShadowScale;

        const float tol = 0.00001f;
        if (inverseShadowScale < tol)
        {
            inverseShadowScale = tol;
        }
    }
    DirectX::SimpleMath::Matrix shadowScaleMat = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(shadowScale, shadowScale, shadowScale));

    aModel.bodyShadowMatrix = aModel.bodyShadowLocalMatrix;
    aModel.bodyShadowMatrix *= aModel.bodyShadowTranslationMatrix * inverseShadowScale;
    aModel.bodyShadowMatrix *= shadowScaleMat;
    aModel.bodyShadowMatrix *= updateMat;
    aModel.bodyShadowMatrix *= shadowMat;

    aModel.barrelShadowMatrix = barrelMat;
    aModel.barrelShadowMatrix *= aModel.barrelLocalMatrix;
    aModel.barrelShadowMatrix *= turretMat;
    aModel.barrelShadowMatrix *= aModel.barrelTransMatrix * inverseShadowScale;
    aModel.barrelShadowMatrix *= shadowScaleMat;
    aModel.barrelShadowMatrix *= updateMat;
    aModel.barrelShadowMatrix *= shadowMat;

    aModel.turretShadowMatrix = aModel.turretOffSetMatrix;
    aModel.turretShadowMatrix *= turretMat;
    aModel.turretShadowMatrix *= aModel.turretLocalMatrix;
    aModel.turretShadowMatrix *= shadowScaleMat;
    aModel.turretShadowMatrix *= updateMat;
    aModel.turretShadowMatrix *= shadowMat;


    //m_debugData->ToggleDebugOnOverRide();
    auto leftTubePos = m_playerModel.localizedTubeLeftPos;
    leftTubePos = DirectX::SimpleMath::Vector3::Transform(leftTubePos, aAlignment);
    leftTubePos += aPos;

    auto rightTubePos = m_playerModel.localizedTubeRightPos;
    rightTubePos = DirectX::SimpleMath::Vector3::Transform(rightTubePos, aAlignment);
    rightTubePos += aPos;

    m_debugData->PushDebugLinePositionIndicator(leftTubePos, 4.0f, 0.0f, DirectX::Colors::Red);
    m_debugData->PushDebugLinePositionIndicator(rightTubePos, 4.0f, 0.0f, DirectX::Colors::Red);

    m_debugData->ToggleDebugOff();
}

void ModelController::UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation, const DirectX::SimpleMath::Plane aPlane)
{
    UpdateModel(m_playerModel, aAlignment, aAltitude, aPos, aBarrelPitch, aTurretRotation, aPlane);
}

void ModelController::SetEnvironment(Environment const* aEnviron)
{
    m_environment = aEnviron;
}

void ModelController::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}

void ModelController::SetGlowColors(const DirectX::SimpleMath::Vector4 aColorCenter, const DirectX::SimpleMath::Vector4 aColorLeft, const DirectX::SimpleMath::Vector4 aColorRight, const DirectX::SimpleMath::Vector3 aLightDir)
{
    m_playerModel.rearGlowCenterColor = aColorCenter;
    m_playerModel.rearGlowLeftColor = aColorLeft;
    m_playerModel.rearGlowRightColor = aColorRight;
    m_playerModel.glowLightDirectionBase = aLightDir;
}

void ModelController::SetGlowVals(const float aCenterVal, const float aLeftVal, const float aRightVal, const DirectX::SimpleMath::Vector3 aPos, const DirectX::SimpleMath::Vector3 aDir, const float aTimeStep)
{
    const float maxValDelta = 0.8f * aTimeStep;
    const float deltaCenter = aCenterVal - m_playerModel.glowCenterVal;
    if (abs(deltaCenter) > maxValDelta)
    {
        if (deltaCenter > 0.0f)
        {
            m_playerModel.glowCenterVal += maxValDelta;
        }
        else
        {
            m_playerModel.glowCenterVal -= maxValDelta;
        }
    }
    else
    {
        m_playerModel.glowCenterVal = aCenterVal;
    }
    const float deltaLeft = aLeftVal - m_playerModel.glowLeftVal;
    if (abs(deltaLeft) > maxValDelta)
    {
        if (deltaLeft > 0.0f)
        {
            m_playerModel.glowLeftVal += maxValDelta;
        }
        else
        {
            m_playerModel.glowLeftVal -= maxValDelta;
        }
    }
    else
    {
        m_playerModel.glowLeftVal = aLeftVal;
    }
    const float deltaRight = aRightVal - m_playerModel.glowRightVal;
    if (abs(deltaRight) > maxValDelta)
    {
        if (deltaRight > 0.0f)
        {
            m_playerModel.glowRightVal += maxValDelta;
        }
        else
        {
            m_playerModel.glowRightVal -= maxValDelta;
        }
    }
    else
    {
        m_playerModel.glowRightVal = aRightVal;
    }
    m_testPos = aPos;
    m_playerModel.glowLightDirectionBase = aDir;
}