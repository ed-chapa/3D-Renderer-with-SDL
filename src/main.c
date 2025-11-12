#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "array.h"
#include "vector.h"
#include "triangle.h"
#include "mesh.h"
#include "matrix.h"
#include "light.h"
#include "triangle.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"

// Array of triangles to render
#define MAX_TRIANGLES_PER_MESH 1000000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running;
int previous_frame_time = 0;
float delta_time = 0;

mat4_t proj_matrix;
mat4_t world_matrix;
mat4_t view_matrix;

// Display options for the polygons
bool show_wireframe = true;
bool show_vertices = false;
bool show_filled = false;
bool show_textured = false;
bool enable_culling = true;

void setup(void) {
	// Allocate memory for the color and depth buffers
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

	// Create an SDL texture to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	// Initialize the perspective matrix
	float aspect_x = (float)window_width / (float)window_height;
	float aspect_y = (float)window_height / (float)window_width;
	float fov_y = M_PI / 3.0;
	float fov_x = 2 * atan(tan(fov_y / 2) * aspect_x);
	float z_near = 0.1;
	float z_far = 20.0;
	proj_matrix = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);

	// Initialize the frustum planes
	init_frustum_planes(fov_x, fov_y, z_near, z_far);
	
	// Load a model from an OBJ file
	load_obj_file_data("./assets/f117.obj");

	// Load texture information from a PNG file
	load_png_texture_data("./assets/f117.png");
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			is_running = false;
			break;
		case SDLK_1:
			show_vertices = true;
			show_wireframe = true;
			show_filled = false;
			show_textured = false;
			break;
		case SDLK_2:
			show_vertices = false;
			show_wireframe = true;
			show_filled = false;
			show_textured = false;
			break;
		case SDLK_3:
			show_vertices = false;
			show_wireframe = false;
			show_filled = true;
			show_textured = false;
			break;
		case SDLK_4:
			show_vertices = false;
			show_wireframe = true;
			show_filled = true;
			show_textured = false;
			break;
		case SDLK_5:
			show_vertices = false;
			show_wireframe = false;
			show_filled = false;
			show_textured = true;
			break;
		case SDLK_6:
			show_vertices = false;
			show_wireframe = true;
			show_filled = false;
			show_textured = true;
			break;
		case SDLK_c:
			enable_culling = true;
			break;
		case SDLK_x:
			enable_culling = false;
			break;
		case SDLK_w:
			camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
			camera.position = vec3_add(camera.position, camera.forward_velocity);
			break;
		case SDLK_s:
			camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
			camera.position = vec3_sub(camera.position, camera.forward_velocity);
			break;
		case SDLK_a:
			camera.yaw_angle += 3.0 * delta_time;
			break;
		case SDLK_d:
			camera.yaw_angle -= 3.0 * delta_time;
			break;
		case SDLK_q:
			camera.position.y -= 3.0 * delta_time;
			break;
		case SDLK_e:
			camera.position.y += 3.0 * delta_time;
			break;
		}		
		break;
	}
}

