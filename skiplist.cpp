#include <iostream>
#include <cstring>
#include <ctime>
#include <climits>

using namespace std;

// 定义结构体Less，用于比较两个元素的大小
template<typename T>
struct Less {
    bool operator () (const T & a , const T & b) const {
        return a < b;
    }
};

// 定义跳表的结构体
template<typename K, typename V,typename Comp = Less<K>>
class skip_list {
private:
    // 定义跳表的节点结构体
    struct skip_list_node {
        int level; // 节点的级别
        const K key; // 节点的key值
        V value; // 节点的value值
        skip_list_node** forward; // 向前指针数组

        skip_list_node() :key{ 0 }, value{ 0 }, level{ 0 }, forward{0} {}

        skip_list_node(K k, V v, int l, skip_list_node* nxt = nullptr) :key(k), value(v), level(l) {
            forward = new skip_list_node * [level + 1];
            for (int i = 0; i <= level; ++i) forward[i] = nxt;
        }

        ~skip_list_node() { delete[] forward; }
    };

    using node = skip_list_node; // 使用别名node

    // 初始化跳表
    void init() {
        srand((uint32_t)time(NULL));
        level = length = 0;
        head->forward = new node * [MAXL + 1];
        for (int i = 0; i <= MAXL; i++)
            head->forward[i] = tail;
    }

    // 随机生成节点的级别
    int randomLevel() {
        int lv = 1;
		while ((rand() / (RAND_MAX + 1.0) < P) && level < MAXL) lv++;
        return lv;
    }

    // 跳表相关参数
    int level;
    int length;
    int MAXL = 32; // 跳表的最大级别
    const int P = 0.5; // 随机层级的概率 一般为0.5或0.25
    const int INVALID = INT_MAX; // 删除节点的标志

    node* head, * tail; // 跳表的头节点和尾节点
    Comp less; // 比较函数


    // 查找节点
    /*
        @brief 在二叉搜索树中查找指定键值的节点
        @param key 要查找的键值
        @param update 用于更新节点的指针
        @return 返回找到的节点指针
    */
    node* find(const K& key, node** update) {
        node* p = head;  // 当前节点指针初始化为头节点指针
        for (int i = level; i >= 0; i--) {  // 从最高层级开始循环，逐级向下查找节点
            while (p->forward[i] != tail && less(p->forward[i]->key, key)) {
                p = p->forward[i];  // 当前节点的后续节点的键值小于目标键值，将当前节点指针更新为后续节点指针
            }
            update[i] = p;  // 更新当前层级的指针
        }
        p = p->forward[0];  // 找到最终目标节点，将当前节点指针指向目标节点
        return p;  // 返回找到的节点指针
    }



public:
    // 跳表的迭代器结构体
    struct Iter {
        node* p;

        Iter() : p(nullptr) {};
        Iter(node* rhs) : p(rhs) {}

        node* operator ->()const { return p;}
        node& operator *() const { return *p;}

        bool operator == (const Iter& rhs) { return rhs.p == p;}
        bool operator != (const Iter& rhs) {return !(rhs.p == p);}

        // 自增迭代器
        void operator ++() {p = p->forward[0];} // ++iterator
        void operator ++(int) { p = p->forward[0]; }// iterator++
    };



    // 构造函数
    skip_list() : head(new node()), tail(new node()), less{Comp()} {
        init();    
    }

    skip_list(Comp _less) : head(new node()), tail(new node()), less{_less} {
        init();
    }

    // 向跳表中插入节点
    void insert(const K& key, const V& value) {
        node * update[MAXL + 1]; // 定义用于更新节点的数组

        node* p = find(key,update); // 查找跳表里是否有已有这个key
        // 如果跳表里面已存在这个key，则更新这个key的value
        if (p->key == key) {
            p->value = value; 
            return;
        }

        // 随机选择一个级别
        int lv = randomLevel(); 

        // 如果选择的级别大于当前的级别
        if (lv > level) {
            lv = ++level; // 将选择的级别设为当前级别
            update[lv] = head; // 更新节点的数组
        }

        node * newNode = new node(key, value, lv); // 创建一个新的节点
        // 遍历每个级别，更新节点的前向指针
        for (int i = lv; i >= 0; --i) {
            p = update[i];
            newNode->forward[i] = p->forward[i];
            p->forward[i] = newNode;
        }

        ++length; // 哈希表长度加一
    }

