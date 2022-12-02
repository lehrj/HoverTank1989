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
    
    aModel.bodyLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.bodyLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    aModel.bodyWorldMatrix = aModel.bodyLocalMatrix;

    aModel.turretLocalMatrix = DirectX::SimpleMath::Matrix::Identity;
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(-90.0f));
    DirectX::SimpleMath::Vector3 turretOffSet = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.5f);
    aModel.turretOffSetMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(turretOffSet);
    DirectX::SimpleMath::Vector3 turretTrans = DirectX::SimpleMath::Vector3(0.250f, 1.52f, 0.0);
    aModel.turretLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(turretTrans);
    aModel.turretWorldMatrix = aModel.turretLocalMatrix;

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
    //aModel.muzzleTransMatrix = aModel.barrelLocalMatrix;
    //aModel.muzzleTransMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -5.0f));
    aModel.muzzleTransMatrix = DirectX::SimpleMath::Matrix::Identity;
    //aModel.muzzleTransMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -5.41f));
    aModel.muzzleTransMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.64f));
    aModel.muzzleLocalMatrix = aModel.barrelLocalMatrix;
    aModel.muzzleLocalMatrix *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));
    aModel.muzzleWorldMatrix = aModel.muzzleLocalMatrix;
}

void ModelController::UpdateModel(TankModel& aModel, const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
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

    DirectX::SimpleMath::Vector3 test1;
    DirectX::SimpleMath::Vector3 test2;
    DirectX::SimpleMath::Vector3 test3;
    DirectX::SimpleMath::Quaternion quat1;
    aModel.turretWorldMatrix.Decompose(test1, quat1, test2);

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
    //aModel.muzzlePosWorld = aModel.weaponPosWorld;
    aModel.muzzlePosWorld = DirectX::SimpleMath::Vector3::Transform(aModel.muzzlePosWorld, aModel.muzzleWorldMatrix);

    
    /// /////////////////////////////
    DirectX::SimpleMath::Vector3 vert1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 vert2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 vert3 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 testPos = aPos;
    bool isTriFound = m_environment->GetTerrainTriangleData(vert1, vert2, vert3, testPos);

    //DirectX::SimpleMath::Vector3 lightDir = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector3 lightDir = m_environment->GetLightDirectionPrime();
    //DirectX::SimpleMath::Plane groundPlane(0.0f, 1.0f, 0.0f, 0.0f);
    //DirectX::SimpleMath::Plane groundPlane(vert1, vert2, vert3);
    //DirectX::SimpleMath::Plane groundPlane(vert3, vert2, vert1);
    DirectX::SimpleMath::Plane groundPlane;// (vert3, vert2, vert1);
    bool isPlaneFound = m_environment->GetGroundPlane(groundPlane, testPos);

    DirectX::SimpleMath::Vector3 zFightOffSet = groundPlane.Normal() * 0.1f;
    DirectX::SimpleMath::Matrix planeTrans = DirectX::SimpleMath::Matrix::Identity;
    //planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0.0f, -2.4f, 0.0f));
    planeTrans *= DirectX::SimpleMath::Matrix::CreateTranslation(zFightOffSet);
    DirectX::SimpleMath::Matrix planeTrans2 = planeTrans;
    planeTrans2 = planeTrans2.Transpose();
    groundPlane = DirectX::SimpleMath::Plane::Transform(groundPlane, planeTrans2);

    //DirectX::SimpleMath::Matrix shadowMat = m_vehicleStruct00.npcModel.worldModelMatrix;
    aModel.barrelShadowMatrix = aModel.barrelWorldMatrix;
    aModel.barrelShadowMatrix *= DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    aModel.bodyShadowMatrix = aModel.bodyWorldMatrix;
    aModel.bodyShadowMatrix *= DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);

    aModel.turretShadowMatrix = aModel.turretWorldMatrix;
    aModel.turretShadowMatrix *= DirectX::SimpleMath::Matrix::CreateShadow(lightDir, groundPlane);


    //DirectX::SimpleMath::Vector3 dir1 = DirectX::SimpleMath::Vector3(-0.5265408f, -0.5735765f, -0.6275069f);
    //DirectX::SimpleMath::Vector3 dir2 = DirectX::SimpleMath::Vector3(0.7198464f, 0.3420201f, 0.6040227f);
    //DirectX::SimpleMath::Vector3 dir3 = DirectX::SimpleMath::Vector3(.4545195f, -0.7660444f, 0.4545195f);

    DirectX::SimpleMath::Vector3 dir1 = DirectX::SimpleMath::Vector3(-1.0f, -2.0, 0.0f);
    dir1.Normalize();
    DirectX::SimpleMath::Vector3 dir2 = DirectX::SimpleMath::Vector3(1.0f, -2.0f, 1.0f);
    dir2.Normalize();
    DirectX::SimpleMath::Vector3 dir3 = DirectX::SimpleMath::Vector3(1.0f, -2.0f, -1.0f);
    dir3.Normalize();

    m_debugData->DebugPushTestLine(aPos, -dir1, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(aPos, -dir2, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    m_debugData->DebugPushTestLine(aPos, -dir3, 10.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void ModelController::UpdatePlayerModel(const DirectX::SimpleMath::Matrix aAlignment, const DirectX::SimpleMath::Vector3 aPos, const float aBarrelPitch, const float aTurretRotation)
{
    UpdateModel(m_playerModel, aAlignment, aPos, aBarrelPitch, aTurretRotation);
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