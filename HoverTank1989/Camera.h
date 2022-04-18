#pragma once
#include "StepTimer.h"
#include "Utility.h"
#include "Vehicle.h"
#include "Environment.h"


enum class CameraState
{
    CAMERASTATE_TRANSITION,
    CAMERASTATE_FIRSTPERSON,
    CAMERASTATE_PRESWINGVIEW,
    CAMERASTATE_PROJECTILEFLIGHTVIEW,
    CAMERASTATE_SWINGVIEW,
    CAMERASTATE_RESET,
    CAMERASTATE_TRANSTONEWSHOT,
    CAMERASTATE_FOLLOWVEHICLE,
    CAMERASTATE_SPRINGCAMERA,
    CAMERASTATE_SPINCAMERA,
    CAMERASTATE_TESTCAMERA01, 
    CAMERASTATE_TESTCAMERA02,
    CAMERASTATE_INTRO,
    CAMERASTATE_STARTSCREEN,
    CAMERASTATE_GAMEPLAYSTARTSPIN,
};

// spring camera target
struct Target
{
    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 forward = -DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
};

// Class to manage game camera
class Camera
{
public:
    Camera();
    Camera(int aWidth, int aHeight);
    ~Camera();

    CameraState GetCameraState() const { return m_cameraState; };
    float GetAimTurnRate() const { return m_aimTurnRate; };

