#pragma once

#include <Eigen/Core>

using namespace std;
using namespace Eigen;

class VertexAttributes
{
	public:
	VertexAttributes(float x = 0, float y = 0, float z = 0, float w = 1)
	{
		position << x,y,z,w;
		color << 1,1,1,1;
		selected = false;
		transform = Matrix4f::Identity(4,4);
		scale_factor = 1.0;
		rotation_angle = 0.0;
		moved = false;
		T1 = Matrix4f::Identity(4,4);
		S = Matrix4f::Identity(4,4);
		R = Matrix4f::Identity(4,4);
		T2 = Matrix4f::Identity(4,4);
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

	Vector4f position;
	Vector4f color;
	int order;
	bool selected;
	float scale_factor;
	float rotation_angle;
	bool moved;

	Matrix4f transform = Matrix4f::Identity(4,4);
	Vector3f barycenter = Vector3f(0,0,0);
	Vector3f to_position = Vector3f(0,0,0);
	Matrix4f T1 = Matrix4f::Identity(4,4);
	Matrix4f S = Matrix4f::Identity(4,4);
	Matrix4f R = Matrix4f::Identity(4,4);
	Matrix4f T2 = Matrix4f::Identity(4,4);

};

class FragmentAttributes
{
	public:
	FragmentAttributes(float r = 0, float g = 0, float b = 0, float a = 1)
	{
		color << r,g,b,a;
	}

	Vector4f color;
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

	Matrix<uint8_t,4,1> color;
	int depth;
};

class UniformAttributes
{
	public:
		int index = 0;
		int selected_triangle = -1;
		float rotation_angle = 0.0;
		float scale_factor = 1.0;
		int selected_vertex = -1;

		Matrix4f T1 = Matrix4f::Identity(4,4);
		Matrix4f T2 = Matrix4f::Identity(4,4);
		Matrix4f T = Matrix4f::Identity(4,4);
		Matrix4f R = Matrix4f::Identity(4,4);
		Matrix4f S = Matrix4f::Identity(4,4);
		Vector3f barycenter = Vector3f(0,0,0);
		Vector3f to_position = Vector3f(0,0,0);
		Vector3f offset = Vector3f(0,0,0);

		MatrixXf preset_colors = MatrixXf::Zero(9,4);

		Matrix4f view = Matrix4f::Identity(4,4);

		vector<vector<Vector3f> >keyframe_to_positions;
		vector<VertexAttributes> start_frame_triangles;
		vector<VertexAttributes> start_frame_lines;

	UniformAttributes() {
		index = 0;
		selected_triangle = -1;
		rotation_angle = 0.0;
		scale_factor = 1.0;
		selected_vertex = -1;
		T1 = Matrix4f::Identity(4,4);
		T2 = Matrix4f::Identity(4,4);
		T = Matrix4f::Identity(4,4);
		R = Matrix4f::Identity(4,4);
		S = Matrix4f::Identity(4,4);

		preset_colors << 0.698, 0.133, 0.133, 1,
						 1, 0.647, 0, 1,
						 0.98, 0.98, 0.823, 1,
						 0.42, 0.557, 0.137, 1,
						 0.686, 0.933, 0.933, 1,
						 0.275, 0.51, 0.706, 1,
						 0.576, 0.478, 0.859, 1,
						 1,1,1,1,
						 0,0,0,1;

		view = Matrix4f::Identity(4,4);
	}
};