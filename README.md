# Probador de PLA del Commodore 64

Probador estático en **C++ para Arduino UNO R3 (ATmega328P, 5 V)** y **dos SN74HC595N** en cascada. Comprueba una **N82S100N programada como PLA estándar del C64**, retirada del computador; no programa el chip.

Antes de alimentar el montaje, sigue [CONEXIONES.md](CONEXIONES.md): incluye materiales, alimentación, resistencias, tablas pin a pin y el buffer HCT244 opcional para resolver la compatibilidad de umbrales TTL.

## Estructura

```text
.
├── README.md                         # Carga, ejecución y resultados
├── CONEXIONES.md                     # Montaje completo
├── .gitignore                        # Excluye resultados y compilaciones
└── firmware/
    └── pla_tester/
        ├── pla_tester.ino            # Programa Arduino
        └── pla_logic.h              # Modelo combinacional esperado
```

La carpeta del sketch y el archivo `.ino` comparten nombre para que Arduino IDE y Arduino CLI los reconozcan. `resultados/` se crea localmente al guardar informes y se excluye de Git.

## Requisitos

- Montaje descrito en [CONEXIONES.md](CONEXIONES.md) y cable USB de datos.
- Arduino IDE con el paquete **Arduino AVR Boards**, o Arduino CLI con el core `arduino:avr`.
- Un monitor serie; `picocom` es una opción para guardar registros desde Artix Linux.

No requiere bibliotecas adicionales ni Python.

## Compilar y cargar

Con Arduino IDE, abre `firmware/pla_tester/pla_tester.ino`, selecciona **Arduino UNO** y el puerto correspondiente, y carga el programa. No requiere bibliotecas adicionales.

También puedes usar Arduino CLI instalado según su [guía oficial](https://docs.arduino.cc/arduino-cli/installation/). Ejecuta los comandos desde la carpeta que contiene este README (`pla_tester`):

```bash
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli board list
arduino-cli compile --fqbn arduino:avr:uno firmware/pla_tester
# Cambia el puerto según board list:
export PLA_PORT=/dev/ttyACM0
arduino-cli upload -p "$PLA_PORT" --fqbn arduino:avr:uno firmware/pla_tester
arduino-cli monitor -p "$PLA_PORT" --config baudrate=115200
```

En Artix, si falta acceso al puerto, consulta `stat -c '%G %A' "$PLA_PORT"` e `id`. Si el grupo del dispositivo es `uucp`, agrega tu usuario con `sudo usermod -aG uucp "$(id -un)"` y vuelve a iniciar sesión. Cierra cualquier otro monitor antes de cargar o abrir el puerto.

## Ejecutar y guardar el informe

Abre el monitor a **115200 baudios** y envía `t`. La prueba comienza únicamente al recibir esa letra; abrir el puerto puede reiniciar el UNO. Un final de línea opcional no afecta la orden. Envía `x` para interrumpir conservando un resumen parcial, o `t` al terminar para repetir. Ctrl+C cierra el monitor y no equivale a enviar `x`.

Para guardar el informe en Artix puedes usar un monitor con registro, como `picocom`:

```bash
sudo pacman -S --needed picocom
mkdir -p resultados
picocom --baud 115200 --logfile "resultados/pla-$(date +%Y%m%d-%H%M%S).log" "$PLA_PORT"
```

Escribe `t` después de abrirlo. Para salir de picocom usa Ctrl+A y después Ctrl+X. El UNO **no guarda informes persistentes**: el registro queda en el PC. No ejecutes dos monitores simultáneos. Una desconexión o reset interrumpe la prueba; un registro sin resumen final no acredita aprobación.

Se recorren las 65.536 combinaciones en orden ascendente y descendente: **131.072 vectores**, con dos lecturas por vector separadas por 50 µs y otros 50 µs tras actualizar entradas. La duración supera los 13 segundos mínimos de espera por el desplazamiento, las lecturas y la salida serie; espera el resumen final. Se detallan hasta 100 fallos; los contadores incluyen todos.

| Resultado | Interpretación |
| --- | --- |
| `PASS_ESTATICO` | Todos los vectores coincidieron en ambas lecturas. |
| `FAIL` | Hubo discrepancias: revisa cableado, alimentación, contactos y umbrales antes de atribuirlas a la PLA. |
| `INTERRUMPIDO` | Prueba parcial detenida con `x`. |

`entrada`, `esperado`, `lectura1`, `lectura2` y `xor` se imprimen en hexadecimal. `xor=01` señala el bit 0, `/CASRAM`; `xor=20` señala `/IO`. Se cuentan vectores inestables cuando las dos lecturas difieren y errores por salida. Los conteos altas/bajas corresponden a la segunda lectura.

Es una prueba estática con `/OE` de la PLA a GND: no mide retardos, glitches, alta impedancia, comportamiento térmico ni carga real del C64. Un aprobado no certifica funcionamiento dentro del computador.

## Estado de validación

El firmware fue compilado y enlazado para ATmega328P a 16 MHz con Arduino AVR core 1.8.8. El modelo C++ fue contrastado previamente con las 65.536 combinaciones del JEDEC publicado; ese verificador no forma parte de esta carpeta. Estas comprobaciones de software no validan el montaje físico.

## Subir a GitHub

Usa **esta carpeta como raíz del repositorio**. Incluye `README.md`, `CONEXIONES.md`, `.gitignore` y `firmware/`. Los informes existentes se conservan localmente y quedan ignorados, junto con los archivos de compilación. No hace falta incluir los documentos de la carpeta superior.

Consulta las fuentes técnicas enlazadas al final de [CONEXIONES.md](CONEXIONES.md).
