#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// 定义相机移动的几种可能选项，作为抽象用来避免特定与窗口系统的输入方法
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// 默认相机数值
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// 一个抽象的相机类，用于处理输入并计算相应的欧拉角，向量与矩阵，以供在OpenGL使用
class Camera
{
public:
	// 相机属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// 欧拉角
	float Yaw;
	float Pitch;
	// 相机选项
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// 构造函数(使用向量)
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// 构造函数（使用标量）
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// 返回使用欧拉角和LookAt矩阵计算得到的视图矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// 处理来自任何类似于键盘的输入系统的输入。接受相机定义的枚举形式的输入参数(以将其与窗口系统抽象分离)
	//---------------更改的是lookAt矩阵中的Position即相机位置，以达到视角的平移效果
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;

		// Position.y = 0.0f;
	}

	// 处理来自鼠标的输入系统的输入
	//---------------更改的是lookAt矩阵中的Front即相机方向向量，以达到视角的旋转效果
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// 确保在俯仰角超出范围时，屏幕不会发生翻转
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// 更新 Front, Right and Up Vectors使用更新后的欧拉角
		updateCameraVectors();
	}

	// 处理来自鼠标滚轮事件的输入
	//---------------此处更改的非view矩阵，而是projection矩阵的fov视场角
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// 从更新后的相机欧拉角计算Front Right和Up向量
	void updateCameraVectors()
	{
		// 计算新的 Front 向量
		glm::vec3 front = glm::vec3(1.0f);
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// 同时更新Right与Up向量
		Right = glm::normalize(glm::cross(Front, WorldUp)); // 对向量进行归一化，
	}
};
#endif