#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
	m_homePosition = DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	m_target = DirectX::SimpleMath::Vector3::Zero;
	m_up = DirectX::SimpleMath::Vector3::UnitY;
	m_homePitch = 0.0f;
	m_homeYaw = 0.0f;
	Reset();
	// ToDo WLJ: Add updated initilzation functions and test 
}

Camera::Camera(int aWidth, int aHeight)
{
	m_clientWidth = (int)aWidth;
	m_clientHeight = aHeight;
	m_homePosition = DirectX::SimpleMath::Vector3(0.0f, 0.4f, 0.0f);
	m_target = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	m_up = DirectX::SimpleMath::Vector3::UnitY;
	m_position = DirectX::SimpleMath::Vector3(-2.0f, 0.2f, 0.0f);
	m_homePitch = -0.053f;
	m_homeYaw = 0.0f;

	m_cameraState = CameraState::CAMERASTATE_SPRINGCAMERANPC;
	//m_cameraState = CameraState::CAMERASTATE_FOLLOWVEHICLE;
	m_cameraState = CameraState::CAMERASTATE_SPRINGCAMERA;
	
	Target springTarget;
	springTarget.forward = DirectX::SimpleMath::Vector3::UnitX;
	springTarget.up = DirectX::SimpleMath::Vector3::UnitY;
	springTarget.position = DirectX::SimpleMath::Vector3(0.0, 0.0f, 0.0);
	float springConst = m_springConstantSet;
	float hDist = -m_springCamPos.x;
	float vDist = m_springCamPos.y;

	InitializeSpringCamera(springTarget, springConst, hDist, vDist);

	Reset();
	InitializeViewMatrix();
	InitializeProjectionMatrix();
	InitializeOrthoganalMatrix();
}

Camera::~Camera()
{
	m_environment = nullptr;
	delete m_environment;
	m_vehicleFocus = nullptr;
	delete m_vehicleFocus;
}

void Camera::CycleNpcFocus(const bool isCycleIncrease)
{
	const int npcCount = m_npcController->GetNpcCount();
	m_transitionTimer = 0.0f;
	if (isCycleIncrease == true)
	{
		if (m_npcFocusID >= npcCount - 1)
		{
			m_npcFocusID = 0;
		}
		else
		{
			m_npcFocusID++;
		}
	}
	else
	{
		if (m_npcFocusID <= 0)
		{
			m_npcFocusID = npcCount - 1;
		}
		else
		{
			m_npcFocusID--;
		}
	}
}

DirectX::SimpleMath::Vector3 Camera::GetPreSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirection)
{
	DirectX::SimpleMath::Vector3 newCamPosition = DirectX::SimpleMath::Vector3::Transform(m_preSwingCamPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(aDirection)) + aPosition;
	return newCamPosition;
}

DirectX::SimpleMath::Vector3 Camera::GetPreSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees)
{
	DirectX::SimpleMath::Vector3 newTargetPosition = DirectX::SimpleMath::Vector3::Transform(m_preSwingTargetPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(aDirectionDegrees))) + aPosition;
	return newTargetPosition;
}

DirectX::SimpleMath::Vector3 Camera::GetSwingCamPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees)
{
	DirectX::SimpleMath::Vector3 newCamPosition = DirectX::SimpleMath::Vector3::Transform(m_swingCamPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(aDirectionDegrees))) + aPosition;
	return newCamPosition;
}

DirectX::SimpleMath::Vector3 Camera::GetSwingTargPos(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees)
{
	DirectX::SimpleMath::Vector3 newTargetPosition = DirectX::SimpleMath::Vector3::Transform(m_swingTargetPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(aDirectionDegrees))) + aPosition;
	return newTargetPosition;
}

void Camera::InitializeOrthoganalMatrix()
{
	m_orthogonalMatrix = DirectX::SimpleMath::Matrix::CreateOrthographic((float)m_clientWidth, (float)m_clientHeight, m_nearPlane, m_farPlane);
}

void Camera::InitializeProjectionMatrix()
{
	m_projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4.f, static_cast<float>(m_clientWidth) / static_cast<float>(m_clientHeight), m_nearPlane, m_farPlane);
}

void Camera::InintializePreSwingCamera(DirectX::SimpleMath::Vector3 aPosition, float aDirectionDegrees)
{
	DirectX::SimpleMath::Vector3 newCamPosition = DirectX::SimpleMath::Vector3::Transform(m_preSwingCamPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(aDirectionDegrees))) + aPosition;

	DirectX::SimpleMath::Vector3 newTargetPosition = DirectX::SimpleMath::Vector3::Transform(m_preSwingTargetPosOffset,
		DirectX::SimpleMath::Matrix::CreateRotationY(Utility::ToRadians(aDirectionDegrees))) + aPosition;

	SetTargetPos(newTargetPosition);
	SetPos(newCamPosition);
}

void Camera::InitializeViewMatrix()
{
	m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
}

bool Camera::IsCameraAtDestination()
{
	if (m_position == m_destinationPosition || m_isCameraAtDestination == true)
	{
		return true;
	}
	else
	{
		if (m_position == m_destinationPosition)
		{
			m_isCameraAtDestination = true;
			return true;
		}
		else
		{
			return false;
		}
	}
}

void Camera::PrepareTrailerStart()
{
	m_cameraState = CameraState::CAMERASTATE_STATIC;
	m_yaw = Utility::ToRadians(180.0f);
	m_pitch = Utility::ToRadians(0.0f);
	DirectX::SimpleMath::Vector3 cameraPosition(450.0, 10.0, 650.0f);
	DirectX::SimpleMath::Vector3 target = cameraPosition;
	target += DirectX::SimpleMath::Vector3(0.0, 0.0, 1.0f);
	DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3::UnitY;
	m_position = m_trailerCamStartPos;
	m_target = m_trailerTargetStartPos;
	m_up = up;
	const float spinTime = 10.0f;
	SetSpinCameraTrailerStart(spinTime);
	DirectX::SimpleMath::Vector3 testTargetEnd = m_trailerTargetEndPos - m_trailerCamEndPos;
	testTargetEnd.Normalize();
	testTargetEnd *= 5.0f;
	DirectX::SimpleMath::Vector3 updateTargetEnd1 = testTargetEnd + m_trailerCamEndPos;
	m_trailerTargetEndPos = updateTargetEnd1;
	m_trailerCamStartPos2 = m_trailerCamEndPos;
	m_trailerTargetStartPos2 = m_trailerTargetEndPos;
	DirectX::SimpleMath::Vector3 testTargetEnd2 = m_trailerTargetEndPos2 - m_trailerCamEndPos2;
	testTargetEnd2.Normalize();
	testTargetEnd2 *= 10.0f;
	DirectX::SimpleMath::Vector3 updateTargetEnd2 = testTargetEnd2 + m_trailerCamEndPos2;
	m_trailerTargetEndPos2 = updateTargetEnd2;
	m_trailerTimer = 0.0f;
}

void Camera::ReturnToOverwatchPosition()
{
	m_trailerCamStartPos2 = m_position;
	m_trailerTargetStartPos2 = m_target;

	DirectX::SimpleMath::Vector3 newTargetEnd = m_trailerTargetEndPos2 - m_trailerCamEndPos2;
	newTargetEnd.Normalize();
	newTargetEnd *= 100.0f;
	newTargetEnd = newTargetEnd + m_trailerCamEndPos2;
	m_trailerTargetEndPos2 = newTargetEnd;
	m_trailerTimer = 0.0f;
	m_trailerTimerDelay2 = 0.0f;
	m_trailerTimeDuration2 = 2.0f;
	m_cameraState = CameraState::CAMERASTATE_TRAILERCAMERA2;
}

