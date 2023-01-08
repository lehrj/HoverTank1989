#include "pch.h"
#include "ModelController.h"


void ModelController::DrawModel(ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection)
{
    DrawTank(m_playerModel, deviceContext, states, aView, aProjection);
}

void ModelController::DrawTank(TankModel& aModel, ID3D11DeviceContext* deviceContext, const DirectX::CommonStates& states, DirectX::SimpleMath::Matrix aView, DirectX::SimpleMath::Matrix aProjection)
{
    aModel.barrelModel->Draw(deviceContext, states, aModel.barrelWorldMatrix, aView, aProjection);
    aModel.bodyModel->Draw(deviceContext, states, aModel.bodyWorldMatrix, aView, aProjection);
    aModel.turretModel->Draw(deviceContext, states, aModel.turretWorldMatrix, aView, aProjection);

    aModel.barrelModel->Draw(deviceContext, states, aModel.barrelShadowMatrix, aView, aProjection);
    aModel.bodyModel->Draw(deviceContext, states, aModel.bodyShadowMatrix, aView, aProjection);
    aModel.turretModel->Draw(deviceContext, states, aModel.turretShadowMatrix, aView, aProjection);
}

void ModelController::InitializeModel(TankModel& aModel, std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret)
{
    aModel.barrelModel = aBarrel;
    aModel.bodyModel = aBody;
    aModel.turretModel = aTurret;
    
    // body
    aModel.bodyLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.bodyLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;
    aModel.bodyShadowLocalMatrix = aModel.bodyLocalMatrix;
    aModel.bodyShadowTranslationMatrix = DirectX::SimpleMath::Matrix::Identity;

    // turret
    aModel.turretLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    DirectX::SimpleMath::Vector3 turretOffSet = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.5f);
    aModel.turretOffSetMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(turretOffSet);
    DirectX::SimpleMath::Vector3 turretTrans = DirectX::SimpleMath::Vector3(0.250f, 1.52f, 0.0);
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(turretTrans);
    aModel.turretWorldMatrix = aModel.turretLocalMatrix;

    // barrel
    aModel.barrelLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.barrelTransMatrix = aModel.turretLocalMatrix;
    DirectX::SimpleMath::Vector3 barrelTrans = DirectX::SimpleMath::Vector3(0.0f, -0.2f, -1.85f);
    aModel.barrelLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(barrelTrans);
    aModel.barrelLocalMatrix *= aModel.turretOffSetMatrix;
    aModel.barrelWorldMatrix = aModel.barrelLocalMatrix;
    
    aModel.weaponPosLocal = DirectX::SimpleMath::Vector3(0.0, 0.0, -0.9);
    aModel.weaponPosWorld = aModel.weaponPosLocal;

    aModel.weaponDirLocal = DirectX::SimpleMath::Vector3::UnitX;
    aModel.weaponDirWorld = aModel.weaponDirLocal;

    aModel.muzzlePosLocal = aModel.weaponPosLocal;
    aModel.muzzlePosLocal += DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    aModel.localizedMuzzlePos = aModel.muzzlePosLocal;
    aModel.muzzlePosWorld = aModel.muzzlePosLocal;
    aModel.muzzleTransMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.muzzleTransMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.64f));
    aModel.muzzleLocalMatrix = aModel.barrelLocalMatrix;
    aModel.muzzleLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
    aModel.muzzleWorldMatrix = aModel.muzzleLocalMatrix;
}

void ModelController::UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
{
    DirectX::SimpleMath::Matrix updateMat = aAlignment;
    updateMat *= DirectX::SimpleMath::Matrix::CreateTranslation(aPos);
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
    aModel.weaponDirWorld = aModel.weaponDirLocal;
    aModel.weaponDirWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirWorld, muzzleMat);
    aModel.weaponDirWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirWorld, turretMat);
    aModel.weaponDirWorld = DirectX::SimpleMath::Vector3::Transform(aModel.weaponDirWorld, aAlignment);

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
    aModel.muzzleWorldMatrix *= updateMat;

    aModel.muzzlePosWorld = aModel.muzzlePosLocal;
    aModel.muzzlePosWorld = DirectX::SimpleMath::Vector3::Transform(aModel.muzzlePosWorld, aModel.muzzleWorldMatrix);
  
    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();
    DirectX::SimpleMath::Plane groundPlane;
    DirectX::SimpleMath::Vector3 modelPos = aPos;
    bool isPlaneFound = m_environment->GetGroundPlane(groundPlane, modelPos);
    if(isPlaneFound == false)
    {
        // add error handleing
    }

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * 0.1f;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    planeTrans = planeTrans.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans);
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
}

void ModelController::UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const float aAltitude, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
{
    UpdateModel(m_playerModel, aAlignment, aAltitude, aPos, aBarrelPitch, aTurretRotation);
}

void ModelController::InitializePlayerModel(std::shared_ptr<DirectX::Model> aBarrel, std::shared_ptr<DirectX::Model> aBody, std::shared_ptr<DirectX::Model> aTurret)
{
    InitializeModel(m_playerModel, aBarrel, aBody, aTurret);
}

void ModelController::SetEnvironment(Environment const* aEnviron)
{
    m_environment = aEnviron;
}

void ModelController::SetDebugData(std::shared_ptr<DebugData> aDebugPtr)
{
    m_debugData = aDebugPtr;
}