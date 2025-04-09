#include <linux/fs.h>
#include <linux/xattr.h>

static const struct inode_operations astolfo_dir_inode_operations = {
    .lookup = simple_lookup,
    .getattr = simple_getattr,
};

static const struct inode_operations astolfo_file_inode_operations = {
    .getattr = simple_getattr,
};

struct inode *astolfo_make_inode(struct super_block *sb, umode_t mode)
{
    struct inode *inode = new_inode(sb);
    if (!inode)
        return ERR_PTR(-ENOMEM);
    
    inode->i_ino = get_next_ino();
    inode->i_mode = mode;
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
    
    if (S_ISDIR(mode)) {
        inode->i_op = &astolfo_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
        inc_nlink(inode);
    } else {
        inode->i_op = &astolfo_file_inode_operations;
        inode->i_fop = &astolfo_file_ops;
    }
    
    return inode;
}
