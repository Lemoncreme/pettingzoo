#include <stdio.h>
#include <stdlib.h>
#include <chromosome.hpp>
#include <stdint.h>

float gen_random_weight();

uint8_t *generate_chromosome(uint8_t in_h, uint8_t in_w, uint8_t hlc, uint16_t npl)
{
    uint8_t *chrom = NULL;
    uint8_t *cur_uint;
    float *cur_float;
    int r, c, hl;
    struct params params;

    params.in_w = in_w;
    params.in_h = in_h;
    params.npl = npl;
    params.hlc = hlc;

    chrom = (uint8_t *)malloc(get_chromosome_size(params));

    chrom[0] = in_w;
    chrom[1] = in_h;
    *((uint16_t *)chrom + 1) = npl; // Bytes 2-3
    chrom[4] = hlc;

    cur_uint = locate_input_act(chrom);
    // Generate input act matrix
    // This is NOT transposed
    for (r = 0; r < in_h; r++) {
        for (c = 0; c < in_w; c++) {
            *cur_uint = random() % 2;
            cur_uint++;
        }
    }

    // Generate input adj matrix
    cur_float = locate_input_adj(chrom);
    for (r = 0; r < npl; r++) {
        for (c = 0; c < in_h * in_w; c++) {
            *cur_float = gen_random_weight();
            cur_float++;
        }
    }

    // Generate hidden act matrix
    cur_uint = locate_hidden_act(chrom);
    for (r = 0; r < hlc; r++) {
        for (c = 0; c < npl; c++) {
            *cur_uint = random() % 2;
            cur_uint++;
        }
    }

    // Generate hidden adj matrices
    for (hl = 0; hl < hlc - 1; hl++) {
        cur_float = locate_hidden_adj(chrom, hl);
        for (r = 0; r < npl; r++) {
            for (c = 0; c < npl; c++) {
                *cur_float = gen_random_weight();
                cur_float++;
            }
        }
    }

    // Generate out adj matrix
    cur_float = locate_out_adj(chrom);
    for (r = 0; r < BUTTON_COUNT; r++) {
        for (c = 0; c < npl; c++) {
            *cur_float = gen_random_weight();
            cur_float++;
        }
    }

    return chrom;
}

void print_chromosome(uint8_t *chrom)
{
    printf("-------------------------------------------\n");
    uint8_t *cur_uint;
    float *cur_float;
    int r, c, hl;
    struct params params;

    get_params(chrom, &params);

    printf("Input activation:\n");
    cur_uint = locate_input_act(chrom);
    for (r = 0; r < params.in_h; r++) {
        for (c = 0; c < params.in_w; c++) {
            printf("%d\t", *cur_uint);
            cur_uint++;
        }
        puts("");
    }

    printf("\nInput to first hidden layer adj:\n");
    cur_float = locate_input_adj(chrom);
    for (r = 0; r < params.npl; r++) {
        for (c = 0; c < params.in_h * params.in_w; c++) {
            printf("%*.3lf\t", 6, *cur_float);
            cur_float++;
        }
        puts("");
    }

    printf("\nHidden layers activation:\n");
    cur_uint = locate_hidden_act(chrom);
    for (r = 0; r < params.hlc; r++) {
        for (c = 0; c < params.npl; c++) {
            printf("%d\t", *cur_uint);
            cur_uint++;
        }
        puts("");
    }

    puts("");
    for (hl = 0; hl < params.hlc - 1; hl++) {
        cur_float = locate_hidden_adj(chrom, hl);
        printf("Hidden layer %d to %d act:\n", hl + 1, hl + 2);
        for (r = 0; r < params.npl; r++) {
            for (c = 0; c < params.npl; c++) {
                printf("%*.3lf\t", 6, *cur_float);
                cur_float++;
            }
            puts("");
        }
        puts("");
    }

    printf("Hidden layer %d to output act:\n", params.hlc);
    cur_float = locate_out_adj(chrom);
    for (r = 0; r < BUTTON_COUNT; r++) {
        for (c = 0; c < params.npl; c++) {
            printf("%*.3lf\t", 6, *cur_float);
            cur_float++;
        }
        puts("");
    }

    printf("\nChromosome:\n");
    printf("in_w:\t%d\nin_h:\t%d\nnpl:\t%d\nhlc:\t%d\n", params.in_h, params.in_w, params.npl, params.hlc);
    printf("\nTotal size: %0.2lfKB\n", get_chromosome_size(params) / 1000.0f);
    printf("-------------------------------------------\n");
}

uint8_t *locate_input_act(uint8_t *chrom)
{
    return chrom + 5;
}

float *locate_input_adj(uint8_t *chrom)
{
    uint8_t in_w, in_h;
    int loc;

    in_w = chrom[0];
    in_h = chrom[1];

    loc = 5 + in_h * in_w;

    return (float *)(chrom + loc);;
}

uint8_t *locate_hidden_act(uint8_t *chrom)
{
    uint8_t in_w, in_h;
    uint16_t npl;
    int loc;

    in_w = chrom[0];
    in_h = chrom[1];
    npl = *((uint16_t *)chrom + 1);

    loc = 5 + in_h * in_w + in_h * in_w * npl * 4;

    return chrom + loc;
}

// Zero indexed. num = 0 is first layer
float *locate_hidden_adj(uint8_t *chrom, int num)
{
    uint8_t in_w, in_h, hlc;
    uint16_t npl;
    int loc;

    in_w = chrom[0];
    in_h = chrom[1];
    npl = *((uint16_t *)chrom + 1);
    hlc = chrom[4];

    loc = 5 + in_h * in_w + in_h * in_w * npl * 4 + npl * hlc + num * (npl * npl) * 4;

    return (float *)(chrom + loc);
}

float *locate_out_adj(uint8_t *chrom)
{
    uint8_t in_w, in_h, hlc;
    uint16_t npl;
    int loc;

    in_w = chrom[0];
    in_h = chrom[1];
    npl = *((uint16_t *)chrom + 1);
    hlc = chrom[4];

    loc = 5 + in_h * in_w + in_h * in_w * npl * 4 + npl * hlc + (hlc - 1) * (npl * npl) * 4;

    return (float *)(chrom + loc);
}

size_t get_chromosome_size(struct params params)
{
    size_t size;
    uint8_t in_h, in_w, hlc;
    uint16_t npl;

    in_h = params.in_h;
    in_w = params.in_w;
    npl = params.npl;
    hlc = params.hlc;

    // Algebra to reduce length of this line
    size = 5 + in_h * (in_w + 4 * in_w * npl) + npl * (4 * npl * (-1 + hlc) + hlc + 4 * BUTTON_COUNT);

    return size;
}

float gen_random_weight()
{
    float weight;
    
    weight = (float)drand48();

    // Connection is inactive
    if ((float)drand48() > 0.9f)
        return 0.0f;

    if (random() % 2 == 0)
        return weight * -1;

    return weight;
}

void get_params(uint8_t *chrom, struct params *params)
{
    params->in_w = chrom[0];
    params->in_h = chrom[1];
    params->npl = *((uint16_t *)chrom + 1);
    params->hlc = chrom[4];
}