#include "SDLViewer.h"

#include <Eigen/Core>

#include <functional>
#include <iostream>

#include "raster.h"

// Image writing library
#define STB_IMAGE_WRITE_IMPLEMENTATION // Do not include this line twice in your project!
#include "stb_image_write.h"

using namespace std;
using namespace Eigen;







void select_triangle() {

}





int main(int argc, char *args[])
{
    int width = 500;
    int height = 500;
    // The Framebuffer storing the image rendered by the rasterizer
	Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(width, height);

	// Global Constants (empty in this example)
	UniformAttributes uniform;

	// Basic rasterization program
	Program program;

	// The vertex shader is the identity
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		return va;
	};

	// The fragment shader uses a fixed color
	program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform)
	{
		return FragmentAttributes(va.color(0),va.color(1),va.color(2));
	};

	// The blending shader converts colors between 0 and 1 to uint8
	program.BlendingShader = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous)
	{
		return FrameBufferAttributes(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
	};

    // vertices
	vector<VertexAttributes> triangle_vertices;
	vector<VertexAttributes> line_vertices;
    vector<VertexAttributes> temp_lines;

    // Initialize the viewer and the corresponding callbacks
    SDLViewer viewer;
    viewer.init("2D Shape Editor", width, height);

    viewer.mouse_move = [&](int x, int y, int xrel, int yrel){
        switch (viewer.current_mode) {
            case insertion: {
                VertexAttributes new_vertex = VertexAttributes((float(x)/float(width) * 2) - 1, (float(height-1-y)/float(height) * 2) - 1, 0);
                new_vertex.color << 1,0,0,1;
                if (viewer.num_new_vertices == 1) {
                    temp_lines.clear();
                    temp_lines.push_back(line_vertices.at(0));
                    temp_lines.push_back(new_vertex);
                }
                else if (viewer.num_new_vertices == 2) {
                    temp_lines.clear();
                    temp_lines.push_back(line_vertices.at(0));
                    temp_lines.push_back(line_vertices.at(1));
                    temp_lines.push_back(line_vertices.at(0));
                    temp_lines.push_back(new_vertex);
                    temp_lines.push_back(line_vertices.at(1));
                    temp_lines.push_back(new_vertex);
                }
                break;
            }
            
            default:
                break;
        }

        viewer.redraw_next = true;
    };

    viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
        switch (viewer.current_mode) {
            case insertion:{
                VertexAttributes new_vertex = VertexAttributes((float(x)/float(width) * 2) - 1, (float(height-1-y)/float(height) * 2) - 1, 0);
                new_vertex.color << 1,0,0,1;
                viewer.num_new_vertices++;

                if (viewer.num_new_vertices == 1) {
                    line_vertices.push_back(new_vertex);
                } 
                else if (viewer.num_new_vertices == 2) {
                    temp_lines.clear();
                    line_vertices.push_back(new_vertex);
                }
                else if (viewer.num_new_vertices == 3) {
                    temp_lines.clear();
                    triangle_vertices.push_back(line_vertices[0]);
                    triangle_vertices.push_back(line_vertices[1]);
                    triangle_vertices.push_back(new_vertex);
                    line_vertices.clear();
                    viewer.num_new_vertices = 0;
                }
            
                break;
            }
            
            default:
                break;
        }

        viewer.redraw_next = true;
    };

    viewer.mouse_wheel = [&](int dx, int dy, bool is_direction_normal) {
    };

    viewer.key_pressed = [&](char key, bool is_pressed, int modifier, int repeat) {
        switch (key) {
            case 'i':
                viewer.current_mode = insertion;
                break;
            case 'o':
                viewer.current_mode = translation;
                break;
            case 'p':
                viewer.current_mode = deletion;
                break;
            case 'c':
                viewer.current_mode = color;
                break;
            
            default:
                break;
        }
    };

    viewer.redraw = [&](SDLViewer &viewer) {
        // Clear the framebuffer
        for (unsigned i=0;i<frameBuffer.rows();i++)
            for (unsigned j=0;j<frameBuffer.cols();j++)
                frameBuffer(i,j).color << 0,0,0,1;

       	rasterize_triangles(program,uniform,triangle_vertices,frameBuffer);
        rasterize_lines(program, uniform, temp_lines, 0.5, frameBuffer);
        rasterize_lines(program, uniform, line_vertices, 0.5, frameBuffer);

        // Buffer for exchanging data between rasterizer and sdl viewer
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> R(width, height);
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> G(width, height);
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> B(width, height);
        Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> A(width, height);

        for (unsigned i=0; i<frameBuffer.rows();i++)
        {
            for (unsigned j=0; j<frameBuffer.cols();j++)
            {
                R(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(0);
                G(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(1);
                B(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(2);
                A(i,frameBuffer.cols()-1-j) = frameBuffer(i,j).color(3);
            }
        }
        viewer.draw_image(R, G, B, A);
    };

    viewer.launch();

    return 0;
}