void Camera::TransitionToNpcSpringCamera()
{
	m_transitionTimer = 0.0f;
	m_cameraState = CameraState::CAMERASTATE_TRANSITIONTOSPRINGCAM;
	m_cameraState = CameraState::CAMERASTATE_SPRINGCAMERANPC;
	Target springTarget;
	springTarget.forward = DirectX::SimpleMath::Vector3::UnitX;
	
	springTarget.up = DirectX::SimpleMath::Vector3::UnitY;
	springTarget.position = m_target;
	float springConst = m_springConstantSet;
	float hDist = -m_springCamPos.x;
	float vDist = m_springCamPos.y;

	DirectX::SimpleMath::Vector3 testVelocity = m_velocity;

	InitializeSpringCamera(springTarget, springConst, hDist, vDist);
	
	m_actualPosition = m_position;
}

void Camera::StartTrailerCamera()
{
	m_cameraState = CameraState::CAMERASTATE_TRAILERCAMERA;
}

void Camera::StartTrailerCamera3()
{
	if (m_cameraState == CameraState::CAMERASTATE_SPRINGCAMERANPC)
	{
		m_position = m_actualPosition;
		m_target = m_npcController->GetNpcPos(m_npcFocusID);
	}
	m_trailerTimer = 0.0f;
	m_trailerCamStartPos3 = m_position;
	m_trailerTargetStartPos3 = m_target;
	m_cameraState = CameraState::CAMERASTATE_TRAILERCAMERA3;
}

void Camera::StartTrailerCamera4()
{
	m_trailerTimer = 0.0f;
	m_trailerCamStartPos2 = m_position;
	m_trailerTargetStartPos2 = m_target;
	m_cameraState = CameraState::CAMERASTATE_TRAILERCAMERA4;
}

void Camera::UpdateTrailerCamera(DX::StepTimer const& aTimer)
{
	m_trailerTimer += aTimer.GetElapsedSeconds();

	const float transitionTime = m_trailerTimeDuration;
	DirectX::SimpleMath::Vector3 startCamPos1 = m_trailerCamStartPos;
	DirectX::SimpleMath::Vector3 startTargPos1 = m_trailerTargetStartPos;

	DirectX::SimpleMath::Vector3 endCamPos1 = m_trailerCamEndPos;
	DirectX::SimpleMath::Vector3 endTargPos1 = m_trailerTargetEndPos;

	DirectX::SimpleMath::Vector3 currentCamPos = m_position;
	DirectX::SimpleMath::Vector3 currentTarget = m_target;

	const float camPosDistance = DirectX::SimpleMath::Vector3::Distance(startCamPos1, endCamPos1);
	const float targPosDistance = DirectX::SimpleMath::Vector3::Distance(startTargPos1, endTargPos1);

	DirectX::SimpleMath::Vector3 cameraDirection = endCamPos1 - startCamPos1;
	cameraDirection.Normalize();
	DirectX::SimpleMath::Vector3 targetDirection = endTargPos1 - startTargPos1;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = camPosDistance / transitionTime;

	m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

	float targetSpeed = targPosDistance / transitionTime;
	if (targPosDistance > 0.0f)
	{
		m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
	}

	const float camPosDistance2 = DirectX::SimpleMath::Vector3::Distance(m_position, endCamPos1);
	if (m_trailerTimer > m_trailerTimeDuration)
	{
		m_trailerTimer = 0.0f;
		m_position = m_trailerCamEndPos;
		m_target = m_trailerTargetEndPos;
		m_cameraState = CameraState::CAMERASTATE_TRAILERCAMERA2;
	}

	m_testCamPos1 = m_position;
	m_testCamTarg1 = m_target;
}

void Camera::UpdateTrailerCamera2(DX::StepTimer const& aTimer)
{
	m_testCamPos2 = m_position;
	m_testCamTarg2 = m_target;

	m_trailerTimer += aTimer.GetElapsedSeconds();

	const float transitionTime = m_trailerTimeDuration2;

	DirectX::SimpleMath::Vector3 startCamPos1 = m_trailerCamStartPos2;
	DirectX::SimpleMath::Vector3 startTargPos1 = m_trailerTargetStartPos2;

	DirectX::SimpleMath::Vector3 endCamPos1 = m_trailerCamEndPos2;
	DirectX::SimpleMath::Vector3 endTargPos1 = m_trailerTargetEndPos2;

	DirectX::SimpleMath::Vector3 currentCamPos = m_position;
	DirectX::SimpleMath::Vector3 currentTarget = m_target;

	const float camPosDistance = DirectX::SimpleMath::Vector3::Distance(startCamPos1, endCamPos1);
	const float targPosDistance = DirectX::SimpleMath::Vector3::Distance(startTargPos1, endTargPos1);

	DirectX::SimpleMath::Vector3 cameraDirection = endCamPos1 - startCamPos1;
	cameraDirection.Normalize();
	DirectX::SimpleMath::Vector3 targetDirection = endTargPos1 - startTargPos1;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = camPosDistance / transitionTime;
	float targetSpeed = targPosDistance / transitionTime;

	m_testCamPos3 = m_position;
	m_testCamTarg3 = m_target;

	if (m_trailerTimer > m_trailerTimerDelay2)
	{
		m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

		if (targPosDistance > 0.0f)
		{
			m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
		}

		const float camPosDistance2 = DirectX::SimpleMath::Vector3::Distance(m_position, endCamPos1);
		if (m_trailerTimer > m_trailerTimeDuration2 + m_trailerTimerDelay2)
		{
			m_position = m_trailerCamEndPos2;
			m_target = m_trailerTargetEndPos2;
			m_cameraState = CameraState::CAMERASTATE_STATIC;
		}
	}
}

void Camera::UpdateTrailerCamera3(DX::StepTimer const& aTimer)
{
	m_testCamPos3 = m_position;
	m_testCamTarg3 = m_target;

	m_trailerTimer += aTimer.GetElapsedSeconds();

	const float transitionTime = m_trailerTimeDuration3;

	DirectX::SimpleMath::Vector3 startCamPos1 = m_trailerCamStartPos3;
	DirectX::SimpleMath::Vector3 startTargPos1 = m_trailerTargetStartPos3;

	DirectX::SimpleMath::Vector3 endCamPos1 = m_trailerCamEndPos3;
	DirectX::SimpleMath::Vector3 endTargPos1 = m_trailerTargetEndPos3;

	DirectX::SimpleMath::Vector3 currentCamPos = m_position;
	DirectX::SimpleMath::Vector3 currentTarget = m_target;

	const float camPosDistance = DirectX::SimpleMath::Vector3::Distance(startCamPos1, endCamPos1);
	const float targPosDistance = DirectX::SimpleMath::Vector3::Distance(startTargPos1, endTargPos1);

	DirectX::SimpleMath::Vector3 cameraDirection = endCamPos1 - startCamPos1;
	cameraDirection.Normalize();
	DirectX::SimpleMath::Vector3 targetDirection = endTargPos1 - startTargPos1;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = camPosDistance / transitionTime;
	float targetSpeed = targPosDistance / transitionTime;

	m_testCamPos3 = m_position;
	m_testCamTarg3 = m_target;

	if (m_trailerTimer > 0.0f)
	{
		m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

		if (targPosDistance > 0.0f)
		{
			m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
		}

		const float camPosDistance2 = DirectX::SimpleMath::Vector3::Distance(m_position, endCamPos1);

		if (m_trailerTimer > m_trailerTimeDuration3)
		{
			m_position = m_trailerCamEndPos3;
			m_target = m_trailerTargetEndPos3;
			m_cameraState = CameraState::CAMERASTATE_STATIC;
		}
	}
}

