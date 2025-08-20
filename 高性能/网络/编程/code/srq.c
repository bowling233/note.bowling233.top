/**
 * @file srq.c
 * @brief Example demonstrating SRQ functionality with RDMA.
 *
 * @details Both the client and server use an SRQ. A number of Queue Pairs (QPs) are
 * created (ctx.qp_count) and each QP uses the SRQ. The connection between the
 * client and server is established using the IP address details passed on the
 * command line. After the connection is established, the client starts
 * blasting sends to the server and stops when the maximum work requests
 * (ctx.max_wr) have been sent. When the server has received all the sends, it
 * performs a send to the client to tell it to continue. The process repeats
 * until the number of requested number of sends (ctx.msg_count) have been
 * performed.
 *
 * @note The executable can operate as either the client or server application. It
 * can be demonstrated on a simple fabric of two nodes with the server
 * application running on one node and the client application running on the
 * other. Each node must be configured to support IPoIB and the IB interface
 * (ex. ib0) must be assigned an IP Address. Finally, the fabric must be
 * initialized using OpenSM.
 *
 * @par Compile Command:
 * @code
 * gcc srq.c -o srq -libverbs -lrdmacm
 * @endcode
 *
 * @par Running the Example:
 * @par Server (-a is IP of local interface):
 * @code
 * ./srq -s -a 192.168.1.12
 * @endcode
 *
 * @par Client (-a is IP of remote interface):
 * @code
 * ./srq -a 192.168.1.12
 * @endcode
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <rdma/rdma_verbs.h>
#define VERB_ERR(verb, ret) \
  fprintf(stderr, "%s returned %d errno %d\n", verb, ret, errno)
/* Default parameters values */
#define DEFAULT_PORT "51216"
#define DEFAULT_MSG_COUNT 100
#define DEFAULT_MSG_LENGTH 100000
#define DEFAULT_QP_COUNT 4
#define DEFAULT_MAX_WR 64
/* Resources used in the example */
struct context
{
  /* User parameters */
  int server;
  char *server_name;
  char *server_port;
  int msg_count;
  int msg_length;
  int qp_count;
  int max_wr;
  /* Resources */
  struct rdma_cm_id *srq_id;
  struct rdma_cm_id *listen_id;
  struct rdma_cm_id **conn_id;
  struct ibv_mr *send_mr;
  struct ibv_mr *recv_mr;
  struct ibv_srq *srq;
  struct ibv_cq *srq_cq;
  struct ibv_comp_channel *srq_cq_channel;
  char *send_buf;
  char *recv_buf;
};
/*
 * Function: init_resources
 *
 * Input:
 * ctx The context object
 * rai The RDMA address info for the connection
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * This function initializes resources that are common to both the client
 * and server functionality.
 * It creates our SRQ, registers memory regions, posts receive buffers
 * and creates a single completion queue that will be used for the receive
 * queue on each queue pair.
 */
int init_resources(struct context *ctx, struct rdma_addrinfo *rai)
{
  int ret, i;
  struct rdma_cm_id *id;
  /* Create an ID used for creating/accessing our SRQ */
  ret = rdma_create_id(NULL, &ctx->srq_id, NULL, RDMA_PS_TCP);
  if (ret)
  {
    VERB_ERR("rdma_create_id", ret);
    return ret;
  }
  /* We need to bind the ID to a particular RDMA device
   * This is done by resolving the address or binding to the address */
  if (ctx->server == 0)
  {
    ret = rdma_resolve_addr(ctx->srq_id, NULL, rai->ai_dst_addr, 1000);
    if (ret)
    {
      VERB_ERR("rdma_resolve_addr", ret);
      return ret;
    }
  }
  else
  {
    ret = rdma_bind_addr(ctx->srq_id, rai->ai_src_addr);
    if (ret)
    {
      VERB_ERR("rdma_bind_addr", ret);
      return ret;
    }
  }
  /* Create the memory regions being used in this example */
  ctx->recv_mr = rdma_reg_msgs(ctx->srq_id, ctx->recv_buf, ctx->msg_length);
  if (!ctx->recv_mr)
  {
    VERB_ERR("rdma_reg_msgs", -1);
    return -1;
  }
  ctx->send_mr = rdma_reg_msgs(ctx->srq_id, ctx->send_buf, ctx->msg_length);
  if (!ctx->send_mr)
  {
    VERB_ERR("rdma_reg_msgs", -1);
    return -1;
  }
  /* Create our shared receive queue */
  struct ibv_srq_init_attr srq_attr;
  memset(&srq_attr, 0, sizeof(srq_attr));
  srq_attr.attr.max_wr = ctx->max_wr;
  srq_attr.attr.max_sge = 1;
  ret = rdma_create_srq(ctx->srq_id, NULL, &srq_attr);
  if (ret)
  {
    VERB_ERR("rdma_create_srq", ret);
    return -1;
  }
  /* Save the SRQ in our context so we can assign it to other QPs later */
  ctx->srq = ctx->srq_id->srq;
  /* Post our receive buffers on the SRQ */
  for (i = 0; i < ctx->max_wr; i++)
  {
    ret = rdma_post_recv(ctx->srq_id, NULL, ctx->recv_buf, ctx->msg_length,
                         ctx->recv_mr);
    if (ret)
    {
      VERB_ERR("rdma_post_recv", ret);
      return ret;
    }
  }
  /* Create a completion channel to use with the SRQ CQ */
  ctx->srq_cq_channel = ibv_create_comp_channel(ctx->srq_id->verbs);
  if (!ctx->srq_cq_channel)
  {
    VERB_ERR("ibv_create_comp_channel", -1);
    return -1;
  }
  /* Create a CQ to use for all connections (QPs) that use the SRQ */
  ctx->srq_cq = ibv_create_cq(ctx->srq_id->verbs, ctx->max_wr, NULL,
                              ctx->srq_cq_channel, 0);
  if (!ctx->srq_cq)
  {
    VERB_ERR("ibv_create_cq", -1);
    return -1;
  }
  /* Make sure that we get notified on the first completion */
  ret = ibv_req_notify_cq(ctx->srq_cq, 0);
  if (ret)
  {
    VERB_ERR("ibv_req_notify_cq", ret);
    return ret;
  }
  return 0;
}
/*
 * Function: destroy_resources
 *
 * Input:
 * ctx The context object
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * This function cleans up resources used by the application
 */
