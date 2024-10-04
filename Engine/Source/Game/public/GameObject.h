#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>
#include <GameObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	class GameObject
	{
	public:
		GameObject() = default;
        virtual ~GameObject() = default;

	public:
		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

        virtual void Update(size_t frame, float dt) = 0;

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
	};

    class GravityObject : public GameObject
    {
    public:
        GravityObject(float gravity = -9.8f, float jumpSpeed = 15.0f, float bounceFactor = 0.7f)
            : m_Gravity(gravity), m_JumpSpeed(jumpSpeed), m_BounceFactor(bounceFactor)
        {
            m_Velocity = Math::Vector3f(0.0f, m_JumpSpeed, 0.0f);
        }

        void Update(size_t frame, float dt) override
        {
            Math::Vector3f curr_pos = GetPosition();
            m_Velocity.y += m_Gravity * dt;
            curr_pos.y += m_Velocity.y * dt;

            if (curr_pos.y <= 0.0f)
            {
                curr_pos.y = 0.0f;
                if (std::abs(m_Velocity.y) > 1.0f)
                {
                    m_Velocity.y = -m_Velocity.y * m_BounceFactor;
                }
                else
                {
                    m_Velocity.y = 0.0f;
                }
            }

            SetPosition(curr_pos, frame);
        }

        void Jump()
        {
            if (GetPosition().y <= 0.0f)
            {
                m_Velocity.y = m_JumpSpeed;
            }
        }

    private:
        Math::Vector3f m_Velocity = Math::Vector3f::Zero();
        float m_Gravity;
        float m_JumpSpeed;
        float m_BounceFactor;
    };



    class ControllableObject : public GameObject
    {
    public:
        ControllableObject(float speed = 5.0f) : m_Speed(speed), m_x(0)
        {
            Core::g_InputHandler->RegisterCallback("GoRight", [&]() { this->m_x = 1; });
            Core::g_InputHandler->RegisterCallback("GoLeft", [&]() { this->m_x = -1; });
        }

        void Update(size_t frame, float dt) override
        {
            Math::Vector3f curr_pos = GetPosition();

            curr_pos.x += m_Speed * dt * m_x;

            SetPosition(curr_pos, frame);
        }

    private:
        float m_Speed;
        int m_x;
    };




    class MovingObject : public GameObject
    {
    public:
        MovingObject(float speed = 5.0f, float timeToChangeDir = 3.0f)
            : m_Speed(speed), m_TimeToChangeDir(timeToChangeDir) {}

        void Update(size_t frame, float dt) override
        {
            m_Timer += dt;

            if (m_Timer >= m_TimeToChangeDir)
            {
                m_Timer = 0;
                m_Direction *= -1;
            }

            Math::Vector3f curr_pos = GetPosition();
            curr_pos.z += m_Speed * dt * m_Direction;

            SetPosition(curr_pos, frame);
        }

    private:
        float m_Speed;
        float m_TimeToChangeDir;
        float m_Timer = 0;
        int m_Direction = 1;
    };
}