void Camera::UpdateTrailerCamera4(DX::StepTimer const& aTimer)
{
	m_trailerTimer += aTimer.GetElapsedSeconds();

	const float transitionTime = m_trailerTimeDuration3;

	DirectX::SimpleMath::Vector3 startCamPos1 = m_trailerCamStartPos2;
	DirectX::SimpleMath::Vector3 startTargPos1 = m_trailerTargetStartPos2;

	DirectX::SimpleMath::Vector3 endCamPos1 = m_trailerCamEndPos2;
	DirectX::SimpleMath::Vector3 endTargPos1 = m_trailerTargetEndPos2;

	DirectX::SimpleMath::Vector3 currentCamPos = m_position;
	DirectX::SimpleMath::Vector3 currentTarget = m_target;

	const float camPosDistance = DirectX::SimpleMath::Vector3::Distance(startCamPos1, endCamPos1);
	const float targPosDistance = DirectX::SimpleMath::Vector3::Distance(startTargPos1, endTargPos1);

	DirectX::SimpleMath::Vector3 cameraDirection = endCamPos1 - startCamPos1;
	cameraDirection.Normalize();
	DirectX::SimpleMath::Vector3 targetDirection = endTargPos1 - startTargPos1;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = camPosDistance / transitionTime;
	float targetSpeed = targPosDistance / transitionTime;

	if (m_trailerTimer > 0.0f)
	{
		m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

		if (targPosDistance > 0.0f)
		{
			m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
		}

		const float camPosDistance2 = DirectX::SimpleMath::Vector3::Distance(m_position, endCamPos1);

		if (m_trailerTimer > m_trailerTimeDuration3)
		{
			m_position = m_trailerCamEndPos2;
			m_target = m_trailerTargetEndPos2;
			m_cameraState = CameraState::CAMERASTATE_STATIC;
		}
	}
}

void Camera::OnResize(uint32_t aWidth, uint32_t aHeight)
{
	m_clientWidth = aWidth;
	m_clientHeight = aHeight;
	UpdateOrthoganalMatrix();
	UpdateProjectionMatrix();
	UpdateViewMatrix();
}

void Camera::Reset()
{
	m_position = m_homePosition;
	m_up = DirectX::SimpleMath::Vector3::UnitY;
	m_pitch = m_homePitch;
	m_yaw = m_homeYaw;
}

void Camera::ResetCameraTransition(DX::StepTimer const& aTimer)
{
	DirectX::SimpleMath::Vector3 cameraStartPos = m_cameraStartPos;

	DirectX::SimpleMath::Vector3 cameraEndPos = m_cameraEndPos;

	float cameraDistance = DirectX::SimpleMath::Vector3::Distance(cameraStartPos, cameraEndPos);
	DirectX::SimpleMath::Vector3 cameraDirection = cameraEndPos - cameraStartPos;

	cameraDirection.Normalize();

	DirectX::SimpleMath::Vector3 targetStartPos = m_targetStartPos;
	DirectX::SimpleMath::Vector3 targetEndPos = m_targetEndPos;

	float targetDistance = DirectX::SimpleMath::Vector3::Distance(targetStartPos, targetEndPos);
	DirectX::SimpleMath::Vector3 targetDirection = targetEndPos - targetStartPos;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = m_cameraTransitionSpeed;

	float targetSpeed;
	if (abs(cameraDistance > 0.0)) // prevent divide by zero if camera position doesn't change
	{
		targetSpeed = cameraSpeed * (targetDistance / cameraDistance);
	}
	else
	{
		targetSpeed = cameraSpeed;
	}

	m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

	if (targetDistance > 0.0f)
	{
		m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
	}

	m_up = DirectX::SimpleMath::Vector3::UnitY;

	if (DirectX::SimpleMath::Vector3::Distance(cameraStartPos, m_position) >= cameraDistance && DirectX::SimpleMath::Vector3::Distance(targetStartPos, m_target) >= targetDistance)
	{
		m_position = cameraEndPos;
		m_isCameraAtDestination = true;
	}
	else
	{

	}
}

void Camera::ReverseTransitionDirection()
{
	std::swap(m_cameraStartPos, m_cameraEndPos);
	std::swap(m_targetStartPos, m_targetEndPos);
}

void Camera::Rotate(DirectX::SimpleMath::Vector3 aAxis, float aDegrees)
{
	if (aAxis == DirectX::SimpleMath::Vector3::Zero || aDegrees == 0.0f)
	{
		return;
	}

	DirectX::SimpleMath::Vector3 lookAtTarget = m_target - m_position;
	DirectX::SimpleMath::Vector3 lookAtUp = m_up - m_position;
	lookAtTarget = DirectX::SimpleMath::Vector3::Transform(lookAtTarget, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aAxis, static_cast<float>(Utility::ToRadians(aDegrees))));
	lookAtUp = DirectX::SimpleMath::Vector3::Transform(lookAtUp, DirectX::SimpleMath::Matrix::CreateFromAxisAngle(aAxis, static_cast<float>(Utility::ToRadians(aDegrees))));

	m_target = DirectX::SimpleMath::Vector3(m_position + lookAtTarget);
	m_up = DirectX::SimpleMath::Vector3(m_position + lookAtUp);

	this->InitializeViewMatrix();
}

void Camera::RotateAtSpeed(float aDx, float aDy)
{
	m_yaw = Utility::WrapAngle(m_yaw + aDx * m_rotationTravelSpeed);
	float rotation = m_pitch + aDy * m_rotationTravelSpeed;
	float min = 0.995f * static_cast<float>(-Utility::GetPi()) / 2.0f;
	float max = 0.995f * static_cast<float>(Utility::GetPi()) / 2.0f;

	m_pitch = std::clamp(rotation, min, max);
}

void Camera::RotateCounterClockWise()
{
	float spinRate = 3.0f;
	m_yaw = Utility::WrapAngle(m_yaw + spinRate * m_rotationTravelSpeed);
	UpdateViewMatrix();
}

void Camera::RotateClockWise()
{
	float spinRate = 3.0f;
	m_yaw = Utility::WrapAngle(m_yaw - spinRate * m_rotationTravelSpeed);
	UpdateViewMatrix();
}

void Camera::SetCameraEndPos(DirectX::SimpleMath::Vector3 aEndPos)
{
	m_cameraEndPos = aEndPos;
}

void Camera::SetCameraStartPos(DirectX::SimpleMath::Vector3 aStartPos)
{
	m_cameraStartPos = aStartPos;
}

void Camera::SetCameraState(const CameraState aCameraState)
{
	m_cameraState = aCameraState;
}

void Camera::SetDestinationPos(const DirectX::SimpleMath::Vector3 aDestPos)
{
	if (aDestPos == m_target)
	{
		// add error handling to prevent crash
		//std::cerr << "Error in Camera::UpdatePosition, updated position = current target position";
		return;
	}
	m_destinationPosition = aDestPos;
}

void Camera::SetFollowCamDirection(const DirectX::SimpleMath::Vector3 aDirection)
{
	m_followCamDirection = aDirection;
}

void Camera::SetFollowCamPos(const DirectX::SimpleMath::Vector3 aPos)
{
	m_followCamPos = aPos;
}

void Camera::SetFollowCamTarget(const DirectX::SimpleMath::Vector3 aTarg)
{
	m_followCamTarget = aTarg;
}

void Camera::SetFollowCamUp(const DirectX::SimpleMath::Vector3 aUp)
{
	m_followCamUp = aUp;
}

void Camera::SetHomePos(DirectX::SimpleMath::Vector3 aHomePos)
{
	if (aHomePos == m_target)
	{
		// add error handling to prevent crash
		//std::cerr << "Error in Camera::UpdatePosition, updated position = current target position";
		return;
	}
	m_position = aHomePos;
}

void Camera::SetPos(DirectX::SimpleMath::Vector3 aPos)
{
	if (aPos == m_target)
	{
		// add error handling to prevent crash
		//std::cerr << "Error in Camera::UpdatePosition, updated position = current target position";
		return;
	}
	m_position = aPos;
	//m_followCamPos = aPos;
}

void Camera::SetTargetEndPos(DirectX::SimpleMath::Vector3 aEndPos)
{
	m_targetEndPos = aEndPos;
}

void Camera::SetTargetStartPos(DirectX::SimpleMath::Vector3 aStartPos)
{
	m_targetStartPos = aStartPos;
}

void Camera::SetTargetPos(const DirectX::SimpleMath::Vector3 aTarget)
{
	if (aTarget == m_position)
	{
		// add error handling to prevent crash
		//std::cerr << "Error in Camera::SetTargetPos, updated target position = current camera position";
		return;
	}
	m_target = aTarget;
}

void Camera::SetUpPos(const DirectX::SimpleMath::Vector3 aPos)
{
	m_up = aPos;
}

