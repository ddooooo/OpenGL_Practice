#pragma once

#include <iostream>
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
	
	bool m_button_up = false;

	float m_x_offset = 0;
	float m_y_offset = 0;

	float mDeltaTime;
	float mLastFrame;

public:
	Camera();
	Camera(vec3 pos, vec3 forward, vec3 up);
	~Camera();
	void ProcessInput();
	void ProcessMouseDown(SDL_Event mouseEvent);
	void ProcessMouseUp(SDL_Event mouseEvent, SDL_Window* window, int x, int y);
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