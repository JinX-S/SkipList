基于跳表实现的轻量级KV存储

简介：
  采用skiplist作为底层数据结构，支持插入，删除，查询等常见操作。
  使用C++模板编程，使用类似STL，支持自定义类型与自定义比较函数（可以传入lambda与仿函数），迭代器遍历,此跳表需有键和值。

详细：
  此跳表默认为顺序排列，也可自己编写labda表达式或仿函数作为排序条件。
  在此跳表中，内置了插入函数：void insert(const K& key, const V& value);
  删除函数：void erase(const K& key);
  查找函数：Iter find(const K&key);
  查找键是否存在：bool count(const K& key);
  跳表长度：int size();
  跳表是否为空：bool empty();
  清空跳表：void clear();
  交换两个跳表:void swap(skip_list& rhs);
  跳表的首尾迭代器：Iter end()； Iter begin()
  按键访问值；V at(const K& key)；
  []访问值：V operator [] (int num)；
