# BlackNDS — protótipo técnico 01

Projeto experimental de adaptação do BLACK SOULS (RPG Maker VX Ace / RGSS3) para Nintendo DS original.

**Status:** código-fonte inicial, ainda sem build ou teste confirmados. Não é um port funcional do jogo.

## Conteúdo
- `source/main.c`: mapa demonstrativo de caracteres, movimentação pelo direcional, limites e colisão com paredes.
- `Makefile`: compilação nativa para DS usando devkitPro/devkitARM/libnds.
- Os arquivos originais (`Data/`, `Audio/`, `Game.exe`) continuam intactos. O mapa demonstrativo **não** foi extraído do BLACK SOULS.

## Compilar
Instale o toolchain oficial devkitPro para Nintendo DS (`nds-dev`), com `DEVKITPRO` e `DEVKITARM` configurados. Em um terminal compatível com GNU Make:

```sh
cd nds-port
make
```

Saída esperada: `blacknds-prototype.nds`. Se houver falhas, confira a instalação do devkitPro e a versão de libnds.

## Testar
Abra `blacknds-prototype.nds` em um emulador Nintendo DS compatível. Use o direcional para mover `@` e START para voltar à posição inicial. Só depois de validar o funcionamento considere copiar o arquivo para o cartão R4, preservando seu backup.

## Próximas fases
1. Confirmar compilação e execução no emulador/DS.
2. Exportar um mapa real de `Data/MapXXX.rvdata2` com ferramentas apropriadas ao RPG Maker VX Ace e verificar licenças de assets.
3. Criar renderização 2D com tiles e sprites nas telas do DS.
4. Migrar eventos, diálogos, sistemas e salvamento gradualmente.

**Aviso:** scripts RGSS3 e executável Win32 não funcionam diretamente no Nintendo DS.
