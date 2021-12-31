#include <stdlib.h>
main()
{
  char template[] = "template-XXXXXX";
  mktemp(template);
  printf("template=%s\n", template);
}
