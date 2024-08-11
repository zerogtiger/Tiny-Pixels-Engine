#include "schrift.h"
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <stdint.h>

#define _USE_MATH_DEFINES

enum ImageType { PNG, JPG, BMP, TGA };

struct Font;

struct Image {
    uint8_t* data = NULL; // Bytes of image
    size_t size = 0;      // size of data
    int w;
    int h;
    int channels;

    Image(const char* filename);
    Image(int w, int h, int channels);
    Image(const Image& img);
    // Image(const Image& img, int channel);
    ~Image(); // destructor

    bool read(const char* filename);
    bool write(const char* filename);

    ImageType getFileType(const char* filename);

    Image& grayscale_avg();
    Image& grayscale_lum();

    Image& color_mask(float r, float g, float b);

    Image& encodemessage(const char* message);

    Image& decodemessage(char* buffer, size_t* messageLength);

    Image& diffmap(Image& img);
    Image& diffmap_scale(Image& img, uint8_t scl = 0);

    Image& std_convolve_clamp_to_zero(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& std_convolve_clamp_to_border(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& std_convolve_cyclic(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);

    // void std_convolve_clamp_to_zero_double(double* new_data, uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc);
    Image& flip_x();
    Image& flip_y();

    Image& overlay(const Image& src, int x, int y);

    Image& overlay_text(const char* txt, const Font& font, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255);

    Image& crop(uint16_t cx, uint16_t cy, uint16_t cw, uint16_t ch);
    static uint32_t rev(uint32_t n, uint32_t a); // n: 2^a
    static void bit_rev(uint32_t n, std::complex<double> a[], std::complex<double>* A);

    static void fft(uint32_t n, std::complex<double> x[], std::complex<double>* X);
    static void ifft(uint32_t n, std::complex<double> x[], std::complex<double>* X);
    static void dft_2D(uint32_t m, uint32_t n, std::complex<double> x[], std::complex<double>* X);
    static void idft_2D(uint32_t m, uint32_t n, std::complex<double> x[], std::complex<double>* X);

    static void pad_kernel(uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, uint32_t pw, uint32_t ph, std::complex<double>* pad_ker);
    static inline void pointwise_product(uint64_t l, std::complex<double> a[], std::complex<double> b[], std::complex<double>* p);

    Image& fd_convolve_clamp_to_zero(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& fd_convolve_clamp_to_border(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& fd_convolve_cyclic(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);

    Image& convolve_linear(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& convolve_clamp_to_border(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);
    Image& convolve_cyclic(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc, bool normalize = false);

    Image& brightness(uint8_t channel, double brightness_delta);
    Image& contrast(uint8_t channel, double contrast_delta);

    Image& shade_h();
    Image& shade_v();
    Image& shade();
    Image& edge(double detail = 0.09, bool gradient = false);

    Image& resize(uint32_t new_w, uint32_t new_h, bool linked = false);
};

struct Color {
    double r, g, b;
    Color(double r, double g, double b);
    ~Color();

    static void hsv_to_rgb(Color color, double h_deg, double s, double v) {
        if (abs(s) > 1 || abs(v) > 1) {
            throw std::invalid_argument("saturation or value not in the range [0, 1]");
        }
        const int mod = 360;
        h_deg = fmod(h_deg, mod);

        // hsl -> rgb
        double c = (1 - abs(2 * v - 1)) * s;
        double x = c * (1 - abs(fmod((h_deg / 60), 2) - 1));
        double m = v - c / 2;

        double rt, gt, bt;
        rt = gt = bt = 0;
        if (h_deg < 60) {
            rt = c;
            gt = x;
        } else if (h_deg < 120) {
            rt = x;
            gt = c;
        } else if (h_deg < 180) {
            gt = c;
            bt = x;
        } else if (h_deg < 240) {
            gt = x;
            bt = c;
        } else if (h_deg < 300) {
            bt = c;
            rt = x;
        } else {
            bt = x;
            rt = c;
        }

        color.r = 255 * (rt + m);
        color.g = 255 * (gt + m);
        color.b = 255 * (bt + m);
    }
};

struct Font {
    SFT sft = {NULL, 12, 12, 0, 0, SFT_DOWNWARD_Y | SFT_RENDER_IMAGE};
    Font(const char* fontfile, uint16_t size) {
        if ((sft.font = sft_loadfile(fontfile)) == NULL) {
            printf("\e[31m[ERROR] Failed to load %s\e[0m\n", fontfile);
            return;
        }
        setSize(size);
    }

    ~Font() { sft_freefont(sft.font); }

    void setSize(uint16_t size) {
        sft.xScale = size;
        sft.yScale = size;
    }
};
