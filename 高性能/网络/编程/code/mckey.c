/**
 * @file mckey.c
 * @brief Multicast Code example
 *
 * This code example for Multicast, uses RDMA-CM and VPI (and hence can be run
 * both over IB and over LLE).
 *
 * @details
 *
 * BUILD COMMAND:
 * @code
 * gcc -g -Wall -D_GNU_SOURCE -g -O2 -o examples/mckey examples/mckey.c -libverbs -lrdmacm
 * @endcode
 *
 * @note
 *
 * 1. In order to run the multicast example on either IB or LLE, no change is
 * needed to the test's code. However if RDMA_CM is used, it is required that
 * the network interface will be configured and up (whether it is used over RoCE
 * or over IB).
 *
 * 2. For the IB case, a join operation is involved, yet it is performed by the
 * rdma_cm kernel code.
 *
 * 3. For the LLE case, no join is required. All MGIDs are resolved into MACs
 * at the host.
 *
 * 4. To inform the multicast example which port to use, you need to specify
 * "-b <IP address>” to bind to the desired device port.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <byteswap.h>
#include <unistd.h>
#include <getopt.h>
#include <rdma/rdma_cma.h>
/**
 * Structure to hold RDMA connection information.
 */
struct cmatest_node
{
  int id;                    /**< Identifier of the connection. */
  struct rdma_cm_id *cma_id; /**< Pointer to the RDMA connection identifier. */
  int connected;             /**< Connection status. */
  struct ibv_pd *pd;         /**< Pointer to the protection domain. */
  struct ibv_cq *cq;         /**< Pointer to the completion queue. */
  struct ibv_mr *mr;         /**< Pointer to the memory region. */
  struct ibv_ah *ah;         /**< Pointer to the address handle. */
  uint32_t remote_qpn;       /**< Remote QP number. */
  uint32_t remote_qkey;      /**< Remote QP key. */
  void *mem;                 /**< Pointer to the memory. */
};
/**
 * Structure to hold RDMA connection information.
 */
struct cmatest
{
  struct rdma_event_channel *channel; /**< Pointer to the RDMA event channel. */
  struct cmatest_node *nodes;         /**< Pointer to the list of cmatest_node structures. */
  int conn_index;                     /**< Index of the current connection. */
  int connects_left;                  /**< Number of connections left to establish. */
  struct sockaddr_in6 dst_in;         /**< Destination address in sockaddr_in6 format. */
  struct sockaddr *dst_addr;          /**< Pointer to the destination address. */
  struct sockaddr_in6 src_in;         /**< Source address in sockaddr_in6 format. */
  struct sockaddr *src_addr;          /**< Pointer to the source address. */
};
/**
 * @defgroup args
 * @{
 */
static struct cmatest test;
static int connections = 1;
static int message_size = 100;
static int message_count = 10;
static int is_sender;
static int unmapped_addr;
static char *dst_addr;
static char *src_addr;
static enum rdma_port_space port_space = RDMA_PS_UDP;
/**
 * @}
 */
