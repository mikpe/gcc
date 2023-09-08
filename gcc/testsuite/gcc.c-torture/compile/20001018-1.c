void
foo (void)
{
  extern char i[10];

  {
    extern char i[];
	/* On the PDP10, arrays are allocated on word boundaries with word length
	   so sizeof(i) will be 12.
	 */
    char x[sizeof (i) >= 10 ? 1 : -1];
    /*char x[sizeof (i) == 10 ? 1 : -1];*/
  }
}
