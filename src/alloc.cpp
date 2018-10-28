#include "include/alloc.h"

namespace TinySTL{

	char *alloc::start_free = 0;
	char *alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	alloc::obj *alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	void *alloc::allocate(size_t bytes){
		if (bytes > EMaxBytes::MAXBYTES){
			return malloc(bytes);
		}
		size_t index = FREELIST_INDEX(bytes);
		obj *list = free_list[index];
		if (list){//此list还有空间给我们
			free_list[index] = list->next;
			return list;
		}
		else{//此list没有足够的空间，需要从内存池里面取空间
			// 为内存池申请空间（调用malloc）
			return refill(ROUND_UP(bytes));  // ROUND_UP(bytes):将bytes上调至8的倍数
		}
	}
	void alloc::deallocate(void *ptr, size_t bytes){
		// 大于128bytes，直接调用1级配置器回收空间，直接free
		if (bytes > EMaxBytes::MAXBYTES){
			free(ptr);
		}
		else{
			// 根据大小寻找对应的free_list下标
			size_t index = FREELIST_INDEX(bytes);
			obj *node = static_cast<obj *>(ptr);
			// 将空间插回到free_list中，代表回收了。
			node->next = free_list[index];
			free_list[index] = node;
		}
	}
	void *alloc::reallocate(void *ptr, size_t old_sz, size_t new_sz){
		deallocate(ptr, old_sz);  // 先回收旧的
		ptr = allocate(new_sz);   // 再分配新的

		return ptr;
	}
	//返回一个大小为n的对象，并且有时候会为适当的free list增加节点
	//假设bytes已经上调为8的倍数
	// bytes: ROUND_UP(bytes)
	void *alloc::refill(size_t bytes){
		size_t nobjs = ENObjs::NOBJS;  // 20
		//从内存池里取
		char *chunk = chunk_alloc(bytes, nobjs);
		obj **my_free_list = 0;
		obj *result = 0;
		obj *current_obj = 0, *next_obj = 0;

		if (nobjs == 1){//取出的空间只够一个对象使用
			return chunk;
		}
		else{
			my_free_list = free_list + FREELIST_INDEX(bytes);  // 找出在free_list的位置
			result = (obj *)(chunk);
			*my_free_list = next_obj = (obj *)(chunk + bytes);
			//将取出的多余的空间加入到相应的free list里面去
			for (int i = 1;; ++i){
				current_obj = next_obj;
				next_obj = (obj *)((char *)next_obj + bytes);
				if (nobjs - 1 == i){
					current_obj->next = 0;
					break;
				}
				else{
					current_obj->next = next_obj;
				}
			}
			return result;
		}
	}
	//假设bytes已经上调为8的倍数
	// bytes: ROUND_UP(bytes), nobjs: ENObjs::NOBJS
	// nobjs: 默认分配20个节点，有可能缩小调整
	char *alloc::chunk_alloc(size_t bytes, size_t& nobjs){
		char *result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes){//内存池剩余空间完全满足需要
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes){//内存池剩余空间不能完全满足需要，但足够供应一个或以上的区块
			// 也直接予以分配
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else{//内存池剩余空间连一个区块的大小都无法提供
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			// 以下试着让内存池中的残余零头还有利用价值
			if (bytes_left > 0){
				// 内存池内还有一些零头，先配给适当的free_list
				// 首先寻找适当的free_list
				// bytes_left一定是8的倍数
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);  // 算出是第几个free-list
				// 调整free_list,将内存池中的残余空间编入
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;  // 将start_free加到链表头部(my_free_list)
			}
			// 配置堆空间
			start_free = (char *)malloc(bytes_to_get);
			if (!start_free){
				// 堆空间不足,malloc失败
				obj **my_free_list = 0, *p = 0;
				// 试着检视我们手上拥有的东西,这不会造成伤害.我们不打算尝试配置较小的区块,
				// 因为那在多进程机器上容易导致灾难。
				// 搜寻适当的free_list
				// 所谓适当是指“尚有未用之块,且区块够大”的free_list
				for (int i = 0; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN){
					my_free_list = free_list + FREELIST_INDEX(i);  // 找到链表的头结点，从0开始找
					p = *my_free_list;
					if (p != 0){  // free_list尚有未用区块
						// 调整free_list以释放未用区块
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						// 递归调用自己,看是否满足
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0;  // 遍历结束了，说明真的无内存可用了，重试malloc，看能否分配出内存
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}
}
