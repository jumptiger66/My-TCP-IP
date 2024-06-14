//
// Created by 孙昊 on 2024/6/7.
//

#ifndef NET_NLIST_H
#define NET_NLIST_H

// 链表中数据块的node信息
typedef struct _nlist_node_t{
    struct _nlist_node_t * pre;
    struct _nlist_node_t * next;
}nlist_node_t;

// 链表结点初始化
static inline void nlist_node_init(nlist_node_t * node){
    node->next = node->pre = (nlist_node_t *) 0;
}

// 结点 node 的下一个结点
static inline nlist_node_t * nlist_node_next(nlist_node_t * node){
    return node->next;
}

// 结点 node 的上一个结点
static inline nlist_node_t * nlist_node_pre(nlist_node_t * node){
    return node->pre;
}

// 使结点 node 的下一个结点指向 next 参数结点
static inline void nlist_node_set_next(nlist_node_t * node, nlist_node_t * next){
    node->next = next;
}

// 数据包-链表结构
typedef struct _nlist_t{
    nlist_node_t * first; // 链表第一个结点
    nlist_node_t * last; // 链表最后一个结点
    int count; // 链表中结点数
}nlist_t;

void nlist_init(nlist_t * list); // 链表初始化（first、last指向空，count置零）

// 是否空列表
static inline int nlist_is_empty (nlist_t * list){
    return list->count == 0;
}

// 列表当前结点数
static inline int nlist_count (nlist_t * list){
    return list->count;
}

// 列表首节点地址
static inline nlist_node_t * nlist_first (nlist_t * list){
    return list->first;
}

// 列表尾部结点地址
static inline nlist_node_t * nlist_last (nlist_t * list){
    return list->last;
}

void nlist_insert_first (nlist_t * list, nlist_node_t * node);
void nlist_insert_last (nlist_t * list, nlist_node_t * node);
void nlist_insert_after (nlist_t * list, nlist_node_t * pre, nlist_node_t * node);

// 从列表首部开始，遍历结点
#define nlist_for_each(node, list) for ((node) = (list)->first; (node); (node) = (node)->next)

#define noffset_in_parent(parent_type, node_name) \
    ((char *)(&(((parent_type *) 0))->node_name))

#define noffset_to_parent(node, parent_type, node_name) \
    (parent_type *)((char *) node - noffset_in_parent(parent_type, node_name))

#define nlist_entry(node, parent_type, node_name) \
    ((node) ? noffset_to_parent((node), parent_type, node_name) : (parent_type *) 0)

// 删除 list 链表中的 任一结点 node
nlist_node_t * nlist_remove (nlist_t * list, nlist_node_t * node);

// 删除 list 链表的首部结点
static inline nlist_node_t * nlist_remove_first (nlist_t * list){
    nlist_node_t * first = nlist_first(list);
    if (first){
        nlist_remove(list, first);
    }
    return first;
}

// 删除 list 链表的尾部结点
static inline nlist_node_t * nlist_remove_last (nlist_t * list){
    nlist_node_t *  last = nlist_last(list);
    if (last){
        nlist_remove(list, last);
    }
    return last;
}

#endif //NET_NLIST_H
