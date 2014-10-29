#pragma once

#include "Voxel.h"

#ifndef VOXEL_GRID_SIZE_X
#define VOXEL_GRID_SIZE_X 128
#endif

#ifndef VOXEL_GRID_SIZE_Y
#define VOXEL_GRID_SIZE_Y 128
#endif

#ifndef VOXEL_GRID_SIZE_Z
#define VOXEL_GRID_SIZE_Z 128
#endif

class VoxelGrid {
public:
	VoxelGrid(void);
	~VoxelGrid(void);

	void SetVoxel(int x, int y, int z, Voxel* target);
	bool VoxelPresent(int x, int y, int z);
	Voxel* GetVoxel(int x, int y, int z);
	void DrawAll(void);
private:
	Voxel**** voxel_buffer; // Quadruple pointer. Wow!
};
