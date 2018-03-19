g++ *.cpp -o test `pkg-config --libs --cflags opencv` \
-I ~/.caffe/include \
-I /usr/local/cuda-9.1/targets/x86_64-linux/include/ \
-L /home/cunyuan/.caffe/build/lib -lcaffe