#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <array>
#include <numeric>
#include <execution>
#include <chrono>

#include "filter.hpp"
#include "conv2d_layer.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    int w_size, bias; 
    cin >> w_size >> bias;
    /**
     * input the image kernel matrix
     */ 
    matrix kernel(w_size, v(w_size));
    trans_matrix([](auto v) -> auto {
                cin >> v;
                return v;
            }, kernel);

    vector<filter*> filters;
    array<unsigned int, 3> rgb = {0, 1, 2};
    for_each(std::execution::par, rgb.begin(), rgb.end(),
        [w_size, bias, kernel, &filters](auto idx) {
            // make edge detector for color idx
            tensor ed(w_size, matrix(w_size, v(3)));

            for (int i = 0; i < w_size; ++i) {
                for (int j = 0; j < w_size; ++j) {
                    ed[i][j][idx] = kernel[i][j];
                }
            }
            filter *f = new filter(ed, w_size, 3, bias);
            f->normalize();
            filters.push_back(f);
        });

    if (argc < 3) {
        cerr << "usage <input_data file name> <output file name>" << endl;
        return 1;
    }
    // output file will be written in the same format as input file
    ifstream ifile (argv[1]);   
    ofstream ofile (argv[2]);
    if (!ofile.is_open()) {
        cerr << "Unable to open " << argv[1] << endl;
        return 1;
    }
    if (!ifile.is_open()) {
        cerr << "Unable to open " << argv[0] << endl;
        return 1;
    }

    int width, num_images, height, depth;
    int stride = 1, padding = 1; 
    ifile >> num_images >> width >> height >> depth;
    ofile << num_images << " "; 
    cerr << num_images << " "; 

    /* vector<tensor> inputs(num_images);
    std::fill(v.begin(), v.end(), vector<tensor>(width, matrix(height, v(depth))); */
    vector<tensor> inputs (num_images);
    fill(inputs.begin(), inputs.end(), tensor(width, matrix(height, v(depth))));
    // tensor input (width, matrix(height, v(depth)));
    /* conv_layer clayer(width, height, depth, w_size, stride, 
                                padding, filters.size()); */
    vector<conv_layer> clayers;
    for_each(inputs.begin(), inputs.end(), [&] (auto i) {
        clayers.push_back(conv_layer (width, height, depth, w_size, stride, padding, filters.size()));
    });

    auto clayer = clayers[0];
    auto o_width = clayer.getWidth(), o_height = clayer.getHeight(), o_depth = clayer.getDepth();
    // print image dimensions only the first time
    ofile << o_width << " " << o_height << " " << o_depth << "\n";
    cerr << o_width << " " << o_height << " " << o_depth << "\n";

    // read each images in sequential
    for_each(inputs.begin(), inputs.end(), [&ifile](auto in) {
        for_each(in.begin(), in.end(), [&ifile](auto &m) {
            for_each(m.begin(), m.end(), [&ifile](auto &v) {
                for_each(v.begin(), v.end(), [&ifile](auto &e){
                    ifile >> e;
                    if (ifile.peek() == ',')
                        ifile.ignore();
                });
            });
        });
    });

    vector<tensor> outs (num_images);
    vector<int> idxs (num_images);
    std::iota(idxs.begin(), idxs.end(), 0);
    for_each(std::execution::par_unseq,
        idxs.begin(), idxs.end(), [&filters, &inputs, &clayers, &outs](auto i) {
        auto start = std::chrono::steady_clock::now();
        outs[i] = clayers[i].conv2d(inputs[i], filters);
        auto elapsed_time = chrono::duration_cast<std::chrono::nanoseconds> (
            std::chrono::steady_clock::now() - start
        );
        cout << "Conv filter: " << elapsed_time.count() << "ns" << endl;
    });

    for_each(outs.begin(), outs.end(), [&ofile](auto out) {
        for_each(out.begin(), out.end(), [&ofile](auto m) {
            for_each(m.begin(), m.end(), [&ofile](auto v) {
                ofile << v[0] << "," << v[1] << "," << v[2] << " ";
            });
            ofile << "\n";
        });
        ofile << "\n";
   });

    ifile.close();
    ofile.close();

    return 0; 
}