    // 删除跳表中的节点
    bool erase(const K& key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p->key != key)return false;
        for (int i = 0; i <= p->level; ++i) {
            update[i]->forward[i] = p->forward[i];
        }
        delete p;
        while (level > 0 && head->forward[level] == tail) --level;
        --length;
        return true;
    }

    // 查找节点
    Iter find(const K&key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p == tail)return tail;
        if (p->key != key)return tail;
        return Iter(p);
    }

    // 判断节点是否存在
    bool count(const K& key) {
        auto p = find(key);
        if (p == end())return false;
        return key == p->key;
    }

    // 获取跳表的结束迭代器
    Iter end() {
        return Iter(tail);
    }

    // 获取跳表的开始迭代器
    Iter begin() {
        return Iter(head->forward[0]);
    }

    // 长度
    int size() {
        return length;
    }


    // at访value
    V at(const K& key) {
        auto it = find(key);
        if (it == end()) {
            cout << "not found" << endl;
            return INVALID;
        }
        else return it->value;
    }
    // []访问value
    V operator [] (int num) {
        if (num >= length) {
            cout << "out of range" << endl;
            throw std::out_of_range("out of range");
            return INVALID;
        } else {
            auto it = begin();
            for (int i = 0; i < num; ++i) {
                ++it;
            }
            return it->value;
        }
    }

    // 判断是否为空
    bool empty() {
        return length == 0;
    }

    // 清空跳表
    void clear() {
        while (!empty()) {
            erase(begin()->key);
        }
    }

    // 交换两个跳表的全部元素
    void swap(skip_list& rhs) {
        std::swap(head, rhs.head);
        std::swap(tail, rhs.tail);
        std::swap(level, rhs.level);
        std::swap(length, rhs.length);
        std::swap(less, rhs.less);
    }



    // 测试
    // void show() {
    //     node *temp = head;
    //     cout << "key1: " << temp->forward[0]->key << " --- value1: " << temp->forward[0]->value << endl;
    //     cout << "key1: " << temp->forward[1]->key << " --- value1: " << temp->forward[1]->value << endl;
    //     temp = temp->forward[0];  
    //     cout << "key2: " << temp->forward[0]->key << " --- value2: " << temp->forward[0]->value << endl;
    //     cout << "key2: " << temp->forward[1]->key << " --- value2: " << temp->forward[1]->value << endl;
    //     temp = temp->forward[0];
    //     cout << "key3: " << temp->forward[0]->key << " --- value3: " << temp->forward[0]->value << endl;
    //     cout << "key3: " << temp->forward[1]->key << " --- value3: " << temp->forward[1]->value << endl;
    //     cout << head->forward[0]->key << "  " << head->forward[0]->value << endl;
    //     // cout << tail->forward[0]->key << "  " << tail->forward[0]->value << endl;
    // }

};

int main()
{
    // {
    //     // lambda表达式自定义比较类型
    //     auto cmp = [](const string& a, const string& b) {return a.length() < b.length(); };
    //     skip_list < string, int, decltype(cmp)> list(cmp);
    //     //skip_list<string, int> list;
    //     list.insert("aab", 1321);
    //     list.insert("hello", 54342);
    //     list.insert("world", 544);
    //     list.show();

    //     for (auto it = list.begin(); it != list.end(); it++) {
    //         cout << it->key << " " << it->value << endl;
    //     }


    // }

    // cout << "==================================" << endl;
    
    // {
    //     // 仿函数自定义比较类型
    //     struct cmp {
    //         bool operator()(int a, int b) {
    //             return a > b;
    //         }
    //     };
    //     skip_list < int, int, cmp> list{};
    //     //skip_list<int, int> list;
    //     for (int i = 0; i <= 10; i++) list.insert(10 + 10 * i, 10 + 10 * i);
    //     list.show();
    //     for (auto it = list.begin(); it != list.end(); it++) cout << it->key << " " << it->value << endl;
    // }

    // cout << "==================================" << endl;

    {
        skip_list<int, int>list;
        list.insert(1, 6);
        list.insert(2, 5);
        list.insert(3, 4);
        list.insert(4, 3);
        list.insert(5, 2);
        list.insert(6, 1);
        // list.show();
        cout << "size: " << list.size() << endl;
        cout << "at(1): " << list.at(1) << endl;
        cout << "[]: " << list[1] << endl;
        cout << "empty: " << list.empty() << endl;
        cout << "count: " << list.count(7) << endl;
        // list.clear();

        for (auto it = list.begin(); it != list.end();it++) {
            cout << it->key << " " << it->value << endl;
        }
    }


    
}
