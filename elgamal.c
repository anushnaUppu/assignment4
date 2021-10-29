#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include<time.h>

#define MAXDIGITS 10000

#define PLUS 1
#define MINUS - 1

typedef struct {
  char digits[MAXDIGITS];
  int signbit;
  int lastdigit;
}
bignum;

clock_t start, end;
double cpu_time_used;
void encrypt();
void decrypt();
void subtractBignum();
void zeroJustify();
int compareBignum();
void addBignum();
void divideBignum();
void multiplyBignum();
void convertToBignum();
void power();
void modulo();
void printBignum();
bignum modInverse();

bignum mb, xb, yb, gb, pb, rb, c1, c2, zero, one, op, temp;
int num;

int main() {
  int m, p = 2111, g = 211;
  convertToBignum(g, & gb);
  printf("ENTER NUMBER(<2111)\n");
  scanf("%d", & m);
  bignum randb;
  convertToBignum(m, & mb);
  convertToBignum(p, & pb);
  convertToBignum(rand(), & randb);
  modulo( & randb, & pb, & xb);
  bignum xbtemp = xb;
  power( & gb, & xbtemp, & temp);
  modulo( & temp, & pb, & yb);
  encrypt();
  decrypt();
  return 0;

}

void encrypt() {
  bignum randb, one, c1temp, c2temp1, c2temp2;
  convertToBignum(rand(), & randb);
  convertToBignum(1, & one);
  subtractBignum( & pb, & one, & temp);
  modulo( & randb, & temp, & rb);
  bignum rbtemp = rb;
  power( & gb, & rb, & c1temp);
  power( & yb, & rbtemp, & c2temp1);
  modulo( & c1temp, & pb, & c1);
  multiplyBignum( & c2temp1, & mb, & c2temp2);
  modulo( & c2temp2, & pb, & c2);
  printf("AFTER ENCRYPTION \nc1=");
  printBignum( & c1);
  printf("  c2=");
  printBignum( & c2);
  printf("\n");
}

void decrypt() {
  bignum temp1, temp2, modInv;
  bignum xbTemp = xb;
  power( & c1, & xbTemp, & temp1);
  modInv = modInverse(temp1, pb);
  bignum ptemp = pb;
  multiplyBignum( & c2, & modInv, & temp2);
  modulo( & temp2, & ptemp, & op);
  printf("THE DECRYPTED NUMBER IS:\n");
  printBignum( & op);
  printf("\n");
}

void gcdExtended(bignum a, bignum b, bignum * s, bignum * t) {
  convertToBignum(0, & zero);
  if (compareBignum( & zero, & a) == 0) {
    convertToBignum(1, & one);
    * s = zero;
    * t = one;
    return;
  }
  bignum x1, y1, c, var1, var2;
  modulo( & b, & a, & c);
  gcdExtended(c, a, & x1, & y1);
  divideBignum( & b, & a, & var2);
  multiplyBignum( & var2, & x1, & var1);
  subtractBignum( & y1, & var1, s);
  * t = x1;

}

bignum modInverse(bignum a, bignum m) {
  bignum s, t, res, temp1, temp2;
  gcdExtended(a, m, & s, & t);
  modulo( & s, & m, & temp2);
  addBignum( & temp2, & m, & temp1);
  modulo( & temp1, & m, & res);
  return res;
}

void printBignum(bignum * n) {
  int i;

  for (i = n -> lastdigit; i >= 0; i--)
    printf("%c", '0' + n -> digits[i]);

}

void convertToBignum(int s, bignum * n) {
  int i;
  int t;

  n -> signbit = PLUS;

  for (i = 0; i < MAXDIGITS; i++) n -> digits[i] = (char) 0;

  n -> lastdigit = -1;

  t = abs(s);

  while (t > 0) {
    n -> lastdigit++;
    n -> digits[n -> lastdigit] = (t % 10);
    t = t / 10;
  }

  if (s == 0) n -> lastdigit = 0;
}

void initializeBignum(bignum * n) {
  convertToBignum(0, n);
}

int max(int a, int b) {
  if (a > b) return (a);
  else return (b);
}

void addBignum(bignum * a, bignum * b, bignum * c) {
  int carry;
  int i;

  initializeBignum(c);

  if (a -> signbit == b -> signbit) c -> signbit = a -> signbit;
  else {
    if (a -> signbit == MINUS) {
      a -> signbit = PLUS;
      subtractBignum(b, a, c);
      a -> signbit = MINUS;
    } else {
      b -> signbit = PLUS;
      subtractBignum(a, b, c);
      b -> signbit = MINUS;
    }
    return;
  }

  c -> lastdigit = max(a -> lastdigit, b -> lastdigit) + 1;
  carry = 0;

  for (i = 0; i <= (c -> lastdigit); i++) {
    c -> digits[i] = (char)(carry + a -> digits[i] + b -> digits[i]) % 10;
    carry = (carry + a -> digits[i] + b -> digits[i]) / 10;
  }

  zeroJustify(c);
}

