#include "Voxel.h"

#include <GL/glew.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

Voxel::Voxel(float color_r, float color_g, float color_b, bool* occlude_buffer, Voxel::VoxelShape mode) {
	if (occlude_buffer) {
		memcpy(occlude, occlude_buffer, sizeof(bool) * 6);
	} else {
		memset(occlude, 0, sizeof(bool) * 6);
	}

	SetColor(color_r, color_g, color_b);
	SetDrawMode(mode);
}

Voxel::~Voxel(void) {
}

Voxel::VoxelShape Voxel::GetDrawMode(void) {
	return draw_mode;
}

bool Voxel::IsFullyOccluded(void) {
	bool flag = true;
	for (int i = 0; i < 6; i++) flag &= occlude[i];

	return flag;
}

void Voxel::SetDrawMode(Voxel::VoxelShape mode) {
	this->draw_mode = mode;
}

void Voxel::SetOcclude(float* occ) {
	if (occ) {
		memcpy(occlude, occ, sizeof(bool) * 6);
	}
}

void Voxel::SetColor(float r, float g, float b) {
	raw_r = r;
	raw_g = g;
	raw_b = b;

	for (int i = 0; i < 6; i++) {
		float noise_magnitude = 0.1f;
		float noise_offset = (((rand() % 1000) / 1000.0f) * noise_magnitude) - noise_magnitude / 2.0f;
		this->r[i] = r + noise_offset;
		this->g[i] = g + noise_offset;
		this->b[i] = b + noise_offset;
	}
}

void Voxel::GetColor(float* r, float* g, float* b) {
	*r = this->raw_r;
	*g = this->raw_g;
	*b = this->raw_b;
}

void Voxel::Draw(int x, int y, int z) {
	// I use this function all too often for voxel debugging.
	// printf("[Voxel] Drawing at %d, %d, %d\n", x, y, z);

	// Possible todo : Voxel-space occlusion by only drawing a potentially visible set.
	// There aren't going to be too many voxels present in this program, so we don't have to worry about that immediately.

	// To completely draw the voxel, we have to wind the cube. Ugh.
	// x, y represents the CENTER of the voxel being drawn.
	// The dimensions should all be 1, centered on the origin.
	bool occlude_flag = true;
	for (int i = 0; i < 6; i++) occlude_flag &= occlude[i];

	if (occlude_flag) return;

	switch (draw_mode) {
	case VoxelShape::Cuboid:
		glMatrixMode(GL_MODELVIEW);
		glBegin(GL_TRIANGLES);
			// Front face.
			if (!occlude[0]) {
				glColor4f(r[0], g[0], b[0], 1.0f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
			}

			// Back face.
			if (!occlude[1]) {
				glColor4f(r[1], g[1], b[1], 1.0f);
				glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
			}

			// Top face.
			if (!occlude[2]) {
				glColor4f(r[2], g[2], b[2], 1.0f);
				glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
			}

			// Bottom face.
			if (!occlude[3]) {
				glColor4f(r[3], g[3], b[3], 1.0f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
			}

			// Right face.
			if (!occlude[4]) {
				glColor4f(r[4], g[4], b[4], 1.0f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y + 0.5f, z - 0.5f);
			}

			// Left face.
			if (!occlude[5]) {
				glColor4f(r[5], g[5], b[5], 1.0f);
				glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y + 0.5f, z + 0.5f);
			}
		glEnd();
		break;
	case VoxelShape::Pyramid:
		glBegin(GL_TRIANGLES);
			if (!occlude[3]) {
				glColor4f(r[0], g[0], b[0], 1.0f);
				glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
				glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
				glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
			}

			glColor4f(r[1], g[1], b[1], 1.0f);
			glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
			glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
			glVertex3f(x, y + 0.5f, z);

			glColor4f(r[2], g[2], b[2], 1.0f);
			glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
			glVertex3f(x - 0.5f, y - 0.5f, z + 0.5f);
			glVertex3f(x, y + 0.5f, z);

			glColor4f(r[3], g[3], b[3], 1.0f);
			glVertex3f(x + 0.5f, y - 0.5f, z + 0.5f);
			glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
			glVertex3f(x, y + 0.5f, z);

			glColor4f(r[4], g[4], b[4], 1.0f);
			glVertex3f(x + 0.5f, y - 0.5f, z - 0.5f);
			glVertex3f(x - 0.5f, y - 0.5f, z - 0.5f);
			glVertex3f(x, y + 0.5f, z);
		glEnd();
		break;
	}
}

