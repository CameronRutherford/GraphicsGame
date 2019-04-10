#ifndef __GAME_MOUSE_H__
#define __GAME_MOUSE_H__

struct mouse_globals {
	float locked_mouse = true;
	int accum_dy = 0;
	int width = 0;
	int height = 0;
} mouse_global;

void mouse_moving(int x, int y) {

	if (mouse_global.locked_mouse) {
		int center_x = int(mouse_global.width / 2);
		int center_y = int(mouse_global.height / 2);

		int dx = x - center_x;
		int dy = y - center_y;

		mouse_global.accum_dy += dy;

		view = RotateY(-dx * 0.1) * view;
		move_back_or_forward = RotateY(-dx * 0.1) * move_back_or_forward;
		move_left_or_right = RotateY(-dx * 0.1) * move_left_or_right;

		at = eye + view;
		at.y -= mouse_global.accum_dy * 0.003;

		glutWarpPointer(center_x, center_y);
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
}


#endif