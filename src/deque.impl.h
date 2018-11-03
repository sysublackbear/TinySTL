#ifndef _DEQUE_IMPL_H_
#define _DEQUE_IMPL_H_

namespace TinySTL{
	namespace Detail{
		// 迭代器下向前移动一位
		template<class T>
		dq_iter<T>& dq_iter<T>::operator ++(){
			// 判断+1之后是否还在同一个桶里面
			// mapIndex_ : 当前在哪一段的连续存储区中
			// 是否已经到该存储区的末尾了
			if (cur_ != getBuckTail(mapIndex_))
				++cur_;  // 还没到末尾,直接相加
			// cur_ == getBuckTail(mapIndex_) && mapIndex_ + 1 < container_->mapSize_
			else if (mapIndex_ + 1 < container_->mapSize_){
				++mapIndex_;  // 跳到下一段存储区
				cur_ = getBuckHead(mapIndex_);
			}
			// cur_ == getBuckTail(mapIndex_) && mapIndex_ + 1 >= container_->mapSize_
			else{//+1后跳出了map
				mapIndex_ = container_->mapSize_;
				//cur_ = container_->map_[mapIndex_] + getBuckSize();//指向map_[mapSize_-1]的尾的下一个位置
				cur_ = container_->map_[mapIndex_];  // 直接推到当前的最后一位,但不分配新的空间了
			}
			return *this;
		}
		template<class T>
		dq_iter<T> dq_iter<T>::operator ++(int){
			auto res = *this;
			++(*this);
			return res;
		}
		template<class T>
		dq_iter<T>& dq_iter<T>::operator --(){
			if (cur_ != getBuckHead(mapIndex_))//当前不指向桶头
				--cur_;
			else if (mapIndex_ - 1 >= 0){//-1后还在map里面
				--mapIndex_;
				cur_ = getBuckTail(mapIndex_);
			}
			else{
				mapIndex_ = 0;
				cur_ = container_->map_[mapIndex_]; //指向map_[0]的头,不分配新的空间了。
			}
			return *this;
		}
		template<class T>
		dq_iter<T> dq_iter<T>::operator --(int){
			auto res = *this;
			--(*this);
			return res;
		}
		template<class T>
		bool dq_iter<T>::operator ==(const dq_iter& it)const{
			return ((mapIndex_ == it.mapIndex_) &&
				(cur_ == it.cur_) && (container_ == it.container_));
		}
		template<class T>
		bool dq_iter<T>::operator !=(const dq_iter<T>& it)const{
			return !(*this == it);
		}
		template<class T>
		dq_iter<T>& dq_iter<T>::operator = (const dq_iter& it){
			if (this != &it){
				mapIndex_ = it.mapIndex_;
				cur_ = it.cur_;
				container_ = it.container_;
			}
			return *this;
		}
		template<class T>
		void dq_iter<T>::swap(dq_iter& it){
			TinySTL::swap(mapIndex_, it.mapIndex_);
			TinySTL::swap(cur_, it.cur_);
			//TinySTL::swap(container_, it.container_);
		}
		template<class T>
		dq_iter<T> operator + (const dq_iter<T>& it, typename dq_iter<T>::difference_type n){//assume n >= 0
			dq_iter<T> res(it);
			auto m = res.getBuckTail(res.mapIndex_) - res.cur_;  // 还差多少步
			if (n <= m){//前进n步仍在同一个桶中
				res.cur_ += n;
			}
			else{  // 多出的距离走到下一个桶里面
				n = n - m;
				res.mapIndex_ += (n / it.getBuckSize() + 1);
				auto p = res.getBuckHead(res.mapIndex_);
				auto x = n % it.getBuckSize() - 1;
				res.cur_ = p + x;
			}
			return res;
		}
		template<class T>
		dq_iter<T> operator + (typename dq_iter<T>::difference_type n, const dq_iter<T>& it){
			return (it + n);
		}
		template<class T>
		dq_iter<T> operator - (const dq_iter<T>& it, typename dq_iter<T>::difference_type n){//assume n >= 0
			dq_iter<T> res(it);
			auto m = res.cur_ - res.getBuckHead(res.mapIndex_);
			if (n <= m)//后退n步还在同一个桶中
				res.cur_ -= n;
			else{
				n = n - m;
				res.mapIndex_ -= (n / res.getBuckSize() + 1);
				res.cur_ = res.getBuckTail(res.mapIndex_) - (n % res.getBuckSize() - 1);
			}
			return res;
		}
		template<class T>
		dq_iter<T> operator - (typename dq_iter<T>::difference_type n, const dq_iter<T>& it){
			return (it - n);
		}
		template<class T>
		typename dq_iter<T>::difference_type operator - (const dq_iter<T>& it1, const dq_iter<T>& it2){
			if (it1.container_ == it2.container_ && it1.container_ == 0)
				return 0;
			return typename dq_iter<T>::difference_type(it1.getBuckSize()) * (it1.mapIndex_ - it2.mapIndex_ - 1)
				+ (it1.cur_ - it1.getBuckHead(it1.mapIndex_)) + (it2.getBuckTail(it2.mapIndex_) - it2.cur_) + 1;
		}
		template<class T>
		void swap(dq_iter<T>& lhs, dq_iter<T>& rhs){
			lhs.swap(rhs);
		}
		// 求出当前存储区位置的末尾
		template<class T>
		T *dq_iter<T>::getBuckTail(size_t mapIndex)const{
			return container_->map_[mapIndex] + (container_->getBuckSize() - 1);
		}
		// 求出当前存储区位置的开头
		template<class T>
		T *dq_iter<T>::getBuckHead(size_t mapIndex)const{
			return container_->map_[mapIndex];
		}
		template<class T>
		size_t dq_iter<T>::getBuckSize()const{
			return container_->getBuckSize();
		}
	}//end of Detail namespace

