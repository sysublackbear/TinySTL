#ifndef _REF_H_
#define _REF_H_

#include <atomic>
#include <functional>
#include <memory>

namespace TinySTL{
	namespace Detail{
		template<class T>
		struct _default_delete{
			void operator ()(T* ptr){ if (ptr) delete ptr; }
		};

		template<class T>
		struct ref_t{
			using deleter_type = std::function < void(T*) >;

			// shared_ptr保持引用计数的原子性：std::atomic
			// std::atomic对int, char, bool等数据结构进行原子性封装，在多线程环境中，
			// 对std::atomic对象的访问不会造成竞争-冒险。利用std::atomic可实现数据结构的无锁设计。
			// 本质上，linux对原子变量的实现——编译器帮你做好CAS
			// 主要有：GCC的CAS：__sync_bool_compare_and_swap
			// C++11的CAS：std::atomic
			std::atomic<size_t> ncount_;
			T *data_;
			deleter_type deleter_;

			// 阻止不应该允许的经过转换构造函数进行的隐式转换的发生
			explicit ref_t(T *p = nullptr, deleter_type pfunc = deleter_type(_default_delete<T>()))
				: ncount_(0), data_(p), deleter_(pfunc){
				if (data_)
					ncount_ = 1;
			}
			// = default: 允许我们使用=default来要求编译器生成一个默认函数
			// = delete：禁止使用编译器默认生成的函数
			ref_t(const ref_t&) = delete;  
			ref_t& operator = (const ref_t&) = delete;

			~ref_t(){
				--ncount_;
				if (ncount_ == 0)  // 内存计数到0，自动释放空间
					deleter_(data_);
			}

			size_t count()const{ return ncount_.load(); }  // std::atomic对象的值的读取和写入可使用load和store实现。
			T *get_data()const{ return data_; }

			// 对ref_t进行累加的实现就是增加其内存计数
			ref_t& operator ++(){
				++ncount_;
				return *this;
			}
			ref_t operator ++(int){
				auto t = *this;
				++*this;
				return t;
			}
			ref_t& operator --(){
				--ncount_;
				return *this;
			}
			ref_t operator --(int){
				auto t = *this;
				--*this;
				return t;
			}
		};
		template<class T>
		bool operator ==(const ref_t<T>& lhs, const ref_t<T>& rhs){
			return lhs.get_data() == rhs.get_data();
		}
		template<class T>
		bool operator !=(const ref_t<T>& lhs, const ref_t<T>& rhs){
			return !(lhs == rhs);
		}
	}
}

#endif
