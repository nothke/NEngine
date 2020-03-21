#pragma once
struct Vertex
{
	float posx, posy, posz;
	float uvs, uvt;
	float colr, colg, colb;

	static const int STRIDE = 3 * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float);
	static const int OFFSET_POSITION = 0;
	static const int OFFSET_UV = 3 * sizeof(float);
	static const int OFFSET_COLOR = 5 * sizeof(float);
};