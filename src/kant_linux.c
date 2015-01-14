#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "kant.h"
#include "kant_linux.h"


const char * libkant = "libkant.so";

internal struct linux_kant_code
kant_load_code(const char *source_lib)
{
  struct stat attr;
  struct linux_kant_code res = {};
  if ((stat(libkant, &attr) == 0) && (kant->id != attr.st_ino)) {
    if (kant->handle) {
      kant->api.unload(kant->state);
      dlclose(kant->handle);
    }
    void *handle = dlopen(source_lib, RTLD_NOW);
    if (handle) {
      res.handle = handle;
      kant->id = attr.st_ino;
      const struct kant_api *api = dlsym(kant->handle, "KANT_API");
      if (api != 0) {
	kant->api = *api;
	if (kant->state == 0) {
	  kant->state = kant->api.init();
	}
	kant->api.reload(kant->state);
      } else {
	dlclose(kant->handle);
	kant->handle = 0;
	kant->id = 0;
      }
    } else {
      kant->handle = 0;
      kant->api = NULL;
    }
  }
}

internal void kant_unload(struct kant * kant) {
  if (kant->handle) {
    kant->api.finalize(kant->state);
    kant->state = 0;
    dlclose(kant->handle);
    kant->handle = 0;
    kant->id = 0;
  }
}


int main(void) {
  struct kant kant = {0};
  for (;;) {
    kant_load(&kant);
    if (kant.handle) {
      
      
    }
  }
  kant_unload(&kant);
  return 0;
}

