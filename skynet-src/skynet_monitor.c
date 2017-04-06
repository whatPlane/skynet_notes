#include "skynet.h"

#include "skynet_monitor.h"
#include "skynet_server.h"
#include "skynet.h"
#include "atomic.h"

#include <stdlib.h>
#include <string.h>

/// 监视的结构
struct skynet_monitor {
	int version; ///< 版本
	int check_version;///< 检查版本
	uint32_t source;///< 来源
	uint32_t destination; ///< 目的
};

/// 新建监视
struct skynet_monitor * 
skynet_monitor_new() {
	struct skynet_monitor * ret = skynet_malloc(sizeof(*ret));// 给结构分配内存
	memset(ret, 0, sizeof(*ret));
	return ret;
}

void 
skynet_monitor_delete(struct skynet_monitor *sm) {
	skynet_free(sm); // 清空结构
}

/// 触发监控
void 
skynet_monitor_trigger(struct skynet_monitor *sm, uint32_t source, uint32_t destination) {
	sm->source = source;// 来源
	sm->destination = destination;// 目的
	ATOM_INC(&sm->version); // 版本 +1
}

/// 检查监控
void 
skynet_monitor_check(struct skynet_monitor *sm) {
	if (sm->version == sm->check_version) {// 如果版本等于检查版本 skynet_monitor_trigger时设置了version加1 如果相等则进入了无限循环
		if (sm->destination) {// 目的是否存在
			skynet_context_endless(sm->destination);
			skynet_error(NULL, "A message from [ :%08x ] to [ :%08x ] maybe in an endless loop (version = %d)", sm->source , sm->destination, sm->version);
		}
	} else {// 否则检查版本等于版本
		sm->check_version = sm->version; 
	}
}
