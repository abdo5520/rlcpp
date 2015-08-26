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

#ifndef __PERCEPTRONMODEL_H__
#define __PERCEPTRONMODEL_H__

#include "nnetmodel.h"

#include <nnetcpp/activation.h>

/**
 * @brief Feed-forward neural network with a single hidden layer
 */
class PerceptronModel : public NnetModel
{
    public:
        /**
         * @brief Constructor.
         *
         * @param hidden_neurons Number of neurons in the hidden layers
         * @param mask_actions Only update values associated with actions that have
         *                     been taken (if true). If false, all the values in
         *                     the episodes are used for learning.
         */
        PerceptronModel(unsigned int hidden_neurons, bool mask_actions);

        virtual Network *createNetwork(Episode *first_episode) const;
        virtual void hiddenValues(Episode *episode, std::vector<float> &rs);

    private:
        TanhActivation *_hidden_activation;
        unsigned int _hidden_neurons;
};

#endif