static int create_message(struct cmatest_node *node)
{
  if (!message_size)
    message_count = 0;
  if (!message_count)
    return 0;
  node->mem = malloc(message_size + sizeof(struct ibv_grh));
  if (!node->mem)
  {
    printf("failed message allocation\n");
    return -1;
  }
  node->mr = ibv_reg_mr(node->pd, node->mem, message_size + sizeof(struct ibv_grh),
                        IBV_ACCESS_LOCAL_WRITE);
  if (!node->mr)
  {
    printf("failed to reg MR\n");
    goto err;
  }
  return 0;
err:
  free(node->mem);
  return -1;
}
static int verify_test_params(struct cmatest_node *node)
{
  struct ibv_port_attr port_attr;
  int ret;
  ret = ibv_query_port(node->cma_id->verbs, node->cma_id->port_num, &port_attr);
  if (ret)
    return ret;
  if (message_count && message_size > (1 << (port_attr.active_mtu + 7)))
  {
    printf("mckey: message_size %d is larger than active mtu %d\n", message_size, 1 << (port_attr.active_mtu + 7));
    return -EINVAL;
  }
  return 0;
}
static int init_node(struct cmatest_node *node)
{
  struct ibv_qp_init_attr init_qp_attr;
  int cqe, ret;
  node->pd = ibv_alloc_pd(node->cma_id->verbs);
  if (!node->pd)
  {
    ret = -ENOMEM;
    printf("mckey: unable to allocate PD\n");
    goto out;
  }
  cqe = message_count ? message_count * 2 : 2;
  node->cq = ibv_create_cq(node->cma_id->verbs, cqe, node, 0, 0);
  if (!node->cq)
  {
    ret = -ENOMEM;
    printf("mckey: unable to create CQ\n");
    goto out;
  }
  memset(&init_qp_attr, 0, sizeof init_qp_attr);
  init_qp_attr.cap.max_send_wr = message_count ? message_count : 1;
  init_qp_attr.cap.max_recv_wr = message_count ? message_count : 1;
  init_qp_attr.cap.max_send_sge = 1;
  init_qp_attr.cap.max_recv_sge = 1;
  init_qp_attr.qp_context = node;
  init_qp_attr.sq_sig_all = 0;
  init_qp_attr.qp_type = IBV_QPT_UD;
  init_qp_attr.send_cq = node->cq;
  init_qp_attr.recv_cq = node->cq;
  ret = rdma_create_qp(node->cma_id, node->pd, &init_qp_attr);
  if (ret)
  {
    printf("mckey: unable to create QP: %d\n", ret);
    goto out;
  }
  ret = create_message(node);
  if (ret)
  {
    printf("mckey: failed to create messages: %d\n", ret);
    goto out;
  }
out:
  return ret;
}
static int post_recvs(struct cmatest_node *node)
{
  struct ibv_recv_wr recv_wr, *recv_failure;
  struct ibv_sge sge;
  int i, ret = 0;
  if (!message_count)
    return 0;
  recv_wr.next = NULL;
  recv_wr.sg_list = &sge;
  recv_wr.num_sge = 1;
  recv_wr.wr_id = (uintptr_t)node;
  sge.length = message_size + sizeof(struct ibv_grh);
  sge.lkey = node->mr->lkey;
  sge.addr = (uintptr_t)node->mem;
  for (i = 0; i < message_count && !ret; i++)
  {
    ret = ibv_post_recv(node->cma_id->qp, &recv_wr, &recv_failure);
    if (ret)
    {
      printf("failed to post receives: %d\n", ret);
      break;
    }
  }
  return ret;
}
static int post_sends(struct cmatest_node *node, int signal_flag)
{
  struct ibv_send_wr send_wr, *bad_send_wr;
  struct ibv_sge sge;
  int i, ret = 0;
  if (!node->connected || !message_count)
    return 0;
  send_wr.next = NULL;
  send_wr.sg_list = &sge;
  send_wr.num_sge = 1;
  send_wr.opcode = IBV_WR_SEND_WITH_IMM;
  send_wr.send_flags = signal_flag;
  send_wr.wr_id = (unsigned long)node;
  send_wr.imm_data = htonl(node->cma_id->qp->qp_num);
  send_wr.wr.ud.ah = node->ah;
  send_wr.wr.ud.remote_qpn = node->remote_qpn;
  send_wr.wr.ud.remote_qkey = node->remote_qkey;
  sge.length = message_size;
  sge.lkey = node->mr->lkey;
  sge.addr = (uintptr_t)node->mem;
  for (i = 0; i < message_count && !ret; i++)
  {
    ret = ibv_post_send(node->cma_id->qp, &send_wr, &bad_send_wr);
    if (ret)
      printf("failed to post sends: %d\n", ret);
  }
  return ret;
}
static void connect_error(void)
{
  test.connects_left--;
}
static int addr_handler(struct cmatest_node *node)
{
  int ret;
  ret = verify_test_params(node);
  if (ret)
    goto err;
  ret = init_node(node);
  if (ret)
    goto err;
  if (!is_sender)
  {
    ret = post_recvs(node);
    if (ret)
      goto err;
  }
  ret = rdma_join_multicast(node->cma_id, test.dst_addr, node);
  if (ret)
  {
    printf("mckey: failure joining: %d\n", ret);
    goto err;
  }
  return 0;
err:
  connect_error();
  return ret;
}
static int join_handler(struct cmatest_node *node, struct rdma_ud_param *param)
{
  char buf[40];
  inet_ntop(AF_INET6, param->ah_attr.grh.dgid.raw, buf, 40);
  printf("mckey: joined dgid: %s\n", buf);
  node->remote_qpn = param->qp_num;
  node->remote_qkey = param->qkey;
  node->ah = ibv_create_ah(node->pd, &param->ah_attr);
  if (!node->ah)
  {
    printf("mckey: failure creating address handle\n");
    goto err;
  }
  node->connected = 1;
  test.connects_left--;
  return 0;
err:
  connect_error();
  return -1;
}
/**
 * @brief Handles the CMA events.
 *
 * @param[in] cma_id
 * @param[in] event
 * @return int
 */
