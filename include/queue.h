#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "deque.h"
#include "functional.h"
#include "vector.h"

namespace TinySTL{
	//class of queue
	template<class T, class Container = TinySTL::deque<T>>
	class queue{
	public:
		typedef T value_type;
		typedef Container container_type;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::size_type size_type;
	private:
		Container container_;
	public:
		queue(){}
		explicit queue(const container_type& ctnr) :container_(ctnr){}

		bool empty() const{ return container_.empty(); }
		size_type size() const{ return container_.size(); }
		reference& front(){ return container_.front(); }
		const_reference& front() const{ return container_.front(); }
		reference& back(){ return container_.back(); }
		const_reference& back() const{ return container_.back(); }
		void push(const value_type& val){ container_.push_back(val); }
		void pop(){ container_.pop_front(); }
		void swap(queue& x){ container_.swap(x.container_); }
	public:
		template <class T, class Container>
		friend bool operator== (const queue<T, Container>& lhs, const queue<T, Container>& rhs);
		template <class T, class Container>
		friend bool operator!= (const queue<T, Container>& lhs, const queue<T, Container>& rhs);
		template <class T, class Container>
		friend void swap(queue<T, Container>& x, queue<T, Container>& y);
	};
	template <class T, class Container>
	bool operator== (const queue<T, Container>& lhs, const queue<T, Container>& rhs){
		return lhs.container_ == rhs.container_;
	}
	template <class T, class Container>
	bool operator!= (const queue<T, Container>& lhs, const queue<T, Container>& rhs){
		return lhs.container_ != rhs.container_;
	}
	template <class T, class Container>
	void swap(queue<T, Container>& x, queue<T, Container>& y){
		TinySTL::swap(x.container_, y.container_);
	}

	// 用vector实现优先队列
	// priority_queue 不是container,
	//class of priority_queue
	template <class T, class Container = TinySTL::vector<T>, 
				class Compare = TinySTL::less<typename Container::value_type>> 
	class priority_queue{
	public:
		typedef T value_type;
		typedef Container container_type;
		typedef typename Container::reference reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::size_type size_type;
	private:
		container_type container_;
		Compare compare_;
	public:
		// 以下用到的make_heap(),push_heap(),pop_heap()都是泛型算法
		// 注意:任一个构造函数都立刻于底层容器内产生一个implicit representation heap
		explicit priority_queue(const Compare& comp = Compare(),
			const Container& ctnr = Container())
			: container_(ctnr), compare_(comp){}
		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last,
			const Compare& comp = Compare(),
			const Container& ctnr = Container())
			: container_(ctnr), compare_(comp){
			container_.insert(container_.end(), first, last);
			// 利用make_heap构造vector
			TinySTL::make_heap(container_.begin(), container_.end());
		}
		bool empty() const{
			return container_.empty();
		}
		size_type size() const{
			return container_.size();
		}
		reference top() {
			return container_.front();
		}
		void push(const value_type& val){
			// push_heap是泛型算法,先利用底层容器的push_back()将新元素推入末端
			// 再重排heap
			container_.push_back(val);
			TinySTL::push_heap(container_.begin(), container_.end(), compare_);
		}
		void pop(){
			// pop_heap 是泛型算法,从heap内取出一个元素。它并不是真正把元素弹出
			// 而是重排heap,然后再以底层容器的 pop_back() 取得被弹出的元素。
			TinySTL::pop_heap(container_.begin(), container_.end(), compare_);
			container_.pop_back();
		}
		void swap(priority_queue& x){
			TinySTL::swap(container_, x.container_);
			TinySTL::swap(compare_, x.compare_);
		}
	public:
		template <class T, class Container, class Compare>
		friend void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y);
	};
	template <class T, class Container, class Compare>
	void swap(priority_queue<T, Container, Compare>& x, priority_queue<T, Container, Compare>& y){
		x.swap(y);
	}
}

#endif
