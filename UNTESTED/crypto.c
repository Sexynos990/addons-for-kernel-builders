#include <linux/crypto.h>
#include <linux/scatterlist.h>

void astolfo_wipe(void *data, size_t len)
{
    static const u8 patterns[] = {0x00, 0xFF, 0xAA, 0x55, 0x6D, 0xB6};
    for (int i = 0; i < ARRAY_SIZE(patterns); i++) {
        memset(data, patterns[i], len);
        mb(); // Memory barrier
    }
    memset(data, 0, len);
}

int astolfo_generate_hmac(struct astolfo_sb_info *sbi, 
                        const u8 *data, size_t len, u8 *mac)
{
    struct crypto_shash *tfm = crypto_alloc_shash("hmac(sha3-512)", 0, 0);
    SHASH_DESC_ON_STACK(desc, tfm);
    int err;
    
    crypto_shash_setkey(tfm, sbi->hmac_key, sizeof(sbi->hmac_key));
    desc->tfm = tfm;
    
    err = crypto_shash_digest(desc, data, len, mac);
    crypto_free_shash(tfm);
    return err;
}
