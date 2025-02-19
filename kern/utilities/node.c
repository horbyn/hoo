#include "node.h"
#include "kern/panic.h"

/**
 * @brief 填充 node 对象
 * 
 * @param node node 对象
 * @param data 被 node 封装的对象
 * @param next 下一个 node
 */
void
node_set(node_t *node, void *data, node_t *next) {
    if (node == null)    panic("node_set(): null pointer");
    if (data == null)    node->data_ = null;
    else    node->data_ = data;
    if (next == null)    node->next_ = null;
    else    node->next_ = next;
}
