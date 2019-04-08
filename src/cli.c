#include <apid.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <threads.h>
#include <unistd.h>

#include <editline.h>

#define PROMPT "SSCLI=> "

static void output(char const *str) {
  printf("\33[s\33[2K\r%s%s%s\33[u", str, PROMPT, rl_line_buffer);
  fflush(stdout);
}

static char *nextline(void) {
  static char *line_buffer = NULL;
  if (line_buffer)
    free(line_buffer);
  return (line_buffer = readline(PROMPT));
}

static void methodres(char const *data, void *user) { output(data); }

thrd_t input_thread, alt_thread;

static int started = false;

static int handler(void *_) {
  prctl(PR_SET_PDEATHSIG, SIGINT);
  started = 1;
  while (1) {
    char buffer[1024] = {0};
    char *line = nextline();
    if (!line) {
      printf("bye!\n");
      apid_stop();
      _exit(0);
      return 0;
    }
    snprintf(buffer, sizeof buffer, "sscli\n%s", line);
    apid_invoke_method(methodres, NULL, "command:execute", buffer);
  }
}

static void startcli(char const *data, void *user) {
  if (!data) {
    printf(".");
    fflush(stdout);
    apid_invoke_method_timeout(startcli, NULL, 1, "core:ping", "");
    return;
  }
  printf("Connected!\n");
  thrd_create(&input_thread, handler, NULL);
  thrd_detach(input_thread);
}

static int alt(void *path) {
  prctl(PR_SET_PDEATHSIG, SIGINT);
  prctl(PR_SET_NAME, path);
  FILE *af = fopen(path, "r");
  if (!af)
    return 0;
  fd_set fdset;
  FD_ZERO(&fdset);
  FD_SET(fileno(af), &fdset);
  char *buffer;
  size_t size;
  for (;;) {
    if (getline(&buffer, &size, af) != -1) {
      if (started)
        output(buffer);
      else {
        printf("\r%s", buffer);
        fflush(stdout);
      }
    }
    thrd_yield();
  }
  return 0;
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < argc - 1; i++) {
    thrd_create(&alt_thread, alt, argv[1 + i]);
    thrd_detach(alt_thread);
  }
  rl_initialize();
  apid_init();
  apid_invoke_method_timeout(startcli, NULL, 1, "core:ping", "");
  printf("Connecting to StoneServer...\n");
  return apid_start();
}