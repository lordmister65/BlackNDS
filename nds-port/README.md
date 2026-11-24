# BlackNDS — protótipo nativo 0.4 (Nintendo DS)

Port experimental de **BLACK SOULS** (original RPG Maker VX Ace / RGSS3) para Nintendo DS. **Ainda não contém mapas, personagens nem recursos extraídos do BLACK SOULS.** O mapa atual é inteiramente demonstrativo e foi criado apenas para construir a engine de DS.

## Implementado na versão 0.4

- Tela superior: mapa colorido desenhado em bitmap 16-bit, blocos 16x16, câmera que acompanha o personagem e colisão.
- Tela inferior: painel de informações, vida, ouro, poções e instruções.
- Exploração de um mapa de demonstração de 32x24 células; NPC interativo, baú de recompensa, fonte de cura e encontro.
- Menu de inventário e poções.
- Combate simples de turnos, com ataque, poções, fuga e recompensa.

### Controles

- **Direcional:** mover
- **A:** interagir; atacar no combate; usar poção no inventário
- **B:** voltar / fugir
- **START:** inventário
- **SELECT:** combate de demonstração
- **X:** poção durante combate

### Compilação

O workflow `.github/workflows/build-nds.yml` compila `nds-port/Makefile` com **devkitARM/libnds** e publica `blacknds-prototype.nds`. Ele também publica `blacknds-r4-check.nds`, ROM separada de diagnóstico de inicialização.

**R4 Gold Pro SDHC 2026 (r4isdhc.com):** o console físico relatou travamento em `Loading` tanto no protótipo anterior quanto na ROM mínima de diagnóstico. Assim, a execução do build 0.4 no R4 **não está confirmada**. Teste primeiro no emulador melonDS. Não formate o microSD nem substitua o kernel do R4 sem backup.

### Próximas etapas reais do port

1. Converter dados de mapas RPG Maker VX Ace `Data/Map*.rvdata2` em formato que a engine do DS possa ler, preservando seus layouts originais.
2. Implementar sistema de tilesets/sprites adaptados à memória do hardware.
3. Migrar diálogos, eventos e progressão do jogo, ajustando o que o DS original consegue executar.
4. Implementar áudio e salvamento.
5. Resolver a inicialização no flashcart em hardware real.

**Importante:** o código RGSS3 do VX Ace e o executável do Windows não rodam diretamente no Nintendo DS. É necessária a reimplementação da engine e conversão dos recursos, respeitando os direitos sobre a obra e seus assets.

### Baixar uma nova compilação

No GitHub, abra **Actions → Compilar BlackNDS e diagnostico R4**. Escolha a execução mais recente que esteja marcada como **success**. Na seção **Artifacts**, baixe **BlackNDS-Prototipo-DS** para obter `blacknds-prototype.nds`; o pacote **BlackNDS-R4-Check-0.3** contém apenas a ROM de diagnóstico. Extraia o ZIP antes de abrir a ROM no melonDS ou copiá-la para o microSD. Para compilar novamente sem alterações de código, use **Run workflow** na aba Actions.
