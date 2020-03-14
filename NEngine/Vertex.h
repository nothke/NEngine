#pragma once
struct Vertex
{
	float posx, posy, posz;
	float colr, colg, colb;

	static const int STRIDE = 3 * sizeof(float) + 3 * sizeof(float);
	static const int OFFSET_POSITION = 0;
	static const int OFFSET_COLOR = 3 * sizeof(float);
};