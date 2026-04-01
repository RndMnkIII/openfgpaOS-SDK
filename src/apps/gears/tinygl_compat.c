/* tinygl_compat.c
 *
 * Stubs de compatibilidad para símbolos que TinyGL espera de la libc
 * estándar pero que no están disponibles en la libc bare metal del SDK.
 *
 * Símbolos proporcionados:
 *   - isfinite : macro POSIX — implementada comprobando el exponente IEEE 754
 *   - fabs     : versión double — redirigimos a fabsf (float es suficiente)
 *   - pow      : versión double — redirigimos a powf
 *
 * Nota: vfprintf ya no necesita stub porque compilamos TinyGL con
 *       -DNO_DEBUG_OUTPUT, lo que elimina todas las llamadas a
 *       vfprintf/fprintf en msghandling.c.
 */

#include <math.h>

/* ── isfinite ────────────────────────────────────────────────────
 * TinyGL usa isfinite() en zgl.h para descartar vértices NaN/Inf.
 * En bare metal con FPU rv32imafc podemos comprobar el exponente
 * directamente: un float es finito si su exponente no es 0xFF.
 */
int isfinite(double x)
{
    float f = (float)x;
    union { float f; unsigned int u; } v;
    v.f = f;
    return ((v.u & 0x7F800000u) != 0x7F800000u);
}

/* ── fabs ────────────────────────────────────────────────────────
 * TinyGL usa fabs() en zmath.c para comparar magnitudes de doubles.
 * Redirigimos a fabsf — pérdida de precisión inocua para este uso.
 */
double fabs(double x)
{
    return (double)fabsf((float)x);
}

/* ── pow ─────────────────────────────────────────────────────────
 * TinyGL usa pow() en light.c y specbuf.c para el exponente especular.
 * Redirigimos a powf — suficiente para cálculos de iluminación.
 */
double pow(double base, double exponent)
{
    return (double)powf((float)base, (float)exponent);
}

/* ── floor ───────────────────────────────────────────────────────
 * TinyGL usa floor() en image_util.c para interpolación de texturas.
 * Redirigimos a floorf — suficiente para este uso.
 */
double floor(double x)
{
    return (double)floorf((float)x);
}
