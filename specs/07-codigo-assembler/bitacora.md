# Bitácora — 07-codigo-assembler

| Fecha | Quién | Qué se hizo / decidió | Notas |
|-------|-------|-----------------------|-------|
|       |       |                       |       |
**Iteración 1 — Integración y Generación Inicial de Código Assembler**
**Fecha:** 18/11/2026 · **Modelo:** GitHub Copilot

- **Qué pedí:** Generar el código assembler en NASM (win32) basándote estrictamente en la especificación de specs/07-codigo-assembler, tomando como entrada el archivo intermedio out/polaca.txt y teniendo en cuenta todas las especificaciones y reglas del repositorio para asegurar la compatibilidad total.
- **Qué se hizo:**
  - Se conectó el módulo assembler.c dentro del flujo de compilación para que procese la Polaca Inversa generada en out/polaca.txt.
  - Se realizaron pruebas iniciales, logrando generar la estructura base del archivo NASM.
  - Se detectó un error ("Asignación inválida en paso 48") al procesar el bucle `para`. El assembler esperaba una estructura distinta a la que envía el código intermedio (`[i] [1] [-] [:=]`).
- **Decisiones tomadas:** Como el código intermedio (generado por el parser) funciona correctamente y respeta el diseño original, se decidió mantenerlo intacto y modificar la especificación del assembler para que la IA sepa interpretar esa constante numérica como un decremento dinámico.
- **Impacto:** Se identificó con precisión el desajuste entre la especificación del assembler y la polaca real, permitiendo planificar la corrección sobre el generador de assembler sin romper componentes previos.
- **Estado al cierre de la iteración:**
  - El compilador procesa el código hasta encontrar el bucle `para`, donde se detiene por la discrepancia detectada.
  - **Pendiente de verificación:** Actualizar el documento de especificación del assembler y regenerar el código para validar la solución completa.
