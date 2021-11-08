#ifndef LAYER_H
#define LAYER_H

#include <cassert>
#include <algorithm>
#include <vector>
#include <execution>

#include "filter.hpp"

class conv_layer {
    int in_width, in_height, in_depth,
        n_filters, window, stride, padding,
        out_width, out_height, out_depth;
public:
    conv_layer(int _width,
            int _height,
            int _depth,
            int _window,
            int _stride = 1,
            int _padding = 0, 
            int n_filters = 1)
        : in_width(_width), 
            in_height(_height),
            in_depth(_depth),
            window(_window),
            stride(_stride),
            padding(_padding),
            n_filters(n_filters) {
        // in_width + 2*padding - window should be divisible by stride
        out_width = (in_width + 2 * padding - window) / stride + 1;
        out_height = (in_height + 2 * padding - window) / stride + 1;
        out_depth = n_filters;
    }

    int getWidth() {
        return out_width;
    }

    int getHeight() {
        return out_height;
    }

    int getDepth() {
        return out_depth;
    }

    virtual ~conv_layer() = default;

    /**
     * Applies convolutional filters in filters to input volume x
     * x treated as in_width * in_height * in_depth
     * assumed n_filters elements in filters vector
     * returns shape of the output volume and pointer to the memory block
     */ 
    tensor
    conv2d(tensor x, /*std::vector<filter> filters*/ const std::vector<filter*> &filters) {
        tensor y(out_width, matrix(out_height, v(out_depth)));

        for (int i = 0, i_start = 0, i_end = 0, i_max = 0; i < out_width; ++i) {
            i_start = -padding + i * stride;
            i_end = i_start + window;
            i_max = std::min(in_width, i_end);
            for (int j = 0, j_start = 0, j_end = 0, j_max = 0; j < out_height; ++j) {
                j_start = -padding + j * stride;
                j_end = j_start + window;
                j_max = std::min(in_height, j_end);
                for (int k = 0; k < n_filters; ++k) { // k_th activation map
                    
                    for (int i_pt = std::max(0, i_start); i_pt < i_max; ++i_pt) {
                        for (int j_pt = std::max(0, j_start); j_pt < j_max; ++j_pt) {
                            for (int k_pt = 0; k_pt < in_depth; ++k_pt) {
                                /**
                                 * fill y[i][j] with kernel computation filters[k]->x + b
                                 * compute boundaries inside original matrix after padding
                                 */
                                y[i][j][k] += x[i_pt][j_pt][k_pt] * filters[k]->w[i_pt - i_start][j_pt - j_start][k_pt];
                            }
                        }
                    }

                    y[i][j][k] += filters[k]->b;  // bias term
                }
            }
        }

        return y;
    }

};

#endif // LAYER_H
