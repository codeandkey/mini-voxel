#include "VoxelGrid.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

VoxelGrid::VoxelGrid(void) {
	// Allocate the 3-dimensional handle buffer.

	voxel_buffer = new Voxel***[VOXEL_GRID_SIZE_X];

	for (int x = 0; x < VOXEL_GRID_SIZE_X; x++) {
		voxel_buffer[x] = NULL;
		voxel_buffer[x] = new Voxel**[VOXEL_GRID_SIZE_Y];

		for (int y = 0; y < VOXEL_GRID_SIZE_Y; y++) {
			voxel_buffer[x][y] = new Voxel*[VOXEL_GRID_SIZE_Z];

			for (int z = 0; z < VOXEL_GRID_SIZE_Z; z++) {
				voxel_buffer[x][y][z] = NULL;
			}
		}
	}
}

VoxelGrid::~VoxelGrid(void) {
	// Progressive deallocation of an enormous buffer.

	for (int x = 0; x < VOXEL_GRID_SIZE_X; x++) {
		for (int y = 0; y < VOXEL_GRID_SIZE_Y; y++) {
			for (int z = 0; z < VOXEL_GRID_SIZE_Z; z++) {
				delete voxel_buffer[x][y][z];
				voxel_buffer[x][y][z] = NULL;
			}

			delete[] voxel_buffer[x][y];
			voxel_buffer[x][y] = NULL;
		}

		delete[] voxel_buffer[x];
		voxel_buffer[x] = NULL;
	}

	delete[] voxel_buffer;
	voxel_buffer = NULL;
}

bool VoxelGrid::VoxelPresent(int x, int y, int z) {
	int index_x = x + VOXEL_GRID_SIZE_X / 2;
	int index_y = y + VOXEL_GRID_SIZE_Y / 2;
	int index_z = z + VOXEL_GRID_SIZE_Z / 2;

	if (index_x < 0 || index_x >= VOXEL_GRID_SIZE_X) return false;
	if (index_y < 0 || index_y >= VOXEL_GRID_SIZE_Y) return false;
	if (index_z < 0 || index_z >= VOXEL_GRID_SIZE_Z) return false;

	return voxel_buffer[index_x][index_y][index_z] != NULL;
}

Voxel* VoxelGrid::GetVoxel(int x, int y, int z) {
	int index_x = x + VOXEL_GRID_SIZE_X / 2;
	int index_y = y + VOXEL_GRID_SIZE_Y / 2;
	int index_z = z + VOXEL_GRID_SIZE_Z / 2;

	if (index_x < 0 || index_x >= VOXEL_GRID_SIZE_X) {
		printf("[VoxelGrid::GetVoxel] Bad index input!\n");
		return NULL;
	}

	if (index_y < 0 || index_y >= VOXEL_GRID_SIZE_Y) {
		printf("[VoxelGrid::GetVoxel] Bad index input!\n");
		return NULL;
	}

	if (index_z < 0 || index_z >= VOXEL_GRID_SIZE_Z) {
		printf("[VoxelGrid::GetVoxel] Bad index input!\n");
		return NULL;
	}

	return voxel_buffer[index_x][index_y][index_z];
}

void VoxelGrid::SetVoxel(int x, int y, int z, Voxel* target) {
	// First, we convert the input coordinates to the array indices.

	int index_x = x + VOXEL_GRID_SIZE_X / 2;
	int index_y = y + VOXEL_GRID_SIZE_Y / 2;
	int index_z = z + VOXEL_GRID_SIZE_Z / 2;

	if (index_x >= VOXEL_GRID_SIZE_X || index_x < 0) {
		printf("[VoxelGrid::SetVoxel] Bad X index for voxel assignment!\n");
		return;
	}

	if (index_y >= VOXEL_GRID_SIZE_Y || index_y < 0) {
		printf("[VoxelGrid::SetVoxel] Bad Y index for voxel assignment!\n");
		return;
	}

	if (index_z >= VOXEL_GRID_SIZE_Z || index_z < 0) {
		printf("[VoxelGrid::SetVoxel] Bad Z index for voxel assignment!\n");
		return;
	}

	if (voxel_buffer[index_x][index_y][index_z]) {
		delete voxel_buffer[index_x][index_y][index_z];
	}

	voxel_buffer[index_x][index_y][index_z] = target;
}

void VoxelGrid::DrawAll(void) {
	for (int x = 0; x < VOXEL_GRID_SIZE_X; x++) {
		for (int y = 0; y < VOXEL_GRID_SIZE_Y; y++) {
			for (int z = 0; z < VOXEL_GRID_SIZE_Z; z++) {
				if (voxel_buffer[x][y][z]) {
					voxel_buffer[x][y][z]->Draw(x - VOXEL_GRID_SIZE_X / 2, y - VOXEL_GRID_SIZE_Y / 2, z - VOXEL_GRID_SIZE_Z / 2);
				}
			}
		}
	}
}
