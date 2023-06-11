#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
//#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>

#define SEC(NAME) __attribute__((section(NAME), used))

SEC("xdp")
int xdp_prog(struct xdp_md *ctx) {
    unsigned char *data_end = (unsigned char *)(long)ctx->data_end;
    unsigned char *data = (unsigned char *)(long)ctx->data;
    struct ethhdr *eth = data;
    struct iphdr *ip = data + sizeof(*eth);
    struct tcphdr *tcp = data + sizeof(*eth) + sizeof(*ip);
    unsigned char *payload = (unsigned char *)tcp + sizeof(*tcp);
    int payload_len = data_end - payload;

    if (ip->protocol == IPPROTO_TCP && tcp->dest == htons(80)) {
        if (payload_len > 4 && payload[0] == 'G' && payload[1] == 'E' && payload[2] == 'T' && payload[3] == ' ') {
            char *url_start = payload + 4;
            char *url_end = strstr(url_start, "\r\n");
            if (url_end) {
                int url_len = url_end - url_start;
                char url[url_len + 1];
                strncpy(url, url_start, url_len);
                url[url_len] = '\0';
                bpf_trace_printk("url: %s\n", url);

                // open file and forward
                char filename[] = "/tmp/test.txt";
                int fd = bpf_syscall_open(filename, O_RDONLY, 0);
                if (fd >= 0) {
                    bpf_trace_printk("open file success\n");
                    char buf[1024];
                    int len;
                    while ((len = bpf_syscall_read(fd, buf, sizeof(buf))) > 0) {
                        bpf_trace_printk("read %d bytes\n", len);
                        void *data_end_new = data + len;
                        memcpy(data, buf, len);
                        bpf_xdp_adjust_head(ctx, -(data - buf));
                        bpf_trace_printk("forward %d bytes\n", len);
                        bpf_xdp_tx(ctx, 0, 0);
                        data = data_end_new;
                    }
                    bpf_syscall_close(fd);
                } else {
                    bpf_trace_printk("open file failed\n");
                }
            }
        }
    }

    return XDP_PASS;
}
