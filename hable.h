#pragma once

#include <algorithm>
#include <iostream>
#include <cstring>

#include "dvaque.h"

inline uint64_t another_hasher(const std::string& input, uint64_t seed) {
    for (unsigned char c : input){
        seed = (seed << 5) + seed + c;
    }
    return seed;
};

struct hash1 {
    uint64_t operator () (const std::string& input, uint64_t seed = 3735928559) const {
        return another_hasher(input, seed);
    }
};

inline uint64_t MurmurHash64A(const void* key, size_t len, uint64_t seed /*начальное значение хеша*/) { //MurmurHash64A — простая и быстрая хеш-функция общего назначения, разработанная Остином Эпплби. Не является криптографически-безопасной, возвращает 32-разрядное беззнаковое число.    Из достоинств функции авторами отмечена простота, хорошее распределение, мощный лавинный эффект, высокая скорость и сравнительно высокая устойчивость к коллизиям.Текущие версии алгоритма оптимизированы под Intel - совместимые процессоры.
    const uint64_t m = 0xc6a4a7935bd1e995ULL; //множитель "перемешивания" битов
    const int r = 47; // кол-во ХОR-овых сдвигов

    uint64_t h = seed ^ (len * m); //инициализация
    const uint8_t* data = reinterpret_cast<const uint8_t*>(key);
    const uint8_t* end = data + (len & ~static_cast<size_t>(7));

    // Основной цикл по 8-байтным блокам
    while (data != end) {
        uint64_t k;
        std::memcpy(&k, data, sizeof(uint64_t));
        data += 8;

        //перемешивание 
        k *= m;
        k ^= k >> r;
        k *= m;

        //интеграция в общий хеш
        h ^= k;
        h *= m;
    }

    // Обработка хвоста - чё осталось то и запихиваем в k и просто смешиваем ручками
    uint64_t k = 0;
    switch (len & 7) {
    case 7: k ^= uint64_t(data[6]) << 48;
    case 6: k ^= uint64_t(data[5]) << 40;
    case 5: k ^= uint64_t(data[4]) << 32;
    case 4: k ^= uint64_t(data[3]) << 24;
    case 3: k ^= uint64_t(data[2]) << 16;
    case 2: k ^= uint64_t(data[1]) << 8;
    case 1: k ^= uint64_t(data[0]);
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
    }

    // Чередование XOR со сдвигом и умножения гарантирует, что каждый бит результата зависит от всех битов входных данных
    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

template <typename T>
uint64_t mumur_hash(const T& key, uint64_t seed) { //оболочка
    return MurmurHash64A(&key, sizeof(T), seed);
}



template <typename T>
struct base_hasher {
    size_t operator()(const T& input, uint64_t seed = 3735928559) const {

        uint64_t temp = mumur_hash(input, seed);

        return static_cast<size_t>(temp);
    }
};

template <>
struct base_hasher<std::string> {

    size_t operator() (const std::string& input, uint64_t seed = 0xDEADBEEF) const {
        return MurmurHash64A(input.data(), input.size(), seed);
    }
};











template <typename _key, typename _val, typename hasher = base_hasher<_key> >
class hable {
public:

    struct para {
        _key key;
        _val val;

        para(const _key& key, const _val& val) :key(key), val(val) {}
    };
    

private:
    //----------- ПОЛЯ -----------

    constexpr static const double rehash_size = 0.75;

    size_t _size;
    hasher _hasher;

    dvaque<dvaque<para>> bucket;// метод цепочек


    ////------------------

    

    void resize() {
        size_t new_size = (bucket.size() * 1.5) + 1;

        dvaque<dvaque<para>> new_bucket(new_size);

        for (size_t i = 0; i < bucket.size(); ++i) {

            for (int j = 0; j < bucket[i].size(); ++j) {

                size_t new_index = _hasher(bucket[i][j].key) % new_size;

                new_bucket[new_index].push_back(bucket[i][j]);
            }

            //new_bucket[i] = std::move(bucket[i]);
        }

        std::swap(new_bucket, bucket);

    }

    void protect_to_add() {
        if (bucket.size() == 0) {
            bucket = dvaque<dvaque<para>>(4);

        }

        if (static_cast<long double>(_size + 1) / static_cast<long double>(bucket.size()) > rehash_size) {
            resize();
        }
    }


public:

    


    hable(size_t sz = 4, hasher func = hasher()) :bucket(), _size(0), _hasher(func) {
        for (size_t i = 0; i < sz; ++i) {
            bucket.push_back(dvaque<para>());
        }
    }

    //возможно запилю конструктор перемещения

    ~hable() = default;

    //hable(const _key& key, const _val& val);



    //----------------МЕТОДЫ ДОСТУПА К ДАННЫМ-------------
    _val& operator[] (const _key& key) {

        protect_to_add();

        size_t indx = index(key);
        dvaque<para>& temp = bucket[indx];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == key) {
                return temp[i].val;
            }
        }

        temp.push_back(para(key, _val{}));
        ++_size;

        return temp[temp.size() - 1].val;
    }

    const _val* find(const _key& key) const {
        size_t ind = index(key);

        const dvaque <para>& temp = bucket[ind];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == key) {
                return &temp[i].val;
            }
        }

        return nullptr;
    }

    _val* find(const _key& key) {
        size_t ind = index(key);

        dvaque <para>& temp = bucket[ind];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == key) {
                return &temp[i].val;
            }
        }

        return nullptr;
    }

    const _val& at(const _key& key) const {
        const _val* ptr = find(key);

        if (ptr == nullptr) {
            throw std::out_of_range("Key not found");
        }

        return *ptr;
    }

    _val& at(const _key& key) {
        _val* ptr = find(key);

        if (ptr == nullptr) {
            throw std::out_of_range("Key not found");
        }

        return *ptr;
    }


    /*const _val& operator [] (const _key& key) const {
        return bucket[index(key)];
    }*/

    size_t size()const {
        return _size;
    }

    //--------------------------------------------------------

    size_t index(const _key& key) const {
        return static_cast<size_t> (_hasher(key)) % bucket.size();
    }

    bool contain(const _key& key) const {

        size_t ind = index(key);
        const dvaque <para>& temp = bucket[ind];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == key) {
                return true;
            }
        }

        return false;
    }


    void insert(const para& ins) {
        protect_to_add();

        size_t ind = index(ins.key);
        dvaque<para>& temp = bucket[ind];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == ins.key) {
                return;
            }
        }

        temp.push_back(ins);
        ++_size;
    }


    bool erase(const _key& key) {

        size_t indx = index(key);
        dvaque<para>& temp = bucket[indx];

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i].key == key) {

                size_t tsize = temp.size() - 1;

                if (i != tsize) {
                    std::swap(temp[i], temp[tsize]);
                }

                temp.pop_back();
                --_size;

                return true;
            }
        }

        return false;
    }

    dvaque<para> get_all()const{
        dvaque <para> res;
        for (size_t i = 0; i < bucket.size(); ++i) {
            for (size_t j = 0; j < bucket[i].size(); ++j) {
                res.push_back(bucket[i][j]);
            }
        }
        return res;
    }


};


