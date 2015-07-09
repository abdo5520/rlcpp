#ifndef __GRIDWORLD_H__
#define __GRIDWORLD_H__

#include "abstractworld.h"

/**
 * @brief Gridworld in which there is an obstacle and a goal
 */
class GridWorld : public AbstractWorld
{
    public:
        struct Point {
            int x;
            int y;
        };

        enum class Action {
            Up,
            Right,
            Down,
            Left
        };

        GridWorld(unsigned int width,
                  unsigned int height,
                  Point initial,
                  Point obstacle,
                  Point goal,
                  bool stochastic);

        virtual void initialState(std::vector<float> &state);
        virtual void reset();
        virtual void step(unsigned int action,
                          bool &finished,
                          float &reward,
                          std::vector<float> &state);

    private:
        /**
         * @brief Encode the current position into a 2-variables (x, y) state
         */
        void encodeState(const Point &point, std::vector<float> &state);

    private:
        unsigned int _width;
        unsigned int _height;
        Point _initial;
        Point _obstacle;
        Point _goal;
        Point _current_pos;

        bool _stochastic;
};

#endif