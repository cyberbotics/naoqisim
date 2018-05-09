#ifndef VMWRAPPER_H
#define VMWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef class VisionManager VisionManager;

VisionManager* create_vision_manager(int width, int height, int alpha);
void delete_vision_manager(VisionManager* vm);
int get_ball_center(VisionManager* vm, int &lock, double &x, double &y, const unsigned char *image);
int get_goal_position(VisionManager* vm, int &wrongSide, double &p1, double &p2, const unsigned char *image);
int get_right_foot_position(VisionManager* vm, double &pos, const unsigned char *image);

#ifdef __cplusplus
}
#endif
#endif