void destroy_resources(struct context *ctx)
{
  int i;
  if (ctx->conn_id)
  {
    for (i = 0; i < ctx->qp_count; i++)
    {
      if (ctx->conn_id[i])
      {
        if (ctx->conn_id[i]->qp &&
            ctx->conn_id[i]->qp->state == IBV_QPS_RTS)
        {
          rdma_disconnect(ctx->conn_id[i]);
        }
        rdma_destroy_qp(ctx->conn_id[i]);
        rdma_destroy_id(ctx->conn_id[i]);
      }
    }
    free(ctx->conn_id);
  }
  if (ctx->recv_mr)
    rdma_dereg_mr(ctx->recv_mr);
  if (ctx->send_mr)
    rdma_dereg_mr(ctx->send_mr);
  if (ctx->recv_buf)
    free(ctx->recv_buf);
  if (ctx->send_buf)
    free(ctx->send_buf);
  if (ctx->srq_cq)
    ibv_destroy_cq(ctx->srq_cq);
  if (ctx->srq_cq_channel)
    ibv_destroy_comp_channel(ctx->srq_cq_channel);
  if (ctx->srq_id)
  {
    rdma_destroy_srq(ctx->srq_id);
    rdma_destroy_id(ctx->srq_id);
  }
}
/*
 * Function: await_completion
 *
 * Input:
 * ctx The context object
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * Waits for a completion on the SRQ CQ
 *
 */
int await_completion(struct context *ctx)
{
  int ret;
  struct ibv_cq *ev_cq;
  void *ev_ctx;
  /* Wait for a CQ event to arrive on the channel */
  ret = ibv_get_cq_event(ctx->srq_cq_channel, &ev_cq, &ev_ctx);
  if (ret)
  {
    VERB_ERR("ibv_get_cq_event", ret);
    return ret;
  }
  ibv_ack_cq_events(ev_cq,
                    1);
  /* Reload the event notification */
  ret = ibv_req_notify_cq(ctx->srq_cq,
                          0);
  if (ret)
  {
    VERB_ERR("ibv_req_notify_cq", ret);
    return ret;
  }
  return 0;
}
/*
 * Function: run_server
 *
 * Input:
 * ctx The context object
 * rai The RDMA address info for the connection
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * Executes the server side of the example
 */
