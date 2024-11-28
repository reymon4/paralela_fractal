//
// Created by fing.labcom on 27/11/2024.
//

#include "fps_counter.h"

#include <fmt/core.h>

fps_counter::fps_counter()
{
    frames = 0;
    fps = 0;
    last_time = ch::system_clock::now();
}

int fps_counter::get_fps()
{
    return this->fps;
}

void fps_counter::update()
{
    frames++;
    auto current_time = ch::system_clock::now();

    /*TEMPLATES == CÓMO GENÉRICOS EN JAVA*/
    ch::duration<double, std::milli> tiempo = current_time - last_time;

    if (tiempo.count() > 1000)
    {
        fps = frames;
        frames = 0;
        last_time = current_time;

        fmt::print("FPS: {}\n", fps);
    }
}
