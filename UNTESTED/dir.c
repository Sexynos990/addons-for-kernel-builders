#include <linux/fs.h>

static bool is_hidden(const char *name)
{
    return strncmp(name, ".ast0lf0_", 9) == 0;
}

static int astolfo_readdir(struct file *file, struct dir_context *ctx)
{
    struct super_block *sb = file->f_path.dentry->d_sb;
    struct astolfo_sb_info *sbi = sb->s_fs_info;
    struct list_head *p;
    
    if (!dir_emit_dots(file, ctx))
        return 0;

    mutex_lock(&sbi->hidden_lock);
    list_for_each(p, &sbi->hidden_files) {
        struct dentry *dentry = list_entry(p, struct dentry, d_child);
        if (!is_hidden(dentry->d_name.name))
            dir_emit(ctx, dentry->d_name.name, dentry->d_name.len,
                    dentry->d_inode->i_ino, dt_type(dentry->d_inode));
    }
    mutex_unlock(&sbi->hidden_lock);
    
    return 0;
}

const struct file_operations astolfo_dir_ops = {
    .read = generic_read_dir,
    .iterate_shared = astolfo_readdir,
    .llseek = generic_file_llseek,
};