int run_server(struct context *ctx, struct rdma_addrinfo *rai)
{
  int ret, i;
  uint64_t send_count =
      0;
  uint64_t recv_count =
      0;
  struct ibv_wc wc;
  struct ibv_qp_init_attr qp_attr;
  ret = init_resources(ctx, rai);
  if (ret)
  {
    printf("init_resources returned %d\n", ret);
    return ret;
  }
  /* Use the srq_id as the listen_id since it is already setup */
  ctx->listen_id = ctx->srq_id;
  ret = rdma_listen(ctx->listen_id,
                    4);
  if (ret)
  {
    VERB_ERR("rdma_listen", ret);
    return ret;
  }
  printf("waiting for connection from client...\n");
  for (i =
           0;
       i < ctx->qp_count; i++)
  {
    ret = rdma_get_request(ctx->listen_id, &ctx->conn_id[i]);
    if (ret)
    {
      VERB_ERR("rdma_get_request", ret);
      return ret;
    }
    /* Create the queue pair */
    memset(&qp_attr,
           0, sizeof(qp_attr));
    qp_attr.qp_context = ctx;
    qp_attr.qp_type = IBV_QPT_RC;
    qp_attr.cap.max_send_wr = ctx->max_wr;
    qp_attr.cap.max_recv_wr = ctx->max_wr;
    qp_attr.cap.max_send_sge =
        1;
    qp_attr.cap.max_recv_sge =
        1;
    qp_attr.cap.max_inline_data =
        0;
    qp_attr.recv_cq = ctx->srq_cq;
    qp_attr.srq = ctx->srq;
    qp_attr.sq_sig_all =
        0;
    ret = rdma_create_qp(ctx->conn_id[i], NULL, &qp_attr);
    if (ret)
    {
      VERB_ERR("rdma_create_qp", ret);
      return ret;
    }
    /* Set the new connection to use our SRQ */
    ctx->conn_id[i]->srq = ctx->srq;
    ret = rdma_accept(ctx->conn_id[i], NULL);
    if (ret)
    {
      VERB_ERR("rdma_accept", ret);
      return ret;
    }
  }
  while (recv_count < ctx->msg_count)
  {
    i =
        0;
    while (i < ctx->max_wr && recv_count < ctx->msg_count)
    {
      int ne;
      ret = await_completion(ctx);
      if (ret)
      {
        printf("await_completion %d\n", ret);
        return ret;
      }
      do
      {
        ne = ibv_poll_cq(ctx->srq_cq, 1, &wc);
        if (ne < 0)
        {
          VERB_ERR("ibv_poll_cq", ne);
          return ne;
        }
        else if (ne == 0)
          break;
        if (wc.status != IBV_WC_SUCCESS)
        {
          printf("work completion status %s\n",
                 ibv_wc_status_str(wc.status));
          return -1;
        }
        recv_count++;
        printf("recv count: %d, qp_num: %d\n", recv_count, wc.qp_num);
        ret = rdma_post_recv(ctx->srq_id, (void *)wc.wr_id,
                             ctx->recv_buf, ctx->msg_length,
                             ctx->recv_mr);
        if (ret)
        {
          VERB_ERR("rdma_post_recv", ret);
          return ret;
        }
        i++;
      } while (ne);
    }
    ret = rdma_post_send(ctx->conn_id[0], NULL, ctx->send_buf,
                         ctx->msg_length, ctx->send_mr, IBV_SEND_SIGNALED);
    if (ret)
    {
      VERB_ERR("rdma_post_send", ret);
      return ret;
    }
    ret = rdma_get_send_comp(ctx->conn_id[0], &wc);
    if (ret <= 0)
    {
      VERB_ERR("rdma_get_send_comp", ret);
      return -1;
    }
    send_count++;
    printf("send count: %d\n", send_count);
  }
  return 0;
}
/*
 * Function: run_client
 *
 * Input:
 * ctx The context object
 * rai The RDMA address info for the connection
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * Executes the client side of the example
 */
