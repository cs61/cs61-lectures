#define _LARGEFILE_SOURCE 1
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <inttypes.h>
#include <fcntl.h>
#include "elf.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#if defined(_MSDOS) || defined(_WIN32)
# include <fcntl.h>
# include <io.h>
#endif

/* This program makes a boot image.
 * It takes at least one argument, the boot sector file.
 * Any succeeding arguments are written verbatim to the output file.
 *
 * Before jumping to the boot sector, the BIOS checks that the last
 * two bytes in the sector equal 0x55 and 0xAA.
 * This code makes sure the code intended for the boot sector is at most
 * 512 - 2 = 510 bytes long, then appends the 0x55-0xAA signature.
 */

int diskfd;
off_t maxoff = 0;
off_t curoff = 0;

int find_partition(off_t partition_sect, off_t extended_sect, int partoff);
void do_multiboot(const char *filename);


void usage(void) {
    fprintf(stderr, "Usage: mkbootdisk BOOTSECTORFILE [FILE | @SECNUM]...\n");
    fprintf(stderr, "   or: mkbootdisk -p DISK [FILE | @SECNUM]...\n");
    fprintf(stderr, "   or: mkbootdisk -m KERNELFILE\n");
    exit(1);
}

FILE *fopencheck(const char *name) {
    FILE *f = fopen(name, "rb");
    if (!f) {
        fprintf(stderr, "%s: %s\n", name, strerror(errno));
        usage();
    }
    return f;
}

void diskwrite(const void *data, size_t amt) {
    if (maxoff && curoff + amt > size_t(maxoff)) {
        fprintf(stderr, "more data than allowed in partition!\n");
        usage();
    }

    while (amt > 0) {
        ssize_t w = write(diskfd, data, amt);
        if (w == -1 && errno != EINTR) {
            perror("write");
            usage();
        } else if (w == 0) {
            fprintf(stderr, "write hit end of file\n");
            usage();
        } else if (w > 0) {
            amt -= w;
            curoff += w;
            data = (const unsigned char *) data + w;
        }
    }
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char zerobuf[512];
    FILE *f;
    size_t n;
    size_t nsectors;
    int i;
    int bootsector_special = 1;

#if defined(_MSDOS) || defined(_WIN32)
    // As our output file is binary, we must set its file mode to binary.
    diskfd = _fileno(stdout);
    _setmode(diskfd, _O_BINARY);
#else
    diskfd = fileno(stdout);
#endif

    // Check for a partition
    if (argc >= 2 && strcmp(argv[1], "-p") == 0) {
        if (argc < 3) {
            usage();
        }
        if ((diskfd = open(argv[2], O_RDWR)) < 0) {
            fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
            usage();
        }
        if (find_partition(0, 0, 0) <= 0) {
            fprintf(stderr, "%s: no JOS partition (type 0x27) found!\n", argv[2]);
            usage();
        }
        argc -= 2;
        argv += 2;
        bootsector_special = 0;
    }

    // Check for multiboot option
    if (argc >= 2 && strcmp(argv[1], "-m") == 0) {
        if (argc < 3) {
            usage();
        }
        do_multiboot(argv[2]);
    }

    // Read files
    if (argc < 2) {
        usage();
    }

    // Read boot sector
    if (bootsector_special) {
        f = fopencheck(argv[1]);
        n = fread(buf, 1, 4096, f);
        if (n > 510) {
            fprintf(stderr, "%s: boot block too large: %s%u bytes (max 510)\n", argv[1], (n == 4096 ? ">= " : ""), (unsigned) n);
            usage();
        }
        fclose(f);

        // Append signature and write modified boot sector
        memset(buf + n, 0, 510 - n);
        buf[510] = 0x55;
        buf[511] = 0xAA;
        diskwrite(buf, 512);
        nsectors = 1;

        argc--;
        argv++;
    } else {
        nsectors = 0;
    }

    // Read any succeeding files, then write them out
    memset(zerobuf, 0, 512);
    for (i = 1; i < argc; i++) {
        size_t pos;
        char *str;
        unsigned long skipto_sector;

        // An argument like "@X" means "skip to sector X".
        if (argv[i][0] == '@' && isdigit(argv[i][1])
            && ((skipto_sector = strtoul(argv[i] + 1, &str, 0)), *str == 0)) {
            if (nsectors > skipto_sector) {
                fprintf(stderr, "mkbootdisk: can't skip to sector %u, already at sector %u\n", (unsigned) skipto_sector, (unsigned) nsectors);
                usage();
            }
            while (nsectors < skipto_sector) {
                diskwrite(zerobuf, 512);
                nsectors++;
            }
            continue;
        }

        // Otherwise, read the file.
        f = fopencheck(argv[i]);
        pos = 0;
        while ((n = fread(buf, 1, 4096, f)) > 0) {
            diskwrite(buf, n);
            pos += n;
        }
        if (pos % 512 != 0) {
            diskwrite(zerobuf, 512 - (pos % 512));
            pos += 512 - (pos % 512);
        }
        nsectors += pos / 512;
        fclose(f);
    }

    // Fill out to 1024 sectors with 0 blocks
    while (nsectors < 1024) {
        diskwrite(zerobuf, 512);
        nsectors++;
    }

    return 0;
}


