#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convenient names for integer types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

// Some basic utility macros/functions
#define UNUSED __attribute__((unused))
// #define ARRAY_SIZE(arr) ((i64) (sizeof(arr) / sizeof((arr)[0])))
// #define streq(a, b) (!strcmp((a), (b)))
#define ATOM_AT(coords) (atoms[(coords).y][(coords).x])

template <class T> constexpr T sign(T x)       { return (x > 0) - (x < 0); }
template <class T> constexpr T abs(T x)        { return x < 0 ? -x : x; }
template <class T> constexpr T min(T x, i64 y) { return x < (T) y ? x : (T) y; }
template <class T> constexpr T max(T x, i64 y) { return x > (T) y ? x : (T) y; }

struct Arm;
typedef void (*Instruction)(Arm*);

struct Coords {
	i32 x;
	i32 y;

	void operator+=(const Coords& that)                  { x += that.x; y += that.y; }
	void operator-=(const Coords& that)                  { x -= that.x; y -= that.y; }
	constexpr Coords operator-()                   const { return { -x, -y }; }
	constexpr Coords operator+(const Coords &that) const { return { x + that.x, y + that.y }; }
	constexpr Coords operator-(const Coords &that) const { return { x - that.x, y - that.y }; }
	constexpr Coords operator*(i8 scalar)          const { return { x * scalar, y * scalar }; }
	constexpr bool operator==(const Coords &that)  const { return x == that.x && y == that.y; }
	constexpr bool operator!=(const Coords &that)  const { return x != that.x || y != that.y; }
};
constexpr Coords direction(Coords d) { return {sign(d.x), sign(d.y)}; }
constexpr Coords cardinal(Coords d) { return {sign(d.x), d.x ? 0 : sign(d.y)}; }

enum Atom {
	NONE,
	SALT,
	AIR,
	EARTH,
	FIRE,
	WATER,
	MERCURY,
	GOLD,
	SILVER,
	COPPER,
	IRON,
	TIN,
	LEAD,
	LIFE,
	DEATH,
	REPEAT,
};

enum Direction {
	LEFT,
	UP_LEFT,
	UP_RIGHT,
	RIGHT,
	DOWN_RIGHT,
	DOWN_LEFT,
};

// y += x -= y;
// RIGHT,
// LEFT,
// SQUIGGLE_RIGHT,
// SQUIGGLE_LEFT,
// PLUS,
// MINUS,

struct Arm {
	Coords pos;
	Coords d;
	bool grab;
	Instruction code[8];
};

static const char* atom_names[] = {
	" ",
	"ө",
	"\033[36;9m▲\033[m",
	"\033[32;9m▼\033[m",
	"\033[31m▲\033[m",
	"\033[34m▼\033[m",
};

static Atom atoms[32][32];
static Arm arms[8];

static void move_atom(Coords start, Coords end) {
	ATOM_AT(end) = ATOM_AT(start);
	ATOM_AT(start) = NONE;
}

static void nop(UNUSED Arm* arm) { }
static void grab(Arm* arm)    { arm->grab = true; } 
static void release(Arm* arm) { arm->grab = false; } 

static void extend(Arm* arm)  {
	Coords start = arm->pos + arm->d;
	arm->d += direction(arm->d);
	if (arm->grab)
		move_atom(start, arm->pos + arm->d);
} 

static void retract(Arm* arm) {
	Coords start = arm->pos + arm->d;
	arm->d -= direction(arm->d);
	if (arm->grab)
		move_atom(start, arm->pos + arm->d);
}

// END .h

static void run_cycle(i64 cycle)
{
	for (i64 i = 0; arms[i].code[0]; ++i)
		arms[i].code[cycle](arms + i);

	for (i64 y = 10; y < 20; ++y) {
		for (i64 x = 0; x < 32; ++x)
			printf("%s", atom_names[atoms[y][x]]);
		putchar('\n');
	}
	putchar('\n');
}

int main(void)
{
	printf("  %s───%s\n", atom_names[FIRE], atom_names[AIR]);
	printf(" ╱ ╲ ╱ ╲\n");
	printf("%s───%s───%s\n", atom_names[SALT], atom_names[SALT], atom_names[SALT]);
	printf(" ╲ ╱ ╲ ╱\n");
	printf("  %s───%s\n", atom_names[EARTH], atom_names[WATER]);

	atoms[16][16] = SALT;
	arms[0] = {
		{ 15, 16 },
		{ 1, 0 },
		false,
		{ grab, extend, release, retract, nop, nop, nop, nop },
	};

	for (i64 i = 0; i < 8; ++i)
		run_cycle(i);
}
