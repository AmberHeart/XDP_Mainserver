#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>

#define SEC(NAME) __attribute__((section(NAME), used))

SEC("xdp")
int xdp_prog(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth = data;
    struct iphdr *ip = data + sizeof(*eth);
    struct tcphdr *tcp = data + sizeof(*eth) + sizeof(*ip);
    char *payload = (char *)tcp + sizeof(*tcp);
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
                // TODO: open file and forward
            }
        }
    }

    return XDP_PASS;
}
