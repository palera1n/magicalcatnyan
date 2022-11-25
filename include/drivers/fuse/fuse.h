#ifndef FUSE_H
#define FUSE_H

extern uint64_t gFuseBase;

void fuse_cmd_demote(void);
void fuse_cmd_lock(void);
void fuse_cmd_status(void);

#endif
