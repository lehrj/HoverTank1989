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
    CAMERASTATE_FOLLOWVEHICLETEST,
    CAMERASTATE_FOLLOWMISSILE,
    CAMERASTATE_SPRINGCAMERA,
    CAMERASTATE_SPRINGCAMERANPC,
    CAMERASTATE_SPINCAMERA,
    CAMERASTATE_TESTCAMERA01,
    CAMERASTATE_TESTCAMERA02,
    CAMERASTATE_INTRO,
    CAMERASTATE_STARTSCREEN,
    CAMERASTATE_GAMEPLAYSTARTSPIN,
    CAMERASTATE_TRAILERCAMERA,
    CAMERASTATE_TRAILERCAMERA2,
    CAMERASTATE_TRAILERCAMERA3,
    CAMERASTATE_TRAILERCAMERA4,
    CAMERASTATE_STATIC,
    CAMERASTATE_FOLLOWNPC,
    CAMERASTATE_TRANSITIONFROMSPRINGCAM,
    CAMERASTATE_TRANSITIONTOSPRINGCAM,
    CAMERASTATE_SNAPCAM,
    CAMERASTATE_POSE,
    CAMERASTATE_RETURN,
};

enum class MissileTrackState
{
    MISSILETRACKSTATE_FOLLOW,
    MISSILETRACKSTATE_SIDE,
    MISSILETRACKSTATE_TOPDOWN,
    MISSILETRACKSTATE_TOPDOWNSTATIC,
    MISSILETRACKSTATE_TOTARGET,
    MISSILETRACKSTATE_SPRING,
    MISSILETRACKSTATE_STEADYTOTARGET,
    MISSILETRACKSTATE_STEADYTOTARGET3QTR,
    MISSILETRACKSTATE_TRACKALLCAM,
    MISSILETRACKSTATE_TRACKFROMVEHICLE,
    MISSILETRACKSTATE_REARVIEW,
    MISSILETRACKSTATE_EXPLOSION,
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

    void ActivateMissleTrackCamera();
    void CycleMissileTrackState();
    void CycleNpcFocus(const bool isCycleIncrease);

    void FreeLookSpeedUp();
    void FreeLookSpeedDown();

    void FovUp(const float aInput);
    void FovDown(const float aInput);

    CameraState GetCameraState() const { return m_cameraState; };
    float GetAimTurnRate() const { return m_aimTurnRate; };
    float GetFreeLookSpeed() const { return m_posTravelSpeed; };

    DirectX::SimpleMath::Vector3 GetForwardAudio() const { return m_forwardAudio; };
    DirectX::SimpleMath::Vector3 GetUpAudio() const { return m_upAudio; };

