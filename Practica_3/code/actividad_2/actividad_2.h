#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h" 

// Structs & Variables nedeed
static struct termios g_old_term;
static int g_raw = 0;
typedef struct {
    FILE    *fp;
    uint32_t data_bytes;
} WavWriter;

#define FUNCTION_CAST(T, ptr) (T)(ptr)

static LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT;
static LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
static LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT;
static LPALCGETSTRINGISOFT               alcGetStringiSOFT;
static LPALCRESETDEVICESOFT             alcResetDeviceSOFT;

#define SAMPLE_RATE      44100
#define NUM_CHANNELS     2                      /* HRTF siempre produce estéreo    */
#define BYTES_PER_SAMPLE 4                      /* float32                         */
#define CHUNK_FRAMES     (SAMPLE_RATE / 100)    /* ~10 ms por iteración */

const char *saveRute = "bin/E18_audio/hrtf_output.wav";


// =============================================================
//  SECCION 1 — Manejo de archivos WAV
// =============================================================
static int      wav_open(WavWriter *w, const char *path, int rate, int ch, int bits);
static void     wav_write(WavWriter *w, const float *buf, int frames, int channels);
static void     wav_close(WavWriter *w, int channels);

// =============================================================
//  SECCION 2 — Utilidades y helpers
// =============================================================
static void     write_u16le(FILE *fp, uint16_t v);
static void     write_u32le(FILE *fp, uint32_t v);
static void     term_raw(void);
static int      key_pressed(void);

// =============================================================
//  SECCION 3 — OpenAL: creación de buffers y HRTF
// =============================================================
static ALuint   create_sine_buffer(void);
static int      enable_hrtf(ALCdevice *dev, const char *hrtf_name);

// =============================================================
//  SECCION 4 — Función principal: guardar audio
// =============================================================
int save_audio(ALuint buffer[]);
    // 1. Verificar extensiones
    // 2. Crear dispositivo real (ctx A)
    // 3. Crear dispositivo loopback (ctx B)
    // 4. Crear fuente A (monitorización)
    // 5. Crear fuente B (captura)
    // 6. Abrir WAV
    // 7. Bucle principal:
    //      - actualizar posición
    //      - render loopback
    //-      - wav_write(...)
    //-      - actualizar salida real
    // 8. Cerrar WAV
    // 9. Liberar todo
