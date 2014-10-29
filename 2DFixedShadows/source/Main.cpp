#include "RecoilRLImage.h"
#include "Defaults.h"

#include <cstdlib>
#include <cstdio>
#include <cmath> // For cosf and sinf.

#include <algorithm> // For std::min, std::max, and std::pow.

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glew.h> // More GL function pointers.
#include <GLFW/glfw3.h> // Windowing system.

/*
 * This program will be very compact and without structure.
 * It is just an algorithmic demonstration.
 */

// Global program constants.
static const float DEFAULT_EXTRUDE_DISTANCE = 6.0f;
static const unsigned int MAX_CASTERS = 16;
static const unsigned int MAX_CASTER_SIDES = 5;
static const float SHADOW_COLOR_R = 0.0f;
static const float SHADOW_COLOR_G = 0.0f;
static const float SHADOW_COLOR_B = 0.0f;
static const float SHADOW_COLOR_A = 1.0f;

static const char* texture_background_filename = "assets/textures/background.png";

// Global program variables.
static GLFWwindow* glfw_window_handle = NULL;
static GLFWvidmode* glfw_video_mode = NULL;

static int glfw_window_width = -1, glfw_window_height = -1;
static bool glfw_window_fullscreen = false, glfw_window_vertical_retrace = false;
static int glfw_window_samples = 0;

// Global graphical variables.
static Recoil::RenderLow::Image* texture_caster = NULL;
static Recoil::RenderLow::Image* texture_background = NULL;
static Recoil::RenderLow::Image* texture_light = NULL;

// Global algorithm variables.
static float* hex_buffer = NULL;

// Program function declarations.
bool InitializeContext(bool native_fullscreen = false);
bool InitializeTextures(void);
void ReleaseTextures(void);

float DotProduct(float x1, float y1, float x2, float y2);
void CrossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float* out_x, float* out_y, float* out_z);

void ClearBuffers(float r, float g, float b);
void RenderScene(float light_x, float light_y);
void SwapBuffers(void);

// Algorithm function declarations.
void GenerateRegularPolygon(unsigned int sides, float** xy_buffer, float radius);
void ReleasePolygon(float* xy_buffer);
void DrawPolygonShadow(unsigned int sides, float* xy_buffer, float offset_x, float offset_y);
void DrawPolygon(unsigned int sides, float* xy_buffer, float offset_x, float offset_y);

void CalculateLineNormal(float x1, float y1, float x2, float y2, float* out_normal_x, float* out_normal_y);
void ExtrudeLine(float x1, float y1, float x2, float y2, float extrude_factor, float* out_x1, float* out_y1, float* out_x2, float* out_y2);
bool CheckBackFacing(float x1, float y1, float x2, float y2);
void NormalizeVec2(float x, float y, float* out_x, float* out_y);

int main(void) {
	printf("[2DS] Initializing subsystems..\n");

	glfw_window_fullscreen = Defaults::setting_fullscreen_mode;
	glfw_window_vertical_retrace = Defaults::setting_vertical_retrace;

	if (glfw_window_fullscreen) {
		glfw_window_width = Defaults::setting_fullscreen_width;
		glfw_window_height = Defaults::setting_fullscreen_height;
	} else {
		glfw_window_width = Defaults::setting_window_width;
		glfw_window_height = Defaults::setting_window_height;
	}

	bool result = false;
	result = InitializeContext(Defaults::setting_native_fullscreen_force);

	if (!result) {
		printf("[2DS] Failed to initialize graphical context.\n");
		return 0;
	}

	result = InitializeTextures();

	if (!result) {
		printf("[2DS] Failed to initialize textures.\n");
		return 0;
	}

	float delta = 0.0f;

	// Using a state tracker variable is no different from keeping an infinite loop running and breaking it, except it takes unnecessary codespace.
	while(true) {
		delta += 0.01f;

		GenerateRegularPolygon(6, &hex_buffer, cos(delta) * 0.1f);

		ClearBuffers(0.0f, 0.0f, 0.0f);
		RenderScene(0.0f, 0.0f);
		DrawPolygon(6, hex_buffer, cos(delta) * 0.4f, sin(delta) * 0.4f);
		DrawPolygon(6, hex_buffer, cos(delta * 2.0f) * 0.7f, sin(delta * 2.0f) * 0.7f);
		DrawPolygonShadow(6, hex_buffer, cos(delta) * 0.4f, sin(delta) * 0.4f);
		DrawPolygonShadow(6, hex_buffer, cos(delta * 2.0f) * 0.7f, sin(delta * 2.0f) * 0.7f);
		SwapBuffers();

		ReleasePolygon(hex_buffer);

		if (glfwGetKey(glfw_window_handle, GLFW_KEY_ESCAPE) || glfwWindowShouldClose(glfw_window_handle)) break;
	}

	ReleaseTextures();
	return 0;
}

