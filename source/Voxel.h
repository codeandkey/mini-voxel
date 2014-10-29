#pragma once

// This isn't exactly an engine, but we do need a class to contain and draw voxels.
// To preserve codespace, the Voxel class will have its own texture caching system.

// Make sure to call Voxel::FlushTextureCache() to flush the allocated texture cache!
// This should be called during program resource deallocation.

class Voxel {
public:
	enum VoxelShape {
		Cuboid,
		Pyramid,
	};

	Voxel(float r, float g, float b, bool* occlude, VoxelShape mode = VoxelShape::Cuboid);
	~Voxel(void);

	void SetColor(float r, float g, float b);
	void GetColor(float* r, float* g, float* b);
	void Draw(int x, int y, int z);
	void SetOcclude(float* occlude);
	void SetDrawMode(VoxelShape mode);
	bool IsFullyOccluded(void);
	VoxelShape GetDrawMode(void);
private:
	bool occlude[6];
	float r[6], g[6], b[6]; // We exclude the transparency channel from the color. Binary-space tree partitioning and quadtree ordering is out of the scope of this program.
	float raw_r, raw_g, raw_b;

	VoxelShape draw_mode;
	// Voxels do not need to and will not keep track of their position.
};