void Camera::SetTransitionSpeed(const float aSpeed)
{
	if (aSpeed > 0.0)
	{
		m_cameraTransitionSpeed = aSpeed;
	}
	else
	{
		// add error handling to prevent transition stall or backwards motion that could break things
		// cerr << "input range out of bounds"
	}
}

void Camera::SetCameraEnvironment(const Environment* aEnviron)
{
	m_environment = aEnviron;
}

void Camera::SetNpcController(std::shared_ptr<NPCController> aNpcController)
{
	m_npcController = aNpcController;
}

void Camera::SetVehicleFocus(const Vehicle* aVehicle)
{
	m_vehicleFocus = aVehicle;
}

void Camera::SpinClockwise(float aRotation)
{
	m_cameraSpin += aRotation;
	DirectX::SimpleMath::Vector3 viewLine = m_followCamPos - m_followCamTarget;
	viewLine = DirectX::SimpleMath::Vector3::Transform(viewLine, DirectX::SimpleMath::Matrix::CreateRotationY(-aRotation));
	m_followCamPos = viewLine + m_followCamTarget;
	SetPos(viewLine + m_followCamTarget);
}

void Camera::SpinCounterClockwise(float aRotation)
{
	m_cameraSpin += aRotation;
	DirectX::SimpleMath::Vector3 viewLine = m_followCamPos - m_followCamTarget;
	viewLine = DirectX::SimpleMath::Vector3::Transform(viewLine, DirectX::SimpleMath::Matrix::CreateRotationY(aRotation));
	m_followCamPos = viewLine + m_followCamTarget;
	SetPos(viewLine + m_followCamTarget);
}

void Camera::TranslateAtSpeed(DirectX::SimpleMath::Vector3 aTranslation)
{
	DirectX::XMStoreFloat3(&aTranslation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&aTranslation),
		DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
		DirectX::XMMatrixScaling(m_posTravelSpeed, m_posTravelSpeed, m_posTravelSpeed)
	));
	m_position = { m_position.x + aTranslation.x, m_position.y + aTranslation.y, m_position.z + aTranslation.z };
}

void Camera::UpdateCamera(DX::StepTimer const& aTimer)
{
	UpdateTimer(aTimer);

	if (m_cameraState == CameraState::CAMERASTATE_FIRSTPERSON)
	{
		UpdateFirstPersonCamera();
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_SWINGVIEW)
	{
		// no update needed in current state
	}
	else if (m_cameraState == CameraState::CAMERASTATE_PROJECTILEFLIGHTVIEW)
	{
		// no update needed in current state
	}
	else if (m_cameraState == CameraState::CAMERASTATE_PRESWINGVIEW)
	{
		// no update needed in current state
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRANSITION)
	{
		if (IsCameraAtDestination() == false)
		{
			UpdateTransitionCamera(aTimer);
		}
		else
		{
			m_isCameraAtDestination = false;
		}
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_RESET)
	{
		if (IsCameraAtDestination() == false)
		{
			UpdateTransitionCamera(aTimer);
		}
		else
		{
			m_cameraState = CameraState::CAMERASTATE_PRESWINGVIEW;
			m_isCameraAtDestination = false;
		}
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRANSTONEWSHOT)
	{
		if (IsCameraAtDestination() == false)
		{
			UpdateTransitionCamera(aTimer);
		}
		else
		{
			SetCameraStartPos(GetPos());
			SetCameraEndPos(GetPreSwingCamPos(GetPos(), 0.0));
			SetTargetStartPos(GetTargetPos());
			SetTargetEndPos(GetPreSwingTargPos(GetPos(), 0.0));

			m_cameraState = CameraState::CAMERASTATE_PRESWINGVIEW;
			m_isCameraAtDestination = false;
		}
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_FOLLOWVEHICLE)
	{
		UpdateChaseCamera();
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_SPINCAMERA)
	{
		UpdateSpinCamera(aTimer);
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_followCamPos, m_followCamTarget, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_SPRINGCAMERA)
	{
		/*
		m_springTarget.position = m_vehicleFocus->GetPos();
		DirectX::SimpleMath::Vector3 testHeading = DirectX::SimpleMath::Vector3::UnitX;
		DirectX::SimpleMath::Matrix rotMat = m_vehicleFocus->GetVehicleOrientation();
		testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);
		m_springTarget.forward = testHeading;
		*/
		UpdateSpringCameraPlayer(aTimer);
		//m_viewMatrix = m_springCameraMatrix;
	}
	else if (m_cameraState == CameraState::CAMERASTATE_SPRINGCAMERANPC)
	{
		UpdateSpringCamera(aTimer);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_GAMEPLAYSTARTSPIN)
	{
		UpdateSpinCameraGamePlayStart(aTimer);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRAILERCAMERA)
	{
		UpdateTrailerCamera(aTimer);
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRAILERCAMERA2)
	{
		UpdateTrailerCamera2(aTimer);
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRAILERCAMERA3)
	{
		UpdateTrailerCamera3(aTimer);
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_TRAILERCAMERA4)
	{
		UpdateTrailerCamera4(aTimer);
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_FOLLOWNPC)
	{
		UpdateChaseCameraNPC();
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else if (m_cameraState == CameraState::CAMERASTATE_STATIC)
	{
		m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	else
	{
		//m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);
	}
	m_debugData->DebugPushUILineDecimalNumber("Camera loc x = ", m_position.x, "");
	m_debugData->DebugPushUILineDecimalNumber("Camera loc y = ", m_position.y, "");
	m_debugData->DebugPushUILineDecimalNumber("Camera loc z = ", m_position.z, "");

	UpdateOrthoganalMatrix();
	UpdateProjectionMatrix();
}

void Camera::UpdateFirstPersonCamera()
{
	// apply setting for inverting first person camer Y axis controls 
	if (m_isFpYaxisInverted == true)
	{
		m_rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0, m_yaw, -m_pitch);
	}
	else
	{
		m_rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0, m_yaw, m_pitch);
	}

	m_target = DirectX::XMVector3TransformCoord(m_defaultForward, m_rotationMatrix);
	m_target.Normalize();

	m_right = DirectX::XMVector3TransformCoord(m_defaultRight, m_rotationMatrix);
	m_forward = DirectX::XMVector3TransformCoord(m_defaultForward, m_rotationMatrix);

	m_up = DirectX::XMVector3Cross(m_right, m_forward);

	m_position += DirectX::operator*(m_moveLeftRight, m_right);
	m_position += DirectX::operator*(m_moveBackForward, m_forward);
	m_position += DirectX::operator*(m_moveUpDown, m_up);

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;
	m_moveUpDown = 0.0f;

	m_target = m_position + m_target;
}

void Camera::SetSpinCameraStart()
{
	float yaw = Utility::ToRadians(90.0);
	float pitch = Utility::ToRadians(10.0);
	DirectX::SimpleMath::Quaternion quatRot = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, 0.0, pitch);

	m_followCamPos = DirectX::SimpleMath::Vector3::Transform(m_followCamPos, quatRot);
	m_spinCamOffset = m_followCamPos - m_followCamTarget;
}

void Camera::UpdateSpinCamera(DX::StepTimer const& aTimer)
{
	const float timeDelta = static_cast<float>(aTimer.GetElapsedSeconds());
	m_cameraSpin += m_cameraSpinSpeed * timeDelta;
	m_cameraSpinPitch -= m_cameraSpinPitchSpeed * timeDelta;
	DirectX::SimpleMath::Vector3 newCamPos = m_spinCamOffset;
	DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix::CreateRotationY(m_cameraSpin);
	DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(m_cameraSpin, m_cameraSpinPitch, 0.0);
	newCamPos = DirectX::SimpleMath::Vector3::Transform(newCamPos, rotQuat);
	newCamPos += m_vehicleFocus->GetPos();
	m_followCamPos = newCamPos;
	m_followCamTarget = m_vehicleFocus->GetPos();

	if (m_cameraSpin >= m_cameraSpinRotationAmount)
	{
		m_cameraSpin = 0.0;
		m_springTarget.position = m_followCamTarget;
		m_actualPosition = newCamPos;
		ComputeSpringMatrix();
		m_cameraState = CameraState::CAMERASTATE_SPRINGCAMERA;
	}
}

