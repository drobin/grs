#ifndef GRSD_H
#define GRSD_H

struct _grs;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grs* grs_t;

/*
 * Creates a new grs-handle.
 *
 * @return A new grs-handle or <code>NULL</code> on error.
 */
grs_t grs_init();

/**
 * Destroys the grs-handle again.
 *
 * @param handle The handle to be destroyed.
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int grs_destroy(grs_t handle);

#endif /* GRSD_H */
