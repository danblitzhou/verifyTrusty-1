#include <ipc_helper.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <seahorn/seahorn.h>
#include <uapi/err.h>

void sea_dispatch_event(const uevent_t* ev) {
    sassert(ev);

    if (ev->event == IPC_HANDLE_POLL_NONE) {
        return;
    }

    struct ipc_context* context = ev->cookie;
    sassert(context);
    sassert(context->evt_handler);
    sassert(context->handle == ev->handle);

    context->evt_handler(context, ev);
}

static void sea_ipc_disconnect_handler(struct ipc_channel_context *context) {
  if (context)
    free(context);
}

static int sea_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                               size_t msg_size) {
  sassert(msg_size <= MSG_BUF_MAX_SIZE);
  struct iovec iov = {
      .iov_base = msg,
      .iov_len = msg_size,
  };
  ipc_msg_t i_msg = {
      .iov = &iov,
      .num_iov = 1,
  };
  int rc = send_msg(context->common.handle, &i_msg);
  if (rc < 0) {
    return rc;
  }
  return NO_ERROR;
}

/*
 * directly return a channel context given uuid and chan handle
 */
static struct ipc_channel_context *
sea_channel_connect(struct ipc_port_context *parent_ctx,
                    const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sea_ipc_msg_handler;
  return pctx;
}

/*
 * constant variable of ipc_port_context
 */
const static struct ipc_port_context ctx = {
      .ops = {.on_connect = sea_channel_connect},
  };

struct ipc_port_context* create_port_context(){
    struct ipc_port_context* pctx = malloc(sizeof(struct ipc_port_context));
    * pctx = ctx;
  return pctx;
}