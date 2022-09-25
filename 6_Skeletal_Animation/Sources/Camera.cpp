#include "Camera.h"

Camera::Camera()
{
	mPos = vec3(0.0f, 0.0f, 3.0f);
	mForward = vec3(0.0f, 0.0f, -1.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);

	mLastFrame = 0.0f;
	mDeltaTime = 0.0f;

	mFov = 45.0f;

	mYaw = -90.0f;
	mPitch = 0.0f;

	mFirstMouse = true;
}

Camera::Camera(vec3 pos, vec3 forward, vec3 up)
	: mPos(pos), mForward(forward), mUp(up)
{
	mLastFrame = 0.0f;
	mDeltaTime = 0.0f;

	mFov = 45.0f;

	mYaw = -90.0f;
	mPitch = 0.0f;

	mFirstMouse = true;
}

Camera::~Camera() {}

void Camera::ProcessInput()
{
	const float cameraSpeed = 2.5f * mDeltaTime;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	if (keyState[SDL_SCANCODE_W])
	{
		mPos += cameraSpeed * mForward;
	}
	else if (keyState[SDL_SCANCODE_S])
	{
		mPos -= cameraSpeed * mForward;
	}
	else if (keyState[SDL_SCANCODE_A])
	{
		vec3 cameraRight = normalize(cross(mForward, mUp));
		mPos -= cameraSpeed * cameraRight;
	}
	else if (keyState[SDL_SCANCODE_D])
	{
		vec3 cameraRight = normalize(cross(mForward, mUp));
		mPos += cameraSpeed * cameraRight;
	}
}

void Camera::ProcessMouseDown(SDL_Event mouseEvent)
{
	if (mouseEvent.button.button == SDL_BUTTON_LEFT)
	{
		//printf("Left mouse button click! \n");
		SDL_ShowCursor(SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_TRUE);

		int x, y;

		SDL_GetRelativeMouseState(&x, &y);


		float sensitivity = 0.1f;
		
		m_x_offset = static_cast<float>(x) * sensitivity;
		m_y_offset = -static_cast<float>(y) * sensitivity;

		if (mFirstMouse)
		{
			m_x_offset = 0.0f;
			m_y_offset = 0.0f;
			mFirstMouse = false;
		}

		mYaw += m_x_offset;
		mPitch += m_y_offset;

		if (mPitch > 89.0f)
		{
			mPitch = 89.0f;
		}
		else if (mPitch < -89.0f)
		{
			mPitch = -89.0f;
		}

		/*printf("Yaw: %f ", mYaw);
		printf("Pitch: %f \n", mPitch);*/

		UpdateCamera();
	}
	else
	{
		// Do nothing, if right button is clicked
		return;
	}
}

void Camera::ProcessMouseUp(SDL_Event mouseEvent, SDL_Window* window, int x, int y)
{
	//printf("Mouse button up!\n");
	SDL_ShowCursor(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	mFirstMouse = true;
	if (m_x_offset == 0 && m_y_offset == 0) return;
	SDL_WarpMouseInWindow(window, x, y);
}

void Camera::UpdateCamera()
{
	vec3 forward;
	forward.x = cos(radians(mYaw)) * cos(radians(mPitch));
	forward.y = sin(radians(mPitch));
	forward.z = sin(radians(mYaw)) * cos(radians(mPitch));


	mForward = normalize(forward);
	//printf("forward: %f, %f, %f \n", mForward.x, mForward.y, mForward.z);
}

mat4 Camera::MyLookAt()
{
	vec3 right = normalize(cross(mForward, mUp));

	vec3 newUp = cross(right, mForward);

	mat4 rotation = { right.x, newUp.x, -mForward.x, 0,
					  right.y, newUp.y, -mForward.y, 0,
					  right.z, newUp.z, -mForward.z, 0,
						0,		 0,	       0,	   1 };

	mat4 translation = mat4(1.0f);
	translation[3][0] = -mPos.x;
	translation[3][1] = -mPos.y;
	translation[3][2] = -mPos.z;

	mat4 view = rotation * translation;

	return view;
}
