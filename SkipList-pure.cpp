#include <iostream>
#include <cstring>
#include <ctime>
#include <climits>

using namespace std;

template<typename T>
struct Less {
    bool operator () (const T & a , const T & b) const {
        return a < b;
    }
};

template<typename K, typename V,typename Comp = Less<K>>
class skip_list {
private:
    struct skip_list_node {
        int level;
        const K key;
        V value;
        skip_list_node** forward;

        skip_list_node() :key{ 0 }, value{ 0 }, level{ 0 }, forward{0} {}

        skip_list_node(K k, V v, int l, skip_list_node* nxt = nullptr) :key(k), value(v), level(l) {
            forward = new skip_list_node * [level + 1];
            for (int i = 0; i <= level; ++i) forward[i] = nxt;
        }

        ~skip_list_node() { delete[] forward; }
    };

    using node = skip_list_node;

    void init() {
        srand((uint32_t)time(NULL));
        level = length = 0;
        head->forward = new node * [MAXL + 1];
        for (int i = 0; i <= MAXL; i++)
            head->forward[i] = tail;
    }

    int randomLevel() {
        int lv = 1;
		while ((rand() / (RAND_MAX + 1.0) < P) && level < MAXL) lv++;
        return lv;
    }

    int level;
    int length;
    int MAXL = 32;
    const int P = 0.5;
    const int INVALID = INT_MAX;

    node* head, * tail;
    Comp less;

    node* find(const K& key, node** update) {
        node* p = head;
        for (int i = level; i >= 0; i--) {
            while (p->forward[i] != tail && less(p->forward[i]->key, key)) {
                p = p->forward[i];
            }
            update[i] = p;
        }
        p = p->forward[0];
        return p;
    }

public:
    struct Iter {
        node* p;

        Iter() : p(nullptr) {};
        Iter(node* rhs) : p(rhs) {}

        node* operator ->()const { return p;}
        node& operator *() const { return *p;}

        bool operator == (const Iter& rhs) { return rhs.p == p;}
        bool operator != (const Iter& rhs) {return !(rhs.p == p);}

        void operator ++() {p = p->forward[0];} // ++iterator
        void operator ++(int) { p = p->forward[0]; }// iterator++
    };

    skip_list() : head(new node()), tail(new node()), less{Comp()} {
        init();    
    }

    skip_list(Comp _less) : head(new node()), tail(new node()), less{_less} {
        init();
    }

    void insert(const K& key, const V& value) {
        node * update[MAXL + 1];
        node* p = find(key,update);
        if (p->key == key) {
            p->value = value; 
            return;
        }

        int lv = randomLevel(); 

        if (lv > level) {
            lv = ++level;
            update[lv] = head;
        }

        node * newNode = new node(key, value, lv);
        for (int i = lv; i >= 0; --i) {
            p = update[i];
            newNode->forward[i] = p->forward[i];
            p->forward[i] = newNode;
        }
        ++length;
    }

    void erase(const K& key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p->key != key)return;
        for (int i = 0; i <= p->level; ++i) update[i]->forward[i] = p->forward[i];
        delete p;
        while (level > 0 && head->forward[level] == tail) --level;
        --length;
    }

    Iter find(const K&key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p == tail)return tail;
        if (p->key != key)return tail;
        return Iter(p);
    }

    bool count(const K& key) {
        auto p = find(key);
        if (p == end())return false;
        return key == p->key;
    }

    Iter end() {
        return Iter(tail);
    }

    Iter begin() {
        return Iter(head->forward[0]);
    }

    int size() {
        return length;
    }

    V at(const K& key) {
        auto it = find(key);
        if (it == end()) {
            cout << "not found" << endl;
            return INVALID;
        }
        else return it->value;
    }

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

    bool empty() {
        return length == 0;
    }

    void clear() {
        while (!empty()) {
            erase(begin()->key);
        }
    }

    void swap(skip_list& rhs) {
        std::swap(head, rhs.head);
        std::swap(tail, rhs.tail);
        std::swap(level, rhs.level);
        std::swap(length, rhs.length);
        std::swap(less, rhs.less);
    }
};

int main()
{
    {
        auto cmp = [](const string& a, const string& b) {return a.length() < b.length(); };
        skip_list < string, int, decltype(cmp)> list(cmp);
        //skip_list<string, int> list;
        list.insert("aab", 1321);
        list.insert("hello", 54342);
        list.insert("world", 544);
        list.show();

        for (auto it = list.begin(); it != list.end(); it++) {
            cout << it->key << " " << it->value << endl;
        }


    }

    cout << "==================================" << endl;
    
    {
        struct cmp {
            bool operator()(int a, int b) {
                return a > b;
            }
        };
        skip_list < int, int, cmp> list{};
        //skip_list<int, int> list;
        for (int i = 0; i <= 10; i++) list.insert(10 + 10 * i, 10 + 10 * i);
        list.show();
        for (auto it = list.begin(); it != list.end(); it++) cout << it->key << " " << it->value << endl;
    }

    cout << "==================================" << endl;

    {
        skip_list<int, int>list;
        list.insert(1, 6);
        list.insert(2, 5);
        list.insert(3, 4);
        list.insert(4, 3);
        list.insert(5, 2);
        list.insert(6, 1);
        list.erase(1);
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
