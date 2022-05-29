#pragma once
#include "igl/opengl/glfw/Display.h"
#include "igl/opengl/glfw/Renderer.h"
#include "sandBox.h"

static void glfw_mouse_press(GLFWwindow* window, int button, int action, int modifier)
{

	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	igl::opengl::glfw::Viewer* scn = rndr->GetScene();

	if (action == GLFW_PRESS)
	{
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);


		double depth, closestZ = 1;
		int i = 0, savedIndx = scn->selected_data_index, lastIndx = scn->selected_data_index;

		for (; i < scn->data_list.size(); i++)
		{
			scn->selected_data_index = i;
			depth = rndr->Picking(x2, y2);
			if (depth < 0 && (closestZ > 0 || closestZ < depth))
			{
				savedIndx = i;
				closestZ = depth;
				std::cout << "found " << depth << std::endl;
			}
		}
		scn->selected_data_index = savedIndx;
		scn->data().set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
		if (lastIndx != savedIndx)
			scn->data_list[lastIndx].set_colors(Eigen::RowVector3d(255.0 / 255.0, 228.0 / 255.0, 58.0 / 255.0));

		rndr->UpdatePosition(x2, y2);

	}
	else
	{
		rndr->GetScene()->isPicked = false;

	}
}


//static void glfw_char_mods_callback(GLFWwindow* window, unsigned int codepoint, int modifier)
//{
//  __viewer->key_pressed(codepoint, modifier);
//}

void glfw_mouse_move(GLFWwindow* window, double x, double y)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	rndr->UpdatePosition(x, y);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{

		rndr->MouseProcessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

		rndr->MouseProcessing(GLFW_MOUSE_BUTTON_LEFT);
	}
}

static void glfw_mouse_scroll(GLFWwindow* window, double x, double y)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	if (rndr->IsPicked())
		rndr->GetScene()->data().MyScale(Eigen::Vector3d(1 + y * 0.01, 1 + y * 0.01, 1 + y * 0.01));
	else
		rndr->GetScene()->MyTranslate(Eigen::Vector3d(0, 0, -y * 0.03), true);
}

void glfw_window_size(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	//igl::opengl::glfw::Viewer* scn = rndr->GetScene();

	rndr->post_resize(window, width, height);

}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	SandBox* scn = (SandBox*)rndr->GetScene();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	else if (action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key)
		{
		case 'A':
		case 'a':
		{
			rndr->core().is_animating = !rndr->core().is_animating;
			break;
		}
		case 'F':
		case 'f':
		{
			scn->data().set_face_based(!scn->data().face_based);
			break;
		}
		case 'I':
		case 'i':
		{
			scn->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
			scn->data().invert_normals = !scn->data().invert_normals;
			break;
		}
		case 'L':
		case 'l':
		{
			rndr->core().toggle(scn->data().show_lines);
			break;
		}
		case 'O':
		case 'o':
		{
			rndr->core().orthographic = !rndr->core().orthographic;
			break;
		}
		case 'T':
		case 't':
		{

			break;

		}
		case '[':
		case ']':
		{
			scn->isChangeCamera2 = true;
			rndr->ChangeCamera(key);
			break;
		}
		case ';':
			scn->data().show_vertid = !scn->data().show_vertid;
			break;
		case ':':
			scn->data().show_faceid = !scn->data().show_faceid;
			break;
		case 'w':
		case 'W':
			if (scn->isGameStarted) {
				scn->up = false;
				scn->down = false;
				scn->left = false;
				scn->right = false;
				scn->in = true;
				scn->out = false;
			}
			// end Project comment
			break;
		case 's':
		case 'S':
			if (scn->isGameStarted) {
					scn->up = false;
					scn->down = false;
					scn->left = false;
					scn->right = false;
					scn->in = false;
					scn->out = true;
			}
			//end comment Project
			break;
		case GLFW_KEY_UP:
			if (scn->isGameStarted) {
				scn->up = true;
				scn->down = false;
				scn->left = false;
				scn->right = false;
				scn->in = false;
				scn->out = false;
			}

			break;
		case GLFW_KEY_DOWN:
			if (scn->isGameStarted) {
				scn->up = false;
				scn->down = true;
				scn->left = false;
				scn->right = false;
				scn->in = false;
				scn->out = false;
			}
			break;
		case GLFW_KEY_LEFT:
			if (scn->isGameStarted) {
				scn->up = false;
				scn->down = false;
				scn->left = true;
				scn->right = false;
				scn->in = false;
				scn->out = false;
			}
			break;
		case GLFW_KEY_RIGHT:
			if (scn->isGameStarted) {
				scn->up = false;
				scn->down = false;
				scn->left = false;
				scn->right = true;
				scn->in = false;
				scn->out = false;
			}
			break;
			//end cpmment Project

		case ' ':
	
			if (scn->isGameStarted) {
				scn->isActive = false;//it ruined the movment
				scn->isResume = true;
			}
			break;
		case 'k':
		case 'K':
		
			break;
		case 'j':
		case 'J':
			scn->isTargetMoveing = !scn->isTargetMoveing;
			break;

		//Ass3
		case'P':
		case 'p':
		
			break;
		case 'D':
		case 'd':
			/*scn->destination_position = Eigen::Vector3d(scn->data_list[0].MakeTransd().col(3)[0], scn->data_list[0].MakeTransd().col(3)[1], scn->data_list[0].MakeTransd().col(3)[2]);
			std::cout << "destination: (" << scn->destination_position << ")" << std::endl;*/
			break;

		case 'H':
		case 'h':
		{
			rndr->core().toggle(scn->data().show_overlay);
			break;
		}
		//end Ass3
		default:
			Eigen::Vector3f shift;
			float scale;
			rndr->core().get_scale_and_shift_to_fit_mesh(scn->data().V, scn->data().F, scale, shift);

			std::cout << "near " << rndr->core().camera_dnear << std::endl;
			std::cout << "far " << rndr->core().camera_dfar << std::endl;
			std::cout << "angle " << rndr->core().camera_view_angle << std::endl;
			std::cout << "base_zoom " << rndr->core().camera_base_zoom << std::endl;
			std::cout << "zoom " << rndr->core().camera_zoom << std::endl;
			std::cout << "shift " << shift << std::endl;
			std::cout << "translate " << rndr->core().camera_translation << std::endl;

			break;//do nothing
		}
}


void Init(Display& display, igl::opengl::glfw::imgui::ImGuiMenu* menu)
{
	display.AddKeyCallBack(glfw_key_callback);
	//display.AddMouseCallBacks(glfw_mouse_press, glfw_mouse_scroll, glfw_mouse_move);
	display.AddResizeCallBack(glfw_window_size);
	menu->init(&display);
}