void update(void) {
	// Maintain target framerate
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);	// calculate the time that has passed since the last frame was rendered
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)						// delay the next frame only if we exceed the FRAME_TARGET_TIME
		SDL_Delay(time_to_wait);

	// Factor converted to secods to be used to update the scene objects
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;
	
	previous_frame_time = SDL_GetTicks();

	// Initialize the array of triangles to render
	num_triangles_to_render = 0;

	//////////////////////////////////
	// Transformations for the mesh //
	//////////////////////////////////
	// Rotation
	mesh.rotation.x = -0.25;
	mesh.rotation.y += 0.5 * delta_time;
	// mesh.rotation.z += 0.5 * delta_time;

	// Scaling
	// mesh.scale.x += 0.002;
	// mesh.scale.y += 0.002;
	// mesh.scale.z += 0.002;

	// Translation
	// mesh.translation.x += 0.01;
	// mesh.translation.y += 0.01;
	// mesh.translation.z += 0.01;

	// Move the mesh away from the camera
	mesh.translation.z = 5.0;

	// Change the camera position
	// camera.position.x += 0.5 * delta_time;
	// camera.position.y += 0.5 * delta_time;

	// Create the view matrix
	vec3_t target = { 0, 0, 1 };
	mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw_angle);
	camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));

	// Offset the camera position in the direction the camera is looking at
	target = vec3_add(camera.position, camera.direction);
	vec3_t up_direction = { 0, 1, 0 };

	view_matrix = mat4_look_at(camera.position, target, up_direction);

	// Create scale, rotation, and translation matrices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

	// Loop through all the triangle faces of the mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {

		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a];
		face_vertices[1] = mesh.vertices[mesh_face.b];
		face_vertices[2] = mesh.vertices[mesh_face.c];

		vec4_t transformed_vertices[3];

		///////////////////////////////////////////////////////////////
		// Apply transformations to the vertices of the current face //
		///////////////////////////////////////////////////////////////
		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);	// convert the current vertex from vec3 to vec4

			// Create a world matrix combining scale, rotation and translation matrices
			world_matrix = mat4_identity();
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			// Multiply the world matrix by the original vector
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

			// Multiply the view matrix by the vector to transform the scene to camera space
			transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

			transformed_vertices[j] = transformed_vertex;
		}

		////////////////////////////
		// Check backface culling //
		////////////////////////////
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		// Get the vector subtraction of B-A and C-A
		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ab);
		vec3_normalize(&vector_ac);

		// Compute the face normal using cross product to find a perpendicular vector
		vec3_t normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&normal);

		// Find the vector between vertex A in the triangle and the camera origin
		vec3_t origin = { 0, 0, 0 };
		vec3_t camera_ray = vec3_sub(origin, vector_a);

		// Calculate how aligned the camera ray is with the face normal using the dot product
		float dot_normal_camera = vec3_dot(normal, camera_ray);

		// Enable or disable back face culling
		if (enable_culling) {
			// Bypass the triangles that are looking away from the camera
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		// Create a polygon from the original transformed triangle to be clipped
		polygon_t polygon = create_polygon_from_triangle(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2])
		);

		// Clip polygon with potential new vertices
		clip_polygon(&polygon);

		// Break the polygon apart back into individual triangles
		triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		// Loop all the assembled triangles after clippig
		for (int t = 0; t < num_triangles_after_clipping; t++) {
			triangle_t triangle_after_clipping = triangles_after_clipping[t];

			////////////////////////////////////////
			// Project the vertices to the screen //
			////////////////////////////////////////
			vec4_t projected_points[3];
			for (int j = 0; j < 3; j++) {
				// Project the current vertex using the projection matrix
				projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle_after_clipping.points[j]);

				// Scale into the view
				projected_points[j].x *= window_width / 2.0;
				projected_points[j].y *= window_height / 2.0;

				// Invert points along Y axis to account for flipped screen cordinate y
				projected_points[j].y *= -1;
				// projected_points[j].x *= -1;

				// Translate the projected point to the middle of the screen
				projected_points[j].x += (window_width / 2.0);
				projected_points[j].y += (window_height / 2.0);
			}

			////////////////////////
			// Calculate lighting //
			////////////////////////
			float light_intensity_factor = -vec3_dot(normal, light.direction);							// invert the result because the light is pointing against the face normal
			uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);	// get the new color based on the angle between the face normal and the light direction

			// Save the projected triangle to the array of triangles to render
			triangle_t triangle_to_render = {
				.points = {
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
				},
				.texcoords = {
					{ mesh_face.a_uv.u, mesh_face.a_uv.v },
					{ mesh_face.b_uv.u, mesh_face.b_uv.v },
					{ mesh_face.c_uv.u, mesh_face.c_uv.v }
				},
				.color = triangle_color,
			};

			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
				triangles_to_render[num_triangles_to_render] = triangle_to_render;
				num_triangles_to_render++;
			}
		}
	}	
}

void render(void) {
	// Render all projected triangles
	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];

		if (show_filled) {
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
				triangle.color
			);
		}
		if (show_textured) {
			draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
				mesh_texture
			);
		}
		if (show_wireframe) {
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				0xFFFFFFFF
			);
		}
		if (show_vertices) {
			draw_rectangle(triangle.points[0].x - 2, triangle.points[0].y - 2, 4, 4, 0xFFFF0000);
			draw_rectangle(triangle.points[1].x - 2, triangle.points[1].y - 2, 4, 4, 0xFFFF0000);
			draw_rectangle(triangle.points[2].x - 2, triangle.points[2].y - 2, 4, 4, 0xFFFF0000);
		}
	}

	render_color_buffer();
	clear_color_buffer(0xFF252525);
	clear_z_buffer();
	SDL_RenderPresent(renderer);
}

// Free memory that was dynamically allocated
void free_resources(void) {
	free(z_buffer);
	free(color_buffer);
	upng_free(png_texture);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while(is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}