void Camera::UpdateSpinCameraGamePlayStart(DX::StepTimer const& aTimer)
{
	const float timeDelta = static_cast<float>(aTimer.GetElapsedSeconds());

	float rotationAmount = m_spinCamTotalRotation * (timeDelta / m_spinCamTotalTime);
	m_spinCamCurrentRotation += rotationAmount;
	if (m_spinCamCurrentRotation < m_spinCamTotalRotation)
	{
		DirectX::SimpleMath::Quaternion quatRot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_spinCamAxis, rotationAmount);
		DirectX::SimpleMath::Vector3 localCamPos = m_position - m_spinCamAxisPos;
		localCamPos = DirectX::SimpleMath::Vector3::Transform(localCamPos, quatRot);
		localCamPos += m_spinCamAxisPos;
		m_position = localCamPos;

		DirectX::SimpleMath::Vector3 targetStartPos = m_target;
		targetStartPos = m_spinStartTarget;
		DirectX::SimpleMath::Vector3 targetEndPos = m_vehicleFocus->GetPos() + m_followCamTargOffset;
		float targetDistance = DirectX::SimpleMath::Vector3::Distance(targetStartPos, targetEndPos);
		DirectX::SimpleMath::Vector3 targetDirection = targetEndPos - targetStartPos;
		targetDirection.Normalize();

		double elapsedTime = double(aTimer.GetElapsedSeconds());
		float cameraSpeed = targetDistance / m_spinCamTotalTime;
		float targetSpeed;
		if (abs(targetDistance > 0.0)) // prevent divide by zero if camera position doesn't change
		{
			targetSpeed = cameraSpeed * (targetDistance / targetDistance);
		}
		else
		{
			targetSpeed = cameraSpeed;
		}
		if (targetDistance > 0.0f)
		{
			m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
		}
	}
	else
	{
		m_cameraState = CameraState::CAMERASTATE_FOLLOWVEHICLE;
	}
}

void Camera::SetSpinCameraStartGamePlayStart(const float aTime)
{
	m_spinStartTarget = m_target;
	m_spinCamStartPos = m_position;
	m_spinCamEndPos = m_followCamPos;
	m_spinCamEndPos.y += 3.0f;
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-16.9999580, 6.51245356, -0.0163976531);
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-17.0, 7.044, 0.0);
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-16.999981, 6.779059, 0.0);
	m_spinCamAxisPos = m_spinCamEndPos + m_spinCamStartPos;
	m_spinCamAxisPos *= 0.5f;
	m_spinCamAxis = m_spinCamEndPos - m_spinCamStartPos;
	m_spinCamAxis.Normalize();
	m_spinCamAxis = m_spinCamAxis.Cross(DirectX::SimpleMath::Vector3::UnitZ);
	m_spinCamTotalRotation = Utility::ToRadians(-180.0f);
	m_spinCamCurrentRotation = 0.0f;
	m_spinCamTotalTime = aTime;
}

void Camera::SetSpinCameraTrailerStart(const float aTime)
{
	m_spinStartTarget = m_target;
	m_spinCamStartPos = m_position;
	m_spinCamEndPos = m_followCamPos;
	m_spinCamEndPos.y += 3.0f;
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-16.9999580, 6.51245356, -0.0163976531);
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-17.0, 7.044, 0.0);
	m_spinCamEndPos = DirectX::SimpleMath::Vector3(-16.999981, 6.779059, 0.0);
	m_spinCamEndPos = m_trailerCamEndPos;

	m_spinCamAxisPos = m_spinCamEndPos + m_spinCamStartPos;

	m_spinCamAxisPos *= 0.5f;

	m_spinCamAxis = m_spinCamEndPos - m_spinCamStartPos;
	m_spinCamAxis.Normalize();
	m_spinCamAxis = m_spinCamAxis.Cross(DirectX::SimpleMath::Vector3::UnitZ);
	m_spinCamTotalRotation = Utility::ToRadians(-180.0f);
	m_spinCamCurrentRotation = 0.0f;
	m_spinCamTotalTime = aTime;
}

void Camera::UpdateTransitionCamera(DX::StepTimer const& aTimer)
{
	DirectX::SimpleMath::Vector3 cameraStartPos = m_cameraStartPos;
	DirectX::SimpleMath::Vector3 cameraEndPos = m_cameraEndPos;

	float cameraDistance = DirectX::SimpleMath::Vector3::Distance(cameraStartPos, cameraEndPos);
	DirectX::SimpleMath::Vector3 cameraDirection = cameraEndPos - cameraStartPos;

	cameraDirection.Normalize();

	DirectX::SimpleMath::Vector3 targetStartPos = m_targetStartPos;
	DirectX::SimpleMath::Vector3 targetEndPos = m_targetEndPos;

	float targetDistance = DirectX::SimpleMath::Vector3::Distance(targetStartPos, targetEndPos);
	DirectX::SimpleMath::Vector3 targetDirection = targetEndPos - targetStartPos;
	targetDirection.Normalize();

	double elapsedTime = double(aTimer.GetElapsedSeconds());
	float cameraSpeed = m_cameraTransitionSpeed;

	float targetSpeed;
	if (abs(cameraDistance > 0.0f)) // prevent divide by zero if camera position doesn't change
	{
		targetSpeed = cameraSpeed * (targetDistance / cameraDistance);
	}
	else
	{
		targetSpeed = cameraSpeed;
	}

	m_position += cameraDirection * cameraSpeed * static_cast<float>(elapsedTime);

	if (targetDistance > 0.0f)
	{
		m_target += targetDirection * targetSpeed * static_cast<float>(elapsedTime);
	}

	m_up = DirectX::SimpleMath::Vector3::UnitY;

	if (DirectX::SimpleMath::Vector3::Distance(cameraStartPos, m_position) >= cameraDistance && DirectX::SimpleMath::Vector3::Distance(targetStartPos, m_target) >= targetDistance)
	{
		m_position = cameraEndPos;
		m_isCameraAtDestination = true;
	}
	else
	{
		m_cameraState = CameraState::CAMERASTATE_SPRINGCAMERA;
	}
}

void Camera::ComputeSpringMatrix()
{
	DirectX::SimpleMath::Vector3 cameraForward = m_springTarget.position - m_actualPosition;
	cameraForward.Normalize();
	DirectX::SimpleMath::Vector3 cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	DirectX::SimpleMath::Vector3 cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_actualPosition, m_springTarget.position, cameraUp);
}

void Camera::InitializeSpringCamera(Target aTarget, float aSpringConstant, float ahDist, float aVDist)
{
	m_springTarget = aTarget;
	m_springConstant = aSpringConstant;
	m_hDistance = ahDist;
	m_vDistance = aVDist;

	m_dampConstant = 2.0f * sqrt(m_springConstant);
	m_actualPosition = m_springTarget.position - m_springTarget.forward * ahDist + m_springTarget.up * aVDist;
	m_velocity = DirectX::SimpleMath::Vector3::Zero;
	ComputeSpringMatrix();
}

DirectX::SimpleMath::Vector3 Camera::GetSpringCameraTarget()
{
	DirectX::SimpleMath::Vector3 updatedTarget = DirectX::SimpleMath::Vector3::Zero;
	if (m_transitionTimer < m_transitionTimeMax)
	{
		DirectX::SimpleMath::Vector3 startTargPos1 = m_target;
		DirectX::SimpleMath::Vector3 endTargPos1 = m_npcController->GetNpcPos(m_npcFocusID);
		const float targPosDistance = DirectX::SimpleMath::Vector3::Distance(startTargPos1, endTargPos1);
		DirectX::SimpleMath::Vector3 targetDirection = endTargPos1 - startTargPos1;
		targetDirection.Normalize();
		float targetSpeed = targPosDistance / m_transitionTimeMax;

		double elapsedTime = m_transitionTimer;
		if (targPosDistance > 0.0f)
		{
			DirectX::SimpleMath::Vector3 posChange = (targetDirection * targetSpeed * static_cast<float>(elapsedTime));
			updatedTarget = m_target + (targetDirection * targetSpeed * static_cast<float>(elapsedTime));
		}

	}
	else
	{
		updatedTarget = m_npcController->GetNpcPos(m_npcFocusID);
	}

	return updatedTarget;
}

