#pragma once
#include <algorithm>
#include <iterator>


template <typename T>
class dvaque{
	//class diterator;
	//class const_diterator;


	//T* body;
	static constexpr size_t CHUNK = 16;// static constexpr


	T** map;
	size_t map_size;
	size_t start, start_offset;
	size_t finish, finish_offset;

	// самая важная штука
	void redistribuere(size_t new_map_size){
		
		void* raw = ::operator new[](new_map_size * sizeof(T*)); // помойму эта штука выделят без начальной инициализации иначе можно uint8_t* а потом также кастануть 
		T** new_map = reinterpret_cast<T**>(raw);
		

		size_t old_blocks = finish - start + 1;
		size_t new_start = (new_map_size - old_blocks) / 2;

		if (map) {
			std::copy(map + start, map + start + old_blocks,
				new_map + new_start);
		}

		//std::swap(new_map, map);
		operator delete[](map); 

		map = new_map;
		map_size = new_map_size;

		finish = new_start + old_blocks - 1;
		start = new_start;
		
		//std::swap()

	}

	void alloc_chunk(size_t node) {
		void* raw_memory = operator new[](CHUNK * sizeof(T));
		map[node] = reinterpret_cast<T*>(raw_memory);
	}

	void the_swap(dvaque& another) {

		std::swap(map, another.map);
		std::swap(map_size, another.map_size);
		std::swap(start, another.start);
		std::swap(start_offset, another.start_offset);
		std::swap(finish, another.finish);
		std::swap(finish_offset, another.finish_offset);
	}

	void add_front() {
		if (start_offset == 0) {

			if (start == 0) {
				redistribuere(map_size + map_size / 3 + 2);
			}

			alloc_chunk(start - 1);

			--start;
			start_offset = CHUNK;
		}
	}

	void add_back() {
		if (finish_offset == CHUNK) {

			if (finish+1 == map_size) {
				redistribuere(map_size + map_size/3 + 2);
			}

			alloc_chunk(finish + 1);

			++finish;
			finish_offset = 0;
		}
	}

public:
	
	dvaque() :map(nullptr),map_size(0), start(0), start_offset(0), finish(0), finish_offset(0) {
		redistribuere(1);
		alloc_chunk(start);
	}

	dvaque(size_t size) : map(nullptr), start(0), start_offset(0), finish(0), finish_offset(0) {
		


		if (size == 0) {
			redistribuere(1);
			alloc_chunk(start);
			return;
		}
		redistribuere(1);
		alloc_chunk(start);
		resize(size);
		

	}

	dvaque(const dvaque& other):map(nullptr)/*не знаю,нужно ли мар инициализировать дефолт значением*/,map_size(0), start(0), start_offset(0), finish(0), finish_offset(0) {
		
		if(other.empty()){
			redistribuere(1);
			alloc_chunk(start);

			return;
		}


		size_t num_blocks = other.finish - other.start + 1;
		redistribuere(num_blocks);
		
		start_offset = other.start_offset;
		finish_offset = other.finish_offset;


		for (size_t i = start; i <= finish; ++i) {
			alloc_chunk(i);

			//T* uninit = map[i];
			//T* init = other.map[i];

			size_t copy_from = (i == other.start) ? other.start_offset : 0;
			size_t copy_to = (i == other.finish) ? other.finish_offset : CHUNK;


			for (size_t j = copy_from; j < copy_to; ++j) {
				new (&map[i][j]) T(other.map[i - start + other.start][j]); // placement new какая же это шняга,  тут инициализиуются чанки
			}
		}
	}


	dvaque(dvaque&& other): map(nullptr), map_size(0), start(0), start_offset(0), finish(0), finish_offset(0) {
		the_swap(other);
	}


	~dvaque() {
		if (!map) return;

		if (start > finish) {
			operator delete[](map);
			return;
		}

		if (start == finish) {
			
			for (size_t j = start_offset; j < finish_offset; ++j){
				map[start][j].~T();
			}
		}
		else {
			
			for (size_t j = start_offset; j < CHUNK; ++j){
				map[start][j].~T();
			}

			
			for (size_t i = start + 1; i < finish; ++i){
				for (size_t j = 0; j < CHUNK; ++j)
				{
					map[i][j].~T();
				}
			}

			
			for (size_t j = 0; j < finish_offset; ++j){
				map[finish][j].~T();
			}
		}

		
		for (size_t i = start; i <= finish; ++i){
			operator delete[](map[i]);
		}

		
		operator delete[](map);
	}

	dvaque& operator =(dvaque other) noexcept {
		
			the_swap(other);
		
		return *this;
	}
	/*dvaque& operator =(dvaque&& other) noexcept {
		if (this != &other) {
			the_swap(other);
		}

		
		return *this;
	}*/

	T& operator [](size_t index) {
		size_t absolute_index = index + start_offset;
		size_t block = start + absolute_index / CHUNK;
		size_t offset = absolute_index % CHUNK;
		return map[block][offset];
	}
	const T& operator [](size_t index) const {
		size_t absolute_index = index + start_offset;
		size_t block = start + absolute_index / CHUNK;
		size_t offset = absolute_index % CHUNK;
		return map[block][offset];
	}

