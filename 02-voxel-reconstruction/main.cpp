#include "cvpch.h"
#include "util.h"
#include "voxel_reconstruction.h"

using namespace nl_uu_science_gmt;

int main(int argc, char** argv)
{
	VoxelReconstruction::showKeys();
	VoxelReconstruction vr("data" + std::string(PATH_SEP), 4);
	vr.run(argc, argv);

	return EXIT_SUCCESS;
}