void Camera::UpdateSpringCamera(DX::StepTimer const& aTimeDelta)
{
	m_transitionTimer += static_cast<float>(aTimeDelta.GetElapsedSeconds());
	DirectX::SimpleMath::Vector3 vehiclePos = GetSpringCameraTarget();
	m_springTarget.position = vehiclePos;
	DirectX::SimpleMath::Vector3 testHeading = DirectX::SimpleMath::Vector3::UnitX;
	if (m_transitionTimer < m_transitionTimeMax)
	{
		testHeading *= m_transitionTimer / m_transitionTimeMax;
	}
	
	DirectX::SimpleMath::Matrix rotMat = m_npcController->GetNpcAlignment(m_npcFocusID);
	DirectX::SimpleMath::Matrix testRotMat = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3::Zero, m_target, m_up);
	DirectX::SimpleMath::Quaternion rotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(rotMat);
	DirectX::SimpleMath::Quaternion testRotQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_viewMatrix);
	DirectX::SimpleMath::Quaternion rotQuat2 = DirectX::SimpleMath::Quaternion::Slerp(rotQuat, testRotQuat, 0.1f);
	testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);
	m_springTarget.forward = testHeading;

	DirectX::SimpleMath::Vector3 idealPosition = m_springTarget.position - m_springTarget.forward * m_hDistance + m_springTarget.up * m_vDistance;
	DirectX::SimpleMath::Vector3 displacement = m_actualPosition - idealPosition;
	DirectX::SimpleMath::Vector3 springAccel = (-m_springConstant * displacement) - (m_dampConstant * m_velocity);
	m_velocity += springAccel * static_cast<float>(aTimeDelta.GetElapsedSeconds());
	m_actualPosition += m_velocity * static_cast<float>(aTimeDelta.GetElapsedSeconds());

	m_position = m_actualPosition;
	m_target = m_springTarget.position;
	ComputeSpringMatrix();

	m_viewMatrix = m_springCameraMatrix;
	m_transitionTimer += static_cast<float>(aTimeDelta.GetElapsedSeconds());
}

void Camera::UpdateSpringCameraPlayer(DX::StepTimer const& aTimeDelta)
{
	DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleFocus->GetPos(), -m_vehicleFocus->GetVehicleRight(), m_vehicleFocus->GetVehicleUp());
	DirectX::SimpleMath::Matrix localizationMatrix = worldMatrix;
	localizationMatrix = localizationMatrix.Invert();

	DirectX::SimpleMath::Vector3 vehiclePos = m_vehicleFocus->GetPos();
	vehiclePos += (m_vehicleFocus->GetForward() * 5.0f);
	m_springTarget.position = vehiclePos;
	//m_springTarget.position = vehiclePos + (m_vehicleFocus->GetWeaponDirection() * 10.0f);
	m_springTarget.position.y += m_vehicleFocus->GetWeaponPitch() * 10.0f;
	

	DirectX::SimpleMath::Vector3 weaponRight = m_vehicleFocus->GetWeaponDirection().Cross(m_vehicleFocus->GetVehicleUp());
	DirectX::SimpleMath::Matrix pitchRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(weaponRight, m_vehicleFocus->GetWeaponPitch());
	DirectX::SimpleMath::Matrix yawRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleFocus->GetVehicleUp(), m_vehicleFocus->GetTurretYaw());
	DirectX::SimpleMath::Matrix rotMat = m_vehicleFocus->GetAlignment();
	
	rotMat *= yawRotMat;
	rotMat *= pitchRotMat;
	DirectX::SimpleMath::Vector3 testHeading = DirectX::SimpleMath::Vector3::UnitX;
	//testHeading = m_vehicleFocus->GetWeaponDirection();
	testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);
	//testHeading = m_vehicleFocus->GetWeaponDirection();
	m_springTarget.forward = testHeading;

	DirectX::SimpleMath::Vector3 idealPosition = m_springTarget.position - m_springTarget.forward * m_hDistance + m_springTarget.up * m_vDistance;
	DirectX::SimpleMath::Vector3 displacement = m_actualPosition - idealPosition;
	DirectX::SimpleMath::Vector3 springAccel = (-m_springConstant * displacement) - (m_dampConstant * m_velocity);
	m_velocity += springAccel * static_cast<float>(aTimeDelta.GetElapsedSeconds());
	m_actualPosition += m_velocity * static_cast<float>(aTimeDelta.GetElapsedSeconds());

	DirectX::SimpleMath::Vector3 testTarget = m_springTarget.position - m_actualPosition;
	testTarget = DirectX::SimpleMath::Vector3::Transform(testTarget, DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleFocus->GetTurretYaw() * 0.1f));
	testTarget += m_actualPosition;
	m_springTarget.position = testTarget;
	//m_debugData->DebugPushTestLinePositionIndicator(m_springTarget.position, 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//m_debugData->DebugPushTestLinePositionIndicator(m_vehicleFocus->GetPos(), 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_debugData->DebugPushTestLinePositionIndicator(m_vehicleFocus->GetWeaponPos(), 5.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_debugData->DebugPushUILineDecimalNumber("Yaw = ", m_vehicleFocus->GetTurretYaw(), "");
	DirectX::SimpleMath::Vector3 testActual =  m_actualPosition - m_springTarget.position;
	testActual = DirectX::SimpleMath::Vector3::Transform(testActual, DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleFocus->GetTurretYaw()));
	testActual += m_springTarget.position;
	//m_actualPosition = testActual;

	//m_actualPosition = DirectX::SimpleMath::Vector3::Transform(m_actualPosition, yawRotMat);
	m_position = m_actualPosition;
	m_target = m_springTarget.position;
	//ComputeSpringMatrix();
	DirectX::SimpleMath::Vector3 cameraForward = m_springTarget.position - m_actualPosition;
	cameraForward.Normalize();
	DirectX::SimpleMath::Vector3 cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	DirectX::SimpleMath::Vector3 cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_actualPosition, m_springTarget.position, cameraUp);

	DirectX::SimpleMath::Vector3 localActualPosition = DirectX::SimpleMath::Vector3::Transform(m_actualPosition, localizationMatrix);
	//localActualPosition = DirectX::SimpleMath::Vector3(-19.0f, 7.0f, 0.0f);
	DirectX::SimpleMath::Matrix weaponRotMat = DirectX::SimpleMath::Matrix::Identity;
	weaponRotMat *= yawRotMat;
	weaponRotMat *= pitchRotMat;
	localActualPosition = DirectX::SimpleMath::Vector3::Transform(localActualPosition, weaponRotMat);
	DirectX::SimpleMath::Vector3 worldActualPosition = DirectX::SimpleMath::Vector3::Transform(localActualPosition, worldMatrix);

	cameraForward = m_springTarget.position - worldActualPosition;
	cameraForward.Normalize();
	cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	//m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(worldActualPosition, m_springTarget.position, cameraUp);

	m_viewMatrix = m_springCameraMatrix;
}

