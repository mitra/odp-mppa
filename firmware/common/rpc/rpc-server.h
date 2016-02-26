#ifndef RPC_SERVER__H
#define RPC_SERVER__H

/**
 * Setup the RPC server.
 * If calls from an IODDR (RM0), spawns a thread on the IOETH
 * to handle RPC queries.
 * If called from an IOETH, simply initializes the buffers and NoC structures
 */
int odp_rpc_server_start(void);

/**
 * RPC server main thread.
 * If #odp_rpc_server_start was called from an IODDR0, this function does
 * not need to be called.
 * If it was called from an IOETH, this one should be called.
 * This function never returns unless there is an error
 */
int odp_rpc_server_thread();

#endif /* RPC_SERVER__H */
