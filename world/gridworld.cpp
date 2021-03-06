/*
 * Copyright (c) 2015 Vrije Universiteit Brussel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "gridworld.h"

#include <cstdlib>

GridWorld::GridWorld(unsigned int width,
                     unsigned int height,
                     GridWorld::Point initial,
                     GridWorld::Point obstacle,
                     GridWorld::Point goal,
                     bool stochastic)
: AbstractWorld(4),
  _width(width),
  _height(height),
  _initial(initial),
  _obstacle(obstacle),
  _goal(goal),
  _current_pos(initial),
  _stochastic(stochastic)
{
}

void GridWorld::initialState(std::vector<float> &state)
{
    encodeState(_initial, state);
}

void GridWorld::reset()
{
    if (_stochastic) {
        // For the next episode, the initial position is moved
        _initial.x = std::rand() % _width;
        _initial.y = std::rand() % _height;
    }

    _current_pos = _initial;
}

void GridWorld::step(unsigned int action,
                     bool &finished,
                     float &reward,
                     std::vector<float> &state)
{
    Point pos = _current_pos;

    // Perform the action
    switch ((Action)action) {
        case Action::Up:
            pos.y += 1;
            break;

        case Action::Down:
            pos.y -= 1;
            break;

        case Action::Left:
            pos.x -= 1;
            break;

        case Action::Right:
            pos.x += 1;
            break;
    }

    // Commit the action if it is compatible with the world (the agent does not
    // hit walls for instance)
    if (pos.x == _goal.x && pos.y == _goal.y) {
        // Goal reached
        _current_pos = pos;

        finished = true;
        reward = 10.0f;
    } else if (pos.x < 0 || pos.y < 0 || pos.x >= _width || pos.y >= _height || (pos.x == _obstacle.x && pos.y == _obstacle.y)) {
        // Wall or obstacle
        finished = false;
        reward = -10.0f;
    } else {
        // Other move
        _current_pos = pos;

        finished = false;
        reward = -1.0;
    }

    encodeState(_current_pos, state);
}

void GridWorld::encodeState(const GridWorld::Point &point, std::vector<float> &state)
{
    state.resize(2);

    state[0] = float(point.x);
    state[1] = float(point.y);
}
