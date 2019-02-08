#ifndef _COWPTR_IMPL_H_
#define _COWPTR_IMPL_H_

// 用shared_ptr来实现Copy-On-Write
namespace TinySTL{
	namespace Detail{
		template<class T>
		const T& proxy<T>::operator *()const{ 
			return *(cp_->ptr_); 
		}
		// 写的时候才另外分配空间
		template<class T>
		T& proxy<T>::operator *(){
			auto t = *(cp_->ptr_);
			cp_->ptr_ = make_shared<T>(t);
			return *(cp_->ptr_);
		}
		template<class T>
		const T *proxy<T>::operator ->()const{
			return cp_->ptr_.operator->(); 
		}
		template<class T>
		T *proxy<T>::operator ->(){
			auto t = *(cp_->ptr_);
			cp_->ptr_ = make_shared<T>(t);
			return cp_->ptr_.operator->();
		}
		template<class T>
		cow_ptr<T>& proxy<T>::operator = (const T& val){
			cp_->ptr_ = make_shared<T>(val);
			return *cp_;
		}
		template<class T>
		proxy<T>::operator T()const{ return *(cp_->ptr_); }
	}
	template<class T>
	cow_ptr<T>::cow_ptr(T *p = nullptr) :ptr_(p){}
	template<class T>
	template<class D>
	cow_ptr<T>::cow_ptr(T *p, D d) : ptr_(p, d){}
	template<class T>
	cow_ptr<T>::cow_ptr(const cow_ptr& cp){
		ptr_ = cp.ptr_;
	}
	template<class T>
	cow_ptr<T>& cow_ptr<T>::operator = (const cow_ptr& cp){
		if (this != &cp){
			ptr_.decrease_ref();  // 先解掉当前引用
			ptr_ = cp.ptr_;
		}
		return *this;
	}
	template<class T>
	typename cow_ptr<T>::element_type *cow_ptr<T>::get(){
		return ptr_.get();  // 不需要修改数据，不需要复制
	}
	template<class T>
	const typename cow_ptr<T>::element_type *cow_ptr<T>::get()const{
		return ptr_.get();
	}
	template<class T>
	cow_ptr<T>::operator bool()const{
		return ptr_ != nullptr;
	}
	template<class T>
	const typename cow_ptr<T>::proxy<T> cow_ptr<T>::operator *()const{
		return proxy<T>(const_cast<cow_ptr *const>(this)); 
	}
	template<class T>
	typename cow_ptr<T>::proxy<T> cow_ptr<T>::operator *(){ 
		return proxy<T>(this); 
	}
	template<class T>
	const typename cow_ptr<T>::proxy<T> cow_ptr<T>::operator ->()const{ 
		return proxy<T>(const_cast<cow_ptr *const>(this)); 
	}
	template<class T>
	typename cow_ptr<T>::proxy<T> cow_ptr<T>::operator ->(){ 
		return proxy<T>(this); 
	}

	template<class T1, class T2>
	bool operator == (const cow_ptr<T1>& cp1, const cow_ptr<T2>& cp2){
		return cp1.ptr_ == cp2.ptr_;
	}
	template<class T>
	bool operator == (const cow_ptr<T>& cp, nullptr_t p){
		return cp.ptr_ == p;
	}
	template<class T>
	bool operator == (nullptr_t p, const cow_ptr<T>& cp){
		return cp == p;
	}
	template<class T1, class T2>
	bool operator != (const cow_ptr<T1>& cp1, const cow_ptr<T2>& cp2){
		return !(cp1 == cp2);
	}
	template<class T>
	bool operator != (const cow_ptr<T>& cp, nullptr_t p){
		return !(cp == p);
	}
	template<class T>
	bool operator != (nullptr_t p, const cow_ptr<T>& cp){
		return !(cp == p);
	}
}

#endif
