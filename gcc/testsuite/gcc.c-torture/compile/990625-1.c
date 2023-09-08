#define __USE_STRING_INLINES
/* customized for PDP10 */
#include <string.h> /* { dg-error "No such file" } */

void test()
{
        char *p, *a;
        const char *s;

        while ( (s = a) )
          p = strcpy(strcpy(p,"/"), s);
}