	// 判断是否到deque末尾
	template<class T, class Alloc>
	bool deque<T, Alloc>::back_full()const{
		return map_[mapSize_ - 1] && (map_[mapSize_]) == end().cur_;
	}
	template<class T, class Alloc>
	bool deque<T, Alloc>::front_full()const{
		return map_[0] && map_[0] == begin().cur_;
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::deque_aux(size_t n, const value_type& val, std::true_type){
		int i = 0;
		for (; i != n / 2; ++i)
			(*this).push_front(val);
		for (; i != n; ++i)
			(*this).push_back(val);
	}
	template<class T, class Alloc>
	template<class Iterator>
	void deque<T, Alloc>::deque_aux(Iterator first, Iterator last, std::false_type){
		difference_type mid = (last - first) / 2;
		// 尽量往两个方向扩散，保证尽可能不倾斜
		for (auto it = first + mid; it != first - 1; --it)
			(*this).push_front(*it);
		for (auto it = first + mid + 1; it != last; ++it)
			(*this).push_back(*it);
	}
	// 空deque初始化
	template<class T, class Alloc>
	void deque<T, Alloc>::init(){
		mapSize_ = 2;  // 默认2个map
		map_ = getANewMap(mapSize_);  // 构造map
		beg_.container_ = end_.container_ = this;  // 最开始没有元素,所以指向同一块空间
		beg_.mapIndex_ = end_.mapIndex_ = mapSize_ - 1;
		beg_.cur_ = end_.cur_ = map_[mapSize_ - 1];  // 一开始位于最后一个存储区的空间
	}
	template<class T, class Alloc>
	T *deque<T, Alloc>::getANewBuck(){
		return dataAllocator::allocate(getBuckSize());
	}
	template<class T, class Alloc>
	T** deque<T, Alloc>::getANewMap(const size_t size){
		T **map = new T*[size];
		for (int i = 0; i != size; ++i)
			map[i] = getANewBuck();  // 每个map需要分配一段连续空间
		return map;
	}
	template<class T, class Alloc>
	size_t deque<T, Alloc>::getNewMapSize(const size_t size){
		return (size == 0 ? 2 : size * 2);
	}
	template<class T, class Alloc>
	size_t deque<T, Alloc>::getBuckSize()const{
		// enum class EBucksSize{BUCKSIZE = 64};
		// 强制规定每个存储区的大小固定为64字节
		return (size_t)EBucksSize::BUCKSIZE;
	}
	// clear()用于清除整个deque，请注意，deque的最初状态（无任何元素时）
	// 保有一个缓冲区，因此，clear()完成之后恢复初始状态，也一样要保留一个缓冲区
	template<class T, class Alloc>
	void deque<T, Alloc>::clear(){
		/*for (int i = 0; i != mapSize_; ++i)
		if (!map_[i])
		dataAllocator::destroy(map_[i], map_[i] + getBuckSize());*/
		for (auto i = 0; i != mapSize_; ++i){
			for (auto p = map_[i] + 0; !p && p != map_[i] + getBuckSize(); ++p)
				dataAllocator::destroy(p);
		}
		mapSize_ = 0;
		beg_.mapIndex_ = end_.mapIndex_ = mapSize_ / 2;
		beg_.cur_ = end_.cur_ = map_[mapSize_ / 2];
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::operator[] (size_type n){
		return *(begin() + n);  // 随机访问通过调用重新实现的+操作符来达到目的
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::front(){
		return *begin();
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::reference deque<T, Alloc>::back(){
		return *(end() - 1);
	}
	//由于const迭代器的设计失误故以下三个const函数会丧失const特性
	template<class T, class Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::operator[] (size_type n) const{
		return *(begin() + n);
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::front() const{
		return *begin();
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::const_reference deque<T, Alloc>::back() const{
		return *(end() - 1);
	}
	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::begin(){ return beg_; }
	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::end(){ return end_; }
	//迭代器设计失误，会使下面两个函数丧失const特性，暂时没有效解决办法故只能先这样妥协
	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::begin()const{ return beg_; }
	template<class T, class Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::end()const{ return end_; }
	template<class T, class Alloc>
	deque<T, Alloc>::~deque(){
		for (int i = 0; i != mapSize_; ++i){
			for (auto p = map_[i] + 0; !p && p != map_[i] + getBuckSize(); ++p)
				dataAllocator::destroy(p);
			if (!map_[i])
				dataAllocator::deallocate(map_[i], getBuckSize());
		}
		delete[] map_;
	}
	template<class T, class Alloc>
	deque<T, Alloc>::deque()
		:mapSize_(0), map_(0){}
	// value_type()：默认值
	template<class T, class Alloc>
	deque<T, Alloc>::deque(size_type n, const value_type& val = value_type()){
		deque();
		deque_aux(n, val, typename std::is_integral<size_type>::type());
	}
	template<class T, class Alloc>
	template <class InputIterator>
	deque<T, Alloc>::deque(InputIterator first, InputIterator last){
		deque();
		deque_aux(first, last, typename std::is_integral<InputIterator>::type());
	}
	// 深拷贝
	template<class T, class Alloc>
	deque<T, Alloc>::deque(const deque& x){
		mapSize_ = x.mapSize_;
		map_ = getANewMap(mapSize_);
		for (int i = 0; i + x.beg_.mapIndex_ != x.mapSize_; ++i)
			for (int j = 0; j != getBuckSize(); ++j)
				map_[x.beg_.mapIndex_ + i][j] = x.map_[x.beg_.mapIndex_ + i][j];
		beg_.mapIndex_ = x.beg_.mapIndex_;
		end_.mapIndex_ = x.end_.mapIndex_;
		beg_.cur_ = map_[beg_.mapIndex_] + (x.beg_.cur_ - x.map_[x.beg_.mapIndex_]);
		end_.cur_ = map_[end_.mapIndex_] + (x.end_.cur_ - x.map_[x.end_.mapIndex_]);
		beg_.container_ = end_.container_ = this;
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::reallocateAndCopy(){
		auto newMapSize = getNewMapSize(mapSize_);  // 扩大mapSize_,直接翻大两倍
		T** newMap = getANewMap(newMapSize);  // 重新分配map
		size_t startIndex = newMapSize / 4;
		for (int i = 0; i + beg_.mapIndex_ != mapSize_; ++i)
			for (int j = 0; j != getBuckSize(); ++j)
				newMap[startIndex + i][j] = map_[beg_.mapIndex_ + i][j];

		size_t n = beg_.cur_ - map_[beg_.mapIndex_];  // 原来的偏移量
		auto size = this->size();
		auto b = beg_, e = end_;
		clear();
		mapSize_ = newMapSize;
		map_ = newMap;
		beg_ = iterator(startIndex, newMap[startIndex] + n, this);  // 新的偏移量
		end_ = beg_ + size;

	}
	template<class T, class Alloc>
	void deque<T, Alloc>::push_back(const value_type& val){
		if (empty()){
			// 队列为空,分配空间
			init();
		}
		else if (back_full()){
			reallocateAndCopy();  // 重新分配空间，扩大为原来的两倍
		}
		//*end_ = val;
		//bug fix
		//2015.01.02
		TinySTL::construct(end_.cur_, val);
		++end_;
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::push_front(const value_type& val){
		if (empty()){
			init();
		}
		else if (front_full()){
			reallocateAndCopy();
		}  
		--beg_;  // beg_和end_一开始就位于最后一个存储空间的第一位，这个值得注意!!!
		//*beg_ = val;
		//bug fix
		//2015.01.02
		TinySTL::construct(beg_.cur_, val);
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::pop_front(){
		dataAllocator::destroy(beg_.cur_);  //析构当前空间
		++beg_;
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::pop_back(){
		--end_;
		dataAllocator::destroy(end_.cur_);  // 析构尾部
	}
	template<class T, class Alloc>
	void deque<T, Alloc>::swap(deque<T, Alloc>& x){
		TinySTL::swap(mapSize_, x.mapSize_);
		TinySTL::swap(map_, x.map_);
		beg_.swap(x.beg_);
		end_.swap(x.end_);
	}

	template <class T, class Alloc>
	bool operator== (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs){
		auto cit1 = lhs.begin(), cit2 = rhs.begin();
		for (; cit1 != lhs.end() && cit2 != rhs.end(); ++cit1, ++cit2){
			if (*cit1 != *cit2)
				return false;
		}
		if (cit1 == lhs.end() && cit2 == rhs.end())
			return true;
		return false;
	}
	template <class T, class Alloc>
	bool operator!= (const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs){
		return !(lhs == rhs);
	}
	template <class T, class Alloc>
	void swap(deque<T, Alloc>& x, deque<T, Alloc>& y){
		x.swap(y);
	}
}
#endif
