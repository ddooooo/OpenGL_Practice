#pragma once

#include <iostream>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

class Camera
{
private:
	glm::mat4 m_projection;

	glm::vec3 m_pos;
	glm::vec3 m_forward;
	glm::vec3 m_up;

	float m_aspect;

	float m_yaw;
	float m_pitch;
	float m_FoV;

	float m_delta_time;
	float m_last_frame;

	bool m_first_click;

	float m_x_offset;
	float m_y_offset;

public:
	Camera(glm::vec3 pos, float aspect, float FoV, float yaw, float pitch);
	~Camera();

	void processInput();
	void processMouseDown(SDL_Event event);
	void processMouseUp(SDL_Event event, SDL_Window* window, int x, int y);
	void updateCamera();

	glm::mat4 MyLookAt();

	float getDeltaTime() { return m_delta_time; };
	float getLastFrame() { return m_last_frame; };

	void updateProjection(float aspect) { m_projection = glm::perspective(glm::radians(m_FoV), aspect, 0.1f, 100.0f); };
	glm::mat4 getProjection() { return m_projection; };

	glm::vec3 getPos() { return m_pos; };
	glm::vec3 getForward() { return m_forward; };
	glm::vec3 getUp() { return m_up; };

	void setDeltaTime(float delta_time) { m_delta_time = delta_time; };
	void setLastFrame(float last_frame) { m_last_frame = last_frame; };
};