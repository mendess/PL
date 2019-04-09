# Fases de desenvolvimento

## No repeat checking
Primeiramente nao estava a ser verificado se havia artigos repetido e como tal a contagem das tags estava errada (No tag checking)

## Add <Post, Tag> Table
Adicionando uma tabela para tomar conta das tags de cada post incorreu num custo significativo de memoria necessaria, mas nao afetou performance

## Add Index
Adicionando um indice para listar todas noticias. Mais uma vez o custo de memoria deste foi mantido ao minimo, ate conseguindo que o pico maximo utilizado pela heap fosse o menor possivel

## Refactor
Nesta fase foi feita uma reestruturação do projecto que teve um aumento significativo de performance mudando apenas uma expressão regular. Esta é usada para capturar a categoria e o titulo de cada noticia.

Antes
```
-AN   [0-9A-Za-zÀ-ÖØ-öø-ÿ\-]
-ANS  [0-9A-Za-zÀ-ÖØ-öø-ÿ\- ]
-<HEADER>{AN}{ANS}+\n            {
```
Depois
```
+<HEADER>^[^\n#<━][^{\n]+\n      {
```
Ao criar um autómato mais simples para esta procura, tendo uma lista mais curta de caracteres com que comparar, foi possivel cortar um segundo ao tempo de execucao do programa (de 2.5s passou para 1.5s).