void Camera::UpdateSpringCameraPlayer2(DX::StepTimer const& aTimeDelta)
{
	m_transitionTimer += static_cast<float>(aTimeDelta.GetElapsedSeconds());
	DirectX::SimpleMath::Vector3 vehiclePos = m_vehicleFocus->GetPos();
	m_springTarget.position = vehiclePos;
	//m_springTarget.position = vehiclePos + (m_vehicleFocus->GetWeaponDirection() * 10.0f);

	DirectX::SimpleMath::Vector3 weaponRight = m_vehicleFocus->GetWeaponDirection().Cross(m_vehicleFocus->GetVehicleUp());
	DirectX::SimpleMath::Matrix pitchRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(weaponRight, m_vehicleFocus->GetWeaponPitch());
	DirectX::SimpleMath::Matrix yawRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleFocus->GetVehicleUp(), m_vehicleFocus->GetTurretYaw());
	DirectX::SimpleMath::Matrix rotMat = m_vehicleFocus->GetAlignment();

	//rotMat *= yawRotMat;
	//rotMat *= pitchRotMat;
	DirectX::SimpleMath::Vector3 testHeading = DirectX::SimpleMath::Vector3::UnitX;
	testHeading = m_vehicleFocus->GetWeaponDirection();
	testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);
	//testHeading = m_vehicleFocus->GetWeaponDirection();
	m_springTarget.forward = testHeading;

	DirectX::SimpleMath::Vector3 idealPosition = m_springTarget.position - m_springTarget.forward * m_hDistance + m_springTarget.up * m_vDistance;
	DirectX::SimpleMath::Vector3 displacement = m_actualPosition - idealPosition;
	DirectX::SimpleMath::Vector3 springAccel = (-m_springConstant * displacement) - (m_dampConstant * m_velocity);
	m_velocity += springAccel * static_cast<float>(aTimeDelta.GetElapsedSeconds());
	m_actualPosition += m_velocity * static_cast<float>(aTimeDelta.GetElapsedSeconds());

	//m_actualPosition = DirectX::SimpleMath::Vector3::Transform(m_actualPosition, yawRotMat);
	m_position = m_actualPosition;
	m_target = m_springTarget.position;
	//ComputeSpringMatrix();
	DirectX::SimpleMath::Vector3 cameraForward = m_springTarget.position - m_actualPosition;
	cameraForward.Normalize();
	DirectX::SimpleMath::Vector3 cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	DirectX::SimpleMath::Vector3 cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_actualPosition, m_springTarget.position, cameraUp);

	//DirectX::SimpleMath::Vector3 testCamPos = m_actualPosition + (-m_vehicleFocus->GetWeaponDirection() * 1.0f);
	//m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(testCamPos, m_springTarget.position, cameraUp);

	m_viewMatrix = m_springCameraMatrix;
}

void Camera::UpdateSpringCameraPlayer3(DX::StepTimer const& aTimeDelta)
{
	DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::CreateWorld(m_vehicleFocus->GetPos(), -m_vehicleFocus->GetVehicleRight(), m_vehicleFocus->GetVehicleUp());
	DirectX::SimpleMath::Matrix localizationMatrix = worldMatrix;
	localizationMatrix = localizationMatrix.Invert();

	DirectX::SimpleMath::Vector3 vehiclePos = m_vehicleFocus->GetPos();
	m_springTarget.position = vehiclePos;
	//m_springTarget.position = vehiclePos + (m_vehicleFocus->GetWeaponDirection() * 10.0f);

	DirectX::SimpleMath::Vector3 weaponRight = m_vehicleFocus->GetWeaponDirection().Cross(m_vehicleFocus->GetVehicleUp());
	DirectX::SimpleMath::Matrix pitchRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(weaponRight, m_vehicleFocus->GetWeaponPitch());
	DirectX::SimpleMath::Matrix yawRotMat = DirectX::SimpleMath::Matrix::CreateFromAxisAngle(m_vehicleFocus->GetVehicleUp(), m_vehicleFocus->GetTurretYaw());
	DirectX::SimpleMath::Matrix rotMat = m_vehicleFocus->GetAlignment();

	//rotMat *= yawRotMat;
	//rotMat *= pitchRotMat;
	DirectX::SimpleMath::Vector3 testHeading = DirectX::SimpleMath::Vector3::UnitX;
	//testHeading = m_vehicleFocus->GetWeaponDirection();
	testHeading = DirectX::SimpleMath::Vector3::Transform(testHeading, rotMat);
	//testHeading = m_vehicleFocus->GetWeaponDirection();
	m_springTarget.forward = testHeading;

	DirectX::SimpleMath::Vector3 idealPosition = m_springTarget.position - m_springTarget.forward * m_hDistance + m_springTarget.up * m_vDistance;
	DirectX::SimpleMath::Vector3 displacement = m_actualPosition - idealPosition;
	DirectX::SimpleMath::Vector3 springAccel = (-m_springConstant * displacement) - (m_dampConstant * m_velocity);
	m_velocity += springAccel * static_cast<float>(aTimeDelta.GetElapsedSeconds());
	m_actualPosition += m_velocity * static_cast<float>(aTimeDelta.GetElapsedSeconds());

	//m_actualPosition = DirectX::SimpleMath::Vector3::Transform(m_actualPosition, yawRotMat);
	m_position = m_actualPosition;
	m_target = m_springTarget.position;
	//ComputeSpringMatrix();
	DirectX::SimpleMath::Vector3 cameraForward = m_springTarget.position - m_actualPosition;
	cameraForward.Normalize();
	DirectX::SimpleMath::Vector3 cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	DirectX::SimpleMath::Vector3 cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(m_actualPosition, m_springTarget.position, cameraUp);

	DirectX::SimpleMath::Vector3 localActualPosition = DirectX::SimpleMath::Vector3::Transform(m_actualPosition, localizationMatrix);
	//localActualPosition = DirectX::SimpleMath::Vector3(-19.0f, 7.0f, 0.0f);
	DirectX::SimpleMath::Matrix weaponRotMat = DirectX::SimpleMath::Matrix::Identity;
	weaponRotMat *= yawRotMat;
	weaponRotMat *= pitchRotMat;
	localActualPosition = DirectX::SimpleMath::Vector3::Transform(localActualPosition, weaponRotMat);
	DirectX::SimpleMath::Vector3 worldActualPosition = DirectX::SimpleMath::Vector3::Transform(localActualPosition, worldMatrix);

	cameraForward = m_springTarget.position - worldActualPosition;
	cameraForward.Normalize();
	cameraLeft = m_springTarget.up.Cross(cameraForward);
	cameraLeft.Normalize();
	cameraUp = cameraForward.Cross(cameraLeft);
	cameraUp.Normalize();
	m_springCameraMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(worldActualPosition, m_springTarget.position, cameraUp);

	m_viewMatrix = m_springCameraMatrix;
}

void Camera::UpdateChaseCameraNPC()
{
	SetUpPos(m_followCamUp);
	DirectX::SimpleMath::Vector3 prevTarg = m_target;
	DirectX::SimpleMath::Vector3 targetPitch = DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f);
	targetPitch = DirectX::SimpleMath::Vector3::Transform(targetPitch, m_npcController->GetNpcAlignment(m_npcFocusID));
	DirectX::SimpleMath::Vector3 targetPos = m_npcController->GetNpcPos(m_npcFocusID) + m_followCamTargOffset;

	DirectX::SimpleMath::Vector3 updatedTarget = DirectX::SimpleMath::Vector3::SmoothStep(prevTarg, targetPos, 0.17);
	SetTargetPos(targetPos);
	targetPos = m_npcController->GetNpcPos(m_npcFocusID);

	DirectX::SimpleMath::Quaternion orientationQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_npcController->GetNpcAlignment(m_npcFocusID));
	DirectX::SimpleMath::Quaternion orientationQuat2 = orientationQuat;
	orientationQuat2.Inverse(orientationQuat2);
	m_chaseCamQuat = DirectX::SimpleMath::Quaternion::Slerp(m_chaseCamQuat, orientationQuat, 0.9041f);
	DirectX::SimpleMath::Vector3 preCamPosition = m_position;
	DirectX::SimpleMath::Vector3 accelCamPos = m_followNpcCamPos;
	DirectX::SimpleMath::Vector3 accelVec = m_npcController->GetNpcAccelVecTest(m_npcFocusID) * 0.1f;
	accelVec = DirectX::SimpleMath::Vector3::UnitX;
	accelVec = DirectX::SimpleMath::Vector3::Transform(accelVec, orientationQuat2);
	accelCamPos += accelVec;
	accelCamPos = DirectX::SimpleMath::Vector3::Lerp(accelCamPos, m_followNpcCamPos, 0.0001);
	accelCamPos = DirectX::SimpleMath::Vector3::Transform(accelCamPos, m_chaseCamQuat);

	accelCamPos += m_npcController->GetNpcPos(m_npcFocusID);

	DirectX::SimpleMath::Vector3 newCamPosition = DirectX::SimpleMath::Vector3::SmoothStep(preCamPosition, accelCamPos, 0.1);
	SetPos(newCamPosition);
}

