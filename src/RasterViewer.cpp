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






void clear_bg(FrameBuffer& frameBuffer) {
	for (int i = 0; i <= frameBuffer.rows() - 1; i++) {
		for (int j = 0; j <= frameBuffer.cols() - 1; j++) {
			frameBuffer(i,j).color(0) = 255;
			frameBuffer(i,j).color(1) = 255;
			frameBuffer(i,j).color(2) = 255;
			frameBuffer(i,j).color(3) = 255;
		}
	}
}


void select_triangle() {

}





int main(int argc, char *args[])
{
    int width = 500;
    int height = 500;
    // The Framebuffer storing the image rendered by the rasterizer
	Eigen::Matrix<FrameBufferAttributes,Eigen::Dynamic,Eigen::Dynamic> frameBuffer(width, height);

	// Global Constants
	UniformAttributes uniform;

	// Basic rasterization program
	Program program;

	// The vertex shader is the identity
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform) {
        return va;
	};

	// The fragment shader uses a fixed color
	program.FragmentShader = [](const VertexAttributes& va, const UniformAttributes& uniform) {
		return FragmentAttributes(va.color(0),va.color(1),va.color(2));
	};

	// The blending shader for things really drawn
	function<FrameBufferAttributes(const FragmentAttributes&, const FrameBufferAttributes&)> BS = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous) {
        if (fa.order >= previous.depth) {
            FrameBufferAttributes out(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
            out.depth = fa.order;
            return out;
        }
        else 
            return previous;        
	};
    // The blending shader for preview segments
    function<FrameBufferAttributes(const FragmentAttributes&, const FrameBufferAttributes&)> temp_BS = [](const FragmentAttributes& fa, const FrameBufferAttributes& previous) {
        return FrameBufferAttributes(fa.color[0]*255,fa.color[1]*255,fa.color[2]*255,fa.color[3]*255);
    };



    // The fragment shader for lines
	function<FragmentAttributes(const VertexAttributes&, const UniformAttributes&)> line_FS = [](const VertexAttributes& va, const UniformAttributes& uniform) {
		FragmentAttributes out = FragmentAttributes(0,0,0);
        out.order = va.order;
        return out;
	};
    // The fragment shader for triangles
	function<FragmentAttributes(const VertexAttributes&, const UniformAttributes&)> triangle_FS = [](const VertexAttributes& va, const UniformAttributes& uniform) {
        FragmentAttributes out(va.color(0),va.color(1),va.color(2));
        out.order = va.order;
        return out;
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
                    temp_lines.push_back(line_vertices.at(line_vertices.size() - 1));
                    temp_lines.push_back(new_vertex);
                }
                else if (viewer.num_new_vertices == 2) {
                    temp_lines.clear();
                    temp_lines.push_back(line_vertices.at(line_vertices.size() - 2));
                    temp_lines.push_back(line_vertices.at(line_vertices.size() - 1));
                    temp_lines.push_back(line_vertices.at(line_vertices.size() - 2));
                    temp_lines.push_back(new_vertex);
                    temp_lines.push_back(line_vertices.at(line_vertices.size() - 1));
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
                new_vertex.order = uniform.index;
                viewer.num_new_vertices++;

                if (viewer.num_new_vertices == 1) {
                    line_vertices.push_back(new_vertex);
                } 
                else if (viewer.num_new_vertices == 2) {
                    line_vertices.push_back(new_vertex);
                }
                else if (viewer.num_new_vertices == 3) {
                    triangle_vertices.push_back(temp_lines.at(0));
                    triangle_vertices.push_back(temp_lines.at(1));
                    triangle_vertices.push_back(new_vertex);
                    
                    line_vertices.push_back(temp_lines.at(0));
                    line_vertices.push_back(new_vertex);
                    line_vertices.push_back(temp_lines.at(1));
                    line_vertices.push_back(new_vertex);

                    viewer.num_new_vertices = 0;
                    uniform.index++;
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
        clear_bg(frameBuffer);

        program.FragmentShader = triangle_FS;
        program.BlendingShader = BS;
       	rasterize_triangles(program,uniform,triangle_vertices,frameBuffer);
        program.FragmentShader = line_FS;
        rasterize_lines(program, uniform, line_vertices, 1, frameBuffer);
        
        program.BlendingShader = temp_BS;
        rasterize_lines(program, uniform, temp_lines, 1, frameBuffer);

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
