#ifndef GRSD_H
#define GRSD_H

struct _grs;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grs* grs_t;

/*
 * Creates a new grsd-handle.
 *
 * @return A new grsd-handle or <code>NULL</code> on error.
 */
grs_t grsd_init();

/**
 * Destroys the grsd-handle again.
 *
 * @param handle The handle to be destroyed.
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int grsd_destroy(grs_t handle);

#endif /* GRSD_H */
