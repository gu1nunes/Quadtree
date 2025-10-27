#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>   
#include <math.h>     
#include "lodepng.h"


// Nó do QuadTree
typedef struct Node {
    int color; // -1 = misto; caso contrário 0..255 representa a cor média da região
    struct Node *nw, *ne, *sw, *se;
} Node;

/* ---------- Prototipos ---------- */
Node* buildQuadTree(int **img, int x, int y, int size, int threshold);
bool isUniform(int **img, int x, int y, int size, int threshold, int *outColor);
void reconstructImage(Node *root, int **outImg, int x, int y, int size);
int countNodes(Node *root);
void freeTree(Node *root);
void printRegion(int **img, int size); // utilitario para imprimir matriz pequena

/* ---------- Implementações ---------- */

// Verifica se região é "uniforme" segundo threshold.
// Se uniforme, grava a cor média em *outColor e retorna true.
// threshold = 0 => precisa ser exatamente igual (lossless).
bool isUniform(int **img, int x, int y, int size, int threshold, int *outColor) {
    int minv = INT_MAX, maxv = INT_MIN;
    long sum = 0;
    for (int i = x; i < x + size; i++) {
        for (int j = y; j < y + size; j++) {
            int v = img[i][j];
            if (v < minv) minv = v;
            if (v > maxv) maxv = v;
            sum += v;
        }
    }
    int avg = (int)round((double)sum / (size * size));
    *outColor = avg;
    return (maxv - minv) <= threshold;
}

// Constroi o quadtree recursivamente.
// threshold controla tolerância para considerar região uniforme.
Node* buildQuadTree(int **img, int x, int y, int size, int threshold) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro de alocacao\n");
        exit(1);
    }

    int color;
    if (isUniform(img, x, y, size, threshold, &color)) {
        node->color = color;
        node->nw = node->ne = node->sw = node->se = NULL;
    } else {
        node->color = -1;
        int half = size / 2;
        node->nw = buildQuadTree(img, x, y, half, threshold);
        node->ne = buildQuadTree(img, x, y + half, half, threshold);
        node->sw = buildQuadTree(img, x + half, y, half, threshold);
        node->se = buildQuadTree(img, x + half, y + half, half, threshold);
    }
    return node;
}

// Reconstrói a imagem a partir do quadtree (preenche outImg no retângulo dado).
void reconstructImage(Node *root, int **outImg, int x, int y, int size) {
    if (!root) return;
    if (root->color != -1) {
        // folha: pinta a região com a cor do nó
        for (int i = x; i < x + size; i++)
            for (int j = y; j < y + size; j++)
                outImg[i][j] = root->color;
    } else {
        int half = size / 2;
        reconstructImage(root->nw, outImg, x, y, half);
        reconstructImage(root->ne, outImg, x, y + half, half);
        reconstructImage(root->sw, outImg, x + half, y, half);
        reconstructImage(root->se, outImg, x + half, y + half, half);
    }
}

// Conta nós no Quadtree
int countNodes(Node *root) {
    if (!root) return 0;
    if (root->color != -1) return 1;
    return 1 + countNodes(root->nw) + countNodes(root->ne) + countNodes(root->sw) + countNodes(root->se);
}

// Libera memória
void freeTree(Node *root) {
    if (!root) return;
    freeTree(root->nw);
    freeTree(root->ne);
    freeTree(root->sw);
    freeTree(root->se);
    free(root);
}

// Imprime matriz (útil para debugging/demonstração)
void printRegion(int **img, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%3d ", img[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// ---------- Funções para gerar PNG ---------- //

// Cria imagem RGBA a partir da matriz de tons de cinza
void generateRGBA(int **img, unsigned char *png, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int v = img[i][j];
            if (v < 0) v = 0;
            if (v > 255) v = 255;
            int idx = 4 * (i * size + j);
            png[idx + 0] = v;   // R
            png[idx + 1] = v;   // G
            png[idx + 2] = v;   // B
            png[idx + 3] = 255; // A (opaco)
        }
    }
}

// Salva PNG em tons de cinza (a partir da matriz reconstruída)
void saveImagePNG(int **img, int size, const char *filename) {
    unsigned char *png = malloc(4 * size * size);
    if (!png) {
        fprintf(stderr, "Erro de alocacao PNG\n");
        return;
    }
    generateRGBA(img, png, size);

    unsigned error = lodepng_encode32_file(filename, png, size, size);
    if (error)
        printf("Erro ao salvar %s: %u: %s\n", filename, error, lodepng_error_text(error));
    else
        printf("Imagem salva como '%s'\n", filename);

    free(png);
}


int main() {
    // Exemplo 8x8 
    int size = 8;

    int **img = malloc(size * sizeof(int*));
    int **out = malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        img[i] = malloc(size * sizeof(int));
        out[i] = malloc(size * sizeof(int));
    }

    int sample[8][8] = {
        {255,253,250,255,  10, 12,  8,  9},
        {254,255,255,250,  12,  9,  7, 11},
        {253,254,255,255,  11, 10,  8, 10},
        {255,255,253,254,  10, 11,  9,  9},
        { 10, 11, 12, 10, 200,198,203,199},
        { 10,  9, 11, 12, 201,200,198,202},
        { 11, 10,  9,  9, 202,200,201,203},
        { 10,  9, 11, 12, 199,200,200,201}
    };
    
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            img[i][j] = sample[i][j];

    printf("Imagem original (matriz %dx%d):\n", size, size);
    printRegion(img, size);

    // Monta quadtree com threshold = 0 (lossless)
    int threshold = 0;
    Node *root_lossless = buildQuadTree(img, 0, 0, size, threshold);
    printf("Quantidade de nos (lossless, threshold=0): %d\n", countNodes(root_lossless));

    // Reconstrói e mostra
    reconstructImage(root_lossless, out, 0, 0, size);
    printf("Reconstrucao sem perdas (lossless):\n");
    printRegion(out, size);
    saveImagePNG(out, size, "saida_lossless.png");

    // Agora um exemplo com threshold = 30 (permitir variação => compressão mais agressiva)
    int threshold2 = 30;
    Node *root_lossy = buildQuadTree(img, 0, 0, size, threshold2);
    printf("Quantidade de nos (lossy, threshold=%d): %d\n", threshold2, countNodes(root_lossy));
    reconstructImage(root_lossy, out, 0, 0, size);
    printf("Reconstrucao com perdas (lossy):\n");
    printRegion(out, size);
    saveImagePNG(out, size, "saida_lossy.png");

    // Limpeza
    freeTree(root_lossless);
    freeTree(root_lossy);
    for (int i = 0; i < size; i++) {
        free(img[i]);
        free(out[i]);
    }
    free(img);
    free(out);

    return 0;
}
