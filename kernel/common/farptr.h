void _farpokeb(unsigned short, unsigned long, unsigned char);
void _farpokew(unsigned short, unsigned long, unsigned short);
void _farpokel(unsigned short, unsigned long, unsigned long);
unsigned char _farpeekb(unsigned short, unsigned long);
unsigned short _farpeekw(unsigned short, unsigned long);
unsigned long _farpeekl(unsigned short, unsigned long);
void _farsetsel(unsigned short);
unsigned short _fargetsel(void);
void _farnspokeb(unsigned long, unsigned char);
void _farnspokew(unsigned long, unsigned short);
void _farnspokel(unsigned long, unsigned long);
unsigned char _farnspeekb(unsigned long);
unsigned short _farnspeekw(unsigned long);
unsigned long _farnspeekl(unsigned long);

void
_farpokeb(unsigned short selector,
	 unsigned long offset,
	 unsigned char value)
{
  __asm__ __volatile__ ("movw %w0,%%fs\n"
      "	.byte 0x64 \n"
      "	movb %b1,(%k2)"
      :
      : "rm" (selector), "qi" (value), "r" (offset));
}

void
_farpokew(unsigned short selector,
	 unsigned long offset,
	 unsigned short value)
{
  __asm__ __volatile__ ("movw %w0,%%fs \n"
      "	.byte 0x64 \n"
      "	movw %w1,(%k2)"
      :
      : "rm" (selector), "ri" (value), "r" (offset));
}

void
_farpokel(unsigned short selector,
	 unsigned long offset,
	 unsigned long value)
{
  __asm__ __volatile__ ("movw %w0,%%fs \n"
      "	.byte 0x64 \n"
      "	movl %k1,(%k2)"
      :
      : "rm" (selector), "ri" (value), "r" (offset));
}

unsigned char
_farpeekb(unsigned short selector,
	 unsigned long offset)
{
  unsigned char result;
  __asm__ __volatile__ ("movw %w1,%%fs \n"
      "	.byte 0x64 \n"
      "	movb (%k2),%b0"
      : "=q" (result)
      : "rm" (selector), "r" (offset));
  return result;
}

unsigned short
_farpeekw(unsigned short selector,
	 unsigned long offset)
{
  unsigned short result;
  __asm__ __volatile__ ("movw %w1, %%fs \n"
      "	.byte 0x64 \n"
      "	movw (%k2),%w0 \n"
      : "=r" (result)
      : "rm" (selector), "r" (offset));
  return result;
}

unsigned long
_farpeekl(unsigned short selector,
	 unsigned long offset)
{
  unsigned long result;
  __asm__ __volatile__ ("movw %w1,%%fs\n"
      "	.byte 0x64\n"
      "	movl (%k2),%k0"
      : "=r" (result)
      : "rm" (selector), "r" (offset));
  return result;
}

void
_farsetsel(unsigned short selector)
{
  __asm__ __volatile__ ("movw %w0,%%fs"
      :
      : "rm" (selector));
}

unsigned short
_fargetsel(void)
{
  unsigned short selector;
  __asm__ __volatile__ ("movw %%fs,%w0 \n"
      : "=r" (selector)
      : );
  return selector;
}

void
_farnspokeb(unsigned long offset,
	 unsigned char value)
{
  __asm__ __volatile__ (".byte 0x64\n"
      "	movb %b0,(%k1)"
      :
      : "qi" (value), "r" (offset));
}

void
_farnspokew(unsigned long offset,
	 unsigned short value)
{
  __asm__ __volatile__ (".byte 0x64\n"
      "	movw %w0,(%k1)"
      :
      : "ri" (value), "r" (offset));
}

void
_farnspokel(unsigned long offset,
	 unsigned long value)
{
  __asm__ __volatile__ (".byte 0x64\n"
      "	movl %k0,(%k1)"
      :
      : "ri" (value), "r" (offset));
}

unsigned char
_farnspeekb(unsigned long offset)
{
  unsigned char result;
  __asm__ __volatile__ (".byte 0x64\n"
      "	movb (%k1),%b0"
      : "=q" (result)
      : "r" (offset));
  return result;
}

unsigned short
_farnspeekw(unsigned long offset)
{
  unsigned short result;
  __asm__ __volatile__ (".byte 0x64\n"
      "	movw (%k1),%w0"
      : "=r" (result)
      : "r" (offset));
  return result;
}

unsigned long
_farnspeekl(unsigned long offset)
{
  unsigned long result;
  __asm__ __volatile__ (".byte 0x64\n"
      "	movl (%k1),%k0"
      : "=r" (result)
      : "r" (offset));
  return result;
}