void myfree (void *, void *, void *, void *);
void myfree2 (void *, void *, void *);

static char a, b, c, d;
int main ()
{
  myfree (&a, &b, &c, &d);
  myfree2 (&a, &b, &c);
}