static int cma_handler(struct rdma_cm_id *cma_id, struct rdma_cm_event *event)
{
  int ret = 0;
  switch (event->event)
  {
  case RDMA_CM_EVENT_ADDR_RESOLVED:
    ret = addr_handler(cma_id->context);
    break;
  case RDMA_CM_EVENT_MULTICAST_JOIN:
    ret = join_handler(cma_id->context, &event->param.ud);
    break;
  case RDMA_CM_EVENT_ADDR_ERROR:
  case RDMA_CM_EVENT_ROUTE_ERROR:
  case RDMA_CM_EVENT_MULTICAST_ERROR:
    printf("mckey: event: %s, error: %d\n", rdma_event_str(event->event), event->status);
    connect_error();
    ret = event->status;
    break;
  case RDMA_CM_EVENT_DEVICE_REMOVAL:
    /* Cleanup will occur after test completes. */
    break;
  default:
    break;
  }
  return ret;
}
static void destroy_node(struct cmatest_node *node)
{
  if (!node->cma_id)
    return;
  if (node->ah)
    ibv_destroy_ah(node->ah);
  if (node->cma_id->qp)
    rdma_destroy_qp(node->cma_id);
  if (node->cq)
    ibv_destroy_cq(node->cq);
  if (node->mem)
  {
    ibv_dereg_mr(node->mr);
    free(node->mem);
  }
  if (node->pd)
    ibv_dealloc_pd(node->pd);
  /* Destroy the RDMA ID after all device resources */
  rdma_destroy_id(node->cma_id);
}
/**
 * @brief
 *
 * Allocates memory for the test nodes.
 *
 * @return int
 */
static int alloc_nodes(void)
{
  int ret, i;
  test.nodes = malloc(sizeof *test.nodes * connections);
  if (!test.nodes)
  {
    printf("mckey: unable to allocate memory for test nodes\n");
    return -ENOMEM;
  }
  memset(test.nodes, 0, sizeof *test.nodes * connections);
  for (i = 0; i < connections; i++)
  {
    test.nodes[i].id = i;
    ret = rdma_create_id(test.channel, &test.nodes[i].cma_id, &test.nodes[i], port_space);
    if (ret)
      goto err;
  }
  return 0;
err:
  while (--i >= 0)
    rdma_destroy_id(test.nodes[i].cma_id);
  free(test.nodes);
  return ret;
}
static void destroy_nodes(void)
{
  int i;
  for (i = 0; i < connections; i++)
    destroy_node(&test.nodes[i]);
  free(test.nodes);
}
static int poll_cqs(void)
{
  struct ibv_wc wc[8];
  int done, i, ret;
  for (i = 0; i < connections; i++)
  {
    if (!test.nodes[i].connected)
      continue;
    for (done = 0; done < message_count; done += ret)
    {
      ret = ibv_poll_cq(test.nodes[i].cq, 8, wc);
      if (ret < 0)
      {
        printf("mckey: failed polling CQ: %d\n", ret);
        return ret;
      }
    }
  }
  return 0;
}
/**
 * @brief Connects to the events.
 *
 * @return int
 */
static int connect_events(void)
{
  struct rdma_cm_event *event;
  int ret = 0;
  while (test.connects_left && !ret)
  {
    ret = rdma_get_cm_event(test.channel, &event);
    if (!ret)
    {
      ret = cma_handler(event->id, event);
      rdma_ack_cm_event(event);
    }
  }
  return ret;
}
/**
 * @brief Get the sockaddr object
 *
 * @param[in] dst The destination address
 * @param[out] addr The address object
 * @return int
 */
static int get_addr(char *dst, struct sockaddr *addr)
{
  struct addrinfo *res;
  int ret;
  ret = getaddrinfo(dst, NULL, NULL, &res);
  if (ret)
  {
    printf("getaddrinfo failed - invalid hostname or IP address\n");
    return ret;
  }
  memcpy(addr, res->ai_addr, res->ai_addrlen);
  freeaddrinfo(res);
  return ret;
}
/**
 *  - Get source (if provided for binding) and destination addresses - convert the
 *    input addresses to socket presentation.
 *
 *  - Joining:
 *    - For all connections:
 *      - if source address is specifically provided, then bind the rdma_cm object to the
 *        corresponding network interface. (Associates a source address with an rdma_cm
 *        identifier).
 *      - if unmapped MC address with bind address provided, check the remote address and
 *        then bind.
 *
 *  - Poll on all the connection events and wait that all rdma_cm objects joined the MC
 *  group.
 *
 *  - Send & receive:
 *    - If sender: send the messages to all connection nodes (function “post_sends”).
 *    - If receiver: poll the completion queue (function “poll_cqs”) till messages arrival.
 *  - On ending - release network resources (per all connections: leaves the multicast group and detaches
its associated QP from the group)
 *
 * @return int
 */
