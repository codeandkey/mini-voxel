#include "Implementation.h"
#include "Voxel.h"
#include "VoxelGrid.h"

#include <ctime>

/* JT Stanley
 * Environment - a fixed-function 3D platforming environment.
 * Written with the legacy OpenGL fixed-function pipeline.
 * Cross-platform, assuming your OpenGL version is 1.1 or higher.
 */

// Global graphical constants.

static unsigned int glfw_window_width = 740;
static unsigned int glfw_window_height = 480;
static bool glfw_vertical_retrace = true;
static bool glfw_mode_fullscreen = true;
static const bool glfw_native_fullscreen_force = true;
static unsigned int glfw_samples = 0;

static const float view_fov = 90.0f;

// Global graphical variables.

static GLFWwindow* glfw_window_handle = NULL;
static GLFWvidmode* glfw_primary_video_mode = NULL;

static bool context_initialized = false;

// Camera movement variables.
static float camera_x = 0.0f, camera_y = 2.5f, camera_z = 0.0f;
static float camera_angle = 0.0f;
static float camera_width = 1.0f; // Width : X
static float camera_height = 1.5f; // Height : Y
static float camera_length = 1.0f; // Length : Z
static const float gravity = 0.01f;
static float camera_xspeed = 0.0f;
static float camera_yspeed = 0.0f;
static float camera_zspeed = 0.0f;

// Global program instances.

static VoxelGrid* program_voxel_grid_handle = NULL; // Since the VoxelGrid only stores the handles, we have to create the Voxel objects ourselves.

// Graphical function declarations.
bool InitializeContext(void);

void ClearBuffers(void);
void SwapBuffers(void);

// Algorithmic function declarations.
void GenerateVoxelMap(VoxelGrid* target_voxel_grid);
void SetCamera(void);
void SetPerspective(void);

void GenerateBlock(int x1, int y1, int z1, int x2, int y2, int z2, VoxelGrid* target, float r, float g, float b, Voxel::VoxelShape shape = Voxel::VoxelShape::Cuboid);
void SliceBlock(int x1, int y1, int z1, int x2, int y2, int z2, VoxelGrid* target);

// Global function definitions.

int main(int argc, char** argv) {
	srand(time(NULL));

	program_voxel_grid_handle = new VoxelGrid();

	GenerateVoxelMap(program_voxel_grid_handle);
	printf("[Implementation] Generated voxel grid.\n");
	// We manually copy our voxel map to the VoxelGrid object.

	bool result = false;
	result = InitializeContext();

	if (!result) {
		printf("[Implementation] Failed to initialize OpenGL context.\n");
		return 1;
	}

	printf("[Implementation] Starting mainloop.\n");

	while(true) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		SetPerspective();
		SetCamera();

		ClearBuffers();
		program_voxel_grid_handle->DrawAll();
		SwapBuffers();

		if (glfwGetKey(::glfw_window_handle, GLFW_KEY_ESCAPE) || glfwWindowShouldClose(::glfw_window_handle)) {
			break;
		}
	}

	delete program_voxel_grid_handle;
	program_voxel_grid_handle = NULL;

	glfwDestroyWindow(::glfw_window_handle);
	glfwTerminate();

	return 0;
}

