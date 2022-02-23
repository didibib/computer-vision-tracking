#include "cvpch.h"
#include "util.h"
#include "voxel_reconstruction.h"

using namespace team45;

int main(int argc, char** argv)
{
	VoxelReconstruction::showKeys();
	VoxelReconstruction vr(4);
	vr.init(argc, argv);

	return EXIT_SUCCESS;
}