static int run(void)
{
  int i, ret;
  printf("mckey: starting %s\n", is_sender ? "client" : "server");
  if (src_addr)
  {
    ret = get_addr(src_addr, (struct sockaddr *)&test.src_in);
    if (ret)
      return ret;
  }
  ret = get_addr(dst_addr, (struct sockaddr *)&test.dst_in);
  if (ret)
    return ret;
  printf("mckey: joining\n");
  for (i = 0; i < connections; i++)
  {
    if (src_addr)
    {
      ret = rdma_bind_addr(test.nodes[i].cma_id, test.src_addr);
      if (ret)
      {
        printf("mckey: addr bind failure: %d\n", ret);
        connect_error();
        return ret;
      }
    }
    if (unmapped_addr)
      ret = addr_handler(&test.nodes[i]);
    else
      ret = rdma_resolve_addr(test.nodes[i].cma_id, test.src_addr, test.dst_addr,
                              2000);
    if (ret)
    {
      printf("mckey: resolve addr failure: %d\n", ret);
      connect_error();
      return ret;
    }
  }
  ret = connect_events();
  if (ret)
    goto out;
  /*
   * Pause to give SM chance to configure switches. We don't want to
   * handle reliability issue in this simple test program.
   */
  sleep(3);
  if (message_count)
  {
    if (is_sender)
    {
      printf("initiating data transfers\n");
      for (i = 0; i < connections; i++)
      {
        ret = post_sends(&test.nodes[i], 0);
        if (ret)
          goto out;
      }
    }
    else
    {
      printf("receiving data transfers\n");
      ret = poll_cqs();
      if (ret)
        goto out;
    }
    printf("data transfers complete\n");
  }
out:
  for (i = 0; i < connections; i++)
  {
    ret = rdma_leave_multicast(test.nodes[i].cma_id, test.dst_addr);
    if (ret)
      printf("mckey: failure leaving: %d\n", ret);
  }
  return ret;
}
/**
 * 1. Get command line parameters.
 *     m - MC address, destination port
 *     M - unmapped MC address, requires also bind address (parameter “b”)
 *     s - sender flag.
 *     b - bind address.
 *     c - connections amount.
 *     C - message count.
 *     S - message size.
 *     p - port space (UDP default; IPoIB)
 * 2. Create event channel to receive asynchronous events.
 * 3. Allocate Node and creates an identifier that is used to track communication information
 * 4. Start the “run” main function.
 * 5. On ending - release and free resources.
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv)
{
  int op, ret;
  while ((op = getopt(argc, argv, "m:M:sb:c:C:S:p:")) != -1)
  {
    switch (op)
    {
    case 'm':
      dst_addr = optarg;
      break;
    case 'M':
      unmapped_addr = 1;
      dst_addr = optarg;
      break;
    case 's':
      is_sender = 1;
      break;
    case 'b':
      src_addr = optarg;
      test.src_addr = (struct sockaddr *)&test.src_in;
      break;
    case 'c':
      connections = atoi(optarg);
      break;
    case 'C':
      message_count = atoi(optarg);
      break;
    case 'S':
      message_size = atoi(optarg);
      break;
    case 'p':
      port_space = strtol(optarg, NULL, 0);
      break;
    default:
      printf("usage: %s\n", argv[0]);
      printf("\t-m multicast_address\n");
      printf("\t[-M unmapped_multicast_address]\n"
             "\t replaces -m and requires -b\n");
      printf("\t[-s(ender)]\n");
      printf("\t[-b bind_address]\n");
      printf("\t[-c connections]\n");
      printf("\t[-C message_count]\n");
      printf("\t[-S message_size]\n");
      printf("\t[-p port_space - %#x for UDP (default), %#x for IPOIB]\n", RDMA_PS_UDP, RDMA_PS_IPOIB);
      exit(1);
    }
  }
  test.dst_addr = (struct sockaddr *)&test.dst_in;
  test.connects_left = connections;
  test.channel = rdma_create_event_channel();
  if (!test.channel)
  {
    printf("failed to create event channel\n");
    exit(1);
  }
  if (alloc_nodes())
    exit(1);
  ret = run();
  printf("test complete\n");
  destroy_nodes();
  rdma_destroy_event_channel(test.channel);
  printf("return status %d\n", ret);
  return ret;
}