int run_client(struct context *ctx, struct rdma_addrinfo *rai)
{
  int ret, i, ne;
  uint64_t send_count = 0;
  uint64_t recv_count = 0;
  struct ibv_wc wc;
  struct ibv_qp_init_attr attr;
  ret = init_resources(ctx, rai);
  if (ret)
  {
    printf("init_resources returned %d\n", ret);
    return ret;
  }
  for (i = 0; i < ctx->qp_count; i++)
  {
    memset(&attr, 0, sizeof(attr));
    attr.qp_context = ctx;
    attr.cap.max_send_wr = ctx->max_wr;
    attr.cap.max_recv_wr = ctx->max_wr;
    attr.cap.max_send_sge = 1;
    attr.cap.max_recv_sge = 1;
    attr.cap.max_inline_data = 0;
    attr.recv_cq = ctx->srq_cq;
    attr.srq = ctx->srq;
    attr.sq_sig_all = 0;
    ret = rdma_create_ep(&ctx->conn_id[i], rai, NULL, &attr);
    if (ret)
    {
      VERB_ERR("rdma_create_ep", ret);
      return ret;
    }
    ret = rdma_connect(ctx->conn_id[i], NULL);
    if (ret)
    {
      VERB_ERR("rdma_connect", ret);
      return ret;
    }
  }
  while (send_count < ctx->msg_count)
  {
    for (i = 0; i < ctx->max_wr && send_count < ctx->msg_count; i++)
    {
      /* perform our send to the server */
      ret = rdma_post_send(ctx->conn_id[i % ctx->qp_count], NULL,
                           ctx->send_buf, ctx->msg_length, ctx->send_mr,
                           IBV_SEND_SIGNALED);
      if (ret)
      {
        VERB_ERR("rdma_post_send", ret);
        return ret;
      }
      ret = rdma_get_send_comp(ctx->conn_id[i % ctx->qp_count], &wc);
      if (ret <= 0)
      {
        VERB_ERR("rdma_get_send_comp", ret);
        return ret;
      }
      send_count++;
      printf("send count: %d, qp_num: %d\n", send_count, wc.qp_num);
    }
    /* wait for a recv indicating that all buffers were processed */
    ret = await_completion(ctx);
    if (ret)
    {
      VERB_ERR("await_completion", ret);
      return ret;
    }
    do
    {
      ne = ibv_poll_cq(ctx->srq_cq, 1, &wc);
      if (ne < 0)
      {
        VERB_ERR("ibv_poll_cq", ne);
        return ne;
      }
      else if (ne == 0)
        break;
      if (wc.status != IBV_WC_SUCCESS)
      {
        printf("work completion status %s\n",
               ibv_wc_status_str(wc.status));
        return -1;
      }
      recv_count++;
      printf("recv count: %d\n", recv_count);
      ret = rdma_post_recv(ctx->srq_id, (void *)wc.wr_id,
                           ctx->recv_buf, ctx->msg_length, ctx->recv_mr);
      if (ret)
      {
        VERB_ERR("rdma_post_recv", ret);
        return ret;
      }
    } while (ne);
  }
  return ret;
}
/*
 * Function: main
 *
 * Input:
 * argc The number of arguments
 * argv Command line arguments
 *
 * Output:
 * none
 *
 * Returns:
 * 0 on success, non-zero on failure
 *
 * Description:
 * Main program to demonstrate SRQ functionality.
 * Both the client and server use an SRQ. ctx.qp_count number of QPs are
 * created and each one of them uses the SRQ. After the connection, the
 * client starts blasting sends to the server upto ctx.max_wr. When the
 * server has received all the sends, it performs a send to the client to
 * tell it that it can continue. Process repeats until ctx.msg_count
 * sends have been performed.
 */
int main(int argc, char **argv)
{
  int ret, op;
  struct context ctx;
  struct rdma_addrinfo *rai, hints;
  memset(&ctx, 0, sizeof(ctx));
  memset(&hints, 0, sizeof(hints));
  ctx.server = 0;
  ctx.server_port = DEFAULT_PORT;
  ctx.msg_count = DEFAULT_MSG_COUNT;
  ctx.msg_length = DEFAULT_MSG_LENGTH;
  ctx.qp_count = DEFAULT_QP_COUNT;
  ctx.max_wr = DEFAULT_MAX_WR;
  /* Read options from command line */
  while ((op = getopt(argc, argv, "sa:p:c:l:q:w:")) != -1)
  {
    switch (op)
    {
    case 's':
      ctx.server = 1;
      break;
    case 'a':
      ctx.server_name = optarg;
      break;
    case 'p':
      ctx.server_port = optarg;
      break;
    case 'c':
      ctx.msg_count = atoi(optarg);
      break;
    case 'l':
      ctx.msg_length = atoi(optarg);
      break;
    case 'q':
      ctx.qp_count = atoi(optarg);
      break;
    case 'w':
      ctx.max_wr = atoi(optarg);
      break;
    default:
      printf("usage: %s -a server_address\n", argv[0]);
      printf("\t[-s server mode]\n");
      printf("\t[-p port_number]\n");
      printf("\t[-c msg_count]\n");
      printf("\t[-l msg_length]\n");
      printf("\t[-q qp_count]\n");
      printf("\t[-w max_wr]\n");
      exit(1);
    }
  }
  if (ctx.server_name == NULL)
  {
    printf("server address required (use -a)!\n");
    exit(1);
  }
  hints.ai_port_space = RDMA_PS_TCP;
  if (ctx.server == 1)
    hints.ai_flags = RAI_PASSIVE; /* this makes it a server */
  ret = rdma_getaddrinfo(ctx.server_name, ctx.server_port, &hints, &rai);
  if (ret)
  {
    VERB_ERR("rdma_getaddrinfo", ret);
    exit(1);
  }
  /* allocate memory for our QPs and send/recv buffers */
  ctx.conn_id = (struct rdma_cm_id **)calloc(ctx.qp_count,
                                             sizeof(struct rdma_cm_id *));
  memset(ctx.conn_id, 0, sizeof(ctx.conn_id));
  ctx.send_buf = (char *)malloc(ctx.msg_length);
  memset(ctx.send_buf, 0, ctx.msg_length);
  ctx.recv_buf = (char *)malloc(ctx.msg_length);
  memset(ctx.recv_buf, 0, ctx.msg_length);
  if (ctx.server)
    ret = run_server(&ctx, rai);
  else
    ret = run_client(&ctx, rai);
  destroy_resources(&ctx);
  free(rai);
  return ret;
}
