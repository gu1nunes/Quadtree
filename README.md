# QuadTree - Compressão de Imagens

Implementação em C da estrutura de dados **QuadTree**, aplicada à **compressão de imagens em tons de cinza**.

Este projeto demonstra como uma Quadtree pode ser usada para comprimir uma imagem dividindo-a recursivamente em quadrantes, com base em um limiar (threshold) de variação de cor.

## 🔍 Funcionalidades
- Construção recursiva da QuadTree.
- Modos **lossless** (sem perdas) e **lossy** (com perdas) controlados pelo `threshold`.
- Reconstrução da imagem e geração de arquivos **PNG** a partir da árvore.
- Visualização opcional das divisões da árvore (bordas pretas) no modo `lossy`.

## 🧠 Tecnologias
- Linguagem C
- Biblioteca [LodePNG](https://github.com/lvandeve/lodepng) (para salvar os arquivos `.png`)

## 📸 Resultados

Os testes foram executados com uma imagem 64x64 gerada automaticamente com quatro quadrantes de cores diferentes, contendo ruído aleatório.

| Modo | Threshold | Nós Gerados | Exemplo PNG |
|:---|:---|:---|:---|
| Lossless | 0 | 5461 | `saida_lossless.png` |
| Lossy | 30 | 5 | `saida_lossy.png` |

### Análise dos Resultados
É importante notar que o modo `Lossless` (`threshold = 0`) gerou **5461 nós**, um número maior que o total de pixels da imagem (64x64 = 4096).

Isso não é um erro: como a imagem de teste possui ruído, a árvore é forçada a se dividir até o nível máximo (blocos de 1x1) para representar cada pixel individual. O total de 5461 nós é a soma das 4096 folhas (uma para cada pixel) e os 1365 nós internos necessários para organizar a árvore.

Isso demonstra que a Quadtree *lossless* não é eficiente para comprimir imagens com ruído (como fotos reais). A verdadeira vantagem da estrutura para compressão está no modo **Lossy** (`threshold > 0`), que reduz drasticamente o número de nós ao agrupar regiões de cor similar.

## 🧩 Execução

**Pré-requisito:** Você precisa dos arquivos `lodepng.c` e `lodepng.h` no mesmo diretório do projeto.

1.  **Compilar:**
    ```bash
    gcc Quadtree.c lodepng.c -o Quadtree -O2 -lm
    ```

2.  **Executar:**
    ```bash
    ./Quadtree
    ```

3.  **Saída:**
    O programa irá gerar três arquivos: `saida_original.png`, `saida_lossless.png`, e `saida_lossy.png`.

---


**Universidade Federal de Alagoas – UFAL**

![Logo IC](https://user-images.githubusercontent.com/91018438/204195385-acc6fcd4-05a7-4f25-87d1-cb7d5cc5c852.png)

**Disciplina:** Estruturas de Dados

**Autores:**
* Jader Rogerio dos Santos Neto
* Guilherme Nunes Alves
* Carlos Antunis Bonfim de Silva Santos
* Pedro Henrique Santos da Silva
* Carlos Leonardo Rodrigues Novaes Carvalho