#include <apid.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include <editline.h>

#define PROMPT "SSCLI=> "

void output(char const *str) {
  printf("\33[s\33[2K\r%s%s%s\33[u", str, PROMPT, rl_line_buffer);
  fflush(stdout);
}

char *nextline(void) {
  static char *line_buffer = NULL;
  if (line_buffer)
    free(line_buffer);
  return (line_buffer = readline(PROMPT));
}

void methodres(char const *data, void *user) { output(data); }

thrd_t input_thread;

int handler(void *_) {
  while (1) {
    char buffer[1024] = {0};
    char *line = nextline();
    if (!line) {
      printf("bye!\n");
      apid_stop();
      exit(0);
      return 0;
    }
    snprintf(buffer, sizeof buffer, "sscli\n%s", line);
    apid_invoke_method(methodres, NULL, "command:execute", buffer);
  }
}

void startcli(char const *data, void *user) {
  printf("Connected!\n");
  thrd_create(&input_thread, handler, NULL);
}

int main() {
  rl_initialize();
  apid_init();
  apid_invoke_method(startcli, NULL, "core:ping", "");
  printf("Connecting to StoneServer...\n");
  return apid_start();
}