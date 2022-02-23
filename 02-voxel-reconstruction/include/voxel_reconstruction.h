#pragma once
#ifndef VOXELRECONSTRUCTION_H
#define VOXELRECONSTRUCTION_H

#include <string>
#include <vector>

#include "camera.h"

namespace team45
{

class VoxelReconstruction
{
	const int m_cam_views_amount;

	std::vector<Camera*> m_cam_views;

	

public:
	VoxelReconstruction(const int);
	virtual ~VoxelReconstruction();

	static void showKeys();

	void init(int, char**);
};

} /* namespace team45 */

#endif /* VOXELRECONSTRUCTION_H */