	void push_back(const T& value) {
		add_back();
		new (&map[finish][finish_offset]) T(value);

		finish_offset++; //можно постфиксным инкрементом, но он копию делает, поэтому пофигж
	}

	void pop_back() {
		if (finish_offset == 0) {
			
			//assert(finish > start);  
			operator delete[](map[finish]);  
			--finish;
			finish_offset = CHUNK;
		}

		--finish_offset;
		map[finish][finish_offset].~T();
	}

	void push_front(const T& value) {
		add_front();
		new (&map[start][--start_offset]) T(value);

	}

	void pop_front() {
		
		map[start][start_offset].~T();
		++start_offset;

		if (start_offset == CHUNK && finish > start) {
			operator delete[](map[start ]);

			++start;
			start_offset = 0;
		}
	}

	void resize(size_t new_size) {
		size_t cur = size();
		if (new_size < cur) {
			
			for (size_t i = new_size; i < cur; ++i){
				pop_back();
			}
		}
		else if (new_size > cur) {
			
			for (size_t i = cur; i < new_size; ++i){
				push_back(T{});
			}
		}
	}

	/*void set_chunk_size(size_t new_size) {
		CHUNK = new_size;
	}*/

	size_t size() const {
		return (finish - start) * CHUNK - start_offset + finish_offset;
	}
	size_t capasity() const {
		return CHUNK * map_size;
	}

	T& back() {
		return map[finish][finish_offset-1];
	}
	const T& back() const{
		return map[finish][finish_offset-1];
	}

	T& front() {
		return map[start][start_offset];
	}
	const T& front() const {
		return map[start][start_offset];
	}

	bool empty()const {
		return  size() == 0; 
	}

	class iterator : public std::iterator<std::random_access_iterator_tag,T> {
		dvaque* ptr;
		size_t node, offset;
	public:	
		/*
		Эта штука работает после c++ 17
		
		using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
		*/

		using typename std::iterator<std::random_access_iterator_tag, T>::iterator_category;
		using typename std::iterator<std::random_access_iterator_tag, T>::value_type;
		using typename std::iterator<std::random_access_iterator_tag, T>::difference_type;
		using typename std::iterator<std::random_access_iterator_tag, T>::pointer;
		using typename std::iterator<std::random_access_iterator_tag, T>::reference;


		iterator(dvaque* _ptr,size_t _node,size_t _offset):ptr(_ptr),node(_node),offset(_offset) {}

		reference operator *()const {
			return ptr->map[node][offset];
		}
		
		pointer operator ->()const {
			return &ptr->map[node][offset];
		}

		iterator& operator++() {
			if (++offset == ptr->CHUNK) {
				offset = 0;
				++node;
			}
			return *this;
		}
		iterator operator++(int) {
			iterator temp = *this;
			++(*this);
			return temp;
		}

		iterator& operator--() {
			if (offset == 0) {
				--node;
				offset = ptr->CHUNK;
			}
			--offset;
			return *this;
		}
		iterator operator--(int) {
			iterator temp = *this;
			--(*this);
			return temp;
		}

		 
		iterator& operator +=(const difference_type& another) {

			difference_type index = to_linar() + another;
			size_t uindex = static_cast<size_t>(index);

			node = ptr->start + (uindex / ptr->CHUNK);
			offset = (uindex % ptr->CHUNK);

			return *this;
		}

		iterator& operator -=(const difference_type& other) {
			return *this += -other;
		}

		/*size_t operator -=(const iterator& other) {
			size_t index_this = (node - ptr->start) * ptr->CHUNK + offset;
			size_t oher_index = (other.node - ptr->start) * ptr->CHUNK + offset;
			return index_this - oher_index;
		}*/
		difference_type to_linar() const {
			return difference_type(node - ptr->start) * ptr->CHUNK + difference_type(offset);
		}


		//костыль нужный для такого синтаксиса for( auto it = b.begin(); it != b.end(); ++it)
		bool operator ==(const typename dvaque<T>::iterator& other) const {
			return (ptr == other.ptr) && (node == other.node) && (offset == other.offset);
		}

		bool operator !=(const typename dvaque<T>::iterator& other) const {
			return !(*this == other);
		}

	};

	class const_iterator : public std::iterator<std::random_access_iterator_tag,T> {
		const dvaque* ptr;
		size_t node, offset;
	public:	
		/*
		Это я позаимствовал, но это надо как-то интегрировать
		
		
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
		*/
		using typename std::iterator<std::random_access_iterator_tag, T>::iterator_category;
		using typename std::iterator<std::random_access_iterator_tag, T>::value_type;
		using typename std::iterator<std::random_access_iterator_tag, T>::difference_type;
		using typename std::iterator<std::random_access_iterator_tag, T>::pointer;
		using typename std::iterator<std::random_access_iterator_tag, T>::reference;

