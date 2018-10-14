#pragma once
struct input_map
{
	bool w;
	bool a;
	bool s;
	bool d;
	bool cul;
	bool cur;
	bool cud;
	bool cuu;
	bool shift;
	bool alt;
	bool del;
	bool lmb;
	bool rmb;

	float axis[2];
	float cursor[2];

	int Width;
	int Height;

	int sCycle;
};