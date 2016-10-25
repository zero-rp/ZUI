/**
* @file		tree.h
* @brief	红黑树实现.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __ZUI_CORE_TREE_H__
#define __ZUI_CORE_TREE_H__

	#include <Windows.h>

#ifdef __cplusplus
	extern "C" {
#endif

	typedef int key_t;
	typedef void *data_t;

	typedef struct rb_node_
	{
		struct rb_node_ *rb_right;
		struct rb_node_ *rb_left;
		unsigned long  rb_parent_color;
		key_t key;
		data_t data;
	}rb_node;

	typedef struct rb_root_
	{
		rb_node *rb_node;
	}rb_root;


	typedef void(*TreeVisitFunc)(void *data);

	/*操作函数*/

	/*初始化*/
	rb_root *rb_new();
	/*插入*/
	BOOL rb_insert(key_t key, data_t data, rb_root *root);
	/*搜索*/
	rb_node *rb_search(key_t key, rb_root*root);
	/*删除*/
	void rb_delete(key_t key, rb_root*root);
	/*释放树*/
	void rb_free(rb_root*root);
	/*遍历树*/
	void rb_foreach(rb_root *root, TreeVisitFunc visitfunc);
#if (defined DEBUG_BORDER) && (DEBUG_BORDER == 1)
	void rb_print(rb_root *root);
#endif




#ifdef __cplusplus
	}
#endif

#endif //__ZUI_CORE_TREE_H__