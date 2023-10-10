#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define TEMPLATE_START "<%"
#define TEMPLATE_END "%>"

char *moveToChar(char *s, char c) {
  while(*s!=0 && *s != c) s++;
  return s;
}

bool startsWith(char *s, char *prefix) {
  while(*prefix != 0 && *s != 0 && *prefix == *s ) {
    s++;
    prefix++;
  }
  return *prefix == 0;
}

char *moveToStr(char *s, char *substr) {
  for (; *(s = moveToChar(s, *substr)) != 0 && !startsWith(s, substr); s++);
  return s;
}

char *moveToStart(char *c) {return moveToStr(c, TEMPLATE_START);}
char *moveToEnd(char *c) {return moveToStr(c, TEMPLATE_END);}

char *skipStr(char *s, char *substr) {return s + strlen(substr);}
char *skipStart(char *c) { return skipStr(c, TEMPLATE_START); }
char *skipEnd(char *c) { return skipStr(c, TEMPLATE_END); }

void printFromTo(char *start, char *end) {
  while(start!=end && *start != 0 ){
    printf("%c", *start);
    start++;
  }
}

void printString(char *start, char *end) {
  printf("\"");
  while (start != end && *start != 0) {
    if (*start == '\"') {
      printf("\\%c", *start);
    } else {
      printf("%c", *start);
    }
    start++;
  }
  printf("\"");
}

void convertContent(char *content) {
  char *offset = content;
  while (*offset != 0) {
    char *nextOpen = moveToStart(offset);
    if (*nextOpen == 0){
      printString(offset, nextOpen);
      return;
    }

    printString(offset, nextOpen);

    offset = skipStart(nextOpen);
    if (*offset == 0) {
      fprintf(stderr, "Can't find end of template: \n\t%s", content);
      return;
    }

    char *nextClose = moveToEnd(offset);
    printFromTo(offset, nextClose);

    offset = skipEnd(nextClose);
  }
}

char *FileContent(const char *path) {
  FILE *f = fopen(path, "r");
  if (f == NULL) {
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  char *content = malloc(size + 1);

  fseek(f, 0, SEEK_SET);
  fread(content, 1, size, f);
  content[size] = 0;
  fclose(f);

  return content;
}

void strreplace(char *c, char *replace, char with) {
  for(char *rr = replace; *rr != 0; rr++)
    for(char *cc = c; *cc != 0; cc++)
      if(*cc == *rr )
        *cc = with;
}

void processFile(char *f) {
  fprintf(stderr, "Processin file: %s\n", f);
  char *content = FileContent(f);

  char *funcName = strdup(f);
  strreplace(funcName, "/.", '_');
  printf("View *%s() {\n", funcName);
  printf("View writer;");
  free(funcName);

  convertContent(content);
  free(content);

  printf("return writer; }\n");
}

void processDir(char *dir) {
  DIR *dfd;

  if ((dfd = opendir(dir)) == NULL) {
    fprintf(stderr, "Can't open %s\n", dir);
    return;
  }

  char filename_qfd[100];
  struct dirent *dp;

  printf(
         "typedef struct View {"
         "void *value;"
         "struct View next;"
         "} View;"
         );

  while ((dp = readdir(dfd)) != NULL) {
    struct stat stbuf;
    sprintf(filename_qfd, "%s/%s", dir, dp->d_name);
    if (stat(filename_qfd, &stbuf) == -1) {
      printf("Unable to stat file: %s\n", filename_qfd);
      continue;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
      continue;
    } else {
      processFile(filename_qfd);
    }
  }

  closedir(dfd);
}


int main() {
  processDir("views");
}
