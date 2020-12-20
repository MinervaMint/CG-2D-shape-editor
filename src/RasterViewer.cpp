#include "SDLViewer.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <functional>
#include <iostream>
#include <cmath>

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

void reset_framebuffer(FrameBuffer& frameBuffer) {
	frameBuffer.setConstant(FrameBufferAttributes());
}

bool orientation_test(Vector3f a, Vector3f b) {
    return ((a.cross(b))(2) > 0);
}


bool in_triangle(float x, float y, VertexAttributes va, VertexAttributes vb, VertexAttributes vc) {
    Vector3f a, b, c, p;
    a << va.position(0), va.position(1), 0;
    b << vb.position(0), vb.position(1), 0;
    c << vc.position(0), vc.position(1), 0;
    p << x, y, 0;

    if ((orientation_test(b-a, p-a) && orientation_test(c-b, p-b) && orientation_test(a-c, p-c)) ||
        (!orientation_test(b-a, p-a) && !orientation_test(c-b, p-b) && !orientation_test(a-c, p-c)))
        return true;
    return false;
}


int select_triangle(vector<VertexAttributes> &triangle_vertices, float x, float y, UniformAttributes& uniform) {
    for (int i = triangle_vertices.size() / 3 - 1; i >=0; i--) {
        // transform x, y using inverse transform
        Vector4f mouse_position = Vector4f(x, y, 0, 1);
        Matrix4f transform = triangle_vertices.at(3*i).T2 * triangle_vertices.at(3*i).R * triangle_vertices.at(3*i).S * triangle_vertices.at(3*i).T1;
        mouse_position = transform.inverse() * mouse_position;
        if (in_triangle(mouse_position(0), mouse_position(1), triangle_vertices.at(3*i), triangle_vertices.at(3*i+1), triangle_vertices.at(3*i+2))) {
            return i;
        }
    }
    return -1;
}


void compute_barycenter(vector<VertexAttributes> &triangle_vertices, int index, UniformAttributes &uniform) {
    Vector4f barycenter = Vector4f(0,0,0,0);
    barycenter += triangle_vertices.at(3*index).position;
    barycenter += triangle_vertices.at(3*index+1).position;
    barycenter += triangle_vertices.at(3*index+2).position;
    barycenter /= 3;
    uniform.barycenter << barycenter(0), barycenter(1), barycenter(2);
}


void construct_T(UniformAttributes &uniform) {
    uniform.T1 << 1,0,0,-uniform.barycenter(0),
          0,1,0,-uniform.barycenter(1),
          0,0,1,-uniform.barycenter(2),
          0,0,0,1;
    uniform.T2 << 1,0,0,uniform.to_position(0),
          0,1,0,uniform.to_position(1),
          0,0,1,uniform.to_position(2),
          0,0,0,1;
    uniform.T = uniform.T2 * uniform.T1;
}

void reset_T(UniformAttributes &uniform) {
    uniform.T1 = Matrix4f::Identity(4,4);
    uniform.T2 = Matrix4f::Identity(4,4);
    uniform.T = Matrix4f::Identity(4,4);
}

void construct_R(UniformAttributes &uniform) {
    float cosine = cos(uniform.rotation_angle);
	float sine = sin(uniform.rotation_angle);
	uniform.R << cosine, -sine, 0, 0,
		 sine, cosine, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1;
}

void construct_S(UniformAttributes &uniform) {
    uniform.S << uniform.scale_factor, 0, 0, 0,
		 0, uniform.scale_factor, 0, 0,
		 0, 0, uniform.scale_factor, 0,
		 0, 0, 0, 1;
}


