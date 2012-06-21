#ifndef GRSD_H
#define GRSD_H

struct _grsd;

/**
 * Handle to the grs-daemon-process.
 */
typedef struct _grsd* grsd_t;

/*
 * Creates a new grsd-handle.
 *
 * @return A new grsd-handle or <code>NULL</code> on error.
 */
grsd_t grsd_init();

/**
 * Destroys the grsd-handle again.
 *
 * @param handle The handle to be destroyed.
 * @return On success <code>0</code> is returned, on error <code>-1</code>.
 */
int grsd_destroy(grsd_t handle);

/**
 * Main listening-loop.
 *
 * This method blocks the calling thread until grsd_listen_exit() is called
 * from another thread.
 *
 * @param handle The handle to the grsd-module
 * @return If the listening-loop was aborted gracefully, <code>0</code> is
 *         returned.
 */
int grsd_listen(grsd_t handle);

/**
 * Requests to leave the grsd_listen-loop.
 *
 * @param handle The handle to the grsd-module
 * @return If the request was issued, then <code>0</code> is returned. On error
 *         the return value is <code>-1</code>.
 */
int grsd_listen_exit(grsd_t handle);

#endif /* GRSD_H */
