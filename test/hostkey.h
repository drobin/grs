#ifndef HOSTKEY_H
#define HOSTKEY_H

const char* hostkey_get_path();
int hostkey_generate();
int hostkey_remove();

#endif  /* HOSTKEY_H */