#include "VisionManager.hpp"
#include "vm_wrapper.h"

extern "C" {
  VisionManager* create_vision_manager(int width, int height, int alpha) {
    return new VisionManager(width, height, alpha);
  }

  void delete_vision_manager(VisionManager* vm) {
    delete vm;
  }

  int get_ball_center(VisionManager* vm, int &lock, double &x, double &y, const unsigned char *image) {
    bool wrappedLock = lock;
    int ret = vm->getBallCenter(wrappedLock, x, y, image);
    lock = wrappedLock;
    return ret;
  }

  int get_goal_position(VisionManager* vm, int &wrongSide, double &p1, double &p2, const unsigned char *image) {
    bool wrappedWrongSide = wrongSide;
    int ret = vm->getGoalPosition(wrappedWrongSide, p1, p2, image);
    wrongSide = wrappedWrongSide;
    return ret;
  }

  int get_right_foot_position(VisionManager* vm, double &pos, const unsigned char *image) {
    return vm->getRFootPosition(pos, image);
  }
}