float dist_sq(float x1, float y1, float x2, float y2) {
    return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


bool is_vertex_overlaid(const VertexAttributes& vertex, const FrameBuffer& frameBuffer) {
    Vector4f position = vertex.T2 * vertex.R * vertex.S * vertex.T1 * vertex.position;
    int x, y;
    x = int((position(0)+1.0)/2.0 * frameBuffer.rows()); y = int((position(1)+1.0)/2.0 * frameBuffer.cols());
    return (frameBuffer(x,y).depth > vertex.order);
}

int select_nearest_vertex(vector<VertexAttributes> triangle_vertices, float x, float y, const FrameBuffer& frameBuffer) {
    if (triangle_vertices.size() == 0) return -1;
    float min_dist = 10000000.0;
    int selected_index = -1;
    for (int i = triangle_vertices.size() - 1; i >= 0; i--) {
        if (is_vertex_overlaid(triangle_vertices[i], frameBuffer)) continue;
        // transform x, y using inverse transform
        Vector4f mouse_position = Vector4f(x, y, 0, 1);
        Matrix4f transform = triangle_vertices.at(i).T2 * triangle_vertices.at(i).R * triangle_vertices.at(i).S * triangle_vertices.at(i).T1;
        mouse_position = transform.inverse() * mouse_position;

        float dist = dist_sq(mouse_position(0), mouse_position(1), triangle_vertices[i].position(0), triangle_vertices[i].position(1));
        if (min_dist > dist) {
            min_dist = dist;
            selected_index = i;
        }
    }
    return selected_index;
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

	// The vertex shader
	program.VertexShader = [](const VertexAttributes& va, const UniformAttributes& uniform) {
        Vector4f position;
        position = va.T2 * va.R * va.S * va.T1 * va.position;
        position = uniform.view * position;
        
        VertexAttributes out(position(0), position(1), position(2));
        out.color = va.color;
        out.order = va.order;
        out.selected = va.selected;
        if (out.selected)
            out.color << 0,0,1,1;
        return out;
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



    // The fragment shader for triangles
	function<FragmentAttributes(const VertexAttributes&, const UniformAttributes&)> triangle_FS = [](const VertexAttributes& va, const UniformAttributes& uniform) {
        FragmentAttributes out(va.color(0),va.color(1),va.color(2));
        out.order = va.order;
        return out;
    };
    // The fragment shader for lines
	function<FragmentAttributes(const VertexAttributes&, const UniformAttributes&)> line_FS = [](const VertexAttributes& va, const UniformAttributes& uniform) {
		FragmentAttributes out = FragmentAttributes(0,0,0);
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
        Vector4f mouse_position = Vector4f((float(x)/float(width) * 2) - 1, (float(height-1-y)/float(height) * 2) - 1, 0, 1);
        mouse_position = uniform.view.inverse() * mouse_position;
        switch (viewer.current_mode) {
            case insertion: {
                VertexAttributes new_vertex = VertexAttributes(mouse_position(0), mouse_position(1), 0);
                new_vertex.color << 1,0,0,1;
                new_vertex.order = uniform.index;
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
            case translation: {
                uniform.to_position << mouse_position(0)+uniform.offset(0), mouse_position(1)+uniform.offset(1), 0;
                construct_T(uniform);
                if (uniform.selected_triangle != -1 && triangle_vertices[3*uniform.selected_triangle].selected) {
                    for (int i = 0; i <= 2; i++) {
                        triangle_vertices[3*uniform.selected_triangle+i].T1 = uniform.T1;
                        triangle_vertices[3*uniform.selected_triangle+i].T2 = uniform.T2;
                    }
                    for (int i = 0; i <=5; i++) {
                        line_vertices[6*uniform.selected_triangle+i].T1 = uniform.T1;
                        line_vertices[6*uniform.selected_triangle+i].T2 = uniform.T2;
                    }
                }
                break;
            }
            
            default:
                break;
        }

        viewer.redraw_next = true;
    };

    viewer.mouse_pressed = [&](int x, int y, bool is_pressed, int button, int clicks) {
        Vector4f mouse_position = Vector4f((float(x)/float(width) * 2) - 1, (float(height-1-y)/float(height) * 2) - 1, 0, 1);
        mouse_position = uniform.view.inverse() * mouse_position;
        switch (viewer.current_mode) {
            case insertion: {
                if (!is_pressed) {
                    VertexAttributes new_vertex = VertexAttributes(mouse_position(0), mouse_position(1), 0);
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
                        temp_lines.clear();
                    }
                }
                
                break;
            }
            case translation: {
                if (is_pressed) {
                    uniform.selected_triangle = select_triangle(triangle_vertices, mouse_position(0), mouse_position(1), uniform);
                    if (uniform.selected_triangle != -1) {
                        compute_barycenter(triangle_vertices, uniform.selected_triangle, uniform);
                        Vector4f new_barycenter;
                        new_barycenter << uniform.barycenter(0), uniform.barycenter(1), uniform.barycenter(2), 1;
                        new_barycenter = triangle_vertices[3*uniform.selected_triangle].T2 * triangle_vertices[3*uniform.selected_triangle].R 
                                        * triangle_vertices[3*uniform.selected_triangle].S * triangle_vertices[3*uniform.selected_triangle].T1 * new_barycenter;
                        uniform.offset << new_barycenter(0)-mouse_position(0), new_barycenter(1)-mouse_position(1), new_barycenter(2)-mouse_position(2);
                        reset_T(uniform);
                        for (int i = 0; i <= 2; i++) {
                            triangle_vertices[3*uniform.selected_triangle+i].selected = true;
                        }
                        for (int i = 0; i <=5; i++) {
                            line_vertices[6*uniform.selected_triangle+i].selected = true;
                        }
                    }
                }
                else {
                    if (uniform.selected_triangle != -1) {
                        uniform.to_position << mouse_position(0)+uniform.offset(0), mouse_position(1)+uniform.offset(1), 0;
                        construct_T(uniform);
                        for (int i = 0; i <= 2; i++) {
                            triangle_vertices[3*uniform.selected_triangle+i].selected = false;
                            triangle_vertices[3*uniform.selected_triangle+i].T1 = uniform.T1;
                            triangle_vertices[3*uniform.selected_triangle+i].T2 = uniform.T2;
                        }
                        for (int i = 0; i <=5; i++) {
                            line_vertices[6*uniform.selected_triangle+i].selected = false;
                            line_vertices[6*uniform.selected_triangle+i].T1 = uniform.T1;
                            line_vertices[6*uniform.selected_triangle+i].T2 = uniform.T2;
                        }

                        uniform.offset << 0,0,0;
                        reset_T(uniform);
                    }
                }

                break;
            }
            case deletion: {
                if (!is_pressed) {
                    int selected = select_triangle(triangle_vertices, mouse_position(0), mouse_position(1), uniform);
                    if (selected != -1) {
                        triangle_vertices.erase(triangle_vertices.begin() + 3*selected, triangle_vertices.begin() + 3*selected+3);
                        line_vertices.erase(line_vertices.begin() + 6*selected, line_vertices.begin() + 6*selected+6);
                        uniform.selected_triangle = -1;
                    }
                }
                break;
            }
            case color: {
                if (!is_pressed)
                    uniform.selected_vertex = select_nearest_vertex(triangle_vertices, mouse_position(0), mouse_position(1), frameBuffer);
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
        if (!is_pressed) {
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
                case 'h':
                    if (viewer.current_mode == translation) {
                        int selected = uniform.selected_triangle;
                        if (selected != -1) {
                            for (int i = 0; i <= 2; i++)
                                triangle_vertices[3*selected+i].rotation_angle -= (10.0 / 180.0) * M_PI;
                            for (int i = 0; i <= 5; i++)
                                line_vertices[6*selected+i].rotation_angle -= (10.0 / 180.0) * M_PI;
                            uniform.rotation_angle = triangle_vertices[3*selected].rotation_angle;
                            construct_R(uniform);

                            for (int i = 0; i <= 2; i++) 
                                triangle_vertices[3*selected+i].R = uniform.R;
                            
                            for (int i = 0; i <= 5; i++) 
                                line_vertices[6*selected+i].R = uniform.R;

                            viewer.redraw_next = true;
                        }
                    }
                    break;
                case 'j':
                    if (viewer.current_mode == translation) {
                        int selected = uniform.selected_triangle;
                        if (selected != -1) {
                            for (int i = 0; i <= 2; i++)
                                triangle_vertices[3*selected+i].rotation_angle += (10.0 / 180.0) * M_PI;
                            for (int i = 0; i <= 5; i++)
                                line_vertices[6*selected+i].rotation_angle += (10.0 / 180.0) * M_PI;
                            uniform.rotation_angle = triangle_vertices[3*selected].rotation_angle;
                            construct_R(uniform);

                            for (int i = 0; i <= 2; i++) 
                                triangle_vertices[3*selected+i].R = uniform.R;
                            
                            for (int i = 0; i <= 5; i++) 
                                line_vertices[6*selected+i].R = uniform.R;

                            viewer.redraw_next = true;
                        }
                    }
                    break;
                case 'k':
                    if (viewer.current_mode == translation) {
                        int selected = uniform.selected_triangle;
                        if (selected != -1) {
                            for (int i = 0; i <= 2; i++)
                                triangle_vertices[3*selected+i].scale_factor += 0.25;
                            for (int i = 0; i <= 5; i++)
                                line_vertices[6*selected+i].scale_factor += 0.25;
                            uniform.scale_factor = triangle_vertices[3*selected].scale_factor;
                            construct_S(uniform);

                            for (int i = 0; i <= 2; i++) 
                                triangle_vertices[3*selected+i].S = uniform.S;
                            
                            for (int i = 0; i <= 5; i++) 
                                line_vertices[6*selected+i].S = uniform.S;

                            viewer.redraw_next = true;
                        }
                    }
                    break;
                case 'l':
                    if (viewer.current_mode == translation) {
                        int selected = uniform.selected_triangle;
                        if (selected != -1) {
                            for (int i = 0; i <= 2; i++) {
                                triangle_vertices[3*selected+i].scale_factor -= 0.25;
                                triangle_vertices[3*selected+i].scale_factor = max(float(0.0), triangle_vertices[3*selected+i].scale_factor);
                            }
                            for (int i = 0; i <= 5; i++) {
                                line_vertices[6*selected+i].scale_factor -= 0.25;
                                line_vertices[6*selected+i].scale_factor = max(float(0.0), line_vertices[6*selected+i].scale_factor);
                            }
                            uniform.scale_factor = triangle_vertices[3*selected].scale_factor;
                            construct_S(uniform);

                            for (int i = 0; i <= 2; i++) 
                                triangle_vertices[3*selected+i].S = uniform.S;
                            
                            for (int i = 0; i <= 5; i++) 
                                line_vertices[6*selected+i].S = uniform.S;

                            viewer.redraw_next = true;
                        }
                    }
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    int color_index = key - '0';
                    if (uniform.selected_vertex != -1) {
                        triangle_vertices[uniform.selected_vertex].color = uniform.preset_colors.row(color_index-1);
                    }
                    viewer.redraw_next = true;
                    break;
                }
                
                case '=':
                case '+': {
                    Matrix4f zoom;
                    zoom << 1.2,0,0,0,
                            0,1.2,0,0,
                            0,0,1.2,0,
                            0,0,0,1;
                    uniform.view = zoom * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }
                case '-': {
                    Matrix4f zoom;
                    zoom << 0.8,0,0,0,
                            0,0.8,0,0,
                            0,0,0.8,0,
                            0,0,0,1;
                    uniform.view = zoom * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }
                case 'w': {
                    Matrix4f pan;
                    pan << 1,0,0,0,
                           0,1,0,-0.2*2/uniform.view(0,0),
                           0,0,1,0,
                           0,0,0,1;
                    uniform.view = pan * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }
                case 'a': {
                    Matrix4f pan;
                    pan << 1,0,0,0.2*2/uniform.view(0,0),
                           0,1,0,0,
                           0,0,1,0,
                           0,0,0,1;
                    uniform.view = pan * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }
                case 's': {
                    Matrix4f pan;
                    pan << 1,0,0,0,
                           0,1,0,0.2*2/uniform.view(0,0),
                           0,0,1,0,
                           0,0,0,1;
                    uniform.view = pan * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }
                case 'd': {
                    Matrix4f pan;
                    pan << 1,0,0,-0.2*2/uniform.view(0,0),
                           0,1,0,0,
                           0,0,1,0,
                           0,0,0,1;
                    uniform.view = pan * uniform.view;

                    viewer.redraw_next = true;
                    break;
                }

                default:
                    break;
            }
        }
    };

    viewer.redraw = [&](SDLViewer &viewer) {
        // Clear the framebuffer
        reset_framebuffer(frameBuffer);
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