void RenderScene(float light_x, float light_y) {
	// We will only have one light for the scene, because the legacy OpenGL API is really sporadic about pixel blending.
	// It will be positioned externally and passed in as arguments.

	// First, we render the background and the light.

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Some messy quad vertex triangulation to make this background gradient fit every resolution.

	glBegin(GL_TRIANGLES);
	glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
	glVertex3f(((float) glfw_window_width) / -700.0f, ((float) glfw_window_height) / 700.0f, 0.0f);
	glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
	glVertex3f(((float) glfw_window_width) / -700.0f, ((float) glfw_window_height) / -700.0f, 0.0f);
	glColor4f(0.15f, 0.15f, 0.15f, 1.0f);
	glVertex3f(((float) glfw_window_width) / 700.0f, ((float) glfw_window_height) / -700.0f, 0.0f);
	glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
	glVertex3f(((float) glfw_window_width) / -700.0f, ((float) glfw_window_height) / 700.0f, 0.0f);
	glColor4f(0.15f, 0.15f, 0.15f, 1.0f);
	glVertex3f(((float) glfw_window_width) / 700.0f, ((float) glfw_window_height) / -700.0f, 0.0f);
	glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
	glVertex3f(((float) glfw_window_width) / 700.0f, ((float) glfw_window_height) / 700.0f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 20; i++) {
		float angle = ((float) i) * ((3.141f * 2.0f) / ((float) 20)) ;

		glVertex3f(cos(angle) * 0.05f, sin(angle) * 0.05f, 0.0f);
	}
	glEnd();
}

void DrawPolygonShadow(unsigned int sides, float* xy_buffer, float offset_x, float offset_y) {
	// xy_buffer needs to be wound in a counter-clockwise order, otherwise the back-facing algorithm will be reversed and shadows will cover the caster.
	// The counter-clockwise order is already done for you when using GenerateNormalPolygon().

	// First, we draw the shadow.
	// This is a transparent black overlay covering shadowed areas.
	// To generate the shadow vertices, we iterate through each line and find the back-facing lines.

	int first_index = 0;
	int second_index = 1;

	// We will cycle through all of the sides : (0, 1 -> 1, 2 -> 2, 3 -> 3, 0)

	for (int i = 0; i < sides; i++) {
		first_index = i;
		second_index = first_index + 1;

		if (second_index >= sides) {
			second_index = 0;
		}

		// We now have the cycle completed.
		// We grab the vertices of each point.

		float x1 = xy_buffer[first_index * 2] + offset_x;
		float y1 = xy_buffer[first_index * 2 + 1] + offset_y;
		float x2 = xy_buffer[second_index * 2] + offset_x;
		float y2 = xy_buffer[second_index * 2 + 1] + offset_y;

		// We have the coordinates for the current side.
		// We check to see if it is back-facing.

		bool back_facing = CheckBackFacing(x1, y1, x2, y2);

		glBegin(GL_TRIANGLE_FAN);

		if (back_facing) {
			// We extrude the vertices and triangulate the projection against the original.
			float x3, y3, x4, y4;
			ExtrudeLine(x1, y1, x2, y2, DEFAULT_EXTRUDE_DISTANCE, &x3, &y3, &x4, &y4);

			glColor4f(SHADOW_COLOR_R, SHADOW_COLOR_G, SHADOW_COLOR_B, SHADOW_COLOR_A / 2.0f);
			glVertex3f(x1, y1, 0.0f);
			glVertex3f(x2, y2, 0.0f);
			glColor4f(SHADOW_COLOR_R, SHADOW_COLOR_G, SHADOW_COLOR_B, SHADOW_COLOR_A);
			glVertex3f(x4, y4, 0.0f);
			glVertex3f(x3, y3, 0.0f);
		}

		glEnd();
	}
}