		const_iterator(const dvaque* _ptr, size_t _node, size_t _offset): ptr(_ptr), node(_node), offset(_offset) {

		}

		reference operator *()const {
			return ptr->map[node][offset];
		}

		pointer operator ->()const {
			return &ptr->map[node][offset];
		}

		const_iterator& operator++() {
			if (++offset == ptr->CHUNK) {
				offset = 0;
				++node;
			}
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator temp = *this;
			++(*this);
			return temp;
		}

		const_iterator& operator--() {
			if (offset == 0) {
				--node;
				offset = ptr->CHUNK;
			}
			--offset;
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator temp = *this;
			--(*this);
			return temp;
		}

		const_iterator& operator +=(const difference_type& other) {

			difference_type index = to_linar() + other;
			size_t uindex = static_cast<size_t>(index);

			node = ptr->start + (uindex / ptr->CHUNK);
			offset = (uindex % ptr->CHUNK);

			return *this;
		}

		/*const_iterator& operator -=(const difference_type& other) {
			return *this += -other;
		}*/

		/*size_t operator -=(const const_iterator& other) {
			size_t index_this = (node - ptr->start) * ptr->CHUNK + offset;
			size_t oher_index = (other.node - ptr->start) * ptr->CHUNK + offset;
			return index_this - oher_index;
		}*/

		difference_type to_linar() const {
			return difference_type(node - ptr->start) * ptr->CHUNK + difference_type(offset);
		}

		//аналогичный костыль 
		bool operator ==(const typename dvaque<T>::const_iterator& other) const {
			return  (ptr == other.ptr) && (node == other.node) && (offset == other.offset);
		}

		bool operator !=(const typename dvaque<T>::const_iterator& other) const {
			return !(*this == other);
		}

	};


	//using iterator = typename dvaque<T>::iterator;
	//using const_iterator = typename dvaque<T>::const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	operator const_iterator() const {
		return const_iterator(this, start, start_offset);
	}


	iterator begin()  {
		return iterator(this, start, start_offset);
	}

	const_iterator begin() const {
		return const_iterator(this, start, start_offset);
	}

	const_iterator cbegin() const {
		return const_iterator(this, start, start_offset);
	}

	iterator end() {
		return iterator(this, finish, finish_offset);
	}

	const_iterator end() const {
		return const_iterator(this, finish, finish_offset);
	}

	const_iterator cend() const{
		return const_iterator(this, finish, finish_offset);
	}

	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const {
		return reverse_iterator(end());
	}
	
	const_reverse_iterator crbegin() const {
		return reverse_iterator(cend());
	}
	
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const {
		return reverse_iterator(begin());
	}
	
	const_reverse_iterator crend() const {
		return reverse_iterator(cbegin());
	}



};


 // внешние функции итераторов

template <typename T>
 typename dvaque<T>::iterator operator+(const typename dvaque<T>::iterator &first,const typename dvaque<T>::iterator::difference_type &other) {
	typename dvaque<T>::iterator copy(first);
	return copy += other;
}

 template <typename T>
 typename dvaque<T>::iterator operator-(const typename dvaque<T>::iterator &first, const typename dvaque<T>::iterator::difference_type &other) {
	 typename dvaque<T>::iterator copy(first);
	 return copy -= other;
 }

 template <typename T>
 typename dvaque<T>::iterator::difference_type operator-(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) { 
	 return first.to_linar() - second.to_linar();
 }

template <typename T>
bool operator >(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return (first - second) > 0;
}

template <typename T>
bool operator <(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return second> first;
}

template <typename T>
bool operator ==(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return !(second > first) && !(second < first);
}

template <typename T>
bool operator !=(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return !(second == first);
}

template <typename T>
bool operator >=(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return !(first < second);
}

template <typename T>
bool operator <=(const typename dvaque<T>::iterator& first, const typename dvaque<T>::iterator& second) {
	return !(first > second);
}


//----------------Константный Итератор---------------------



template <typename T>
 typename dvaque<T>::const_iterator operator+(const typename dvaque<T>::const_iterator &first, typename dvaque<T>::const_iterator::difference_type other) {
	typename dvaque<T>::const_iterator copy(first);
	return copy += other;
}

 //template <typename T>
 //typename dvaque<T>::const_iterator operator-(const typename dvaque<T>::const_iterator &first, const typename dvaque<T>::const_iterator::difference_type &other) {
	// typename dvaque<T>::const_iterator copy(first);
	// return copy -= other;
 //}

 template <typename T>
 typename dvaque<T>::const_iterator::difference_type operator-(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) { 
	 return first.to_linar() - second.to_linar();
 }

template <typename T>
bool operator >(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return (first - second) > 0;
}

template <typename T>
bool operator <(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return second> first;
}

template <typename T>
bool operator ==(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return !(second > first) && !(second < first);
}

template <typename T>
bool operator !=(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return !(second == first);
}

template <typename T>
bool operator >=(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return !(first < second);
}

template <typename T>
bool operator <=(const typename dvaque<T>::const_iterator& first, const typename dvaque<T>::const_iterator& second) {
	return !(first > second);
}