void GenerateVoxelMap(VoxelGrid* target_voxel_grid) {
	/* Instead of using a stack-allocated Voxel array, we will dynamically push Voxel objects to the grid.
	 * This allows for much less memory-based data copying.
	 * This does come at the detriment of being limited in the amount of Voxels.
	 */

	// We place a 20x20 simple floor and ceiling.

	// Floors / Ceilings.

	GenerateBlock(-20, 0, -20, 20, 0, 20, program_voxel_grid_handle, 0.2f, 0.6f, 0.1f);
	GenerateBlock(-20, 10, -20, 20, 10, 20, program_voxel_grid_handle, 0.0f, 0.8f, 1.0f);
	GenerateBlock(-20, 20, -20, 20, 20, 20, program_voxel_grid_handle, 0.1f, 0.1f, 0.1f);

	// FB walls.

	GenerateBlock(-20, 1, -20, 20, 20, -20, program_voxel_grid_handle, 0.2f, 0.2f, 0.2f);
	GenerateBlock(-20, 1, 20, 20, 20, 20, program_voxel_grid_handle, 0.2f, 0.2f, 0.2f);

	// LR walls.

	GenerateBlock(-20, 1, -19, -20, 19, 19, program_voxel_grid_handle, 0.2f, 0.2f, 0.2f);
	GenerateBlock(20, 1, -19, 20, 19, 19, program_voxel_grid_handle, 0.2f, 0.2f, 0.2f);

	GenerateBlock(-1, 1, -6, 1, 2, -4, program_voxel_grid_handle, 0.4f, 0.1f, 0.4f);
	GenerateBlock(-4, 1, -6, -4, 4, -4, program_voxel_grid_handle, 0.2f, 0.1f, 0.5f);
	GenerateBlock(-8, 1, -7, -5, 5, -5, program_voxel_grid_handle, 0.1f, 0.4f, 0.3f);
	GenerateBlock(-8, 1, -1, -8, 3, 1, program_voxel_grid_handle, 0.5f, 0.5f, 0.1f);
	GenerateBlock(-6, 1, 3, -4, 5, 5, program_voxel_grid_handle, 0.2f, 0.4f, 0.4f);
	GenerateBlock(-6, 1, 8, -4, 6, 13, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(-6, 5, 10, -4, 7, 13, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(-6, 7, 12, -4, 9, 13, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f, Voxel::VoxelShape::Pyramid);
	GenerateBlock(-19, 11, -19, 19, 11, 19, program_voxel_grid_handle, 0.6f, 0.0f, 0.0f, Voxel::VoxelShape::Pyramid);
	GenerateBlock(-6, 11, 0, -4, 11, 10, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(-7, 11, -10, -3, 13, -5, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(0, 11, -10, 5, 15, -5, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(5, 1, -10, 15, 5, 10, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	GenerateBlock(-7, 17, -7, 0, 17, 0, program_voxel_grid_handle, 0.2f, 0.1f, 0.2f);
	SliceBlock(-7, 1, -6, -6, 5, -5, program_voxel_grid_handle);
	SliceBlock(-7, 1, -7, -6, 2, -7, program_voxel_grid_handle);
	SliceBlock(-10, 10, 11, 0, 11, 13, program_voxel_grid_handle);
}

bool InitializeContext(void) {
	if (context_initialized) {
		printf("[InitializeContext] OpenGL context already initialized!\n");
		return false;
	}

	glfwInit();
	glewInit();

	::glfw_primary_video_mode = (GLFWvidmode*) glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (!::glfw_primary_video_mode) {
		printf("[InitializeContext] Failed to query video context.\n");
		return false;
	}

	if (::glfw_native_fullscreen_force) {
		::glfw_window_width = ::glfw_primary_video_mode->width;
		::glfw_window_height = ::glfw_primary_video_mode->height;
		::glfw_mode_fullscreen = true;
	}

	glfwWindowHint(GLFW_SAMPLES, ::glfw_samples);

	::glfw_window_handle = glfwCreateWindow(::glfw_window_width, ::glfw_window_height, "VoxelEnvironment", ::glfw_mode_fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

	if (!::glfw_window_handle) {
		printf("[InitializeContext] Failed to initialize GLFW/GL context.\n");
		return false;
	}

	glfwMakeContextCurrent(::glfw_window_handle);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, ::glfw_window_width, ::glfw_window_height);

	SetPerspective();
	return true;
}

void SetPerspective(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view_fov, (float) ::glfw_window_width / (float) ::glfw_window_height, 0.1f, 180.0f);
}

void ClearBuffers(void) {
	glClearColor(0.0f, 0.5f, 0.8f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SwapBuffers(void) {
	glfwPollEvents();
	glfwSwapInterval(::glfw_vertical_retrace ? 1 : 0);
	glfwSwapBuffers(::glfw_window_handle);
}

void SetCamera(void) {
	const float camera_speed = 0.01f;

	if (glfwGetKey(::glfw_window_handle, GLFW_KEY_RIGHT)) camera_angle += 0.04f;
	if (glfwGetKey(::glfw_window_handle, GLFW_KEY_LEFT)) camera_angle -= 0.04f;

	if (glfwGetKey(::glfw_window_handle, 'A')) {
		camera_xspeed += cos(camera_angle - (3.141f / 2.0f)) * camera_speed;
		camera_zspeed += sin(camera_angle - (3.141f / 2.0f)) * camera_speed;
	}

	if (glfwGetKey(::glfw_window_handle, 'D')) {
		camera_xspeed += cos(camera_angle + (3.141f / 2.0f)) * camera_speed;
		camera_zspeed += sin(camera_angle + (3.141f / 2.0f)) * camera_speed;
	}

	if (glfwGetKey(::glfw_window_handle, 'W')) {
		camera_xspeed += cos(camera_angle) * camera_speed;
		camera_zspeed += sin(camera_angle) * camera_speed;
	}

	if (glfwGetKey(::glfw_window_handle, 'S')) {
		camera_xspeed += -cos(camera_angle) * camera_speed;
		camera_zspeed += -sin(camera_angle) * camera_speed;
	}

	camera_yspeed -= gravity;
	camera_zspeed /= 1.05f;
	camera_xspeed /= 1.05f;

	// Now, we check for collisions with other blocks.

	// I've never done a triple loop like this, but as we need to iterate 3 dimensions..
	for (int x = -20; x <= 20; x++) for (int y = -20; y <= 20; y++) for (int z = -20; z <= 20; z++) {
		if (!program_voxel_grid_handle->VoxelPresent(x, y, z)) continue;
		if (program_voxel_grid_handle->GetVoxel(x, y, z)->IsFullyOccluded()) continue;
		// There is a voxel at the current location.

		bool overlap_x = (camera_x + camera_width / 2.0f > x - 0.5f && camera_x - camera_width / 2.0f < x + 0.5f);
		bool overlap_future_x = (camera_x + camera_xspeed + camera_width / 2.0f >= x - 0.5f && camera_x + camera_xspeed - camera_width / 2.0f <= x + 0.5f);

		bool overlap_y = (camera_y > y - 0.5f && camera_y - camera_height < y + 0.5f);
		bool overlap_future_y = (camera_y + camera_yspeed >= y - 0.5f && camera_y + camera_yspeed - camera_height <= y + 0.5f);

		bool overlap_z = (camera_z + camera_length / 2.0f > z - 0.5f && camera_z - camera_length / 2.0f < z + 0.5f);
		bool overlap_future_z = (camera_z + camera_zspeed + camera_length / 2.0f >= z - 0.5f && camera_z + camera_zspeed - camera_length / 2.0f <= z + 0.5f);

		if (overlap_future_y && overlap_x && overlap_z) {
			if (camera_yspeed < 0.0f) {
				camera_y = y + 0.5f + camera_height;
				camera_yspeed = 0.0f;

				if (program_voxel_grid_handle->GetVoxel(x, y, z)->GetDrawMode() == Voxel::Pyramid) {
					camera_x = 0.0f;
					camera_y = 0.5f + camera_height;
					camera_z = 0.0f;
					camera_xspeed = 0.0f;
					camera_yspeed = 0.0f;
					camera_zspeed = 0.0f;
				}

				if (glfwGetKey(::glfw_window_handle, GLFW_KEY_SPACE)) camera_yspeed = 0.2f;
			} else if (camera_yspeed > 0.0f) {
				camera_y = y - 0.5f;
				camera_yspeed = 0.0f;
			}
		}

		if (!overlap_z && overlap_future_z && overlap_y && overlap_x) {
			if (camera_zspeed < 0.0f) {
				camera_z = z + 0.5f + camera_length / 2.0f;
				camera_zspeed = 0.0f;
			} else if (camera_zspeed > 0.0f) {
				camera_z = z - 0.5f - camera_length / 2.0f;
				camera_zspeed = 0.0f;
			}
		}

		if (!overlap_x && overlap_future_x && overlap_y && overlap_z) {
			if (camera_xspeed < 0.0f) {
				camera_x = x + 0.5f + camera_width / 2.0f;
				camera_xspeed = 0.0f;
			} else if (camera_xspeed > 0.0f) {
				camera_x = x - 0.5f - camera_width / 2.0f;
				camera_xspeed = 0.0f;
			}
		}
	}

	camera_x += camera_xspeed;
	camera_y += camera_yspeed;
	camera_z += camera_zspeed;

	float camera_target_x = camera_x + cos(camera_angle);
	float camera_target_z = camera_z + sin(camera_angle);

	gluLookAt(camera_x, camera_y, camera_z, camera_target_x, camera_y, camera_target_z, 0.0f, 1.0f, 0.0f);
}

void GenerateBlock(int x1, int y1, int z1, int x2, int y2, int z2, VoxelGrid* target_grid, float r, float g, float b, Voxel::VoxelShape shape) {
	// Useful debugging function.
	// printf("[GenerateBlock] Recieved %d, %d, %d, %d, %d, %d\n", x1, y1, z1, x2, y2, z2);

	bool occlude[6] = {0};

	// This is where the occlusion algorithm takes place.
	// 0 : Front, 1 : Back, 2 : Top , 3 : Bottom, 4 : Right, 5 : Left
	for (int x = x1; x <= x2; x++) for (int y = y1; y <= y2; y++) for (int z = z1; z <= z2; z++) {
		target_grid->SetVoxel(x, y, z, new Voxel(0.0f, 0.0f, 0.0f, occlude, shape));
	}

	for (int x = x1; x <= x2; x++) for (int y = y1; y <= y2; y++) for (int z = z1; z <= z2; z++) {
		for (int i = 0; i < 6; i++) occlude[i] = false;

		if (target_grid->VoxelPresent(x - 1, y, z)) if (target_grid->GetVoxel(x - 1, y, z)->GetDrawMode() == Voxel::Cuboid) occlude[5] = true;
		if (target_grid->VoxelPresent(x + 1, y, z)) if (target_grid->GetVoxel(x + 1, y, z)->GetDrawMode() == Voxel::Cuboid) occlude[4] = true;
		if (target_grid->VoxelPresent(x, y + 1, z)) if (target_grid->GetVoxel(x, y + 1, z)->GetDrawMode() == Voxel::Cuboid) occlude[2] = true;
		if (target_grid->VoxelPresent(x, y - 1, z)) if (target_grid->GetVoxel(x, y - 1, z)->GetDrawMode() == Voxel::Cuboid) occlude[3] = true;
		if (target_grid->VoxelPresent(x, y, z + 1)) if (target_grid->GetVoxel(x, y, z + 1)->GetDrawMode() == Voxel::Cuboid) occlude[0] = true;
		if (target_grid->VoxelPresent(x, y, z - 1)) if (target_grid->GetVoxel(x, y, z - 1)->GetDrawMode() == Voxel::Cuboid) occlude[1] = true;

		target_grid->SetVoxel(x, y, z, new Voxel(r, g, b, occlude, shape));
	}
}

void SliceBlock(int x1, int y1, int z1, int x2, int y2, int z2, VoxelGrid* target) {
	for (int x = x1; x <= x2; x++) for (int y = y1; y <= y2; y++) for (int z = z1; z <= z2; z++) {
		target->SetVoxel(x, y, z, NULL);
	}

	for (int x = x1 - 1; x <= x2 + 1; x++) for (int y = y1 - 1; y <= y2 + 1; y++) for (int z = z1 - 1; z <= z2 + 1; z++) {
		if (x == x1 - 1 || x == x2 + 1 || y == y1 - 1 || y == y2 + 1 || z == z1 - 1 || z == z2 + 1) {
			// Recalculate the occlusion buffer for this voxel, it is on the outline.

			if (!target->GetVoxel(x, y, z)) continue;
			Voxel::VoxelShape shape = target->GetVoxel(x, y, z)->GetDrawMode();

			bool occlude[6] = {0};
			// Fix cuboid / pyramid occlusion here.

			if (target->VoxelPresent(x - 1, y, z)) if (target->GetVoxel(x - 1, y, z)->GetDrawMode() == Voxel::Cuboid) occlude[5] = true;
			if (target->VoxelPresent(x + 1, y, z)) if (target->GetVoxel(x + 1, y, z)->GetDrawMode() == Voxel::Cuboid) occlude[4] = true;
			if (target->VoxelPresent(x, y + 1, z)) if (target->GetVoxel(x, y + 1, z)->GetDrawMode() == Voxel::Cuboid) occlude[2] = true;
			if (target->VoxelPresent(x, y - 1, z)) if (target->GetVoxel(x, y - 1, z)->GetDrawMode() == Voxel::Cuboid) occlude[3] = true;
			if (target->VoxelPresent(x, y, z + 1)) if (target->GetVoxel(x, y, z + 1)->GetDrawMode() == Voxel::Cuboid) occlude[0] = true;
			if (target->VoxelPresent(x, y, z - 1)) if (target->GetVoxel(x, y, z - 1)->GetDrawMode() == Voxel::Cuboid) occlude[1] = true;

			float r, g, b;
			target->GetVoxel(x, y, z)->GetColor(&r, &g, &b);

			target->SetVoxel(x, y, z, new Voxel(r, g, b, occlude, shape));
		}
	}
}
