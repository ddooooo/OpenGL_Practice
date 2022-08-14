#pragma once

#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

using namespace glm;

class Camera
{
private:
	vec3 mPos;
	vec3 mForward;
	vec3 mUp;

	float mYaw;
	float mPitch;
	float mFov;

	bool mFirstMouse;

	float mDeltaTime;
	float mLastFrame;

public:
	Camera();
	~Camera();
	void ProcessInput();
	void ProcessMouseMotion(SDL_Event mouseEvent);
	void UpdateCamera();
	mat4 MyLookAt();

	// Getter
	float GetDeltaTime() { return mDeltaTime; };
	float GetLastFrame() { return mLastFrame; };
	float GetFov() { return mFov; };

	vec3 GetPos() { return mPos; };
	vec3 GetForward() { return mForward; };
	vec3 GetUp() { return mUp; };

	// Setter
	void SetDeltaTime(float deltaTime) { mDeltaTime = deltaTime; };
	void SetLastFrame(float lastFrame) { mLastFrame = lastFrame; };


};