    DirectX::SimpleMath::Vector3 GetHomePos() const { return m_homePosition; };
    float GetPitch() const { return m_pitch; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_position; };
    DirectX::SimpleMath::Vector3 GetPreSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirection);
    DirectX::SimpleMath::Vector3 GetPreSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    DirectX::SimpleMath::Matrix GetProjectionMatrix() const { return m_projectionMatrix; };
    DirectX::SimpleMath::Vector3 GetSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    DirectX::SimpleMath::Vector3 GetSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    DirectX::SimpleMath::Matrix GetViewMatrix() const { return m_viewMatrix; };
    DirectX::SimpleMath::Vector3 GetTargetPos() const { return m_target; };
    DirectX::SimpleMath::Vector3 GetUp() const { return m_up; };
    float GetViewPlaneNear() const { return m_nearPlane; };
    float GetViewPlaneFar() const { return m_farPlane; };
    float GetYaw() const { return m_yaw; };
    void InintializePreSwingCamera(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    bool IsCameraAtDestination();
    void OnResize(uint32_t aWidth, uint32_t aHeight);

    void Reset();
    void ResetCameraTransition(DX::StepTimer const& aTimer);
    void ResetIsCameraAtDestination() { m_isCameraAtDestination = false; };
    void ReverseTransitionDirection();
    void Rotate(DirectX::SimpleMath::Vector3 aAxis, float aDegrees); //Pavel
    void RotateAtSpeed(float aDx, float aDy);  //Chili
    void RotateCounterClockWise();
    void RotateClockWise();
    void SetCameraEnvironment(const Environment* aEnviron);
    void SetCameraState(const CameraState aCameraState);
    void SetDestinationPos(const DirectX::SimpleMath::Vector3 aDestPos);

    void SetFollowCamDirection(const DirectX::SimpleMath::Vector3 aDirection);
    void SetFollowCamPos(const DirectX::SimpleMath::Vector3 aPos);
    void SetFollowCamTarget(const DirectX::SimpleMath::Vector3 aTarg);   
    void SetFollowCamUp(const DirectX::SimpleMath::Vector3 aUp);

    void SetHomePos(const DirectX::SimpleMath::Vector3 aHomePos);
    void SetTargetPos(const DirectX::SimpleMath::Vector3 aTarget);
    void SetPos(const DirectX::SimpleMath::Vector3 aPos);
    void SetUpPos(const DirectX::SimpleMath::Vector3 aPos);

    void SetCameraEndPos(DirectX::SimpleMath::Vector3 aEndPos);   
    void SetCameraStartPos(DirectX::SimpleMath::Vector3 aStartPos);
    void SetSpinCameraStart();
    void SetSpinCameraStartGamePlayStart(const float aTime);
    void SetTargetEndPos(DirectX::SimpleMath::Vector3 aEndPos);
    void SetTargetStartPos(DirectX::SimpleMath::Vector3 aStartPos);
    void SetTransitionSpeed(const float aSpeed);
    
    void SetVehicleFocus(const Vehicle* aVehicle);
    void SpinClockwise(float aRotation);
    void SpinCounterClockwise(float aRotation);
    void TranslateAtSpeed(DirectX::SimpleMath::Vector3 aTranslation); //Chili

    
    void UpdateCamera(DX::StepTimer const& aTimer);
    void UpdateFirstPersonCamera();
    void UpdatePitchYaw(const float aPitch, const float aYaw);
    void UpdatePos(const float aX, const float aY, const float aZ);
    
    void UpdateTransitionCamera(DX::StepTimer const& aTimer);
    
    void UpdateTimer(DX::StepTimer const& aTimer) { m_cameraTimer = aTimer; };
    DX::StepTimer GetCameraTimer() { return m_cameraTimer; };

    void YawSpin(float aTurn);
    void TurnAroundPoint(float aTurn, DirectX::SimpleMath::Vector3 aCenterPoint);
    void TurnEndPosAroundPoint(float aTurn, DirectX::SimpleMath::Vector3 aCenterPoint);

private:
    void InitializeOrthoganalMatrix(); //Pavel
    void InitializeProjectionMatrix(); //Pavel
    void InitializeViewMatrix(); //Pavel

    void UpdateChaseCamera();
    void UpdateChaseCameraTest01();
    void UpdateChaseCameraTest02();
    void UpdateChaseCameraTest03();
    void UpdateChaseCameraTest04();
    void UpdateChaseCameraTest05();
    void UpdateProjectionMatrix();
    void UpdateOrthoganalMatrix();
    void UpdateSpinCamera(DX::StepTimer const& aTimer);
    void UpdateSpinCameraGamePlayStart(DX::StepTimer const& aTimer);
    void UpdateViewMatrix();

    DirectX::SimpleMath::Vector3    m_destinationPosition;
    DirectX::SimpleMath::Vector3    m_position;
    DirectX::SimpleMath::Vector3    m_homePosition;
    DirectX::SimpleMath::Vector3    m_target;
    DirectX::SimpleMath::Vector3    m_up;

    float                           m_homePitch;
    float                           m_pitch;
    float                           m_homeYaw;
    float                           m_yaw;
    //float                           m_frustumAngle;
    int                             m_clientWidth;
    int                             m_clientHeight;

    const float                     m_nearPlane = 0.3f;
    const float                     m_farPlane = 255000.0f;

    DirectX::SimpleMath::Matrix     m_viewMatrix;
    DirectX::SimpleMath::Matrix     m_projectionMatrix;
    DirectX::SimpleMath::Matrix     m_orthogonalMatrix;

    const float                     m_posTravelSpeed = 10.6f;
    const float                     m_rotationTravelSpeed = 2.3f;
    const float                     m_aimTurnRate = 0.3f;

    //bool                            m_isCameraAtDestination;
    bool                            m_isCameraAtDestination = false;

    CameraState                     m_cameraState;
    
    DirectX::SimpleMath::Matrix     m_rotationMatrix;
    
    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_forward        = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_defaultRight   = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_right          = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);


    float                           m_moveBackForward = 0.0f;
    float                           m_moveLeftRight = 0.0f;
    float                           m_moveUpDown = 0.0f;

    const DirectX::SimpleMath::Vector3 m_preSwingCamPosOffset = DirectX::SimpleMath::Vector3(-0.25f, 0.07f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_preSwingTargetPosOffset = DirectX::SimpleMath::Vector3(0.5f, 0.11f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_swingCamPosOffset = DirectX::SimpleMath::Vector3(0.0f, 0.02f, 0.2f);
    const DirectX::SimpleMath::Vector3 m_swingTargetPosOffset = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

    DirectX::SimpleMath::Vector3    m_cameraStartPos;
    DirectX::SimpleMath::Vector3    m_cameraEndPos;
    DirectX::SimpleMath::Vector3    m_targetStartPos;
    DirectX::SimpleMath::Vector3    m_targetEndPos;

    float                           m_cameraTransitionSpeed = 10.9f;
    DX::StepTimer                   m_cameraTimer;

    bool                            m_isFpYaxisInverted = true; // toggle of turning on/off inverting the firstperson camera y axis control, set to true because I am weirdo that likes an inverted y axis

    DirectX::SimpleMath::Vector3 m_followCamDirection = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 m_followCamPos = DirectX::SimpleMath::Vector3(-17.0, 3.5f, 0.0f);
    DirectX::SimpleMath::Vector3 m_followCamTarget = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_followCamUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 m_followCamPosOffset = DirectX::SimpleMath::Vector3(-1.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector3 m_followCamPosOffsetTest = DirectX::SimpleMath::Vector3(-15.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 m_followCamTargOffset = DirectX::SimpleMath::Vector3(0.0, 0.0, 0.0);
    float                       m_followCamDistance = 1.0;

    Environment const* m_environment;
    Vehicle const* m_vehicleFocus;

    DirectX::SimpleMath::Quaternion m_testRotQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_chaseCamQuat = DirectX::SimpleMath::Quaternion::Identity;
    const float m_chaseCamLerpFactor = 0.1;

    /////// Spring Camera Variables
    float m_hDistance;
    float m_fDistance;
    float m_vDistance;

    float m_springConstant;
    float m_dampConstant;
    DirectX::SimpleMath::Vector3 m_velocity;
    DirectX::SimpleMath::Vector3 m_actualPosition;
    DirectX::SimpleMath::Matrix m_springCameraMatrix;
    Target m_springTarget;
    void ComputeSpringMatrix();
    void InitializeSpringCamera(Target aTarget, float aSpringConstant, float ahDist, float aVDist);
    void UpdateSpringCamera(DX::StepTimer const& aTimeDelta);


    // SpinCamera
    float m_cameraSpin = 0.0;
    float m_cameraSpinPitch = 0.0;
    const float m_camSpinTime = 4.0;
    float m_cameraSpinRotationAmount = Utility::ToRadians(270.0);
    float m_cameraSpinPitchAmount = Utility::ToRadians(10.0);
    float m_cameraSpinSpeed = m_cameraSpinRotationAmount / m_camSpinTime;
    float m_cameraSpinPitchSpeed = m_cameraSpinPitchAmount / m_camSpinTime;
    DirectX::SimpleMath::Vector3 m_spinCamOffset = m_followCamPos - m_followCamTarget;


    DirectX::SimpleMath::Vector3 m_spinCamStartPos;
    DirectX::SimpleMath::Vector3 m_spinCamEndPos;
    DirectX::SimpleMath::Vector3 m_spinCamAxis;
    DirectX::SimpleMath::Vector3 m_spinCamAxisPos;
    DirectX::SimpleMath::Vector3 m_spinStartTarget;
    float                        m_spinCamTotalTime;
    float                        m_spinCamTotalRotation;
    float                        m_spinCamCurrentRotation;
};

