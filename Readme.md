# Caffe Config

> **再一次经过了艰苦的Caffe GPU配置过程（笑）**



[TOC]



## 1. 编译依赖

看[官网](http://caffe.berkeleyvision.org/install_apt.html) 和 [Gist](https://gist.github.com/arundasan91/b432cb011d1c45b65222d0fac5f9232c)：

```bash
sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler libatlas-base-dev
sudo apt-get install --no-install-recommends libboost-all-dev
sudo apt-get install libgflags-dev libgoogle-glog-dev liblmdb-dev
```



## 2. Config文件

Makefile.config for caffe cpu version is attached in appendix.

成功编译CPU版本的Config文件放在附录。


## 3. Make过程
> cmake & gnu-make 关系： cmake可以为gnumake生成需要的文件。因此，修改Makefile.config对cmake的配置无济于事。如果需要使用cmake+make的模式，需要在CMakeList.txt中修改各个开关值。
Make 过程中出现的一些Error已经在之前config中说明。

### 3.1. DIRECTLY MAKE！！ (BEST IDEA)

```bash
# in <caffe>
# edit Makefile.config
make all -j8 && make test -j8 && make runtest -j8
make pycaffe -j8
make distribute -j8
# not tested yet
sudo cp build/lib/libcaffe.so* /usr/lib
sudo cp -r python/caffe/ /usr/local/lib/python3.6/dist-packages/
```

### 3.2. CMake + make

Here's bash script for cmake+make process:

**注意： CMake + make 只有在修改\<cafferoot\>/CMakeLists.txt之后才能打开相应开关！！Config文件对CMake不起作用！！**

```bash
# edit CMakeList.txt
cd <caffe>
mkdir build
cd build
cmake ..
sudo make all -j8
sudo make pycaffe && sudo make distribute
sudo make test
sudo make runtest -j8

sudo make install

# ./build/tools/caffe
sudo ln -s ./tools/caffe /usr/bin/caffe
```

## 4. 安装

编译完成之后把caffe文件夹放到需要的位置。然后做如下操作：

1. **环境变量设置**

   编辑 `~/.bashrc`:

   ```bash
   # caffe configs
   export CAFFE_ROOT=/home/cunyuan/.caffe/
   export PYTHONPATH=/home/cunyuan/.caffe/distribute/python:$PYTHONPATH
   export PYTHONPATH=/home/cunyuan/.caffe/python:$PYTHONPATH
   # cuda
   export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
   export PATH=/usr/local/cuda/bin:$PATH
   ```

2. `bin`文件

   ```bash
   # ./build/tools/caffe
   sudo ln -s ./tools/caffe /usr/bin/caffe
   ```

3. 真的需要这样吗

   ```bash
   ...
   # not tested yet
   sudo cp build/lib/libcaffe.so* /usr/lib
   sudo cp -r python/caffe/ /usr/local/lib/python3.6/dist-packages/
   ```

   ​

## 5. Runtime Bugs && Env configs

### 5.1. Error “cublas_v2.h: No such file or directory"
```bash
locate cublas_v2.h
>> /usr/local/cuda-9.1/targets/x86_64-linux/include/cublas_v2.h
```

如上搜索包含cuda的头文件目录。之后在命令行编译中包含。

### 5.2. Error "caffe/proto/caffe.pb.h: No such file or directory"
```bash
# You need to generate caffe.pb.h manually using protoc as follows.
# In the directory you installed Caffe to
protoc src/caffe/proto/caffe.proto --cpp_out=.
mkdir include/caffe/proto
mv src/caffe/proto/caffe.pb.h include/caffe/proto
```

### 5.3. How to compile your code correctly

Remember:

- Link caffe lib;
- Include cuda and caffe headers.

```bash
g++ *.cpp -o test `pkg-config --libs --cflags opencv` \
-I ~/.caffe/include \
-I /usr/local/cuda-9.1/targets/x86_64-linux/include/ \
-L /home/cunyuan/.caffe/build/lib -lcaffe
```

### 5.4. python import error

```bash
pip3 install python-dateutil --upgrade
```

### 5.5. opencv import error: Undefined reference to cv::foo( )

> [GitHub issue and solution](https://github.com/BVLC/caffe/issues/2348)

Just replace some lines in Makefile.config.

我也不知道这是什么情况. 我的opencv在早前即已配置完成. 原先直接在caffe根目录编译报此错误, 神秘地建立caffe/build文件夹, cd build, 再在其中make就不会报错.

### 5.6. Pycaffe build error

>  ERROR : /usr/bin/ld: cannot find -lboost_python3

```bash
locate libboost_python-py36.so
>> /usr/lib/x86_64-linux-gnu/libboost_python-py36.so
>> /usr/lib/x86_64-linux-gnu/libboost_python-py36.so.1.62.0
```

Then:

```bash
sudo ln -s /usr/lib/x86_64-linux-gnu/libboost_python-py36.so /usr/lib/x86_64-linux-gnu/libboost_python3.so

make pycaffe -j4
```

如果说Nothing to do with make 'pycaffe'，要make clean才行。

```bash
make all -j8 && make test -j8 && make runtest -j8
make pycaffe
make distribute
```



### 5.7. Visualize Caffe Networks

Caffe 自己有可视化网络的工具。

```bash
python3  ./python/draw_net.py \    
./examples/siamese/mnist_siamese.prototxt \    
./examples/siamese/mnist_siamese.png
```

缺什么装什么：

```bash
pip3 install pydotplus && sudo apt install graphviz
```

也可到网上找。基于prototxt的网络可视化工具还是不少的。

##实例运行

### LeNet - MNIST

注意运行test时候，要把命令在caffe根目录运行。因为prototxt 里面有网络数据层输入的config，这个数据位置是用相对路径书写的。

```bash
caffe test \
-model ~/.caffe/examples/mnist/lenet_train_test.prototxt \
-weights ~/.caffe/examples/mnist/lenet_iter_10000.caffemodel \
-iterations 100
```



## 附录

### $.1. Makefile.config (Geforce 940MX GPU)

 ```cmake
## Refer to http://caffe.berkeleyvision.org/installation.html
# Contributions simplifying and improving our build system are welcome!

# cuDNN acceleration switch (uncomment to build with cuDNN).
USE_CUDNN := 1

# CPU-only switch (uncomment to build without GPU support).
# CPU_ONLY := 1

# uncomment to disable IO dependencies and corresponding data layers
# USE_OPENCV := 0
# USE_LEVELDB := 0
# USE_LMDB := 0

# uncomment to allow MDB_NOLOCK when reading LMDB files (only if necessary)
#       You should not set this flag if you will be reading LMDBs with any
#       possibility of simultaneous read and write
# ALLOW_LMDB_NOLOCK := 1

# Uncomment if you're using OpenCV 3
OPENCV_VERSION := 3

# To customize your choice of compiler, uncomment and set the following.
# N.B. the default for Linux is g++ and the default for OSX is clang++
# CUSTOM_CXX := g++

# CUDA directory contains bin/ and lib/ directories that we need.
CUDA_DIR := /usr/local/cuda
# On Ubuntu 14.04, if cuda tools are installed via
# "sudo apt-get install nvidia-cuda-toolkit" then use this instead:
# CUDA_DIR := /usr

# CUDA architecture setting: going with all of them.
# For CUDA < 6.0, comment the *_50 lines for compatibility.
CUDA_ARCH := -gencode arch=compute_30,code=sm_30 \
		-gencode arch=compute_35,code=sm_35 \
		-gencode arch=compute_50,code=sm_50 \
		-gencode arch=compute_52,code=sm_52 \
		-gencode arch=compute_60,code=sm_60 \
		-gencode arch=compute_61,code=sm_61 \
		-gencode arch=compute_61,code=compute_61

# BLAS choice:
# atlas for ATLAS (default)
# mkl for MKL
# open for OpenBlas
BLAS := atlas
# Custom (MKL/ATLAS/OpenBLAS) include and lib directories.
# Leave commented to accept the defaults for your choice of BLAS
# (which should work)!
# BLAS_INCLUDE := /path/to/your/blas
# BLAS_LIB := /path/to/your/blas

# Homebrew puts openblas in a directory that is not on the standard search path
# BLAS_INCLUDE := $(shell brew --prefix openblas)/include
# BLAS_LIB := $(shell brew --prefix openblas)/lib

# This is required only if you will compile the matlab interface.
# MATLAB directory should contain the mex binary in /bin.
MATLAB_DIR := /usr/local/MATLAB/R2017b
# MATLAB_DIR := /Applications/MATLAB_R2012b.app

# NOTE: this is required only if you will compile the python interface.
# We need to be able to find Python.h and numpy/arrayobject.h.
PYTHON_INCLUDE := /usr/include/python2.7 \
                /usr/lib/python2.7/dist-packages/numpy/core/include
# Anaconda Python distribution is quite popular. Include path:
# Verify anaconda location, sometimes it's in root.
#ANACONDA_HOME := $(HOME)/miniconda2/envs/caffe
#PYTHON_INCLUDE := $(ANACONDA_HOME)/include \
#                $(ANACONDA_HOME)/include/python2.7 \
#                $(ANACONDA_HOME)/lib/python2.7/site-packages/numpy/core/include \

# Uncomment to use Python 3 (default is Python 2)
PYTHON_LIBRARIES := boost_python3 python3.6m
PYTHON_INCLUDE := /usr/include/python3.6m \
                 /usr/local/lib/python3.6/dist-packages/numpy/core/include

# We need to be able to find libpythonX.X.so or .dylib.
PYTHON_LIB := /usr/lib
# PYTHON_LIB := $(ANACONDA_HOME)/lib

# Homebrew installs numpy in a non standard path (keg only)
# PYTHON_INCLUDE += $(dir $(shell python -c 'import numpy.core; print(numpy.core.__file__)'))/include
# PYTHON_LIB += $(shell brew --prefix numpy)/lib

# Uncomment to support layers written in Python (will link against Python libs)
WITH_PYTHON_LAYER := 1

# Whatever else you find you need goes here.
INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial/
LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu/hdf5/serial/

# If Homebrew is installed at a non standard location (for example your home directory) and you use it for general dependencies
# INCLUDE_DIRS += $(shell brew --prefix)/include
# LIBRARY_DIRS += $(shell brew --prefix)/lib

# Uncomment to use `pkg-config` to specify OpenCV library paths.
# (Usually not necessary -- OpenCV libraries are normally installed in one of the above $LIBRARY_DIRS.)
# USE_PKG_CONFIG := 1

BUILD_DIR := build
DISTRIBUTE_DIR := distribute

# Uncomment for debugging. Does not work on OSX due to https://github.com/BVLC/caffe/issues/171
# DEBUG := 1

# The ID of the GPU that 'make runtest' will use to run unit tests.
TEST_GPUID := 0

# enable pretty build (comment to see full commands)
Q ?= @

 ```



### $.2. Makefile.config (CPU)

```cmake
## Refer to http://caffe.berkeleyvision.org/installation.html
# Contributions simplifying and improving our build system are welcome!

# cuDNN acceleration switch (uncomment to build with cuDNN).
# USE_CUDNN := 1

# CPU-only switch (uncomment to build without GPU support).
CPU_ONLY := 1

# uncomment to disable IO dependencies and corresponding data layers
# USE_OPENCV := 0
# USE_LEVELDB := 0
# USE_LMDB := 0

# uncomment to allow MDB_NOLOCK when reading LMDB files (only if necessary)
#   You should not set this flag if you will be reading LMDBs with any
#   possibility of simultaneous read and write
# ALLOW_LMDB_NOLOCK := 1

# Uncomment if you're using OpenCV 3
OPENCV_VERSION := 3

# To customize your choice of compiler, uncomment and set the following.
# N.B. the default for Linux is g++ and the default for OSX is clang++
# CUSTOM_CXX := g++

# CUDA directory contains bin/ and lib/ directories that we need.
CUDA_DIR := /usr/local/cuda
# On Ubuntu 14.04, if cuda tools are installed via
# "sudo apt-get install nvidia-cuda-toolkit" then use this instead:
# CUDA_DIR := /usr

# CUDA architecture setting: going with all of them.
# For CUDA < 6.0, comment the *_50 through *_61 lines for compatibility.
# For CUDA < 8.0, comment the *_60 and *_61 lines for compatibility.
CUDA_ARCH := -gencode arch=compute_30,code=sm_30 \
-gencode arch=compute_35,code=sm_35 \
-gencode arch=compute_50,code=sm_50 \
-gencode arch=compute_52,code=sm_52 \
-gencode arch=compute_60,code=sm_60 \
-gencode arch=compute_61,code=sm_61 \
-gencode arch=compute_61,code=compute_61

# BLAS choice:
# atlas for ATLAS (default)
# mkl for MKL
# open for OpenBlas
BLAS := atlas
# Custom (MKL/ATLAS/OpenBLAS) include and lib directories.
# Leave commented to accept the defaults for your choice of BLAS
# (which should work)!
# BLAS_INCLUDE := /path/to/your/blas
# BLAS_LIB := /path/to/your/blas

# Homebrew puts openblas in a directory that is not on the standard search path
# BLAS_INCLUDE := $(shell brew --prefix openblas)/include
# BLAS_LIB := $(shell brew --prefix openblas)/lib

# This is required only if you will compile the matlab interface.
# MATLAB directory should contain the mex binary in /bin.
# MATLAB_DIR := /usr/local
# MATLAB_DIR := /Applications/MATLAB_R2012b.app

# NOTE: this is required only if you will compile the python interface.
# We need to be able to find Python.h and numpy/arrayobject.h.
PYTHON_INCLUDE := /usr/include/python2.7 \
/usr/lib/python2.7/dist-packages/numpy/core/include
# Anaconda Python distribution is quite popular. Include path:
# Verify anaconda location, sometimes it's in root.
# ANACONDA_HOME := $(HOME)/anaconda
# PYTHON_INCLUDE := $(ANACONDA_HOME)/include \
# $(ANACONDA_HOME)/include/python2.7 \
# $(ANACONDA_HOME)/lib/python2.7/site-packages/numpy/core/include

# Uncomment to use Python 3 (default is Python 2)
PYTHON_LIBRARIES := boost_python3 python3.6m
PYTHON_INCLUDE := /usr/include/python3.6m \
                 /usr/lib/python3.6/dist-packages/numpy/core/include

# We need to be able to find libpythonX.X.so or .dylib.
PYTHON_LIB := /usr/lib
# PYTHON_LIB := $(ANACONDA_HOME)/lib

# Homebrew installs numpy in a non standard path (keg only)
# PYTHON_INCLUDE += $(dir $(shell python -c 'import numpy.core; print(numpy.core.__file__)'))/include
# PYTHON_LIB += $(shell brew --prefix numpy)/lib

# Uncomment to support layers written in Python (will link against Python libs)
# WITH_PYTHON_LAYER := 1

# Whatever else you find you need goes here.
INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial/ 
LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu/hdf5/serial/

# If Homebrew is installed at a non standard location (for example your home directory) and you use it for general dependencies
# INCLUDE_DIRS += $(shell brew --prefix)/include
# LIBRARY_DIRS += $(shell brew --prefix)/lib

# NCCL acceleration switch (uncomment to build with NCCL)
# https://github.com/NVIDIA/nccl (last tested version: v1.2.3-1+cuda8.0)
# USE_NCCL := 1

# Uncomment to use `pkg-config` to specify OpenCV library paths.
# (Usually not necessary -- OpenCV libraries are normally installed in one of the above $LIBRARY_DIRS.)
# USE_PKG_CONFIG := 1

# N.B. both build and distribute dirs are cleared on `make clean`
BUILD_DIR := build
DISTRIBUTE_DIR := distribute

# Uncomment for debugging. Does not work on OSX due to https://github.com/BVLC/caffe/issues/171
# DEBUG := 1

# The ID of the GPU that 'make runtest' will use to run unit tests.
TEST_GPUID := 0

# enable pretty build (comment to see full commands)
Q ?= @

```

