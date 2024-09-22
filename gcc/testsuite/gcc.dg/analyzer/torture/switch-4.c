struct snd_ac97 {
  // snip
  unsigned long id;
  // snip
};

int snd_ac97_valid_reg(struct snd_ac97 *ac97, unsigned short reg) {

  switch (ac97->id) {
  case 0x53544d02UL:
    if (reg == 0x22 || reg == 0x7a)
      return 1;
    __attribute__((__fallthrough__));
  case 0x414b4d00UL:
    return 0;
  }
  return 1;
}

int snd_ac97_update_bits(struct snd_ac97 *ac97, unsigned short reg) {
  if (ac97->id == 0x414c4781UL)
    {
      if (!snd_ac97_valid_reg(ac97, reg))
	return -22;
    }
  return 0;
}
