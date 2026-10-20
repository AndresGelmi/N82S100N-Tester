# Conexiones del probador de PLA

Montaje para **Arduino UNO R3 de 5 V**, dos **SN74HC595N DIP-16** y una **N82S100N DIP-28 programada para C64**. Las tablas coinciden con los pines del [firmware](firmware/pla_tester/pla_tester.ino). Para cargarlo y ejecutar la prueba, consulta el [README](README.md).

## Materiales y alimentación

- Arduino UNO R3, cable USB de datos y dos SN74HC595N DIP-16 (U1 y U2).
- PLA, zócalo DIP-28 ancho, protoboard y cables cortos.
- Tres condensadores de 100 nF, uno junto a cada integrado externo; uno de 10 µF en el riel de 5 V.
- Una resistencia de 10 kΩ para `/OE` y 16 de 10 kΩ para las entradas de la PLA.

Monta y modifica el circuito sin alimentación. La PLA debe estar **fuera del C64**. Alimenta todo desde el USB del UNO: pin **5V** al riel de 5 V y **GND** a masa común; no uses VIN, 3,3 V ni otra fuente simultánea. Comprueba 4,75–5,25 V bajo carga y la orientación de los chips antes de probar. Numeración vista desde arriba, muesca arriba, pin 1 arriba a la izquierda, en sentido antihorario.

**Limitación de la lectura directa:** compartir alimentación de 5 V evita sobretensión, pero no garantiza compatibilidad de umbrales. El 82S100 garantiza un alto TTL desde 2,4 V y el ATmega328P requiere 0,6 × VCC (3 V a 5 V) en estos GPIO. El montaje directo puede funcionar con el chip concreto, pero puede producir falsos fallos. Para una lectura garantizada añade un **74HCT244 a 5 V** entre las ocho salidas de la PLA y el UNO: VCC pin 20, GND pin 10, habilitaciones pines 1 y 19 a GND y 100 nF de desacoplo. Usa los pares entrada→salida 2→18, 4→16, 6→14, 8→12, 11→9, 13→7, 15→5 y 17→3, en el orden de la tabla de salidas. Debe ser **HCT**, con umbrales TTL. Los dos 595 siguen controlando las entradas.

## Cableado de control

U1 es el registro más cercano al UNO; U2 recibe los datos de U1.

| Conexión | Destino |
| --- | --- |
| UNO D11 | U1 SER, pin 14 |
| U1 QH′, pin 9 | U2 SER, pin 14 |
| UNO D13 | SRCLK, pin 11 de ambos 595 |
| UNO D10 | RCLK/latch, pin 12 de ambos |
| UNO D12 | `/OE`, pin 13 de ambos; resistencia de 10 kΩ de esta línea a 5 V |
| 5 V | VCC pin 16 y `/SRCLR` pin 10 de ambos |
| GND | Pin 8 de ambos |
| U2 QH′, pin 9 | Sin conectar |
| PLA pin 28 | 5 V |
| PLA pin 14 | GND |
| PLA pin 1, FE | GND |
| PLA pin 19, `/OE` | GND |

El pull-up en `/OE` deshabilita las salidas de los 595 durante el reset. Añade una resistencia de **10 kΩ a GND en cada una de las 16 entradas de la PLA** para mantenerlas definidas en ese intervalo. El firmware carga cero antes de habilitar los registros. `/SRCLR` borra el registro de desplazamiento; por sí solo no inicializa el latch de salida.

## Entradas de la PLA

QA corresponde al bit menor de cada byte. El firmware envía primero el byte alto, después el bajo, ambos MSB primero, y finalmente pulsa el latch: U1 conserva bits 0–7 y U2 bits 8–15. Las salidas permanecen estables mientras se desplazan los datos.

| Bit | Señal | Pin PLA | Registro | Salida 595 (pin) |
| --- | --- | --- | --- | --- |
| 0 | A13 | 2 | U1 | QA (15) |
| 1 | A14 | 3 | U1 | QB (1) |
| 2 | A15 | 4 | U1 | QC (2) |
| 3 | /VA14 | 5 | U1 | QD (3) |
| 4 | CHAREN | 6 | U1 | QE (4) |
| 5 | HIRAM | 7 | U1 | QF (5) |
| 6 | LORAM | 8 | U1 | QG (6) |
| 7 | /CAS | 9 | U1 | QH (7) |
| 8 | VA12 | 20 | U2 | QA (15) |
| 9 | VA13 | 21 | U2 | QB (1) |
| 10 | /GAME | 22 | U2 | QC (2) |
| 11 | /EXROM | 23 | U2 | QD (3) |
| 12 | R/W | 24 | U2 | QE (4) |
| 13 | /AEC | 25 | U2 | QF (5) |
| 14 | BA | 26 | U2 | QG (6) |
| 15 | A12 | 27 | U2 | QH (7) |

## Salidas de la PLA

Conecta directamente según esta tabla para el montaje básico, o intercala el HCT244 descrito arriba. El firmware usa `INPUT`, sin pull-ups internos. D0/D1 quedan libres para el puerto serie.

| Bit | Señal | Pin PLA | UNO |
| --- | --- | --- | --- |
| 0 | /CASRAM | 18 | D2 |
| 1 | /BASIC | 17 | D3 |
| 2 | /KERNAL | 16 | D4 |
| 3 | /CHAROM | 15 | D5 |
| 4 | /GRW | 13 | D6 |
| 5 | /IO | 12 | D7 |
| 6 | /ROML | 11 | D8 |
| 7 | /ROMH | 10 | D9 |

Los vectores representan niveles eléctricos: `/` indica activo bajo, pero el software no invierte automáticamente las entradas.

## Comprobación antes de conectar USB

1. Confirma la orientación y numeración de los tres integrados externos.
2. Comprueba que no haya cortocircuito entre 5 V y GND, y que todas las masas estén unidas.
3. Verifica el enlace U1 pin 9 → U2 pin 14; no lo confundas con QH, pin 7.
4. Comprueba que las 16 entradas tengan su resistencia a GND y que `/OE` de los 595 tenga su resistencia a 5 V.
5. Revisa las ocho salidas: deben llegar a D2–D9, configurados como entradas.
6. Conecta USB y verifica la tensión antes de enviar `t`.

## Referencias

- [Arduino UNO R3](https://docs.arduino.cc/hardware/uno-rev3/) y [hoja de datos ATmega328P](https://docs.arduino.cc/resources/datasheets/ATmega328P-datasheet.pdf).
- [Texas Instruments SN74HC595](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf): pines, cascada, latch y habilitación.
- [Signetics 82S100](https://s3.amazonaws.com/com.c64os.resources/weblog/versa64cart/82S100-datasheet.pdf): pinout y niveles eléctricos.
- [Ecuaciones y JEDEC del C64](https://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/c64-pla.txt).
- [SN74HCT244](https://www.ti.com/lit/ds/symlink/sn74hct244.pdf): buffer opcional para compatibilidad de umbrales.
