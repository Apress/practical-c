/* my_app.c
 *
 */
#include "mongoose.h"

/******************************************************************* ev_handler
*/
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct mbuf *io = &nc->recv_mbuf;

  switch (ev) {
    case MG_EV_RECV: // This event handler implements simple TCP echo server
      mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
      char *null_c = { '\0' };
      mbuf_append(io, null_c, 1);
      mg_printf_http_chunk(nc, "%s", io->buf);
      mg_send_http_chunk(nc, "", 0);
      mbuf_remove(io, io->len);      // Discard data from recv buffer
      break;
    default:
      break;
    }
  }

/************************************************************************* main
*/
int main(void) {
  struct mg_mgr mgr;
  
  mg_mgr_init(&mgr, NULL);
  mg_bind(&mgr, "1234", ev_handler);
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
    }
  mg_mgr_free(&mgr);
  return 0;
  }
