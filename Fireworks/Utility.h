#pragma once

inline float movevar(float var, float to, float dt) {
	if (var == to) { return(var); }
	float a;
	a = var - to;
	if (a > 0) {
		var -= dt;
		if (var - to < 0) { var = to; return(var); }
	}
	else {
		if (a < 0) {
			var += dt;
			if (var - to > 0) { var = to; return(var); }
		}
	}
	return(var);
}