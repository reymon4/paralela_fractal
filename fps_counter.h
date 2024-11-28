//
// Created by fing.labcom on 27/11/2024.
//

#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include<chrono>

namespace ch = std::chrono;

class fps_counter
{
private:
    int frames;
    int fps;
    ch::time_point<ch::system_clock> last_time;

public:
    fps_counter();
    void update();
    int get_fps();

};
#endif //FPS_COUNTER_H
