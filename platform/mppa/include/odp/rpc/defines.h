#ifndef __ODP_RPC_DEFINES_H__
#define __ODP_RPC_DEFINES_H__

#define _ODP_RPC_STRINGIFY(x) #x
#define _ODP_RPC_TOSTRING(x) _ODP_RPC_STRINGIFY(x)

#define ODP_RPC_CHECK_STRUCT_SIZE(sname) _Static_assert(sizeof(sname) == sizeof(odp_rpc_inl_data_t),		\
							"ODP_RPC_CMD_" _ODP_RPC_TOSTRING(sname) "__SIZE_ERROR")
#define ODP_RPC_TIMEOUT_1S ((uint64_t)__bsp_frequency)


#define RPC_BASE_RX 10
#define RPC_MAX_PAYLOAD 168 * 8 /* max payload in bytes */

#endif /* __ODP_RPC_DEFINES_H__ */
