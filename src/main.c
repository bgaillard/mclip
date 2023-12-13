#define _GNU_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

static const char SHARED_MEMORY_NAME[] = "/mclip";

/**
 * Read text from stdin and write into the shared memory.
 */
void in() {
  void *addr = NULL;
  int fd = 0;

  fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

  if (fd == -1) {
    fprintf(stderr, "shm_open() failed\n");
    exit(1);
  }

  char characters[2]  = {0};

  int offset = 0;
  int nb_read = 0;
  int total_nb_read = 0;

  while(fgets(characters, sizeof(characters), stdin) != NULL) {
    nb_read = strlen(characters);
    total_nb_read = total_nb_read + nb_read;

    // Truncate the file to reserve space in it
    if(ftruncate(fd, total_nb_read) == -1) {
      fprintf(stderr, "ftruncate() failed\n");
      exit(1);
    }

    if (addr != NULL)
      addr = mremap(addr, offset, total_nb_read, MREMAP_MAYMOVE);
    else
      addr = mmap(NULL, total_nb_read, PROT_WRITE, MAP_SHARED, fd, 0);

    sprintf(addr + offset, "%s", characters);

    offset = total_nb_read;
  }

  munmap(addr, total_nb_read);
  memset(characters, 0, sizeof(characters));

  return;
}

/**
 * Read text from the shared memory and write it to stdout.
 */
void out() {
  struct stat st;
  int fd = 0;
  void *addr = NULL;

  fd = shm_open(SHARED_MEMORY_NAME, O_RDONLY, S_IRUSR | S_IWUSR);

  if (fd == -1) {
    fprintf(stderr, "shm_open() failed\n");
    exit(1);
  }

  fstat(fd, &st);

  // The shared memory is empty when it has just been initialized the first time with 'shm_open()'.
  if (st.st_size != 0) {
    addr = mmap(NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
      fprintf(stderr, "mmap() failed\n");
      exit(1);
    }
  }

  printf("%s", (char*) addr);
}

/**
 * Display the help.
 */
void help() {
  printf("Usage: mclip [OPTION]\n\n");
  printf("Clipboard which stores its data inside shared memory.\n\n");
  printf("\t -h, --help  \t show quick summary of options\n");
  printf("\t -i, --in    \t read text from standard input and write it into the clipboard shared memory (default)\n");
  printf("\t -o, --out   \t print the content of the clipboard shared memory\n\n");
}

/**
 * Main entry of the mclip program.
 *
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 */
int main(int argc, char *argv[])
{
  int option_help = 0;
  int option_in = 0;
  int option_out = 0;

  int c;
  int option_index = 0;

  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"in",   no_argument, 0, 'i'},
    {"out",  no_argument, 0, 'o'},
    {0, 0, 0, 0}
  };

  while(1) {
    c = getopt_long(argc, argv, "hio", long_options, &option_index);

    // No more options to read
    if (c == -1)
      break;

    switch(c) {

      // --help
      case 'h':
        option_help = 1;
        break;

      // --in
      case 'i':
        option_in = 1;
        break;

      // --out
      case 'o':
        option_out = 1;
        break;

      // Error encountered (i.e. bad option specified)
      case '?':
        exit(1);
    }

  }

  if (option_help == 1 && (option_in == 1 || option_out == 1)) {
    fprintf(stderr, "The '--help' option cannot be provided with the '--in' or '--out' option!\n");
    return 1;
  } else if (option_help == 0 && option_in == 1 && option_out == 1) {
    fprintf(stderr, "The '--in' and '--out' options cannot be specified together!\n");
    return 1;
  } else if (optind < argc) {
    fprintf(stderr, "Unknown argument speficied '%s'!\n", argv[optind]);
    return 1;
  } else if (option_help == 1) {
    help();
    return 0;
  }

  if (option_in == 1 || (option_in == 0 && option_out == 0)) {
    in();
  } else if (option_out == 1) {
    out();
  } else {
    fprintf(stderr, "Unknown program state!\n");
    return 1;
  }

  return 0;
}