void Camera::UpdateChaseCamera()
{
	SetUpPos(m_followCamUp);
	DirectX::SimpleMath::Vector3 targetPitch = DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f);
	//targetPitch = DirectX::SimpleMath::Vector3::Transform(targetPitch, DirectX::SimpleMath::Matrix::CreateRotationZ(m_vehicleFocus->GetWeaponPitch()));
	targetPitch = DirectX::SimpleMath::Vector3::Transform(targetPitch, DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleFocus->GetTurretYaw()));
	targetPitch = DirectX::SimpleMath::Vector3::Transform(targetPitch, m_vehicleFocus->GetVehicleOrientation());
	DirectX::SimpleMath::Vector3 targetPos = m_vehicleFocus->GetPos() + m_followCamTargOffset;
	targetPos += targetPitch;
	SetTargetPos(targetPos);

	// this is causing to much camera shake with weapon recoil
	DirectX::SimpleMath::Vector3 weaponPos = m_vehicleFocus->GetWeaponPos();
	const float targetDistanceMod = 30.0f;
	DirectX::SimpleMath::Vector3 weaponDir = m_vehicleFocus->GetWeaponDirection() * targetDistanceMod;
	targetPos = weaponDir;
	targetPos += weaponPos;
	//SetTargetPos(targetPos);
	//m_debugData->DebugPushTestLinePositionIndicator(targetPos, 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//m_debugData->DebugPushTestLine(m_vehicleFocus->GetWeaponPos(), m_vehicleFocus->GetWeaponDirection(), 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//m_debugData->DebugPushTestLinePositionIndicator(m_vehicleFocus->GetWeaponPos(), 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//m_debugData->DebugPushTestLinePositionIndicator(m_vehicleFocus->GetWeaponDirection(), 25.0f, 0.0f, DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	const float pitchSensitivity = 0.5f;
	DirectX::SimpleMath::Quaternion orientationQuat = DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(
		DirectX::SimpleMath::Matrix::CreateRotationZ(pitchSensitivity * m_vehicleFocus->GetWeaponPitch()));
	orientationQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleFocus->GetTurretYaw()));
	orientationQuat *= DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(m_vehicleFocus->GetVehicleOrientation());

	m_chaseCamQuat = orientationQuat;
	m_chaseCamQuat = DirectX::SimpleMath::Quaternion::Slerp(m_chaseCamQuat, orientationQuat, 0.01f);
	DirectX::SimpleMath::Vector3 preCamPosition = m_position;
	DirectX::SimpleMath::Vector3 accelCamPos = m_followCamPos;
	DirectX::SimpleMath::Vector3 accelVec = m_vehicleFocus->GetAccelVec() * 0.01f;
	accelCamPos += accelVec;
	accelCamPos = DirectX::SimpleMath::Vector3::Lerp(accelCamPos, m_followCamPos, 0.0001);
	accelCamPos = DirectX::SimpleMath::Vector3::Transform(accelCamPos, m_chaseCamQuat);
	accelCamPos += m_vehicleFocus->GetPos();
	DirectX::SimpleMath::Vector3 newCamPosition = DirectX::SimpleMath::Vector3::SmoothStep(preCamPosition, accelCamPos, 0.15);
	SetPos(newCamPosition);

	/*
	DirectX::SimpleMath::Vector3 newCamPosition = m_followCamPos;
	const float pitchSensitivity = 0.5f;
	newCamPosition = DirectX::SimpleMath::Vector3::Transform(newCamPosition, DirectX::SimpleMath::Matrix::CreateRotationZ(pitchSensitivity * m_vehicleFocus->GetWeaponPitch()));
	newCamPosition = DirectX::SimpleMath::Vector3::Transform(newCamPosition, DirectX::SimpleMath::Matrix::CreateRotationY(m_vehicleFocus->GetTurretYaw()));
	newCamPosition = DirectX::SimpleMath::Vector3::Transform(newCamPosition, m_vehicleFocus->GetVehicleOrientation());
	newCamPosition += m_vehicleFocus->GetPos();
	SetPos(newCamPosition);
	*/
}

void Camera::UpdatePitchYaw(const float aPitch, const float aYaw)
{
	m_pitch += aPitch * m_rotationTravelSpeed;
	m_yaw += aYaw * m_rotationTravelSpeed;

	// keep longitude in sane range by wrapping
	if (m_yaw > DirectX::XM_PI)
	{
		m_yaw -= DirectX::XM_PI * 2.0f;
	}
	else if (m_yaw < -DirectX::XM_PI)
	{
		m_yaw += DirectX::XM_PI * 2.0f;
	}

	// limit pitch to straight up or straight down
	// with a little fudge-factor to avoid gimbal lock
	float limit = DirectX::XM_PI / 2.0f - 0.01f;
	m_pitch = std::max(-limit, m_pitch);
	m_pitch = std::min(+limit, m_pitch);
}

void Camera::UpdatePos(const float aX, const float aY, const float aZ)
{
	m_moveLeftRight += aX * m_posTravelSpeed;
	m_moveBackForward += aZ * m_posTravelSpeed;
	m_moveUpDown += aY * m_posTravelSpeed;
}

void Camera::UpdateOrthoganalMatrix()
{
	m_orthogonalMatrix = DirectX::SimpleMath::Matrix::CreateOrthographic((float)m_clientWidth, (float)m_clientHeight, m_nearPlane, m_farPlane);
}

void Camera::UpdateProjectionMatrix()
{
	m_projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4.f, (float)m_clientWidth / (float)m_clientHeight, m_nearPlane, m_farPlane);
}

void Camera::UpdateViewMatrix()
{
	DirectX::SimpleMath::Vector3 newPosition = DirectX::SimpleMath::Vector3::Transform(m_position, DirectX::SimpleMath::Matrix::CreateRotationY(m_yaw));
	m_viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(newPosition, m_target, m_up);
}

void Camera::YawSpin(float aTurn)
{
	Utility::WrapAngle(m_yaw += aTurn * m_cameraTransitionSpeed);
}

void Camera::TurnAroundPoint(float aTurn, DirectX::SimpleMath::Vector3 aCenterPoint)
{
	DirectX::SimpleMath::Vector3 updateTarget = m_target;
	DirectX::SimpleMath::Vector3 updateCamPos = m_position;

	updateTarget -= aCenterPoint;
	updateCamPos -= aCenterPoint;

	DirectX::SimpleMath::Matrix rotMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(aTurn * m_aimTurnRate);
	updateTarget = DirectX::SimpleMath::Vector3::Transform(updateTarget, rotMatrix);
	updateCamPos = DirectX::SimpleMath::Vector3::Transform(updateCamPos, rotMatrix);

	updateTarget += aCenterPoint;
	updateCamPos += aCenterPoint;

	m_target = updateTarget;
	m_position = updateCamPos;
}

void Camera::TurnEndPosAroundPoint(float aTurn, DirectX::SimpleMath::Vector3 aCenterPoint)
{
	DirectX::SimpleMath::Vector3 updateTarget = m_targetEndPos;
	DirectX::SimpleMath::Vector3 updateCamPos = m_cameraEndPos;

	updateTarget -= aCenterPoint;
	updateCamPos -= aCenterPoint;

	DirectX::SimpleMath::Matrix rotMatrix = DirectX::SimpleMath::Matrix::CreateRotationY(aTurn);
	updateTarget = DirectX::SimpleMath::Vector3::Transform(updateTarget, rotMatrix);
	updateCamPos = DirectX::SimpleMath::Vector3::Transform(updateCamPos, rotMatrix);

	updateTarget += aCenterPoint;
	updateCamPos += aCenterPoint;

	m_targetEndPos = updateTarget;
	m_cameraEndPos = updateCamPos;
}