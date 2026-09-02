# Estados del Autómata
Se describen los **estados** utilizados por el **AF** del analizador léxico del compilador.  

---

## Tabla de Estados del Autómata

| Estado | Descripción |
|--------|-------------|
| **0**  | Estado inicial |
| **1**  | Posible ID o palabra reservada |
| **2**  | Digito |
| **3**  | Pasar de entero a real con `.` |
| **4**  | `{` |
| **5**  | `}` |
| **6**  | `(` |
| **7**  | `)` |
| **8** | `!` |
| **9** | `!=` |
| **10** | `>` |
| **11** | `>=` |
| **12** | `<` |
| **13** | `<=` |
| **14** | `&` |
| **15** | `&&` |
| **16** | `\|` |
| **17** | `\|\|`|
| **18** | `+` |
| **19** | `++` |
| **20** | `+=` |
| **21** | `-` |
| **22** | `--` |
| **23** | `-=` |
| **24** | `*` |
| **25** | `*=` |
| **26** | `=` |
| **27** | `==` |
| **28** | `;` |
| **29** | `,` |
| **30** | `.` |
| **31** | `/` |
| **32** | `/=` |
| **33** | Inicio comentario `/*` |
| **34** | Continuar o terminar comentario |

---