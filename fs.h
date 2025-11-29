#ifndef FS_H
#define FS_H

void fs_init(void);
void fs_list(void);
const char *fs_read(const char *name);
int  fs_create(const char *name, const char *contents);

#endif
