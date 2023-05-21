#include "Camera.h"

Camera::Camera(glm::vec3 pos, float aspect, float FoV, float yaw, float pitch) :
	m_projection(0), m_pos(pos), m_forward(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f),
	m_FoV(FoV), m_yaw(yaw), m_pitch(pitch), m_x_offset(0.0f), m_y_offset(0.0f),
	m_last_frame(0.0f), m_delta_time(0.0f), m_first_click(true), m_aspect(0.0f) 
{
	updateProjection(m_aspect);
}

Camera::~Camera() {}

void Camera::processInput()
{
	const float speed = 2.5f * m_delta_time;
	const Uint8* state = SDL_GetKeyboardState(NULL);

	glm::vec3 right = normalize(cross(m_forward, m_up));

	if (state[SDL_SCANCODE_W])
		m_pos += speed * m_forward; // Move forward
	else if (state[SDL_SCANCODE_S])
		m_pos -= speed * m_forward; // Move Backward
	else if (state[SDL_SCANCODE_A])
		m_pos -= speed * right;
	else if (state[SDL_SCANCODE_D])
		m_pos += speed * right;
}

void Camera::processMouseDown(SDL_Event event)
{
	if (event.button.button == SDL_BUTTON_LEFT)
	{
		SDL_ShowCursor(SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_TRUE);

		int x, y;

		SDL_GetRelativeMouseState(&x, &y);

		float sensitivity = 0.1f;

		m_x_offset = static_cast<float>(x) * sensitivity;
		m_y_offset = static_cast<float>(y) * sensitivity;

		if (m_first_click)
		{
			m_x_offset = 0.0f;
			m_y_offset = 0.0f;
			m_first_click = false;
		}

		m_yaw += m_x_offset;
		m_pitch += m_y_offset;

		if (m_pitch > 89.0f)
		{
			m_pitch = 89.0f;
		}
		else if (m_pitch < -89.0f)
		{
			m_pitch = -89.0f;
		}

		updateCamera();
	}
	else
	{
		return;
	}
}

void Camera::processMouseUp(SDL_Event event, SDL_Window* window, int x, int y)
{
	SDL_ShowCursor(SDL_TRUE);
	SDL_SetRelativeMouseMode(SDL_FALSE);
	m_first_click = true;
	if (m_x_offset <= 0.00001 && m_y_offset <= 0.00001) return;
	SDL_WarpMouseInWindow(window, x, y);
}

void Camera::updateCamera()
{
	glm::vec3 forward;
	float yaw_rad = glm::radians(m_yaw);
	float pitch_rad = glm::radians(m_pitch);

	forward.x = cos(yaw_rad) * cos(pitch_rad);
	forward.y = sin(pitch_rad);
	forward.z = sin(yaw_rad) * cos(pitch_rad);

	m_forward = normalize(forward);
}

glm::mat4 Camera::MyLookAt()
{
	glm::vec3 right = normalize(cross(m_forward, m_up));
	glm::vec3 new_up = cross(right, m_forward);

	glm::mat4 rotation = { right.x, new_up.x, -m_forward.x, 0,
						   right.y, new_up.y, -m_forward.y, 0,
						   right.z, new_up.z, -m_forward.z, 0,
						   0,		 0,	       0,	   1 };

	glm::mat4 translate = glm::mat4(1.0f);
	translate[3][0] = -m_pos.x;
	translate[3][1] = -m_pos.y;
	translate[3][2] = -m_pos.z;

	glm::mat4 view = rotation * translate;

	return view;
}
