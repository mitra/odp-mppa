#ifndef __ODP_RPC_BAS_H__
#define __ODP_RPC_BAS_H__

#include <odp/rpc/defines.h>

#define ODP_RPC_CMD_LIST_BAS														\
	ODP_RPC_CMD_BAS_INVL = 0 /**< BASE: Invalid command. Skip */,					\
	ODP_RPC_CMD_BAS_PING     /**< BASE: Ping command. server sends back ack = 0 */

#define ODP_RPC_CMD_NAMES_BAS			\
	"INVALID",							\
		"PING"

#define ODP_RPC_ACK_LIST_BAS

#endif /* __ODP_RPC_BAS_H__ */