void DrawPolygon(unsigned int sides, float* xy_buffer, float offset_x, float offset_y) {
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < sides; i++) {
		glVertex3f(xy_buffer[i * 2] + offset_x, xy_buffer[i * 2 + 1] + offset_y, 0.0f);
	}
	glEnd();
}

void ClearBuffers(float r, float g, float b) {
	glClearColor(r, g, b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void SwapBuffers(void) {
	glfwPollEvents();
	glfwSwapInterval(glfw_window_vertical_retrace ? 1 : 0);
	glfwSwapBuffers(glfw_window_handle);
}

void ExtrudeLine(float x1, float y1, float x2, float y2, float extrude_factor, float* out_x1, float* out_y1, float* out_x2, float* out_y2) {
	*out_x1 = x1 * extrude_factor;
	*out_x2 = x2 * extrude_factor;
	*out_y1 = y1 * extrude_factor;
	*out_y2 = y2 * extrude_factor;
}

void CalculateLineNormal(float x1, float y1, float x2, float y2, float* out_x, float* out_y) {
	*out_x = -y2 + y1;
	*out_y = x2 - x1;
}

bool CheckBackFacing(float x1, float y1, float x2, float y2) {
	// First, we grab the normal of the line and then normalize it.

	float normal_x = -10.0f, normal_y = -10.0f;
	CalculateLineNormal(x1, y1, x2, y2, &normal_x, &normal_y);

	// We have the normalized unit vector.
	// We check if it is back-facing.

	float projection = DotProduct(-x1, -y1, normal_x, normal_y);

	return projection < 0.0f;
}

bool InitializeContext(bool native) {
	glfwInit();
	glewInit();

	glfw_video_mode = (GLFWvidmode*) glfwGetVideoMode(glfwGetPrimaryMonitor());

	if (!glfw_video_mode) {
		printf("[InitializeContext] Failed to query primary video mode.\n");
		return false;
	}

	if (native) {
		glfw_window_width = glfw_video_mode->width;
		glfw_window_height = glfw_video_mode->height;
		glfw_window_fullscreen = true;
	}

	glfwWindowHint(GLFW_SAMPLES, glfw_window_samples);

	glfw_window_handle = glfwCreateWindow(glfw_window_width, glfw_window_height, "Shadows2DFixed", (glfw_window_fullscreen) ? glfwGetPrimaryMonitor() : NULL, NULL);

	if (!glfw_window_handle) {
		printf("[InitializeContext] Failed to create GLFW window instance.\n");
		return false;
	}

	glfwMakeContextCurrent(glfw_window_handle);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-glfw_window_width / 700.0f, glfw_window_width / 700.0f, -glfw_window_height / 700.0f, glfw_window_height / 700.0f);
	glMatrixMode(GL_MODELVIEW);

	return true;
}

bool InitializeTextures(void) {
	texture_background = new Recoil::RenderLow::Image(texture_background_filename, false, false, 1.0f);

	return true;
}

void ReleaseTextures(void) {
	if (texture_background) {
		delete texture_background;
		texture_background = NULL;
	}
}

float DotProduct(float x1, float y1, float x2, float y2) {
	return (x1 * x2) + (y1 * y2);
}

void CrossProduct(float x1, float y1, float z1, float x2, float y2, float z2, float* out_x, float* out_y, float* out_z) {
	*out_x = (y1 * z2) - (y2 * z1);
	*out_y = (x2 * z1) - (x1 * z2);
	*out_z = (x1 * y2) - (x2 * y1);
}

void NormalizeVec2(float x, float y, float* out_x, float* out_y) {
	float length = sqrt(std::pow(x, 2) + std::pow(y, 2));

	*out_x = x / length;
	*out_y = y / length;
}

void GenerateRegularPolygon(unsigned int sides, float** xy_buffer, float radius) {
	// xy_buffer should be sized to sides * 2.
	// (One vertex for each side.)

	*xy_buffer = new float[sides * 2];

	for (int i = 0; i < sides; i++) {
		// We will rotate around in a counter-clockwise fashion by subtracting the iteration from 2pi.
		float angle = (3.141f * 2.0f) - (((3.141f * 2.0f) / ((float) sides)) * ((float) i));

		(*xy_buffer)[i * 2] = cosf(angle) * radius;
		(*xy_buffer)[i * 2 + 1] = sinf(angle) * radius;
	}
}

void ReleasePolygon(float* xy_buffer) {
	delete[] xy_buffer;
}
