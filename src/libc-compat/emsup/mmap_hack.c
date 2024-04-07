// See:
//   https://github.com/emscripten-core/emscripten/issues/20459
//
// shared read/write file mapping on Emscripten currently
// requires keeping the original fd open before a call to
// munmap() to sync the changes to the filesystem
//
// so here we wrap around some related functions to delay
// the close() to where munmap() is called

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

struct map_entry {
    void *ptr;
    size_t len;
    int fd;
    bool want_close;
    struct map_entry *next;
};

// TODO: not thread-safe, add a lock if needed
// (currently magiskboot is a single-threaded program)
static struct map_entry *maps = NULL;

#define filter_map_by_key_or(_name, _key, _handler, _else)  \
    {   \
        __attribute__((unused)) struct map_entry *i; \
        __attribute__((unused)) struct map_entry *prev = NULL; \
        bool flag = false;  \
        for (i = maps; i; i = i->next) {    \
            if (i->_name == _key) {     \
                flag = true;    \
                _handler;   \
            }   \
            prev = i;   \
        }   \
        if (!flag)  \
            _else;      \
    }

void *_mmap_stub_impl(void *addr, size_t len, int prot,
                int flags, int fildes, off_t off) {
    void *ptr = mmap(addr, len, prot, flags, fildes, off);

    if (ptr == MAP_FAILED)
        return ptr;

    if (!(fildes < 0) &&
            (prot & PROT_WRITE) && (flags & MAP_SHARED)) {
        // this is the map we are interested in

        struct map_entry *map = malloc(sizeof(struct map_entry));

        assert(map);

        map->ptr = ptr;
        map->len = len;
        map->fd = fildes;
        map->want_close = false;

        // add to linked list
        map->next = maps;
        maps = map;
    }

    return ptr;
}

// close() stub is using ld's --wrap feature
// bcs we must also override its usage in Rust STD

int __real_close(int fildes);

int __wrap_close(int fildes) {
    if (fcntl(fildes, F_GETFD) < 0)
        return -1;  // invalid fd

    filter_map_by_key_or(fd, fildes, {
        // mark all matches as todo for munmap
        i->want_close = true;
    }, {
        // no match, closing as-is
        return __real_close(fildes);
    });

    return 0;
}

static inline void post_munmap_hook(struct map_entry *map, size_t len) {
    // we dont support partial munmapping
    assert(map->len == len);

    filter_map_by_key_or(fd, map->fd, {
        // delay the close until the last file mapping
        // associcated with this fd is munmapped
        assert(i->want_close);

        break;
    }, {
        // perform the delayed close
        __real_close(map->fd);
    });
}

int _munmap_stub_impl(void *addr, size_t len) {
    int res = munmap(addr, len);

    if (res < 0)
        return res;

    filter_map_by_key_or(ptr, addr, {
        // remove from linked list
        if (prev)
            prev->next = i->next;
        else
            maps = i->next;

        if (i->want_close)
            post_munmap_hook(i, len);

        free(i);

        break;
    }, {
        // no match, no action needed
    });

    return res;
}
