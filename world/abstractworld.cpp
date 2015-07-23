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

#include "abstractworld.h"

#include <learning/abstractlearning.h>
#include <model/abstractmodel.h>
#include <model/episode.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

AbstractWorld::AbstractWorld(unsigned int num_actions)
: _num_actions(num_actions)
{
}

unsigned int AbstractWorld::numActions() const
{
    return _num_actions;
}

std::vector<Episode *> AbstractWorld::run(AbstractModel *model,
                                          AbstractLearning *learning,
                                          unsigned int num_episodes,
                                          unsigned int max_episode_length,
                                          unsigned int batch_size)
{
    std::vector<Episode *> episodes;
    std::vector<Episode *> learn_episodes;
    std::vector<float> state;
    std::vector<float> values;

    for (unsigned int e=0; e<num_episodes; ++e) {
        Episode *episode = new Episode(_num_actions);

        // Initial state
        reset();
        initialState(state);
        updateMinMax(state);
        episode->addState(state);

        // Initial value
        model->values(episode, values);
        episode->addValues(values);

        // Perform the steps
        unsigned int steps = 0;
        bool finished = false;
        float reward;

        while (steps < max_episode_length && !finished) {
            learning->actions(episode, values);

            // Choose an action according to the probabilities
            float rnd = float(std::rand() % 65536) / 65536.0f;
            float acc = 0.0f;
            unsigned int action;

            for (action = 0; action < episode->valueSize() - 1; ++action) {
                acc += values[action];

                if (acc > rnd) {
                    break;
                }
            }

            // Carry out the action
            step(action, finished, reward, state);
            updateMinMax(state);

            episode->addAction(action);
            episode->addReward(reward);
            episode->addState(state);

            model->values(episode, values);
            episode->addValues(values);

            steps++;
        }

        // Let the learning update the values of the last state that has been visited
        learning->actions(episode, values);

        // If a batch has been finished, update the model
        episodes.push_back(episode);
        learn_episodes.push_back(episode);

        std::cout << "[Episode " << e << "] " << episode->cumulativeReward() << std::endl;

        if (learn_episodes.size() == batch_size) {
            std::cout << "Learning..." << std::flush;

            model->learn(learn_episodes);
            learn_episodes.clear();

            std::cout << "done" << std::endl;
        }
    }

    return episodes;
}

void AbstractWorld::plotModel(AbstractModel *model)
{
    if (_min_state.size() > 2) {
        std::cout << "Cannot plot models with more than 2 state variables" << std::endl;
        return;
    }

    // Define some variables that allow to handle 1D and 2D worlds in a generic way
    bool onedimension = _min_state.size() == 1;
    float min_x = _min_state[0];
    float max_x = _max_state[0];
    float dx = (max_x - min_x) * 0.01f;
    float min_y = onedimension ? 0.0f : _min_state[1];
    float max_y = onedimension ? 1.0f : _max_state[1];
    float dy = onedimension ? 1.0f : (max_y - min_y) * 0.01f;

    // Open the different output files, one for each action
    std::vector<std::ofstream *> streams;
    char filename[] = "model_0.dat";

    for (unsigned int action=0; action<_num_actions; ++action) {
        filename[6] = '0' + char(action);

        streams.push_back(new std::ofstream(filename));
    }

    // Sample the model at regular points
    std::vector<float> state(_min_state.size());
    std::vector<float> values;

    for (float y = min_y; y < max_y; y += dy) {
        for (float x = min_x; x < max_x; x += dx) {
            // Make a dummy episode used
            Episode episode(numActions());

            state[0] = x;

            if (!onedimension) {
                state[1] = y;
            }

            episode.addState(state);

            // Query the values from the model
            model->values(&episode, values);

            // And print them in the output streams
            for (unsigned int action=0; action<_num_actions; ++action) {
                std::ofstream &stream = *streams[action];

                stream << x;

                if (!onedimension) {
                    stream << ' ' << y;
                }

                stream << ' ' << values[action] << '\n';
            }
        }

        // Gnuplot requires that rows are separated by a blank line
        for (auto stream : streams) {
            *stream << std::endl;
        }
    }

    // Close and delete the streams
    for (auto stream : streams) {
        stream->close();
        delete stream;
    }
}

void AbstractWorld::updateMinMax(const std::vector<float> &state)
{
    if (_min_state.size() == 0) {
        // First time the state is encountered, it is a minimum and a maximum at
        // the same time
        _min_state = state;
        _max_state = state;
    } else {
        for (std::size_t i=0; i<state.size(); ++i) {
            _min_state[i] = std::min(_min_state[i], state[i]);
            _max_state[i] = std::max(_max_state[i], state[i]);
        }
    }
}

