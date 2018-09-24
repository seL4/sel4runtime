#define ALIGN_UP(x, n) (((x) + (n) - 1) & ~((n) - 1))
#define ALIGN_DOWN(x, n) ((x) & ~((n) - 1))

#define ROUND_DOWN(n, b) \
    ({ typeof (n) _n = (n); \
       typeof (b) _b = (b); \
       _n - (_n % _b); \
    })

#define ROUND_UP(n, b) \
    ({ typeof (n) _n = (n); \
       typeof (b) _b = (b); \
       (_n + (_n % _b == 0 ? 0 : (_b - (_n % _b)))); \
    })
