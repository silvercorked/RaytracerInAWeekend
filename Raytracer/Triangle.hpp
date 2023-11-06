#pragma once

/*
	not doing this file until after finishing book 3.
	- big moves with this file:
		- allow all other shapes to be "triangularized", ie turned into a set of triangles
		- include linear interpolation function for some point along a triangle:
			- current thought
							(1, 0, 0)
							|\
							| \
							|  \ (0.5, 0.5, 0)
							|   \
				 (0, 0, 1)  |____\ (0, 1, 0)
				 this vec3 can multiple the 3 colors associated with the vertices:
					ie (color 1, color 2, color 3) * (0.5, 0.5, 0) [3x3 * 3x1] and then add the values in the resulting 3x1 together into a single value
				not sure how to get vec3 yet. will explore later
		- with things triangularized, one step closer to working inside gpu
*/
