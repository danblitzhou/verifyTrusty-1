#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include <ipc.h>

#include "tipc_limits.h"
#include <interface/storage/storage.h>

#include <handle_table.h>
#include <seahorn/seahorn.h>
#include <ipc_helper.h>

/**
   verification entry point
 */
int main(void) {
  handle_table_init(INVALID_IPC_HANDLE, INVALID_IPC_HANDLE, INVALID_IPC_HANDLE);
  struct ipc_port_context* ctx = create_port_context();
  int rc =
      ipc_port_create(ctx, STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  if (rc < 0) {
    return rc;
  }

  // ipc_loop();
  // first event should be port event
  uevent_t event1;
  event1.handle = INVALID_IPC_HANDLE;
  event1.event = 0;
  event1.cookie = NULL;
  rc = wait_any(&event1, INFINITE_TIME);
  if (rc < 0) {
    return rc;
  }
  if (rc == NO_ERROR) { /* got an event */
    sea_dispatch_event(&event1);
  }
  // get second event, could be either port or channel
  uevent_t event2;
  event2.handle = INVALID_IPC_HANDLE;
  event2.event = 0;
  event2.cookie = NULL;
  rc = wait_any(&event2, INFINITE_TIME);
  if (rc < 0) {
    return rc;
  }
  if (rc == NO_ERROR) { /* got an event */
    sea_dispatch_event(&event2);
  }

  ipc_port_destroy(ctx);

  return 0;
}
