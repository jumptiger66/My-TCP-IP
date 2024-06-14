//
// Created by 孙昊 on 2024/6/7.
//

#include "nlist.h"

void nlist_init(nlist_t * list){
    list->first = list->last = (nlist_node_t *) 0;
    list->count = 0;
}

void nlist_insert_first (nlist_t * list, nlist_node_t * node){
    /*
     * func: 在链表头部插入一个结点
     */
    node->pre = (nlist_node_t *) 0; // 首部结点 pre 都指向 0；
    node->next = list->first; // 若此为第一个结点，first指向空，node也指向空，若不是第一个节点，则next指向第一个结点

    if (nlist_is_empty(list)) {
        list->first = list->last = node;
    } else {
        list->first->pre = node;
        list->first = node;
    }
    list->count++;
}

void nlist_insert_last (nlist_t * list, nlist_node_t * node){
    /*
     * func: 在链表尾部插入一个结点
     */
    node->next = (nlist_node_t *) 0;
    node->pre = list->last; // 若此为第一个结点，first指向空，node也指向空，若不是第一个节点，则next指向第一个结点

    if (nlist_is_empty(list)) {
        list->first = list->last = node;
    } else {
        list->last->next = node;
        list->last = node;
    }
    list->count++;
}

void nlist_insert_after (nlist_t * list, nlist_node_t * pre, nlist_node_t * node){
    if (nlist_is_empty(list) && !pre){
        nlist_insert_first(list, node);
        return;
    }

    node->next = pre->next;
    node->pre = pre;

    if (pre->next){
        pre->next->pre = node;
    }
    pre->next = node;

    if (list->last == pre){
        list->last = node;
    }

    list->count++;
};



nlist_node_t * nlist_remove (nlist_t * list, nlist_node_t * node){
    if (node == list->first){
        list->first = node->next;
    }

    if (node == list->last){
        list->last = node->pre;
    }

    if (node->pre){
        node->pre->next = node->next;
    }

    if (node->next){
        node->next->pre = node->pre;
    }

    node->pre = node->next = (nlist_node_t *) 0;

    list->count--;

    return node;
};
