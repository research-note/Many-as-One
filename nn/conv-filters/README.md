# [Convolutional filter(s) for images](https://github.com/research-note/conv-filters)

* Images are stored in folder images/. All images should be of the same size.

* Python Imaging Library (PIL) was used to convert images into RGB matrices and to convert filtered matrices back to images. PIL was preferred over other C++ libraries 
  due to ease of use.

## Install

Install g++ libtbb latest version.

```bash
 $ sudo apt -y update && sudo apt -y install g++-10 libtbb-dev
```

Install python libraries.

```bash
 $ pip3 install numpy image matplotlib
```

## Code structure
  * filter.hpp defines a 3-D convolutional kernel class with a bias term. It contains some helper functions to allocate memory to tensors and to normalize them.
  * conv2d\_layer.hpp defines a convolutional layer. One can set the stride and zero-padding of the filter in this. Also, dimensions of the output layer are calculated
    automatically.
  * conv2d method takes as argument a 3-D data volume and a list of filters (one filter generates one activation map). For example, applying a 3 x 3 x 3 filter on a 512 x 512 x 3 image (with 1 zero padding and 1 stride) will generate an 2-D output layer of 512 x 512. See example (taken from course [cs231n](http://cs231n.stanford.edu/syllabus.html)).
  ![One filter](./images/one_map.png)
  * List of filters would make the output layer. Shape of output layer as well as the data block is returned by the function conv2d.
  ![Many filter](./images/multi_map.png)
  * main.cpp runs some example filters on a batch of 3 images. It generates 3 filters, one as an edge detector for each color channel (see push\_filter). Then defines
    a convolution layer with given params and applies the layer to each of the images. It then writes the output to a different file.
  * make\_mats.py and load\_img.py are used to generate images\-matrices and vice versa.

```cpp
for (int id = 0; id < num_images; ++id) {
  ...
  auto output = clayer.conv2d(input, filters);
  ...
}
```

## Steps to run
* Compile using make. Build file for convolutional filter demo program is 'main'. See run.sh for a complete run

```bash
bash run.sh
```

or

```bash
bash bazel.sh
```

## Compile each step

```bazel
bazel build --config=clang_config //src:main
```

or direct compile.

```bash
rm *.o main
g++-10  -std=gnu++20 -O3 filter.hpp -o filter.o
g++-10  -std=gnu++20 -O3 conv2d_layer.hpp -o conv2d_layer.o
g++-10  -std=gnu++20 -O3 NDGrid.hpp -o NDGrid.o
g++-10  -std=gnu++20 -O3 main.cpp -o main
```

* List of images to use is in file make\_mats.py. In the demo it uses a batch of 3  512 \* 512 \* 3 (color) images.

```bash
python make_mats.py img_mats/out.dat
```

* Run the convolutional filter (read from standard input)

```bash
./main img_mats/out.dat img_mats/filter_out.dat < filter.txt
```

* Make output images from matrices

```bash
python load_img.py img_mats/filter_out.dat out_mats
```

## Results
Images taken from [USC Viterbi image dataset](http://sipi.usc.edu/database/database.php?volume=misc)

Input images               |  Output images
:-------------------------:|:-------------------------:
![](./images/color/airplane.png) | ![](./images/filter/0.bmp)
![](./images/color/fruits.png) | ![](./images/filter/1.bmp)
![](./images/color/lena.png) | ![](./images/filter/2.bmp)

## RGB to Greyscale

* PAL, NTSC : [Y'UV](https://en.wikipedia.org/wiki/YUV), [Y'IQ](https://en.wikipedia.org/wiki/YIQ)

```
Y = 0.299 * R + 0.587 * G + 0.114 * B
```

* HDTV : [IUT-R BT.709](https://en.wikipedia.org/wiki/Rec._709)

```
Y = 0.2126 * R + 0.7152 * G + 0.0722 * B
```

## References

- [convert-rgb-to-grayscale](https://blog.leocat.kr/notes/2016/01/12/convert-rgb-to-grayscale)