    DirectX::BoundingFrustum GetCameraFrustum() const { return m_boundingFrustum; };
    DirectX::SimpleMath::Vector3 GetHomePos() const { return m_homePosition; };
    float GetPitch() const { return m_pitch; };
    DirectX::SimpleMath::Vector3 GetPos() const { return m_position; };
    DirectX::SimpleMath::Vector3 GetPreSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirection);
    DirectX::SimpleMath::Vector3 GetPreSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    DirectX::SimpleMath::Matrix GetProjectionMatrix() const { return m_projectionMatrix; };
    DirectX::SimpleMath::Vector3 GetSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    DirectX::SimpleMath::Vector3 GetSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);    
    DirectX::SimpleMath::Vector3 GetTargetPos() const { return m_target; };
    DirectX::SimpleMath::Vector3 GetUp() const { return m_up; };
    DirectX::SimpleMath::Matrix GetViewMatrix() const { return m_viewMatrix; };
    float GetViewPlaneNear() const { return m_nearPlane; };
    float GetViewPlaneFar() const { return m_farPlane; };
    int GetWindowHeight() const { return m_clientHeight; };
    int GetWindowWidth() const { return m_clientWidth; };
    float GetYaw() const { return m_yaw; };

    float GetZoom() const { return m_fovZoomPercent; };

    void InintializePreSwingCamera(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees);
    bool IsCameraAtDestination();
    void OnResize(uint32_t aWidth, uint32_t aHeight);

    void PrepareTrailerStart();
    void Reset();
    void ResetCameraTransition(DX::StepTimer const& aTimer);
    void ResetIsCameraAtDestination() { m_isCameraAtDestination = false; };
    void ReturnToOverwatchPosition();
    void ReverseTransitionDirection();
    void Rotate(DirectX::SimpleMath::Vector3 aAxis, float aDegrees); //Pavel
    void RotateAtSpeed(float aDx, float aDy);  //Chili
    void RotateCounterClockWise();
    void RotateClockWise();
    void SetCameraEnvironment(const Environment* aEnviron);
    void SetCameraState(const CameraState aCameraState);
    void SetDestinationPos(const DirectX::SimpleMath::Vector3 aDestPos);

    void SetDebugData(std::shared_ptr<DebugData> aDebugPtr) { m_debugData = aDebugPtr; };

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
    void SetSpinCameraTrailerStart(const float aTime);
    void SetTargetEndPos(DirectX::SimpleMath::Vector3 aEndPos);
    void SetTargetStartPos(DirectX::SimpleMath::Vector3 aStartPos);
    void SetTransitionSpeed(const float aSpeed);

    void SetFireControl(std::shared_ptr<FireControl> aFireControlPtr);
    void SetNpcController(std::shared_ptr<NPCController> aNpcController);
    void SetVehicleFocus(std::shared_ptr<Vehicle> aVehicle);
    void SpinClockwise(float aRotation);
    void SpinCounterClockwise(float aRotation);

    void StartTrailerCamera();
    void StartTrailerCamera3();
    void StartTrailerCamera4();

    void TransitionToNpcSpringCamera();
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

    void DrawUI();

    void InitializeOrthoganalMatrix(); //Pavel
    void InitializeProjectionMatrix(); //Pavel
    void InitializeViewMatrix(); //Pavel

    void UpdateBoundingFrustum();
    void UpdateChaseCameraNPC();
    void UpdateChaseCamera();
    void UpdateChaseCameraTest();
    
    void UpdateFollowMissile(DX::StepTimer const& aTimer);
    void UpdateFollowMissile2(DX::StepTimer const& aTimer);
    void UpdateFollowMissile3(DX::StepTimer const& aTimer);

    void UpdateMissileExplodingCam(DX::StepTimer const& aTimer);
    void UpdateMissileTrackCam(DX::StepTimer const& aTimer);
    void UpdateMissileSteadyCam(DX::StepTimer const& aTimer);
    void UpdateMissileReturnCam(DX::StepTimer const& aTimer);

    void UpdateTrackAllMissilesCam(DX::StepTimer const& aTimer);
    void UpdateMissileTrackFromVehicleCam(DX::StepTimer const& aTimer);

    void UpdateProjectionMatrix();
    void UpdateOrthoganalMatrix();
    void UpdateSpinCamera(DX::StepTimer const& aTimer);
    void UpdateSpinCameraGamePlayStart(DX::StepTimer const& aTimer);
    void UpdateTrailerCamera(DX::StepTimer const& aTimer);
    void UpdateTrailerCamera2(DX::StepTimer const& aTimer);
    void UpdateTrailerCamera3(DX::StepTimer const& aTimer);
    void UpdateTrailerCamera4(DX::StepTimer const& aTimer);

    void UpdateViewMatrix();

    DirectX::SimpleMath::Vector3    m_destinationPosition;
    DirectX::SimpleMath::Vector3    m_position;
    DirectX::SimpleMath::Vector3    m_homePosition;
    DirectX::SimpleMath::Vector3    m_target;
    DirectX::SimpleMath::Vector3    m_targetLocal = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3    m_up;
    DirectX::SimpleMath::Vector3    m_forwardAudio = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3    m_upAudio = DirectX::SimpleMath::Vector3::UnitY;

    float                           m_homePitch;
    float                           m_pitch;
    float                           m_homeYaw;
    float                           m_yaw;
    int                             m_clientWidth;
    int                             m_clientHeight;

    const float                     m_nearPlane = 0.3f;
    const float                     m_farPlane = 255000.0f;

    const float                     m_fov = DirectX::XM_PI / 4.f;
    float                           m_fovMod = 1.0f;
    const float                     m_fovModDelta = 1.0f;
    //const float                     m_fovMax = DirectX::XM_PI / 2.f;
    //const float                     m_fovMin = DirectX::XM_PI / 8.f;
    const float                     m_fovMax = 1.0f;
    const float                     m_fovMin = 0.3f;
    float                           m_fovZoomPercent = 0.0f;

    DirectX::SimpleMath::Matrix     m_viewMatrix;
    DirectX::SimpleMath::Matrix     m_projectionMatrix;
    DirectX::SimpleMath::Matrix     m_orthogonalMatrix;

    DirectX::BoundingFrustum        m_boundingFrustum;
    const float                     m_frustumPlanePaddingVertical = Utility::ToRadians(15.0f);
    const float                     m_frustumPlanePaddingHorizontal = Utility::ToRadians(2.0f);

    float                           m_posTravelSpeed = 24.0f;
    const float                     m_freeLookSpeedDelta = 3.0f;
    const float                     m_freeLookSpeedMax = 150.0f;
    const float                     m_freeLookSpeedMin = 1.0f;

    const float                     m_rotationTravelSpeed = 2.3f;
    const float                     m_aimTurnRate = 0.3f;

    bool                            m_isCameraAtDestination = false;

    CameraState                     m_cameraState;
    MissileTrackState               m_missileTrackState;
    MissileTrackState               m_missileTrackStatePrevious;

    DirectX::SimpleMath::Matrix     m_rotationMatrix;

    DirectX::SimpleMath::Vector4    m_defaultForward = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_forward = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_defaultRight = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector4    m_right = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

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

    float                           m_cameraTransitionSpeed = 5.9f;
    float                           m_cameraTransitionSpeedMissileReturn = 55.9f;
    DX::StepTimer                   m_cameraTimer;

    bool                            m_isFpYaxisInverted = true; // toggle of turning on/off inverting the firstperson camera y axis control, set to true because I am weirdo that likes an inverted y axis

    DirectX::SimpleMath::Vector3 m_followCamDirection = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 m_followCamPos = DirectX::SimpleMath::Vector3(-25.0, 6.5f, 0.0f);
    DirectX::SimpleMath::Vector3 m_followNpcCamPos = DirectX::SimpleMath::Vector3(-44.0, 25.5f, 0.0f);
    DirectX::SimpleMath::Vector3 m_followCamTarget = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_followCamUp = DirectX::SimpleMath::Vector3::UnitY;
    DirectX::SimpleMath::Vector3 m_followCamPosOffset = DirectX::SimpleMath::Vector3(-1.0, 1.0, 0.0);
    DirectX::SimpleMath::Vector3 m_followCamPosOffsetTest = DirectX::SimpleMath::Vector3(-15.0, 0.0, 0.0);
    DirectX::SimpleMath::Vector3 m_followCamTargOffset = DirectX::SimpleMath::Vector3(0.0, 5.0, 0.0);
    float                       m_followCamDistance = 1.0;
    //DirectX::SimpleMath::Vector3 m_springCamPos = DirectX::SimpleMath::Vector3(-25.0, 7.0f, 20.0f);
    //DirectX::SimpleMath::Vector3 m_springCamPos = DirectX::SimpleMath::Vector3(-25.0, 9.0f, 20.0f);
    DirectX::SimpleMath::Vector3 m_springCamPos = DirectX::SimpleMath::Vector3(-25.0, 6.5f, 20.0f);

    Environment const* m_environment;
    std::shared_ptr<FireControl>    m_fireControl;
    std::shared_ptr<Vehicle> m_vehicleFocus;
    std::shared_ptr<DebugData>      m_debugData;
    std::shared_ptr<NPCController> m_npcController;

    DirectX::SimpleMath::Quaternion m_testRotQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_chaseCamQuat = DirectX::SimpleMath::Quaternion::Identity;
    const float m_chaseCamLerpFactor = 0.1;

    /////// Spring Camera Variables
    float m_hDistance;
    float m_fDistance;
    float m_vDistance;

    //const float m_springConstantSet = 75.0f;
    const float m_springConstantSet = 30.0f;

    float m_springConstant;
    float m_dampConstant;
    DirectX::SimpleMath::Vector3 m_velocity;
    DirectX::SimpleMath::Vector3 m_actualPosition;
    DirectX::SimpleMath::Matrix m_springCameraMatrix;
    Target m_springTarget;
    void ComputeSpringMatrix();
    DirectX::SimpleMath::Vector3 GetSpringCameraTarget();
    void InitializeSpringCamera(Target aTarget, float aSpringConstant, float ahDist, float aVDist);
    void UpdateSpringCamera(DX::StepTimer const& aTimeDelta);
    void UpdateSpringCameraPlayer(DX::StepTimer const& aTimeDelta);
    void UpdateSpringCameraPlayer2(DX::StepTimer const& aTimeDelta);
    void UpdateSpringCameraPlayer3(DX::StepTimer const& aTimeDelta);
    void UpdateSpringCameraPlayer4(DX::StepTimer const& aTimeDelta);
    void UpdateSpringCameraPlayerLastUsed(DX::StepTimer const& aTimeDelta);
    void UpdateSnapCamera(DX::StepTimer const& aTimeDelta);
    void UpdateSnapCamera2(DX::StepTimer const& aTimeDelta);
    void UpdateSnapCameraMissile(DX::StepTimer const& aTimeDelta);
    void UpdateSnapCameraOld(DX::StepTimer const& aTimeDelta);

    void UpdateSmoothStepVal(const float aTimeStep);
    void ResetSmoothStepVal();

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

    DirectX::SimpleMath::Vector3 m_trailerCamStartPos = DirectX::SimpleMath::Vector3(390.0f, 11.0f, 585.0f);
    DirectX::SimpleMath::Vector3 m_trailerCamEndPos = DirectX::SimpleMath::Vector3(390.0f, 11.0f, 10.0f);
    DirectX::SimpleMath::Vector3 m_trailerTargetStartPos = DirectX::SimpleMath::Vector3(329.0f, 8.0f, 585.0f);
    DirectX::SimpleMath::Vector3 m_trailerTargetEndPos = DirectX::SimpleMath::Vector3(389.0f, 11.0f, 11.6f);

    DirectX::SimpleMath::Vector3 m_trailerCamStartPos2 = m_trailerCamEndPos;
    DirectX::SimpleMath::Vector3 m_trailerCamEndPos2 = DirectX::SimpleMath::Vector3(562.0f, 444.0, 0.0f);
    DirectX::SimpleMath::Vector3 m_trailerTargetStartPos2 = m_trailerTargetEndPos;
    DirectX::SimpleMath::Vector3 m_trailerTargetEndPos2 = DirectX::SimpleMath::Vector3(560.0f, 441.4f, 0.0f);  

    DirectX::SimpleMath::Vector3 m_trailerCamStartPos3 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_trailerCamEndPos3 = DirectX::SimpleMath::Vector3(640.0f, 75.0, 0.0f);
    DirectX::SimpleMath::Vector3 m_trailerTargetStartPos3 = DirectX::SimpleMath::Vector3::UnitX;
    DirectX::SimpleMath::Vector3 m_trailerTargetEndPos3 = DirectX::SimpleMath::Vector3(187.0f, 20.0f, 0.0f);

    const float m_trailerTimeDuration = 9.0f;
    float m_trailerTimerDelay2 = 4.0f;
    float m_trailerTimeDuration2 = 7.0f;
    float m_trailerTimeDuration3 = 2.0f;
    
    DirectX::SimpleMath::Vector3 m_testCamPos1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testCamPos2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testCamPos3 = DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 m_testCamTarg1 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testCamTarg2 = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 m_testCamTarg3 = DirectX::SimpleMath::Vector3::Zero;

    double                       m_trailerTimer = 0.0f;
    int                          m_trailerStep = 0;

    float                        m_transitionTimer = 0.0f;
    const float                  m_transitionTimeMax = 2.0f;

    int m_npcFocusID = 0;

    const DirectX::SimpleMath::Vector3 m_missileCamSide = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 3.0f);
    const DirectX::SimpleMath::Vector3 m_missileCamSideOffset = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 2.0f);

    const DirectX::SimpleMath::Vector3 m_missileCamTop = DirectX::SimpleMath::Vector3(0.0f, 18.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_missileCamToTarget = DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_missileCamToTarget3Qrt = DirectX::SimpleMath::Vector3(-3.0f, 1.0f, 0.0f);

    const DirectX::SimpleMath::Vector3 m_missileFollowCamPos = DirectX::SimpleMath::Vector3(-3.0f, 0.0f, 0.0f);

    const DirectX::SimpleMath::Vector3 m_missileSnapPos = DirectX::SimpleMath::Vector3(-3.0f, 0.3f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_missileSnapPosSide = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 13.0f);
    const DirectX::SimpleMath::Vector3 m_missileSnapPosTop = DirectX::SimpleMath::Vector3(0.0f, 3.3f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_missileSnapTarg = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_snapPosBase = DirectX::SimpleMath::Vector3(-3.0f, 0.3f, 0.0f);
    //const DirectX::SimpleMath::Vector3 m_snapPosBase = DirectX::SimpleMath::Vector3(-23.0f, 6.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_snapPosBase = DirectX::SimpleMath::Vector3(-23.0f, 3.0f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_snapZoomModPos = DirectX::SimpleMath::Vector3(-14.0f, 3.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 m_snapZoomModPos = DirectX::SimpleMath::Vector3(7.0f, .5f, 0.0f);

    //const DirectX::SimpleMath::Vector3 m_snapZoomModTarget = DirectX::SimpleMath::Vector3(0.0f, 6.0f, -14.0f);
    const DirectX::SimpleMath::Vector3 m_snapZoomModTarget = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -100.0f);

    //const DirectX::SimpleMath::Vector3 m_snapTargBase = DirectX::SimpleMath::Vector3(0.0f, 0.5f, -4.0f);
    //const DirectX::SimpleMath::Vector3 m_snapTargBase = DirectX::SimpleMath::Vector3(0.0f, 3.0f, -4.0f);
    const DirectX::SimpleMath::Vector3 m_snapTargBase = DirectX::SimpleMath::Vector3(0.0f, 3.0f, -4.0f);

    DirectX::SimpleMath::Vector3 m_snapPosNorm = DirectX::SimpleMath::Vector3(-1.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_snapTargNorm = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_snapPos = DirectX::SimpleMath::Vector3(-1.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_snapTarg = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_snapPosPrev = DirectX::SimpleMath::Vector3(-1.0f, 1.0f, 0.0f);
    DirectX::SimpleMath::Vector3 m_snapTargPrev = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    DirectX::SimpleMath::Quaternion m_snapQuat = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Quaternion m_snapTargetQuat = DirectX::SimpleMath::Quaternion::Identity;

    DirectX::SimpleMath::Vector3 m_staticCamPos = DirectX::SimpleMath::Vector3(137.0f, 40.0f, 600.0f);
    DirectX::SimpleMath::Vector3 m_staticCamTarget = DirectX::SimpleMath::Vector3(137.0f, 30.0f, 455.0f);
    DirectX::SimpleMath::Vector3 m_staticCamUp = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

    const float m_smoothStepToMissile = 0.1f;
    const float m_smoothStepToVehicle = 0.2f;
    const float m_smoothStepTarget = 0.1f;
    const float m_smoothStepSnapCamPos = 0.1f;

    const float m_slerpSnapCam = 0.1f;
    const float m_slerpSnapTarg = 0.1f;

    float m_camStateTimer = 0.0f;
    const float m_camStateTimerDelta = 0.1f;
    const float m_camStateTimerMaxTime = 5.0f;
    const float m_camStateTimerMaxValMax = 0.9f;
    const float m_camStateTimerMaxValMin = 0.1f;
    float m_smoothStepVal = 0.9f;
    float m_smoothStepValTarget = 0.9f;
    float m_smoothStepValPosition = 0.9f;

    const float m_missileReturnPauseDelay = 1.0f;
    float m_missileReturnPauseTimer = 0.0f;


    const float m_missileExplosionTimeMax = 2.0f;
    float m_missileExplosionTimer = 0.0f;

    const float m_missileExplosionCamPosTimeMax = 0.5f;
    float m_missileExplosionCampPosTime = 0.0f;

};