#define SECTORSIZE              512

static void readsect(void *buf, uint32_t sectno) {
    ssize_t s;
    off_t o = lseek(diskfd, (off_t) sectno * (off_t) SECTORSIZE, SEEK_SET);
    if (o == (off_t) -1) {
        perror("lseek");
        usage();
    }
    do {
        s = read(diskfd, buf, SECTORSIZE);
    } while (s == -1 && errno == EINTR);
    if (s != SECTORSIZE) {
        perror("read");
        usage();
    }
}


// Partitions

// Offset of 1st partition descriptor in a partition sector
#define PTABLE_OFFSET           446
// 2-byte partition table magic number location and value
#define PTABLE_MAGIC_OFFSET     510
#define PTABLE_MAGIC1           0x55
#define PTABLE_MAGIC2           0xAA

// Partition type constants
#define PTYPE_JOS_KERN          0x27    // JOS kernel
#define PTYPE_JOSFS             0x28    // JOS file system
// Extended partition identifiers
#define PTYPE_DOS_EXTENDED      0x05
#define PTYPE_W95_EXTENDED      0x0F
#define PTYPE_LINUX_EXTENDED    0x85

struct Partitiondesc {
    uint8_t boot;
    uint8_t chs_begin[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint32_t lba_start;
    uint32_t lba_length;
};


// Find a JOS kernel partition
int find_partition(off_t partition_sect, off_t extended_sect, int partoff) {
    int i, r;
    uint8_t buf[SECTORSIZE];
    off_t o;
    struct Partitiondesc *ptable;

    // read the partition sector: initially sector 0
    readsect(buf, partition_sect);

    // check for partition table magic number
    if ((uint8_t) buf[PTABLE_MAGIC_OFFSET] != PTABLE_MAGIC1
        || (uint8_t) buf[PTABLE_MAGIC_OFFSET + 1] != PTABLE_MAGIC2) {
        return 0;
    }

    // search partition table
    ptable = (struct Partitiondesc*) (buf + PTABLE_OFFSET);
    for (i = 0; i < 4; i++) {
        if (ptable[i].lba_length == 0) {
            /* ignore entry */;
        } else if (ptable[i].type == PTYPE_JOS_KERN) {
            // use this partition
            partition_sect += (off_t) ptable[i].lba_start;
            fprintf(stderr, "Using partition %d (start sector %ld, sector length %ld)\n", partoff + i + 1, (long) partition_sect, (long) ptable[i].lba_length);
            o = lseek(diskfd, partition_sect * SECTORSIZE, SEEK_SET);
            if (o != partition_sect * SECTORSIZE) {
                fprintf(stderr, "cannot seek to partition start: %s\n", strerror(errno));
                usage();
            }
            maxoff = (off_t) ptable[i].lba_length * SECTORSIZE;
            return 1;
        } else if (ptable[i].type == PTYPE_DOS_EXTENDED
                   || ptable[i].type == PTYPE_W95_EXTENDED
                   || ptable[i].type == PTYPE_LINUX_EXTENDED) {
            off_t inner_sect = extended_sect;
            if (!inner_sect) {
                inner_sect = ptable[i].lba_start;
            }
            if ((r = find_partition(ptable[i].lba_start + extended_sect, inner_sect, (partoff ? partoff + 1 : 4))) > 0) {
                return r;
            }
        }
    }

    // no partition number found
    return 0;
}


const uint32_t multiboot_header[3] = { 0x1BADB002U, 0U, -0x1BADB002U };

void do_multiboot(const char *filename) {
    uint8_t buf[SECTORSIZE];
    elf_header *elfh = (elf_header *) buf;
    off_t o;

    if ((diskfd = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "%s: %s\n", filename, strerror(errno));
        usage();
    }

    readsect(buf, 0);

    if (elfh->e_magic != ELF_MAGIC) {
        fprintf(stderr, "%s: not an ELF executable file\n", filename);
        usage();
    }

    o = elfh->e_phoff + sizeof(elf_program) * elfh->e_phnum;
    if (size_t(o) >= 4096 - sizeof(multiboot_header)) {
        fprintf(stderr, "%s: ELF header too large to accommodate multiboot header\n", filename);
        usage();
    } else if (lseek(diskfd, o, SEEK_SET) != o) {
        perror("lseek");
        usage();
    }

    diskwrite(multiboot_header, sizeof(multiboot_header));
    exit(0);
}
