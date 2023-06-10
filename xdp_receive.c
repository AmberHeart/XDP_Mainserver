#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#define SEC(NAME) __attribute__((section(NAME), used))

SEC("xdp_receive")
int xdp_receive(struct __sk_buff *skb) {
    void *data = (void *)(long)skb->data;
    void *data_end = (void *)(long)skb->data_end;

    struct ethhdr *eth = data;
    struct iphdr *ip = data + sizeof(struct ethhdr);

    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end)
        return XDP_DROP;

    // 可以在这里进行数据包的解析和提取需要的信息
    // 例如，提取文件路径和其他必要的信息

    // 返回XDP_PASS继续处理数据包
    return XDP_PASS;
}
