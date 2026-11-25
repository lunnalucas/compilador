# Bitácora — 07-codigo-assembler

| Fecha | Quién | Qué se hizo / decidió | Notas |
|-------|-------|-----------------------|-------|


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

**Iteración 2 — Actualización e Implementación Final del Módulo Assembler**
**Fecha:** 18/11/2026 · **Modelo:** GitHub Copilot

- **Qué pedí:** @workspace Actualiza e implementa "assembler.c" basándote en la especificación actualizada en "specs/07-codigo-assembler" para que genere correctamente el archivo "assembler.asm".
- **Qué se hizo:**
  - Se actualizó y ajustó el código fuente de `assembler.c` para corregir la generación del archivo `assembler.asm` basándose en la especificación actualizada.
  - Se probó el pipeline completo de punta a punta, logrando compilar con éxito los scripts `.beta`, generar el código intermedio y el ensamblador, ensamblar con NASM y enlazar con GCC hasta obtener los ejecutables finales.
  - Se validaron todos los casos de prueba correctos y los casos de error, comprobando la interrupción correcta del parser.
  
- **Impacto:** El compilador quedó completamente operativo, estable y validado de punta a punta, cumpliendo con todos los requisitos funcionales y de versionado para la presentación.
- **Estado al cierre de la iteración:**
  - Pipeline de compilación 100% funcional (Bison → Parser C → NASM → GCC Linker → Ejecutable).
  - Repositorio limpio, sincronizado y listo para la defensa del proyecto.


**Iteración 3 — Mostrar por pantalla solo la salida del programa**

Se agrego en la especificacion que el compilador solo debe mostrar la salida de un programa beta si es compilado correctamente, en caso contrario se debe informar el error.

En el estado anterior se mostraba ademas el debug del parser y la traza de assembler que mostraba el proceso de traduccion de la polaca a assembler.

- **Estado al cierre de la iteración:** Compilador funcionando correctamente sin informar por consola debug del parser y traduccion de polaca a assembler.