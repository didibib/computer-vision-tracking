#include "cvpch.h"
#include "util.h"
#include "voxel_reconstruction.h"

using namespace team45;

int main(int argc, char** argv)
{
	log::init();
	VoxelReconstruction::showKeys();
	VoxelReconstruction vr(4);
	vr.init(argc, argv);

	log::shutdown();
	return EXIT_SUCCESS;
}
