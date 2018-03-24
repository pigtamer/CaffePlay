# Play Caffe !
> **用caffe瞎玩的笔记**
## 可视化工具

1. 训练曲线
    用python工具``` tools/extra/plot_train_log.py ```可绘制训练曲线。这个脚本是用 python2 写的，注意在随机Marker那一段要把marker.keys()改成list(...), 因为我们知道返回的字典是不可以索引的。

    在[cat_dog的例子](https://github.com/adilmoujahid/deeplearning-cats-dogs-tutorial)里面有另外一个训练曲线的可视化实现，可供参考。

    (到底 Visualize 什么的还是自己写的看着顺眼吖~)

2. 网络图

    设计出来的网络是什么样的？
    
    利用python工具```python/draw_net.py```可以绘制prototxt描述的网络。也有一些解析prototxt的在线工具。
    ``` bash
    python3 draw_net.py <protxt path> <output image path>
    ```