void subtractBignum(bignum * a, bignum * b, bignum * c) {
  int borrow;
  int v;
  int i;

  initializeBignum(c);

  if ((a -> signbit == MINUS) || (b -> signbit == MINUS)) {
    b -> signbit = -1 * b -> signbit;
    addBignum(a, b, c);
    b -> signbit = -1 * b -> signbit;
    return;
  }

  if (compareBignum(a, b) == PLUS) {
    subtractBignum(b, a, c);
    c -> signbit = MINUS;
    return;
  }

  c -> lastdigit = max(a -> lastdigit, b -> lastdigit);
  borrow = 0;

  for (i = 0; i <= (c -> lastdigit); i++) {
    v = (a -> digits[i] - borrow - b -> digits[i]);
    if (a -> digits[i] > 0)
      borrow = 0;
    if (v < 0) {
      v = v + 10;
      borrow = 1;
    }

    c -> digits[i] = (char) v % 10;
  }

  zeroJustify(c);
}

int compareBignum(bignum * a, bignum * b) {
  int i;

  if ((a -> signbit == MINUS) && (b -> signbit == PLUS)) return (PLUS);
  if ((a -> signbit == PLUS) && (b -> signbit == MINUS)) return (MINUS);

  if (b -> lastdigit > a -> lastdigit) return (PLUS * a -> signbit);
  if (a -> lastdigit > b -> lastdigit) return (MINUS * a -> signbit);

  for (i = a -> lastdigit; i >= 0; i--) {
    if (a -> digits[i] > b -> digits[i]) return (MINUS * a -> signbit);
    if (b -> digits[i] > a -> digits[i]) return (PLUS * a -> signbit);
  }

  return (0);
}

void zeroJustify(bignum * n) {
  while ((n -> lastdigit > 0) && (n -> digits[n -> lastdigit] == 0))
    n -> lastdigit--;

  if ((n -> lastdigit == 0) && (n -> digits[0] == 0))
    n -> signbit = PLUS;
}

void digit_shift(bignum * n, int d) {
  int i;

  if ((n -> lastdigit == 0) && (n -> digits[0] == 0)) return;

  for (i = n -> lastdigit; i >= 0; i--)
    n -> digits[i + d] = n -> digits[i];

  for (i = 0; i < d; i++) n -> digits[i] = 0;

  n -> lastdigit = n -> lastdigit + d;
}

void multiplyBignum(bignum * a, bignum * b, bignum * c) {
  bignum row;
  bignum tmp;
  int i, j;

  initializeBignum(c);

  row = * a;

  for (i = 0; i <= b -> lastdigit; i++) {
    for (j = 1; j <= b -> digits[i]; j++) {
      addBignum(c, & row, & tmp);
      * c = tmp;
    }
    digit_shift( & row, 1);
  }

  c -> signbit = a -> signbit * b -> signbit;

  zeroJustify(c);
}

void divideBignum(bignum * a, bignum * b, bignum * c) {
  bignum row;
  bignum tmp;
  int asign, bsign;
  int i, j;

  initializeBignum(c);

  c -> signbit = a -> signbit * b -> signbit;

  asign = a -> signbit;
  bsign = b -> signbit;

  a -> signbit = PLUS;
  b -> signbit = PLUS;

  initializeBignum( & row);
  initializeBignum( & tmp);

  c -> lastdigit = a -> lastdigit;

  for (i = a -> lastdigit; i >= 0; i--) {
    digit_shift( & row, 1);
    row.digits[0] = a -> digits[i];
    c -> digits[i] = 0;
    while (compareBignum( & row, b) != PLUS) {
      c -> digits[i]++;
      subtractBignum( & row, b, & tmp);
      row = tmp;
    }
  }

  zeroJustify(c);

  a -> signbit = asign;
  b -> signbit = bsign;
}
void modulo(bignum * num, bignum * divisor, bignum * remainder) {
  bignum nx1, nx2;
  initializeBignum( & nx1);
  initializeBignum( & nx2);
  divideBignum(num, divisor, & nx1);
  multiplyBignum(divisor, & nx1, & nx2);
  subtractBignum(num, & nx2, remainder);

}
void power(bignum * base, bignum * exponent, bignum * result) {
  bignum zero, one, nx1, nx2;
  convertToBignum(0, & zero);
  convertToBignum(1, & one);
  convertToBignum(1, & nx2);
  nx1 = * exponent;
  while (compareBignum( & zero, exponent) != 0) {
    multiplyBignum( & nx2, base, result);
    subtractBignum( & nx1, & one, exponent);
    nx1 = * exponent;
    nx2 = * result;
  }
}
