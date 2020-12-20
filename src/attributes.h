#pragma once

#include <Eigen/Core>

class VertexAttributes
{
	public:
	VertexAttributes(float x = 0, float y = 0, float z = 0, float w = 1)
	{
		position << x,y,z,w;
		color << 1,1,1,1;
		selected = false;
		transform = Eigen::Matrix4f::Identity(4,4);
		scale_factor = 1.0;
		rotation_angle = 0.0;
	}

    // Interpolates the vertex attributes
    static VertexAttributes interpolate(
        const VertexAttributes& a,
        const VertexAttributes& b,
        const VertexAttributes& c,
        const float alpha, 
        const float beta, 
        const float gamma
    ) 
    {
        VertexAttributes r;
        r.position = alpha*a.position + beta*b.position + gamma*c.position;
		r.color = alpha*a.color + beta*b.color + gamma*c.color;
		r.order = a.order;
		r.selected = a.selected;
        return r;
    }

	Eigen::Vector4f position;
	Eigen::Vector4f color;
	int order;
	bool selected;
	float scale_factor;
	float rotation_angle;

	Eigen::Matrix4f transform = Eigen::Matrix4f::Identity(4,4);
	Eigen::Vector3f barycenter = Eigen::Vector3f(0,0,0);
	Eigen::Vector3f to_position = Eigen::Vector3f(0,0,0);

};

class FragmentAttributes
{
	public:
	FragmentAttributes(float r = 0, float g = 0, float b = 0, float a = 1)
	{
		color << r,g,b,a;
	}

	Eigen::Vector4f color;
	int order;
};

class FrameBufferAttributes
{
	public:
	FrameBufferAttributes(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
	{
		color << r,g,b,a;
		depth = -1;
	}

	Eigen::Matrix<uint8_t,4,1> color;
	int depth;
};

class UniformAttributes
{
	public:
		int index = 0;
		int selected_triangle = -1;
		float rotation_angle = 0.0;
		float scale_factor = 1.0;

		Eigen::Matrix4f T1 = Eigen::Matrix4f::Identity(4,4);
		Eigen::Matrix4f T2 = Eigen::Matrix4f::Identity(4,4);
		Eigen::Matrix4f T = Eigen::Matrix4f::Identity(4,4);
		Eigen::Matrix4f R = Eigen::Matrix4f::Identity(4,4);
		Eigen::Matrix4f S = Eigen::Matrix4f::Identity(4,4);
		Eigen::Vector3f barycenter = Eigen::Vector3f(0,0,0);
		Eigen::Vector3f to_position = Eigen::Vector3f(0,0,0);

	UniformAttributes() {
		index = 0;
		selected_triangle = -1;
		rotation_angle = 0.0;
		scale_factor = 1.0;
		T1 = Eigen::Matrix4f::Identity(4,4);
		T2 = Eigen::Matrix4f::Identity(4,4);
		T = Eigen::Matrix4f::Identity(4,4);
		R = Eigen::Matrix4f::Identity(4,4);
		S = Eigen::Matrix4f::Identity(4,4);
	}
};