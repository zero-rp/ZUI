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

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>

#define BOOL int
#define FALSE 0
#define TRUE 1

#endif

#ifdef __cplusplus
	extern "C" {
#endif

	typedef size_t key_t;
	typedef void *data_t;

	typedef struct rb_node_
	{
		struct rb_node_ *rb_right;
		struct rb_node_ *rb_left;
		size_t  rb_parent_color;
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
    rb_node *rb_minkey(rb_root *root);
#if (defined LOG_DEBUG) && (LOG_DEBUG == 1)
	void rb_print(rb_root *root);
#endif




#ifdef __cplusplus
	}
#endif

#endif //__ZUI_CORE_TREE_H__